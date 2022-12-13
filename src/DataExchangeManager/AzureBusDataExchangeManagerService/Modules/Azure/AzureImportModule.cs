using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
using log4net;
using Microsoft.ServiceBus.Messaging;
using Microsoft.WindowsAzure.Storage.Blob;
using Newtonsoft.Json;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService.Settings;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeCommon.Settings;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService.Modules.Azure
{
    public class AzureImportModule : IDataExchangeModule
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        internal const string Modulename = "AZURE_IMPORT";
        private readonly IServiceEventLogger _serviceEventLogger;
        private readonly AzureLogic _logic;
        private AzureParameters _azureParameters;   // Used for parameters which it can be convenient to change at runtime without restarting.
        private readonly FileSystemWatcher _fileWatcher = new FileSystemWatcher();
        private readonly AzureModuleSettings _settings = new AzureModuleSettings(); // Used for parameters controlled at startup.

        private volatile bool _stopRequested;
        private volatile bool _isRunning;
        private volatile bool _isExecutingJobRightNow;

        private int _pollNumber = 0;
        private DateTime _lastPollLog= DateTime.Now;

        public Exception FailureReason { get; private set; }
        public bool IsExecutingJobRightNow => _isExecutingJobRightNow;
        public bool IsRunning => _isRunning;

        private readonly CancellationTokenSource _cancellationSource = new CancellationTokenSource();
        private static readonly object lockObj = new object();
        private long _jobsProcessed = 0;
        readonly DateTime _serviceStarted = DateTime.Now;
        public AzureImportModule(AzureLogic logic, IServiceEventLogger serviceEventLogger)
        {
            _serviceEventLogger = serviceEventLogger;
            _logic = logic;
            string paramFile = _settings.ImportParametersFileFilter;
            string paramPath = _settings.ImportParametersFilePath; 
            InitAzureParametersFromJsonFile(Path.Combine(paramPath, paramFile));
            SetUpFileWatcherForParametersFile(paramPath, paramFile);
        }

        public string ModuleName => Modulename;

        public void Start()
        {
            _isRunning = true;
            try
            {
                if (_settings.UsePumpingReceiver)
                {
                    StartReceiving();
                }
                else
                {
                    PollForMessages();
                }
            }
            catch (Exception exc)
            {
                Log.Error("AzureBusDataExchangeManager Failed to start receiver or message poller:", exc);
                throw new ApplicationException("AzureBusDataExchangeManager Failed to start receiver or message poller", exc);
            }
        }

        public void RequestStop()
        {
            Log.Info("");
            _cancellationSource.Cancel();
            _stopRequested = true;
        }

        public void Stop(TimeSpan timeout)
        {
            Log.Info("");
            _cancellationSource.Cancel();
            _stopRequested = true;
            _isRunning = false;
        }

        public void Abort()
        {
            Log.Debug("Abort called. NOT IMPLEMENTED Exception will be thrown");
            throw new NotImplementedException();
        }

        private void StartReceiving()
        {
            Log.Info("Messages are pumped");
            Log.Debug($"Creating message receiver on {_azureParameters.pipInputQueuePrimaryConnectionString}, {_azureParameters.pipInputQueueName}");
            var manager = new Volue.Secrets.Storage.SecretsManager(IccConfiguration.Data.OracleConnectionString);
            var messageReceiver = new ServiceBusReceiver(manager.Decrypt(_azureParameters.pipInputQueuePrimaryConnectionString)).GetMessageReceiver(_azureParameters.pipInputQueueName);
            Log.Debug("Message receiver created");
            var opts = new OnMessageOptions
            {
                AutoComplete = false,
                MaxConcurrentCalls = _settings.MaxConcurrentImports
            };
            opts.ExceptionReceived += OnExceptionReceived;
            messageReceiver.OnMessage(DownloadPayloadAndStartWork, opts);
            if (_azureParameters.DebugPolling)
                SetupTimerForHeartbeat();
        }

        private void SetupTimerForHeartbeat()
        {
            var timer = new System.Timers.Timer(TimeSpan.FromMinutes(_azureParameters.HeartbeatIntervalWhenPumpingInMinutes).Milliseconds)
            {
                AutoReset = true,
                Enabled = true,
            };
            timer.Elapsed += TimerOnElapsed;
        }

        private void TimerOnElapsed(object sender, ElapsedEventArgs eventArgs)
        {            
            Log.Info($"Has been alive for {eventArgs.SignalTime.Subtract(_serviceStarted).ToString(@"dd\.hh\:mm\:ss")}, {_jobsProcessed} messages processed.");
        }

        private void OnExceptionReceived(object sender, ExceptionReceivedEventArgs e)
        {
            FailureReason = e.Exception;
            // ToDo: Check for Connection error related to ConnectionString: Toggle to pipInputQueueSecondaryConnectionString.
            Log.Error(e);
            _serviceEventLogger.LogCritical(new ApplicationException("AzureDataExchangeManager received an error on the bus", e.Exception), false);
        }

        private void PollForMessages()
        {
            // Sequential/Single thread import.
            MessageReceiver messageReceiver = null; 
            var manager = new Volue.Secrets.Storage.SecretsManager(IccConfiguration.Data.OracleConnectionString);
            while (!_stopRequested)
            {
                if (messageReceiver == null || messageReceiver.IsClosed)
                {
                    try
                    {
                        Log.Debug($"Instantiating bus receiver, connection: {_azureParameters.pipInputQueuePrimaryConnectionString}, Queue name: {_azureParameters.pipInputQueueName}");
                        messageReceiver = new ServiceBusReceiver(manager.Decrypt(_azureParameters.pipInputQueuePrimaryConnectionString)).GetMessageReceiver(_azureParameters.pipInputQueueName);
                        Log.Debug("Bus receiver Instantiated.");
                    }
                    catch (Exception exc)
                    {
                        Log.Error(exc);
                        _serviceEventLogger.LogCritical(new ApplicationException("ServiceBusReceiver failed to initialize!",exc), false);
                        throw;
                    }
                }
                var brokeredMessage = GetNextBrokeredMessage(messageReceiver, _azureParameters.AzurePollTimeoutMs);
                if (brokeredMessage != null)
                {
                    DownloadPayloadAndStartWork(brokeredMessage);
                }
                Thread.Sleep(_azureParameters.AzurePollTimeoutMs);  // Is this necessary. The timeout in GetNextBrokeredMessage() is a sleep.
            }
        }

        private void DownloadPayloadAndStartWork(BrokeredMessage brokeredMessage)
        {
            // Called by OnMessage/PumpingReceiver and PollForMessages
            lock (lockObj)
            {
                if (_jobsProcessed == long.MaxValue)
                {
                    _jobsProcessed = 0;
                }
                _jobsProcessed++;
            }
            Log.Info($"AzureBus Data Exchange: Starting processing of message {brokeredMessage.MessageId}");
            try
            {
                string filename = GetBlobFile(brokeredMessage);
                if (filename == null)
                {
                    return;
                }
                var payload = DownloadFromStorage(filename);
                _isExecutingJobRightNow = true;
                StartJobAndKeepLockAlive(
                    new ImportJob
                    {
                        MessageId = brokeredMessage.MessageId,
                        Message = brokeredMessage
                    }, 
                    _logic.BuildDataExchangeImportMessage(brokeredMessage, payload), _cancellationSource.Token);
                _isExecutingJobRightNow = false;
            }
            catch (Exception exc)
            {
                Log.Error(exc);
                _serviceEventLogger.LogCritical(
                    new ApplicationException("AzureServiceBusManager failed to download payload or start import job", exc),
                    false);
            }
        }

        private BrokeredMessage GetNextBrokeredMessage(MessageReceiver messageReceiver, int azureBusPollInterval)
        {
            try
            {
                if (_azureParameters.DebugPolling)
                {
                    _pollNumber++;
                    if (_pollNumber >= _azureParameters.PollLogInterval)
                    {
                        Log.Info($"Has polled the bus {_pollNumber} times since {_lastPollLog}");
                        _lastPollLog = DateTime.Now;
                        _pollNumber = 0;
                    }
                    else if (_pollNumber == 1)
                    {
                        Log.Info($"Is polling the bus (path: {messageReceiver.Path})!");
                    }
                }
                var brokeredMessage = messageReceiver.Receive(TimeSpan.FromMilliseconds(azureBusPollInterval));
                if (_azureParameters.DebugPolling && _pollNumber == 1)
                {
                    Log.Info($"Polling done. Message " + (brokeredMessage == null ? "not ": "") + "received.");
                }
                return brokeredMessage;
            }
            catch (Exception e)
            {
                FailureReason = e;
                Log.Error(e);
                // ToDo: Check for Connection error related to ConnectionString: Toggle to pipInputQueueSecondaryConnectionString.
                _serviceEventLogger.LogCritical(new ApplicationException("Receiver failed", e), false);
            }
            return null;
        }

        private void StartJobAndKeepLockAlive(ImportJob importJob, DataExchangeImportMessage dataExchangeImportMessage, CancellationToken cancellationToken)
        {
            Log.Info($"Is starting job {importJob.MessageId}");
            var brokeredMessage = importJob.Message;
            var workTask = Task.Run(() =>
                {
                    _logic.SubmitImport(dataExchangeImportMessage, r => { importJob.Result = r; });
                    dataExchangeImportMessage.DeleteMessageData();
                },
                cancellationToken);
            // Waits for the workTask to finish.
            while (!cancellationToken.IsCancellationRequested && !workTask.IsCompleted && !workTask.IsCanceled)
            {
                // Lock expire? 
                if (DateTime.UtcNow > brokeredMessage.LockedUntilUtc.AddSeconds(-10))
                    try
                    {
                        Log.Debug($"Message lock about to expire for {brokeredMessage.MessageId}. Renewing lock, expiry was {brokeredMessage.LockedUntilUtc}");
                        brokeredMessage.RenewLock();
                        Log.Debug($"Message lock was about to expire for {brokeredMessage.MessageId}. Renewed lock, new expiry {brokeredMessage.LockedUntilUtc}");
                    }
                    catch(Exception e)
                    {
                        Log.Error(e);
                        _serviceEventLogger.LogCritical(new ApplicationException($"Renew lock failed for job with messageid {importJob.MessageId}",e), false);
                        importJob.ThrownException = e;
                        break;
                    }
                Thread.Sleep(500);
            }
            importJob.ThrownException = workTask.Exception ?? importJob.ThrownException;
            importJob.Failed = importJob.Failed || workTask.Exception != null || workTask.IsCanceled;

            if (workTask.IsCanceled) //If work was cancelled, we do not want to complete the message or dead letter it.
            {
                Log.Info("Job is canceled.");
                _serviceEventLogger.LogCritical(importJob.ThrownException, false);
                return;
            }
            ImportHasFinished(importJob);
            workTask.Dispose();
        }

        private string GetBlobFile(BrokeredMessage message)
        {
            object obj;
            if (!message.Properties.TryGetValue("Upload_PipBlob_BlobPath", out obj))
            {
                Log.Error("Non existing Upload_PipBlob_BlobPath. Message discarded.");
                _serviceEventLogger.LogToEventLog("Non existing Upload_PipBlob_BlobPath. Message discarded.", EventLogEntryType.Error);
                message.Complete();
                return null;
            }
            return obj.ToString();
        }

        private void ImportHasFinished(ImportJob job)
        {
            Log.Info($"Job.MessageId: {job.MessageId}");
            job.HasRun = true;

            try
            {
                if (job.Failed)
                {
                    //Do some logging if job failed? 
                    job.Message.DeadLetter("Import has failed.", job.ThrownException?.Message ?? "");
                    Log.Error($"Import has failed. {job.ThrownException?.Message ?? ""}");
                }
                else
                {
                    job.Message.Complete();

                    if (!job.Result)
                    {
                        _serviceEventLogger.LogToEventLog("Import has failed.", EventLogEntryType.Warning);
                        Log.Error("Import has failed.");
                    }
                }
            }

            catch (Exception exception)
            {
                Log.Error($"Deadletter or Complete failed for job {job.MessageId}: {exception}");
                _serviceEventLogger.LogCritical(new ApplicationException($"Deadletter or Complete failed for job {job.MessageId}", exception), false);
            }
        }

        private string DownloadFromStorage(string filename)
        {
            string blobUriWithSas = _azureParameters.pipBlobStorageUrlAndSas;
            var container = new CloudBlobContainer(new Uri(blobUriWithSas));
            var blob = container.GetBlockBlobReference(filename);
            var payload = blob.DownloadText();
            return payload;
        }
        private void SetUpFileWatcherForParametersFile(string paramPath, string paramFile)
        {
            _fileWatcher.NotifyFilter = NotifyFilters.CreationTime | NotifyFilters.LastWrite;
            _fileWatcher.Path = paramPath;
            _fileWatcher.Filter = paramFile;
            _fileWatcher.Changed += OnAzureParametersFileChanged;
        }

        private void OnAzureParametersFileChanged(object source, FileSystemEventArgs eArgs)
        {
            _serviceEventLogger.LogMessage(Constants.MessageIdentifiers.GeneralInfoMsg, eArgs.FullPath + " is changed.");
            InitAzureParametersFromJsonFile(eArgs.FullPath);
        }

        private void InitAzureParametersFromJsonFile(string fullPathFilename)
        {
            var strReader = new StreamReader(fullPathFilename);
            _azureParameters = JsonConvert.DeserializeObject<AzureParameters>(strReader.ReadToEnd());
        }        

        internal class ImportJob
        {
            public string MessageId { get; set; }
            public BrokeredMessage Message { get; set; }
            public bool Result { get; set; }
            public bool HasRun { get; set; }
            public bool Failed { get; set; }
            public Exception ThrownException { get; set; }
        }

    }
}
