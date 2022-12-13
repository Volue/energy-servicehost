using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Builders.Abstract
{
    public interface IExportSettingsBuilder
    {
        Settings Build(DataExchangeExportMessage exportMessage);
    }
}