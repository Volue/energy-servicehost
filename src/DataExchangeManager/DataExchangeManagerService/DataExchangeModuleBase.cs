using System;
using System.Threading;
using Powel.Icc.Diagnostics;
using log4net;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService
{
    public abstract class DataExchangeModuleBase : IDataExchangeModuleBase
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        protected IDataExchangeApi DataExchangeApi { get { return _dataExchangeApi; } }

        private readonly IDataExchangeApi _dataExchangeApi;
        private volatile bool _isRunning;
        private volatile bool _stopRequested;
        private readonly IServiceEventLogger _serviceEventLogger;
        private Thread _moduleThread;

        protected DataExchangeModuleBase(IDataExchangeApi dataExchangeApi, IServiceEventLogger serviceEventLogger)
        {
            _dataExchangeApi = dataExchangeApi;
            _serviceEventLogger = serviceEventLogger;
        }

        public virtual string ModuleName
        {
            get { throw new NotImplementedException(); }
        }

        public bool IsRunning()
        {          
            return _isRunning;
        }

        protected void RunningComplete()
        {
            _isRunning = false;
        }

        protected bool StopRequested()
        {
            return _stopRequested;
        }

        public void Start()
        {
            _isRunning = true;

            _moduleThread = new Thread(delegate()
                {
                    try
                    {
                        RunThread();
                    }
                    catch (ThreadAbortException)
                    {
                    }
                    catch (Exception ex)
                    {
                        Log.Debug("The module thread caused an exception.", ex);
                        Console.WriteLine("The module thread caused an exception.");
                        Console.WriteLine(ex.ToString());
                        Exception = ex;
                    }
                    finally
                    {
                        _isRunning = false;
                    }
                });

            _moduleThread.Start();
        }

        public void Stop(bool waitForThreadToJoin = false)
        {
            if (Log.IsDebugEnabled) { Log.Debug("Will stop thread"); }
            _stopRequested = true;

            if (waitForThreadToJoin)
            {
                _moduleThread.Join();
            }
        }

        public Exception Exception { get; private set; } 

        public void AbortModuleThread()
        {
            if (_moduleThread != null && _moduleThread.IsAlive)
            {
                if (Log.IsDebugEnabled) { Log.Debug("Will abort thread"); }
                _moduleThread.Abort();
            }
        }

        protected abstract void RunThread();

        protected void LogError(string error)
        {
            _serviceEventLogger.LogMessage(8100, error);
        }

        protected void LogError(int msgKey, params string[] errorArgs)
        {
            _serviceEventLogger.LogMessage(msgKey, errorArgs);
        }
    }
}
