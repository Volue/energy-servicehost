using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging;
using Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerService;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerService.Service;
using Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService;
using Powel.Icc.Messaging.IccpDataExchangeManager.IccpDataExchangeManagerService;
using Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager;
using Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager;
using Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService;

namespace ServiceHost
{
    public class ServiceDependencies
    {
        private readonly EventLog _eventLog;
        private readonly EventLogModuleItem _iccLog;
        private readonly IUnityContainer _container = new UnityContainer();
        private readonly Dictionary<string, IUnityContainer> _childContainers = new Dictionary<string, IUnityContainer>();

        public ServiceDependencies() : this(new EventLog(), new EventLogModuleItem())
        {
        }

        public ServiceDependencies(EventLog eventLog, EventLogModuleItem iccLog)
        {
            _eventLog = eventLog;
            _iccLog = iccLog;

            RegisterContainer();
        }

        public IUnityContainer GetContainer(string serviceName = "")
        {
            IUnityContainer container;

            if (_childContainers.ContainsKey(serviceName))
            {
                container = _childContainers[serviceName];
            }
            else
            {
                container = _container;
            }

            return container;
        }

        private void RegisterContainer()
        {
            // Logging
            _container.RegisterInstance<IEventLogModuleItem>(_iccLog);
            _container.RegisterInstance<EventLogModuleItem>(_iccLog);
            _container.RegisterInstance<EventLog>(_eventLog);
            _container.RegisterType<ICriticalLogger, CriticalLogger>();
            _container.RegisterType<IServiceEventLogger, ServiceEventLogger>();

            // DataExchangeApi
            _container.RegisterModule<DataExchangeApiModule>();

            // ImportApplicationManagerService
            _childContainers.Add(typeof(ImportApplicationManagerService).Name, RegisterImportApplicationManagerServiceContainer(_container.CreateChildContainer()));

            // DataExchangeManagerService
            _childContainers.Add(typeof(DataExchangeManagerService).Name, RegisterDataExchangeManagerServiceContainer(_container.CreateChildContainer()));

            // AzureBusDataExchangeManagerService
            if (Feature.AzureDataExchangeManager.IsEnabled((s) => ConfigurationManager.AppSettings[s]/* == "True"*/))
            {
                _childContainers.Add(typeof(AzureBusDataExchangeManagerService).Name, RegisterAzureBusDataExchangeManagerServiceContainer(_container.CreateChildContainer()));
            }

            // EcpAmqpDataExchangeManagerService
            if (Convert.ToBoolean(ConfigurationManager.AppSettings["EcpAmqpDataExchangeManager.Enabled"] ?? "false"))
            {
                _childContainers.Add(typeof(EcpAmqpDataExchangeManagerService).Name, RegisterEcpAmqpDataExchangeManagerServiceContainer(_container.CreateChildContainer()));
            }

            // IccpDataExchangeManagerService
            if (Convert.ToBoolean(ConfigurationManager.AppSettings["IccpDataExchangeManager.Enabled"] ?? "false"))
            {
                _childContainers.Add(typeof(IccpDataExchangeManagerService).Name, RegisterIccpDataExchangeManagerServiceContainer(_container.CreateChildContainer()));
            }

            // NpAuctionDataExchangeManagerService
            if (Convert.ToBoolean(ConfigurationManager.AppSettings["NpAuctionDataExchangeManager.Enabled"] ?? "false"))
            {
                _childContainers.Add(typeof(NpAuctionDataExchangeManagerService).Name, RegisterNpAuctionDataExchangeManagerServiceContainer(_container.CreateChildContainer()));
            }

            // MassTransitFileWatcherDataExchangeManagerService
            if (Convert.ToBoolean(ConfigurationManager.AppSettings["MassTransitFileWatcherDataExchangeManager.Enabled"] ?? "false"))
            {
                _childContainers.Add(typeof(MassTransitFileWatcherDataExchangeManagerService).Name, RegisterMassTransitFileWatcherDataExchangeManagerServiceContainer(_container.CreateChildContainer()));
            }
            // MeteringOrderService
            //_container.RegisterType<MeteringOrderService>();

            // OnDemandMeteringOrderService
            //_container.RegisterType<OnDemandMeteringOrderService>();

            // TimeSeriesImport
            _container.RegisterType<TimeSeriesImport>();

            // GetGs2ExportServiceContainer
            _container.RegisterType<GS2ExportService>();
        }

        private static IUnityContainer RegisterImportApplicationManagerServiceContainer(IUnityContainer container)
        {
            var iccLog = new EventLogModuleItem(IccModule.M_IMPORT_APPLICATION_MANAGER);
            container.RegisterInstance<IEventLogModuleItem>(iccLog);
            container.RegisterInstance<EventLogModuleItem>(iccLog);

            container.RegisterModule<ImportApplicationManagerServiceModule>();

            return container;
        }

        private static IUnityContainer RegisterDataExchangeManagerServiceContainer(IUnityContainer container)
        {
            var iccLog = new EventLogModuleItem(IccModule.M_DATA_EXCHANGE_MANAGER);
            container.RegisterInstance<IEventLogModuleItem>(iccLog);
            container.RegisterInstance<EventLogModuleItem>(iccLog);

            container.RegisterModule<DataExchangeManagerServiceModule>();

            return container;
        }

        private static IUnityContainer RegisterAzureBusDataExchangeManagerServiceContainer(IUnityContainer container)
        {
            if (Feature.AzureDataExchangeManager.IsEnabled((s) => ConfigurationManager.AppSettings[s]/* == "True"*/))
            {
                var iccLog = new EventLogModuleItem(IccModule.M_AZUREBUS_DATA_EXCHANGE_MANAGER);
                container.RegisterInstance<IEventLogModuleItem>(iccLog);
                container.RegisterInstance<EventLogModuleItem>(iccLog);
                container.RegisterModule<AzureBusDataExchangeManagerServiceModule>();
            }
            return container;
        }

        private static IUnityContainer RegisterEcpAmqpDataExchangeManagerServiceContainer(IUnityContainer container)
        {
            var iccLog = new EventLogModuleItem(IccModule.M_ECP_AMQP_DATA_EXCHANGE_MANAGER);
            container.RegisterInstance<IEventLogModuleItem>(iccLog);
            container.RegisterInstance<EventLogModuleItem>(iccLog);
            container.RegisterModule<EcpAmqpDataExchangeManagerServiceModule>();
            return container;
        }

        private static IUnityContainer RegisterIccpDataExchangeManagerServiceContainer(IUnityContainer container)
        {
            var iccLog = new EventLogModuleItem(IccModule.M_ICCP_DATA_EXCHANGE_MANAGER);
            container.RegisterInstance<IEventLogModuleItem>(iccLog);
            container.RegisterInstance<EventLogModuleItem>(iccLog);
            container.RegisterModule<IccpDataExchangeManagerServiceModule>();
            return container;
        }

        private static IUnityContainer RegisterNpAuctionDataExchangeManagerServiceContainer(IUnityContainer container)
        {
            var iccLog = new EventLogModuleItem(IccModule.M_NP_AUCTION_DATA_EXCHANGE_MANAGER);
            container.RegisterInstance<IEventLogModuleItem>(iccLog);
            container.RegisterInstance<EventLogModuleItem>(iccLog);
            container.RegisterModule<NpAuctionDataExchangeManagerServiceModule>();
            return container;
        }

        private static IUnityContainer RegisterMassTransitFileWatcherDataExchangeManagerServiceContainer(IUnityContainer container)
        {
            var iccLog = new EventLogModuleItem(IccModule.M_MASSTRANSIT_FILEWATCHER_DATA_EXCHANGE_MANAGER);
            container.RegisterInstance<IEventLogModuleItem>(iccLog);
            container.RegisterInstance<EventLogModuleItem>(iccLog);
            container.RegisterModule<MassTransitFileWatcherDataExchangeManagerServiceModule>();
            return container;
        }
    }
}
