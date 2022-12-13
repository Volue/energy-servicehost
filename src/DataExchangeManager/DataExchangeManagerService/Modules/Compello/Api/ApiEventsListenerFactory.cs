using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api
{
    public class ApiEventsListenerFactory : IApiEventsListenerFactory
    {
        private readonly ISettingsProvider _settingsProvider;

        public ApiEventsListenerFactory(ISettingsProvider settingsProvider)
        {
            _settingsProvider = settingsProvider;
        }

        public IApiEventsListener Create()
        {
            return Create(_settingsProvider.GetSettings());
        }

        public IApiEventsListener Create(Settings settings)
        {
            return new ApiEventsListener(settings);
        }
    }
}