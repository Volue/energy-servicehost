using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Builders.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Builders
{
    public class ExportSettingsBuilder : IExportSettingsBuilder
    {
        private readonly ISettingsProvider _settingsProvider;

        public ExportSettingsBuilder(ISettingsProvider settingsProvider)
        {
            _settingsProvider = settingsProvider;
        }

        public Settings Build(DataExchangeExportMessage exportMessage)
        {
            var settingsFromConfigSystem = _settingsProvider.GetSettings();

            var settings = new Settings(
                settingsFromConfigSystem.HostAddress,
                settingsFromConfigSystem.Port,
                settingsFromConfigSystem.ApiKey, 
                settingsFromConfigSystem.HeartbeatInterval, 
                settingsFromConfigSystem.RestartInterval);

            return settings;
        }
    }
}