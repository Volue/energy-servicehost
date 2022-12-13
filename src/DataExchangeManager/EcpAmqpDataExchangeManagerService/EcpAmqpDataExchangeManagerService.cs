using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Threading;
using Powel.Icc.Diagnostics;
using Powel.Icc.Process;
using log4net;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeCommon.Settings;

namespace Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService
{
    public class EcpAmqpDataExchangeManagerService : ServiceIterationBase
    {
        private static readonly ILog Log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        private readonly ReadOnlyCollection<IDataExchangeModule> _modules;
        public EcpAmqpDataExchangeManagerService(IServiceEventLogger serviceEventLogger, Func<IEnumerable<IDataExchangeModule>> dataExchangeModuleFactory)
            : base(serviceEventLogger)
        {
            TimeoutInSecondsBeforeTerminatingModules = 20;

            _modules = dataExchangeModuleFactory()
                .Select(
                    module =>
                    {
                        ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.ServiceClassLoaded, module.ModuleName); // ToDo: Denne brukes fra ServiceHost.cs Burde kanskje laget en egen melding hvor adresse og port etc. var med.
                        return module;
                    })
                .ToList()
                .AsReadOnly();
        }

        public int TimeoutInSecondsBeforeTerminatingModules { get; set; }

        public override string ServiceIterationName => "Ecp/Amqp Data Exchange Manager Service";

        public override IccModule ServiceIterationModule => IccModule.M_ECP_AMQP_DATA_EXCHANGE_MANAGER;

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
            Log.Info("");
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
