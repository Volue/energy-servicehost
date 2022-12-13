using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Threading;
using log4net;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeCommon.Settings;
using Powel.Icc.Process;

namespace Powel.Icc.Messaging.IccpDataExchangeManager.IccpDataExchangeManagerService
{
    public class IccpDataExchangeManagerService : ServiceIterationBase
    {
        private static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        private readonly ReadOnlyCollection<IDataExchangeModule> _modules;
        public IccpDataExchangeManagerService(IServiceEventLogger serviceEventLogger, Func<IEnumerable<IDataExchangeModule>> dataExchangeModuleFactory)
            : base(serviceEventLogger)
        {
            TimeoutInSecondsBeforeTerminatingModules = 20;

            _modules = dataExchangeModuleFactory()
                .Select(
                    module =>
                    {
                        ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.ServiceClassLoaded, module.ModuleName); // ToDo: This is used from ServiceHost.cs Should create a specific message where address and port are parameters.
                        return module;
                    })
                .ToList()
                .AsReadOnly();
        }

        public int TimeoutInSecondsBeforeTerminatingModules { get; set; }

        public override string ServiceIterationName => "ICCP/TASE.2 Data Exchange Manager Service";

        public override IccModule ServiceIterationModule => IccModule.M_ICCP_DATA_EXCHANGE_MANAGER;

        public override void RunIteration(out bool actualWorkDone)
        {
            StartModules();

            while (!StopRequested())
            {
                Thread.Sleep(100);
            }

            // request the Stop first so all the modules are going to know that they should stop at the same time
            // if we do this in loop with Join, the time is going to be wasted
            // because first Module is going to block other Modules (thread.Join()) so they will have less time to Stop
            RequestStop();

            StopModules();

            actualWorkDone = IsActualWorkDone();

            if (!IsAnyModuleStillRunning())
                return;

            // If any module is still running after timeout, let's shut them down by force
            TerminateRunningModules();
        }

        public override void Initialize()
        {
        }

        private void StartModules()
        {
            foreach (var module in _modules)
            {
                try
                {
                    Log.Debug($"Starting {module.ModuleName}");
                    module.Start();
                }
                catch (Exception e)
                {
                    Log.Warn($"The Data Exchange module {module.ModuleName} failed to start.", e);
                }
            }
        }

        public override void RequestStop()
        {
            Log.Info($"# of modules: {_modules.Count}");
            base.RequestStop();
            foreach (var module in _modules)
            {
                module.RequestStop();
            }
        }

        private void StopModules()
        {
            Log.Info($"#Modules: {_modules.Count}");
            if (_modules.Count == 0)
                return;
            var averageTimeout = TimeSpan.FromSeconds(TimeoutInSecondsBeforeTerminatingModules / (double)_modules.Count);

            StopModule(0, averageTimeout, averageTimeout);
        }

        private void StopModule(int index, TimeSpan averageTimeout, TimeSpan timeoutWithBonusIfPreviousHasFinishedEarlier)
        {
            if (index >= _modules.Count)
            {
                return;
            }

            var stopwatch = Stopwatch.StartNew();
            _modules[index++].Stop(timeoutWithBonusIfPreviousHasFinishedEarlier);
            stopwatch.Stop();

            // if this has finished earlier, then the next can take more time - due to this we can succesfully close more modules without Abort
            StopModule(index, averageTimeout, averageTimeout + (timeoutWithBonusIfPreviousHasFinishedEarlier - stopwatch.Elapsed));
        }

        private void TerminateRunningModules()
        {
            foreach (var module in _modules)
            {
                if (module.IsRunning)
                {
                    Log.Warn("Will abort module thread");
                    module.Abort();
                }
            }
        }

        private bool IsAnyModuleStillRunning()
        {
            return _modules.Any(module => module.IsRunning);
        }

        private bool IsActualWorkDone()
        {
            return _modules.All(module => !module.IsExecutingJobRightNow);
        }
    }
}
