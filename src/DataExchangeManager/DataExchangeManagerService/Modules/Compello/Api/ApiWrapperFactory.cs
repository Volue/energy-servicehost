using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api
{
    public class ApiWrapperFactory : IApiWrapperFactory
    {
        private readonly ISettingsProvider _settingsProvider;

        public ApiWrapperFactory(ISettingsProvider settingsProvider)
        {
            _settingsProvider = settingsProvider;
        }

        public IApiWrapper Create()
        {
            var settings = _settingsProvider.GetSettings();

            return Create(settings);
        }

        public IApiWrapper Create(Settings settings)
        {
            return new ApiWrapper(settings);
        }
    }
}