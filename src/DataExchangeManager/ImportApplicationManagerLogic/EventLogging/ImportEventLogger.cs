using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Enums;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging
{
    public class ImportEventLogger : IImportEventLogger
    {
        private readonly IServiceEventLogger _serviceEventLogger;

        public ImportEventLogger(IServiceEventLogger serviceEventLogger)
        {
            _serviceEventLogger = serviceEventLogger;
        }

        public void LogSuccessfulImport(DataExchangeImportMessage message)
        {
            _serviceEventLogger.LogMessage((int)MessageKeyType.ImportSuccess, GetLogArgs(message));
        }

        public void LogFailedImport(DataExchangeImportMessage message)
        {
            _serviceEventLogger.LogMessage((int)MessageKeyType.ImportFailure, GetLogArgs(message));
        }

        private static string[] GetLogArgs(DataExchangeImportMessage message)
        {
            return new[]
                {
                    message.ExternalReference??string.Empty, message.SenderName??string.Empty,
                    message.ReceiverName??string.Empty, message.Format ?? message.Protocol
                };
        }
    }
}
