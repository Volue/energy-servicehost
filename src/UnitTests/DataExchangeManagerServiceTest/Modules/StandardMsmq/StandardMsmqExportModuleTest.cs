using System;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardMsmq;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.StandardMsmq
{
    [TestFixture]
    public class StandardMsmqExportModuleTest
    {
        private const string MODULE_NAME = "STANDARDMSMQ";
        private const string NORMAL_PRIORITY = "NORMAL";

        private StandardMsmqExportModule _standardMsmqExportModule;
        private Mock<IDataExchangeApi> _dataExchangeApiMock;
        private Mock<IDataExchangeQueue<DataExchangeImportMessage>> _dataExchangeImportQueueMock;
        private Mock<IDataExchangeQueueTransaction> _dataExchangeImportQueueTransactionMock;
        private Mock<IDataExchangeQueueFactory> _dataExchangeImportQueueFactoryMock;
        private Mock<IDataExchangeManagerServiceSettingsFactory> _dataExchangeManagerServiceSettingsFactoryMock;
        private MessageExporter _messageExporter;

        [SetUp]
        public void SetUp()
        {
            var dataExchangeMessageLogMock = new Mock<IDataExchangeMessageLog>();
            dataExchangeMessageLogMock.SetupAllProperties();

            var dataExchangeApiTransactionMock = new Mock<IDataExchangeQueueTransaction>();
            dataExchangeApiTransactionMock.SetupAllProperties();

            _dataExchangeApiMock = new Mock<IDataExchangeApi>();
            _dataExchangeApiMock.Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>()))
                .Returns(dataExchangeApiTransactionMock.Object);

            _dataExchangeImportQueueTransactionMock = new Mock<IDataExchangeQueueTransaction>();
            _dataExchangeImportQueueTransactionMock.SetupAllProperties();

            _dataExchangeImportQueueMock = new Mock<IDataExchangeQueue<DataExchangeImportMessage>>();

            _dataExchangeImportQueueFactoryMock = new Mock<IDataExchangeQueueFactory>();
            _dataExchangeImportQueueFactoryMock.Setup(x => x.GetCustomImportQueue(It.IsAny<string>(), It.IsAny<string>()))
                .Returns(_dataExchangeImportQueueMock.Object);
            _dataExchangeImportQueueFactoryMock.Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>()))
                .Returns(_dataExchangeImportQueueTransactionMock.Object);

            DataExchangeManagerServiceSettings _dataExchangeManagerServiceSettings = new DataExchangeManagerServiceSettings
            {
                TimeoutBeforeSendExportRetryInMs = 1
            };
            _dataExchangeManagerServiceSettingsFactoryMock = new Mock<IDataExchangeManagerServiceSettingsFactory>();
            _dataExchangeManagerServiceSettingsFactoryMock.Setup(x => x.GetSettings()).Returns(_dataExchangeManagerServiceSettings);
            
            var serviceEventLoggerMock = new Mock<IServiceEventLogger>();
            serviceEventLoggerMock.SetupAllProperties();

            _messageExporter = new MessageExporter(_dataExchangeApiMock.Object, serviceEventLoggerMock.Object, dataExchangeMessageLogMock.Object, MessageExporterSettings.Default);

            _standardMsmqExportModule = new StandardMsmqExportModule(_dataExchangeImportQueueFactoryMock.Object, _dataExchangeManagerServiceSettingsFactoryMock.Object, _messageExporter, serviceEventLoggerMock.Object);
        }

        [Test]
        public void SendExportMessage_ExportMessageIsSent_MessageDataReceivedIsCorrect()
        {
            // Assign
            const string msgDta = "DummyMessageData";
            var messageToSend = new DataExchangeExportMessage
                {
                    Priority = NORMAL_PRIORITY,
                    RoutingAddress = "STANDARDMSMQ:dummymachine:1234"
                };
            messageToSend.SetMessageData(msgDta,null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), MODULE_NAME))
                .Returns(messageToSend);

            DataExchangeImportMessage messageReceived = null;

            _dataExchangeImportQueueMock.Setup(x => x.Enqueue(It.IsAny<DataExchangeImportMessage>(), It.IsAny<DataExchangeQueuePriority>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Callback<DataExchangeImportMessage, DataExchangeQueuePriority, IDataExchangeQueueTransaction>((m, p, t) => { messageReceived = m; });

            // Act

            _standardMsmqExportModule.Start();
            _standardMsmqExportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            Assert.AreEqual(msgDta, messageReceived.GetMessageData());
            messageToSend.DeleteMessageData();
        }

        [Test]
        public void SendExportMessage_ExportMessageIsSent_MessageIsEnqueuedUsingAnEnqueueTransaction()
        {
            // Assign

            var messageToSend = new DataExchangeExportMessage
                {
                    Priority = NORMAL_PRIORITY,
                    RoutingAddress = "STANDARDMSMQ:dummymachine:1234"
                };
            messageToSend.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), MODULE_NAME))
                .Returns(messageToSend);

            DataExchangeImportMessage messageReceived = null;

            _dataExchangeImportQueueMock.Setup(x => x.Enqueue(It.IsAny<DataExchangeImportMessage>(), It.IsAny<DataExchangeQueuePriority>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Callback<DataExchangeImportMessage, DataExchangeQueuePriority, IDataExchangeQueueTransaction>((m, p, t) => { messageReceived = m; });

            // Act

            _standardMsmqExportModule.Start();
            _standardMsmqExportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueFactoryMock.Verify(x => x.GetTransaction(DataExchangeQueueTransactionType.Enqueue));
            messageToSend.DeleteMessageData();
        }

        [Test]
        public void SendExportMessage_ExportMessageIsSent_MessageIsSentToTheCorrectMachine()
        {
            // Assign

            var messageToSend = new DataExchangeExportMessage
                {
                    Priority = NORMAL_PRIORITY,
                    RoutingAddress = "STANDARDMSMQ:dummymachine:1234"
                };
            messageToSend.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), MODULE_NAME))
                .Returns(messageToSend);

            // Act

            _standardMsmqExportModule.Start();
            _standardMsmqExportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueFactoryMock.Verify(x => x.GetTransaction(DataExchangeQueueTransactionType.Enqueue));
            messageToSend.DeleteMessageData();
        }

        [Test]
        public void SendExportMessage_ExportMessageIsSent_MessageIsSentWithTheCorrectIdentifier()
        {
            // Assign

            var messageToSend = new DataExchangeExportMessage
                {
                    Priority = NORMAL_PRIORITY,
                    RoutingAddress = "STANDARDMSMQ:dummymachine:1234"
                };
            messageToSend.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), MODULE_NAME))
                .Returns(messageToSend);

            // Act

            _standardMsmqExportModule.Start();
            _standardMsmqExportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueFactoryMock.Verify(x => x.GetCustomImportQueue(It.IsAny<string>(), "POWEL"));
            messageToSend.DeleteMessageData();
        }

        [Test]
        public void SendExportMessage_ExportMessageIsSent_MessageIsEnqueuedWithTheCorrectPriority()
        {
            // Assign

            var messageToSend = new DataExchangeExportMessage
                {
                    Priority = NORMAL_PRIORITY,
                    RoutingAddress = "STANDARDMSMQ:dummymachine:1234"
                };
            messageToSend.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), MODULE_NAME))
                .Returns(messageToSend);

            // Act

            _standardMsmqExportModule.Start();
            _standardMsmqExportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueMock.Verify(x => x.Enqueue(It.IsAny<DataExchangeImportMessage>(), DataExchangeQueuePriority.Normal, It.IsAny<IDataExchangeQueueTransaction>()));
            messageToSend.DeleteMessageData();
        }

        [Test]
        public void SendExportMessage_ExportMessageIsSent_BeginIsCalledOnTransaction()
        {
            // Assign

            var messageToSend = new DataExchangeExportMessage
                {
                    Priority = NORMAL_PRIORITY,
                    RoutingAddress = "STANDARDMSMQ:dummymachine:1234"
                };
            messageToSend.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), MODULE_NAME))
                .Returns(messageToSend);

            // Act

            _standardMsmqExportModule.Start();
            _standardMsmqExportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueTransactionMock.Verify(x => x.Begin());
            messageToSend.DeleteMessageData();
        }

        [Test]
        public void SendExportMessage_ExportMessageIsSent_CommitIsCalledOnTransaction()
        {
            // Assign

            var messageToSend = new DataExchangeExportMessage
                {
                    Priority = NORMAL_PRIORITY,
                    RoutingAddress = "STANDARDMSMQ:dummymachine:1234"
                };
            messageToSend.SetMessageData("DummyMessageData",null);
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), MODULE_NAME))
                .Returns(messageToSend);

            // Act

            _standardMsmqExportModule.Start();
            _standardMsmqExportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueTransactionMock.Verify(x => x.Commit());
            messageToSend.DeleteMessageData();
        }
    }
}
