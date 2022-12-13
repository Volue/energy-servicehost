using System;
using EDI.Server.API.Client;
using log4net;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Events;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api
{
    public sealed class ApiEventsListener : IApiEventsListener
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private readonly ApiClient _apiClient;

        private readonly EventHandlerManager<ApiErrorEventArgs, ClientErrorEventArgs> _clientErrorHandlerManager;
        private readonly EventHandlerManager<MessageReceivedEventArgs, ImportMessageReceivedEventArgs> _messageReceivedHandlerManager;
        private readonly EventHandlerManager<MessageStatusChangedEventArgs, StatusChangeEventArgs> _statusChangedHandlerManager;
        private readonly EventHandlerManager<HeartbeatEventArgs, ClientHeartbeatEventArgs> _heartbeatReceivedHandlerManager;

        private bool _isDisposed;

        public ApiEventsListener(Settings settings)
        {
            if (Log.IsInfoEnabled) Log.Info($"ApiEventsListener: HostAddress={settings.HostAddress}, Port={settings.Port}, ApiKey={settings.ApiKey}");
            // Two different .dll's with the same name and interface.
            _apiClient = settings.Port > 0 
                ? new ApiClient(settings.HostAddress, settings.Port, settings.ApiKey)    // net_tcp address for old FMS/local EDIserver etc.
                : new ApiClient(settings.ApiKey, settings.HostAddress); // https address for Azure.

            _clientErrorHandlerManager = new EventHandlerManager<ApiErrorEventArgs, ClientErrorEventArgs>
                (
                    handler => _apiClient.Error += handler,
                    handler => _apiClient.Error -= handler,
                    eventArgs => new ClientErrorEventArgs(eventArgs.Exception)
                );
            _messageReceivedHandlerManager = new EventHandlerManager<MessageReceivedEventArgs, ImportMessageReceivedEventArgs>
                (
                    handler => _apiClient.MessageReceived += handler,
                    handler => _apiClient.MessageReceived -= handler,
                    eventArgs => new ImportMessageReceivedEventArgs(eventArgs.MessageId, eventArgs.Metadata)
                );
            _statusChangedHandlerManager = new EventHandlerManager<MessageStatusChangedEventArgs, StatusChangeEventArgs>
                (
                    handler => _apiClient.MessageStatusChanged += handler,
                    handler => _apiClient.MessageStatusChanged -= handler,
                    eventArgs => MessageStatusService.CreateArguments(eventArgs.Status, eventArgs.TransactionId)
                );
            _heartbeatReceivedHandlerManager = new EventHandlerManager<HeartbeatEventArgs, ClientHeartbeatEventArgs>
                (
                    handler => _apiClient.HeartbeatReceived += handler,
                    handler => _apiClient.HeartbeatReceived -= handler,
                    eventArgs => new ClientHeartbeatEventArgs(eventArgs.NodeCount)
                );
        }

        public event EventHandler<ClientErrorEventArgs> ClientError
        {
            add { _clientErrorHandlerManager.Handler += value; }
            remove { _clientErrorHandlerManager.Handler -= value; }
        }

        public event EventHandler<ImportMessageReceivedEventArgs> MessageReceived
        {
            add { _messageReceivedHandlerManager.Handler += value; }
            remove { _messageReceivedHandlerManager.Handler -= value; }
        }

        public event EventHandler<StatusChangeEventArgs> StatusChanged
        {
            add { _statusChangedHandlerManager.Handler += value; }
            remove { _statusChangedHandlerManager.Handler -= value; }
        }

        public event EventHandler<ClientHeartbeatEventArgs> Heartbeat
        {
            add { _heartbeatReceivedHandlerManager.Handler += value; }
            remove { _heartbeatReceivedHandlerManager.Handler -= value; }
        }

        public bool IsStarted { get; private set; }

        public void Start()
        {
            if (IsStarted)
                return;
            if (Log.IsInfoEnabled) Log.Info("Starting");
            /* If this is called after a Stop() and before the internal EDI-server polling interval of 1 minute has elapsed (should be handled by CompelloTimer.Interval),
             * it may throw an exception saying it is busy. We try again for 2 minutes (should be parameterized if this is something that needs to be controlled better)
             * Therefore we loop on this function
             */
            var nTries = 0;
            while (!IsStarted)
            {
                try
                {
                    _apiClient.Start();
                    IsStarted = true;
                }
                catch (InvalidOperationException Ex)
                {
                    if (Log.IsInfoEnabled) Log.Info(Ex.Message);
                    nTries++;
                    if (nTries > 3)
                    {
                        if (Log.IsInfoEnabled) Log.Info($"Tried {nTries} times. Skipping out, waiting for new restart.");
                        throw;
                    }
                }
            }
            if (Log.IsInfoEnabled) Log.Info("Started");
        }

        private bool _isStoppingAsync = false;   // Flag to indicate if we are in an asynchronously stopping operation.

        public void Stop(bool synchronously = false)
        {
            if (!IsStarted || _isStoppingAsync)
                return;
            if (Log.IsInfoEnabled) Log.Info($"Stopping synchronously = {synchronously}");
            /* The _apiClient.Stop(false) returns immmediately, but in the backend there is a polling activity 
             * which has to be finished before .Start() can be called.
             */
            if (!synchronously)
            {
                _apiClient.Stopped += OnStopped;
                _isStoppingAsync = true;
            }
            _apiClient.Stop(synchronously); // Only Asynchronously if called from OnClientError
            if (synchronously)
            {
                IsStarted = false;
                if (Log.IsInfoEnabled) Log.Info("Stopped synchronously");
            }
        }

        private void OnStopped(object sender, EventArgs args)
        {
            // Invoked on finished with asynchronous stop.
            IsStarted = false;
            if (Log.IsInfoEnabled) Log.Info("Stopped asynchronously");
            _apiClient.Stopped -= OnStopped;
            _isStoppingAsync = false;
        }

        public void InvokeHeartbeat()
        {
            if (Log.IsInfoEnabled) Log.Info("InvokeHeartbeat");
            _apiClient.Heartbeat();
        }

        public void Dispose()
        {
            if (Log.IsInfoEnabled) Log.Info("Dispose");
            if (_isDisposed)
            {
                return;
            }

            try
            {
                Stop(true);
                _statusChangedHandlerManager.Dispose();
                _messageReceivedHandlerManager.Dispose();
                _clientErrorHandlerManager.Dispose();
                _heartbeatReceivedHandlerManager.Dispose();

                _apiClient.Dispose();
            }
            finally
            {
                _isDisposed = true;
            }
        }
    }
}
