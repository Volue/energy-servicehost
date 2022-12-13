using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public enum DataExchangeQueueTransactionType
    {
        Enqueue,
        Dequeue,
        EnqueueAndDequeue,
        EnqueueAndRemoteDequeue
    }

    public interface IDataExchangeQueueTransaction : IDisposable
    {
        void Begin();

        void Commit();

        void Rollback();
    }
}
