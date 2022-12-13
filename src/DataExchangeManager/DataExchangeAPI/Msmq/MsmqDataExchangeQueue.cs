using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.Linq;
using System.Messaging;
using log4net;
using log4net.Util;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq.Exceptions;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq
{
    public class MsmqDataExchangeQueue<T> : IDataExchangeQueue<T> where T : DataExchangeMessageBase
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private readonly List<DataExchangeQueuePriority> _orderedPriorities;
        private readonly Dictionary<DataExchangeQueuePriority, MsmqPath> _msmqPaths;
        private readonly IServiceEventLogger _serviceEventLogger;
        private static readonly bool _logNoMessageFound = Convert.ToBoolean(ConfigurationManager.AppSettings["LogNoMessageFound"] ?? "false");

        public MsmqDataExchangeQueue(List<DataExchangeQueuePriority> orderedPriorities, Dictionary<DataExchangeQueuePriority, MsmqPath> msmqPaths, IServiceEventLogger serviceEventLogger)
        {
            _orderedPriorities = orderedPriorities;
            _msmqPaths = msmqPaths;
            _serviceEventLogger = serviceEventLogger;

            Debug.Assert(_orderedPriorities.All(x => _msmqPaths.ContainsKey(x)), "Not all priorities has a path specified.");
        }

        public void Enqueue(T message, IDataExchangeQueueTransaction transaction)
        {
            Enqueue(message, DataExchangeQueuePriority.Undefined, transaction);
        }

        public void Enqueue(T message, DataExchangeQueuePriority priority, IDataExchangeQueueTransaction transaction)
        {
            if (!_orderedPriorities.Contains(priority))
            {
                throw new ArgumentException("Unable to accept the " + priority + " priority.", "priority");
            }

            try
            {
                using (var messageQueue = GetMessageQueue(priority))
                {
                    if (transaction is MsmqDataExchangeQueueTransaction)
                    {
                        messageQueue.Send<T>(message, ((MsmqDataExchangeQueueTransaction)transaction).Transaction);
                    }
                    else if (transaction is MsdtcDataExchangeTransaction)
                    {
                        messageQueue.Send<T>(message, MessageQueueTransactionType.Automatic);
                    }
                    else
                    {
                        throw new ArgumentException("Unexpected type of transaction.", "transaction");
                    }

                    Log.DebugExt($"Enqueuing message {message.ExternalReference} to the MSMQ queue with path '{messageQueue.Path}'.");
                    _serviceEventLogger.LogMessage(8203, messageQueue.Path); // Enqueuing message to the MSMQ queue with path '%01'
                }
            }
            catch (MessageQueueException exception)
            {
                Log.Error(exception);
                throw new DataExchangeQueueOperationFailedException(string.Format("Failed to enqueue message to the MSMQ queue with path {0}.", _msmqPaths[priority].FullPath), exception);
            }
        }

        public T Dequeue(TimeSpan timeout, IDataExchangeQueueTransaction transaction)
        {
            T dataExchangeMessage = null;
            var qno = _msmqPaths.WaitAny(timeout);
            if (qno >= 0)
            {
                Log.DebugExt($"Queue {qno} has a message");
                // We could just do a Receive() on the queue that returned from WaitAny. However, upon a system start up, there may be messages
                // in all queues, and we want to handle them in order of priority
                foreach (var priority in _orderedPriorities)
                {
                    using (var messageQueue = GetMessageQueue(priority))
                    {
                        Log.DebugExt("Reading/Dequeue(), QueueName='" + messageQueue.QueueName + "'");
                        dataExchangeMessage = Dequeue(transaction, messageQueue);
                    }

                    if (dataExchangeMessage != null)
                    {
                        Log.DebugExt($"Found message: {dataExchangeMessage.ExternalReference}");
                        break;
                    }
                }
            }
            else if (_logNoMessageFound)
                Log.DebugExt("No messages found");

            return dataExchangeMessage;
        }

        public T Dequeue(string internalMessageId, IDataExchangeQueueTransaction transaction)
        {
            T dataExchangeMessage = null;

            foreach (var priority in _orderedPriorities)
            {
                using (var messageQueue = GetMessageQueue(priority))
                {
                    dataExchangeMessage = Dequeue(internalMessageId, transaction, messageQueue);
                }

                if (dataExchangeMessage != null)
                {
                    break;
                }
            }

            return dataExchangeMessage;
        }

        private T Dequeue(string internalMessageId, IDataExchangeQueueTransaction transaction, MessageQueue messageQueue)
        {
            T dataExchangeMessage;

            if (transaction is MsmqDataExchangeQueueTransaction)
            {
                dataExchangeMessage = messageQueue.Receive<T>(internalMessageId, ((MsmqDataExchangeQueueTransaction)transaction).Transaction);
            }
            else if (transaction is MsdtcDataExchangeTransaction)
            {
                dataExchangeMessage = messageQueue.Receive<T>(internalMessageId, MessageQueueTransactionType.Automatic);
            }
            else
            {
                throw new ArgumentException("Unexpected type of transaction.", "transaction");
            }

            if (dataExchangeMessage != null)
            {
                _serviceEventLogger.LogMessage(8204, messageQueue.Path); // Receiving message from the MSMQ queue with path '%01'
            }

            return dataExchangeMessage;
        }

        public List<T> Dequeue(int maxNumberOfItems, IDataExchangeQueueTransaction transaction)
        {
            var dataExchangeMessages = new List<T>();

            foreach (var priority in _orderedPriorities)
            {
                if (dataExchangeMessages.Count >= maxNumberOfItems)
                {
                    break;
                }

                using (var messageQueue = GetMessageQueue(priority))
                {
                    while (true)
                    {
                        if (dataExchangeMessages.Count >= maxNumberOfItems)
                        {
                            break;
                        }

                        var dataExchangeMessage = Dequeue(transaction, messageQueue);

                        if (dataExchangeMessage == null)
                        {
                            break;
                        }

                        dataExchangeMessages.Add(dataExchangeMessage);
                    }
                }
            }

            return dataExchangeMessages;
        }

        private T Dequeue(IDataExchangeQueueTransaction transaction, MessageQueue messageQueue)
        {
            T dataExchangeMessage = null;

            try
            {
                if (transaction is MsmqDataExchangeQueueTransaction)
                {
                    dataExchangeMessage = messageQueue.Receive<T>(((MsmqDataExchangeQueueTransaction)transaction).Transaction);
                }
                else if (transaction is MsdtcDataExchangeTransaction)
                {
                    dataExchangeMessage = messageQueue.Receive<T>(MessageQueueTransactionType.Automatic);
                }
                else
                {
                    throw new ArgumentException("Unexpected type of transaction.", "transaction");
                }

                if (dataExchangeMessage != null)
                {
                    _serviceEventLogger.LogMessage(8204, messageQueue.Path); // Receiving message from the MSMQ queue with path '%01'
                }
            }
            catch (MessageQueueException exception)
            {
                if (exception.MessageQueueErrorCode != MessageQueueErrorCode.IOTimeout)
                {
                    throw new DataExchangeQueueOperationFailedException(string.Format("Failed to dequeue a message on the message queue {0}.", messageQueue.Path), exception);
                }
            }
            catch(InvalidMessageBodyException exception)
            {
                throw new DataExchangeInvalidMessageException(exception.MessageId, string.Format("Unable to interpret the content of the message in the message queue {0}, the message is possibly corrupt or invalid.", messageQueue.Path), exception);
            }

            return dataExchangeMessage;
        }

        public T Peek(TimeSpan timeout, out string internalMessageId)
        {
            T dataExchangeMessage = null;
            internalMessageId = null;

            if (_msmqPaths.WaitAny(timeout) >= 0)
            {
                foreach (var priority in _orderedPriorities)
                {
                    dataExchangeMessage = PeekInternal(priority, ref internalMessageId);

                    if (dataExchangeMessage != null)
                    {
                        break;
                    }
                }
            }

            return dataExchangeMessage;
        }

        public T PeekForPriority(DataExchangeQueuePriority priority, out string internalMessageId)
        {
            internalMessageId = null;

            T dataExchangeMessage = PeekInternal(priority, ref internalMessageId);

            return dataExchangeMessage;
        }

        private T PeekInternal(DataExchangeQueuePriority priority, ref string internalMessageId)
        {
            T dataExchangeMessage = null;
            using (var messageQueue = GetMessageQueue(priority))
            {
                try
                {
                    dataExchangeMessage = messageQueue.Peek<T>(out internalMessageId);
                }
                catch (MessageQueueException exception)
                {
                    if (exception.MessageQueueErrorCode != MessageQueueErrorCode.IOTimeout)
                    {
                        throw new DataExchangeQueueOperationFailedException(string.Format("Failed to peek a message on the message queue {0}.", messageQueue.Path), exception);
                    }
                }
                catch (InvalidMessageBodyException exception)
                {
                    throw new DataExchangeInvalidMessageException(exception.MessageId, string.Format("Unable to interpret the content of the message in the message queue {0}, the message is possibly corrupt or invalid.", messageQueue.Path), exception);
                }
            }
            return dataExchangeMessage;
        }


        public IDictionary<string, T> Peek(int maxNumberOfItems)
        {
            var dataExchangeMessages = new Dictionary<string, T>();

            foreach (var priority in _orderedPriorities)
            {
                if (dataExchangeMessages.Count >= maxNumberOfItems)
                {
                    break;
                }

                using (var messageQueue = GetMessageQueue(priority))
                {
                    var messageIds = messageQueue.GetMessageIds();

                    foreach (var messageId in messageIds)
                    {
                        if (dataExchangeMessages.Count >= maxNumberOfItems)
                        {
                            break;
                        }

                        try
                        {
                            var dataExchangeMessage = messageQueue.PeekById<T>(messageId);

                            if (dataExchangeMessage == null)
                            {
                                break;
                            }

                            dataExchangeMessages.Add(messageId, dataExchangeMessage);
                        }
                        catch (MessageQueueException exception)
                        {
                            if (exception.MessageQueueErrorCode == MessageQueueErrorCode.MessageNotFound)
                            {
                                throw new DataExchangeQueueOperationFailedException(string.Format("Failed to peek a message by id on the message queue {0}.", messageQueue.Path), exception);
                            }
                        }
                        catch(InvalidMessageBodyException exception)
                        {
                            throw new DataExchangeInvalidMessageException(exception.MessageId, string.Format("Unable to interpret the content of the message in the message queue {0}, the message is possibly corrupt or invalid.", messageQueue.Path), exception);
                        }
                    }
                }
            }

            return dataExchangeMessages;
        }

        public void Remove(string internalMessageId, IDataExchangeQueueTransaction transaction)
        {
            foreach (var priority in _orderedPriorities)
            {
                using (var messageQueue = GetMessageQueue(priority))
                {
                    bool removed = true;

                    try
                    {
                        if (transaction is MsmqDataExchangeQueueTransaction)
                        {
                            messageQueue.Remove(internalMessageId, ((MsmqDataExchangeQueueTransaction)transaction).Transaction);
                        }
                        else if (transaction is MsdtcDataExchangeTransaction)
                        {
                            messageQueue.Remove(internalMessageId, MessageQueueTransactionType.Automatic);
                        }
                        else
                        {
                            throw new ArgumentException("Unexpected type of transaction.", "transaction");
                        }
                    }
                    catch (InvalidOperationException)
                    {
                        removed = false;
                    }

                    if (removed)
                    {
                        _serviceEventLogger.LogMessage(8205, messageQueue.Path); // Removing message from the MSMQ queue with path '%01'
                        break;
                    }
                }
            }
        }

        public void Remove(IList<string> internalMessageIds, IDataExchangeQueueTransaction transaction)
        {
            foreach (var internalMessageId in internalMessageIds)
            {
                Remove(internalMessageId, transaction);
            }
        }

        public int GetQueueSize()
        {
            return _msmqPaths.CountMessages();
        }

        public Dictionary<string, T> GetAll(IDataExchangeQueueTransaction transaction)
        {
            var dataExchangeMessages = new Dictionary<string, T>();

            foreach (var priority in _orderedPriorities)
            {
                using (var messageQueue = GetMessageQueue(priority))
                {
                    var allMessages = messageQueue.GetAllMessages();
                    foreach (var message in allMessages)
                    {
                        try
                        {
                            var messageBody = messageQueue.GetMessageBody<T>(message);
                            dataExchangeMessages.Add(message.Id, messageBody);
                        }
                        catch(InvalidMessageBodyException exception)
                        {
                            // When fetching all messages we remove invalid messages
                            _serviceEventLogger.LogCritical(exception, false);
                            _serviceEventLogger.LogMessage(8209);
                            Remove(message.Id, transaction);
                        }
                    }
                }
            }

            return dataExchangeMessages;
        }

        public void Dispose()
        {
            // Nothing to clean up here right now
        }

        private MessageQueue GetMessageQueue(DataExchangeQueuePriority priority)
        {
            MessageQueue messageQueue;

            try
            {
                messageQueue = new MessageQueue(_msmqPaths[priority].FullPath)
                    {
                        Formatter = new XmlMessageFormatter(new[] { typeof(T) })
                    };
            }
            catch (Exception ex)
            {
                throw new DataExchangeConfigurationException(string.Format(@"Failed to find a MSMQ queue with path {0}, this may be due to MSMQ not being installed on the current machine.

To install Message Queuing 4.0 on Windows Server 2008 or Windows Server 2008 R2
    In Server Manager, click Features.
    In the right-hand pane under Features Summary, click Add Features.
    In the resulting window, expand Message Queuing.
    Expand Message Queuing Services.
    Click Directory Services Integration (for computers joined to a Domain), then click HTTP Support.
    Click Next,then click Install.
    
To install Message Queuing 4.0 on Windows 7 or Windows Vista
   Open Control Panel.
   Click Programs and then, under Programs and Features, click Turn Windows Features on and off.
   Expand Microsoft Message Queue (MSMQ) Server, expand Microsoft Message Queue (MSMQ) Server Core, and then select the check boxes for the following Message Queuing features to install:
    * MSMQ Active Directory Domain Services Integration (for computers joined to a Domain).
    * MSMQ HTTP Support.
   Click OK.
   If you are prompted to restart the computer, click OK to complete the installation.
", _msmqPaths[priority].FullPath), ex);
            }

            return messageQueue;
        }

        public List<DataExchangeQueuePriority> GetPriorities()
        {
            return _orderedPriorities;
        }
    }
}
