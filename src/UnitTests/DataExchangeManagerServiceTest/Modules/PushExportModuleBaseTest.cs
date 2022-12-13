using System;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules
{
    [TestFixture]
    public class PushExportModuleBaseTest
    {
        private class DummyModule : PushExportModule
        {
            public bool IsSendExportMessageCalled { get; set; }

            public bool SendExportMessageResult { get; set; }

            public override string ModuleName
            {
                get { return "DummyModule"; }
            }

            public DataExchangeExportMessage DataExchangeExportMessage { get; set; }

            public DummyModule(
                IDataExchangeManagerServiceSettingsFactory dataExchangeManagerServiceSettingsFactory, 
                MessageExporter messageExporter, 
                IServiceEventLogger serviceEventLogger) 
                : base(dataExchangeManagerServiceSettingsFactory, messageExporter, serviceEventLogger)
            {
            }

            protected override string SendExportMessage(DataExchangeExportMessage export)
            {
                IsSendExportMessageCalled = true;

                DataExchangeExportMessage = export;

                return "DummyExternalReference";
            }
        }

        private DummyModule _dummyModule;
        private Mock<IDataExchangeApi> _dataExchangeApiMock;
        private Mock<IDataExchangeQueueTransaction> _dataExchangeApiTransactionMock;
        private Mock<IDataExchangeMessageLog> _dataExchangeMessageLogMock;
        private Mock<IDataExchangeManagerServiceSettingsFactory> _dataExchangeManagerServiceSettingsFactoryMock;
        private MessageExporter _messageExporter;

        [SetUp]
        public void SetUp()
        {
            _dataExchangeApiTransactionMock = new Mock<IDataExchangeQueueTransaction>();
            
            _dataExchangeMessageLogMock = new Mock<IDataExchangeMessageLog>();
            _dataExchangeMessageLogMock.SetupAllProperties();

            _dataExchangeApiMock = new Mock<IDataExchangeApi>();
            _dataExchangeApiMock.Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>()))
                .Returns(_dataExchangeApiTransactionMock.Object);

            var serviceEventLoggerMock = new Mock<IServiceEventLogger>();
            serviceEventLoggerMock.SetupAllProperties();

            DataExchangeManagerServiceSettings dataExchangeManagerServiceSettings = new DataExchangeManagerServiceSettings
                {
                    TimeoutBeforeSendExportRetryInMs = 1
                };

            _dataExchangeManagerServiceSettingsFactoryMock = new Mock<IDataExchangeManagerServiceSettingsFactory>();
            _dataExchangeManagerServiceSettingsFactoryMock.Setup(x => x.GetSettings()).Returns(dataExchangeManagerServiceSettings);

            _messageExporter = new MessageExporter(_dataExchangeApiMock.Object, serviceEventLoggerMock.Object, _dataExchangeMessageLogMock.Object, MessageExporterSettings.Default);

            _dummyModule = new DummyModule(_dataExchangeManagerServiceSettingsFactoryMock.Object, _messageExporter, serviceEventLoggerMock.Object);
        }

        [Test,Ignore("_dummyModule.DataExchangeExportMessage is cleared after successful sending.")]
        public void RunThread_ExportMessageIsAvailable_ExportMessageSentWithTheCorrectMessageData()
        {
            // Assign
            const string msgDta = "DummyMessageData";
            var messageToExport = new DataExchangeExportMessage();
            messageToExport.SetMessageData(msgDta,null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(messageToExport);

            _dummyModule.SendExportMessageResult = true;
            _dummyModule.IsSendExportMessageCalled = false;

            // Act

            _dummyModule.Start();
            _dummyModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            Assert.AreEqual(msgDta, _dummyModule.DataExchangeExportMessage.GetMessageData());
            messageToExport.DeleteMessageData();
        }

        [Test]
        public void RunThread_ExportMessageIsAvailableAndHasPositiveMessageLogId_MessageLogStatusIsUpdated()
        {
            // Assign

            var messageToExport = new DataExchangeExportMessage
                {
                    MessageLogId = 3,
                    RoutingAddress = "STANDARDMSMQ:dummyaddress:1234"
                };
            messageToExport.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(messageToExport);

            _dummyModule.SendExportMessageResult = true;
            _dummyModule.IsSendExportMessageCalled = false;

            // Act

            _dummyModule.Start();
            _dummyModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeMessageLogMock.Verify(x => x.SetStatusToExportTransferredToHub(3, "DummyExternalReference", "STANDARDMSMQ:dummyaddress:1234"));
            messageToExport.DeleteMessageData();
        }

        [Test]
        public void RunThread_ExportMessageIsAvailableButHasMessageLogIdZero_MessageLogStatusIsNotUpdated()
        {
            // Assign

            var messageToExport = new DataExchangeExportMessage
                {
                    MessageLogId = 0,
                    RoutingAddress = "STANDARDMSMQ:dummyaddress:1234"
                };
            messageToExport.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(messageToExport);

            _dummyModule.SendExportMessageResult = true;
            _dummyModule.IsSendExportMessageCalled = false;

            // Act

            _dummyModule.Start();
            _dummyModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeMessageLogMock.Verify(x => x.SetStatusToExportTransferredToHub(It.IsAny<long>(), It.IsAny<string>(), It.IsAny<string>()), Times.Never());
            messageToExport.DeleteMessageData();
        }

        [Test]
        public void RunThread_ExportMessageIsAvailable_CallsSendExportMessage()
        {
            // Assign

            var messageToExport = new DataExchangeExportMessage();
            messageToExport.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(messageToExport);

            _dummyModule.SendExportMessageResult = true;
            _dummyModule.IsSendExportMessageCalled = false;

            // Act

            _dummyModule.Start();
            _dummyModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            Assert.IsTrue(_dummyModule.IsSendExportMessageCalled, "SendExportMessage was not called on the dummy module based on PushExportModule.");
            messageToExport.DeleteMessageData();
        }
    }
}
