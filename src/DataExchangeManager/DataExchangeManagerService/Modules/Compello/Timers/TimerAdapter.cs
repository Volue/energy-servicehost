using System.Timers;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Timers
{
    public class TimerAdapter :  ITimer
    {
        public double Interval { get; set; }
        private readonly Timer _timer = new Timer();

        public void Start()
        {
            _timer.Interval = Interval;
            _timer.Start();
        }

        public void Stop()
        {
            _timer.Stop();
        }

        public void Dispose()
        {
            _timer.Dispose();
        }

        public event ElapsedEventHandler Elapsed
        {
            add { _timer.Elapsed += value; }
            remove { _timer.Elapsed -= value; }
        }
    }
}
