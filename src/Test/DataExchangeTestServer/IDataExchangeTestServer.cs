using System.ServiceModel;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeTestServer
{
    [ServiceContract]
    public interface IDataExchangeTestServer
    {
        [OperationContract]
        string Echo(string value);

        [OperationContract]
        bool Consume(string value);
    }
}
