using System.Data;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public interface IDataExchangeMetaData
    {
        string GetProtocolName(int protocolId);
        DataTable GetSenderInfo(int opunKey);  //TODO: Other structure than DataTable
        DataTable GetReceiverInfo(int opunKey);  //TODO: Other structure than DataTable
    }
}
