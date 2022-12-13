using System;
using System.ServiceModel;
using System.Threading;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS
{
    public class StandardWsModule : ThreadBasedModule
    {
        private readonly IStandardWsWcfServiceHostFactory _serviceHostFactory;

        public const string MODULE_NAME = "STANDARDWS";

        public StandardWsModule(IStandardWsWcfServiceHostFactory serviceHostFactory, IServiceEventLogger serviceEventLogger) 
            : base(serviceEventLogger)
        {
            _serviceHostFactory = serviceHostFactory;
        }

        public override string ModuleName
        {
            get { return MODULE_NAME; }
        }

        protected sealed override void ExecuteUntilStopped()
        {
            using (var host = (ServiceHost)_serviceHostFactory.CreateServiceHost(string.Empty, new Uri[0]))
            {
                try
                {
                    host.Open();
                }
                catch (Exception Ex)
                {
                    ServiceEventLogger.LogCritical(Ex,false);
                    throw;
                }

                while (!IsStopRequested)
                {
                    Thread.Sleep(1000);
                }

                host.Close();
            }
        }
    }
}
