using System;
using System.Threading;
using Powel.Icc.Diagnostics;

namespace Powel.Icc.Messaging.DataExchangeCommon.Abstract
{
    public abstract class TaskBasedModule : ThreadBasedModule
    {
        protected TaskBasedModule(IServiceEventLogger serviceEventLogger)
            : base(serviceEventLogger)
        {
        }

        protected abstract TimeSpan SleepTime { get; }

        protected abstract bool TryExecuteSingleTask();

        protected sealed override void ExecuteUntilStopped()
        {
            do
            {
                bool isNewMessageFound;
                do
                {
                    isNewMessageFound = ExecuteSingle(TryExecuteSingleTask);
                } while (!IsStopRequested && isNewMessageFound);

                OnNoMoreMessages();

                if (!IsStopRequested)
                {
                    Thread.Sleep(SleepTime);
                }
            }
            while (!IsStopRequested);
        }

        protected virtual void OnNoMoreMessages() { }    // E.g. Disconnect
    }
}
