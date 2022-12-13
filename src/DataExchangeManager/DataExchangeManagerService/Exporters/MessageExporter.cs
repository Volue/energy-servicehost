using System;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters
{
    public class MessageExporter
    {
        private const int GeneralErrorMsg = 1010;
        private const int AnErrorOccurred = 8000;
        private const int CorruptOrInvalidMessage = 8209;

        private readonly IDataExchangeApi _dataExchangeApi;
        private readonly IServiceEventLogger _serviceEventLogger;
        private readonly IDataExchangeMessageLog _dataExchangeMessageLog;
        private readonly TimeSpan _howOftenWeShouldLogFailureForTheSameMessage;

        private long _messageLogIdOfFailingMessage;
        private DateTime _lastEventReportTime;

        public MessageExporter(
            IDataExchangeApi dataExchangeApi, 
            IServiceEventLogger serviceEventLogger, 
            IDataExchangeMessageLog dataExchangeMessageLog,
            MessageExporterSettings settings)
        {
            _dataExchangeApi = dataExchangeApi;
            _serviceEventLogger = serviceEventLogger;
            _dataExchangeMessageLog = dataExchangeMessageLog;
            _howOftenWeShouldLogFailureForTheSameMessage = settings.HowOftenWeShouldLogFailureForTheSameMessage;
        }

        /// <summary>
        /// From Birger:
        /// -	If the receiver returns InvalidMessage, then the receiver will never receive the message. 
        ///         The message is discarded and a message is written to the Event log. 
        ///         Because this is an export it is possible for our system to export the message once more when the error is fixed.
        ///     Any other error is a symptom of that the receiver has problems. 
        ///         A message is written to the event log to let the user fix the receiver, the message is NOT removed 
        ///         and the system should try to export it after some seconds. If the problem cannot be resolved, the message can be removed manually.
        /// </summary>
        public bool ExportMessage(string moduleName, TimeSpan dequeueTimeout, Func<DataExchangeExportMessage, string> sendExportMessageDelegate)
        {
            using (var transaction = _dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.Dequeue))
            {
                bool IsNewMessageFound = true;
                transaction.Begin();

                var message = _dataExchangeApi.DequeueExportMessage(dequeueTimeout, transaction, moduleName);
                if (message == null)
                {
                    IsNewMessageFound = false;
                    return IsNewMessageFound;
                }

                try
                {
                    var externalReference = sendExportMessageDelegate(message);

                    OnSuccess(externalReference, message);

                    transaction.Commit();
                    message.DeleteMessageData();
                }
                catch (DataExchangeInvalidMessageException exception)
                {
                    // Invalid messages are discarded immediately, so perform a commit so it is removed from the queue
                    transaction.Commit();
                    OnInvalidMessageException(exception);

                    // do NOT rethrow on PURPOSE: this service can NOT fail!
                }
                catch (ClaimHandlerException exception)
                {
                    // Messages where we have lost the payload are discarded immediately, so perform a commit so it is removed from the queue
                    transaction.Commit();
                    OnInvalidMessageException(new DataExchangeInvalidMessageException(message.MessageLogId.ToString(),exception.Message,exception));    // evt. OnUnknownError("",exception);

                    // do NOT rethrow on PURPOSE: this service can NOT fail!
                }
                catch (MissingFieldException exception)
                {
                    /* From Azure exporter: Handling No_rule_found_for__0_
                     * We move this message to Low priority queue so the message don't block any important messages until the system administrator can adjust the rules to match the message.
                     * If the message already has Low priority then I assume it is put at the back of the queue so all other Low prioritized messages are handled first.
                     * It is advised to set alarm on 8104 to monitor this type of error which only should happen when introducing new messages.
                     */
                    OnUnknownError(message, exception);
                    try
                    {
                        EnqueueToLowPriority(message,transaction);
                        transaction.Commit();
                    }
                    catch (Exception ex)
                    {
                        transaction.Rollback(); // Most probably the two-phase commit failed.
                        OnUnknownError(message, ex);
                    }
                }
                catch (Exception exception)
                {
                    OnUnknownError(message, exception);
                    // Move the message to Low priority queue.
                    try
                    {
                        EnqueueToLowPriority(message,transaction);
                        transaction.Commit();
                    }
                    catch (Exception ex)
                    {
                        _serviceEventLogger.LogCritical(ex,false);
                        transaction.Rollback(); // Most probably the two-phase commit failed.
                        OnUnknownError(message, ex);
                    }
                    // do NOT rethrow ON PURPOSE: this service can NOT fail!
                }
                return IsNewMessageFound;
            }
        }

        private void OnSuccess(string externalReference, DataExchangeExportMessage message)
        {
            _messageLogIdOfFailingMessage = 0;
            _serviceEventLogger.LogMessage(8101, externalReference, string.IsNullOrEmpty(message.SenderName) ? message.SenderId : message.SenderName, string.IsNullOrEmpty(message.ReceiverName) ? message.ReceiverId : message.ReceiverName, string.IsNullOrEmpty(message.Format) ? message.Protocol : message.Format);

            if (message.MessageLogId > 0)
            {
                _dataExchangeMessageLog.SetStatusToExportTransferredToHub(
                    message.MessageLogId,
                    externalReference,
                    message.RoutingAddress);
            }
        }

        private void OnInvalidMessageException(DataExchangeInvalidMessageException exception)
        {
            try
            {
                _serviceEventLogger.LogCritical(exception, false);
                _serviceEventLogger.LogMessage(CorruptOrInvalidMessage);
                _serviceEventLogger.LogMessage(AnErrorOccurred,exception.Message);
            }
            finally
            {
                long msgId;
                if (long.TryParse(exception.MessageId, out msgId))
                {
                    UpdateMessageLogStatusForDiscardedMessage(msgId);
                }
                else
                {
                    _serviceEventLogger.LogMessage(GeneralErrorMsg,string.Format("{0}: {1} is not an integer. Message log not updated.", GetType(), exception.MessageId));
                }
            }
        }

        private void UpdateMessageLogStatusForDiscardedMessage(long messageLogId)
        {
            _dataExchangeMessageLog.SetStatus(messageLogId, TransLogMessageStatus.ExportFailedDuringCreation);
        }

        private void OnUnknownError(DataExchangeExportMessage message, Exception exception)
        {
            if (IsTheSameMessageFailingAgain(message))
            {
                DoNotWriteToEventLogForEveryFailure(message, exception);
            }
            else
            {
                MessageFailedForTheFirstTime(message, exception);
            }
        }

        private void EnqueueToLowPriority(DataExchangeExportMessage message, IDataExchangeQueueTransaction tx)
        {
            // ToDo: We should have a Retries-counter in the ExportMessage which we can test against a limit on retries and discard the message.
            //using (var tx = _dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
            //{
            //    tx.Begin();
                message.Priority = "LOW";
                _dataExchangeApi.EnqueueExportMessage(message, tx);
            //    tx.Commit();
            //}
        }

        private bool IsTheSameMessageFailingAgain(DataExchangeExportMessage message)
        {
            return message.MessageLogId == _messageLogIdOfFailingMessage;
        }

        private void DoNotWriteToEventLogForEveryFailure(DataExchangeExportMessage message, Exception exception)
        {
            if (DateTime.Now.Subtract(_lastEventReportTime) >= _howOftenWeShouldLogFailureForTheSameMessage)
            {
                _serviceEventLogger.LogMessage(8104, message.ReceiverName, message.RoutingAddress, exception.Message);
                _lastEventReportTime = DateTime.Now;
            }
        }

        private void MessageFailedForTheFirstTime(DataExchangeExportMessage message, Exception exception)
        {
            _serviceEventLogger.LogMessage(8104, message.ReceiverName, message.RoutingAddress, exception.Message);
            _serviceEventLogger.LogCritical(exception,false);
            _lastEventReportTime = DateTime.Now;

            _messageLogIdOfFailingMessage = message.MessageLogId;
        }
    }
}
