using System;
using System.ServiceModel;
using System.Timers;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Timers
{
    public class HeartbeatTimer : IHeartbeatTimer
    {
        private const int GENERAL_ERROR_CODE = 8100;
        private IApiEventsListener _listener;
        public IApiEventsListener Listener { get { return _listener; } set { _listener = value; } }
        private readonly int _heartbeatInterval;
        private DateTime _lastHeartbeatReceived;
        private DateTime _lastHeartbeatInvoked;
        private readonly IServiceEventLogger _serviceEventLogger;
        private readonly ITimer _timer;
        private bool _heartbeatErrorLoged;
        private bool _stopRequested;

        public HeartbeatTimer(ISettingsProvider settingsProvider, IServiceEventLogger serviceEventLogger, ITimer timer)
        {
            _heartbeatInterval = settingsProvider.GetSettings().HeartbeatInterval;
            _serviceEventLogger = serviceEventLogger;
            _timer = timer;
            _timer.Elapsed += OnTimeEvent;
        }

        public void Run()
        {
            _timer.Interval = _heartbeatInterval;
            _timer.Start();
            _lastHeartbeatInvoked = DateTime.MinValue;
        }

        public void Dispose()
        {
            _timer.Elapsed -= OnTimeEvent;
            _timer.Dispose();
        }

        public void HeartbeatReceived()
        {
            _lastHeartbeatReceived = DateTime.Now;
        }

        public void RequestStop()
        {
            _stopRequested = true;
        }

        public void Stop()
        {
            _timer.Stop();
        }

        public bool Enabled { get; set; }   // Not in use by now. Look at MadesHeartbeatTimer to see use.

        void OnTimeEvent(object sender, ElapsedEventArgs e)
        {
            try
            {
                LogErrorMessageIfHeartbeatNotReceived();
                LogHeartbeatReceivedCorrectly();
                _lastHeartbeatInvoked = DateTime.Now;
                if (!_stopRequested)
                    _listener.InvokeHeartbeat();
            }
            catch (CommunicationException Ex)
            {
                _serviceEventLogger.LogMessage(GENERAL_ERROR_CODE,Ex.Message);
                _heartbeatErrorLoged = true;
            }
        }

        private void LogErrorMessageIfHeartbeatNotReceived()
        {
            if (!IsPreviousHeartbeatReceived() && !_heartbeatErrorLoged)
            {
                _serviceEventLogger.LogMessage(30224);  // Compello heartbeat didn't receive.
                _heartbeatErrorLoged = true;
            }
        }

        private void LogHeartbeatReceivedCorrectly()
        {
            if (IsPreviousHeartbeatReceived() && _heartbeatErrorLoged 
                || _lastHeartbeatInvoked == DateTime.MinValue)
            {
                _serviceEventLogger.LogMessage(30225);  // Compello heartbeat received correctly
                _heartbeatErrorLoged = false;
            }
        }

        private bool IsPreviousHeartbeatReceived()
        {
            return DateTime.Compare(_lastHeartbeatReceived, _lastHeartbeatInvoked) >= 0;
        }
    }
}
