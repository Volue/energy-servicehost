namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public interface IDataExchangeQueueFactory
    {
        IDataExchangeQueue<DataExchangeExportMessage> GetInternalExportQueue(string moduleName);
        IDataExchangeQueue<DataExchangeImportMessage> GetInternalImportQueue();
        IDataExchangeQueue<DataExchangeImportMessage> GetInternalImportErrorQueue();
        IDataExchangeQueueTransaction GetTransaction(DataExchangeQueueTransactionType type);
        IDataExchangeQueue<DataExchangeImportMessage> GetCustomImportQueue(string machineName, string queueIdentifier);
    }
}
