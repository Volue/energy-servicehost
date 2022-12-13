using System.Collections.Generic;
using System.Runtime.Serialization;
using System.ServiceModel;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface
{

    [ServiceContract]
    public interface IStandardWs
    {
        /// <summary>
        /// Get an export. If no export is available, the service will wait for the specified time, until an
        /// export is available and then return it. If maxWaitTimeInSeconds is set to 0, the service will
        /// return straight away. However, it is recommended that a time is specified, for example 10 seconds.
        /// 
        /// NB: the client must call SendExportAck() to acknowledge for the exports received from ReadExports()
        /// </summary>
        /// <param name="maxWaitTimeInSeconds">Non-negative number. Max number of seconds before returning.</param>
        /// <returns></returns>
        [OperationContract]
        GetExportResponse GetExport(int maxWaitTimeInSeconds);

        /// <summary>
        /// Send positive acknowledgement for a previously obtained export
        /// </summary>
        /// <param name="reference">The reference returned in response from GetExport()</param>
        /// <returns></returns>
        [OperationContract]
        AcknowledgeExportResponse SendExportAck(string reference);

        /// <summary>
        ///  The optional statusText is written to the Message Log transaction details.
        /// 
        /// </summary>
        /// <param name="reference"></param>
        /// <param name="status"></param>
        /// <param name="statusText">Optional, not used if set to null. However, if status is ExportSentContrlOk and text is null, then the system will default to statusText "OK"</param>
        /// <returns></returns>
        [OperationContract]
        SetMessageStatusResponse SetMessageStatus(string reference, MessageStatusTypeEnum status, string statusText);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="metadata">Metadata as key-value pairs. 
        /// M/O  Key         Default value    Valid values
        /// M    priority    -                high, normal, low
        /// M    protocol
        /// 
        /// </param>
        /// <param name="importData"></param>
        /// <param name="reference"></param>
        /// <returns></returns>
        [OperationContract]
        SubmitImportResponse SubmitImport(Dictionary<string, string> metadata, string importData, string reference);
        
        [OperationContract]
        SubmitEventResponse SubmitEvent(EventLevelTypeEnum level, string eventText);

    }

    /// <summary>
    /// These states has corresponding types in Powel Message Log
    /// </summary>
    [DataContract(Name = "MessageStatusType")]
    public enum MessageStatusTypeEnum
    {
        [EnumMember]
        ExportTransferredProcessingStarted,
        [EnumMember]
        ExportTransferredProcessingOk,
        [EnumMember]
        ExportTransferredError,
        [EnumMember]
        ExportSentReceivedOk,
        [EnumMember]
        ExportSentReceivedError
    }

    /// <summary>
    /// 
    /// Error: problems with handling message, for example some server is down
    /// Fatal: actual message is faulty, further processing not possible
    /// </summary>
    [DataContract(Name = "EventLevelType")]
    public enum EventLevelTypeEnum
    {
        [EnumMember]
        Info,
        [EnumMember]
        Warning,
        [EnumMember]
        Error,
        [EnumMember]
        Fatal
    }

    [DataContract]
    public class ExportMessage
    {
        [DataMember(IsRequired = true)]
        public string ExportData { get; set; }

        [DataMember(IsRequired = true)]
        public string Reference { get; set; }

        [DataMember(IsRequired = true)]
        public Dictionary<string, string> Metadata { get; set; }
    }

    [DataContract]
    public class GetExportResponse
    {
        [DataMember(IsRequired = true)]
        public bool RequestOk { get; set; }

        [DataMember]
        public string RequestNotOkReasonText { get; set; }

        [DataMember(IsRequired = true)]
        public ExportMessage ExportMessage { get; set; }
    }

// following copy-paste could be replaced with SINGLE type! Think twice when designing public interfaces!

    [DataContract]
    public class AcknowledgeExportResponse
    {
        [DataMember(IsRequired = true)]
        public bool RequestOk { get; set; }

        [DataMember]
        public string RequestNotOkReasonText { get; set; }
    }

    [DataContract]
    public class SetMessageStatusResponse
    {
        [DataMember(IsRequired = true)]
        public bool RequestOk { get; set; }

        [DataMember]
        public string RequestNotOkReasonText { get; set; }
    }

    [DataContract]
    public class SubmitImportResponse
    {
        [DataMember(IsRequired = true)]
        public bool RequestOk { get; set; }

        [DataMember]
        public string RequestNotOkReasonText { get; set; }
    }

    [DataContract]
    public class SubmitEventResponse
    {
        [DataMember(IsRequired = true)]
        public bool RequestOk { get; set; }

        [DataMember]
        public string RequestNotOkReasonText { get; set; }
    }
}
