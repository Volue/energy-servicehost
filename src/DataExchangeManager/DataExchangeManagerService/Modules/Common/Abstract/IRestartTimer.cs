using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
{
    public interface IRestartTimer : IDisposable
    {
        void Run();
        void Stop(bool synchronously = false);
    }
}