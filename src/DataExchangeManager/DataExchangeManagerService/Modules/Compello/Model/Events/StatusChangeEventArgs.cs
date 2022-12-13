using System;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events
{
    public class StatusChangeEventArgs : EventArgs
    {
        public readonly TransLogMessageStatus Status;
        public readonly long TransactionId;

        public StatusChangeEventArgs(TransLogMessageStatus status, long transactionId)
        {
            Status = status;
            TransactionId = transactionId;
        }
    }
}