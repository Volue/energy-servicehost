using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public class SettingsProvider : ISettingsProvider
    {
        public Settings GetSettings()
        {
            return new Settings
            (
               IccConfiguration.IccCompelloAddress,
               IccConfiguration.IccCompelloPort,
               IccConfiguration.IccCompelloApiKey,
               IccConfiguration.IccCompelloHeartbeatInterval,
               IccConfiguration.IccCompelloRestartInterval
            );
        }

        public string GetRoutingAddressForImport()
        {
            return string.Format("COMPELLO:{0}:{1}", IccConfiguration.IccCompelloAddress, IccConfiguration.IccCompelloPort);
        }
    }
}