using System;
using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService.Modules.Azure;
using Powel.Icc.Messaging.DataExchangeCommon;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.MessageLog;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Enums;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService
{
    public class AzureBusDataExchangeManagerServiceModule : IUnityContainerModule
    {
        public void Register(IUnityContainer container)
        {
            RegisterCommonUtilities(container);
            RegisterAzureModules(container);
        }

        private static void RegisterCommonUtilities(IUnityContainer container)
        {
            var settingsRepository = new SettingsRepository();
            container.RegisterType<IImportApplication, ImportApplication>();
            container.RegisterType<IFileUtility, FileUtility>();
            container.RegisterType<IImportEventLogger, ImportEventLogger>();
            container.RegisterFactory<IImportApplicationRunner>();
            container.RegisterTypeIfNotRegistered<IProcessRunner, ProcessRunner>();
            container.RegisterInstance<Func<ImportSettings>>(settingsRepository.GetSettingsFromEnvironmentVariables);

            // Modules
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, EdiImpRunner>(ImportApplicationType.EdiImp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, EdkInRunner>(ImportApplicationType.EdkIn.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, UtsImpRunner>(ImportApplicationType.UtsImp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, Gs2ImpRunner>(ImportApplicationType.Gs2imp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, XmlElImpRunner>(ImportApplicationType.XmlElImp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, ProdatImpRunner>(ImportApplicationType.ProdatImp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, NppoLoadRunner>(ImportApplicationType.Nppoload.ToString());
            
            container.RegisterFactory<IDataExchangeModule>();
            container.RegisterType<IDataExchangeMessageLog, DataExchangeMessageLog>();
            container.RegisterType<AzureBusDataExchangeManagerService>();
            //container.RegisterType<IDataExchangeManagerServiceSettingsFactory, DataExchangeManagerServiceSettingsFactory>();
            container.RegisterType<IExternalEventLogger, ExternalEventLogger>();
        }
        
        /// <summary>
        /// Handles only imports as of 25.08.16!
        /// Exports should eventually also be placed here.
        /// </summary>
        /// <param name="container"></param>
        private static void RegisterAzureModules(IUnityContainer container)
        {
            container.RegisterType<IWsLogicBase, AzureLogic>();
            container.RegisterType<IDataExchangeModule, AzureImportModule>(AzureImportModule.Modulename);
        }
    }
}
