using System;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeTestServer
{
    public class DataExchangeTestServerServiceHost : ServiceHost
    {
        public DataExchangeTestServerServiceHost(Type serviceType)
            : base(serviceType)
        {
            foreach (var cd in ImplementedContracts.Values)
            {
                cd.Behaviors.Add(new DataExchangeServerInstanceProvider(null, null));
            }
        }

        public DataExchangeTestServerServiceHost(DataExchangeTestServerArguments arguments, Type serviceType)
            : base(serviceType)
        {
            foreach (var cd in ImplementedContracts.Values)
            {
                cd.Behaviors.Add(new DataExchangeServerInstanceProvider(arguments, null));
            }
        }

        public DataExchangeTestServerServiceHost(IDataExchangeTestServer dataExchangeTestServer, Type serviceType)
            : base(serviceType)
        {
            foreach (var cd in ImplementedContracts.Values)
            {
                cd.Behaviors.Add(new DataExchangeServerInstanceProvider(null, dataExchangeTestServer));
            }
        }

        public DataExchangeTestServerServiceHost(DataExchangeTestServerArguments arguments, Type serviceType, params Uri[] baseAddresses)
            : base(serviceType, baseAddresses)
        {
            foreach (var cd in ImplementedContracts.Values)
            {
                cd.Behaviors.Add(new DataExchangeServerInstanceProvider(arguments, null));
            }
        }

        public DataExchangeTestServerServiceHost(IDataExchangeTestServer dataExchangeTestServer, Type serviceType, params Uri[] baseAddresses)
            : base(serviceType, baseAddresses)
        {
            foreach (var cd in ImplementedContracts.Values)
            {
                cd.Behaviors.Add(new DataExchangeServerInstanceProvider(null, dataExchangeTestServer));
            }
        }
    }

    public class DataExchangeServerInstanceProvider : IInstanceProvider, IContractBehavior
    {
        private readonly IDataExchangeTestServer _dataExchangeService = null;

        public DataExchangeServerInstanceProvider(DataExchangeTestServerArguments arguments, IDataExchangeTestServer dataExchangeTestServer)
        {
            _dataExchangeService = new Messaging.DataExchangeManager.DataExchangeTestServer.DataExchangeTestServer(arguments);
        }

        // IInstanceProvider Members
        public object GetInstance(InstanceContext instanceContext, Message message)
        {
            return this.GetInstance(instanceContext);
        }

        public object GetInstance(InstanceContext instanceContext)
        {
            return _dataExchangeService;
        }

        public void ReleaseInstance(InstanceContext instanceContext, object instance)
        {
        }

        // IContractBehavior Members
        public void AddBindingParameters(ContractDescription contractDescription, ServiceEndpoint endpoint, BindingParameterCollection bindingParameters)
        {
        }

        public void ApplyClientBehavior(ContractDescription contractDescription, ServiceEndpoint endpoint, ClientRuntime clientRuntime)
        {
        }

        public void ApplyDispatchBehavior(ContractDescription contractDescription, ServiceEndpoint endpoint, DispatchRuntime dispatchRuntime)
        {
            dispatchRuntime.InstanceProvider = this;
        }

        public void Validate(ContractDescription contractDescription, ServiceEndpoint endpoint)
        {
        }
    }

}
