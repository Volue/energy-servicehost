using System;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Events.Extensions;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Events
{
    public class AsyncEventHandlerManager<TSourceEventArgs, TTargetEventArgs> : EventHandlerManager<TSourceEventArgs, TTargetEventArgs>
        where TSourceEventArgs : EventArgs
        where TTargetEventArgs : EventArgs
    {
        public AsyncEventHandlerManager(Action<EventHandler<TSourceEventArgs>> subscribeToSourceEvent,
                                            Action<EventHandler<TSourceEventArgs>> unsubscribeFromSourceEvent,
                                            Func<TSourceEventArgs, TTargetEventArgs> map)
            : base(subscribeToSourceEvent, unsubscribeFromSourceEvent, map)
        {
        }

        protected override void RaiseEvent(TTargetEventArgs mapped, ref EventHandler<TTargetEventArgs> handler)
        {
            mapped.RaiseSafeAsync(this, ref handler);
        }
    }
}