using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApiTest
{
    [TestFixture]
    public class DataExchangeAPITest
    {
        private DataExchangeAPI _dataExchangeApi;
        private Mock<IDataExchangeQueueTransaction> _dataExchangeQueueTransactionMock;
        private DataExchangeSettings _dataExchangeSettings;
        private Mock<IDataExchangeFileWriter> _dataExchangeFileWriterMock;
        private Mock<IDataExchangeMessageLog> _dataExchangeMessageLogMock;
        private Mock<IDataExchangeMetaData> _dataExchangeMetaDataMock;

        [SetUp]
        public void SetUp()
        {
            _dataExchangeSettings = new DataExchangeSettings
                {
                    ImportQueueMachineName = ".",
                    ExportQueueMachineName = ".",
                    StoreExportMessages = false
                };

            var dataExchangeSettingsFactory = new Mock<IDataExchangeSettingsFactory>();
            dataExchangeSettingsFactory.Setup(x => x.GetSettings()).Returns(_dataExchangeSettings);

            _dataExchangeQueueTransactionMock = new Mock<IDataExchangeQueueTransaction>();
            _dataExchangeQueueTransactionMock.SetupAllProperties();

            _dataExchangeMessageLogMock = new Mock<IDataExchangeMessageLog>();
            _dataExchangeFileWriterMock = new Mock<IDataExchangeFileWriter>();
            _dataExchangeMetaDataMock = new Mock<IDataExchangeMetaData>();

            var serviceEventLoggerMock = new Mock<IServiceEventLogger>();
            _dataExchangeApi = new DataExchangeAPI(dataExchangeSettingsFactory.Object, _dataExchangeMessageLogMock.Object, _dataExchangeFileWriterMock.Object, _dataExchangeMetaDataMock.Object, serviceEventLoggerMock.Object);
        }

        [Test]
        public void EnqueueExportMessage_StoreExportMessagesIsTrue_ExportMessageIsStored()
        {
            // Assign
            var message = new DataExchangeExportMessage
                {
                    Priority = "NORMAL",
                    RoutingAddress = "STANDARDMSMQ:dummyaddress:1234"
                };
            _dataExchangeSettings.StoreExportMessages = true;

            // Act
            _dataExchangeApi.EnqueueExportMessage(message, _dataExchangeApi.GetTransaction(_dataExchangeApi.GetDataExchangeSettings().ExportQueueMachineName));

            // Assert
            _dataExchangeFileWriterMock.Verify(x => x.LogExportMessageToFile(It.IsAny<DataExchangeExportMessage>()));
        }

        [Test]
        public void EnqueueExportMessage_StoreExportMessagesIsFalse_ExportMessageIsNotStored()
        {
            // Assign
            var message = new DataExchangeExportMessage
                {
                    Priority = "NORMAL",
                    RoutingAddress = "STANDARDMSMQ:dummyaddress:1234"
                };

            // Act
            _dataExchangeApi.EnqueueExportMessage(message, _dataExchangeApi.GetTransaction(_dataExchangeApi.GetDataExchangeSettings().ExportQueueMachineName));

            // Assert
            _dataExchangeFileWriterMock.Verify(x => x.LogExportMessageToFile(It.IsAny<DataExchangeExportMessage>()), Times.Never());
        }

        [Test]
        public void EnqueueExportMessage_MessageIsEnqueuedWithIdGreaterThanZero_StatusIsUpdatedInMessageLog()
        {
            // Assign
            var message = new DataExchangeExportMessage
                {
                    Priority = "HIGH",
                    RoutingAddress = "STANDARDMSMQ:dummyaddress:1234",
                    MessageLogId = 3
                };

            // Act
            _dataExchangeApi.EnqueueExportMessage(message, _dataExchangeApi.GetTransaction(_dataExchangeApi.GetDataExchangeSettings().ExportQueueMachineName));

            // Assert
            _dataExchangeMessageLogMock.Verify(x => x.SetStatusToExportEnqueued(3));
        }

        [Test]
        public void EnqueueExportMessage_MessageIsEnqueuedWithZeroId_StatusIsNotUpdatedInMessageLog()
        {
            // Assign
            var message = new DataExchangeExportMessage
                {
                    Priority = "HIGH",
                    RoutingAddress = "STANDARDMSMQ:dummyaddress:1234",
                    MessageLogId = 0,
                    Protocol = "DUMMYPROTOCOL"
                };

            // Act
            _dataExchangeApi.EnqueueExportMessage(message, _dataExchangeApi.GetTransaction(_dataExchangeApi.GetDataExchangeSettings().ExportQueueMachineName));

            // Assert
            _dataExchangeMessageLogMock.Verify(x => x.SetStatusToExportEnqueued(It.IsAny<int>()), Times.Never());
        }
    }
}
