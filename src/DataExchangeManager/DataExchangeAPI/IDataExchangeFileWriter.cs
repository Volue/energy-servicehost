namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public interface IDataExchangeFileWriter
    {
        void LogExportMessageToFile(DataExchangeExportMessage message);
    }
}
