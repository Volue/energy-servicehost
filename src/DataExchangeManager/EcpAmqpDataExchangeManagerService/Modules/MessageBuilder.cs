using System;
using System.Text;
using Amqp;
using Amqp.Framing;

namespace Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Modules
{
    public static class MessageBuilder
    {
        /// <summary>
        /// Create a text message with content encoded as UTF-8
        /// </summary>
        /// <param name="content">Content to send</param>
        /// <returns></returns>
        public static Message CreateMessage(string businessType, string content)
        {
            return CreateMessage(businessType, Encoding.UTF8.GetBytes(content));
        }

        /// <summary>
        /// Create a new message with a given body and content settings
        /// </summary>
        /// <param name="body"></param>
        /// <param name="businessType">Required for EDX reception</param>
        /// <returns></returns>
        public static Message CreateMessage(string businessType, byte[] body)
        {
            var msg = new Message(body)
            {
                BodySection = new Data { Binary = body },
                Properties = new Properties
                {
                    MessageId = Guid.NewGuid().ToString(),
                    ContentType = "application/octetstream",
                },
                ApplicationProperties = new ApplicationProperties
                {
                    Map =
                    {
                        { MessageReaderExtensions.ApplicationPropertyKeys.MessageType, businessType }, // swissgrid uses messageType (EDC), but calls it business type.
                        { MessageReaderExtensions.ApplicationPropertyKeys.BusinessType, businessType }  // In Statnett examples (EDX) and Unicorn documentation they use businessType.
                    } // We add both, so they can pick what they like, and we can use common code without switching.
                }
            };

            return msg;
        }

        /// <summary>
        /// Used to track conversations / sagas
        /// </summary>
        /// <param name="correlationId">Original message</param>
        /// <returns></returns>
        public static Message WithCorrelationId(this Message message, Guid correlationId)
        {
            return message.WithCorrelationId(correlationId.ToString());
        }

        /// <summary>
        /// Used to track conversations / sagas
        /// </summary>
        /// <param name="correlationId">Original message</param>
        /// <returns></returns>
        public static Message WithCorrelationId(this Message message, string correlationId)
        {
            message.EnsurePropertiesExist();
            message.Properties.CorrelationId = correlationId;
            message.EnsureApplicationPropertiesExist();
            message.ApplicationProperties.Map.Add(MessageReaderExtensions.ApplicationPropertyKeys.CorrelationId,correlationId);
            return message;
        }

        /// <summary>
        /// Point the message to a given receiver address
        /// </summary>
        /// <param name="address"></param>
        /// <returns></returns>
        public static Message WithReceiverAddress(this Message message, string address)
        {
            message.EnsureApplicationPropertiesExist();
            //message.ApplicationProperties.Map.Add(Constants.ApplicationPropertyKeys.Receiver, address);
            message.ApplicationProperties.Map.Add(MessageReaderExtensions.ApplicationPropertyKeys.ReceiverCode, address);
            return message;
        }

        /// <summary>
        /// Set address based on endpoint and service
        /// </summary>
        /// <param name="endpoint">ECP endpoint address, usually a GLN number</param>
        /// <param name="service">Service you want to reach on the endpoint, e.g. SERVICE-FOS</param>
        /// <returns>self with receiver and receiverCode set to endpoint@address</returns>
        public static Message WithEdxToolboxSpecificServiceAddress(this Message message, string endpoint, string service)
        {
            return message.WithReceiverAddress(string.Format("{0}@{1}", endpoint, service));
        }

        public static Message WithMessageId(this Message message, string messageId)
        {
            // Although documentation refers to it, Swissgrid implementation example https://powelas.sharepoint.com/:w:/r/sites/extranet/20314/_layouts/15/Doc.aspx?sourcedoc=%7BBA995563-86DB-4290-B031-5CB3CB7055C4%7D&file=ECP%20Anbindung%20%C3%BCber%20AMQP%201.0.docx&action=default&mobileredirect=true 
            // says their using baMessageId in stead.
            message.EnsureApplicationPropertiesExist();
            message.ApplicationProperties.Map.Add(MessageReaderExtensions.ApplicationPropertyKeys.MessageId, messageId);
            return message;
        }

        public static Message WithBusinessMessageId(this Message message, Guid businessMessageId)
        {
            return message.WithBusinessMessageId(businessMessageId.ToString());
        }

        public static Message WithBusinessMessageId(this Message message, string businessMessageId)
        {
            message.EnsureApplicationPropertiesExist();
            message.ApplicationProperties.Map.Add(MessageReaderExtensions.ApplicationPropertyKeys.BusinessMessageId, businessMessageId);
            return message;
        }

        public static Message WithSenderApplication(this Message message, string senderApplication)
        {
            message.EnsureApplicationPropertiesExist();
            message.ApplicationProperties.Map.Add(MessageReaderExtensions.ApplicationPropertyKeys.SenderApplication, senderApplication);
            return message;
        }

        public static Message WithCreationTime(this Message message, DateTime created)
        {
            message.EnsurePropertiesExist();
            message.Properties.CreationTime = created;
            return message;
        }

        public static Message WithAbsoluteExpiryTime(this Message message, DateTime expired)
        {
            message.EnsurePropertiesExist();
            message.Properties.AbsoluteExpiryTime = expired;
            return message;
        }

        /// <summary>
        /// Guard in case property setter  methods are used on Message objects created manually (not using EdxMessageFactory)
        /// </summary>
        /// <param name="message"></param>
        private static void EnsurePropertiesExist(this Message message)
        {
            if (message.Properties == null)
            {
                message.Properties = new Properties();
            }
        }

        /// <summary>
        /// Guard in case property setter  methods are used on Message objects created manually (not using EdxMessageFactory)
        /// </summary>
        /// <param name="message"></param>
        private static void EnsureApplicationPropertiesExist(this Message message)
        {
            if (message.ApplicationProperties == null)
            {
                message.ApplicationProperties = new ApplicationProperties();
            }
        }
    }
}
