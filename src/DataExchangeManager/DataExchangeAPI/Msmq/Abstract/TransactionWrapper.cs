using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq.Abstract
{
    public abstract class TransactionWrapper<T> : IDataExchangeQueueTransaction
        where T : IDisposable
    {
        protected readonly T transaction;

        private bool _isDisposed;

        protected TransactionWrapper(T transaction)
        {
            this.transaction = transaction;
        }

        public virtual void Begin()
        {
        }

        public virtual void Rollback()
        {
        }

        public abstract void Commit();

        public void Dispose()
        {
            if (_isDisposed)
            {
                return;
            }

            try
            {
                transaction.Dispose();
            }
            finally
            {
                _isDisposed = true;
            }
        }

        protected void ThrowIfDisposed()
        {
            if (_isDisposed)
            {
                throw new ObjectDisposedException("transaction has been already Disposed");
            }
        }
    }
}