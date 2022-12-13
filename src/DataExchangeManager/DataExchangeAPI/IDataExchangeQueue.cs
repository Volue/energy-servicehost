using System;
using System.Collections.Generic;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public interface IDataExchangeQueue<T> : IDisposable where T : DataExchangeMessageBase
    {
        void Enqueue(T message, IDataExchangeQueueTransaction transaction);
        void Enqueue(T message, DataExchangeQueuePriority priority, IDataExchangeQueueTransaction transaction);
        T Dequeue(TimeSpan timeout, IDataExchangeQueueTransaction transaction);
        List<T> Dequeue(int maxNumberOfItems, IDataExchangeQueueTransaction transaction);
        T Dequeue(string internalMessageId, IDataExchangeQueueTransaction transaction);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="timeout"></param>
        /// <param name="internalMessageId"></param>
        /// <returns></returns>
        T Peek(TimeSpan timeout, out string internalMessageId);

        T PeekForPriority(DataExchangeQueuePriority priority, out string internalMessageId);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="maxNumberOfItems"></param>
        /// <returns>Dictionart key is internalMessageId</returns>
        IDictionary<string, T> Peek(int maxNumberOfItems);
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="internalMessageId">This is the ID that the underlying queue is using to identify the message</param>
        /// <param name="transaction"></param>
        void Remove(string internalMessageId, IDataExchangeQueueTransaction transaction);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="internalMessageIds">These are the IDs that the underlying queue is using to identify the messages</param>
        /// <param name="transaction"></param>
        void Remove(IList<string> internalMessageIds, IDataExchangeQueueTransaction transaction);

        int GetQueueSize();

        Dictionary<string, T> GetAll(IDataExchangeQueueTransaction transaction);

        List<DataExchangeQueuePriority> GetPriorities();
    }
}
