using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using log4net;
using log4net.Util;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS
{
    public class StandardWsLogic
    {
        private const string MODULE_NAME = "STANDARDWS";

        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private readonly Func<IDataExchangeApi> _dataExchangeApiFactory;
        private readonly IDataExchangeMessageLog _dataExchangeMessageLog;
        private readonly IServiceEventLogger _serviceEventLogger;
        private readonly IExternalEventLogger _externalEventLogger;

        public StandardWsLogic(
            Func<IDataExchangeApi> dataExchangeApiFactory, 
            IDataExchangeMessageLog dataExchangeMessageLog, 
            IServiceEventLogger serviceEventLogger, 
            IExternalEventLogger externalEventLogger)
        {
            _dataExchangeApiFactory = dataExchangeApiFactory;
            _dataExchangeMessageLog = dataExchangeMessageLog;
            _serviceEventLogger = serviceEventLogger;
            _externalEventLogger = externalEventLogger;
        }

        public GetExportResponse GetExport(int maxWaitTimeInSeconds)
        {
            if (Log.IsDebugEnabled) { Log.Debug("maxWaitTimeInSeconds=" + maxWaitTimeInSeconds); }

            string errorText;
            if (!ValidateExportLongPollingTimeoutInSeconds(maxWaitTimeInSeconds, out errorText))
            {
                Log.DebugExt($"{MODULE_NAME}: Timeout validation error: {errorText}");
                return new GetExportResponse {RequestOk = false, RequestNotOkReasonText = errorText};
            }

            GetExportResponse response;
            TimeSpan timeout = TimeSpan.FromSeconds(maxWaitTimeInSeconds);
            try
            {
                var api = _dataExchangeApiFactory();

                string messageInternalReference = string.Empty;
                DataExchangeExportMessage exportMessage = null;
                bool validMessageFound = false;

                do
                {
                    try
                    {
                        // The timeout resets every loop iteration here, so the maxWaitTimeInSeconds won't be used properly when there are poisonous messages in the queue
                        exportMessage = api.PeekExportMessage(out messageInternalReference, timeout, MODULE_NAME);
                        if (exportMessage == null)
                        {
                            Log.DebugExt($"{MODULE_NAME}: Got no export from queue. Timeout: {timeout}");
                            return new GetExportResponse { RequestOk = true, RequestNotOkReasonText = null, ExportMessage = null };
                        }
                        exportMessage.GetMessageData(); // Check if the ClaimHandler has the message content. Throws ClaimHandlerException.
                        validMessageFound = true;
                    }
                    catch (DataExchangeInvalidMessageException exception)
                    {
                        // Discard invalid messages from the queue immediately
                        _serviceEventLogger.LogCritical(exception, false);
                        _serviceEventLogger.LogMessage(8209);
                        using (var transaction = api.GetTransaction(DataExchangeQueueTransactionType.Dequeue))
                        {
                            transaction.Begin();
                            api.RemoveExportMessage(exception.MessageId, transaction, MODULE_NAME);
                            transaction.Commit();
                        }
                    }
                    catch (ClaimHandlerException exception)
                    {
                        // Missing message content from the ClaimHandler.
                        // Discard invalid messages from the queue immediately
                        _serviceEventLogger.LogCritical(exception, false);
                        _serviceEventLogger.LogMessage(8209);
                        using (var transaction = api.GetTransaction(DataExchangeQueueTransactionType.Dequeue))
                        {
                            transaction.Begin();
                            api.RemoveExportMessage(messageInternalReference, transaction, MODULE_NAME);
                            transaction.Commit();
                        }
                    }
                } while (!validMessageFound);

                response = new GetExportResponse { RequestOk = true, RequestNotOkReasonText = null, ExportMessage = null };
                response.ExportMessage = GetExportObject(exportMessage);
            }
            catch (Exception e)
            {
                Log.Error($"{MODULE_NAME}: Failed to dequeue export messages.", e);
                response = new GetExportResponse
                {
                    RequestOk = false,
                    RequestNotOkReasonText = "Internal error ('" + e.Message + "')"
                };
            }

            return response;
        }

        public AcknowledgeExportResponse SendExportAck(string externalReference)
        {
            AcknowledgeExportResponse response = new AcknowledgeExportResponse();
            response.RequestOk = false;
            response.RequestNotOkReasonText = null;

            if (externalReference == null)
            {
                Log.DebugExt($"{MODULE_NAME}: externalReference is null");
                response.RequestNotOkReasonText = "Provided reference is null";
            }
            else
            {
                Log.DebugExt($"{MODULE_NAME}: externalReference={externalReference}");
                try
                {
                    var api = _dataExchangeApiFactory();

                    string internalId;
                    DataExchangeExportMessage message = api.PeekExportMessageByExternalReference(out internalId, externalReference, MODULE_NAME);
                    if (message != null && message.ExternalReference != null && message.ExternalReference == externalReference)
                    {
                        // match

                        using (var transaction = api.GetTransaction(DataExchangeQueueTransactionType.Dequeue))
                        {
                            transaction.Begin();

                            api.RemoveExportMessage(internalId, transaction, MODULE_NAME);

                            transaction.Commit();
                            message.DeleteMessageData();

                            response.RequestOk = true;

                            Log.Debug($"{MODULE_NAME}: MessageLogId={message.MessageLogId}");
                            _dataExchangeMessageLog.SetStatusToExportTransferredToHub(message.MessageLogId, message.ExternalReference);
                        }
                    }
                    else
                    {
                        response.RequestNotOkReasonText = "Reference not found";
                    }
                }
                catch (Exception e)
                {
                    Log.Error($"{MODULE_NAME}: Failed to dequeue export message: {externalReference}.", e);
                    response.RequestNotOkReasonText = "Internal error ('" + e.Message + "')";
                }
            }
            return response;
        }

        public SetMessageStatusResponse SetMessageStatus(string externalReference, MessageStatusTypeEnum status, string statusText)
        {
            try
            {
                var internalStatus = MessageStatusTypeConverter.ConvertExternalToInternal(status);
                var properStatusText = GetProperStatusText(statusText, internalStatus);

                if (IsError(internalStatus))
                {
                    _serviceEventLogger.LogMessage(8207, status.ToString(), statusText, externalReference);
                }

                if (properStatusText != null)
                {
                    _dataExchangeMessageLog.SetStatusAndStatusText(externalReference, internalStatus, properStatusText);
                }
                else
                {
                    _dataExchangeMessageLog.SetStatus(externalReference, internalStatus);
                }

                return CreateOkResponse();
            }
            catch (DataExchangeInvalidMetadataException exception)
            {
                Log.Error($"{MODULE_NAME}: Failed to set message status and status text.", exception);

                return CreateErrorResponse(exception.Message);
            }
            catch (Exception exception)
            {
                Log.Error($"{MODULE_NAME}: Failed to set message status and status text.", exception);

                return CreateErrorResponse("Internal Server Error");
            }
        }

        public SubmitImportResponse SubmitImport(Dictionary<string, string> metadata,
                                                                    string importData, string externalReference)
        {
            string errorText;
            if (!ValidateImportExternalReference(externalReference, out errorText))
            {
                return GetNakSubmitImportResponse(errorText);
            }

            Dictionary<string, string> caseInsensitiveMetadata = null;

            if (metadata != null)
            {
                caseInsensitiveMetadata = new Dictionary<string, string>(metadata, StringComparer.OrdinalIgnoreCase);
            }

            return SubmitImportInternal(caseInsensitiveMetadata, importData, externalReference);
        }

        private SubmitImportResponse GetNakSubmitImportResponse(string requestNotOkReasonText)
        {
            SubmitImportResponse response = new SubmitImportResponse();
            response.RequestOk = false;
            response.RequestNotOkReasonText = requestNotOkReasonText;
            return response;
        }

        private SubmitImportResponse SubmitImportInternal(Dictionary<string, string> caseInsensitiveMetadata, string importData, string externalReference)
        {
            SubmitImportResponse response;

            try
            {
                var importMessage = new DataExchangeImportMessage()
                {
                    Version = DataExchangeImportMessage.LatestVersion,
                    Priority = GetMetadataValue(caseInsensitiveMetadata, "priority", true),
                    Protocol = GetMetadataValue(caseInsensitiveMetadata, "protocol", true),
                    Country = GetMetadataValue(caseInsensitiveMetadata, "country", false),
                    RoutingAddress = MODULE_NAME,
                    ExternalReference = externalReference,
                    ExternalText = GetMetadataValue(caseInsensitiveMetadata, "externaltext", false)
                };
                importMessage.SetMessageData(importData,null);
                if (importMessage.Protocol == "UTILTS")
                {
                    var re = new Regex(@"PD00 UTILTS.*\nPD01 S08");
                    var match = re.Match(importData);
                    if (match.Success)
                        importMessage.Protocol = DataExchangeMessageBase.ProtocolUtiltsS08;
                }

                var api = _dataExchangeApiFactory();

                using (IDataExchangeQueueTransaction transaction = api.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
                {
                    transaction.Begin();
                    api.EnqueueImportMessage(importMessage, transaction);
                    transaction.Commit();
                }

                response = new SubmitImportResponse
                    {
                        RequestOk = true,
                        RequestNotOkReasonText = null,
                    };
            }
            catch (DataExchangeInvalidMetadataException ex)
            {
                response = GetNakSubmitImportResponse(ex.Message);
            }
            catch (Exception ex)
            {
                Log.Error($"{MODULE_NAME}: Failed to enqueue import message.", ex);
                response = GetNakSubmitImportResponse("Internal error ('" + ex.Message + "')");
            }

            return response;
        }

        public SubmitEventResponse SubmitEvent(EventLevelTypeEnum level, string eventText)
        {
            SubmitEventResponse response = new SubmitEventResponse();

            try
            {
                switch (level)
                {
                    case EventLevelTypeEnum.Info:
                        _externalEventLogger.Info(MODULE_NAME, eventText);
                        response.RequestOk = true;
                        break;
                    case EventLevelTypeEnum.Warning:
                        _externalEventLogger.Warning(MODULE_NAME, eventText);
                        response.RequestOk = true;
                        break;
                    case EventLevelTypeEnum.Error:
                        _externalEventLogger.Error(MODULE_NAME, eventText);
                        response.RequestOk = true;
                        break;
                    case EventLevelTypeEnum.Fatal:
                        _externalEventLogger.Fatal(MODULE_NAME, eventText);
                        response.RequestOk = true;
                        break;
                    default:
                        response.RequestOk = false;
                        response.RequestNotOkReasonText = string.Format("Invalid value of the level argument, supported values are {0}, {1}, {2} and {3}.", EventLevelTypeEnum.Info, EventLevelTypeEnum.Warning, EventLevelTypeEnum.Error, EventLevelTypeEnum.Fatal);
                        break;
                }
            }
            catch (Exception exception)
            {
                Log.Error($"{MODULE_NAME}:Failed to submit event.", exception);
                response.RequestOk = false;
                response.RequestNotOkReasonText = "Internal error ('" + exception.Message + "')";
            }

            return response;
        }

        private static ExportMessage GetExportObject(DataExchangeExportMessage exportMessage)
        {
            return new ExportMessage
            {
                Reference = exportMessage.ExternalReference,
                ExportData = exportMessage.GetMessageData(),
                Metadata = exportMessage.Metadata()
            };
        }

        private static string GetMetadataValue(Dictionary<string, string> caseInsensitiveMetadata, string keyName, bool mandatory)
        {
            if (caseInsensitiveMetadata == null)
            {
                throw new DataExchangeInvalidMetadataException("No metadata available.");
            }

            string value = null;
            if (caseInsensitiveMetadata.ContainsKey(keyName))
            {
                value = caseInsensitiveMetadata[keyName];

                if (mandatory && string.IsNullOrEmpty(value))
                {
                    throw new DataExchangeInvalidMetadataException("Field with name '" + keyName + "' was empty in the metadata, this field is required.");
                }
            }
            else
            {
                if (mandatory)
                {
                    throw new DataExchangeInvalidMetadataException("Field with name '" + keyName + "' is missing in the metadata, this field is required.");
                }
            }

            return value;
        }

        private static bool ValidateExportLongPollingTimeoutInSeconds(int maxWaitTimeInSeconds, out string errorText)
        {
            if (maxWaitTimeInSeconds < 0)
            {
                errorText = "maxWaitTimeInSeconds must be a non-negative value";
            }
            else
            {
                // Validate OK
                errorText = null;
            }
            return errorText == null;
        }

        private static bool ValidateImportExternalReference(string externalReference, out string errorText)
        {
            if (string.IsNullOrEmpty(externalReference))
            {
                errorText = "externalReference is null or empty";
            }
            else
            {
                // Validate OK
                errorText = null;
            }
            return errorText == null;
        }

        /// <summary>
        /// this method is just a workaround to make sure that transaction_details is going to have SOME value for OK and Errors
        /// </summary>
        private static string GetProperStatusText(string statusText, TransLogMessageStatus internalStatus)
        {
            if (statusText != null)
            {
                return statusText;
            }

            if (internalStatus == TransLogMessageStatus.ExportSentReceivedOk)
            {
                return "OK";
            }
            if (IsError(internalStatus))
            {
                return "Error";
            }

            return null;
        }

        private static SetMessageStatusResponse CreateOkResponse()
        {
            return new SetMessageStatusResponse
            {
                RequestOk = true
            };
        }

        private static SetMessageStatusResponse CreateErrorResponse(string message)
        {
            return new SetMessageStatusResponse
            {
                RequestOk = false,
                RequestNotOkReasonText = message
            };
        }

        private static bool IsError(TransLogMessageStatus internalStatus)
        {
            return internalStatus == TransLogMessageStatus.ExportTransferredError
                || internalStatus == TransLogMessageStatus.ExportSentReceivedError;
        }
    }
}
