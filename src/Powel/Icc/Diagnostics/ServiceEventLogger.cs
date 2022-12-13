using System;
using System.Data;
using System.Diagnostics;
using Powel.Icc.Interop;

namespace Powel.Icc.Diagnostics
{
    public class ServiceEventLogger : IServiceEventLogger
    {
        public const int InfoMessage = 1010;
        public const int WarningMessage = 1011;
        public const int ErrorMessage = 1012;
        public const int FatalMessage = 1019;

        private readonly ICriticalLogger _criticalLogger;
        private readonly Func<IEventLogModuleItem> _iccLogFactory;
        private readonly Func<EventLog> _windowsLogFactory;

        public ServiceEventLogger(Func<IEventLogModuleItem> iccLogFactory, Func<EventLog> windowsLogFactory, ICriticalLogger criticalLogger)
		{
		    _iccLogFactory = iccLogFactory;
            _windowsLogFactory = windowsLogFactory;
		    _criticalLogger = criticalLogger;
		}

        public IEventLogModuleItem IccLog { get { return _iccLogFactory(); } }

        public void LogMessage(int messageKey, params string[] args)
		{
            _iccLogFactory().LogMessage(messageKey, args);
		}

        public void LogMessage(IccModule moduleKey, int messageKey, params string[] args)
        {
            _iccLogFactory().LogMessage(moduleKey, messageKey, args);
        }

        public IDbConnection GetLogConnection()
        {
            return _iccLogFactory().Connection;
        }

        public void LogToEventLog(string message, EventLogEntryType type)
		{
			// Logging to event log can fail if:
			// * The application source is not created and a non-admin tries to automatically create it on logging.
			// * The log is full.
            try
            {
                _windowsLogFactory().WriteEntry(message, type);
            }
            catch
            {
            }
		}

        public void LogCritical(Exception ex, bool retrying)
		{
			_criticalLogger.Log(ex, _iccLogFactory(), _windowsLogFactory(), retrying);
		}

        public void LogCritical(Exception ex, int messageKey, bool retrying)
        {
            _criticalLogger.Log(ex, messageKey, _iccLogFactory(), _windowsLogFactory(), retrying);
        }

        public void RecycleLog(int i)
		{
			_iccLogFactory().Recycle(i);
		}

		public void Dispose()
		{
			_iccLogFactory().Dispose();
		}
    }
}
