using System;
using System.Diagnostics;
using System.Threading.Tasks;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.Regions;
using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Interop;
using Powel.Icc.Services.Time;
using Powel.Wpf.Common;

namespace DataExchange.Administration.ImportModule
{
    public class ImportModule : IModule
    {
        private readonly IUnityContainer _unityContainer;
        private readonly IRegionManager _regionManager;

        public ImportModule(IUnityContainer unityContainer, IRegionManager regionManager)
        {
            _unityContainer = unityContainer;
            _regionManager = regionManager;
        }

        public void Initialize()
        {
            RegisterViewsAndServices();
        }

        private void RegisterViewsAndServices()
        {
            var eventLogModuleItemTask = new TaskFactory<IEventLogModuleItem>().StartNew(() => new EventLogModuleItem(IccModule.M_MESSAGELOG));
            var windowsLogTask = new TaskFactory<EventLog>().StartNew(() => new EventLog());
            var regionalCalendarTask = new TaskFactory<RegionalCalendar>().StartNew(() => IccConfiguration.Time.UserCalendar);

            _unityContainer.RegisterType<MainView>();
            _unityContainer.RegisterType<MainViewModel>();
            
            _unityContainer.RegisterType<Func<IEventLogModuleItem>>(new InjectionFactory(c => new Func<IEventLogModuleItem>(() =>
                {
                    eventLogModuleItemTask.Wait();
                    if(eventLogModuleItemTask.Exception != null)
                    {
                        throw eventLogModuleItemTask.Exception;
                    }
                    return eventLogModuleItemTask.Result;
                })));

            _unityContainer.RegisterType<Func<EventLog>>(new InjectionFactory(c => new Func<EventLog>(() =>
                {
                    windowsLogTask.Wait();
                    if (windowsLogTask.Exception != null)
                    {
                        throw windowsLogTask.Exception;
                    }
                    return windowsLogTask.Result;
                })));

            _unityContainer.RegisterType<Func<RegionalCalendar>>(new InjectionFactory(c => new Func<RegionalCalendar>(() =>
                {
                    regionalCalendarTask.Wait();
                    if(regionalCalendarTask.Exception != null)
                    {
                        throw regionalCalendarTask.Exception;
                    }
                    return regionalCalendarTask.Result;
                })));

            _unityContainer.RegisterType<ICriticalLogger, CriticalLogger>();
            _unityContainer.RegisterType<IServiceEventLogger, ServiceEventLogger>();
            _unityContainer.RegisterType<IErroneousImportsRepository, ErroneousImportsRepository>();
            _unityContainer.RegisterInstance<IDateTimeUtility>(DateTimeUtility.Current);

            _unityContainer.RegisterModule<DataExchangeApiModule>();

            _unityContainer.RegisterInstance(TaskScheduler.FromCurrentSynchronizationContext());

            _regionManager.RegisterViewWithRegion("MainRegion", typeof(MainView));
        }
    }
}
