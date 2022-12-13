using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract
{
    public interface IImportMessageTranslator
    {
        DataExchangeImportMessage Translate(ImportMessage importMessage, bool areMetaDataMandatory = true);
    }
}