using System;
using System.ServiceModel;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS
{
    public interface IStandardWsWcfServiceHostFactory
    {
        ServiceHostBase CreateServiceHost(string constructorString, Uri[] baseAddresses);
    }
}