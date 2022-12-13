using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract
{
    public interface IImportApplicationRunner
    {
        void Run(DataExchangeImportMessage message);
    }
}
