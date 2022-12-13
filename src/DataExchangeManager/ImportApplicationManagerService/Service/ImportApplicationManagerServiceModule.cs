using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerService.Service
{
    internal class ImportApplicationManagerServiceModule : IUnityContainerModule
    {
        public void Register(IUnityContainer container)
        {
            container.RegisterType<ImportApplicationManagerService>();
            container.RegisterModule<ImportApplicationManagerLogicModule>();
        }
    }
}
