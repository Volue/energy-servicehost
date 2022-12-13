using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;

namespace DataExchangeTestClient
{
    public class MessageStatusComboboxItem
    {
        public MessageStatusComboboxItem(MessageStatusTypeEnum type)
        {
            _status = type;
        }

        private readonly MessageStatusTypeEnum _status;

        public MessageStatusTypeEnum Status { get { return _status; } }

        public override string ToString()
        {
            string statusString;
            switch (_status)
            {
                case MessageStatusTypeEnum.ExportTransferredProcessingStarted:
                    statusString = "Export transferred, processing started";
                    break;
                case MessageStatusTypeEnum.ExportTransferredProcessingOk:
                    statusString = "Export transferred, processing ok";
                    break;
                case MessageStatusTypeEnum.ExportTransferredError:
                    statusString = "Export transferred, error";
                    break;
                case MessageStatusTypeEnum.ExportSentReceivedOk:
                    statusString = "Export sent, received ok";
                    break;
                case MessageStatusTypeEnum.ExportSentReceivedError:
                    statusString = "Export sent, received error";
                    break;
                default:
                    statusString = "<invalid status type>";
                    break;
            }

            return statusString;
        }
    }
}
