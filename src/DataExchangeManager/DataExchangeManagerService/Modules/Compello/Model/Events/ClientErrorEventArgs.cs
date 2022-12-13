using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events
{
    public class ClientErrorEventArgs : EventArgs
    {
        public readonly Exception Error;

        public ClientErrorEventArgs(Exception error)
        {
            Error = error;
        }
    }
}
