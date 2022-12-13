using System;

namespace Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Modules
{
    public class EcpAmqpParameters
    {
        // Overall
        public bool UseEdx { get; set; } = true;
        // Import, Export and Status
        public string EcpAmqpUrl { get; set; } = "amqp://ecp:[$SECRET:EcpAmqpUrlPassword:$]@localhost:5672";    // amqp://localhost:17001, 172.21.54.40:5672
        public bool Enable { get; set; } = true;    // If false, disables the module.
        // Import and Status
        public int EcpAmqpPollTimeoutMs { get; set; } = 1000;
        public int EcpAmqpPollIntervalMs { get; set; } = 5000;
        // Import
        public string EcpAmqpInboxQueue { get; set; } = "ecp.endpoint.inbox";
        public string EcpAmqpImportDocQueryPriorityList { get; set; } = "<DocumentType v=\"Activation\",High";
        public string EcpAmqpImportDocQueryProtocolList { get; set; } = "<ActivationDocument xmlns=\"urn:entsoe.eu:wgedi:errp,ENTSOE";   // "<DocumentType v=\"(A40|Z15|A39)\"";
        public bool UseImportApplicationManagerService { get; set; } = false;
        // Export
        public string EcpAmqpOutboxQueue { get; set; } = "ecp.endpoint.outbox";
        public string EcpAmqpOutboxResponseQueue { get; set; } = "ecp.endpoint.outbox.reply";
        public int EcpAmqpSendTimeoutMs { get; set; } = 1000;
        public TimeSpan MessageExpiryDuration { get; set; } = new TimeSpan(1,0,0);  // 1 hour
        // Status
        public string EcpAmqpStatusQueue { get; set; } = "ecp.endpoint.send.event";

        /* States (MADES status):
         * ACCEPTED: Accepted by local ECP. ExportTransferredProcessingStarted (22)
         * TRANSPORTED: Message uploaded to recipient's node. (Time measurement mail from Ove Morten Stalheim (20150319)). ExportSentToRemoteSystem (19)
         * DELIVERED: Delivered to remote ECP. ExportSentReceivedOk (25)
         * RECEIVED: Received by remote business application. ExportSentReceivedOk (25)
         * FAILED: Delivery failed. ExportTransferredError (24)
         * DELIVERED and RECCEIVED are mapped to the same MessageLog-status. We do not have any status for RECEIVED.
         */
        public string EcpAmqpStatusConversionList { get; set; } = "ACCEPTED,22;TRANSPORTED,19;DELIVERED,25;RECEIVED,25;FAILED,24";
    }
}
