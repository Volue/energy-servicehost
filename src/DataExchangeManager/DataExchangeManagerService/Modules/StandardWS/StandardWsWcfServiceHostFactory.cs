using System;
using System.ServiceModel;
using System.ServiceModel.Activation;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS
{
    public class StandardWsWcfServiceHostFactory : ServiceHostFactory, IStandardWsWcfServiceHostFactory
    {
        private readonly StandardWsLogic _logic;

        public StandardWsWcfServiceHostFactory(StandardWsLogic logic)
        {
            _logic = logic;
        }

        protected override ServiceHost CreateServiceHost(Type serviceType, Uri[] baseAddresses)
        {
            return new StandardWsWcfServiceHost(_logic, serviceType, baseAddresses);
        }

        public override ServiceHostBase CreateServiceHost(string constructorString, Uri[] baseAddresses)
        {
            return CreateServiceHost(typeof(StandardWsWcfService), baseAddresses);
        }
    }
}
