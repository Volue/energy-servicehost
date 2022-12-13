using System.Messaging;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq
{
    public class MsmqDataExchangeQueueTransaction : TransactionWrapper<MessageQueueTransaction>
    {
        public MsmqDataExchangeQueueTransaction()
            : base(new MessageQueueTransaction())
        {
        }

        public MsmqDataExchangeQueueTransaction(MsmqDataExchangeQueueTransaction dataExchangeTransaction)
            : base(dataExchangeTransaction.Transaction)
        {
        }

        public MessageQueueTransaction Transaction
        {
            get { return transaction; }
        }

        public override void Begin()
        {
            ThrowIfDisposed();

            transaction.Begin();
        }

        public override void Commit()
        {
            ThrowIfDisposed();

            transaction.Commit();
        }

        public override void Rollback()
        {
            ThrowIfDisposed();

            transaction.Abort();
        }
    }
}
