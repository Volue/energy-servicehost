using System;
using System.Threading;
using Moq;
using NUnit.Framework;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Exporters
{
    [TestFixture]
    public class MessageExporterTest
    {
        private MessageExporter _messageExporter;
        private Mock<IDataExchangeQueueTransaction> _dataExchangeApiTransactionMock;
        private Mock<IDataExchangeApi> _dataExchangeApiMock;

        private string _errorMessage;
        private string _externalReference;
        private bool _sendExportMessageException;

        private DataExchangeExportMessage _messageToExport;
        private Mock<IServiceEventLogger> _serviceEventLoggerMock;
        private Mock<IDataExchangeMessageLog> _dataExchangeMessageLogMock;

        private string SendExportMessage(DataExchangeExportMessage exportMessage)
        {
            if (_sendExportMessageException)
            {
                throw new Exception(_errorMessage);
            }

            return _externalReference;
        }

        private string SendExportMessageThatThrowsInvalidMessageException(DataExchangeExportMessage exportMessage)
        {
            throw new DataExchangeInvalidMessageException("1234");
        }

        [SetUp]
        public void SetUp()
        {
            _dataExchangeMessageLogMock = new Mock<IDataExchangeMessageLog>();
            _dataExchangeMessageLogMock.SetupAllProperties();

            _dataExchangeApiTransactionMock = new Mock<IDataExchangeQueueTransaction>();

            _dataExchangeApiMock = new Mock<IDataExchangeApi>();
            _dataExchangeApiMock.Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>()))
                .Returns(_dataExchangeApiTransactionMock.Object);

            _errorMessage = "";
            _externalReference = "";
            _sendExportMessageException = false;

            _messageToExport = new DataExchangeExportMessage
                {
                    MessageLogId = 1234,
                    ReceiverName = "DummyReceiver",
                    RoutingAddress = "DummyRoutingAddress"
                };
            _messageToExport.SetMessageData("DummyMessageData",null);
            _serviceEventLoggerMock = new Mock<IServiceEventLogger>();
            _serviceEventLoggerMock.SetupAllProperties();

            _messageExporter = new MessageExporter(_dataExchangeApiMock.Object, _serviceEventLoggerMock.Object, _dataExchangeMessageLogMock.Object, MessageExporterSettings.Default);
        }

        [Test]
        public void ExportMessage_ExportMessageIsAvailable_ExportMessageIsDequeued()
        {
            // Assign

            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            // Act
            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            // Assert

            _dataExchangeApiMock.Verify(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()));
        }

        [Test]
        public void ExportMessage_ExportMessageIsAvailable_BeginIsCalledOnTransaction()
        {
            // Assign

            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            // Act
            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Begin());
        }

        [Test]
        public void ExportMessage_ExportMessageIsAvailableAndSuccessfullySent_CommitIsCalledOnTransaction()
        {
            // Assign

            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            // Act
            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
        }

        [Test]
        public void ExportMessage_ExportMessageIsAvailableButNotSuccessfullySentDueToUnknownError_CommitIsCalledOnTransaction_MessageMovedToLowQueue()
        {
            // ToDo: We should have a Retries-counter in the ExportMessage which we can test against a limit on retries and discard the message.
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            _sendExportMessageException = true;

            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            // Message is Dequeued and Enqueued to LOW queue.
            _dataExchangeApiTransactionMock.Verify(x => x.Commit(), Times.Exactly(1));
            _dataExchangeApiTransactionMock.Verify(x => x.Rollback(), Times.Never());
            _dataExchangeApiMock.Verify(x => x.EnqueueExportMessage(It.IsAny<DataExchangeExportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.Exactly(1));
        }

        [Test]
        public void ExportMessage_ExportMessageIsAvailableButNotSuccessfullySentDueToInvalidMessageException_CommitIsCalledOnTransaction()
        {
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessageThatThrowsInvalidMessageException);

            _dataExchangeApiTransactionMock.Verify(x => x.Commit(), Times.Exactly(1));
            _dataExchangeApiTransactionMock.Verify(x => x.Rollback(), Times.Never());
        }

        [Test]
        public void ExportMessage_ExportMessageIsNotAvailable_BeginIsCalledOnTransaction()
        {
            // Assign

            _messageToExport = null;
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            // Act
            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Begin());
        }

        [Test]
        public void ExportMessage_ExportMessageIsNotAvailable_CommitIsNotCalledOnTransaction()
        {
            // Assign

            _messageToExport = null;
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            // Act
            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Commit(), Times.Never());
        }

        // adsi: I wounder why it should not be called?
        [Test]
        public void ExportMessage_ExportMessageIsNotAvailable_RollbackIsNotCalledOnTransaction()
        {
            _messageToExport = null;
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            _dataExchangeApiTransactionMock.Verify(x => x.Rollback(), Times.Never());
        }

        [Test]
        public void ExportMessage_ExportMessageIsAvailable_TheTransactionIsOfTypeDequeue()
        {
            // Assign

            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            // Act
            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            // Assert

            _dataExchangeApiMock.Verify(x => x.GetTransaction(DataExchangeQueueTransactionType.Dequeue));
        }

        [Test]
        public void ExportMessage_ExportMessageIsAvailableButNotSuccessfullySent_LogErrorIsCalled()
        {
            // Assign

            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            _sendExportMessageException = true;
            _errorMessage = "DummyErrorMessage";

            // Act
            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            // Assert

            _serviceEventLoggerMock.Verify(x => x.LogMessage(8104, "DummyReceiver", "DummyRoutingAddress", "DummyErrorMessage"));
        }

        [Test]
        public void ExportMessageForSameMessageMultipleTimes_ExportMessageIsAvailableButNotSuccessfullySent_StartedIsChanged()
        {
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            _sendExportMessageException = true;
            _errorMessage = "DummyErrorMessage";
            const int howOftenWeShouldLogFailureForTheSameMessageInSeconds = 1;
            const int count = 4;

            _messageExporter = new MessageExporter(_dataExchangeApiMock.Object, _serviceEventLoggerMock.Object, _dataExchangeMessageLogMock.Object,
                MessageExporterSettings.Custom(howOftenWeShouldLogFailureForTheSameMessageInSeconds));

            for (int i = 0; i < count; i++)
            {
                _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

                // wait but not as long as the howOftenWeShouldLogFailureForTheSameMessageInSeconds to test behaviour
                Thread.Sleep(TimeSpan.FromSeconds(howOftenWeShouldLogFailureForTheSameMessageInSeconds / 2.0));
            }

            _serviceEventLoggerMock.Verify(eventLog =>
                eventLog.LogMessage(8104, _messageToExport.ReceiverName, _messageToExport.RoutingAddress, _errorMessage),
                Times.Exactly(count / 2));
        }

        [Test]
        public void ExportMessageForSameMessageMultipleTimes_ExportMessageIsAvailableButNotSuccessfullySent_ErrorLoggedToEventLogOnlyOnce()
        {
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            _sendExportMessageException = true;
            _errorMessage = "DummyErrorMessage";

            for (int i = 0; i < 10; i++)
            {
                _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);
            }
            
            _serviceEventLoggerMock.Verify(eventLog => 
                eventLog.LogMessage(8104, _messageToExport.ReceiverName, _messageToExport.RoutingAddress, _errorMessage),
                Times.Exactly(1));
        }

        [Test]
        public void ExportMessage_ExportMessageIsAvailableButNotSuccessfullySent_TimeForRetryHasPassed()
        {
            // Assign

            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            _sendExportMessageException = true;
            _errorMessage = "DummyErrorMessage";

            // Act
            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessage);

            // Assert
            _serviceEventLoggerMock.Verify(x => x.LogMessage(8104, "DummyReceiver", "DummyRoutingAddress", "DummyErrorMessage"));
        }

        [Test]
        public void ExportMessage_InvalidExportMessageIsAvailable_ExportMessageIsDiscardedAndItsStatusInMessageLogIsSetToxportFailedDuringCreation()
        {
            _dataExchangeApiMock.Setup(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()))
                .Returns(_messageToExport);

            _messageExporter.ExportMessage("", TimeSpan.FromSeconds(1), SendExportMessageThatThrowsDataExchangeInvalidMessageException);

            _dataExchangeApiTransactionMock.Verify(x => x.Begin());
            _dataExchangeApiMock.Verify(x => x.DequeueExportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()));
            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
            _dataExchangeMessageLogMock
                .Verify(ml => ml.SetStatus(123, TransLogMessageStatus.ExportFailedDuringCreation), Times.Exactly(1));
        }

        private string SendExportMessageThatThrowsDataExchangeInvalidMessageException(DataExchangeExportMessage arg)
        {
            throw new DataExchangeInvalidMessageException("123");
        }
    }
}
