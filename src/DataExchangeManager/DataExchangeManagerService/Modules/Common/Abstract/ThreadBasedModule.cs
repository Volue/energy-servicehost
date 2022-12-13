using System;
using System.Diagnostics;
using System.Threading;
using System.Reflection;
using log4net;
using Powel.Icc.Diagnostics;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
{
    public abstract class ThreadBasedModule : IDataExchangeModule
    {
        protected static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        protected readonly IServiceEventLogger ServiceEventLogger;
        private readonly Thread _moduleThread;

        private volatile bool _stopRequested;
        private volatile bool _isRunning;
        private volatile bool _isExecutingJobRightNow;

        protected ThreadBasedModule(IServiceEventLogger serviceEventLogger)
        {
            ServiceEventLogger = serviceEventLogger;

            _moduleThread = new Thread(ThreadCore);
        }

        public abstract string ModuleName { get; }

        public Exception FailureReason { get; private set; }

        public bool IsRunning
        {
            get { return _isRunning; }
        }

        public bool IsExecutingJobRightNow
        {
            get { return _isExecutingJobRightNow; }
        }

        protected bool IsStopRequested
        {
            get { return _stopRequested; }
        }

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
            if (Log.IsInfoEnabled) Log.Info($"{ModuleName}: Timeout: {timeout}");
            _stopRequested = true;

            if (!_moduleThread.Join(timeout))
                Abort();
        }

        public void Abort()
        {
            if (Log.IsInfoEnabled) Log.Info($"{ModuleName}: IsAlive: {_moduleThread.IsAlive}");
            if (_moduleThread.IsAlive)
            {
                _moduleThread.Abort();
            }
        }

        protected abstract void ExecuteUntilStopped();

        /// <summary>
        /// always call this method when doing something to make sure that we know when module is working
        /// </summary>
        protected bool ExecuteSingle(Func<bool> command)
        {
            _isExecutingJobRightNow = true;
            bool isNewMessageFound = command.Invoke();
            _isExecutingJobRightNow = false;

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
                ServiceEventLogger.LogToEventLog(ex.Message, EventLogEntryType.Error);
                Log.Error($"{ModuleName}: The module thread caused an exception.", ex);
            }
            finally
            {
                _isRunning = false;
            }
        }
    }
}