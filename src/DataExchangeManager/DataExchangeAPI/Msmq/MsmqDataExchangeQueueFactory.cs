using System;
using System.Collections.Generic;
using System.Linq;
using Powel.Icc.Diagnostics;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq
{
    public class MsmqDataExchangeQueueFactory : IDataExchangeQueueFactory
    {
        private readonly IServiceEventLogger _serviceEventLogger;
        private readonly MsmqPathFactory _pathFactory;
        private readonly IDataExchangeSettingsFactory _settingsFactory;

        private static readonly List<DataExchangeQueuePriority> OrderedPriorities = new List<DataExchangeQueuePriority>
            {
                DataExchangeQueuePriority.High,
                DataExchangeQueuePriority.Normal,
                DataExchangeQueuePriority.Low
            };

        private static readonly Dictionary<DataExchangeQueuePriority, string> PrioritySuffixes = new Dictionary<DataExchangeQueuePriority, string>
                {
                    { DataExchangeQueuePriority.High, "_HIGH" },
                    { DataExchangeQueuePriority.Normal, "_NORMAL" },
                    { DataExchangeQueuePriority.Low, "_LOW" }
                };

        public MsmqDataExchangeQueueFactory(IDataExchangeSettingsFactory settingsFactory, IServiceEventLogger serviceEventLogger)
        {
            _serviceEventLogger = serviceEventLogger;
            _settingsFactory = settingsFactory;
            _pathFactory = new MsmqPathFactory(_settingsFactory);
        }

        /// <summary>
        ///  Gets one of the internal export queues.
        /// </summary>
        /// <param name="moduleName"> The name which identifies the queue, e.g. "STANDARDMSMQ", "COMPELLO".</param>
        /// <returns>A queue object linked to the specified queue identifier.</returns>
        public IDataExchangeQueue<DataExchangeExportMessage> GetInternalExportQueue(string moduleName)
        {
            MsmqPath msmqBasePath = _pathFactory.CreateInternalExportQueuePath(moduleName);
            var msmqPaths = GetMessageQueuePaths(msmqBasePath.FullPath);
            return new MsmqDataExchangeQueue<DataExchangeExportMessage>(OrderedPriorities, msmqPaths, _serviceEventLogger);
        }

        /// <summary>
        /// Get a queue which has no connection the internal queues (using the prefix ICC_ ..)
        /// </summary>
        /// <returns>Returns the internal import queue (ICC_IMPORT).</returns>
        public IDataExchangeQueue<DataExchangeImportMessage> GetInternalImportQueue()
        {
            MsmqPath msmqBasePath = _pathFactory.CreateInternalImportQueuePath();
            var msmqPaths = GetMessageQueuePaths(msmqBasePath.FullPath);
            return new MsmqDataExchangeQueue<DataExchangeImportMessage>(OrderedPriorities, msmqPaths, _serviceEventLogger);
        }

        public IDataExchangeQueue<DataExchangeImportMessage> GetInternalImportErrorQueue()
        {
            MsmqPath msmqBasePath = _pathFactory.CreateInternalImportQueuePath();
            msmqBasePath.FullPath = msmqBasePath.FullPath + "_ERROR";
            var msmqPaths = new Dictionary<DataExchangeQueuePriority, MsmqPath>
                {
                    {DataExchangeQueuePriority.Undefined, msmqBasePath}
                };
            return new MsmqDataExchangeQueue<DataExchangeImportMessage>(new List<DataExchangeQueuePriority> { DataExchangeQueuePriority.Undefined }, msmqPaths, _serviceEventLogger);
        }

        /// <summary>
        /// Get a queue which has no connection the internal queues (using the prefix ICC_ ..)
        /// </summary>
        /// <param name="machineName">The name for the machine where the queue can be found, set to empty, null or "." for local machine.</param>
        /// <param name="queueIdentifier">The name which identifies the queue, e.g. "POWEL"</param>
        /// <returns>A custom queue.</returns>
        public IDataExchangeQueue<DataExchangeImportMessage> GetCustomImportQueue(string machineName, string queueIdentifier)
        {
            MsmqPath msmqBasePath = _pathFactory.CreateCustomImportQueuePath(machineName, queueIdentifier);
            var msmqPaths = GetMessageQueuePaths(msmqBasePath.FullPath);
            return new MsmqDataExchangeQueue<DataExchangeImportMessage>(OrderedPriorities, msmqPaths, _serviceEventLogger);
        }

        /// <summary>
        /// Gets a transaction for use with any data exchange queue created with this factory.
        /// </summary>
        /// <param name="type">
        /// If Enqueue: The transaction is always of type MessageQueueTransaction.
        /// If Dequeue: The transaction is of type MessageQueueTransaction if
        ///     ICC_MSMQ_MACHINE_EXPORT or ICC_MSMQ_MACHINE_IMPORT is
        ///     pointing to a local machine, otherwise it is of type MSDTC.
        /// If EnqueueAndDequeue: The transaction is of type MessageQueueTransaction if
        ///     ICC_MSMQ_MACHINE_EXPORT or ICC_MSMQ_MACHINE_IMPORT is
        ///     pointing to a local machine, otherwise it is of type MSDTC.
        /// If EnqueueAndRemoteDequeue: The transaction is always of type MSDTC.
        /// </param>
        /// <returns>A transaction.</returns>
        public IDataExchangeQueueTransaction GetTransaction(DataExchangeQueueTransactionType type)
        {
            IDataExchangeQueueTransaction transaction;

            switch (type)
            {
                case DataExchangeQueueTransactionType.Enqueue:
                    transaction = new MsmqDataExchangeQueueTransaction();
                    break;
                case DataExchangeQueueTransactionType.Dequeue:
                case DataExchangeQueueTransactionType.EnqueueAndDequeue:
                    if(QueuesAreOnRemoteMachine())
                    {
                        transaction = new MsdtcDataExchangeTransaction();
                    }
                    else
                    {
                        transaction = new MsmqDataExchangeQueueTransaction();
                    }
                    break;
                case DataExchangeQueueTransactionType.EnqueueAndRemoteDequeue:
                    transaction = new MsdtcDataExchangeTransaction();
                    break;
                default:
                    throw new ArgumentException("Unsupported transaction type.", "type");
            }

            return transaction;
        }

        private bool QueuesAreOnRemoteMachine()
        {
            return MachineNameTypeUtility.DetermineMachineNameType(_settingsFactory.GetSettings().ExportQueueMachineName) != MachineNameType.Local ||
                   MachineNameTypeUtility.DetermineMachineNameType(_settingsFactory.GetSettings().ImportQueueMachineName) != MachineNameType.Local;
        }

        private Dictionary<DataExchangeQueuePriority, MsmqPath> GetMessageQueuePaths(string basePath)
        {
            return OrderedPriorities.ToDictionary(p => p, p => new MsmqPath {FullPath = basePath + PrioritySuffixes[p]});
        }
    }
}
