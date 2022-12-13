using System;
using System.Timers;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract
{
    public interface ITimer : IDisposable
    {
        event ElapsedEventHandler Elapsed;

        double Interval { get; set; }

        void Start();

        void Stop();
    }
}
