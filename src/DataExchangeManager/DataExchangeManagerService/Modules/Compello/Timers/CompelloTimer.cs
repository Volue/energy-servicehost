using System;
using System.ServiceModel;
using System.Timers;
using log4net;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Timers
{
    public class CompelloTimer : IRestartTimer
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private const int GENERAL_ERROR_CODE = 8100;
        private readonly int _restartInterval;
        private readonly IServiceEventLogger _serviceEventLogger;
        private readonly ITimer _timer;
        private IApiEventsListener _listener;
        public IApiEventsListener Listener { get { return _listener; } set { _listener = value;} }
        private IHeartbeatTimer _heartbeatTimer;

        public CompelloTimer(ISettingsProvider settingsProvider, IServiceEventLogger serviceEventLogger, ITimer timer, IHeartbeatTimer heartbeatTimer)
        {
            _restartInterval = settingsProvider.GetSettings().RestartInterval;
            _serviceEventLogger = serviceEventLogger;
            _timer = timer;
            _heartbeatTimer = heartbeatTimer;
        }

        public void Dispose()
        {
            Stop();
            _timer.Dispose();
        }

        public void Run()
        {
            _timer.Elapsed += OnTimeEvent;
            _timer.Interval = _restartInterval;
            _timer.Start();
        }

        public void Stop(bool synchronously = false)
        {
            _listener.Stop(synchronously);
            _heartbeatTimer.Stop();
            _timer.Stop();
        }

        void OnTimeEvent(object sender, ElapsedEventArgs e)
        {
            if (_listener.IsStarted)
                return;

            if (Log.IsInfoEnabled) Log.Info("Restarting.");
            try
            {
                _listener.Start();
                _heartbeatTimer.Run();
                _serviceEventLogger.LogMessage(30260,null); //Data Exchange Manager has started the connection to the Compello EDI-server.
            }
            catch (InvalidOperationException Ex)
            {
                // This BackgroundWorker is currently busy and cannot run multiple tasks concurrently.
                Log.Error(Ex.Message);
                _listener.Stop(true);
            }
            catch (EndpointNotFoundException Ex)
            {
                Log.Error(Ex.Message);
                _listener.Stop(true);
            }
        }
    }
}
