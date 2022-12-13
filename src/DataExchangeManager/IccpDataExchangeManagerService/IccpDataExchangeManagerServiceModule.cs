using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic;
using Powel.Icc.Messaging.DataExchangeCommon;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.MessageLog;
using Powel.Icc.Messaging.IccpDataExchangeManager.Modules;
using Powel.Icc.Messaging.IccpDataExchangeManager.Settings;

namespace Powel.Icc.Messaging.IccpDataExchangeManager.IccpDataExchangeManagerService
{
    internal class IccpDataExchangeManagerServiceModule : IUnityContainerModule
    {
        public void Register(IUnityContainer container)
        {
            RegisterCommonUtilities(container);
            RegisterModules(container);
        }

        private static void RegisterCommonUtilities(IUnityContainer container)
        {
            container.RegisterModule<ImportApplicationManagerLogicModule>();

            container.RegisterFactory<IDataExchangeModule>();
            container.RegisterType<IDataExchangeMessageLog, DataExchangeMessageLog>();
            container.RegisterType<IccpDataExchangeManagerService>();
            container.RegisterType<IExternalEventLogger, ExternalEventLogger>();
        }

        /// <summary>
        /// </summary>
        /// <param name="container"></param>
        private static void RegisterModules(IUnityContainer container)
        {
            container.RegisterType<IccpModuleSettings>(IccpModuleSettings.Modulename);
            container.RegisterType<IWsLogicBase, IccpLogic>();
            if (container.Resolve<IccpModuleSettings>().UseDualRole)
                container.RegisterType<IDataExchangeModule, IccpDualRoleImportModule>(IccpImportModule.Modulename);
            else
                container.RegisterType<IDataExchangeModule, IccpImportModule>(IccpImportModule.Modulename);
        }
    }
}
