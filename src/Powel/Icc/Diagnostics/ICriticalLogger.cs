using System;
using System.Diagnostics;

namespace Powel.Icc.Diagnostics
{
    public interface ICriticalLogger
    {
        void Log(Exception ex, int messageKey, IEventLogModuleItem iccLog, EventLog windowsLog, bool retrying);
        void Log(Exception ex, IEventLogModuleItem iccLog, EventLog windowsLog, bool retrying);
    }
}