using System.Collections.Generic;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data.Entities.MessageLog
{
    public class MessageHeaderStatus
    {
        public MessageHeaderStatus()
        {
            TransferDate = UtcTime.MinValue;
            DateCreated = UtcTime.MinValue;
            DateAckReceived = UtcTime.MinValue;
            Transactions = new List<MessageHeaderTransaction>();
        }

        public long MessageId { get; set; }

        public string MessageReference { get; set; }

        public int Status { get; set; }

        public string StatusName { get; set; }

        public UtcTime TransferDate { get; set; }

        public UtcTime DateCreated { get; set; }

        public UtcTime DateAckReceived { get; set; }

        public List<MessageHeaderTransaction> Transactions { get; set; }

        public int Direction { get; set; }

        public int ProtocolId { get; set; }

        public int SenderCountryKey { get; set; }
        
        public int ReceiverCountryKey { get; set; }
    }
}
