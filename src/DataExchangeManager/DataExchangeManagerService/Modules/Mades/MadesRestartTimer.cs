using System;
using System.Timers;
using log4net;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Mades
{
    public class MadesRestartTimer : IRestartTimer
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private readonly Timer _timer = new Timer();
        private MadesImportModule _module;

        public MadesRestartTimer(double interval, MadesImportModule module)
        {
            _timer.Interval = interval;
            _module = module;
        }

        public void Run()
        {
            if (_timer.Enabled)
                return;
            _timer.Elapsed += OnTimerElapsed;
            _timer.Start();
        }

        private void OnTimerElapsed(object sender, ElapsedEventArgs args)
        {
            if (!_module.IsPollingStopped)
                return;

            // Do a test first to avoid enabling heartbeat timer when the connection is down.
            var ok = _module.InvokeHeartbeat();
            if (!ok)
                return;

            if (Log.IsInfoEnabled) Log.Info("Restarting.");
            _module.StartHeartbeatTimer();
            Stop();
        }

        public void Stop(bool synchronously = false)
        {
            _timer.Elapsed -= OnTimerElapsed;
            _timer.Stop();
        }

        public void Dispose()
        {
            Stop();
        }
    }
}