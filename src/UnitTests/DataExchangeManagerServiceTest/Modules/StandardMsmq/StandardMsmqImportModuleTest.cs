using System;
using Moq;
using NUnit.Framework;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardMsmq;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.StandardMsmq
{
    [TestFixture]
    public class StandardMsmqImportModuleTest
    {
        private StandardMsmqImportModule _powelImportModule;
        private Mock<IDataExchangeQueueFactory> _dataExchangeImportQueueFactoryMock;
        private Mock<IDataExchangeQueue<DataExchangeImportMessage>> _dataExchangeImportQueueMock;
        private Mock<IDataExchangeApi> _dataExchangeApiMock;
        private Mock<IDataExchangeQueueTransaction> _dataExchangeImportQueueTransactionMock;
        private Mock<IEventLogModuleItem> _eventLogModuleItemMock;
        private Mock<IServiceEventLogger> _serviceEventLoggerMock;
        private DataExchangeSettings _dataExchangeSettings;

        [SetUp]
        public void SetUp()
        {
            var dataExchangeMessageLogMock = new Mock<IDataExchangeMessageLog>();
            dataExchangeMessageLogMock.SetupAllProperties();

            _dataExchangeImportQueueTransactionMock = new Mock<IDataExchangeQueueTransaction>();

            _dataExchangeImportQueueMock = new Mock<IDataExchangeQueue<DataExchangeImportMessage>>();
            _dataExchangeImportQueueMock.SetupAllProperties();

            _dataExchangeImportQueueFactoryMock = new Mock<IDataExchangeQueueFactory>();
            _dataExchangeImportQueueFactoryMock.Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>()))
                .Returns(_dataExchangeImportQueueTransactionMock.Object);
            _dataExchangeImportQueueFactoryMock.Setup(x => x.GetCustomImportQueue(It.IsAny<string>(), It.IsAny<string>()))
                .Returns(_dataExchangeImportQueueMock.Object);

            _serviceEventLoggerMock = new Mock<IServiceEventLogger>();
            _serviceEventLoggerMock.SetupAllProperties();

            _dataExchangeSettings = new DataExchangeSettings();

            _dataExchangeApiMock = new Mock<IDataExchangeApi>();
            _dataExchangeApiMock.SetupAllProperties();
            _dataExchangeApiMock.Setup(x => x.GetDataExchangeSettings()).Returns(_dataExchangeSettings);

            _eventLogModuleItemMock = new Mock<IEventLogModuleItem>();

            _powelImportModule = new StandardMsmqImportModule(_dataExchangeImportQueueFactoryMock.Object, () => _dataExchangeApiMock.Object, _serviceEventLoggerMock.Object);
        }

        [Test]
        public void RunThread_ModuleIsStarted_MachineNameForCustomImportQueueIsCorrect()
        {
            // Assign

            _dataExchangeSettings.ImportQueueMachineName = "dummymachinename";
            _powelImportModule = new StandardMsmqImportModule(_dataExchangeImportQueueFactoryMock.Object, () => _dataExchangeApiMock.Object, _serviceEventLoggerMock.Object);
            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueFactoryMock.Verify(x => x.GetCustomImportQueue("dummymachinename", It.IsAny<string>()));
        }

        [Test]
        public void RunThread_ModuleIsStarted_IdentifierForCustomImportQueueIsCorrect()
        {
            // Assign

            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueFactoryMock.Verify(x => x.GetCustomImportQueue(It.IsAny<string>(), "POWEL"));
        }

        [Test]
        public void ProcessImports_NoImportMessageIsAvailable_ImportMessageIsNotProcessed()
        {
            // Assign

            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.Never());
        }

        [Test]
        public void ProcessImports_ImportMessageIsAvailable_BeginIsCalledOnTransaction()
        {
            // Assign

            var messageToReceive = new DataExchangeImportMessage();
            messageToReceive.SetMessageData("DummyMessageData",null);
            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(messageToReceive);

            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueTransactionMock.Verify(x => x.Begin());
            messageToReceive.DeleteMessageData();
        }

        [Test]
        public void ProcessImports_ImportMessageIsAvailable_CommitIsCalledOnTransaction()
        {
            // Assign

            var messageToReceive = new DataExchangeImportMessage();
            messageToReceive.SetMessageData("DummyMessageData",null);
            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(messageToReceive);

            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueTransactionMock.Verify(x => x.Commit());
            messageToReceive.DeleteMessageData();
        }

        [Test]
        public void ProcessImports_NoImportMessageIsAvailable_BeginIsCalledOnTransaction()
        {
            // Assign

            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(default(DataExchangeImportMessage));

            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueTransactionMock.Verify(x => x.Begin());
        }

        [Test]
        public void ProcessImports_NoImportMessageIsAvailable_RollbackIsCalledOnTransaction()
        {
            // Assign

            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(default(DataExchangeImportMessage));

            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueTransactionMock.Verify(x => x.Rollback());
        }

        [Test]
        public void ProcessImports_NoImportMessageIsAvailable_CommitIsNotCalledOnTransaction()
        {
            // Assign

            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(default(DataExchangeImportMessage));

            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueTransactionMock.Verify(x => x.Commit(), Times.Never());
        }

        [Test]
        public void ProcessImports_ImportMessageIsAvailable_MessageDataIsCorrect()
        {
            // Assign
            const string msgDta = "DummyMessageData";
            var messageToReceive = new DataExchangeImportMessage();
            messageToReceive.SetMessageData(msgDta,null);
            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(messageToReceive);

            DataExchangeImportMessage messageReceived = null;

            _dataExchangeApiMock.Setup(x => x.EnqueueImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Callback<DataExchangeImportMessage, IDataExchangeQueueTransaction>((m, t) => { messageReceived = m; });
                
            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            Assert.AreEqual(msgDta, messageReceived.GetMessageData());
            messageToReceive.DeleteMessageData();
        }

        [Test]
        public void ImportMessageIsAvailableButImportFailsWithInvalidMessageException_MessageIsMovedToErrorQueueAndTransactionIsCommitted()
        {
            var messageToReceive = new DataExchangeImportMessage();
            messageToReceive.SetMessageData("DummyMessageData",null);
            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(messageToReceive);
            _dataExchangeApiMock.Setup(x => x.EnqueueImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Throws(new DataExchangeInvalidMessageException("1234"));

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            _dataExchangeApiMock.Verify(queue => queue.EnqueueErroneousImportMessage(messageToReceive, _dataExchangeImportQueueTransactionMock.Object), Times.Exactly(1));
            _dataExchangeImportQueueTransactionMock.Verify(x => x.Rollback(), Times.Never());
            _dataExchangeImportQueueTransactionMock.Verify(x => x.Commit(), Times.Exactly(1));
            messageToReceive.DeleteMessageData();
        }

        [Test]
        public void ImportMessageIsAvailableButImportFailsWithInternalError_MessageIsNOTMovedToErrorQueueButTransactionIsCommitted()
        {
            var messageToReceive = new DataExchangeImportMessage();
            messageToReceive.SetMessageData("DummyMessageData",null);
            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(messageToReceive);
            _dataExchangeApiMock.Setup(x => x.EnqueueImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Throws(new Exception("probably internal server error that nobody was expecting"));

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            _dataExchangeApiMock.Verify(queue => queue.EnqueueErroneousImportMessage(messageToReceive, _dataExchangeImportQueueTransactionMock.Object), Times.Never());
            _dataExchangeImportQueueTransactionMock.Verify(x => x.Rollback(), Times.Never());
            _dataExchangeImportQueueTransactionMock.Verify(x => x.Commit(), Times.Exactly(1));
            messageToReceive.DeleteMessageData();
        }

        [Test]
        public void ProcessImports_MachineNameIsLocal_TransactionIsOfTypeEnqueueAndDequeue()
        {
            // Assign

            _dataExchangeSettings.ImportQueueMachineName = ".";

            var messageToReceive = new DataExchangeImportMessage();
            messageToReceive.SetMessageData("DummyMessageData",null);
            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(messageToReceive);

            DataExchangeImportMessage messageReceived = null;

            _dataExchangeApiMock.Setup(x => x.EnqueueImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Callback<DataExchangeImportMessage, IDataExchangeQueueTransaction>((m, t) => { messageReceived = m; });
                
            // Act

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueFactoryMock.Verify(x => x.GetTransaction(DataExchangeQueueTransactionType.EnqueueAndDequeue));
            messageToReceive.DeleteMessageData();
        }

        [Test]
        public void ProcessImports_MachineNammeIsRemote_TransactionIsOfTypeEnqueueAndRemoteDequeue()
        {
            // Assign
            
            _dataExchangeSettings.ImportQueueMachineName = "dummymachine";

            var messageToReceive = new DataExchangeImportMessage();
            messageToReceive.SetMessageData("DummyMessageData",null);
            _dataExchangeImportQueueMock.Setup(x => x.Dequeue(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(messageToReceive);

            DataExchangeImportMessage messageReceived = null;

            _dataExchangeApiMock.Setup(x => x.EnqueueImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Callback<DataExchangeImportMessage, IDataExchangeQueueTransaction>((m, t) => { messageReceived = m; });
                
            // Act

            _powelImportModule = new StandardMsmqImportModule(_dataExchangeImportQueueFactoryMock.Object, () => _dataExchangeApiMock.Object, _serviceEventLoggerMock.Object);

            _powelImportModule.Start();
            _powelImportModule.Stop(Defaults.DefaultModuleStopTimeout);

            // Assert

            _dataExchangeImportQueueFactoryMock.Verify(x => x.GetTransaction(DataExchangeQueueTransactionType.EnqueueAndRemoteDequeue));
            messageToReceive.DeleteMessageData();
        }
    }
}
