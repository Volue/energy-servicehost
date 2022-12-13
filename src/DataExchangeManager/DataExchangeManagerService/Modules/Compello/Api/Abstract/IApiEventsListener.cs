using System;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract
{
    public interface IApiEventsListener : IDisposable
    {
        event EventHandler<ClientErrorEventArgs> ClientError;
        event EventHandler<ImportMessageReceivedEventArgs> MessageReceived;
        event EventHandler<StatusChangeEventArgs> StatusChanged;
        event EventHandler<ClientHeartbeatEventArgs> Heartbeat;

        bool IsStarted { get; }

        void Start();
        void Stop(bool synchronously = false);

        void InvokeHeartbeat();
    }
}