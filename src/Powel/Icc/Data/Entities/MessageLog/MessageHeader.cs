using System.Collections.Generic;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data.Entities.MessageLog
{
    public class MessageHeader
    {
        public MessageHeader()
        {
            SortDate = UtcTime.MinValue;
            DateCreated = UtcTime.MinValue;
            StartDate = UtcTime.MinValue;
            EndDate = UtcTime.MinValue;
            TransferDate = UtcTime.MinValue;
            DateAckReceived = UtcTime.MinValue;
            Transactions = new List<MessageHeaderTransaction>();
        }

        public string StatusName { get; set; }

        public UtcTime SortDate { get; set; }

        public long MessageId { get; set; }

        public int Direction { get; set; }

        public int ProtocolId { get; set; }

        public UtcTime DateCreated { get; set; }

        public string MessageReference { get; set; }

        public UtcTime StartDate { get; set; }

        public UtcTime EndDate { get; set; }

        public int SenderId { get; set; }

        public int ReceiverId { get; set; }

        public UtcTime TransferDate { get; set; }

        public int Status { get; set; }

        public UtcTime DateAckReceived { get; set; }

        public int Filesize { get; set; }

        public string SenderName { get; set; }

        public string ReceiverName { get; set; }

        public string ReceiverShortName { get; set; }

        public string SenderShortName { get; set; }

        public string ProtocolName { get; set; }

        public string ExchangeMethodName { get; set; }

        public List<MessageHeaderTransaction> Transactions { get; set; }

        public int SenderCountryKey { get; set; }

        public int ReceiverCountryKey { get; set; }
    }
}
