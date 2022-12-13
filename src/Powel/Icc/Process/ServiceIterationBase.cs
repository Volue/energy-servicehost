using System;
using System.Diagnostics;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;

namespace Powel.Icc.Process
{
	/// <summary>
	/// Summary description for ServiceIterationBase.
	/// </summary>
	public abstract class ServiceIterationBase : IDisposable
	{
	    private readonly IServiceEventLogger _serviceEventLogger;
	    protected IServiceEventLogger ServiceEventLogger
	    {
	        get { return _serviceEventLogger; }
	    }

	    private volatile bool _stopRequested = false;

        [Obsolete]
	    protected EventLogModuleItem iccLog
	    {
	        get { return (EventLogModuleItem) _serviceEventLogger.IccLog; }
	    }

		protected ServiceIterationBase(IServiceEventLogger serviceEventLogger)
		{
		    _serviceEventLogger = serviceEventLogger;
		    IsRunning = true;
		}

        [Obsolete]
		protected ServiceIterationBase(EventLog eventLog, EventLogModuleItem iccLog) : this(new ServiceEventLogger(() => iccLog, () => eventLog, new CriticalLogger()))
		{
        }

	    public abstract string ServiceIterationName { get; }

		public abstract IccModule ServiceIterationModule { get; }

		public abstract void Initialize();
		
        public abstract void RunIteration(out bool actualWorkDone);

        public void RecycleLog(int i)
        {
            _serviceEventLogger.RecycleLog(i);
        }

		protected void LogToEventLog(string message, EventLogEntryType type)
		{
            _serviceEventLogger.LogToEventLog(message, type);
		}

	    public void CriticalLog(Exception ex)
	    {
            _serviceEventLogger.LogCritical(ex, true);
	    }
        
        public bool IsRunning
        {
            get; set;
        }

	    public virtual void RequestStop()
	    {
	        _stopRequested = true;
	    }

        public bool StopRequested()
        {
            return _stopRequested;
        }

	    public void Dispose()
	    {
            // dispose the log in order to call close so event log is going to be notified
	        _serviceEventLogger.Dispose();
	    }
	}
}
