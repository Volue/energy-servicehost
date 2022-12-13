using System;
using System.Threading;
using Powel.Icc.Diagnostics;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
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

                if (!IsStopRequested)
                {
                    Thread.Sleep(SleepTime);
                }
            }
            while (!IsStopRequested);
        }
    }
}
