using System;
using System.Linq;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Events
{
    public class EventHandlerManager<TSourceEventArgs, TTargetEventArgs> : IDisposable
        where TSourceEventArgs: EventArgs
        where  TTargetEventArgs : EventArgs
    {
        private readonly EventHandler<TSourceEventArgs> _onSourceEvent;
        private readonly Action<EventHandler<TSourceEventArgs>> _subscribeToSourceEvent;
        private readonly Action<EventHandler<TSourceEventArgs>> _unsubscribeFromSourceEvent;

        private event EventHandler<TTargetEventArgs> _handler;

        private volatile bool _isEventSubscribed;

        private bool _isDisposed;

        public EventHandlerManager(Action<EventHandler<TSourceEventArgs>> subscribeToSourceEvent, Action<EventHandler<TSourceEventArgs>> unsubscribeFromSourceEvent,
            Func<TSourceEventArgs, TTargetEventArgs> map)
        {
            _subscribeToSourceEvent = subscribeToSourceEvent;
            _unsubscribeFromSourceEvent = unsubscribeFromSourceEvent;

            _onSourceEvent = (sender, args) =>
            {
                TTargetEventArgs mapped = map(args);

                RaiseEvent(mapped, ref _handler);
            };
        }

        public void Dispose()
        {
            if(_isDisposed)
            {
                return;
            }

            try
            {
                if (_handler != null)
                {
                    foreach (EventHandler<TTargetEventArgs> subscribed in _handler.GetInvocationList())
                    {
                        Handler -= subscribed;
                    }
                }
            }
            finally
            {
                _isDisposed = true;
            }
        }

        public event EventHandler<TTargetEventArgs> Handler
        {
            add
            {
                ThrowIfDisposed();

                _handler += value; // subscribe to our event first!

                EnsureSourceEventIsSubscribed(); // now we can subscribe to source event, so when compello event is raised we know for sure that we can always handle it!
            }
            remove
            {
                ThrowIfDisposed();

                UnsubscribeFromSourceIfNeeded(value); // we need to unsubscribe from source event first so we are sure that we can always handle it!

                _handler -= value;
            }
        }

        protected virtual void RaiseEvent(TTargetEventArgs mapped, ref EventHandler<TTargetEventArgs> handler)
        {
            if (_handler != null)
            {
                _handler.Invoke(this, mapped);
            }
        }

        private void EnsureSourceEventIsSubscribed()
        {
            if(_isEventSubscribed)
            {
                return;
            }

            _subscribeToSourceEvent(_onSourceEvent);
            _isEventSubscribed = true;
        }

        private void UnsubscribeFromSourceIfNeeded(EventHandler<TTargetEventArgs> value)
        {
            if(_handler == null)
            {
                return;
            }

            if(_handler.GetInvocationList().Length != 1)
            {
                return;
            }

            if(!_handler.GetInvocationList()
                    .Any(handler => handler.Method == value.Method && handler.Target == value.Target))
            {
                return;
            }

            _unsubscribeFromSourceEvent(_onSourceEvent);
            _isEventSubscribed = false;
        }

        private void ThrowIfDisposed()
        {
            if(_isDisposed)
            {
                throw new ObjectDisposedException("event handler manager");
            }
        }
    }
}