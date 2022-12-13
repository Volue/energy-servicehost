using System;
using System.Collections.Generic;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public interface IDataExchangeApi
    {
        DataExchangeSettings GetDataExchangeSettings();
        void IncrementExportMachineIndex();
        void IncrementImportMachineIndex();
        IDataExchangeQueueTransaction GetTransaction(DataExchangeQueueTransactionType type);
        void EnqueueExportMessage(DataExchangeExportMessage message, IDataExchangeQueueTransaction transaction);
        void EnqueueImportMessage(DataExchangeImportMessage message, IDataExchangeQueueTransaction transaction);
        void EnqueueErroneousImportMessage(DataExchangeImportMessage message, IDataExchangeQueueTransaction transaction);
        DataExchangeExportMessage DequeueExportMessage(TimeSpan queueReadTimeout, IDataExchangeQueueTransaction transaction, string moduleName);
        DataExchangeImportMessage DequeueImportMessage(TimeSpan queueReadTimeout, IDataExchangeQueueTransaction transaction);
        IList<DataExchangeExportMessage> DequeueExportMessages(int maxNumberOfMessages, IDataExchangeQueueTransaction transaction, string moduleName);
        DataExchangeImportMessage DequeueErroneousImportMessage(string internalQueueId, IDataExchangeQueueTransaction transaction);

        DataExchangeExportMessage PeekExportMessage(out string internalMessageId, TimeSpan queueReadTimeout, string moduleName);
        DataExchangeExportMessage PeekExportMessageByExternalReference(out string internalId, string externalReference, string moduleName);
        void RemoveExportMessage(string internalMessageId, IDataExchangeQueueTransaction transaction, string moduleName);
        void RemoveExportMessages(IList<string> internalQueueIds, IDataExchangeQueueTransaction transaction, string moduleName);

        int GetExportMessagesCount(string moduleName);
        int GetImportMessagesCount();
        int GetErroneousImportMessagesCount();

        IDictionary<string, DataExchangeImportMessage> GetAllErroneousImportMessages(IDataExchangeQueueTransaction transaction);
        void RemoveErroneousImportMessages(IList<string> internalQueueIds, IDataExchangeQueueTransaction transaction);

        bool SetMessageLogStatus(string ExternalReference, TransLogMessageStatus state);
    }
}
