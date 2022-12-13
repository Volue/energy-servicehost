using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Validators.Abstract
{
    public interface IDataExchangeExportMessageValidator
    {
        void Validate(DataExchangeExportMessage exportMessage);
    }
}