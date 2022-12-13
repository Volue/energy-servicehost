using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class StatusChangerTest
    {
        private Mock<IDataExchangeMessageLog> _dataExchangeMessageLog;
        private Mock<IServiceEventLogger> _serviceEventLog;

        [SetUp]
        public void SetUp()
        {
            _dataExchangeMessageLog = new Mock<IDataExchangeMessageLog>();
            _serviceEventLog = new Mock<IServiceEventLogger>();
        }

        [TestCase(1111, 13, TransLogMessageStatus.ExportSentReceivedOk)]
        [TestCase(2222, 13, TransLogMessageStatus.ExportSentReceivedError)]
        [TestCase(2222, 19, TransLogMessageStatus.ExportSentReceivedOk)]
        [TestCase(2222, 19, TransLogMessageStatus.ExportSentReceivedError)]
        [TestCase(3333, 22, TransLogMessageStatus.ExportTransferredProcessingOk)]
        [TestCase(3333, 22, TransLogMessageStatus.ExportTransferredError)]
        [TestCase(3333, 22, TransLogMessageStatus.ExportSentReceivedOk)]
        [TestCase(3333, 22, TransLogMessageStatus.ExportSentReceivedError)]
        public void TryChangeStatus_RestrictionFulfilled_SetStatusIsCalled(long transactionId,
            int currentMessageStatusId, TransLogMessageStatus newMessageStatus)
        {
            _dataExchangeMessageLog.Setup(x => x.GetStatus(transactionId))
                .Returns(currentMessageStatusId);

            var newStatusChangeArgs = new StatusChangeEventArgs(newMessageStatus, transactionId);

            var statusChanger = new StatusChanger(_dataExchangeMessageLog.Object, _serviceEventLog.Object);
            statusChanger.TryChangeStatus(newStatusChangeArgs);

            _dataExchangeMessageLog.Verify(x => x.SetStatus(transactionId, newMessageStatus));
        }

        [TestCase(1111, 13, TransLogMessageStatus.ImportError)]
        [TestCase(2222, 19, TransLogMessageStatus.ImportFailed)]
        [TestCase(3333, 22, TransLogMessageStatus.ImportSuccessful)]
        public void TryChangeStatus_RestrictionNotFulfilled_SetStatusIsNotCalled(long transactionId,
            int currentMessageStatusId, TransLogMessageStatus newMessageStatus)
        {
            _dataExchangeMessageLog.Setup(x => x.GetStatus(transactionId))
                .Returns(currentMessageStatusId);

            var newStatusChangeArgs = new StatusChangeEventArgs(newMessageStatus, transactionId);

            var statusChanger = new StatusChanger(_dataExchangeMessageLog.Object, _serviceEventLog.Object);
            statusChanger.TryChangeStatus(newStatusChangeArgs);

            _dataExchangeMessageLog.Verify(x => x.SetStatus(transactionId, newMessageStatus), Times.Exactly(0));
        }

        [TestCase(14)]
        [TestCase(15)]
        [TestCase(16)]
        [TestCase(17)]
        [TestCase(24)]
        public void TryChangeStatus_NotAllowedToChangeStatuses_SetStatusIsNotCalled(int currentMessageStatusId)
        {
            const long irrelewantTransactionId = 666;
            const TransLogMessageStatus irrelewantNewMessageStatus =
                TransLogMessageStatus.ExportSentReceivedOk;

            _dataExchangeMessageLog.Setup(x => x.GetStatus(irrelewantTransactionId))
                .Returns(currentMessageStatusId);

            var newIrrelewantStatusChangeArgs = new StatusChangeEventArgs(irrelewantNewMessageStatus,
                irrelewantTransactionId);

            var statusChanger = new StatusChanger(_dataExchangeMessageLog.Object, _serviceEventLog.Object);
            statusChanger.TryChangeStatus(newIrrelewantStatusChangeArgs);

            _dataExchangeMessageLog.Verify(x => x.SetStatus(It.IsAny<long>(), It.IsAny<TransLogMessageStatus>()),
                Times.Exactly(0));
        }

        [TestCase(11)]
        [TestCase(12)]
        [TestCase(18)]
        [TestCase(20)]
        [TestCase(21)]
        [TestCase(23)]
        public void TryChangeStatus_RestrictionDoesntExists_SetStatusIsCalled(int currentMessageStatusId)
        {
            const long irrelewantTransactionId = 666;
            const TransLogMessageStatus irrelewantNewMessageStatus =
                TransLogMessageStatus.ExportSentReceivedOk;

            _dataExchangeMessageLog.Setup(x => x.GetStatus(It.IsAny<long>()))
                .Returns(currentMessageStatusId);

            var newStatusChangeArgs = new StatusChangeEventArgs(irrelewantNewMessageStatus, irrelewantTransactionId);

            var statusChanger = new StatusChanger(_dataExchangeMessageLog.Object, _serviceEventLog.Object);
            statusChanger.TryChangeStatus(newStatusChangeArgs);

            _dataExchangeMessageLog.Verify(x => x.SetStatus(irrelewantTransactionId, irrelewantNewMessageStatus));
        }

        [TestCase(1111, 13, TransLogMessageStatus.ExportSentReceivedOk)]
        public void TryChangeStatus_NextStatusIsNotError_LogMessageIsNotCalled(long transactionId,
            int currentMessageStatusId,
            TransLogMessageStatus newMessageStatus)
        {
            _dataExchangeMessageLog.Setup(x => x.GetStatus(transactionId))
                .Returns(currentMessageStatusId);

            var newStatusChangeArgs = new StatusChangeEventArgs(newMessageStatus, transactionId);

            var statusChanger = new StatusChanger(_dataExchangeMessageLog.Object, _serviceEventLog.Object);
            statusChanger.TryChangeStatus(newStatusChangeArgs);

            _serviceEventLog.Verify(
                x =>x.LogMessage(30213, It.IsAny<string>(), It.IsAny<string>(), It.IsAny<string>()), Times.Exactly(0));
        }

        [TestCase(1111, 13, TransLogMessageStatus.ExportTransferredError)]
        [TestCase(1111, 15, TransLogMessageStatus.ExportSentReceivedError)]
        public void TryChangeStatus_NextStatusIsError_LogMessageIsCalled(long transactionId,
            int currentMessageStatusId,
            TransLogMessageStatus newMessageStatus)
        {
            const string messagReceiver = "MessageReceiver";

            _dataExchangeMessageLog.Setup(x => x.GetStatus(transactionId))
                .Returns(currentMessageStatusId);
            _dataExchangeMessageLog.Setup(x => x.GetMessageReceiver(transactionId))
                .Returns(messagReceiver);

            var newStatusChangeArgs = new StatusChangeEventArgs(newMessageStatus, transactionId);

            var statusChanger = new StatusChanger(_dataExchangeMessageLog.Object, _serviceEventLog.Object);
            statusChanger.TryChangeStatus(newStatusChangeArgs);

            _serviceEventLog.Verify(
                x =>
                    x.LogMessage(30213, ((TransLogMessageStatus)currentMessageStatusId).ToString(),
                        newMessageStatus.ToString(), messagReceiver));
        }
    }
}