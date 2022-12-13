using System.Transactions;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq
{
    public class MsdtcDataExchangeTransaction : TransactionWrapper<TransactionScope>
    {
        public MsdtcDataExchangeTransaction()
            : base(new TransactionScope(TransactionScopeOption.Required))
        {
        }

        public override void Commit()
        {
            transaction.Complete();
        }
    }
}
