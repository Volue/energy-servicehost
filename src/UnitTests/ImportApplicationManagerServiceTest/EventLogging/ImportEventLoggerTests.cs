using System;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging;


namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerServiceTest.EventLogging
{
    [TestFixture]
    public class ImportEventLoggerTests
    {
        private const int IMPORT_SUCCESS = 8001;
        private const int IMPORT_FAILURE = 8002;

        private Mock<IServiceEventLogger> _mockEventLogger;
        private ImportEventLogger _eventLogger;

        [SetUp]
        public void SetUp()
        {
            _mockEventLogger = new Mock<IServiceEventLogger>();
            _eventLogger = new ImportEventLogger(_mockEventLogger.Object);
        }

        [Test]
        public void LogSuccessfulImport_ValidMessage_CallsEventLog()
        {
            Execute_And_VerifyCalls(_eventLogger.LogSuccessfulImport, IMPORT_SUCCESS);
        }

        [Test]
        public void LogFailedImport_ValidMessage_CallsEventLog()
        {
            Execute_And_VerifyCalls(_eventLogger.LogFailedImport, IMPORT_FAILURE);
        }

        [Test]
        public void LogSuccessfulImport_ValidMessageWithoutFormat_CallsEventLog()
        {
            Execute_And_VerifyCalls_MessageWithoutFormat(_eventLogger.LogSuccessfulImport, IMPORT_SUCCESS);
        }

        [Test]
        public void LogFailedImport_ValidMessageWithoutFormat_CallsEventLog()
        {
            Execute_And_VerifyCalls_MessageWithoutFormat(_eventLogger.LogFailedImport, IMPORT_FAILURE);
        }

        [Test]
        public void LogSuccessfulImport_ValidMessageWithoutSenderReciverAndReference_CallsEventLog()
        {
            Execute_And_VerifyCalls_ValidMessageWithoutSenderReciverAndReference(_eventLogger.LogSuccessfulImport, IMPORT_SUCCESS);
        }

        [Test]
        public void LogFailedImport_ValidMessageWithoutSenderReciverAndReference_CallsEventLog()
        {
            Execute_And_VerifyCalls_ValidMessageWithoutSenderReciverAndReference(_eventLogger.LogFailedImport, IMPORT_FAILURE);
        }

        private void Execute_And_VerifyCalls(Action<DataExchangeImportMessage> loggingMethod, int expectedMessageType)
        {
            DataExchangeImportMessage message = new DataExchangeImportMessage
            {
                ExternalReference = "test1",
                SenderName = "test2",
                ReceiverName = "test3",
                Format = "test4"
            };

            loggingMethod(message);

            _mockEventLogger
                .Verify(m => m.LogMessage(expectedMessageType, "test1", "test2", "test3", "test4"), Times.Exactly(1));
        }

        private void Execute_And_VerifyCalls_MessageWithoutFormat(Action<DataExchangeImportMessage> loggingMethod, int expectedMessageType)
        {
            DataExchangeImportMessage message = new DataExchangeImportMessage
            {
                ExternalReference = "test1",
                SenderName = "test2",
                ReceiverName = "test3",
                Protocol = "test99"
            };

            loggingMethod(message);

            _mockEventLogger
                .Verify(m => m.LogMessage(expectedMessageType, "test1", "test2", "test3", "test99"), Times.Exactly(1));
        }

        private void Execute_And_VerifyCalls_ValidMessageWithoutSenderReciverAndReference(Action<DataExchangeImportMessage> loggingMethod, int expectedMessageType)
        {
            DataExchangeImportMessage message = new DataExchangeImportMessage
            {
                ExternalReference = null,
                SenderName = null,
                ReceiverName = null,
                Protocol = "test99"
            };

            loggingMethod(message);

            _mockEventLogger
                .Verify(m => m.LogMessage(expectedMessageType, string.Empty, string.Empty, string.Empty, "test99"), Times.Exactly(1));
        }
    }
}
