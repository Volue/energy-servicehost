using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract
{
    public interface IApiWrapper
    {
        long SendMessage(Stream data, Dictionary<string, object> metaData);

        string GetTransactionData(long messageId,Encoding encoding);
    }
}