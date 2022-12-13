using System;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
{
    public abstract class WsLogicBase
    {
        protected readonly Func<IDataExchangeApi> _dataExchangeApiFactory;
        private IServiceEventLogger _serviceEventLogger;

        public WsLogicBase(Func<IDataExchangeApi> dataExchangeApiFactory, IServiceEventLogger serviceEventLogger)
        {
            _dataExchangeApiFactory = dataExchangeApiFactory;
            _serviceEventLogger = serviceEventLogger;
        }

        public bool SubmitImport(DataExchangeImportMessage message)
        {
            bool messageIsHandled = false;
            var api = _dataExchangeApiFactory();
            using (IDataExchangeQueueTransaction transaction = api.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
            {
                transaction.Begin();
                try
                {
                    // Send the import to ImportApplicationManagerService. 
                    api.EnqueueImportMessage(message, transaction);
                    messageIsHandled = true;
                }
                catch (DataExchangeInvalidMessageException exception)
                {
                    // Invalid messages are discarded directly with a log message
                    api.EnqueueErroneousImportMessage(message, transaction); // Remove from import queue, but must be handled from the error queue.
                    LogError(exception.Message);
                    LogError(8209);
                    messageIsHandled = true;
                }
                catch (DataExchangeInvalidMetadataException exception)
                {
                    // Invalid messages are discarded directly with a log message
                    api.EnqueueErroneousImportMessage(message, transaction); // Remove from import queue, but must be handled from the error queue.
                    LogError(exception.Message);
                    LogError(8209);
                    messageIsHandled = true;
                }

                if (messageIsHandled)
                    transaction.Commit();
            }
            return messageIsHandled;
        }

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
