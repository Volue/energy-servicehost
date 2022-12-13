using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.MessageLog;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.FileWriter;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.MetaData;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public class DataExchangeApiModule : IUnityContainerModule
    {
        public void Register(IUnityContainer container)
        {
            container.RegisterType<IDataExchangeMessageLog, DataExchangeMessageLog>();
            container.RegisterType<IDataExchangeFileWriter, DataExchangeFileWriter>();
            container.RegisterType<IDataExchangeMetaData, DataExchangeMetaData>();
            container.RegisterType<IDataExchangeSettingsFactory, DataExchangeSettingsFactory>();
            container.RegisterType<IDataExchangeQueueFactory, MsmqDataExchangeQueueFactory>();
            container.RegisterType<IDataExchangeApi, DataExchangeAPI>();
            container.RegisterFactory<IDataExchangeApi>();
        }
    }
}
