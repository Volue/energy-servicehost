using System;
using Microsoft.Practices.Unity;
using Powel.Icc.Common;
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
using Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Modules;
using Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Settings;

namespace Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService
{
    public class EcpAmqpDataExchangeManagerServiceModule : IUnityContainerModule
    {
        public void Register(IUnityContainer container)
        {
            RegisterCommonUtilities(container);
            RegisterModules(container);
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
            container.RegisterType<MessageExporter>();

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
            container.RegisterType<EcpAmqpDataExchangeManagerService>();
            container.RegisterType<IExternalEventLogger, ExternalEventLogger>();
        }

        /// <summary>
        /// </summary>
        /// <param name="container"></param>
        private static void RegisterModules(IUnityContainer container)
        {
            container.RegisterType<EcpAmqpModuleSettings>(EcpAmqpModuleSettings.Modulename);
            container.RegisterType<IWsLogicBase, EcpAmqpLogic>();
            container.RegisterType<IDataExchangeModule, EcpAmqpImportModule>(EcpAmqpImportModule.Modulename);
            container.RegisterType<IDataExchangeModule, EcpAmqpExportModule>(EcpAmqpExportModule.Modulename);
            container.RegisterType<IDataExchangeModule, EcpAmqpStatusModule>(EcpAmqpStatusModule.Modulename);
        }
    }
}
