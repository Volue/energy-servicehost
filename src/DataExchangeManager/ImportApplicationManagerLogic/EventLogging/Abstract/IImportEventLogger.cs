using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract
{
    public interface IImportEventLogger
    {
        void LogSuccessfulImport(DataExchangeImportMessage message);

        void LogFailedImport(DataExchangeImportMessage message);
    }
}
