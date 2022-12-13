using System.Collections.Generic;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model
{
    public class ImportMessage
    {
        public readonly long MessageId;
        public readonly string Data;
        public readonly Dictionary<string, object> Metadata;

        public ImportMessage(long messageId, string data ,Dictionary<string, object> metadata)
        {
            MessageId = messageId;
            Data = data;
            Metadata = metadata;
        }
    }
}