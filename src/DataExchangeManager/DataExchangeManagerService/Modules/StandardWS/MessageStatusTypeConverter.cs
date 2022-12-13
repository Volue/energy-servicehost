using System.Collections.Generic;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS
{
    public class MessageStatusTypeConverter
    {
        private static readonly Dictionary<MessageStatusTypeEnum, TransLogMessageStatus> _translations 
            = new Dictionary<MessageStatusTypeEnum, TransLogMessageStatus>
            {
                { MessageStatusTypeEnum.ExportTransferredProcessingStarted, TransLogMessageStatus.ExportTransferredProcessingStarted },
                { MessageStatusTypeEnum.ExportTransferredProcessingOk, TransLogMessageStatus.ExportTransferredProcessingOk },
                { MessageStatusTypeEnum.ExportTransferredError, TransLogMessageStatus.ExportTransferredError },
                { MessageStatusTypeEnum.ExportSentReceivedOk, TransLogMessageStatus.ExportSentReceivedOk },
                { MessageStatusTypeEnum.ExportSentReceivedError, TransLogMessageStatus.ExportSentReceivedError }
            };

        public static TransLogMessageStatus ConvertExternalToInternal(MessageStatusTypeEnum externalStatus)
        {
            TransLogMessageStatus internalStatus;
            if (_translations.TryGetValue(externalStatus, out internalStatus))
            {
                return internalStatus;
            }

            throw new DataExchangeInvalidMetadataException(
                string.Format("Unable to convert the status from '{0}', this status is not supported.", externalStatus));
        }
    }
}
