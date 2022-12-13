using System;
using System.Data;
using System.Diagnostics;
using Powel.Icc.Interop;

namespace Powel.Icc.Diagnostics
{
    public interface IServiceEventLogger : IDisposable
    {
        [Obsolete]
        IEventLogModuleItem IccLog { get; }

        void LogMessage(int messageKey, params string[] args);

        void LogMessage(IccModule moduleKey, int messageKey, params string[] args);

        [Obsolete]
        IDbConnection GetLogConnection();

        void LogToEventLog(string message, EventLogEntryType type);

        void LogCritical(Exception ex, bool retrying);

        void LogCritical(Exception ex, int messageKey, bool retrying);

        void RecycleLog(int i);
    }
}
