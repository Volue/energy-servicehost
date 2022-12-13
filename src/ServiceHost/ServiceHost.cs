using System;
using System.Collections;
using System.Configuration;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Security.Principal;
using System.ServiceProcess;
using System.Threading;
using Microsoft.Practices.Unity;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging;
using Powel.Icc.Process;
using log4net;
using Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using AssemblyInfo = Powel.Reflection.AssemblyInfo;
using Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService;
using Powel.Icc.Messaging.IccpDataExchangeManager.IccpDataExchangeManagerService;
using Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService;
using Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager;

namespace ServiceHost
{
	public class ServiceHost : System.ServiceProcess.ServiceBase
	{
		private System.ComponentModel.Container components = null;
	    private readonly int ServiceClassLoaded = 6701;
        private readonly int FailedServiceInstantiation = 6715;

		Thread processingThread;
		bool processingThreadStarted = false;

		ServiceIterationBase[] iterationClasses;

		bool stopRequested;
		//TimeSpan errorWait;
		//TimeSpan minimumErrorWait;
		//TimeSpan maximumErrorWait;

        private static readonly ILog logger = LogManager.GetLogger(typeof(ServiceHost));

		public ServiceHost()
		{
			// This call is required by the Windows.Forms Component Designer.
			InitializeComponent();
		}

		// The main entry point for the process
		[MTAThread]
		static void Main()
		{
			// Følgende triks forenkler arbeidet med å utvikle en Windows Service.
			// Ved å sette flagget CONSOLE, f.eks. i toppen av denne filen, kaller
			// vi ikke ServiceBase.Run, men i stedet kaller vi koden i OnStart 
			// direkte, og dermed kjører prosjektet som en alminnelig konsoll-
			// applikasjon.

            // 09.03.2009: PER: Test på UserInteractive gjør at man ikke manuelt behøver å sette
            // "unsette" CONSOLE. Dette flagget vil bli satt automatisk.
            if (Environment.UserInteractive)
                new ServiceHost().OnStart(new string[0]);
            else
                Run(new ServiceHost());
		}

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			components = new System.ComponentModel.Container();
			this.AutoLog = false;
			this.CanShutdown = true;
            if (string.IsNullOrEmpty(ConfigurationManager.AppSettings["ServiceName"]))
                this.ServiceName = "Powel Service Host";
            else
                this.ServiceName = ConfigurationManager.AppSettings["ServiceName"];
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		/// <summary>
		/// Set things in motion so your service can do its work.
		/// </summary>
		protected override void OnStart(string[] args)
		{
            logger.Debug("Enter");

			stopRequested = false;

			//minimumErrorWait = new TimeSpan(0, 0, 5);		
			//maximumErrorWait = new TimeSpan(1, 0, 0);
			//errorWait = minimumErrorWait;

			processingThread = new Thread(new ThreadStart(ProcessingThread));
			processingThread.Start();

		    if (Environment.UserInteractive)
		    {
                Console.WriteLine("S || Enter: Stop");
                Console.WriteLine("K: KillService");
                Console.Write("Push key to exit:");
		        var keyInfo = Console.ReadKey();
                if (keyInfo.Key == ConsoleKey.S || keyInfo.Key == ConsoleKey.Enter)
                    Stop();
                else if (keyInfo.Key == ConsoleKey.K)
                    KillService();
		    }
		    else
		    {
		        while (!processingThreadStarted && !stopRequested)
		            Thread.Sleep(1);
		    }

		    if (stopRequested)
				throw new ApplicationException("The service host was unable to start.");

            logger.Debug("Exit");
		}
 
		/// <summary>
		/// Stop this service.
		/// </summary>
		protected override void OnStop()
		{
		    Stop();
		}

		protected override void OnShutdown()
		{
		    Stop();
		}

	    new void Stop()
        {
            stopRequested = true;
            logger.Info("Stopping services.");
            RequestStopServices();
            stopRequested = false;
        }

        void LogToEventLog(string message, EventLogEntryType type)
		{
			// Logging to event log can fail if:
			// * The application source is not created and a non-admin tries to automatically create it on logging.
			// * The log is full.

			try
			{
				EventLog.WriteEntry(message, type);
			}
			catch
			{
			}
		}

		void CriticalLog(Exception ex, EventLogModuleItem log)
		{
			// We will try to write to both the Windows Event Log and the ICC Event Log
			// and hope that at least one of them works.

			if (processingThreadStarted)
			{
				// Error while running.

			    var logger = new CriticalLogger();
                logger.Log(ex, log, EventLog, false);
			}
			else
			{
				// Error upon startup. Abort.

				string message = String.Format(
					"An error occurred while the service host was trying to start. Will now abort. The error was:\n{0}", 
					ex);

				LogToEventLog(message, EventLogEntryType.Error);

				try
				{
					log.LogMessage(5604, ex.Message);
				}
				catch
				{
				}

				stopRequested = true;
			}
		}

		void KillService()
		{
            if (Environment.UserInteractive)
                Environment.Exit(-1);
            else
                new ServiceController(this.ServiceName).Stop();
		}

		void ProcessingThread()
		{
            logger.Debug("Enter");

			try
			{
				int pollingSeconds = IccConfiguration.Messaging.PollingSeconds;
				int logsAgeInMinutes = IccConfiguration.Messaging.LogsAgeInMinutes;

				using (EventLogModuleItem log = new EventLogModuleItem(IccModule.M_SERVICE_HOST, Util.GetConfiguredConnectionString(true)))
				{
                    try
                    {
                        LogStartupInfo(log, pollingSeconds);

                        iterationClasses = LoadIterationClasses(log);

                        processingThreadStarted = true;

                        foreach (ServiceIterationBase iteration in iterationClasses)
                            iteration.Initialize();

                        ProcessServices(pollingSeconds, logsAgeInMinutes);

                        while (AreServicesRunning())
                            Thread.Sleep(100);

            			LogToEventLog("The service host has stopped.", EventLogEntryType.Information);
                    }
                    catch (Exception ex)
                    {
                        CriticalLog(ex, log);
                    }
				}
			}
			catch (Exception ex)
			{
				CriticalLog(ex, null);
			}

            logger.Info("Exit");
		}

        private void RequestStopServices()
        {
            if (iterationClasses == null) return;
            foreach (ServiceIterationBase iteration in iterationClasses)
                iteration.RequestStop();
        }

        private bool AreServicesRunning()
        {
            return iterationClasses.Any(iteration => iteration.IsRunning);
        }

	    ServiceIterationBase[] LoadIterationClasses(EventLogModuleItem iccLog)
		{
            logger.Debug("Enter");
			ArrayList list = new ArrayList();
			string[] classNames = (string[])ConfigurationManager.GetSection("serviceIterationClasses");
			// ToDo: Child containers should be initialized when the module is defined/loaded.
            var serviceDependencies = new ServiceDependencies(EventLog, iccLog);

			foreach (string className in classNames)
			{
				// ToDo: This test should be generalized and done after the type is resolved.
			    var clsTpName = className.Split(',')[0];
                if ((clsTpName == typeof(AzureBusDataExchangeManagerService).FullName && !Feature.AzureDataExchangeManager.IsEnabled(k => ConfigurationManager.AppSettings[k]/* == "True"*/)) ||
                    (clsTpName == typeof(EcpAmqpDataExchangeManagerService).FullName && !Convert.ToBoolean(ConfigurationManager.AppSettings["EcpAmqpDataExchangeManager.Enabled"] ?? "false")) ||
                    (clsTpName == typeof(IccpDataExchangeManagerService).FullName && !Convert.ToBoolean(ConfigurationManager.AppSettings["IccpDataExchangeManager.Enabled"] ?? "false")) ||
                    (clsTpName == typeof(NpAuctionDataExchangeManagerService).FullName && !Convert.ToBoolean(ConfigurationManager.AppSettings["NpAuctionDataExchangeManager.Enabled"] ?? "false")) || 
                    (clsTpName == typeof(MassTransitFileWatcherDataExchangeManagerService).FullName && !Convert.ToBoolean(ConfigurationManager.AppSettings["MassTransitFileWatcherDataExchangeManager.Enabled"] ?? "false")))
                {
                    continue;
                }

				logger.Debug($"Starting {className}");
				LogToEventLog("Starting " + className, EventLogEntryType.Information);
				// className can be a full reference to any type that inherits from
				// ServiceBase, but we support a simpler syntax for referring to our
				// standard services.
                Type type;				
				switch (className)
				{
					//case "Powel.Icc.Messaging.MeteringOrderService":
					//	type = typeof(MeteringOrderService);
					//	break;
     //               case "Powel.Icc.Messaging.OnDemandMeteringOrderService":
     //                   type = typeof(OnDemandMeteringOrderService);
     //                   break;
                    case "Powel.Icc.Messaging.TimeSeriesImport":
						type = typeof(TimeSeriesImport);
						break;
					case "Powel.Icc.Messaging.GS2ExportService":
						type = typeof(GS2ExportService);
						break;
					default:
						type = Type.GetType(className);
				        serviceDependencies.GetContainer().RegisterType(type);
						break;
				}

                try
                {
                    if(type == null)
                    {
                        throw new ApplicationException("The service has no type.");
                    }

                    var iteration = (ServiceIterationBase) serviceDependencies.GetContainer(type.Name).Resolve(type);
				    iccLog.LogMessage(IccModule.M_SERVICE_HOST,ServiceClassLoaded, iteration.ServiceIterationName);
				    list.Add(iteration);
                }
                catch(ResolutionFailedException ex)
                {
                    // Ex: Resolve() on CompelloImportModule with IccConfiguration parameters of type int (ICC_COMPELLO_PORT), not defined.
                    LogToEventLog(ex.Message,EventLogEntryType.Warning);
                    iccLog.LogMessage(FailedServiceInstantiation, className); // Failed to instantiate the service with class name {0}, this service may be missing some dependency or may not be supported. See system eventlog.
                    // Don't shut down, just run with the OK resolved types.
                }
			}
            if (list.Count == 0)
            {
                // No services instantiated. Stops.
                var log = new EventLogModuleItem();
                CriticalLog(new ApplicationException("No services instantiated. Stops."),log);
                log.Dispose();
            }

            logger.Info("Exit");

			return (ServiceIterationBase[])list.ToArray(
				typeof(ServiceIterationBase));
		}

		void LogStartupInfo(EventLogModuleItem log, int pollingSeconds)
		{
			string startMessage = String.Format("The service host, version {0}, has started. It is running as the user {1}. Processing will run in {2} second intervals. All further log messages will be logged to the ICC Event Log.",
				new AssemblyInfo(Assembly.GetEntryAssembly()).AssemblyFileVersion,
				WindowsIdentity.GetCurrent().Name, 
				pollingSeconds);

			LogToEventLog(startMessage, EventLogEntryType.Information);

			log.LogMessage(6700,
				new AssemblyInfo(Assembly.GetEntryAssembly()).AssemblyFileVersion,
				WindowsIdentity.GetCurrent().Name, 
				pollingSeconds.ToString());
		}

		void ProcessServices(int iterationSeconds,int logsAgeInMinutes)
		{
			foreach (ServiceIterationBase iteration in iterationClasses)
			{
				ServiceThread st = new ServiceThread(iteration, iterationSeconds, logsAgeInMinutes);
				st.StartProcess();
			}
		}
	}
}
