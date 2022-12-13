using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract
{
    public interface ISettingsProvider
    {
        Settings GetSettings();
        string GetRoutingAddressForImport();
    }
}