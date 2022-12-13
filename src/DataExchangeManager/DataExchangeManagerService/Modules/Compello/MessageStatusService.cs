using System;
using EDI.Server.API.Client.MessageServiceReference;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public static class MessageStatusService
    {
        public static StatusChangeEventArgs CreateArguments(TransactionStatus transactionStatus, long transactionId)
        {
            var convertedStatus = ConvertToMessageStatus(transactionStatus);

            return new StatusChangeEventArgs(convertedStatus, transactionId);
        }

        private static TransLogMessageStatus ConvertToMessageStatus(TransactionStatus transactionStatus)
        {
            switch (transactionStatus)
            {
                case TransactionStatus.Unprocessed:
                case TransactionStatus.Collected:
                case TransactionStatus.WaitingForExecution:
                case TransactionStatus.Executed:
                    return TransLogMessageStatus.ExportTransferredProcessingStarted;
                case TransactionStatus.Delivered:
                    return TransLogMessageStatus.ExportSentToRemoteSystem;
                case TransactionStatus.Ok:
                case TransactionStatus.OkManual:
                    return TransLogMessageStatus.ExportTransferredProcessingOk;
                case TransactionStatus.Waiting:
                case TransactionStatus.WaitingManual:
                    return TransLogMessageStatus.ExportSentWaitingForAcknowledgement;
                case TransactionStatus.Failed:
                case TransactionStatus.FailedManual:
                    return TransLogMessageStatus.ExportTransferredError;
                default:
                    throw new ArgumentOutOfRangeException("transactionStatus");
            }
        }
    }
}