using System;
using System.Collections.Generic;
using System.Net;
using System.ServiceModel;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.MadesService;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Mades
{
    public class MadesImportModule : TaskBasedModule
    {
        public const string MODULE_NAME = "MADES_IMPORT";
        private const int GeneralInfoMessage = 1010;
        private const int NoConnection = 30390;

        public override string ModuleName => MODULE_NAME;

        private readonly string _endpointConfigName;
        List<string> _prioritizedBusinesstypes;    // Should be initialized from config.
        private TimeSpan _requestTimeout;
        private readonly MadesWsLogic _logic;
        private readonly IHeartbeatTimer _heartbeatTimer;
        private readonly IRestartTimer _restartTimer;
        private readonly string[] _testReceivers;
        private readonly string[] _testBusinessTypes;

        public MadesImportModule(MadesWsLogic logic, IServiceEventLogger serviceEventLogger)
            : base(serviceEventLogger)
        {
            if (Log.IsInfoEnabled) Log.Info($"Ctor: {ModuleName}");

            _logic = logic;

            _endpointConfigName = IccConfiguration.DataExchangeManager.Mades.EndpointConfigurationName("MadesEndpointSOAP12");

            // Do some kind of prioritizing.
            _prioritizedBusinesstypes = new List<string>();
            _prioritizedBusinesstypes.AddRange(IccConfiguration.DataExchangeManager.Mades.ImportBusinessTypes("BRM-A40,BRM-A39,BRM-Z15,BRM-ACK,BRM"));  // Business types as prioritized list. E.g: "BRM-A40,BRM-A39,BRM-Z15,BRM-ACK". BRM is old (pre 20200921) business type kept for handling in transition period.
            if (!_prioritizedBusinesstypes.Contains("") && IccConfiguration.DataExchangeManager.Mades.ImportBusinessTypesAddAll(false))
                _prioritizedBusinesstypes.Add("");  // Lowest Import priority should always be blank. I.e: Not prioritized business type/The rest

            _requestTimeout = TimeSpan.FromSeconds(IccConfiguration.DataExchangeManager.Mades.ImportIdleWait(5));

            _testReceivers = IccConfiguration.DataExchangeManager.Mades.HeartbeatReceivers("SERVICE-MMS");
            _testBusinessTypes = IccConfiguration.DataExchangeManager.Mades.HeartbeatBusinessTypes("BRM");
            _heartbeatTimer = new MadesHeartbeatTimer(IccConfiguration.DataExchangeManager.Mades.HeartbeatInterval(60000),this, serviceEventLogger); // Milliseconds. 1 minute

            _restartTimer = new MadesRestartTimer(IccConfiguration.DataExchangeManager.Mades.RestartInterval(120000), this); // Milliseconds. 2 minutes
        }

        protected override TimeSpan SleepTime => _requestTimeout;

        public override void Start()
        {
            if (Log.IsInfoEnabled) Log.Info($"{ModuleName} Starting.");
            LogError(GeneralInfoMessage, new string[] { $"{ModuleName} is Starting." });
            _heartbeatTimer.Run();
            base.Start();
            if (Log.IsInfoEnabled) Log.Info($"{ModuleName} Started.");
        }

        public bool IsPollingStopped { get; set; }

        public void StartHeartbeatTimer()
        {
            IsPollingStopped = false;
            _heartbeatTimer.Enabled = true;
        }

        public void StopPolling()
        {
            if (Log.IsInfoEnabled) Log.Info("");
            _heartbeatTimer.Enabled = false;;
            IsPollingStopped = true;
            StartRestartTimer();
        }

        private void StartRestartTimer()
        {
            _restartTimer.Run();
        }

        public override void Stop(TimeSpan timeout)
        {
            if (Log.IsInfoEnabled) Log.Info($"{ModuleName} Stopping");
            _heartbeatTimer.Stop();
            _restartTimer.Stop();
            base.Stop(timeout);
            if (Log.IsInfoEnabled) Log.Info($"{ModuleName} Stopped");
        }

        protected override bool TryExecuteSingleTask()
        {
            // Create instance of ECP endpoint service reference
            long nMoreMessages = 0;
            if (IsPollingStopped || IsStopRequested)
                return false;

            if (Log.IsInfoEnabled) Log.Info($"{ModuleName} Polling");
            try
            {
                using (var ecp = new MadesEndpointClient(_endpointConfigName))
                {
                    bool messageIsHandled = false;
                    using (var prioI = _prioritizedBusinesstypes.GetEnumerator())
                    {
                        while (!messageIsHandled && prioI.MoveNext())
                        {
                            if (Log.IsDebugEnabled) Log.Debug($"{ModuleName} BusinessType: {prioI.Current}");
                            var receivedMessage = ecp.ReceiveMessage(prioI.Current, true, out nMoreMessages);
                            if (receivedMessage == null)
                                continue; // No message for this businessType, try next in prioritized list.

                            if (Log.IsDebugEnabled) Log.Debug($"{ModuleName} Message received. Id: {receivedMessage.messageID}");
                            messageIsHandled = _logic.SubmitImport(receivedMessage);

                            string msgId = receivedMessage.messageID;
                            ecp.ConfirmReceiveMessage(ref msgId);
                            if (Log.IsDebugEnabled) Log.Debug($"{ModuleName} Confirmed. Id: {msgId}");
                        }
                    }
                }
            }
            catch (EndpointNotFoundException E)
            {
                LogError(NoConnection, ModuleName, E.Message);
                StopPolling();
            }
            catch (WebException E)
            {
                LogError(E.Message);
            }
            catch (InvalidOperationException E)
            {
                LogError(E.Message);
            }
            catch (TimeoutException E)
            {
                LogError(NoConnection, ModuleName, E.Message);
                StopPolling();
            }
            catch (CommunicationException E)
            {
                LogError(NoConnection,ModuleName,E.Message);
                Log.Error(E);
                StopPolling();
            }
            catch (Exception E)
            {
                LogError(E.Message);
            }
            return nMoreMessages > 0;
        }

        public bool InvokeHeartbeat()
        {
            bool ret = true;
            if (IsStopRequested) return ret;
            if (Log.IsInfoEnabled) Log.Info(ModuleName);
            try
            {
                using (var ecp = new MadesEndpointClient(_endpointConfigName))
                {
                    var testBusinessTypeI = _testBusinessTypes.GetEnumerator();
                    foreach (var testReceiver in _testReceivers)
                    {
                        if (IsStopRequested) return ret;
                        if (!testBusinessTypeI.MoveNext())
                            break;
                        var resp = ecp.ConnectivityTest(testReceiver, (string)testBusinessTypeI.Current);
                        if (Log.IsDebugEnabled) Log.Debug($"{ModuleName} Receiver: {testReceiver}, BusinessType: {(string)testBusinessTypeI.Current}, Response: {resp}");
                        if (string.IsNullOrEmpty(resp))
                        {
                            LogError($"{ModuleName} ConnectivityTest failed for: {testReceiver}, {(string)testBusinessTypeI.Current}");
                            ret = false;
                        }
                    }
                }
            }
            catch (TimeoutException E)
            {
                LogError(NoConnection, ModuleName, E.Message);
                StopPolling();
                ret = false;
            }
            catch (Exception E)
            {
                LogError(E.Message);
                ret = false;
            }
            return ret;
        }
    }
}
