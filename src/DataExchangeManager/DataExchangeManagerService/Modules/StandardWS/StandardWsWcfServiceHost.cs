using System;
using System.ServiceModel;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS
{
    public class StandardWsWcfServiceHost : ServiceHost
    {
        public StandardWsWcfServiceHost(StandardWsLogic logic, Type serviceType, params Uri[] baseAddresses) : base(serviceType, baseAddresses)
        {
            if (logic == null)
            {
                throw new ArgumentNullException("logic");
            }

            foreach (var cd in ImplementedContracts.Values)
            {
                cd.Behaviors.Add(new StandardWsWcfServiceInstanceProvider(logic));
            }
        }
    }
}
