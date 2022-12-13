using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeCommon;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Microsoft.Practices.Unity;
using Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Modules;
using Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Settings;

namespace Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager
{
    internal class MassTransitFileWatcherDataExchangeManagerServiceModule : IUnityContainerModule
    {
        public void Register(IUnityContainer container)
        {
            RegisterCommonUtilities(container);
            RegisterModules(container);
        }

        private static void RegisterCommonUtilities(IUnityContainer container)
        {
            container.RegisterFactory<IDataExchangeModule>();
            container.RegisterType<IExternalEventLogger, ExternalEventLogger>();
        }

        /// <summary>
        /// </summary>
        /// <param name="container"></param>
        private static void RegisterModules(IUnityContainer container)
        {
            //container.RegisterType<MessageExporter>();
            container.RegisterType<MassTransitFileWatcherDataExchangeManagerService>();
            container.RegisterType<MassTransitFileWatcherModuleSettings>(MassTransitFileWatcherModuleSettings.Modulename);
            //container.RegisterType<IWsLogicBase, MassTransitFileWatcherLogic>();
            container.RegisterType<MassTransitFileWatcherLogic>();
            container.RegisterType<IDataExchangeModule, MassTransitFileWatcherExportModule>(MassTransitFileWatcherExportModule.Modulename);
            container.RegisterType<IDataExchangeModule, MassTransitFileWatcherImportModule>(MassTransitFileWatcherImportModule.Modulename);
        }
    }
}
