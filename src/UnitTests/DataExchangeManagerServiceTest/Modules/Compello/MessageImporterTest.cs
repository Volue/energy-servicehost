using System.Collections.Generic;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class MessageImporterTest
    {
        private Mock<IDataExchangeApi> _dataExchangeApiMock;
        private ImportMessageTranslator _importMessageTranslator;
        private MessageImporter _messagImporter;
        private Mock<IServiceEventLogger> _serviceEventLogger;
        private Mock<ISettingsProvider> _settingsProviderMock;

        [SetUp]
        public void SetUp()
        {
            var dataExchangeQueueTransactionMock = new Mock<IDataExchangeQueueTransaction>();
            dataExchangeQueueTransactionMock.SetupAllProperties();

            _serviceEventLogger = new Mock<IServiceEventLogger>();
            
            _dataExchangeApiMock = new Mock<IDataExchangeApi>();
            _dataExchangeApiMock
                .Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>()))
                .Returns(dataExchangeQueueTransactionMock.Object);

            _settingsProviderMock = new Mock<ISettingsProvider>();
            _settingsProviderMock
                .Setup(sp => sp.GetRoutingAddressForImport())
                .Returns("COMPELLO:vm-compello:9999");

            _importMessageTranslator = new ImportMessageTranslator(new MetaDataValueProvider(), _settingsProviderMock.Object);
            _messagImporter = new MessageImporter(_dataExchangeApiMock.Object, _importMessageTranslator, _serviceEventLogger.Object);
        }

        [Test]
        public void ReceiveImportMessageCorrectMessageReceived()
        {
            var metaData = new Dictionary<string, object>
                {
                    {"priority", "normal"},
                    {"protocol", "dummyProtocol"},
                    {"country", "dummyCountry"},
                    {"externaltext", "dummyExternalText"}
                };
            var message = new ImportMessage(1,"dummyData",metaData);
            
            var response =_messagImporter.Import(message);

            Assert.IsNotNull(response);
            Assert.IsTrue(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Null.Or.Empty);
        }

        [Test]
        public void ReceiveImportMessageMetaDataIsNull()
        {
            var message = new ImportMessage(1, "dummyData", null);

            var response = _messagImporter.Import(message);

            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
            Assert.AreEqual("No metadata available.", response.RequestNotOkReasonText);
            _serviceEventLogger.Verify(x => x.LogMessage(30264), Times.Exactly(1));
        }

        [Test]
        public void ReceiveImportMessageMetaDataPriorityIsEmpty()
        {
            var metaData = new Dictionary<string, object>
                {
                    {"priority", ""},
                    {"protocol", "dummyProtocol"},
                    {"country", "dummyCountry"},
                    {"externaltext", "dummyExternalText"}
                };
            var message = new ImportMessage(1, "dummyData", metaData);

            var response = _messagImporter.Import(message);

            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
            Assert.AreEqual("Field with name 'priority' was empty in the metadata, this field is required.", response.RequestNotOkReasonText);
            _serviceEventLogger.Verify(x => x.LogMessage(30264), Times.Exactly(1));
        }

        [Test]
        public void ReceiveImportMessageMetaDataProtocolIsEmpty()
        {
            var metaData = new Dictionary<string, object>
                {
                    {"priority", "normal"},
                    {"protocol", ""},
                    {"country", "dummyCountry"},
                    {"externaltext", "dummyExternalText"}
                };
            var message = new ImportMessage(1, "dummyData", metaData);

            var response = _messagImporter.Import(message);

            Assert.IsNotNull(response);
            Assert.IsFalse(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
            Assert.AreEqual("Field with name 'protocol' was empty in the metadata, this field is required.", response.RequestNotOkReasonText);
            _serviceEventLogger.Verify(x => x.LogMessage(30264), Times.Exactly(1));
        }

        [Test]
        public void ReceiveImportMessageMetaDataNotMandatoryFieldIsEmpty()
        {
            var metaData = new Dictionary<string, object>
                {
                    {"priority", "normal"},
                    {"protocol", "dummyProtocol"},
                    {"country", ""},
                    {"externaltext", "dummyExternalText"}
                };
            var message = new ImportMessage(1, "dummyData", metaData);

            var response = _messagImporter.Import(message);

            Assert.IsNotNull(response);
            Assert.IsTrue(response.RequestOk);
            Assert.That(response.RequestNotOkReasonText, Is.Null.Or.Empty);
            
        }

        [Test]
        public void ReceiveImportMessageMetaDataPriorityFieldIsNull()
        {
            var metaData = new Dictionary<string, object>
                {
                    {"priority", null},
                    {"protocol", "dummyProtocol"},
                    {"country", ""},
                    {"externaltext", "dummyExternalText"}
                };
            var message = new ImportMessage(1, "dummyData", metaData);

            var response = _messagImporter.Import(message);

             Assert.IsNotNull(response);
             Assert.IsFalse(response.RequestOk);
             Assert.That(response.RequestNotOkReasonText, Is.Not.Null.Or.Empty);
             Assert.AreEqual("Internal error ('Object reference not set to an instance of an object.')", response.RequestNotOkReasonText);             
        }
    }
}
