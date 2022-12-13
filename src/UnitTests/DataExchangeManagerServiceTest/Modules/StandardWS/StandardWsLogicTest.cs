using System;
using System.Collections.Generic;
using Moq;
using NUnit.Framework;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.StandardWS
{
    [TestFixture]
    public class StandardWsLogicTest
    {
        private StandardWsLogic _logic;
        private Mock<IDataExchangeApi> _apiMock;
        private Mock<IDataExchangeMessageLog> _messageLogMock;
        private Mock<IServiceEventLogger> _serviceEventLoggerMock;
        private Mock<IDataExchangeQueueTransaction> _transactionMock;
        private Mock<IExternalEventLogger> _externalEventLoggerMock;

        [SetUp]
        public void SetUp()
        {
            _transactionMock = new Mock<IDataExchangeQueueTransaction>();
            _apiMock = new Mock<IDataExchangeApi>();
            _apiMock.Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>())).Returns(_transactionMock.Object);

            _messageLogMock = new Mock<IDataExchangeMessageLog>();

            _serviceEventLoggerMock = new Mock<IServiceEventLogger>();
            _serviceEventLoggerMock.SetupAllProperties();

            _externalEventLoggerMock = new Mock<IExternalEventLogger>();
            _externalEventLoggerMock.SetupAllProperties();

            _logic = new StandardWsLogic(() => _apiMock.Object, _messageLogMock.Object, _serviceEventLoggerMock.Object, _externalEventLoggerMock.Object);
        }

        [Test]
        public void SubmitImport_SendMessageWithLowPriority_MessageIsSubmittedToDataExchangeApiWithLowPriority()
        {
            // Assign

            var metadata = new Dictionary<string, string> { { "priority", "low" }, { "protocol", "dummy" } };
            const string importdata = "data123";

            DataExchangeImportMessage enqueuedDataExchangeImportMessage = null;
            _apiMock.Setup(x => x.EnqueueImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Callback<DataExchangeImportMessage, IDataExchangeQueueTransaction>((m, t) =>
                    {
                        enqueuedDataExchangeImportMessage = m;
                    });

            // Act

            _logic.SubmitImport(metadata, importdata, "externalreference");

            // Assert

            Assert.AreEqual("low", enqueuedDataExchangeImportMessage.Priority);
        }

        [Test]
        public void SubmitImport_ValidRequest_ReturnsValidResponse()
        {
            // Assign

            var metadata = new Dictionary<string, string> { { "priority", "normal" }, { "protocol", "dummy" } };
            const string importdata = "data123";
            const string externalReference = "reference123";

            // Act

            SubmitImportResponse response = _logic.SubmitImport(metadata, importdata, externalReference);

            // Assert

            Assert.IsNotNull(response);
            Assert.IsTrue(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Null.Or.Empty);
        }

        [Test]
        public void SubmitImport_ReferenceIsNull_ReturnsRequestNotOk()
        {
            // Assign

            var metadata = new Dictionary<string, string> { { "priority", "normal" }, { "protocol", "dummy" } };
            const string importdata = "data123";
            const string externalReference = null;

            // Act

            var response = _logic.SubmitImport(metadata, importdata, externalReference);

            // Assert

            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
        }

        [Test]
        public void SubmitImport_ReferenceIsEmptyString_ReturnsRequestNotOk()
        {
            // Assign

            var metadata = new Dictionary<string, string> { { "priority", "normal" }, { "protocol", "dummy" } };
            const string importdata = "data123";
            const string externalReference = "";

            // Act

            var response = _logic.SubmitImport(metadata, importdata, externalReference);

            // Assert

            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
        }

        [Test]
        public void SubmitImport_NullMetadataAndNullImportdata_ReturnsNakResponse()
        {
            // Assign

            const string importdata = null;
            // Act

            SubmitImportResponse response = _logic.SubmitImport(null, importdata, "externalreference");

            // Assert

            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
        }


        [Test]
        public void SubmitImport_NoPriority_ReturnsNakResponse()
        {
            // Assign

            var metadata = new Dictionary<string, string> { { "protocol", "dummy" } };
            const string importdata = "data123";

            // Act

            SubmitImportResponse response = _logic.SubmitImport(metadata, importdata, "externalreference");

            // Assert

            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
        }

        [Test]
        public void SubmitImport_InvalidPriority_ReturnsValidResponse()
        {
            // Assign

            var metadata = new Dictionary<string, string> { { "priority", "dummy" }, { "protocol", "dummy" } };
            const string importdata = "data123";

            // Act

            var response = _logic.SubmitImport(metadata, importdata, "externalreference");

            // Assert

            Assert.IsNotNull(response);
            Assert.IsTrue(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Null.Or.Empty);
        }

        [Test]
        public void SubmitImport_NoProtocol_ReturnsNakResponse()
        {
            // Assign

            var metadata = new Dictionary<string, string> { { "priority", "normal" } };
            const string importdata = "data123";
            const string externalReference = "transaction123";

            // Act

            var response = _logic.SubmitImport(metadata, importdata, externalReference);

            // Assert

            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
        }

        [Test]
        public void SubmitImport_EmptyProtocol_ReturnsNakResponse()
        {
            // Assign

            var metadata = new Dictionary<string, string> { { "priority", "normal" }, { "protocol", string.Empty } };
            const string importdata = "data123";
            const string externalReference = "transaction123";

            // Act

            var response = _logic.SubmitImport(metadata, importdata, externalReference);

            // Assert

            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
        }

        [Test]
        public void SendExportsAck_ValidRequest_ReturnValidResponse()
        {
            // Valid id
            const string externalReference = "5";    

            // Assign
            var internalMessageId = "123";
            var dataExchangeExportMessage = new DataExchangeExportMessage {ExternalReference = externalReference};
            _apiMock.Setup(x => x.PeekExportMessageByExternalReference(out internalMessageId, It.IsAny<string>(), It.IsAny<string>())).Returns(dataExchangeExportMessage);

            // Act
            var response = _logic.SendExportAck(externalReference);
            
            // Assert
            Assert.IsTrue(response.RequestOk);
        }

        [Test]
        public void SendExportsAck_InvalidRequest_ReturnInvalidResponse()
        {
            // Valid id
            const string externalReference = "5";

            // Assign
            var internalMessageId = "123";
            var dataExchangeExportMessage = new DataExchangeExportMessage { ExternalReference = "4" };
            _apiMock.Setup(x => x.PeekExportMessage(out internalMessageId, It.IsAny<TimeSpan>(), It.IsAny<string>())).Returns(dataExchangeExportMessage);

            // Act
            var response = _logic.SendExportAck(externalReference);

            // Assert
            Assert.IsFalse(response.RequestOk);
        }

        [Test]
        public void SendExportsAck_ValidRequest_ExportMessageIsRemovedUsingADequeueTransaction()
        {
            // Valid id
            const string externalReference = "5";

            // Assign
            var internalMessageId = "123";
            var dataExchangeExportMessage = new DataExchangeExportMessage { ExternalReference = externalReference };
            _apiMock.Setup(x => x.PeekExportMessageByExternalReference(out internalMessageId, It.IsAny<string>(), It.IsAny<string>())).Returns(dataExchangeExportMessage);

            // Act
            _logic.SendExportAck(externalReference);

            // Assert

            _apiMock.Verify(x => x.GetTransaction(DataExchangeQueueTransactionType.Dequeue));
        }

        [Test]
        public void GetExport_ValidRequest_ReturnValidResponse()
        {
            // Assign
            const string msgDta = "data123";
            DataExchangeExportMessage exportMessage = new DataExchangeExportMessage();
            exportMessage.SetMessageData(msgDta,null);
            string internalMessageId = "1";
            _apiMock.Setup(x => x.PeekExportMessage(out internalMessageId, It.IsAny<TimeSpan>(), It.IsAny<string>())).Returns(exportMessage);

            // Act
            var response = _logic.GetExport(1);

            // Assert
            Assert.IsNotNull(response);
            Assert.IsTrue(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Null.Or.Empty);
            Assert.IsNotNull(response.ExportMessage);
            Assert.AreEqual(msgDta, response.ExportMessage.ExportData);
            exportMessage.DeleteMessageData();
        }

        [Test]
        public void GetExport_InalidRequest_ReturnNotOkResponse()
        {
            // Assign
            
            // Act
            var response = _logic.GetExport(-1);

            // Assert
            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
            Assert.IsNull(response.ExportMessage);
        }

        [Test]
        public void SetMessageStatus_ValidStatusAndStatusText_MessageStatusAndStatusTextIsUpdated()
        {
            // Valid id
            const string externalReference = "dummy-external-reference";

            // Act

            _logic.SetMessageStatus(externalReference, MessageStatusTypeEnum.ExportTransferredProcessingOk, "Dummy status text");

            // Assert

            _messageLogMock.Verify(x => x.SetStatusAndStatusText(externalReference, TransLogMessageStatus.ExportTransferredProcessingOk, "Dummy status text"));
        }

        [Test]
        public void SetMessageStatus_ValidStatusAndStatusText_RequestIsOk()
        {
            // Assign

            // Act

            var response = _logic.SetMessageStatus("dummy-external-reference", MessageStatusTypeEnum.ExportTransferredProcessingOk, "Dummy status text");

            // Assert

            Assert.IsTrue(response.RequestOk);
        }

        [Test]
        public void SetMessageStatus_ValidStatusAndEmptyStatusText_MessageStatusAndStatusTextIsUpdated()
        {
            const string externalReference = "dummy-external-reference";

            // Assign

            // Act

            _logic.SetMessageStatus(externalReference, MessageStatusTypeEnum.ExportTransferredProcessingOk, "");

            // Assert

            _messageLogMock.Verify(x => x.SetStatusAndStatusText(externalReference, TransLogMessageStatus.ExportTransferredProcessingOk, ""));
        }

        [Test]
        public void SetMessageStatus_ValidStatusAndEmptyStatusText_RequestIsOk()
        {
            // Assign

            // Act

            var response = _logic.SetMessageStatus("dummy-external-reference", MessageStatusTypeEnum.ExportTransferredProcessingOk, "");

            // Assert

            Assert.IsTrue(response.RequestOk);
        }

        [Test]
        public void SetMessageStatus_ValidStatusAndNullStatusText_MessageStatusIsUpdated_ButStatusTextIsNotUpdated()
        {
            const string externalReference = "dummy-external-reference";

            _logic.SetMessageStatus(externalReference, MessageStatusTypeEnum.ExportTransferredProcessingOk, null);

            _messageLogMock.Verify(x => x.SetStatus(externalReference, TransLogMessageStatus.ExportTransferredProcessingOk), Times.Exactly(1));
            _messageLogMock.Verify(x => x.SetStatusAndStatusText(It.IsAny<string>(), It.IsAny<TransLogMessageStatus>(), It.IsAny<string>()), Times.Never());
        }

        [Test]
        [TestCase(MessageStatusTypeEnum.ExportTransferredError, TransLogMessageStatus.ExportTransferredError)]
        [TestCase(MessageStatusTypeEnum.ExportSentReceivedError, TransLogMessageStatus.ExportSentReceivedError)]
        public void SetMessageStatus_ValidStatusAndNullStatusTextForErrorMessage_MessageStatusAndStatusTextAreUpdated(
            MessageStatusTypeEnum messageStatus, 
            TransLogMessageStatus internalStatus)
        {
            const string externalReference = "dummy-external-reference";

            _logic.SetMessageStatus(externalReference, messageStatus, null);

            _messageLogMock.Verify(x => x.SetStatus(It.IsAny<string>(), It.IsAny<TransLogMessageStatus>()), Times.Never());
            _messageLogMock.Verify(x => x.SetStatusAndStatusText(externalReference, internalStatus, "Error"), Times.Exactly(1));
        }

        [Test]
        public void SetMessageStatus_ValidStatusAndNullStatusText_RequestIsOk()
        {
            // Assign

            // Act

            var response = _logic.SetMessageStatus("dummy-external-reference", MessageStatusTypeEnum.ExportTransferredProcessingOk, null);

            // Assert

            Assert.IsTrue(response.RequestOk);
        }

        [Test]
        public void SetMessageStatus_MessageStatusIsExportTransferredError_EventIsLogged()
        {
            // Assign

            // Act

            _logic.SetMessageStatus("dummy-external-reference", MessageStatusTypeEnum.ExportTransferredError, "Dummy message status");

            // Assert

            _serviceEventLoggerMock.Verify(x => x.LogMessage(8207, TransLogMessageStatus.ExportTransferredError.ToString(), "Dummy message status", "dummy-external-reference"));
        }

        [Test]
        public void SetMessageStatus_MessageStatusIsExportSentReceivedError_EventIsLogged()
        {
            // Assign

            // Act

            _logic.SetMessageStatus("dummy-external-reference", MessageStatusTypeEnum.ExportSentReceivedError, "Dummy message status");

            // Assert

            _serviceEventLoggerMock.Verify(x => x.LogMessage(8207, TransLogMessageStatus.ExportSentReceivedError.ToString(), "Dummy message status", "dummy-external-reference"));
        }

        [Test]
        public void SetMessageStatus_StatusIsExportSentSuccessfulAndStatusTextIsNull_StatusTextIsChangedToOk()
        {
            // Assign

            // Act

            _logic.SetMessageStatus("dummy-external-reference", MessageStatusTypeEnum.ExportSentReceivedOk, null);

            // Assert

            _messageLogMock.Verify(x => x.SetStatusAndStatusText(It.IsAny<string>(), TransLogMessageStatus.ExportSentReceivedOk, "OK"));
        }

        [Test]
        public void GetExport_InvalidMessageInQueueBeforeValidMessage_ValidMessageIsReturned()
        {
            // Assign

            int call = 0;
            const string msgDta = "data123";
            DataExchangeExportMessage exportMessage = new DataExchangeExportMessage();
            exportMessage.SetMessageData(msgDta,null);
            string internalMessageId = "2";
            const string invalidMessageInternalMessageId = "1";
            _apiMock.Setup(x => x.PeekExportMessage(out internalMessageId, It.IsAny<TimeSpan>(), It.IsAny<string>()))
                    .Returns(() =>
                    {
                        switch (call++)
                        {
                            case 0:
                                throw new DataExchangeInvalidMessageException(invalidMessageInternalMessageId);
                            case 1:
                                return exportMessage;
                            default:
                                return null;
                        }
                    });

            // Act
            
            var response = _logic.GetExport(1);

            // Assert

            Assert.AreEqual(msgDta, response.ExportMessage.ExportData);
            exportMessage.DeleteMessageData();
        }

        [Test]
        public void GetExport_InvalidMessageInQueueBeforeValidMessage_InvalidMessageIsRemoved()
        {
            // Assign

            int call = 0;

            DataExchangeExportMessage exportMessage = new DataExchangeExportMessage();
            exportMessage.SetMessageData("data123",null);
            string internalMessageId = "2";
            const string invalidMessageInternalMessageId = "1";
            _apiMock.Setup(x => x.PeekExportMessage(out internalMessageId, It.IsAny<TimeSpan>(), It.IsAny<string>()))
                    .Returns(() =>
                    {
                        switch (call++)
                        {
                            case 0:
                                throw new DataExchangeInvalidMessageException(invalidMessageInternalMessageId);
                            case 1:
                                return exportMessage;
                            default:
                                return null;
                        }
                    });

            // Act
            
            var response = _logic.GetExport(1);

            // Assert

            _apiMock.Verify(x => x.RemoveExportMessage(invalidMessageInternalMessageId, It.IsAny<IDataExchangeQueueTransaction>(), It.IsAny<string>()));
            exportMessage.DeleteMessageData();
        }

        [Test]
        public void GetExport_InvalidMessageInQueueBeforeValidMessage_TransactionIsCompletedWhenRemovingInvalidMessage()
        {
            // Assign

            int call = 0;

            DataExchangeExportMessage exportMessage = new DataExchangeExportMessage();
            exportMessage.SetMessageData("data123",null);
            string internalMessageId = "2";
            const string invalidMessageInternalMessageId = "1";
            _apiMock.Setup(x => x.PeekExportMessage(out internalMessageId, It.IsAny<TimeSpan>(), It.IsAny<string>()))
                    .Returns(() =>
                    {
                        switch (call++)
                        {
                            case 0:
                                throw new DataExchangeInvalidMessageException(invalidMessageInternalMessageId);
                            case 1:
                                return exportMessage;
                            default:
                                return null;
                        }
                    });

            // Act
            
            var response = _logic.GetExport(1);

            // Assert

            _transactionMock.Verify(x => x.Begin());
            _transactionMock.Verify(x => x.Commit());
            exportMessage.DeleteMessageData();
        }
    }
}
