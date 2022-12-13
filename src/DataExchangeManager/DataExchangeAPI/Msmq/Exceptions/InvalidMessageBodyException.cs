using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq.Exceptions
{
    public class InvalidMessageBodyException : Exception
    {
        private readonly string _messageId;

        public InvalidMessageBodyException(string internalMessageId)
        {
            _messageId = internalMessageId;
        }

        public InvalidMessageBodyException(string internalMessageId, string message) : base(message)
        {
            _messageId = internalMessageId;
        }

        public InvalidMessageBodyException(string internalMessageId, string message, Exception innerException) : base(message, innerException)
        {
            _messageId = internalMessageId;
        }

        public string MessageId
        {
            get { return _messageId; }
        }
    }
}