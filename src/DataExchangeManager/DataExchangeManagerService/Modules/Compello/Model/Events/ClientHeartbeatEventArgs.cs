using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events
{
    public class ClientHeartbeatEventArgs : EventArgs
    {
        public readonly int NodeCount;        

        public ClientHeartbeatEventArgs(int nodeCount)
        {
            NodeCount = nodeCount;         
        }
    }
}
