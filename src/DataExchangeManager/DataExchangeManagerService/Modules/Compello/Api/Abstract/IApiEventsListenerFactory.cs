using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract
{
    public interface IApiEventsListenerFactory : IFactory<IApiEventsListener>
    {
        IApiEventsListener Create(Settings settings);
    }
}