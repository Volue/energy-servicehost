using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Events.Extensions
{
    public static class EventArgsExtensions
    {
        public static void RaiseSafeAsync<TEventArgs>(this TEventArgs e, Object sender, ref EventHandler<TEventArgs> eventDelegate)
            where  TEventArgs : EventArgs
        {
            EventHandler<TEventArgs> threadSafeCopy = eventDelegate; // avoid the race condition -> create a copy
            // for more info go to the "CLR via C#" book, Chapter 11. Events, Designing a Type That Exposes an Event -> RAISING AN EVENT IN A THREAD-SAFE WAY

            if(threadSafeCopy != null)
            {
                foreach (EventHandler<TEventArgs> action in threadSafeCopy.GetInvocationList())
                {
                    action.BeginInvoke(sender, e, CompleteHandler<TEventArgs>, action); // invoke handler in async way, so the underlying class in not blocked until task is complete
                }
            }
        }

        private  static void CompleteHandler<TEventArgs>(IAsyncResult result)
            where  TEventArgs : EventArgs
        {
            ((EventHandler<TEventArgs>)result.AsyncState).EndInvoke(result); // call to EndInvoke is necessary
        }
    }
}