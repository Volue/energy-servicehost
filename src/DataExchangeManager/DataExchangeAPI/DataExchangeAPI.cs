using System;
using System.Collections.Generic;
using System.Data;
using Powel.Icc.Common;
using log4net;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq;
using Powel.Icc.Services.DataContracts.EventMonitor;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public class DataExchangeAPI : IDataExchangeApi
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private readonly IDataExchangeSettingsFactory _dataExchangeSettingsFactory;
        private readonly IDataExchangeQueueFactory _dataExchangeQueueFactory;
        private readonly IDataExchangeMessageLog _dataExchangeMessageLog;
        private readonly IDataExchangeFileWriter _dataExchangeFileWriter;
        private readonly IDataExchangeMetaData _dataExchangeMetaData;

        public DataExchangeAPI(IDataExchangeSettingsFactory dataExchangeSettingsFactory, IDataExchangeMessageLog dataExchangeMessageLog, IDataExchangeFileWriter dataExchangeFileWriter, IDataExchangeMetaData dataExchangeMetaData, IServiceEventLogger serviceEventLogger)
        {
            _dataExchangeSettingsFactory = dataExchangeSettingsFactory;
            _dataExchangeQueueFactory = new MsmqDataExchangeQueueFactory(_dataExchangeSettingsFactory, serviceEventLogger);
            _dataExchangeMessageLog = dataExchangeMessageLog;
            _dataExchangeFileWriter = dataExchangeFileWriter;
            _dataExchangeMetaData = dataExchangeMetaData;
        }

        public DataExchangeSettings GetDataExchangeSettings()
        {
            return _dataExchangeSettingsFactory.GetSettings();
        }

        public void IncrementExportMachineIndex()
        {
            _dataExchangeSettingsFactory.ExportIndex++;
        }

        public void IncrementImportMachineIndex()
        {
            _dataExchangeSettingsFactory.ImportIndex++;
        }

        public static DataExchangeQueueTransactionType GetTransactionType(string machineName)
        {
            return MachineNameTypeUtility.DetermineMachineNameType(machineName) == MachineNameType.Local
                ? DataExchangeQueueTransactionType.EnqueueAndDequeue
                : DataExchangeQueueTransactionType.EnqueueAndRemoteDequeue;
        }

        public IDataExchangeQueueTransaction GetTransaction(string machineName)
        {
            return GetTransaction(GetTransactionType(machineName));
        }

        public IDataExchangeQueueTransaction GetTransaction(DataExchangeQueueTransactionType type)
        {
            return _dataExchangeQueueFactory.GetTransaction(type);
        }

        private static string GetModuleNameFromMessage(DataExchangeExportMessage message)
        {
            return GetModuleFromRoutingAddress(message.RoutingAddress);
        }

        private static string GetModuleFromRoutingAddress(string routingAddress)
        {
            string module = null;
            if (!string.IsNullOrEmpty(routingAddress))
            {
                int indexOfDelimiter = routingAddress.IndexOf(':');
                if (indexOfDelimiter > 0)
                {
                    module = routingAddress.Substring(0, indexOfDelimiter);
                }
                else
                {
                    module = routingAddress;
                }
            }
            return module;
        }

        private static DataExchangeQueuePriority GetPriorityFromMessage(DataExchangeMessageBase message, bool useDefaultIfUndefined)
        {
            DataExchangeQueuePriority priority;

            if(string.IsNullOrEmpty(message.Priority) && useDefaultIfUndefined)
            {
                Log.Warn("Import message has not priority set, will default to 'normal'");
                priority = DataExchangeQueuePriority.Normal;
            }
            else
            {
                priority = DataExchangeQueuePriorityConverter.FromString(message.Priority);

                if(priority == DataExchangeQueuePriority.Undefined)
                {
                    throw new DataExchangeInvalidMetadataException(0,string.Format("The value for the priority, '{0}', is invalid, expected it to be any of the following value: 'high', 'normal' or 'low'. The value is case insensitive.", message.Priority),"priority");
                }
            }

            return priority;
        }

        public void EnqueueExportMessage(DataExchangeExportMessage message, IDataExchangeQueueTransaction transaction)
        {
            if(message == null)
            {
                throw new ArgumentNullException("message");
            }
            if(transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }

            if(string.IsNullOrEmpty(message.RoutingAddress))
            {
                message.RoutingAddress = GetDefaultRoutingAddress();
            }

            if(string.IsNullOrEmpty(message.Priority))
            {
                message.Priority = GetDefaultPriority();
            }

            if(string.IsNullOrEmpty(message.ExternalReference))
            {
                // Generate the reference that will be used in communication with remote systems (EDI-server)
                message.ExternalReference = Guid.NewGuid().ToString();
                if (message.MessageLogId > 0)
                {
                    _dataExchangeMessageLog.SetExternalReference(message.MessageLogId, message.ExternalReference);
                }
                
            }

            if(string.IsNullOrEmpty(message.Protocol) && message.ProtocolId > 0)
            {
                // Get the protocol type (name) from the protocol id (key)
                message.Protocol = _dataExchangeMetaData.GetProtocolName(message.ProtocolId);
            }

            if(string.IsNullOrEmpty(message.SenderId) && message.SenderKey > 0)
            {
                // Get SenderId and SenderName from SenderKey
                //TODO: Other structure than DataTable
                DataTable senderDT = _dataExchangeMetaData.GetSenderInfo(message.SenderKey);
                if (senderDT.Rows.Count > 0)
                {
                    message.SenderId = senderDT.Rows[0]["BANKACC"].ToString();
                    message.SenderName = senderDT.Rows[0]["NAME"].ToString();
                }
            }

            if(string.IsNullOrEmpty(message.ReceiverId) && message.ReceiverKey > 0)
            {
                // Get ReceiverId, ReceiverName and Country from ReceiverKey
                //TODO: Other structure than DataTable
                DataTable receiverDT = _dataExchangeMetaData.GetReceiverInfo(message.ReceiverKey);
                if (receiverDT.Rows.Count > 0)
                {
                    message.ReceiverId = receiverDT.Rows[0]["BANKACC"].ToString();
                    message.ReceiverName = receiverDT.Rows[0]["NAME"].ToString();
                    message.Country = receiverDT.Rows[0]["COUNTRY_CODE"].ToString();
                }
            }

            string moduleName = GetModuleNameFromMessage(message);
            DataExchangeMessageBase messageBase = (DataExchangeMessageBase) message;
            DataExchangeQueuePriority priority = GetPriorityFromMessage(messageBase, true);

            using (var queue = _dataExchangeQueueFactory.GetInternalExportQueue(moduleName))
            {
                queue.Enqueue(message, priority, transaction);
            }

            if (_dataExchangeSettingsFactory.GetSettings().StoreExportMessages)
            {
                _dataExchangeFileWriter.LogExportMessageToFile(message);
            }

            if (message.MessageLogId > 0)
            {
                _dataExchangeMessageLog.SetStatusToExportEnqueued(message.MessageLogId);
            }
        }

        public void EnqueueImportMessage(DataExchangeImportMessage message, IDataExchangeQueueTransaction transaction)
        {
            if(message == null)
            {
                throw new ArgumentNullException("message");
            }
            if(transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }
            
            if(string.IsNullOrEmpty(message.RoutingAddress))
            {
                message.RoutingAddress = GetDefaultRoutingAddress();
                Log.Warn($"RoutingAddress not given. Set to default: {message.RoutingAddress}");
            }

            if (string.IsNullOrEmpty(message.Priority))
            {
                message.Priority = GetDefaultPriority();
                Log.Warn($"Priority not given. Set to default: {message.Priority}");
            }
            
            if(string.IsNullOrEmpty(message.Protocol))
            {
                throw new DataExchangeInvalidMetadataException("Protocol is not specified in the message.");
            }

            DataExchangeMessageBase messageBase = (DataExchangeMessageBase) message;
            DataExchangeQueuePriority priority = GetPriorityFromMessage(messageBase, true);

            message.EnqueuedTimeUtc = new DateTime(UtcTime.Now.Ticks);

            using(var queue = _dataExchangeQueueFactory.GetInternalImportQueue())
            {
                queue.Enqueue(message, priority, transaction);
            }
        }

        public void EnqueueErroneousImportMessage(DataExchangeImportMessage message, IDataExchangeQueueTransaction transaction)
        {
            if (message == null)
            {
                throw new ArgumentNullException("message");
            }
            if(transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }

            using(var queue = _dataExchangeQueueFactory.GetInternalImportErrorQueue())
            {
                queue.Enqueue(message, transaction);
            }
        }

        public DataExchangeExportMessage DequeueExportMessage(TimeSpan queueReadTimeout, IDataExchangeQueueTransaction transaction, string moduleName)
        {
            if (transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }
            if (moduleName == null)
            {
                throw new ArgumentNullException("moduleName");
            }

            DataExchangeExportMessage message;

            using(var queue = _dataExchangeQueueFactory.GetInternalExportQueue(moduleName))
            {
                message = queue.Dequeue(queueReadTimeout, transaction);
            }

            return message;
        }

        public DataExchangeImportMessage DequeueImportMessage(TimeSpan queueReadTimeout, IDataExchangeQueueTransaction transaction)
        {
            if (transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }

            DataExchangeImportMessage message;

            using(var queue = _dataExchangeQueueFactory.GetInternalImportQueue())
            {
                message = queue.Dequeue(queueReadTimeout, transaction);
            }

            return message;
        }

        public DataExchangeImportMessage DequeueErroneousImportMessage(string internalQueueId, IDataExchangeQueueTransaction transaction)
        {
            if (transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }

            DataExchangeImportMessage message;

            using (var queue = _dataExchangeQueueFactory.GetInternalImportErrorQueue())
            {
                message = queue.Dequeue(internalQueueId, transaction);
            }

            return message;
        }

        public IList<DataExchangeExportMessage> DequeueExportMessages(int maxNumberOfMessages, IDataExchangeQueueTransaction transaction, string moduleName)
        {
            if (maxNumberOfMessages < 1)
            {
                throw new ArgumentException("maxNumberOfMessages must be greater than zero");
            }
            if (transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }
            if (moduleName == null)
            {
                throw new ArgumentNullException("moduleName");
            }

            IList<DataExchangeExportMessage> messages;

            using(var queue = _dataExchangeQueueFactory.GetInternalExportQueue(moduleName))
            {
                messages = queue.Dequeue(maxNumberOfMessages, transaction);
            }

            return messages;
        }

        public DataExchangeExportMessage PeekExportMessage(out string internalMessageId, TimeSpan queueReadTimeout, string moduleName)
        {
            if (moduleName == null)
            {
                throw new ArgumentNullException("moduleName");
            }

            IDataExchangeQueue<DataExchangeExportMessage> queue = _dataExchangeQueueFactory.GetInternalExportQueue(moduleName);

            DataExchangeExportMessage message = queue.Peek(queueReadTimeout, out internalMessageId);

            return message;
        }

        /// <summary>
        /// NOTE: This is a Peek, not Find, so we will only check top message of queues
        /// </summary>
        /// <param name="internalMessageId"></param>
        /// <param name="externalReference"></param>
        /// <param name="moduleName"></param>
        /// <returns></returns>
        public DataExchangeExportMessage PeekExportMessageByExternalReference(out string internalMessageId, string externalReference, string moduleName)
        {
            if (moduleName == null)
            {
                throw new ArgumentNullException("moduleName");
            }

            DataExchangeExportMessage message = null;
            internalMessageId = null;

            IDataExchangeQueue<DataExchangeExportMessage> queue = _dataExchangeQueueFactory.GetInternalExportQueue(moduleName);

            List <DataExchangeQueuePriority> priorities = queue.GetPriorities();
            foreach (DataExchangeQueuePriority priority in priorities)
            {
                message = queue.PeekForPriority(priority, out internalMessageId);
                if(message != null && externalReference == message.ExternalReference)
                {
                    break;
                }
            }

            return message;
        }

        public void RemoveExportMessage(string internalMessageId, IDataExchangeQueueTransaction transaction, string moduleName)
        {
            if (internalMessageId == null)
            {
                throw new ArgumentNullException("internalMessageId");
            }
            if (transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }
            if (moduleName == null)
            {
                throw new ArgumentNullException("moduleName");
            }

            using(var queue = _dataExchangeQueueFactory.GetInternalExportQueue(moduleName))
            {
                queue.Remove(internalMessageId, transaction);
            }
        }

        public void RemoveExportMessages(IList<string> internalQueueIds, IDataExchangeQueueTransaction transaction, string moduleName)
        {
            if (internalQueueIds == null)
            {
                throw new ArgumentNullException("internalQueueIds");
            }
            if (transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }
            if (moduleName == null)
            {
                throw new ArgumentNullException("moduleName");
            }

            using(var queue = _dataExchangeQueueFactory.GetInternalExportQueue(moduleName))
            {
                queue.Remove(internalQueueIds, transaction);
            }
        }

        public int GetExportMessagesCount(string moduleName)
        {
            if (moduleName == null)
            {
                throw new ArgumentNullException("moduleName");
            }

            using(var queue = _dataExchangeQueueFactory.GetInternalExportQueue(moduleName))
            {
                return queue.GetQueueSize();
            }
        }

        public int GetImportMessagesCount()
        {
            return _dataExchangeQueueFactory.GetInternalImportQueue().GetQueueSize();
        }

        public int GetErroneousImportMessagesCount()
        {
            return _dataExchangeQueueFactory.GetInternalImportErrorQueue().GetQueueSize();
        }

        public IDictionary<string, DataExchangeImportMessage> GetAllErroneousImportMessages(IDataExchangeQueueTransaction transaction)
        {
            IDictionary<string, DataExchangeImportMessage> messages = _dataExchangeQueueFactory.GetInternalImportErrorQueue().GetAll(transaction);

            return messages;
        }

        public void RemoveErroneousImportMessages(IList<string> internalQueueIds, IDataExchangeQueueTransaction transaction)
        {
            if (internalQueueIds == null)
            {
                throw new ArgumentNullException("internalQueueIds");
            }
            if (transaction == null)
            {
                throw new ArgumentNullException("transaction");
            }

            using (var queue = _dataExchangeQueueFactory.GetInternalImportErrorQueue())
            {
                queue.Remove(internalQueueIds, transaction);
            }
        }

        private static string GetDefaultRoutingAddress()
        {
            var defaultRoutingAddress = Environment.GetEnvironmentVariable("ICC_DEFAULT_ROUTING_ADDRESS");
                
            if(string.IsNullOrEmpty(defaultRoutingAddress))
            {
                throw new DataExchangeInvalidMetadataException("No routing address was specified for the message, and no default routing address has been configured in the ICC_DEFAULT_ROUTING_ADDRESS parameter.");
            }

            return defaultRoutingAddress;
        }

        private static string GetDefaultPriority()
        {
            return "Normal";
        }

        public bool SetMessageLogStatus(string externalReference, TransLogMessageStatus state)
        {
            return _dataExchangeMessageLog.SetStatus(externalReference,state);
        }
    }
}

