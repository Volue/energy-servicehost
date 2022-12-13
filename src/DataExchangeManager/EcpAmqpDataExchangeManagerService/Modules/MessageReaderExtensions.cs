using System;
using System.Text;
using Amqp;

namespace Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Modules
{
    public static class MessageReaderExtensions
    {
        public static class ApplicationPropertyKeys
        {
            public const string Receiver = "receiver";
            public const string ReceiverCode = "receiverCode";
            public const string BusinessType = "businessType";
            public const string BusinessMessageId = "baMessageID";
            public const string CorrelationId = "correlationID";
            public const string SenderApplication = "senderApplication";
            public const string SenderCode = "senderCode";
            public const string MessageId = "messageID";
            public const string MessageType = "messageType";
            public const string ErrorId = "errorID";
            public const string ErrorCode = "errorCode";
            public const string ErrorMessage = "errorMessage";
            public const string ErrorDetails = "errorDetails";
        }

        public static string DecodeBodyAsString(this Message message, string defaultValue = null)
        {
            var body = message.Body as byte[];
            if (body != null)
            {
                // Ordinary message.
                var content = new char[body.Length];
                int charsUsed;
                int bytesUsed;
                bool completed;

                Encoding.UTF8.GetDecoder().Convert(body, 0, body.Length, content, 0, body.Length, true, out bytesUsed,
                    out charsUsed, out completed);
                if (completed)
                {
                    return new string(content);
                }

                if (defaultValue != null)
                {
                    return defaultValue;
                }
                throw new Exception("Conversion error: Wrong encoding"); // TODO: Custom exception
            }

            var str = message.Body as string;
            if (str != null)
            {
                // Status message.
                return str;
            }

            var bodyType = message.Body != null ? message.Body.GetType().Name : "<null>";
            var result = String.Format("Expected byte array, got {0}", bodyType);
            throw new Exception(result); // TODO: Custom exception
        }

        public static string GetReceiver(this Message message)
        {
            return GetProperty(message, ApplicationPropertyKeys.Receiver);
        }

        public static string GetReceiverCode(this Message message)
        {
            return GetProperty(message, ApplicationPropertyKeys.ReceiverCode);
        }

        public static string GetBusinessType(this Message message)
        {
            // This is what the documentation says shall tell what kind of message this is.
            return GetProperty(message, ApplicationPropertyKeys.BusinessType);
        }

        public static string GetMessageType(this Message message)
        {
            // Used by Swissgrid although their documentation says BusinessType
            return GetProperty(message, ApplicationPropertyKeys.MessageType);
        }

        public static string GetBusinessMessageId(this Message message)
        {
            return GetProperty(message, ApplicationPropertyKeys.BusinessMessageId);
        }

        public static string GetMessageId(this Message message)
        {
            return GetProperty(message, ApplicationPropertyKeys.MessageId);
        }

        public static string GetSenderApplication(this Message message)
        {
            return GetProperty(message, ApplicationPropertyKeys.SenderApplication);
        }

        #region ErrorProperties
        public static string GetErrorId(this Message message)
        {
            return GetProperty(message, ApplicationPropertyKeys.ErrorId);
        }

        public static string GetErrorCode(this Message message)
        {
            return GetProperty(message, ApplicationPropertyKeys.ErrorCode);
        }

        public static string GetErrorMessage(this Message message)
        {
            return GetProperty(message, ApplicationPropertyKeys.ErrorMessage);
        }

        public static string GetErrorDetails(this Message message)
        {
            return GetProperty(message, ApplicationPropertyKeys.ErrorDetails);
        }
        #endregion ErrorProperties

        public static string GetProperty(Message message, string propertyName)
        {
            if (!message.ApplicationProperties.Map.ContainsKey(propertyName))
            {
                return null; // throw new KeyNotFoundException($"{propertyName} was not found in ApplicationProperties.");
            }
            return message.ApplicationProperties[propertyName] as string;
        }
    }
}
