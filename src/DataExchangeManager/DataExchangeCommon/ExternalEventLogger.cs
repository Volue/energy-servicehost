using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;

namespace Powel.Icc.Messaging.DataExchangeCommon
{
    public class ExternalEventLogger : IExternalEventLogger
    {
        private const int INFO_MESSAGE_KEY = 30156;
        private const int WARNING_MESSAGE_KEY = 30157;
        private const int ERROR_MESSAGE_KEY = 30158;
        private const int FATAL_MESSAGE_KEY = 30159;

        private readonly IServiceEventLogger _serviceEventLogger;

        public ExternalEventLogger(IServiceEventLogger serviceEventLogger)
        {
            _serviceEventLogger = serviceEventLogger;
        }

        public void Info(string moduleName, string message)
        {
            LogMessage(INFO_MESSAGE_KEY, moduleName, message);
        }

        public void Warning(string moduleName, string message)
        {
            LogMessage(WARNING_MESSAGE_KEY, moduleName, message);
        }

        public void Error(string moduleName, string message)
        {
            LogMessage(ERROR_MESSAGE_KEY, moduleName, message);
        }

        public void Fatal(string moduleName, string message)
        {
            LogMessage(FATAL_MESSAGE_KEY, moduleName, message);
        }

        private void LogMessage(int messageKey, string moduleName, string message)
        {
            _serviceEventLogger.LogMessage(messageKey, moduleName, message);
        }
    }
}
