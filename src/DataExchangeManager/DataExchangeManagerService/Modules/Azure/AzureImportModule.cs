using System;
using System.IO;
using System.Configuration;
using Microsoft.WindowsAzure.Storage.Blob;
using Microsoft.ServiceBus.Messaging;
using Newtonsoft.Json;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure
{
    public class AzureImportModule : IDataExchangeModule
    {
        public const string MODULE_NAME = "AZURE_IMPORT";
        private const int CommonGeneralInformationMessage = 3;
        protected readonly IServiceEventLogger ServiceEventLogger;
        private ServiceBusReceiver _sbReceiver;
        private readonly AzureLogic _logic;
        private AzureParameters _azureParameters = null;
        private FileSystemWatcher _fileWatcher = new FileSystemWatcher();

        private volatile bool _stopRequested;
        private volatile bool _isRunning;
        private volatile bool _isExecutingJobRightNow;

        public Exception FailureReason { get; private set; }
        public bool IsExecutingJobRightNow { get { return _isExecutingJobRightNow; } }
        public bool IsRunning { get { return _isRunning; } }

        public AzureImportModule(AzureLogic Logic, IServiceEventLogger serviceEventLogger)
        {
            ServiceEventLogger = serviceEventLogger;
            _logic = Logic;

            string paramFile = ConfigurationManager.AppSettings["AzureImportParametersFileFilter"] ?? "AzureImportParametersFile.json";
            string paramPath = ConfigurationManager.AppSettings["AzureImportParametersFilePath"] ?? ".";
            InitAzureParameterString(Path.Combine(paramPath, paramFile));
            _fileWatcher.NotifyFilter = NotifyFilters.CreationTime | NotifyFilters.LastWrite;
            _fileWatcher.Path = paramPath;
            _fileWatcher.Filter = paramFile;
            _fileWatcher.Changed += new FileSystemEventHandler(OnAzureParametersFileChanged);
        }

        public string ModuleName
        {
            get { return MODULE_NAME; }
        }

        private void OnMessageArrived(BrokeredMessage Message)
        {
            if (_stopRequested)
                return;

            _isExecutingJobRightNow = true;
            try
            {
                object obj;
                if (!Message.Properties.TryGetValue("Upload_PipBlob_BlobPath", out obj))
                {
                    ServiceEventLogger.LogToEventLog("Non existing Upload_PipBlob_BlobPath. Message discarded.",
                        System.Diagnostics.EventLogEntryType.Error);
                    Message.Complete();
                    return;
                }
                string filename = obj.ToString();
                // Use message.Properties to read metadata, if required
                // Handle
                var payload = DownloadFromStorage(filename);
                _logic.SubmitImport(Message, payload);
                // Delete message from queue to avoid processing again
                Message.Complete();
            }
            catch (Exception ex)
            {
                // i.e MissingFieldException("BrokeredMessage.Properties"...)
                ServiceEventLogger.LogToEventLog(ex.Message, System.Diagnostics.EventLogEntryType.Error);
                Message.DeadLetter("DataExchangeManagerService exception", ex.Message + ex.StackTrace);
            }
            finally
            {
                _isExecutingJobRightNow = false;
            }
        }

        void OnExceptionReceived(object Sender, ExceptionReceivedEventArgs E)
        {
            FailureReason = E.Exception;
            // ToDo: Check for Connection error related to ConnectionString: Toggle to pipInputQueueSecondaryConnectionString.
            ServiceEventLogger.LogCritical(E.Exception, false);
        }

        private string DownloadFromStorage(string filename)
        {
            string blobUriWithSas = _azureParameters.pipBlobStorageUrlAndSas;
            var container = new CloudBlobContainer(new Uri(blobUriWithSas));
            var blob = container.GetBlockBlobReference(filename);
            var payload = blob.DownloadText();
            return payload;
        }

        private void OnAzureParametersFileChanged(object Source, FileSystemEventArgs EArgs)
        {
            LogMessage.Log(CommonGeneralInformationMessage, false, EArgs.FullPath + " is changed.");
            InitAzureParameterString(EArgs.FullPath);
        }

        private void InitAzureParameterString(string Filename)
        {
            var strReader = new StreamReader(Filename);
            _azureParameters = JsonConvert.DeserializeObject<AzureParameters>(strReader.ReadToEnd());
        }

        public void Start()
        {
            var manager = new Volue.Secrets.Storage.SecretsManager(IccConfiguration.Data.OracleConnectionString);
            var sasConnectionString = manager.Decrypt(_azureParameters.pipInputQueuePrimaryConnectionString);
            _sbReceiver = new ServiceBusReceiver(sasConnectionString);
            string queueName = _azureParameters.pipInputQueueName;
            var opts = new OnMessageOptions()
            {
                AutoComplete = false,
                MaxConcurrentCalls = Convert.ToInt32(ConfigurationManager.AppSettings["MaxConcurrentImports"] ?? "5") //_settings.MaxConcurrentImports
            };
            if (ConfigurationManager.AppSettings["AutoRenewTimeout"] != null)
                opts.AutoRenewTimeout = TimeSpan.FromMilliseconds(Convert.ToDouble(ConfigurationManager.AppSettings["AutoRenewTimeout"]));  // Default 5 minutes
            opts.ExceptionReceived += OnExceptionReceived;
            _sbReceiver.OnMessage(queueName, OnMessageArrived, opts);
            _isRunning = true;
        }

        public void RequestStop()
        {
            _stopRequested = true;
        }

        public void Stop(TimeSpan timeout)
        {
            _stopRequested = true;
            _isRunning = false;
        }

        public void Abort()
        {
            throw new NotImplementedException();
        }
    }
}
