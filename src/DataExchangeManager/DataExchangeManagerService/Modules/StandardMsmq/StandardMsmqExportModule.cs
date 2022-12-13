using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardMsmq
{
    /// <summary>
    /// This class is used to exportMessage to other Powel systems. 
    /// DataExchangeManager takes an exportMessage from the exportMessage queue and then uses this module to put it into an import queue. 
    /// </summary>
    public class StandardMsmqExportModule : PushExportModule
    {
        public const string MODULE_NAME = "STANDARDMSMQ";

        private readonly IDataExchangeQueueFactory _dataExchangeQueueFactory;

        public StandardMsmqExportModule(
            IDataExchangeQueueFactory dataExchangeQueueFactory, 
            IDataExchangeManagerServiceSettingsFactory dataExchangeManagerServiceSettingsFactory,
            MessageExporter messageExporter, 
            IServiceEventLogger serviceEventLogger) 
            : base(dataExchangeManagerServiceSettingsFactory, messageExporter, serviceEventLogger)
        {
            _dataExchangeQueueFactory = dataExchangeQueueFactory;
        }

        public override string ModuleName
        {
            get { return MODULE_NAME; }
        }

        protected sealed override string SendExportMessage(DataExchangeExportMessage exportMessage)
        {
            // Get the machine name from the routing address (skip the export queue machine name since we're not using it)
            var machineName = RoutingAddressParser.ParseMachineNameFromRoutingAddress(exportMessage.RoutingAddress);
            var importMessage = new DataExchangeImportMessage(exportMessage);

            // Get a queue which has name outside of the internal queue system
            using (var transaction = _dataExchangeQueueFactory.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
            {
                using(var importQueue = _dataExchangeQueueFactory.GetCustomImportQueue(machineName, "POWEL"))
                {
                    transaction.Begin();
                    importQueue.Enqueue(importMessage, DataExchangeQueuePriorityConverter.FromString(importMessage.Priority), transaction);
                    transaction.Commit();
                }
            }

            return importMessage.ExternalReference;
        }
    }
}
