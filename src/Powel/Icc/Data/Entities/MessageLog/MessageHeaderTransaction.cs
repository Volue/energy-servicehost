namespace Powel.Icc.Data.Entities.MessageLog
{
    public class MessageHeaderTransaction
    {
        public string TransactionReference { get; set; }

        public short TransactionStatus { get; set; }

        public string ErrorText { get; set; }
    }
}
