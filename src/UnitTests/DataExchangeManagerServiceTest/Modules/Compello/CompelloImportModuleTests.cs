using System;
using System.Collections.Generic;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class CompelloImportModuleTests
    {
        public const int GENERAL_ERROR_CODE = 8100;

        private CompelloImportModule _importModule;
        private ApiEventsListenerFake _apiEventsListenerFake;
        private Mock<IApiEventsListenerFactory> _listenerFactoryMock;
        private Mock<IServiceEventLogger> _eventLoggerMock;
        private Mock<IApiWrapper> _apiWrapperMock;
        private Mock<IApiWrapperFactory> _apiWrapperFactoryMock;
        private Mock<IStatusChanger> _statusChangerMock;
        private Mock<IMessageImporter> _messageImporterMock;
        private Mock<ISettingsProvider> _settingsProvider;
        private Settings _settings = new Settings("http://localhost",9999,"ads",60000,120000);

        [SetUp]
        public void SetUp()
        {
            _apiEventsListenerFake = new ApiEventsListenerFake();

            _listenerFactoryMock = new Mock<IApiEventsListenerFactory>();
            _listenerFactoryMock
                .Setup(m => m.Create())
                .Returns(_apiEventsListenerFake);
            _listenerFactoryMock
                .Setup(m => m.Create(It.IsAny<Settings>()))
                .Returns(_apiEventsListenerFake);

            _eventLoggerMock = new Mock<IServiceEventLogger>();

            _apiWrapperMock = new Mock<IApiWrapper>();
            _apiWrapperFactoryMock = new Mock<IApiWrapperFactory>();
            _apiWrapperFactoryMock
                .Setup(m => m.Create())
                .Returns(_apiWrapperMock.Object);

            _statusChangerMock = new Mock<IStatusChanger>();
            _messageImporterMock = new Mock<IMessageImporter>();
            _settingsProvider = new Mock<ISettingsProvider>();
            _settingsProvider.Setup(m => m.GetSettings()).Returns(_settings);
            _importModule = new CompelloImportModule(_listenerFactoryMock.Object, _eventLoggerMock.Object, _apiWrapperFactoryMock.Object,
                _statusChangerMock.Object, _messageImporterMock.Object, _settingsProvider.Object);
        }

        [Test]
        public void Start_WhenErrorEventIsTriggered_CallsLogger()
        {
            Exception dummyException = new Exception("test");
            ClientErrorEventArgs errorEventArgs = new ClientErrorEventArgs(dummyException);

            _importModule.Start();
            _apiEventsListenerFake.InvokeClientError(errorEventArgs);

            Assert.AreSame(dummyException, _importModule.FailureReason);
            Assert.AreEqual("test", _importModule.FailureReason.Message);
            _eventLoggerMock
                .Verify(m => m.LogMessage(GENERAL_ERROR_CODE, dummyException.Message));
        }

        [Test]
        public void Stop_AfterStart_StopsErrorEventHandling()
        {
            Exception dummyException = new Exception("test");
            ClientErrorEventArgs errorEventArgs = new ClientErrorEventArgs(dummyException);

            _importModule.Start();
            _importModule.Stop(Defaults.DefaultModuleStopTimeout);

            _apiEventsListenerFake.InvokeClientError(errorEventArgs);
            _eventLoggerMock
                .Verify(m => m.LogMessage(GENERAL_ERROR_CODE, dummyException.Message), Times.Never());
        }

        [Test]
        public void Start_WhenStatusChangedIsTriggered_CallsStatusChanger()
        {
            StatusChangeEventArgs eventArgs = new StatusChangeEventArgs(TransLogMessageStatus.ImportSuccessful, 666);

            _importModule.Start();
            _apiEventsListenerFake.InvokeStatusChanged(eventArgs);

            _statusChangerMock
                .Verify(m => m.TryChangeStatus(eventArgs));
        }

        [Test]
        public void Stop_AfterStart_StopsnStatusChangedEventHandling()
        {
            StatusChangeEventArgs eventArgs = new StatusChangeEventArgs(TransLogMessageStatus.ImportSuccessful, 666);

            _importModule.Start();
            _importModule.Stop(Defaults.DefaultModuleStopTimeout);
            _apiEventsListenerFake.InvokeStatusChanged(eventArgs);

            _statusChangerMock
                .Verify(m => m.TryChangeStatus(eventArgs), Times.Never());
        }

        [Test]
        public void Start_WhenMessageReceivedIsTriggered_CallsImporter()
        {
            ImportMessageReceivedEventArgs eventArgs = new ImportMessageReceivedEventArgs(666, new Dictionary<string, object>());

            _importModule.Start();
            _apiEventsListenerFake.InvokeMessageReceive(eventArgs);

            _messageImporterMock
                .Verify(m => m.Import(It.IsAny<ImportMessage>()), Times.Exactly(1));
        }

        [Test]
        public void Stop_AfterStart_StopsMessageReceivedEventHandling()
        {
            ImportMessageReceivedEventArgs eventArgs = new ImportMessageReceivedEventArgs(666, new Dictionary<string, object>());

            _importModule.Start();
            _importModule.Stop(Defaults.DefaultModuleStopTimeout);
            _apiEventsListenerFake.InvokeMessageReceive(eventArgs);

            _messageImporterMock
                .Verify(m => m.Import(It.IsAny<ImportMessage>()), Times.Exactly(0));
        }

        [Test]
        public void Start_WhenMessageReceivedIsTriggered_IsCreatingNewWrapperInstance()
        {
            ImportMessageReceivedEventArgs eventArgs = new ImportMessageReceivedEventArgs(666, new Dictionary<string, object>());

            _importModule.Start();
            _apiEventsListenerFake.InvokeMessageReceive(eventArgs);

            _apiWrapperFactoryMock
                .Verify(m => m.Create(), Times.Exactly(1));
        }

        [Test]
        public void Start_WhenStatusChangedIsTriggeredAndCatchesException_ExceptionIsNotThrownButLogged()
        {
            TestException dummyException = new TestException("test");
            StatusChangeEventArgs eventArgs = new StatusChangeEventArgs(TransLogMessageStatus.ImportSuccessful, 666);
            _statusChangerMock
                .Setup(m => m.TryChangeStatus(It.IsAny<StatusChangeEventArgs>()))
                .Throws(dummyException);

            _importModule.Start();
            _apiEventsListenerFake.InvokeStatusChanged(eventArgs);

            Assert.AreSame(dummyException, _importModule.FailureReason);
            Assert.AreEqual("test", _importModule.FailureReason.Message);
            _eventLoggerMock
                .Verify(m => m.LogMessage(GENERAL_ERROR_CODE, dummyException.Message));
        }

        [Test]
        public void Start_WhenMessageReceivedIsTriggeredAndCatchesException_ExceptionIsLoggedAndThrownWithoutDetails()
        {
            TestException dummyException = new TestException("test");
            ImportMessageReceivedEventArgs eventArgs = new ImportMessageReceivedEventArgs(666, new Dictionary<string, object>());
            _messageImporterMock
                .Setup(m => m.Import(It.IsAny<ImportMessage>()))
                .Throws(dummyException);

            _importModule.Start();
            var ex = Assert.Catch<Exception>(() => _apiEventsListenerFake.InvokeMessageReceive(eventArgs));

            Assert.IsFalse(ex is TestException);
            StringAssert.Contains("internal server error", ex.Message);
            Assert.AreSame(dummyException, _importModule.FailureReason);
            Assert.AreEqual("test", _importModule.FailureReason.Message);
            _eventLoggerMock
                .Verify(m => m.LogMessage(GENERAL_ERROR_CODE, dummyException.Message));
        }
    }

    internal class TestException : Exception
    {
        public TestException(string msg) : base(msg){}
    }

    internal class ApiEventsListenerFake : IApiEventsListener
    {
        public event EventHandler<ClientErrorEventArgs> ClientError;
        public event EventHandler<ImportMessageReceivedEventArgs> MessageReceived;
        public event EventHandler<StatusChangeEventArgs> StatusChanged;
        public event EventHandler<ClientHeartbeatEventArgs> Heartbeat;

        public bool IsStarted { get; private set; }

        public void Dispose()
        {
        }

        public void InvokeClientError(ClientErrorEventArgs eventArgs)
        {
            if (ClientError != null)
            {
                ClientError.Invoke(this, eventArgs);
            }
        }

        public void InvokeHeartbeat(ClientHeartbeatEventArgs eventArgs)
        {
            if (Heartbeat != null)
            {
                Heartbeat.Invoke(this, eventArgs);
            }
        }

        public void InvokeMessageReceive(ImportMessageReceivedEventArgs eventArgs)
        {
            if (MessageReceived != null)
            {
                MessageReceived.Invoke(this, eventArgs);
            }
        }

        public void InvokeStatusChanged(StatusChangeEventArgs eventArgs)
        {
            if (StatusChanged != null)
            {
                StatusChanged.Invoke(this, eventArgs);
            }
        }

        public void Start()
        {
        }
        
        public void Stop(bool synchronously = false)
        {   
        }

        public void InvokeHeartbeat()
        {           
        }
    }
}