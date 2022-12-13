using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
{
    public interface IEventUnsubscribable : IDisposable
    {
        void UnsubscribeFromAllEvents();
    }
}