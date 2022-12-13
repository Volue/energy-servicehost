using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService
{
    public interface IDataExchangeModuleBase
    {
        string ModuleName { get; }
        Exception Exception { get; }
        bool IsRunning();
        void Start();
        void Stop(bool waitForThreadToJoin = false);
        void AbortModuleThread();
    }
}
