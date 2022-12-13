namespace Powel.Icc.Services.DataContracts.EventMonitor
{
    public enum TransLogMessageStatus
    {
        StatusUndefined,
        ImportSuccessful = 1,
        ImportPartly = 2,
        ImportFailed = 3,
        ImportError = 4,
        ImportFailedReImportSuccessful = 5,
        ExportFailedDuringCreation = 11,
        ExportCreated = 12,
        ExportSentWaitingForAcknowledgement = 13,
        ExportSentWaitingForMoreAcknowledgement = 14,
        ExportSentSuccessful = 15,  // Positive APERAK/Acknowledgment
        ExportSentPartlySuccessful = 16,
        ExportSentFailed = 17,  // Negative APERAK/Acknowledgment.
        ExportEnqueued = 18, // set by DataExchange-system
        ExportSentToRemoteSystem = 19, // set by EDI-server ("hub")
        ExportTransferredProcessingStarted = 22, // set by DataExchange-system
        ExportTransferredProcessingOk = 23, // set by EDI-server ("hub")
        ExportTransferredError = 24,
        ExportSentReceivedOk = 25, // set by EDI-server ("hub"). Positive CONTRL
        ExportSentReceivedError = 26, // set by EDI-server ("hub"). Negative CONTRL
        ExportSentPartlyAcknowledged = 30, // This is a special status only found in the EventMonitorService, this is used when at least one transaction status is missing. This status does not exist in the database.
        Missing
    }
}
