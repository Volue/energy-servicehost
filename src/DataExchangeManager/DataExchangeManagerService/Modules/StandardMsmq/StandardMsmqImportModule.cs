using System;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardMsmq
{
    public class StandardMsmqImportModule : TaskBasedModule
    {
        public const string MODULE_NAME = "STANDARDMSMQIMPORT";

        private static readonly TimeSpan ReadQueueTimeout = TimeSpan.FromSeconds(1);

        private readonly IDataExchangeQueueFactory _dataExchangeQueueFactory;
        private readonly IDataExchangeApi _importApi;

        private readonly string _machineName;
        private readonly DataExchangeQueueTransactionType _dataExchangeQueueTransactionType;

        public StandardMsmqImportModule(
            IDataExchangeQueueFactory dataExchangeQueueFactory,
            Func<IDataExchangeApi> dataExchangeApiFactory,
            IServiceEventLogger serviceEventLogger)
            : base(serviceEventLogger)
        {
            _dataExchangeQueueFactory = dataExchangeQueueFactory;

            _importApi = dataExchangeApiFactory.Invoke();

            // Get the settings (we're not using the export queue, therefore we won't specify ExportQueueMachineName)
            _machineName = _importApi.GetDataExchangeSettings().ImportQueueMachineName;
            _dataExchangeQueueTransactionType = GetTransactionType(_machineName);
        }

        public override string ModuleName
        {
            get { return MODULE_NAME; }
        }

        protected override TimeSpan SleepTime
        {
            get { return ReadQueueTimeout; }
        }

        protected sealed override bool TryExecuteSingleTask()
        {
            using (var transaction = _dataExchangeQueueFactory.GetTransaction(_dataExchangeQueueTransactionType))
            using (var importQueue = _dataExchangeQueueFactory.GetCustomImportQueue(_machineName, "POWEL")) // Get a queue which is outside of the API.
            {
                transaction.Begin();

                // First off, look for imports in the external import queue
                var message = importQueue.Dequeue(ReadQueueTimeout, transaction);
                if (message == null)
                {
                    transaction.Rollback();
                    return false;
                }

                try
                {
                    // Send the import to ImportApplicationManagerService. 
                    _importApi.EnqueueImportMessage(message, transaction);

                    transaction.Commit();
                }
                catch (DataExchangeInvalidMessageException exception)
                {
                    // from Birger:
                    //  "If an InvalidMessage/MetaData is received, the message should be moved to importError 
                    //  where the user can fix or delete the message."
                    _importApi.EnqueueErroneousImportMessage(message, transaction);
                    // Invalid messages are discarded directly with a log message
                    transaction.Commit();

                    ServiceEventLogger.LogCritical(exception, false);
                    ServiceEventLogger.LogMessage(8209);
                }
                catch (Exception anyError)
                {
                    // from Birger:
                    // "The StandardMSMQ plugin should only receive information from another Powel installation and therefore 
                    // the message should be removed on any error."
                    transaction.Commit();

                    ServiceEventLogger.LogCritical(anyError, false);
                }
            }
            return false;
        }

        private static DataExchangeQueueTransactionType GetTransactionType(string machineName)
        {
            return MachineNameTypeUtility.DetermineMachineNameType(machineName) == MachineNameType.Local
                ? DataExchangeQueueTransactionType.EnqueueAndDequeue
                : DataExchangeQueueTransactionType.EnqueueAndRemoteDequeue;
        }
    }
}
