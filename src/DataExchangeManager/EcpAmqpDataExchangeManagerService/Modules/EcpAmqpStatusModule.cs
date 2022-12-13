using System;
using System.IO;
using System.Threading.Tasks;
using Amqp;
using log4net.Util;
using Newtonsoft.Json;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Settings;
using Volue.Secrets.Storage;

namespace Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Modules
{
    public class EcpAmqpStatusModule : TaskBasedModule
    {
        /*  The changes to status of a message sent through AMQP channel are pushed to the dedicated queue by a specialized sent handler of
         * ECP(see 10.2 Send Event Handler in ECP Public interface). Application interested in the status of sent messages consumes the information
         * from the queue. Default send event handler queue is “ecp.endpoint.send.event”.
        */
        internal const string Modulename = "EcpAmqpStatus";
        public override string ModuleName => Modulename;
        protected override TimeSpan SleepTime => TimeSpan.FromMilliseconds(_ecpAmqpParameters.EcpAmqpPollIntervalMs);

        private readonly FileSystemWatcher _fileWatcher = new FileSystemWatcher();
        private readonly object _importLock = new object();
        private EcpAmqpParameters _ecpAmqpParameters;
        private readonly EcpAmqpLogic _logic;
        private readonly EcpAmqpModuleSettings _settings = new EcpAmqpModuleSettings();
        private ReceiverLink _inbox;
        public EcpAmqpStatusModule(EcpAmqpLogic logic, IServiceEventLogger serviceEventLogger) : base(serviceEventLogger)
        {
            _logic = logic;
            var paramFile = _settings.StatusParametersFile;
            var paramPath = Environment.ExpandEnvironmentVariables(_settings.StatusParametersFilePath);
            var paramFilter = _settings.StatusParametersFileFilter;
            InitEcpAmqpParametersFromJsonFile(Path.Combine(paramPath, paramFile));
            SetUpFileWatcherForParametersFile(paramPath, paramFilter);
        }

        protected sealed override bool TryExecuteSingleTask()
        {
            return PollForMessages();
        }

        public override void Abort()
        {
            Disconnect();
            base.Abort();
        }

        public override void Start()
        {
            if (_ecpAmqpParameters.UseEdx || !_ecpAmqpParameters.Enable)
            {
                // EDX does not report this kind of status on any queue.
                Log.Info($"{ModuleName}: Disabled");
                RequestStop();
                return;
            }
            try
            {
                if (_settings.UsePumpingReceiver)
                {
                    StartReceiving().Wait();
                }
                else
                {
                    base.Start();
                }
            }
            catch (Exception exc)
            {
                Log.Error($"{ModuleName}: Failed to start receiver or message poller:", exc);
                ServiceEventLogger.LogCritical(exc, false);
                throw new ApplicationException("EcpAmqpDataExchangeManager Failed to start receiver or message poller", exc);
            }
        }

        public override void Stop(TimeSpan timeout)
        {
            Log.Info($"{ModuleName}: Stopping");
            Disconnect();
            base.Stop(timeout);
        }

        private void Connect()
        {
            Log.Info($"{ModuleName}: Connecting and creating message receiver on {_ecpAmqpParameters.EcpAmqpUrl}, {_ecpAmqpParameters.EcpAmqpStatusQueue}");
            var url = new SecretsManager(IccConfiguration.Data.OracleConnectionString).Decrypt(_ecpAmqpParameters.EcpAmqpUrl);
            var connection = new Connection(new Address(url));
            var session = new Session(connection);
            _inbox = new ReceiverLink(session, "statusEvent", _ecpAmqpParameters.EcpAmqpStatusQueue);
        }

        private void Disconnect()
        {
            if (_inbox == null)
                return;
            Log.Info("Disconnecting.");
            _inbox.Close();
            _inbox = null;
        }

        private async Task StartReceiving()
        {
            Log.Info($"{ModuleName}: Messages are pumped");
            if (_inbox == null)
                Connect();
            _inbox.Start(Int32.MaxValue, OnMessageReceived);
        }

        private void OnMessageReceived(IReceiverLink receiver, Message message)
        {
            try
            {
                lock (_importLock)
                {
                    _isExecutingJobRightNow = true;
                    if (HandleMessage(message))
                        receiver.Accept(message);
                }
            }
            catch (Exception Ex)
            {
                Log.Error($"{ModuleName}:", Ex);
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.CommonGeneralErrorMessage, Ex.Message);
            }
            finally
            {
                _isExecutingJobRightNow = false;
            }
            if (_ecpAmqpParameters.UseEdx || !_ecpAmqpParameters.Enable)
            {
                Log.Info($"{ModuleName}: Disabled.");
                RequestStop();
            }
        }

        private bool PollForMessages()
        {
            // Sequential/Single thread import.
            if (_ecpAmqpParameters.UseEdx || !_ecpAmqpParameters.Enable)
            {
                Log.Info($"{ModuleName}: Disabled.");
                RequestStop();
                return false;
            }
            bool messageFound = true;
            try
            {
                lock (_importLock)
                {
                    if (_inbox == null)
                        Connect();
                    var message = _inbox.Receive(TimeSpan.FromMilliseconds(_ecpAmqpParameters.EcpAmqpPollTimeoutMs));
                    if (message != null)
                    {
                        if (HandleMessage(message)) 
                            _inbox.Accept(message);
                    }
                    else messageFound = false;
                }
            }
            catch (AmqpException Ex)
            {
                Log.Error($"{ModuleName}:",Ex);
                //throw;
                Disconnect();   // if "The transport 'TcpTransport' is closed.". Try reconnecting.
                messageFound = false;
            }
            return messageFound;
        }

        private bool HandleMessage(Message message)
        {
            Log.DebugExt($"{ModuleName}: Handle message. CorrelationId: {message.Properties.CorrelationId}, CreationTime: {message.Properties.CreationTime}, ApplicationProperties={message.ApplicationProperties.ToString()}");
            var externalReference = message.Properties.CorrelationId;
            if (externalReference == null)
                externalReference = message.GetMessageId();
            if (externalReference != null)
            {
                var body = message.DecodeBodyAsString();
                Log.Debug($"{ModuleName}: New status {body}");
                var ret = _logic.changeMessageLogState(externalReference, body);
                Log.Debug($"{ModuleName}: Status save operation returned: {ret}");
                return ret;
            }
            Log.Debug($"{ModuleName}: Not handled. Missing both correlation and message Id.");
            return false;
        }

        private void InitEcpAmqpParametersFromJsonFile(string fullPathFilename)
        {
            lock (_importLock)
            {
                using (var strReader = new StreamReader(fullPathFilename))
                {
                    _ecpAmqpParameters = JsonConvert.DeserializeObject<EcpAmqpParameters>(strReader.ReadToEnd());
                }

                if (_inbox != null && !_settings.UsePumpingReceiver)
                    Disconnect();
                _logic.InitializeStateRules(_ecpAmqpParameters.EcpAmqpStatusConversionList);
                Log.DebugExt(() => $"{ModuleName}: New parameters loaded. UseEdx: {_ecpAmqpParameters.UseEdx}, Connection: {_ecpAmqpParameters.EcpAmqpUrl}, ReceiverLink.statusQueue: {_ecpAmqpParameters.EcpAmqpStatusQueue}, Polling: timeout: {_ecpAmqpParameters.EcpAmqpPollTimeoutMs}, interval: {_ecpAmqpParameters.EcpAmqpPollIntervalMs}");
            }
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
            LogMessage.Log(Constants.MessageIdentifiers.CommonGeneralInformationMessage, false, eArgs.FullPath + " is changed.");
            InitEcpAmqpParametersFromJsonFile(eArgs.FullPath);
        }
    }
}
