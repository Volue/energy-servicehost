using System;
using System.ServiceModel;
using System.Text;
using log4net.Util;
using Microsoft.ServiceBus.Messaging;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Timers;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public class CompelloImportModule : EventBasedModule<IApiEventsListener>
    {
        public const string MODULE_NAME = "COMPELLO_IMPORT";

        private readonly IApiWrapperFactory _apiWrapperFactory;
        private readonly IStatusChanger _statusChanger;
        private readonly IMessageImporter _messageImporter;
        private readonly HeartbeatTimer _heartbeatTimer;
        private readonly CompelloTimer _compelloTimer;

        private IApiEventsListener _listener;

        public CompelloImportModule(
            IApiEventsListenerFactory eventClientFactory, 
            IServiceEventLogger eventLogger, 
            IApiWrapperFactory apiWrapperFactory,
            IStatusChanger statusChanger, 
            IMessageImporter messageImporter, 
            ISettingsProvider settingsProvider)
            : base(eventClientFactory, eventLogger)
        {
            Log.DebugExt($"Ctor: {ModuleName}");

            _apiWrapperFactory = apiWrapperFactory;
            _statusChanger = statusChanger;
            _messageImporter = messageImporter;
            _heartbeatTimer = new HeartbeatTimer(settingsProvider,eventLogger, new TimerAdapter()); // heartbeatTimer;
            _compelloTimer = new CompelloTimer(settingsProvider,eventLogger,new TimerAdapter(),_heartbeatTimer); // compelloTimer for restart;
        }

        public override string ModuleName => MODULE_NAME;

        protected override void SubscribeToEvents(IApiEventsListener listener)
        {
            Log.InfoExt($"{ModuleName}: SubscribeToEvents");
            _heartbeatTimer.Listener = listener;
            _compelloTimer.Listener = listener;
            listener.ClientError += OnClientError; // subscribe to error first
            listener.MessageReceived += OnMessageReceived;
            listener.Heartbeat += OnHeartbeat;
            listener.StatusChanged += OnStatusChanged;
        }

        protected override void UnsubscribeFromEvents(IApiEventsListener listener)
        {
            Log.InfoExt($"{ModuleName}: UnsubscribeFromEvents");
            listener.MessageReceived -= OnMessageReceived;
            listener.StatusChanged -= OnStatusChanged;
            listener.Heartbeat -= OnHeartbeat;
            listener.ClientError -= OnClientError; // unsubscribe from error as the last one            
        }

        protected override void StartListening(IApiEventsListener listener)
        {
            Log.InfoExt($"{ModuleName}: StartListening");
            _listener = listener;
            listener.Start();

            StartHeartbeatTimer(listener);
            StartCompelloTimer(listener);
        }

        protected override void StopListening(IApiEventsListener listener)
        {
            Log.DebugExt($"{ModuleName}: StopListening");
            StopHeartbeatTimer();
            bool synchronously = true;
            StopCompelloTimer(synchronously);

            listener.Stop(synchronously);
        }

        private void StartHeartbeatTimer(IApiEventsListener listener)
        {
            Log.DebugExt($"{ModuleName}: StartHeartbeatTimer");
            _heartbeatTimer.Run();
        }

        private void StopHeartbeatTimer()
        {
            Log.DebugExt($"{ModuleName}: StopHeartbeatTimer");
            _heartbeatTimer.Stop();
        }

        private void StartCompelloTimer(IApiEventsListener listener)
        {
            Log.DebugExt($"{ModuleName}: StartCompelloTimer");
            _compelloTimer.Run();
        }

        private void StopCompelloTimer(bool synchronously = false)
        {
            Log.DebugExt($"{ModuleName}: StopCompelloTimer. Synchronously={synchronously}");
            _compelloTimer.Stop(synchronously);
        }

        public override void RequestStop()
        {
            _heartbeatTimer.RequestStop();
        }
        private void OnClientError(object sender, ClientErrorEventArgs eventArgs)
        {
            Log.Error($"{ModuleName}: eventArgs.Error: {eventArgs.Error.Message}");
            Log.DebugExt($"{ModuleName}: eventArgs.Error.GetType(): {eventArgs.Error.GetType()}");
            LogError(eventArgs.Error);
            if (eventArgs.Error is CommunicationException)
            {
                StopCompelloListeningAndLogEventLog();  // Start is executed by CompelloTimer.OnTimeEvent
            }
            else if (eventArgs.Error is MessagingEntityNotFoundException)
            {
                Stop(new TimeSpan(0, 0, 5));
                _listener.Stop(false);  // Cannot run synchronously from OnClientError
            }
        }

        private void OnHeartbeat(object sender, ClientHeartbeatEventArgs e)
        {
            Log.InfoExt($"{ModuleName}: OnHeartbeat. NodeCount: {e.NodeCount}");
            _heartbeatTimer.HeartbeatReceived();
        }

        private void OnMessageReceived(object sender, ImportMessageReceivedEventArgs eventArgs)
        {
            /* From Morten Knutsen <mkn@compello.com>
             * "You should assume that if you get and exception in the MessageReceived, the message is not removed from the delivery queue"
             * 
             * oh 20160331: I'm not sure if the message is designed correct. The xml wrapper should use encoding UTF8, but the content of the MessageData should not necessarily follow that encoding.
             * The danish datahub also did a similar wrapping of EDIFACT inside a xml message. They stored the content with EDIFACT in a CDATA to escape the encoding handling.
             * See N:\Teknisk dokumentasjon Kraft\ediel\Danmark\Datahub\Old\Implementation guide for DataHub webservice interface.docx chapter 3.3.1 and 4.1.4 and http://en.wikipedia.org/wiki/XML#Escaping
             */
            Log.InfoExt($"{ModuleName}: OnMessageReceived: {eventArgs.MessageId}");
            ExecuteAndLogExceptions(() =>
            {
                var apiWrapper = _apiWrapperFactory.Create();
                object encName;
                var encoding = eventArgs.Metadata.TryGetValue("ENCODING",out encName) ? Encoding.GetEncoding(encName.ToString()) : Encoding.UTF8;
                var data = apiWrapper.GetTransactionData(eventArgs.MessageId,encoding);
                var message = new ImportMessage(eventArgs.MessageId, data, eventArgs.Metadata);

                _messageImporter.Import(message);
            }, true);
        }

        private void OnStatusChanged(object sender, StatusChangeEventArgs eventArgs)
        {
            Log.InfoExt($"{ModuleName}: OnStatusChanged: {eventArgs.TransactionId} Status: {eventArgs.Status}");
            ExecuteAndLogExceptions(() => _statusChanger.TryChangeStatus(eventArgs));
        }

        private void ExecuteAndLogExceptions(Action command, bool rethrowAsInternalException = false)
        {
            Log.InfoExt($"{ModuleName}: ExecuteAndLogExceptions");
            try
            {
                command();
            }
            catch (FaultException<ExceptionDetail> ex)
            {
                /* Error message from Compello:
                 * The process cannot access the file 'C:\\Compello\\FileStore\\2017\\10\\13\\10561894-61c4-4a95-aa7c-ef695d9aacb3\\0\\629696\\3189e1b2-a842-41a4-9a34-4da93d952268' because it is being used by another process. (Fault Detail is equal to An ExceptionDetail, likely created by IncludeExceptionDetailInFaults=true, whose value is:
                 */
                LogError(ex);
                // Don't rethrow an exception to Compello on their exception.
            }
            catch (Exception ex)
            {
                /* Error on own MSMQ handling
                 */
                LogError(ex);
                StopCompelloListeningAndLogEventLog();

                if (rethrowAsInternalException)
                {
                    throw new Exception("internal server error"); // we do not propagate any exception details to 3rd party code
                }
                // otherwise we do not rethrow since this module is not running in it's own thread so we do not want to corrupt whole app
            }
        }

        private void StopCompelloListeningAndLogEventLog()
        {
            StopHeartbeatTimer();
            _listener.Stop(false);   // Cannot run synchronously from OnClientError. See CompelloTimer.OnTimeEvent for restart
            LogEventLogMessage(30259, null); // Data Exchange Manager has stopped the connection to the Compello EDI-server.
        }
    }
}
