using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using System.Threading;
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
    public class EcpAmqpImportModule : TaskBasedModule
    {
        internal const string Modulename = "EcpAmqpImport";
        public override string ModuleName => Modulename;
        protected override TimeSpan SleepTime => TimeSpan.FromMilliseconds(_ecpAmqpParameters.EcpAmqpPollIntervalMs);

        private readonly CancellationTokenSource _cancellationSource = new CancellationTokenSource();

        private EcpAmqpParameters _ecpAmqpParameters;
        private readonly FileSystemWatcher _fileWatcher = new FileSystemWatcher();
        private readonly EcpAmqpLogic _logic;
        private readonly EcpAmqpModuleSettings _settings = new EcpAmqpModuleSettings();
        private Session _session;

        public EcpAmqpImportModule(EcpAmqpLogic logic, IServiceEventLogger serviceEventLogger) : base(serviceEventLogger)
        {
            _logic = logic;
            var paramFile = _settings.ImportParametersFile;
            var paramPath = Environment.ExpandEnvironmentVariables(_settings.ImportParametersFilePath);
            var paramFilter = _settings.ImportParametersFileFilter;
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
            if (!_ecpAmqpParameters.Enable)
            {
                Log.Info($"{ModuleName}: Disabled.");
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
                Log.Error("EcpAmqpDataExchangeManager Failed to start receiver or message poller:", exc);
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
            Log.Debug($"{ModuleName}: Creating session on {_ecpAmqpParameters.EcpAmqpUrl}");
            var url = new SecretsManager(IccConfiguration.Data.OracleConnectionString).Decrypt(_ecpAmqpParameters.EcpAmqpUrl);
            var connection = new Connection(new Address(url));
            _session = new Session(connection);
        }

        private void Disconnect()
        {
            if (_session == null)
                return;
            Log.Debug($"{ModuleName}: Disconnecting.");
            _session.Close();
            _session = null;
        }

        private async Task StartReceiving()
        {
            Log.Info($"{ModuleName}: Messages are pumped");
            if (_session == null)
                Connect();
            var queues = _ecpAmqpParameters.EcpAmqpInboxQueue.Split(',');
            var inboxes = new List<IReceiverLink>();
            int index = 0;
            foreach (var queue in queues)
            {
                var inbox = new ReceiverLink(_session,"inbox_" + index, queue);
                inbox.Start(int.MaxValue,OnMessageReceived);
                inboxes.Add(inbox);
                Log.Info($"{ModuleName}: Started receiving on {queue}");
            }
        }

        private void OnMessageReceived(IReceiverLink receiver, Message message)
        {
            try
            {
                receiver.Accept(message);
                HandleMessage(message);
            }
            catch (Exception Ex)
            {
                Log.Error(Ex);
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.CommonGeneralErrorMessage, Ex.Message);
            }
            finally
            {
                _isExecutingJobRightNow = false;
            }
            if (!_ecpAmqpParameters.Enable)
            {
                Log.Info($"{ModuleName}: Disabled.");
                RequestStop();
            }
        }

        private bool PollForMessages()
        {
            // Sequential/Single thread import.
            if (!_ecpAmqpParameters.Enable)
            {
                Log.Info($"{ModuleName}: Disabled.");
                RequestStop();
                return false;
            }

            try
            {
                if (_session == null)
                    Connect();

                // queues in prioritized order. Sync with setup in ecp.properties on ECP server (ecp.endpoiint.routes.incomingQueue[n].incomingQueue|messageTypes
                var queues = _ecpAmqpParameters.EcpAmqpInboxQueue.Split(',');
                bool messagesHandled = true; // Continue polling as long as we find messages.
                while (messagesHandled && !IsStopRequested)
                {
                    messagesHandled = false;
                    foreach (var queue in queues)
                    {
                        if (string.IsNullOrEmpty(queue))
                            continue;
                        var inbox = new ReceiverLink(_session, "inbox", queue);
                        Message message;
                        message = inbox.Receive(TimeSpan.FromMilliseconds(_ecpAmqpParameters.EcpAmqpPollTimeoutMs));
                        if (message != null)
                        {
                            HandleMessage(message);
                            inbox.Accept(message);
                            messagesHandled = true;
                        }

                        inbox.Close();
                        if (message != null)
                            break; // For each message handled, restart with polling on highest priority queue.
                    }
                }
            }
            catch (AmqpException Ex)
            {
                Log.Error(Ex);
                Disconnect(); // if "The transport 'TcpTransport' is closed.". Try reconnecting.
            }
            catch (SocketException Ex)
            {
                Log.Error(Ex);
                Disconnect();
            }
            return false;
        }

        private bool HandleMessage(Message message)
        {
            Log.DebugExt($"{ModuleName}: Received message: CorrelationID={message.Properties.CorrelationId}, ApplicationProperties={message.ApplicationProperties.ToString()}");
            var dataExchangeImportMessage = _logic.BuildDataExchangeImportMessage(message, message.DecodeBodyAsString(null));
            if (dataExchangeImportMessage != null)
            {
                var impJob = new ImportJob { MessageId = message.GetBusinessMessageId(), Msg = message };
                var workTask = Task.Run(() =>
                    {
                        _logic.SubmitImport(dataExchangeImportMessage, r => { impJob.Result = r; });
                    },
                    _cancellationSource.Token);
            }
            return true;
        }

        private void InitEcpAmqpParametersFromJsonFile(string fullPathFilename)
        {
            ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.CommonGeneralInformationMessage, $"Loading {fullPathFilename}");
            using (var strReader = new StreamReader(fullPathFilename))
            {
                _ecpAmqpParameters = JsonConvert.DeserializeObject<EcpAmqpParameters>(strReader.ReadToEnd());
            }

            if (_session != null)
                Disconnect();
            _logic.UseEdx = _ecpAmqpParameters.UseEdx;
            _logic.InitializePriorityList(_ecpAmqpParameters.EcpAmqpImportDocQueryPriorityList);
            _logic.InitializeProtocolRules(_ecpAmqpParameters.EcpAmqpImportDocQueryProtocolList);
            _logic.UseImportApplicationManagerService = _ecpAmqpParameters.UseImportApplicationManagerService;
            Log.DebugExt(() => $"{ModuleName}: New parameters loaded. UseEdx: {_ecpAmqpParameters.UseEdx}, Connection: {_ecpAmqpParameters.EcpAmqpUrl}, ReceiverLink.inbox: {_ecpAmqpParameters.EcpAmqpInboxQueue}, Polling: timeout: {_ecpAmqpParameters.EcpAmqpPollTimeoutMs}, interval: {_ecpAmqpParameters.EcpAmqpPollIntervalMs}, UseImportApplicationManagerService: {_ecpAmqpParameters.UseImportApplicationManagerService}");
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
            InitEcpAmqpParametersFromJsonFile(eArgs.FullPath);
        }

        internal class ImportJob
        {
            public string MessageId { get; set; }
            public Message Msg { get; set; }
            public bool Result { get; set; }
            public bool HasRun { get; set; }
            public bool Failed { get; set; }
            public Exception ThrownException { get; set; }
        }
    }
}
