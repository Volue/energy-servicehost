using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public interface IDataExchangeMessageLog
    {
        void SetStatusToExportEnqueued(long messageLogId);
        void SetExternalReference(long messageLogId, string externalReference);
        void SetStatusToExportTransferredToHub(long messageLogId, string externalReference, string routingAddress);
        void SetStatusToExportTransferredToHub(long messageLogId, string externalReference);
        void SetStatusAndStatusText(string externalReference, TransLogMessageStatus status, string statusText);
        int GetStatus(long messageLogId);
        bool SetStatus(string externalReference, TransLogMessageStatus status);
        void SetStatus(long messageLogId, TransLogMessageStatus status);
        string GetMessageReceiver(long messageLogId);
    }
}
    