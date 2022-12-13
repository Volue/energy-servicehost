using System;
using System.Collections.Generic;
using log4net;
using Powel.Icc.Diagnostics;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
{
    public abstract class EventBasedModule<TEventClient> : IDataExchangeModule, IDisposable
        where TEventClient : IDisposable
    {
        private const int GENERAL_INFO_MESSAGE = 1010;
        private const int GENERAL_ERROR_CODE = 8100;

        protected static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private readonly IFactory<TEventClient> _eventClientFactory;
        private readonly IServiceEventLogger _serviceEventLogger;

        private TEventClient _eventClient;

        private bool _isDisposed = false;
        private bool _isRunning;

        protected EventBasedModule(IFactory<TEventClient> eventClientFactory, IServiceEventLogger serviceEventLogger)
        {
            _eventClientFactory = eventClientFactory;
            _serviceEventLogger = serviceEventLogger;
        }

        public Exception FailureReason { get; private set; }

        public bool IsExecutingJobRightNow
        {
            get { return false; } // Event based modules should not be controlled like this. They should work when an event is signaled.
        }  

        public abstract string ModuleName { get; }

        public bool IsRunning
        {
            get { return _isRunning; }
        }

        protected abstract void SubscribeToEvents(TEventClient client);

        protected abstract void UnsubscribeFromEvents(TEventClient client);

        protected abstract void StartListening(TEventClient client);

        protected abstract void StopListening(TEventClient client);

        public void Start()
        {
            Log.Info($"{ModuleName}: Starting.");
            if (IsRunning)
                return;
            LogEventLogMessage(GENERAL_INFO_MESSAGE, new string[] { $"{ModuleName} is Starting." });
            _eventClient = _eventClientFactory.Create();
            _isRunning = true;

            SubscribeToEvents(_eventClient);

            StartListening(_eventClient);
        }

        public abstract void RequestStop();

        public void Stop(TimeSpan timeout)
        {
            Log.Info($"{ModuleName}: Timeout: {timeout}");
            if (!_isRunning)
                return;
            LogEventLogMessage(GENERAL_INFO_MESSAGE, new string[] { $"{ModuleName} is stopping." });
            StopListening(_eventClient);
            UnsubscribeFromEvents(_eventClient);
            _isRunning = false;
        }

        public void Abort()
        {
            Log.Info($"{ModuleName}");
            Stop(TimeSpan.Zero);
        }

        public void Dispose()
        {
            if (_isDisposed)
            {
                return;
            }

            try
            {
                if (!EqualityComparer<TEventClient>.Default.Equals(_eventClient, default(TEventClient)))
                {
                    Abort();
                    _eventClient.Dispose();
                }
            }
            finally
            {
                _isDisposed = true;
            }
        }

        protected void LogError(Exception error)
        {
            try
            {
                FailureReason = error;

                _serviceEventLogger.LogMessage(GENERAL_ERROR_CODE, error.Message);
            }
            finally
            {
                Log.Error($"{ModuleName}: Exception occured!", error);
            }
        }

        protected void LogEventLogMessage(int messageKey, string[] messageArguments)
        {
            _serviceEventLogger.LogMessage(messageKey, messageArguments);
        }
    }
}
