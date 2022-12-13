using System;
using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Enums;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic
{
    internal class ImportApplicationManagerLogicModule : IUnityContainerModule
    {
        public void Register(IUnityContainer container)
        {
            var settingsRepository = new SettingsRepository();
            container.RegisterType<IImportApplication, ImportApplication>();
            container.RegisterType<IFileUtility, FileUtility>();
            container.RegisterType<IImportEventLogger, ImportEventLogger>();
            container.RegisterFactory<IImportApplicationRunner>();
            container.RegisterTypeIfNotRegistered<IProcessRunner, ProcessRunner>();
            container.RegisterInstance<Func<ImportSettings>>(settingsRepository.GetSettingsFromEnvironmentVariables);
            //container.RegisterInstance<Func<INServiceBusEndpoint>>(NServiceBusEndpoint.Create);
            //container.RegisterTypeIfNotRegistered<INServiceBusEndpoint, NServiceBusEndpoint>();
            container.RegisterType<INServiceBusEndpoint, NServiceBusEndpoint>(new ContainerControlledLifetimeManager());

            // Modules
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, EdiImpRunner>(ImportApplicationType.EdiImp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, EdkInRunner>(ImportApplicationType.EdkIn.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, UtsImpRunner>(ImportApplicationType.UtsImp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, Gs2ImpRunner>(ImportApplicationType.Gs2imp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, XmlElImpRunner>(ImportApplicationType.XmlElImp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, ProdatImpRunner>(ImportApplicationType.ProdatImp.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, NppoLoadRunner>(ImportApplicationType.Nppoload.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, AncitraQueueRunner>(ImportApplicationType.AncitraQueue.ToString());
            container.RegisterTypeIfNotRegistered<IImportApplicationRunner, ServiceBus2TssRunner>(ImportApplicationType.ServiceBus2Tss.ToString());
        }
    }
}
