using log4net;
using Powel.Icc.Diagnostics;
using System;
using System.Reflection;
using System.Threading;
using log4net.Util;

namespace Powel.Icc.Messaging.DataExchangeCommon.Abstract
{
    public abstract class ThreadBasedModule : IDataExchangeModule
    {
        protected static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        protected readonly IServiceEventLogger ServiceEventLogger;
        private readonly Thread _moduleThread;

        private volatile bool _stopRequested;
        private volatile bool _isRunning;
        protected volatile bool _isExecutingJobRightNow;

        protected ThreadBasedModule(IServiceEventLogger serviceEventLogger)
        {
            ServiceEventLogger = serviceEventLogger;

            _moduleThread = new Thread(ThreadCore);
        }

        public abstract string ModuleName { get; }

        public Exception FailureReason { get; private set; }

        public bool IsRunning => _isRunning;

        public bool IsExecutingJobRightNow => _isExecutingJobRightNow;

        protected bool IsStopRequested => _stopRequested;

        public virtual void Start()
        {
            _moduleThread.Start();
        }

        public void RequestStop()
        {
            _stopRequested = true;
        }

        public virtual void Stop(TimeSpan timeout)
        {
            Log.InfoExt($"{ModuleName}: Timeout: {timeout}");
            _stopRequested = true;

            if (_moduleThread.IsAlive && !_moduleThread.Join(timeout))
                Abort();
        }

        public virtual void Abort()
        {
            Log.InfoExt($"{ModuleName}: IsAlive: {_moduleThread.IsAlive}");
            if (_moduleThread.IsAlive)
                _moduleThread.Abort();
        }

        protected abstract void ExecuteUntilStopped();

        /// <summary>
        /// always call this method when doing something to make sure that we know when module is working
        /// </summary>
        protected bool ExecuteSingle(Func<bool> command)
        {
            bool isNewMessageFound = false;
            try
            {
                _isExecutingJobRightNow = true;
                isNewMessageFound = command.Invoke();
            }
            finally
            {
                _isExecutingJobRightNow = false;
            }
            return isNewMessageFound;
        }

        protected void LogError(string error)
        {
            ServiceEventLogger.LogMessage(8100, error);
        }

        protected void LogError(int msgKey, params string[] errorArgs)
        {
            ServiceEventLogger.LogMessage(msgKey, errorArgs);
        }

        private void ThreadCore()
        {
            _isRunning = true; // now we can truly say that the thread started running 

            try
            {
                ExecuteUntilStopped();
            }
            catch (Exception ex)
            {
                FailureReason = ex;
                //ServiceEventLogger.LogToEventLog($"{ModuleName}: {ex.Message}. Stopping.", EventLogEntryType.Error);
                //Log.Error($"{ModuleName}: The module thread caused an exception. Stopping.", ex);
                ServiceEventLogger.LogCritical(ex,false);
            }
            finally
            {
                _isRunning = false;
            }
        }
    }
}
