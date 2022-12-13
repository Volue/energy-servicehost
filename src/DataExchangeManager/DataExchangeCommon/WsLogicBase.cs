using System;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeCommon
{
    public abstract class WsLogicBase : IWsLogicBase
    {
        protected readonly Func<IDataExchangeApi> _dataExchangeApiFactory;
        protected IServiceEventLogger _serviceEventLogger;

        public WsLogicBase(Func<IDataExchangeApi> dataExchangeApiFactory, IServiceEventLogger serviceEventLogger)
        {
            _dataExchangeApiFactory = dataExchangeApiFactory;
            _serviceEventLogger = serviceEventLogger;
        }

        public virtual void SubmitImport(DataExchangeImportMessage message, Action<bool> onCompletion)
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
                    onCompletion?.Invoke(messageIsHandled);
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
        }

        public bool changeMessageLogState(string externalReference, TransLogMessageStatus state)
        {
            var api = _dataExchangeApiFactory();
            return api.SetMessageLogStatus(externalReference,state);
        }

        protected void LogError(string error)
        {
            _serviceEventLogger.LogMessage(Settings.Constants.MessageIdentifiers.AnErrorOccurred, error);
        }

        protected void LogError(int msgKey, params string[] errorArgs)
        {
            _serviceEventLogger.LogMessage(msgKey, errorArgs);
        }
    }
}
