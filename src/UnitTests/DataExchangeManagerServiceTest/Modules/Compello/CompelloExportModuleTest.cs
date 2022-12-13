using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Builders;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Exceptions;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Validators;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class CompelloExportModuleTest
    {
        private Mock<IDataExchangeApi> _dataExchangeApiMock;
        private Mock<IApiWrapper> _compelloClientMock;
        private PushExportModule _compelloModule;
        private Mock<IApiWrapperFactory> _compelloClientFactoryMock;
        private Mock<IDataExchangeManagerServiceSettingsFactory> _dataExchangeManagerServiceSettingsFactoryMock;
        private Mock<ISettingsProvider> _settingsProviderMock;
        private Mock<IServiceEventLogger> _serviceEventLoggerMock;
        private MessageExporter _messageExporter;
        private Mock<IDataExchangeMessageLog> _dataExchangeMessageLogMock;

        [SetUp]
        public void SetUp()
        {
            _dataExchangeMessageLogMock = new Mock<IDataExchangeMessageLog>();
            _dataExchangeMessageLogMock.SetupAllProperties();

            var dataExchangeQueueTransactionMock = new Mock<IDataExchangeQueueTransaction>();
            dataExchangeQueueTransactionMock.SetupAllProperties();

            _dataExchangeApiMock = new Mock<IDataExchangeApi>();
            _dataExchangeApiMock
                .Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>()))
                .Returns(dataExchangeQueueTransactionMock.Object);

            _compelloClientMock = new Mock<IApiWrapper>();

            _compelloClientFactoryMock = new Mock<IApiWrapperFactory>();
            _compelloClientFactoryMock
                .Setup(x => x.Create(It.IsAny<Settings>()))
                .Returns(_compelloClientMock.Object);

            var dataExchangeManagerServiceSettings = new DataExchangeManagerServiceSettings
                {
                    TimeoutBeforeSendExportRetryInMs = 1
                };

            _dataExchangeManagerServiceSettingsFactoryMock = new Mock<IDataExchangeManagerServiceSettingsFactory>();
            _dataExchangeManagerServiceSettingsFactoryMock
                .Setup(x => x.GetSettings())
                .Returns(dataExchangeManagerServiceSettings);

            _serviceEventLoggerMock = new Mock<IServiceEventLogger>();
            _serviceEventLoggerMock.SetupAllProperties();

            _messageExporter = new MessageExporter(_dataExchangeApiMock.Object, _serviceEventLoggerMock.Object,
                _dataExchangeMessageLogMock.Object, MessageExporterSettings.Default);

            _settingsProviderMock = new Mock<ISettingsProvider>();
            _settingsProviderMock
                .Setup(m => m.GetSettings())
                .Returns(() => new Settings("SettingsRoutingAddress", 4321, "apiKey", 30000, 30000));

            _compelloModule = new CompelloExportModule(_compelloClientFactoryMock.Object,
                _dataExchangeManagerServiceSettingsFactoryMock.Object, _messageExporter, _serviceEventLoggerMock.Object,
                new ExportSettingsBuilder(_settingsProviderMock.Object), new DataExchangeExportMessageValidator());
        }

        [Test]
        public void SendExportMessage_ModuleIsSpecified_ExportMessageSentToTheSpecifiedAddress()
        {
            var message = new DataExchangeExportMessage
                {
                    MessageLogId = 1,
                    MessageReference = "DummyMessageReference",
                    RoutingAddress = "COMPELLO:"
//                    RoutingAddress = "COMPELLO:DummyRoutingAddress:1234"
                };
            message.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock
                .Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), "COMPELLO"))
                .Returns(message);

            _compelloModule.Start();
            _compelloModule.Stop(Defaults.DefaultModuleStopTimeout);

            _compelloClientFactoryMock.Verify(x => x.Create(new Settings("SettingsRoutingAddress", 4321, "apiKey", 30000, 30000)));
            message.DeleteMessageData();
        }

        [Test]
        public void SendExportMessage_RoutingAddressIsInvalid_MessageGetsDiscarded()
        {
            var message = new DataExchangeExportMessage
            {
                MessageLogId = 1,
                MessageReference = "DummyMessageReference",
                RoutingAddress = "incorrect"
            };
            message.SetMessageData("DummyMessageData",null);
            int count = 0;
            var expectedExceptionMessage = DataExchangeInvalidRoutingAddressException.GetInvalidRoutingAddressMessage(message.RoutingAddress);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), "COMPELLO"))
                // ensure that message is going to be returned only once, so the message is not going to be handled many times so we can verify the mocks methods invocation count
                .Returns(() => count++ == 0 ? message : null);

            _compelloModule.Start();
            while (!_compelloModule.IsRunning)
            {
                // wait until the module thread starts, I think that this could make the test unstable in the past
                Thread.Sleep(100);
            }
            _compelloModule.Stop(Defaults.DefaultModuleStopTimeout);

            _compelloClientFactoryMock.Verify(x => x.Create(It.IsAny<Settings>()), Times.Never());
            _serviceEventLoggerMock.Verify(sel => sel.LogMessage(8100, expectedExceptionMessage), Times.Exactly(1));
            _serviceEventLoggerMock.Verify(sel => sel.LogCritical(
                    It.Is<DataExchangeInvalidRoutingAddressException>(ex => ex.Message == expectedExceptionMessage && ex.MessageId == message.MessageLogId.ToString(CultureInfo.InvariantCulture)), 
                    false), 
                Times.Exactly(1));
            _serviceEventLoggerMock.Verify(sel => sel.LogMessage(8000, It.Is<String[]>(array => array.Single() == expectedExceptionMessage)), Times.Exactly(1));
            _dataExchangeMessageLogMock.Verify(messageLog => messageLog.SetStatus(message.MessageLogId, TransLogMessageStatus.ExportFailedDuringCreation), Times.Exactly(1));
            message.DeleteMessageData();
        }

        [Test]
        public void SendExportMessage_SendsExportMessage_ExportMessageIsSentCorrectly()
        {
            const string msgDta = "DummyMessageData";
            var message = new DataExchangeExportMessage
                {
                    MessageLogId = 1,
                    MessageReference = "DummyMessageReference",
                    RoutingAddress = "COMPELLO:"
                };
            message.SetMessageData(msgDta,null);
            _dataExchangeApiMock
                .Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), "COMPELLO"))
                .Returns(message);

            MemoryStream data = null;
            _compelloClientMock
                .Setup(x => x.SendMessage(It.IsAny<Stream>(), It.IsAny<Dictionary<string, object>>()))
                .Callback<Stream, Dictionary<string, object>>((d, m) => { data = (MemoryStream)d; })
                .Returns(13);


            _compelloModule.Start();
            _compelloModule.Stop(Defaults.DefaultModuleStopTimeout);

            Assert.AreEqual(msgDta, Encoding.UTF8.GetString(data.ToArray()));
            message.DeleteMessageData();
        }
    }
}