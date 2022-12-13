using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using log4net.Util;
using Newtonsoft.Json;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.IccpDataExchangeManager.Settings;
using TASE2.Library.Client;
using TASE2.Library.Common;
using CommonConstants = Powel.Icc.Messaging.DataExchangeCommon.Settings.Constants;
using IccpConstants = Powel.Icc.Messaging.IccpDataExchangeManager.Settings.Constants;

namespace Powel.Icc.Messaging.IccpDataExchangeManager.Modules
{
    public class IccpImportModule : ThreadBasedModule
    {
        internal const string Modulename = "IccpImport";
        public override string ModuleName => Modulename;

        protected IccpParameters _iccpParameters;

        private readonly FileSystemWatcher _fileWatcher = new FileSystemWatcher();
        private readonly IccpLogic _logic;
        private readonly IccpModuleSettings _settings = new IccpModuleSettings();
        protected Client _client = null;
        protected int _clientIndex = 0;
        protected bool _reconnect;
        private bool _isDisconnecting = false;

        private readonly object _importLock = new object();

        public IccpImportModule(IccpLogic logic, IServiceEventLogger serviceEventLogger) : base(serviceEventLogger)
        {
            _logic = logic;

            var paramFile = _settings.ImportParametersFile;
            var paramPath = Environment.ExpandEnvironmentVariables(_settings.ImportParametersFilePath);
            var paramFilter = _settings.ImportParametersFileFilter;
            InitIccpParametersFromJsonFile(Path.Combine(paramPath, paramFile));
            SetUpFileWatcherForParametersFile(paramPath, paramFilter);

            Tase2Library.SetLogLevel(Log.IsDebugEnabled ? LogLevel.DEBUG : Log.IsInfoEnabled ? LogLevel.INFO : Log.IsWarnEnabled ? LogLevel.WARNING : Log.IsErrorEnabled || Log.IsFatalEnabled ? LogLevel.ERROR : LogLevel.NONE);
            _tase2LoggingEnabled = _iccpParameters.EnableTase2Logging;
            Tase2Library.SetLogHandler(LogHandler);

        }

        public override void Abort()
        {
            Disconnect();
            base.Abort();
        }

        public override void Start()
        {
            try
            {
                if (!Initialize())
                {
                    if (!IsStopRequested && _reconnect)
                        Reconnect();
                    if (_client == null)
                    {
                        RequestStop();
                        return;
                    }
                }

                Log.Info($"{ModuleName}: Messages are pushed");
            }
            catch (ClientException exc)
            {
                var msg = $"{ModuleName}: Failed to start. Error code: {exc.GetError()}, Message: {exc.Message}";
                Log.Error(msg,exc);
                ServiceEventLogger.LogMessage(CommonConstants.MessageIdentifiers.AnErrorOccurred,msg);
                RequestStop();
                throw new ApplicationException($"{ModuleName}: IccpDataExchangeManager Failed to start receiver or message poller", exc);
            }
            catch (Exception exc)
            {
                Log.Error($"{ModuleName}: IccpDataExchangeManager Failed to start receiver or message poller:", exc);
                ServiceEventLogger.LogCritical(exc, false);
                RequestStop();
                throw new ApplicationException($"{ModuleName}: IccpDataExchangeManager Failed to start receiver or message poller", exc);
            }
        }

        private bool Reconnect()
        {
            Log.Info($"Reconnect started. Interval: {_iccpParameters.ReconnectDelayTime}");
            ServiceEventLogger.LogMessage(IccpConstants.MessageIdentifiers.ReconnectMessage);
            _clientIndex++;
            if (_clientIndex > _iccpParameters.Clients.Length - 1)
                _clientIndex = 0;
            while (!IsStopRequested && !Initialize() && _clientIndex == _iccpParameters.Clients.Length-1)  // Try all clients before Sleep.
                Thread.Sleep(_iccpParameters.ReconnectDelayTime);
            if (!IsStopRequested && _client != null)
                _reconnect = false;
            return _client != null;
        }

        protected override void ExecuteUntilStopped()
        {
            throw new NotImplementedException("ExecuteUntilStopped");
        }

        public override void Stop(TimeSpan timeout)
        {
            Log.Info($"{ModuleName}: Stopping");
            Disconnect();
            base.Stop(timeout);
        }

        private bool Initialize()
        {
            var ret = Connect();
            if (!ret && _reconnect)
                return ret;

            /* Setup subscriptions:
             * A bit confusing this:
             * At HECO the GetDomainNames() method returned "". The answer from Michael Zillgith (MZ) was:
             *    Maybe the server hasn't implemented it because usually the client has to know the domain name.
             * So I added the DomainNames parameter in order to specify it.
             * On the test server (IccpVolumeTestServer) I added 2 BLT's for the client. 1 for icc1 domain and the second for icc2 domain.
             * When I connected the server did only find the first. So when I tried to create a synamic dataset for icc2, I was denied access.
             * MZ's answer was:
             *    Every client can only have one bilateral table associated. When you define two BLTs then the server will use the first one matching.
             *    The idea of ICCP is to have one dedicated domain for each client and the VCC scope that is shared by all clients.
             * So I don't understand why GetDomainNames() returns a list, when there is only 1 domain available for the client.
             */
            var domainNames = _client.GetDomainNames();
            if (domainNames.Count == 0 && _iccpParameters.DomainNames?.Length > 0)
                domainNames = _iccpParameters.DomainNames.Split(',').ToList();
            if (_iccpParameters.LogDomainDescription)
                Log.Info($"DomainNames: {string.Join(",", domainNames)}");
            foreach (var domainName in domainNames)
            {
                var dataSetNames = _client.GetDomainDataSets(domainName);
                var domVars = _client.GetDomainVariables(domainName);
                if (_iccpParameters.LogDomainDescription)
                {
                    Log.Info($"Domain: {domainName}, DataSetNames: {string.Join(",", dataSetNames)}");
                    Log.Debug($"Domain variables: {string.Join(",", domVars)}");
                }

                foreach (var dataSetName in dataSetNames)
                {
                    if (_iccpParameters.LogDomainDescription)
                    {
                        Log.Info($"DataSet: {dataSetName}");
                        if (Log.IsDebugEnabled)
                        {
                            var dsDirs = _client.GetDataSetDirectory(domainName, dataSetName);
                            Log.Debug($"DataSet directories: {string.Join(",",dsDirs.Select(dir => $"{dir.DomainId}/{dir.ItemId}"))}");
                        }
                    }

                    var dataSet = _client.GetDataSet(domainName, dataSetName);
                    dataSet.Read();
                    for (int i = 0; i < dataSet.GetSize(); i++)
                    {
                        string variableName = dataSet.GetPointVariableName(i);
                        if (domVars.Contains(variableName))
                            domVars.Remove(variableName);
                        var pointValue = dataSet.GetPointValue(i);
                        var pointDomainName = dataSet.GetPointDomainName(i);
                        if (_iccpParameters.LogDomainDescription)
                            Log.Info($"Variable: {variableName}, Type: {pointValue.Type.ToString()}, HasTimeStamp: {pointValue.HasTimeStamp}, PointDomain: {pointDomainName}");
                        _logic.AddDataSetPoint(pointDomainName, dataSetName, variableName, pointValue);   // Uses GetPointDomainName because it may be a VCC variable which domainName = null
                    }
                }

                if (_iccpParameters.UseDynamicDatasets && domVars.Count > 0)
                {
                    foreach (var domVar in domVars)
                    {
                        _logic.AddDataSetPoint(domainName,string.Empty,domVar,null); // Add PointValue when dynamic dataset is created.
                    }
                }
            }

            var transferSets = new List<ClientDSTransferSet>();
            var nMatched = _logic.MatchTimeseriesMapping();
            if (nMatched > 0)
            {
                Log.Debug("Creating transfer sets for predefined datasets.");
                foreach (var domainName in domainNames)
                {
                    foreach (var dataSetName in _logic.GetDataSetsWithExternalReference(domainName))
                    {
                        transferSets.Add(CreateTransferSet(domainName, dataSetName));
                    }
                }
            }
            if (_iccpParameters.UseDynamicDatasets)
            {
                Log.Debug("Creating dynamic datasets.");
                foreach (var domainName in domainNames)
                {
                    var dataSets = _logic.GetDynamicDatasets(domainName);
                    if (dataSets.Count == 0)
                        continue;
                    if (_iccpParameters.LogDomainDescription)
                        Log.Info($"Domain: {domainName}, Dynamic dataSets: {string.Join(",", dataSets)}");
                    foreach (var dataSetName in dataSets)
                    {
                        try
                        {
                            var variables = _logic.GetDynamicDatasetVariables(domainName, dataSetName);
                            if (_iccpParameters.LogDomainDescription)
                                Log.Info($"Dataset: {dataSetName}, Variables: {string.Join(",", variables)}");
                            if (_client.CreateDataSet(domainName, dataSetName, variables))
                            {
                                Log.Debug("Dynamic dataset created.");
                                var dataSet = _client.GetDataSet(domainName, dataSetName);
                                dataSet.Read();
                                for (int i = 0; i < dataSet.GetSize(); i++)
                                {
                                    string variableName = dataSet.GetPointVariableName(i);
                                    var pointValue = dataSet.GetPointValue(i);
                                    var pointDomainName = dataSet.GetPointDomainName(i);
                                    if (_iccpParameters.LogDomainDescription)
                                        Log.Info($"Variable: {variableName}, Type: {pointValue.Type.ToString()}, HasTimeStamp: {pointValue.HasTimeStamp}, PointDomain: {pointDomainName}");
                                    _logic.AddDataSetPoint(pointDomainName, dataSetName, variableName, pointValue);   // Sets pointValue for dynamic dataset. Uses GetPointDomainName because it may be a VCC variable which domainName = null
                                }

                                _logic.MatchTimeseriesMapping();    // To set externalReference
                                transferSets.Add(CreateTransferSet(domainName, dataSetName));
                                Log.Debug("Transfer set created.");
                            }
                        }
                        catch (ClientException Ex)
                        {
                            Log.Error(Ex);
                            Log.Info($"Exception error code: {Ex.GetError()}, Bilateral table id: {_client.GetBilateralTableId(domainName)}");
                            ServiceEventLogger.LogMessage(CommonConstants.MessageIdentifiers.AnErrorOccurred,$"{Ex.Message}: Dataset: {dataSetName}");
                        }
                    }
                }
            }

            if (transferSets.Count == 0)
            {
                Log.Info("No DSTransferSet(s). Stopping.");
                RequestStop();
                return false;
            }

            /* Enable transfer set reporting */
            Log.DebugExt($"Starting transfer sets: {string.Join(",",transferSets.Select(ts => ts.GetName()))}");
            foreach (var transferSet in transferSets)
            {
                transferSet.SetStatus(true);    // Siemens does not accept WriteStatus.
                transferSet.WriteValues();
            }

            return ret;
        }

        private ClientDSTransferSet CreateTransferSet(string domainName, string datasetName)
        {
            var transSet = _client.GetNextDSTransferSet(domainName);
            if (transSet == null)
            {
                var msg = $"No DSTS for {domainName}/{datasetName}";
                Log.Error(msg);
                ServiceEventLogger.LogMessage(CommonConstants.MessageIdentifiers.AnErrorOccurred,msg);
                return transSet;
            }

            /* connect data set with transfer set (received report values will be stored in the ClientDataSet instance) */
            var dataSet = _client.GetDataSet(domainName, datasetName);
            transSet.SetDataSet(dataSet);
            /* read transfer set parameters from server */
            transSet.ReadValues();
            /* configure data set transfer set */
            transSet.SetDataSetName(domainName, datasetName);
            /* Set some transfer set parameters */
            // The "interval timeout" means the server sends the data for events during the interval. It will send an empty report when no changes happen during the interval.
            // You should use either "INTERVAL" or "CHANGE". Both options together make no sense.
            // But you can combine CHANGE with "INTEGRITY". "INTEGRITY" means that all data is sent when the integrity timeout expires.
            transSet.SetRBE(true); // Report By Exception
            // When the client receives the report it sends a positive ACK to the server (because the "critical" flag is set in the transfer set).
            transSet.SetCritical(true); // Critical transfer report
            ReportReason reason = 0;

            TimeSpan timespan = (_iccpParameters.TransferSetIntervals.FirstOrDefault(i => i.Dataset == datasetName) ??
                                _iccpParameters.TransferSetIntervals.FirstOrDefault(i => i.Dataset == IccpParameters.Default)).Timespan;
            if (timespan != TimeSpan.Zero)
            {
                transSet.SetInterval((int)timespan.TotalSeconds); // seconds
                transSet.SetStartTime(TrimToNextUnixTime(DateTime.Now, timespan));
                reason |= ReportReason.INTERVAL_TIMEOUT;
            }

            TimeSpan integrityTimeout = (_iccpParameters.TransferSetReportIntegrityTimeouts.FirstOrDefault(i => i.Dataset == datasetName) ??
                                _iccpParameters.TransferSetReportIntegrityTimeouts.FirstOrDefault(i => i.Dataset == IccpParameters.Default)).Timespan;
            if (integrityTimeout != TimeSpan.Zero)
            {
                transSet.SetIntegrityCheck((int)integrityTimeout.TotalSeconds);
                transSet.SetStartTime(TrimToNextUnixTime(DateTime.Now, integrityTimeout));    // Make reporting start on whole timespan.
                reason |= ReportReason.INTEGRITY_TIMEOUT;
            }

            var objectChange = (_iccpParameters.TransferSetReportObjectChanges.FirstOrDefault(i => i.Dataset == datasetName) ??
                                    _iccpParameters.TransferSetReportObjectChanges.FirstOrDefault(i => i.Dataset == IccpParameters.Default)).Boolean;
            if (objectChange) reason |= ReportReason.OBJECT_CHANGE;

            transSet.SetDSConditionsRequested(reason);
            transSet.WriteValues();
            Log.Debug($"DsTransferSet: {transSet.GetName()}, DataSet: {transSet.GetDataSetDomain()}/{transSet.GetDataSetName()} defined. ConditionsRequested: {reason}, Interval: {timespan}, IntegrityTimeout: {integrityTimeout}");
            return transSet;
        }

        private int TrimToNextUnixTime(DateTime t, TimeSpan interval)
        {
            t = t.AddTicks(-(t.Ticks % interval.Ticks));    // Trim/truncate
            t = t.Add(interval);
            return (int)new DateTimeOffset(t).ToUnixTimeSeconds();
        }

        protected static TLSConfiguration CreateTlsConfiguration(bool chainValidation, bool allowOnlyKnownCertificates, string tlsOwnKeyFile, string ownCertificateFile, string CACertificateFiles, 
            string certificateDirectory = ".", string ownKeyPassword = null)
        {
            var tlsConfig = new TLSConfiguration();
            try
            {
                tlsConfig.ChainValidation = chainValidation;
                tlsConfig.AllowOnlyKnownCertificates = allowOnlyKnownCertificates;
                var dir = certificateDirectory;
                tlsConfig.SetOwnKey(Path.Combine(dir, tlsOwnKeyFile), ownKeyPassword);
                tlsConfig.SetOwnCertificate(Path.Combine(dir, ownCertificateFile));
                foreach (var filename in CACertificateFiles.Split(','))
                {
                    tlsConfig.AddCACertificate(Path.Combine(dir, filename));
                }
                Log.Debug("TLS configuration defined.");
            }
            catch (CryptographicException ex)
            {
                Log.Error(ex);
                tlsConfig = null;
            }

            return tlsConfig;
        }

        protected virtual bool Connect()
        {
            TLSConfiguration tlsConfig = null;

            // Single association type connection.
            if (_iccpParameters.Clients[_clientIndex].Tls.UseTls)
            {
                tlsConfig = CreateTlsConfiguration(_iccpParameters.Clients[_clientIndex].Tls.TlsChainValidation,
                    _iccpParameters.Clients[_clientIndex].Tls.TlsAllowOnlyKnownCertificates,
                    _iccpParameters.Clients[_clientIndex].Tls.TlsOwnKeyFile,
                    _iccpParameters.Clients[_clientIndex].Tls.TlsOwnCertificateFile,
                    _iccpParameters.Clients[_clientIndex].Tls.TlsCACertificateFiles,
                    _iccpParameters.TlsCertificateDirectory,
                    _iccpParameters.Clients[_clientIndex].Tls.TlsOwnKeyPassword);
            }

            string hostname = _iccpParameters.Clients[_clientIndex].Hostname;

            Log.Info($"{ModuleName}: Client parameters: apTitle: {_iccpParameters.LocalParameters.ApTitle}, aeQualifier: {_iccpParameters.LocalParameters.AeQualifier}, PSelector: {_iccpParameters.LocalParameters.PSelector}, SSelector: {_iccpParameters.LocalParameters.SSelector}, TSelector: {_iccpParameters.LocalParameters.TSelector}");
            string apTitle = _iccpParameters.Clients[_clientIndex].ApTitle;
            int aeQualifier = _iccpParameters.Clients[_clientIndex].AeQualifier;
            Log.Info($"{ModuleName}: Creating client on {hostname}, apTitle: {apTitle}, aeQualifier: {aeQualifier}, TASE.2 version: {Tase2Library.GetVersionString()}");
            try
            {
                if (_client == null)
                    _client = new Client(tlsConfig);
                _client.SetIdentity(FileVersionInfo.GetVersionInfo(Assembly.GetEntryAssembly().Location).CompanyName, ModuleName, Assembly.GetExecutingAssembly().GetName().Version.ToString() );
                _client.SetRemoteApTitle(apTitle, aeQualifier);
                _client.SetLocalAddresses(_iccpParameters.LocalParameters.PSelectorArray, _iccpParameters.LocalParameters.SSelectorArray, _iccpParameters.LocalParameters.TSelectorArray);
                _client.SetRemoteAddresses(_iccpParameters.Clients[_clientIndex].PSelectorArray, _iccpParameters.Clients[_clientIndex].SSelectorArray, _iccpParameters.Clients[_clientIndex].TSelectorArray);
                _client.SetConnectionClosedHandler(connectionClosedHandler, this);
                if (_iccpParameters.ReceiveInformationMessages)
                    _client.SetInformationMessageHandler(informationMessageHandler, this);
                _client.Connect(hostname, _iccpParameters.LocalParameters.ApTitle, _iccpParameters.LocalParameters.AeQualifier);

                ServiceEventLogger.LogMessage(IccpConstants.MessageIdentifiers.ConnectedMessage, hostname);
                if (Log.IsDebugEnabled)
                {
                    string vendor, model, revision;
                    _client.GetPeerIdentity(out vendor, out model, out revision);
                    Log.Debug($"Peer identity: Vendor={vendor}, Model={model}, Revision={revision}");
                    var supFeat = _client.GetSupportedFeatures();
                    Log.Debug($"Supported features: {supFeat}");
                    var vccVars = _client.GetVCCVariables();
                    Log.Debug($"VCC variables: {string.Join(",", vccVars)}");
                    var vccDs = _client.GetVCCDataSets();
                    Log.Debug($"VCC datasets: {string.Join(",", vccDs)}");
                }

                if (_iccpParameters.ReceiveInformationMessages)
                    _client.IMTransferSetEnable();

                /* set callback handlers for transfer set reports */
                _client.SetDSTransferSetValueHandler(dsTransferSetValueHandler, this);
                _client.SetDSTransferSetReportHandler(dsTransferSetReportHandler, this);
            }
            catch (ClientException exc)
            {
                Log.Error(exc);
                if (exc.GetError() == ClientError.FAILED)
                {
                    // "Failed to connect". Service not running. Enter reconnect mode.
                    _reconnect = true;
                    return false;
                }

                throw;
            }

            return true;
        }

        private void Disconnect()
        {
            if (_client == null)
                return;
            Log.Debug($"{ModuleName}: Disconnecting.");
            _isDisconnecting = true;
            try
            {
                _client.Disconnect();
            }
            catch (AccessViolationException ex)
            {
                // Sometimes gets this exception at HECO (version 12.4) if Log level > DEBUG. Some kind of Race condition?
                // The result is ServiceHost crash and other modules not properly closed. (Red flag in Evntlog).
                // Trying to avoid this by catching and ignoring the exception.
                Log.Error($"{ModuleName}: Exception ignored:" + ex.Message);
            }
            ServiceEventLogger.LogMessage(IccpConstants.MessageIdentifiers.DisconnectedMessage);
            //_client = null; // Reconnecting doesn't work well unless a new Client is created.
            _isDisconnecting = false;
        }

        /* callback handler that is called twice for each received transfer set report */
        private static void dsTransferSetReportHandler(object parameter, bool finished, UInt32 seqNo, ClientDSTransferSet transferSet)
        {
            var module = parameter as IccpImportModule;
            if (finished)
            {
                Log.Debug(string.Format("--> ({0}) report processing finished", seqNo));
                module.SendMessage(seqNo,transferSet);
            }
            else
            {
                Log.Debug(string.Format("New report received with seq no: {0} transfer set: {1}. Reason: {2}", seqNo, transferSet != null ? transferSet.GetName() : string.Empty, transferSet != null ? transferSet.GetDSConditionsDetected().ToString() : string.Empty));
                module.CreateNewMessage(seqNo,transferSet);
            }
        }

        private void CreateNewMessage(UInt32 seqNo, ClientDSTransferSet transferSet)
        {
            Monitor.Enter(_importLock);
            _logic.CreateNewMessage(string.Concat(Guid.NewGuid(),"-",seqNo), _iccpParameters.MessageOrigin);
        }

        private void SendMessage(UInt32 seqNo, ClientDSTransferSet transferSet)
        {
            var ret = _logic.SendMessage(transferSet != null? transferSet.GetDataSetName() : String.Empty);
            if (Monitor.IsEntered(_importLock))
                Monitor.Exit(_importLock);
            if (ret && (_iccpParameters.LogMessageReceived || Log.IsDebugEnabled))
            {
                var transferSetInfo = transferSet == null ? string.Empty : $" on transfer set: {transferSet.GetName()} sent.";
                ServiceEventLogger.LogMessage(CommonConstants.MessageIdentifiers.GeneralInfoMsg, $"Report no: {seqNo}{transferSetInfo}");
                Log.Debug($"Report no: {seqNo}{transferSetInfo}");
            }
        }

        /* callback handler that is called for each data point of a received transfer set report */
        private static void dsTransferSetValueHandler(object parameter, ClientDSTransferSet transferSet, string domainName, string pointName, PointValue pointValue)
        {
            //Log.DebugExt(string.Format("  Received report value for: {0}:{1} type: {2}", domainName != null ? domainName : "-", pointName, pointValue.Type));
            var module = parameter as IccpImportModule;
            module.AddMessageValue(transferSet,domainName,pointName,pointValue);
        }

        private void AddMessageValue(ClientDSTransferSet transferSet, string domainName, string pointName, PointValue pointValue)
        {
            // transferSet is null for datasets without Transfer_Set_Name defined.
            var datasetName = transferSet != null ? transferSet.GetDataSetName() : String.Empty;
            _logic.AddMessageValue(domainName, datasetName, pointName, pointValue);
        }

        /* callback handler that is called when the server closes the connection */
        private static void connectionClosedHandler(object parameter, Client client)
        {
            var module = parameter as IccpImportModule;
            module?.OnConnectionClosed();
        }

        protected void OnConnectionClosed()
        {
            if (_isDisconnecting)
                return;

            // Server closed the connection.
            ServiceEventLogger.LogMessage(IccpConstants.MessageIdentifiers.DisconnectedMessage);
            //Disconnect();
            if (IsStopRequested)
            {
                Stop(TimeSpan.FromSeconds(5));
                return;
            }

            _client.Disconnect();
            _client = null;
            _reconnect = true;
            Reconnect();
        }

        /* callback handler for information messages */
        protected static void informationMessageHandler(object parameter, int infoRef, int localRef, int msgId, byte[] message)
        {
            // Writes messages to EventLog.
            // The customer can then create Alarms on the events.
            var module = parameter as IccpImportModule;
            module?.OnInformationMessage(infoRef,localRef,msgId,message);
        }

        private void OnInformationMessage(int infoRef, int localRef, int msgId, byte[] message)
        {
            var msg = Encoding.Default.GetString(message);
            Log.Debug(string.Format("IM message: info-ref {0} local-ref: {1} msg-id: {2} size: {3} content: {4}", infoRef, localRef, msgId, message.Length, msg));
            ServiceEventLogger.LogMessage(IccpConstants.MessageIdentifiers.InformationMessage,infoRef.ToString(), localRef.ToString(), msgId.ToString(), msg);
        }

        private void InitIccpParametersFromJsonFile(string fullPathFilename)
        {
            lock (_importLock)  // Ensure no import executes while changing parameters.
            {
                ServiceEventLogger.LogMessage(CommonConstants.MessageIdentifiers.GeneralInfoMsg, $"Loading {fullPathFilename}");
                using (var strReader = new StreamReader(fullPathFilename))
                {
                    _iccpParameters = JsonConvert.DeserializeObject<IccpParameters>(strReader.ReadToEnd());
                }

                if (_client != null)
                {
                    Disconnect();
                    _reconnect = true;
                }

                _tase2LoggingEnabled = _iccpParameters.EnableTase2Logging;
                _logic.UseImportApplicationManagerService = _iccpParameters.UseImportApplicationManagerService;
                _logic.TimeseriesMaps = _iccpParameters.GetTimeseriesMaps;
                _logic.Priorities = _iccpParameters.Priorities;
                Log.DebugExt(() =>
                    $@"{ModuleName}: New parameters loaded. 
MessageOrigin: {_iccpParameters.MessageOrigin},
LocalParameters: ApTitle: {_iccpParameters.LocalParameters.ApTitle}, AeQualifier: {_iccpParameters.LocalParameters.AeQualifier}, PSelector: {_iccpParameters.LocalParameters.PSelector}, SSelector: {_iccpParameters.LocalParameters.SSelector}, TSelector: {_iccpParameters.LocalParameters.TSelector}, UseTls: { _iccpParameters.LocalParameters.Tls.UseTls}, TlsAllowOnlyKnownCertificates: { _iccpParameters.LocalParameters.Tls.TlsAllowOnlyKnownCertificates}, TlsCACertificatesFiles: { _iccpParameters.LocalParameters.Tls.TlsCACertificateFiles}, TlsChainValidation: { _iccpParameters.LocalParameters.Tls.TlsChainValidation}, TlsOwnCertificatesFile: { _iccpParameters.LocalParameters.Tls.TlsOwnCertificateFile}, TlsOwnKeyFile: { _iccpParameters.LocalParameters.Tls.TlsOwnKeyFile}, TlsOwnKeyPassword: { Regex.Replace(_iccpParameters.LocalParameters.Tls.TlsOwnKeyPassword ?? string.Empty, @"([^$]*)", "$1*****")}, ListeningTimeout: {_iccpParameters.LocalParameters.ListeningTimeout}
Clients: {string.Join("\n",_iccpParameters.Clients.Select(c => $"Hostname: { c.Hostname}, ApTitle: { c.ApTitle}, AeQualifier: { c.AeQualifier}, PSelector: { c.PSelector}, SSelector: { c.SSelector}, TSelector: { c.TSelector}, UseTls: { c.Tls.UseTls}, TlsAllowOnlyKnownCertificates: { c.Tls.TlsAllowOnlyKnownCertificates}, TlsCACertificatesFiles: { c.Tls.TlsCACertificateFiles}, TlsChainValidation: { c.Tls.TlsChainValidation}, TlsOwnCertificatesFile: { c.Tls.TlsOwnCertificateFile}, TlsOwnKeyFile: { c.Tls.TlsOwnKeyFile}, TlsOwnKeyPassword: { Regex.Replace(c.Tls.TlsOwnKeyPassword ?? string.Empty, @"([^$]*)", "$1*****")} "))}
TlsCertificateDirectory: {_iccpParameters.TlsCertificateDirectory}, 
DomainNames: {_iccpParameters.DomainNames},
TransferSetIntervals: {string.Join(";",_iccpParameters.TransferSetIntervals.Select(i => $"{i.Dataset},{i.Timespan}"))}, 
TransferSetReportIntegrityTimeouts: {string.Join(";",_iccpParameters.TransferSetReportIntegrityTimeouts.Select(i => $"{i.Dataset},{i.Timespan}"))}, 
TransferSetReportObjectChanges: {string.Join(";",_iccpParameters.TransferSetReportObjectChanges.Select(i => $"{i.Dataset},{i.Boolean}"))}, 
Priorities: {string.Join(";", _iccpParameters.Priorities.Select(i => $"{i.Dataset},{i.String}"))}, 
LogDomainDescription: {_iccpParameters.LogDomainDescription}, 
UseImportApplicationManagerService: {_iccpParameters.UseImportApplicationManagerService}, 
UseDynamicDatasets: {_iccpParameters.UseDynamicDatasets}, 
ReconnectDelayTime: {_iccpParameters.ReconnectDelayTime}, 
EnableTase2Logging: {_iccpParameters.EnableTase2Logging},
UseDbTimeseriesMaps: {_iccpParameters.UseDbTimeseriesMaps}, 
ExternalReferenceDelimiter: {_iccpParameters.ExternalReferenceDelimiter}, 
TimeseriesMaps: {string.Join(";",_iccpParameters.GetTimeseriesMaps.Select(m => $"{m.Variable.Domain}/{m.Variable.Dataset}/{m.Variable.VariableCode},{m.ExternalReference},{m.ValueUnit}"))}"
                );
            }
            if (_reconnect)
                Reconnect();
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
            InitIccpParametersFromJsonFile(eArgs.FullPath);
        }

        #region Tase2Logging
        private static bool _tase2LoggingEnabled;

        private static void LogHandler(LogLevel level, LogSource source, Endpoint ep, EndpointConnection epCon, string message)
        {
            if (!_tase2LoggingEnabled)
                return;
            if (ep != null)
            {
                if (epCon != null)
                    Log.Debug(string.Format("LIBTASE2[{0}|{4}]: [{2}][{3}] {1}", level.ToString(), message, ep.Id, epCon.PeerAddress, source.ToString()));
                else
                    Log.Debug(string.Format("LIBTASE2[{0}|{3}]: [{2}] {1}", level.ToString(), message, ep.Id, source.ToString()));
            }
            else
            {
                Log.Debug(string.Format("LIBTASE2[{0}|{2}]: {1}", level.ToString(), message, source.ToString()));
            }
        }
        #endregion
    }
}