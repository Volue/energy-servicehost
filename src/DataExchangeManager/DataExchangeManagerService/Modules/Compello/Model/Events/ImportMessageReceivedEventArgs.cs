using System;
using System.Collections.Generic;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events
{
    public class ImportMessageReceivedEventArgs : EventArgs
    {
        public readonly long MessageId;
        public readonly Dictionary<string, object> Metadata;

        public ImportMessageReceivedEventArgs(long messageId, Dictionary<string, object> metadata)
        {
            MessageId = messageId;
            Metadata = metadata;
        }
    }
}
