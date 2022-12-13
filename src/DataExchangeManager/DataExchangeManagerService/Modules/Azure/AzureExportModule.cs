using System;
using System.Configuration;
using System.Linq;
//using Microsoft.Practices.ObjectBuilder2;
using Microsoft.WindowsAzure.Storage.Blob;
using Microsoft.ServiceBus.Messaging;
using Powel.Extensions;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure.PropertyRuleSet;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Newtonsoft.Json;
using System.IO;
using System.Text.RegularExpressions;
using log4net.Util;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure
{
    public class AzureExportModule : PushExportModule
    {
        public const string MODULE_NAME = "AZURE";
        private const int CommonGeneralInformationMessage = 3;

        private  ServiceBusSender _sbSender = null;
        private readonly IDataExchangeQueueFactory _dataExchangeQueueFactory;
        private AzureParameters _azureParameters = null;
        private FileSystemWatcher _fileWatcher = new FileSystemWatcher();
        private PropertyRuleSetMapper _ruleMapper;

        public AzureExportModule(
            IDataExchangeQueueFactory dataExchangeQueueFactory,
            IDataExchangeManagerServiceSettingsFactory dataExchangeManagerServiceSettingsFactory,
            MessageExporter messageExporter,
            IServiceEventLogger serviceEventLogger) 
            : base(dataExchangeManagerServiceSettingsFactory, messageExporter, serviceEventLogger)
        {
            _dataExchangeQueueFactory = dataExchangeQueueFactory;

            string paramFile = ConfigurationManager.AppSettings["AzureExportParametersFile"] ?? "AzureExportParametersFile.json";
            string paramPath = ConfigurationManager.AppSettings["AzureExportParametersFilePath"] ?? IccConfiguration.IccHome + @"\\bin\\AzureConfigs";
            InitAzureParameterString(Path.Combine(paramPath,paramFile));
            _fileWatcher.NotifyFilter = NotifyFilters.LastAccess | NotifyFilters.LastWrite;
            _fileWatcher.Path = paramPath;
            _fileWatcher.Filter = ConfigurationManager.AppSettings["AzureExportParametersFileFilter"] ?? "AzureExport*File.*";
            _fileWatcher.Changed += OnAzureParametersFileChanged;
            _fileWatcher.EnableRaisingEvents = true;

            string ruleFile = ConfigurationManager.AppSettings["AzureExportPropertyRuleFile"] ?? "AzureExportPropertyRuleFile.xml";

            _ruleMapper = new PropertyRuleSetMapper(Path.Combine(paramPath,ruleFile), serviceEventLogger);
            _ruleMapper.Map();
        }

        public override string ModuleName
        {
            get { return MODULE_NAME; }
        }

        protected sealed override string SendExportMessage(DataExchangeExportMessage exportMessage)
        {
            /* Routing address: AZURE:
             * Get the BlobUri from the routing address
             * Init blob storage:
             */
            var blobUriWithSas = _azureParameters.pipBlobStorageUrlAndSas;
            var container = new CloudBlobContainer(new Uri(blobUriWithSas));
            var blobName = Guid.NewGuid().ToString();
            var blob = container.GetBlockBlobReference(blobName);

            // Upload message to BlobStorage.
            blob.UploadText(exportMessage.GetMessageData());

            var message = GenerateTriggerMessage(blobName, exportMessage);
            /* 
             * Init service bus queue:
             */
            string queueName = _azureParameters.pipOutputTopicName;

            string sasConnectionString = _azureParameters.pipOutputTopicPrimaryConnectionString;
            if (_sbSender == null) _sbSender = new ServiceBusSender(sasConnectionString);
            try {
                // Sending trigger message to service bus
                _sbSender.Send(message,queueName);
            }
            catch (Exception E1)
            {
                Log.Error($"{ModuleName}: {E1}");
                LogMessage.Log(CommonGeneralInformationMessage, false, E1.Message);
                // Current connection string failed, toggle.
                sasConnectionString = _sbSender.ConnectionString == _azureParameters.pipOutputTopicPrimaryConnectionString ? _azureParameters.pipOutputTopicSecondaryConnectionString : _azureParameters.pipOutputTopicPrimaryConnectionString;
                Log.DebugExt($"Switching connection to: {sasConnectionString}");
                _sbSender = new ServiceBusSender(sasConnectionString);
                try
                {
                    _sbSender.Send(message, queueName);
                }
                catch (Exception E2)
                {
                    Log.Error($"{ModuleName}: {E2}");
                    LogError(E2.Message);
                    blob.Delete();
                    Log.Debug($"Blob {blobName} deleted.");
                    return string.Empty;
                }
            }
            Log.Info($"Message {message.CorrelationId} sent as {message.MessageId} to {queueName} referencing blob {blobName}");
            return blobName;
        }

        private void OnAzureParametersFileChanged(object Source, FileSystemEventArgs EArgs)
        {
            LogMessage.Log(CommonGeneralInformationMessage, false, EArgs.FullPath + " is changed.");
            while (IsExecutingJobRightNow) ;    // Wait for no job execution. ToDo: Should have a way of pausing the execution also.
            if (EArgs.FullPath.Contains(".xml"))
            {
                _ruleMapper = new PropertyRuleSetMapper(EArgs.FullPath,ServiceEventLogger);
                _ruleMapper.Map();
            }
            else
                InitAzureParameterString(EArgs.FullPath);
        }

        private void InitAzureParameterString(string Filename)
        { 
            var strReader = new StreamReader(Filename);
            _azureParameters = JsonConvert.DeserializeObject<AzureParameters>(strReader.ReadToEnd());
        }

        private BrokeredMessage GenerateTriggerMessage(string DataUrl, DataExchangeExportMessage exportMessage)
        {
            var message = new BrokeredMessage();
            message.Properties["Download_PipBlob_BlobPath"] = DataUrl;
            message.CorrelationId = exportMessage.ExternalReference;
            message.Properties["Priority"] = Regex.Replace(exportMessage.Priority.ToLower(),@"^\w",c => c.Value.ToUpper()); // Capitalize

            var rule = _ruleMapper.RuleSets.Rules.FirstOrDefault(r => Test(exportMessage, r.Select));
            if (rule == null)
            {
                Log.Error(string.Format(Properties.Resources.No_rule_found_for__0_, exportMessage.MessageReference));
                throw new MissingFieldException(string.Format(Properties.Resources.No_rule_found_for__0_, exportMessage.MessageReference));
            }
            Log.Info(string.Format("Using Rule {0} For {1}, messageId: {2}",rule.Name,exportMessage.MessageReference,message.MessageId));
            exportMessage.Format = rule.Name;   // For info in evntlog message 8101.
            rule.Properties.ForEach(p => message.Properties[p.Key] = _ruleMapper.MapValue(p.Value.ToString()));

            return message;
        }

        private bool Test(DataExchangeExportMessage exportMessage, SelectEntity select)
        {
            bool ret = true;
            if (select.MetaData != null)
            {
                foreach (var property in select.MetaData)
                {
                    var pvalue = _ruleMapper.MapValue(property.Value);
                    Regex re;
                    switch (property.Key)
                    {
                        case "ProtocolId":
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.ProtocolId.ToString()))
                                return false;
                            break;
                        case "SenderKey":
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.SenderKey.ToString()))
                                return false;
                            break;
                        case "ReceiverKey":
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.ReceiverKey.ToString()))
                                return false;
                            break;
                        case "SenderName":
                            if (exportMessage.SenderName == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.SenderName))
                                return false;
                            break;
                        case "ReceiverName":
                            if (exportMessage.ReceiverName == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.ReceiverName))
                                return false;
                            break;
                        case "Format":
                            if (exportMessage.Format == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.Format))
                                return false;
                            break;
                        case "Country":
                            if (exportMessage.Country == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.Country))
                                return false;
                            break;
                        case "Priority":
                            if (exportMessage.Priority == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.Priority))
                                return false;
                            break;
                        case "SubAddress":
                            if (exportMessage.SubAddress == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.SubAddress))
                                return false;
                            break;
                        case "ProductCode":
                            if (exportMessage.ProductCode == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.ProductCode))
                                return false;
                            break;
                        case "Protocol":
                            if (exportMessage.Protocol == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.Protocol))
                                return false;
                            break;
                        case "RoutingAddress":
                            if (exportMessage.RoutingAddress == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.RoutingAddress))
                                return false;
                            break;
                        case "MessageLogId":
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.MessageLogId.ToString()))
                                return false;
                            break;
                        case "MessageReference":
                            if (exportMessage.MessageReference == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.MessageReference))
                                return false;
                            break;
                        case "SenderId":
                            if (exportMessage.SenderId == null)
                                return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.SenderId))
                                return false;
                            break;
                        case "ReceiverId":
                            if (exportMessage.ReceiverId == null)
                                return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.ReceiverId))
                                return false;
                            break;
                        case "ExternalReference":
                            if (exportMessage.ExternalReference == null) return false;
                            re = new Regex(pvalue);
                            if (!re.IsMatch(exportMessage.ExternalReference))
                                return false;
                            break;
                        default:
                            string v;
                            if (exportMessage.Metadata().TryGetValue(property.Key, out v))
                            {
                                re = new Regex(pvalue);
                                if (!re.IsMatch(v))
                                    return false;
                            }
                            else return false;
                            break;
                    }
                }
            }

            if (!string.IsNullOrEmpty(select.Payload))
            {
                var value = exportMessage.GetMessageData();
                if (value == null) return false;
                var payload = _ruleMapper.MapValue(select.Payload);
                var re = new Regex(payload);
                ret = re.IsMatch(value);
            }

            return ret;
        }
    }
}
