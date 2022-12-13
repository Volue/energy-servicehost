using System;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
{
    public interface IHeartbeatTimer : IDisposable
    {
        void Run();

        void HeartbeatReceived();

        bool Enabled { get; set; }

        void Stop();
    }
}