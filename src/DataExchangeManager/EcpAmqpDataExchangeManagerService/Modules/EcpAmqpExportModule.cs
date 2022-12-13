using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Linq;
using Amqp;
using log4net.Util;
using Newtonsoft.Json;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Settings;
using Volue.Secrets.Storage;

namespace Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Modules
{
    public class EcpAmqpExportModule : PushExportModule
    {
        internal const string Modulename = "EcpAmqp";  // ToDo: Mix up of definitions. ModuleName is both used as name of the plugin (Unity injection) but also used as part of message queue name. See PushExportModule.TryExecuteSingleTask()

        public override string ModuleName => Modulename;
        private EcpAmqpParameters _ecpAmqpParameters;
        private readonly FileSystemWatcher _fileWatcher = new FileSystemWatcher();

        public EcpAmqpExportModule(EcpAmqpModuleSettings settings,MessageExporter messageExporter,IServiceEventLogger serviceEventLogger)
            : base(settings, messageExporter, serviceEventLogger)
        {
            var paramFile = settings.ExportParametersFile;
            var paramPath = Environment.ExpandEnvironmentVariables(settings.ExportParametersFilePath);
            var paramFilter = settings.ExportParametersFileFilter;
            InitEcpAmqpParametersFromJsonFile(Path.Combine(paramPath, paramFile));
            SetUpFileWatcherForParametersFile(paramPath, paramFilter);
        }

        protected sealed override string SendExportMessage(DataExchangeExportMessage document)
        {
            string ret;
            var url = new SecretsManager(IccConfiguration.Data.OracleConnectionString).Decrypt(_ecpAmqpParameters.EcpAmqpUrl);
            var connection = new Connection(new Address(url));
            var session = new Session(connection);
            var outbox = new SenderLink(session, "outbox", _ecpAmqpParameters.EcpAmqpOutboxQueue);
            try
            {
                var message = createMessage(document);
                Log.DebugExt($"{ModuleName}: Sending message: CorrelationID={message.Properties.CorrelationId}, ApplicationProperties={message.ApplicationProperties}, MessageId: {message.Properties.MessageId}");
                outbox.Send(message, TimeSpan.FromMilliseconds(_ecpAmqpParameters.EcpAmqpSendTimeoutMs));

                var inbox = new ReceiverLink(session, "inbox", _ecpAmqpParameters.EcpAmqpOutboxResponseQueue);
                Message response;
                /* Looping is normally not necessary.
                 * Implemented because we once had a situation where there was several responses that was not accepted, so we had to spool until we emptied the queue/found the correct correlationId.
                 */
                do
                {
                    response = inbox.Receive(TimeSpan.FromMilliseconds(_ecpAmqpParameters.EcpAmqpPollTimeoutMs));
                    if (response != null)
                    {
                        inbox.Accept(response);
                        Log.DebugExt($"{ModuleName}: Response CorrelationId: {response.Properties.CorrelationId}");
                    }
                } while (response != null && message.Properties.CorrelationId != response.Properties.CorrelationId);
                string status = String.Empty;
                if (response != null)
                {
                    Log.Debug($"{ModuleName}: Response from sending {response.Properties.CorrelationId}: ApplicationProperties={response.ApplicationProperties}");
                    if (_ecpAmqpParameters.UseEdx)
                    {
                        Log.DebugExt($"{ModuleName}: Edx mode. {response.DecodeBodyAsString(null)}");
                        XDocument xmlDoc;
                        using (var stream = new StringReader(response.DecodeBodyAsString(null)))
                        {
                            xmlDoc = XDocument.Load(stream);
                        }
                        var root = xmlDoc.Root;
                        Exception exception = null;
                        bool failed = false;
                        foreach (var xElement in root.Elements())
                        {
                            if (xElement.Name.ToString().Equals("finalMessageStatus"))
                            {
                                foreach (var messageStatusElement in xElement.Elements())
                                {
                                    if (messageStatusElement.Name.ToString().Equals("status"))
                                    {
                                        status = messageStatusElement.Attribute("v").Value;
                                        if (status.Equals("FAILED"))
                                        {
                                            failed = true;  // Error
                                        }
                                    }
                                    else if (failed && messageStatusElement.Name.ToString().Equals("statusText"))
                                    {
                                        exception = new DataExchangeQueueOperationFailedException(messageStatusElement.Attribute("v").Value);
                                    }
                                }
                            }
                            else if (failed && xElement.Name.ToString().Equals("edxReplyMetadata"))
                            {
                                foreach (var metadataElement in xElement.Elements())
                                {
                                    exception.Data.Add(metadataElement.Name.ToString(),metadataElement.Attribute("v").Value);
                                }
                            }
                        }
                        if (exception != null)
                        {
                            throw exception;
                        }
                    }
                    else
                    {
                        // ECP
                        Log.Debug($"{ModuleName}: ECP mode. MessageId: {response?.Properties.MessageId}");
                        if (string.IsNullOrEmpty(response.GetMessageId()))  // From ApplicationProperties.
                        {
                            // Error
                            var exception = new DataExchangeQueueOperationFailedException(response.GetErrorMessage());
                            foreach (var errorProperty in response.ApplicationProperties.Map)
                            {
                                exception.Data.Add(errorProperty.Key, errorProperty.Value);
                            }

                            throw exception;
                        }
                        status = "RECEIVED";
                    }
                }
                Log.Info($"{ModuleName}: Status: {status}.");
                ret = response?.Properties.MessageId;
            }
            catch (DataExchangeQueueOperationFailedException Ex)
            {
                Log.Error(Ex);
                LogError($"{ModuleName}: {Ex.Message}");
                ret = string.Empty;
                if (_ecpAmqpParameters.UseEdx)
                {
                    throw new DataExchangeInvalidMessageException(document.MessageLogId.ToString(), "Send operation failed. See inner exception.", Ex); // Throw an exception that removes the message from the queue.
                }
                // Ecp reports error even though the sending goes well and we receive acknowledges.
            }
            catch (DataExchangeInvalidMetadataException Ex)
            {
                if (Ex.Data.Count > 0)
                {
                    var builder = new StringBuilder("Data:");
                    builder.AppendLine();
                    foreach (DictionaryEntry data in Ex.Data)
                    {
                        builder.AppendLine($"{data.Key}:{data.Value}");
                    }
                    Log.Error(builder.ToString());
                }
                throw;
            }
            catch (AmqpException Ex)
            {
                Log.Error(Ex);
                LogError(Ex.Message);
                throw new DataExchangeQueueOperationFailedException("Amqp operation failed.",Ex);
            }
            catch (Exception Ex)
            {
                Log.Error(Ex);
                LogError(Ex.Message);
                throw new DataExchangeInvalidMessageException(document.MessageLogId.ToString(), "EcpAmqp failure", Ex);
            }
            finally
            {
                outbox.Close();
                session.Close();
                connection.Close();
            }
            if (!_ecpAmqpParameters.Enable)
            {
                Log.Info($"{ModuleName}: Disabled.");
                RequestStop();
                return string.Empty;
            }
            return ret;
        }

        private Message createMessage(DataExchangeExportMessage document)
        {
            var businessType = GetBusinessType(document);
            var now = DateTime.Now;
            if (_ecpAmqpParameters.UseEdx)
            {
                return MessageBuilder.CreateMessage(businessType, document.GetMessageData())
                    .WithReceiverAddress(GetReceiverCode(document)).WithBusinessMessageId(document.MessageReference)
                    .WithCorrelationId(document.ExternalReference).WithSenderApplication(ModuleName);
            }
            return MessageBuilder.CreateMessage(businessType, document.GetMessageData())
                .WithReceiverAddress(GetReceiverCode(document)).WithBusinessMessageId(document.MessageReference)//.WithMessageId(document.MessageReference) MessageId is set in CreateMessage()
                .WithCorrelationId(document.ExternalReference).WithSenderApplication(ModuleName)
                .WithCreationTime(now).WithAbsoluteExpiryTime(now + _ecpAmqpParameters.MessageExpiryDuration);
        }

        private string GetBusinessType(DataExchangeExportMessage document)
        {
            /* 
             * Direction   DocumentType   ECP MessageType   Description
             * export      Response       TRE-active-ack    For domestic activations
             * export      Response       RD-active-ack     For international activations
             * export      ReserveBids    TRE-bids-new      Tech note
             */
            return document.SubAddress;
        }

        private static string GetReceiverCode(DataExchangeMessageBase message)
        {
            // Using routing address in stead of ReceiverId for receiver to make it equal to MADES/edx setup which has service in this parameter.
            var routing = message.RoutingAddress.Split(new char[] { ':' });   // Format: ECPAMQP:<EICaddress>
            if (routing.Length < 2)
                throw new MissingFieldException("Routing.EICaddress for receiver.");
            return routing[1];
        }

        private void InitEcpAmqpParametersFromJsonFile(string fullPathFilename)
        {
            using (var strReader = new StreamReader(fullPathFilename))
            {
                _ecpAmqpParameters = JsonConvert.DeserializeObject<EcpAmqpParameters>(strReader.ReadToEnd());
            }

            Log.DebugExt(() => $"{ModuleName}: New parameters loaded. UseEdx: {_ecpAmqpParameters.UseEdx}, Connection: {_ecpAmqpParameters.EcpAmqpUrl}, ReceiverLink.outbox: {_ecpAmqpParameters.EcpAmqpOutboxQueue}, ReceiverLink.responsebox: {_ecpAmqpParameters.EcpAmqpOutboxResponseQueue}, Sending: timeout: {_ecpAmqpParameters.EcpAmqpSendTimeoutMs}");
        }

        private void SetUpFileWatcherForParametersFile(string paramPath, string paramFilter)
        {
            _fileWatcher.NotifyFilter = NotifyFilters.CreationTime | NotifyFilters.LastWrite;
            _fileWatcher.Path = paramPath;
            _fileWatcher.Filter = paramFilter;
            _fileWatcher.Changed += OnParametersFileChanged;
            _fileWatcher.EnableRaisingEvents = true;
        }

        private void OnParametersFileChanged(object source, FileSystemEventArgs eArgs)
        {
            LogMessage.Log(Constants.MessageIdentifiers.CommonGeneralInformationMessage, false, eArgs.FullPath + " is changed.");
            InitEcpAmqpParametersFromJsonFile(eArgs.FullPath);
        }
    }
}
