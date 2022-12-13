using System;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Process;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerService // if you change this namespace you break backward compatibility with xml config files
{
    public class ImportApplicationManagerService : ServiceIterationBase
    {
        private static int AnErrorOccured = 8000;
        private readonly IDataExchangeApi _dataExchangeApi;
        private readonly IImportApplication _importApplication;

        private static readonly TimeSpan ReadQueueTimeout = TimeSpan.FromSeconds(1);

        public ImportApplicationManagerService(IServiceEventLogger serviceEventLogger, IDataExchangeApi dataExchangeApi, IImportApplication importApplication) : base(serviceEventLogger)
        {
            _importApplication = importApplication;
            _dataExchangeApi = dataExchangeApi;
        }

        public override string ServiceIterationName { get; } = "Import Application Manager Service";

        public override IccModule ServiceIterationModule { get; } = IccModule.M_IMPORT_APPLICATION_MANAGER;

        public override void Initialize()
        {
        }

        public override void RunIteration(out bool possiblyMoreWork)
        {
            try
            {
                do
                {
                    possiblyMoreWork = ProcessNextImportMessage(_dataExchangeApi);
                } while (possiblyMoreWork && !StopRequested());

            }
            catch (Exception exception)
            {
                CriticalLog(exception); // _importApplication.LogException(exception);
                throw;
            }
        }

        public bool ProcessNextImportMessage(IDataExchangeApi dataExchangeApi)
        {
            bool possiblyMoreWork = false;
            IDataExchangeQueueTransaction transaction = null;
            using (transaction = dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.Dequeue))
            {
                transaction.Begin();
                DataExchangeImportMessage message = null;
                bool messageIsHandled = _importApplication.HandleMessageWithExceptionHandling(
                    () => {
                        message = dataExchangeApi.DequeueImportMessage(ReadQueueTimeout, transaction);
                        possiblyMoreWork = message != null;
                        var ret = _importApplication.HandleMessage(message);
                        message?.DeleteMessageData();
                        return ret;
                    },
                    onImportRunnerFailed: (exc) =>
                    {
                        // The message will not be put on error import queue. Further handling happens from messageLogSearch.
                        message?.DeleteMessageData();
                    },
                    onTimeout: (exc) =>
                    {
                        // The message will not be put on error import queue. Further handling happens from messageLogSearch.
                        message?.DeleteMessageData();
                    },
                    onErrorCaught: () =>
                    {
                        try
                        {
                            dataExchangeApi.EnqueueErroneousImportMessage(message, transaction);
                        }
                        catch (ArgumentNullException e)
                        {
                            ServiceEventLogger.LogMessage(AnErrorOccured, "EnqueueErroneousImportMessage:" + e.Message);
                        }
                    });

                if (messageIsHandled)
                {
                    transaction.Commit();
                }
                else
                {
                    transaction.Rollback();
                }
            }

            return possiblyMoreWork;
        }
    }
}
