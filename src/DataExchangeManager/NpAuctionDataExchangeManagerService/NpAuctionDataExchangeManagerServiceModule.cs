using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeCommon;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.MessageLog;
using Powel.Icc.Messaging.NpAuctionDataExchangeManager.Modules;
using Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService.Modules;
using Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService.Settings;
//using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic;

namespace Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService
{
    internal class NpAuctionDataExchangeManagerServiceModule : IUnityContainerModule
    {
        public void Register(IUnityContainer container)
        {
            RegisterCommonUtilities(container);
            RegisterModules(container);
        }

        private static void RegisterCommonUtilities(IUnityContainer container)
        {
            //container.RegisterModule<ImportApplicationManagerLogicModule>();

            container.RegisterFactory<IDataExchangeModule>();
            container.RegisterType<IDataExchangeMessageLog, DataExchangeMessageLog>();
            container.RegisterType<NpAuctionDataExchangeManagerService>();
            container.RegisterType<IExternalEventLogger, ExternalEventLogger>();
        }

        /// <summary>
        /// </summary>
        /// <param name="container"></param>
        private static void RegisterModules(IUnityContainer container)
        {
            container.RegisterType<NpAuctionModuleSettings>(NpAuctionModuleSettings.Modulename);
            container.RegisterType<IWsLogicBase, NpAuctionLogic>();
            container.RegisterType<IDataExchangeModule, NpAuctionExportModule>(NpAuctionExportModule.Modulename);
        }
    }
}
