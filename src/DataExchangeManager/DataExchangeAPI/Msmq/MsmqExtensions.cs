using System;
using System.Collections.Generic;
using System.Linq;
using System.Messaging;
using System.Xml;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq.Exceptions;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq
{
    public static class MsmqExtensions
    {
        /// <summary>
        ///<exception cref="DataExchangeInvalidMessageException">size limit is 4MB</exception>
        /// </summary>
        public static void Send<T>(this MessageQueue messageQueue, T messageBody, MessageQueueTransaction transaction)
        {
            ThrowTooBigMessageExceptionWhenSizeLimitIsReached(
                () =>
                {
                    var message = GetMessage(messageBody);
                    messageQueue.Send(message, transaction);
                });
        }

        /// <summary>
        ///<exception cref="DataExchangeInvalidMessageException">size limit is 4MB</exception>
        /// </summary>
        public static void Send<T>(this MessageQueue messageQueue, T messageBody, MessageQueueTransactionType transactionType)
        {
            ThrowTooBigMessageExceptionWhenSizeLimitIsReached(
                () =>
                {
                    var message = GetMessage(messageBody);
                    messageQueue.Send(message, transactionType);
                });
        }

        public static void Remove(this MessageQueue messageQueue, string messageId, MessageQueueTransaction transaction)
        {
            messageQueue.ReceiveById(messageId, transaction);
        }

        public static void Remove(this MessageQueue messageQueue, string messageId, MessageQueueTransactionType transactionType)
        {
            messageQueue.ReceiveById(messageId, transactionType);
        }

        public static List<string> GetMessageIds(this MessageQueue messageQueue)
        {
            List<string> ids;

            using(var messageQueueWithMinimalPayload = new MessageQueue(messageQueue.Path))
            {
                messageQueueWithMinimalPayload.MessageReadPropertyFilter = new MessagePropertyFilter
                    {
                        AdministrationQueue = false,
                        ArrivedTime = false,
                        CorrelationId = false,
                        Priority = false,
                        ResponseQueue = false,
                        SentTime = false,
                        Body = false,
                        Label = false,
                        Id = true
                    };

                ids = messageQueue.GetAllMessages().Select(m => m.Id).ToList();

                messageQueueWithMinimalPayload.Close();
            }

            return ids;
        }
        
        public static T Receive<T>(this MessageQueue messageQueue, MessageQueueTransaction transaction)
        {
            Message message = messageQueue.Receive(new TimeSpan(0), transaction);
            T messageBody = messageQueue.GetMessageBody<T>(message);
            return messageBody;
        }

        public static T Receive<T>(this MessageQueue messageQueue, MessageQueueTransactionType transactionType)
        {
            Message message = messageQueue.Receive(new TimeSpan(0), transactionType);
            T messageBody = messageQueue.GetMessageBody<T>(message);
            return messageBody;
        }

        public static T Receive<T>(this MessageQueue messageQueue, string messageId, MessageQueueTransaction transaction)
        {
            T messageBody;

            try
            {
                Message message = messageQueue.ReceiveById(messageId, transaction);
                messageBody = messageQueue.GetMessageBody<T>(message);
            }
            catch (InvalidOperationException)
            {
                messageBody = default(T);
            }

            return messageBody;
        }

        public static T Receive<T>(this MessageQueue messageQueue, string messageId, MessageQueueTransactionType transactionType)
        {
            T messageBody;

            try
            {
                Message message = messageQueue.ReceiveById(messageId, transactionType);
                messageBody = messageQueue.GetMessageBody<T>(message);
            }
            catch (InvalidOperationException)
            {
                messageBody = default(T);
            }

            return messageBody;
        }

        public static T Peek<T>(this MessageQueue messageQueue, out string messageId)
        {
            Message message = messageQueue.Peek(new TimeSpan(0));
            T messageBody = messageQueue.GetMessageBodyAndId<T>(message, out messageId);
            return messageBody;
        }

        public static T PeekById<T>(this MessageQueue messageQueue, string messageId)
        {
            Message message = messageQueue.PeekById(messageId);
            T messageBody = messageQueue.GetMessageBody<T>(message);
            return messageBody;
        }
        
        public static T GetMessageBodyAndId<T>(this MessageQueue messageQueue, Message message, out string messageId)
        {
            T messageBody;
            messageId = message.Id;

            try
            {
                if (message.Body is T)
                {
                    messageBody = (T)message.Body;
                }
                else
                {
                    throw new InvalidMessageBodyException(messageId, string.Format("The message on the message queue {0} is invalid, maybe the correct formatter hasn't been set on the message queue.", messageQueue.Path));
                }
            }
            catch(InvalidOperationException exception)
            {
                throw new InvalidMessageBodyException(messageId, string.Format("The message on the message queue {0} is invalid, maybe the correct formatter hasn't been set on the message queue.", messageQueue.Path), exception);
            }
            catch (XmlException exception)
            {
                throw new InvalidMessageBodyException(messageId, string.Format("The message on the message queue {0} is invalid, maybe the correct formatter hasn't been set on the message queue.", messageQueue.Path), exception);
            }

            return messageBody;
        }
        
        public static T GetMessageBody<T>(this MessageQueue messageQueue, Message message)
        {
            string messageId;
            return messageQueue.GetMessageBodyAndId<T>(message, out messageId);
        }

        private static Message GetMessage<T>(T messageBody)
        {
            return new Message
                {
                    Formatter = new XmlMessageFormatter(new[] {typeof (T)}),
                    Body = messageBody
                };
        }

        private static void ThrowTooBigMessageExceptionWhenSizeLimitIsReached(Action command)
        {
            try
            {
                command.Invoke();
            }
            catch (MessageQueueException ex)
            {
                if (ex.MessageQueueErrorCode == MessageQueueErrorCode.InsufficientResources)
                {
                    throw new DataExchangeInvalidMessageException(string.Empty, "Message not accepted due to its size, 4MB is maximum", ex);
                }

                throw;
            }
        }
    }
}
