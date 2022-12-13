using System.Timers;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Mades
{
    public class MadesHeartbeatTimer : IHeartbeatTimer
    {
        private const int HeartbeatFailed = 30391;
        private const int HeartbeatOk = 30392;
        private readonly Timer _timer = new Timer();
        private MadesImportModule _module;
        private IServiceEventLogger _serviceEventLogger;
        private bool _heartbeatFailed;
        private bool _firstEvent = true;

        public MadesHeartbeatTimer(double interval, MadesImportModule module, IServiceEventLogger serviceEventLogger)
        {
            _serviceEventLogger = serviceEventLogger;
            _module = module;

            _timer.Interval = interval;
            _timer.Elapsed += OnTimeEvent;
        }

        public void Run()
        {
            _timer.Start();
        }

        public bool Enabled
        {
            get { return _timer.Enabled; }
            set {
                _timer.Enabled = value;
                _firstEvent = true;
            }
        }

        public void HeartbeatReceived()
        {
            // No heartbeat reception for this module.
            throw new System.NotImplementedException();
        }

        public void Stop()
        {
            _timer.Stop();
            _firstEvent = true;
        }

        private void OnTimeEvent(object sender, ElapsedEventArgs e)
        {
            bool ok = _module.InvokeHeartbeat();
            if (_firstEvent)
            {
                _serviceEventLogger.LogMessage(ok ? HeartbeatOk : HeartbeatFailed,_module.ModuleName);
                _firstEvent = false;
                _heartbeatFailed = !ok;
                return;
            }

            if (!ok)
            {
                if (!_heartbeatFailed)  // Log only when state is changed.
                    _serviceEventLogger.LogMessage(HeartbeatFailed, _module.ModuleName);
                _heartbeatFailed = true;
                _module.StopPolling();
                return;
            }

            // ok
            if (_heartbeatFailed)
                _serviceEventLogger.LogMessage(HeartbeatOk,_module.ModuleName);
            _heartbeatFailed = false;
        }

        public void Dispose()
        {
            Stop();
        }
    }
}