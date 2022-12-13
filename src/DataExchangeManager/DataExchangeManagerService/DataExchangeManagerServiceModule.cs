using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.MessageLog;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Builders;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Builders.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Validators;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Validators.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Mades;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardMsmq;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService
{
    public class DataExchangeManagerServiceModule : IUnityContainerModule
    {
        public void Register(IUnityContainer container)
        {
            RegisterCommonUtilities(container);

            container.RegisterType<IDataExchangeModule, StandardMsmqExportModule>(StandardMsmqExportModule.MODULE_NAME);
            container.RegisterType<IDataExchangeModule, StandardMsmqImportModule>(StandardMsmqImportModule.MODULE_NAME);
            
            container.RegisterType<IDataExchangeModule, StandardWsModule>(StandardWsModule.MODULE_NAME);

            RegisterCompelloModules(container);

            RegisterMadesModules(container);

            RegisterAzureModules(container);
        }

        private static void RegisterCommonUtilities(IUnityContainer container)
        {
            container.RegisterFactory<IDataExchangeModule>();
            container.RegisterType<IDataExchangeQueueFactory, MsmqDataExchangeQueueFactory>();
            container.RegisterType<IDataExchangeMessageLog, DataExchangeMessageLog>();
            container.RegisterType<StandardWsLogic>();
            container.RegisterTypeIfNotRegistered<IStandardWsWcfServiceHostFactory, StandardWsWcfServiceHostFactory>();
            container.RegisterType<DataExchangeManagerService>();
            container.RegisterType<IDataExchangeManagerServiceSettingsFactory, DataExchangeManagerServiceSettingsFactory>();
            container.RegisterType<MessageExporter>();
            container.RegisterInstance(MessageExporterSettings.Default);
            container.RegisterType<IExternalEventLogger, ExternalEventLogger>();
        }

        private void RegisterCompelloModules(IUnityContainer container)
        {
            if (!IsCompelloModuleRegisterable())
            {
                return;
            }

            container.RegisterType<ISettingsProvider, SettingsProvider>();
            container.RegisterTypeIfNotRegistered<IApiWrapperFactory, ApiWrapperFactory>();
            container.RegisterType<IDataExchangeModule, CompelloExportModule>(CompelloExportModule.MODULE_NAME);
            container.RegisterType<IExportSettingsBuilder, ExportSettingsBuilder>();
            container.RegisterType<IDataExchangeExportMessageValidator, DataExchangeExportMessageValidator>();

            container.RegisterType<IApiEventsListenerFactory, ApiEventsListenerFactory>();
            container.RegisterType<IMetaDataValueProvider, MetaDataValueProvider>();
            container.RegisterType<IImportMessageTranslator, ImportMessageTranslator>();
            container.RegisterType<IMessageImporter, MessageImporter>();
            container.RegisterType<IStatusChanger, StatusChanger>();
            //container.RegisterType<IHeartbeatTimer, HeartbeatTimer>();
            //container.RegisterType<IRestartTimer, CompelloTimer>();
            //container.RegisterType<ITimer, TimerAdapter>();
            container.RegisterType<IDataExchangeModule, CompelloImportModule>(CompelloImportModule.MODULE_NAME);
        }

        private static bool IsCompelloModuleRegisterable()
        {
            try
            {
                return new SettingsProvider().GetSettings() != null;
            }
            catch (ConfigurationException)
            {
                return false;
            }
        }

        #region Mades
        private void RegisterMadesModules(IUnityContainer container)
        {
            if (!IsMadesModuleRegisterable())
            {
                return;
            }

            container.RegisterType<MadesWsLogic, MadesWsLogic>();
            container.RegisterType<IDataExchangeModule, MadesImportModule>(MadesImportModule.MODULE_NAME);
            container.RegisterType<IDataExchangeModule, MadesExportModule>(MadesExportModule.MODULE_NAME);
        }

        private static bool IsMadesModuleRegisterable()
        {
            return IccConfiguration.DataExchangeManager.Mades.RunModule(true);
        }
        #endregion

        #region Azure
        private static void RegisterAzureModules(IUnityContainer container)
        {
            if (!IsAzureModuleRegisterable())
            {
                return;
            }

            container.RegisterType<WsLogicBase, AzureLogic>();
            if (Feature.AzureDataExchangeManager.IsEnabled(k => System.Configuration.ConfigurationManager.AppSettings[k]/* != "True"*/))
            {
                container.RegisterType<IDataExchangeModule, AzureImportModule>(AzureImportModule.MODULE_NAME);
            }
            container.RegisterType<IDataExchangeModule, AzureExportModule>(AzureExportModule.MODULE_NAME);
        }

        private static bool IsAzureModuleRegisterable()
        {
            return IccConfiguration.DataExchangeManager.RunAzureModule(false); // ToDo: Change to true when PIP is part of Standard modules.
        }
        #endregion
    }
}
