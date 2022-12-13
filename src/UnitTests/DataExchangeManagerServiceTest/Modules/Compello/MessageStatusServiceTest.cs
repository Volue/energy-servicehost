using EDI.Server.API.Client.MessageServiceReference;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class MessageStatusServiceTest
    {
        [TestCase(1, TransactionStatus.Unprocessed, TransLogMessageStatus.ExportTransferredProcessingStarted)]
        [TestCase(2, TransactionStatus.Collected, TransLogMessageStatus.ExportTransferredProcessingStarted)]
        [TestCase(3, TransactionStatus.WaitingForExecution,
            TransLogMessageStatus.ExportTransferredProcessingStarted)]
        [TestCase(4, TransactionStatus.Executed, TransLogMessageStatus.ExportTransferredProcessingStarted)]
        [TestCase(5, TransactionStatus.Delivered, TransLogMessageStatus.ExportSentToRemoteSystem)]
        [TestCase(6, TransactionStatus.Ok, TransLogMessageStatus.ExportTransferredProcessingOk)]
        [TestCase(7, TransactionStatus.OkManual, TransLogMessageStatus.ExportTransferredProcessingOk)]
        [TestCase(8, TransactionStatus.Waiting, TransLogMessageStatus.ExportSentWaitingForAcknowledgement)]
        [TestCase(9, TransactionStatus.WaitingManual, TransLogMessageStatus.ExportSentWaitingForAcknowledgement)]
        [TestCase(10, TransactionStatus.Failed, TransLogMessageStatus.ExportTransferredError)]
        [TestCase(11, TransactionStatus.FailedManual, TransLogMessageStatus.ExportTransferredError)]
        public void CreateArguments_ConvertTransactionStatusIntoMessageStatus(long transactionId,
            TransactionStatus transactionStatus,
            TransLogMessageStatus messageStatus)
        {
            var translatedMessageStatus = MessageStatusService.CreateArguments(transactionStatus, transactionId);

            Assert.AreEqual(translatedMessageStatus.Status, messageStatus);
            Assert.AreEqual(translatedMessageStatus.TransactionId, transactionId);
        }
    }
}