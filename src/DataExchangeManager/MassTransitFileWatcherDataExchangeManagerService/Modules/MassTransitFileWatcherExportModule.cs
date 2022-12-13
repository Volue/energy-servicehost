using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using log4net.Util;
using MassTransit;
using Newtonsoft.Json;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeCommon.Settings;
using Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Settings;
using Volue.MassTransit.Contracts.TimeSeriesMessage.ActivationRequestModels;
using Volue.MassTransit.RabbitMq;
using Volue.MassTransit.RabbitMq.Configuration;

namespace Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Modules
{
    public class MassTransitFileWatcherExportModule : ThreadBasedModule, IConsumer<Volue.MassTransit.Contracts.TimeSeriesMessage.ActivationRequestModels.TimeSeriesMessage>
    {
        // This class receives messages from RabbitMQ and stores them as files on parameterized catalogues and file names.
        // The files will be fetched by the SCADA system.
        // Subclassing ThreadBasedModule to avoid reference to DataExchangeAPI.DataExchangeMessage
        internal const string Modulename = "MassTransitFileWatcherExporter";
        public override string ModuleName => Modulename;

        private MassTransitFileWatcherParameters _parameters;
        private MassTransitFileWatcherModuleSettings _settings;
        private readonly FileSystemWatcher _fileWatcher = new FileSystemWatcher();
        private readonly MassTransitFileWatcherLogic _logic;
        private IBusControl _busControl;
        protected TimeSpan SleepTime { get; private set; }

        //private readonly object _executionLock = new object();

        public MassTransitFileWatcherExportModule(MassTransitFileWatcherModuleSettings settings, IServiceEventLogger serviceEventLogger, MassTransitFileWatcherLogic logic)
            : base(serviceEventLogger)
        {
            _settings = settings;
            _logic = logic;
            SleepTime = settings.ExportPollingInterval;
            var paramFile = settings.ExportParametersFile;
            var paramPath = Environment.ExpandEnvironmentVariables(settings.ExportParametersFilePath);
            var paramFilter = settings.ExportParametersFileFilter;
            InitParametersFromJsonFile(Path.Combine(paramPath, paramFile));
            SetUpFileWatcherForParametersFile(paramPath, paramFilter);
        }

        private void InitMassTransit()
        {
           var configuration = MassTransitRabbitMqSetup.BusConfiguration
                .StartNew()
                .ConfigureConnection(new RabbitMqConnectionConfiguration
                {
                    Host = new Uri(_settings.RabbitMqHost)
                })
                .ConfigureConsumers(new List<RabbitMqConsumerOptions>
                {
                    new RabbitMqConsumerOptions(typeof(TimeSeriesMessage),typeof(TimeSeriesMessage).FullName)
               })
                .Create();

            _busControl = MassTransitRabbitMqSetup.CreateBusControl(configuration);
            _busControl.StartAsync().GetAwaiter();
        }

        #region ConsumerMethods

        public Task Consume(ConsumeContext<Volue.MassTransit.Contracts.TimeSeriesMessage.ActivationRequestModels.TimeSeriesMessage> context)
        {
            return _logic.SubmitExport(context.Message);
        }

        #endregion

        protected override void ExecuteUntilStopped()
        {
            InitMassTransit();
            do
            {
                // Keep thread alive.
                Thread.Sleep(SleepTime);
            }
            while (!IsStopRequested);
        }

        #region ParameterFile

        private void InitParametersFromJsonFile(string fullPathFilename)
        {
            //lock (_executionLock)
            //{
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.GeneralInfoMsg, $"Loading {fullPathFilename}");
                using (var strReader = new StreamReader(fullPathFilename))
                {
                    _parameters = JsonConvert.DeserializeObject<MassTransitFileWatcherParameters>(strReader.ReadToEnd());
                }

                _logic.MessageFileParametersArray = _parameters.MessageFileParametersArray;
                Log.DebugExt(() => $"{ModuleName}: New parameters loaded. MessageFileParametersArray: {string.Join("\n", _parameters.MessageFileParametersArray.Select(m => $"MessageType: {m.MessageType}, Directory: {m.Directory}, FileNameTemplate: {m.FileNameTemplate}"))}");
            //}
        }

        private void SetUpFileWatcherForParametersFile(string paramPath, string paramFilter)
        {
            _fileWatcher.NotifyFilter = NotifyFilters.CreationTime | NotifyFilters.LastWrite;
            _fileWatcher.Path = paramPath;
            _fileWatcher.Filter = paramFilter;
            _fileWatcher.Changed += OnParametersFileChanged;
            _fileWatcher.EnableRaisingEvents = true;
        }

        private void OnParametersFileChanged(object source, FileSystemEventArgs eArgs)
        {
            InitParametersFromJsonFile(eArgs.FullPath);
        }

        #endregion
    }
}
