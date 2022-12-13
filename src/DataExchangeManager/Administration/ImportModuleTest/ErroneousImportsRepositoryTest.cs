using System;
using System.Collections.Generic;
using System.Globalization;
using DataExchange.Administration.ImportModule;
using Moq;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Services.Time;
using Powel.Wpf.Common;

namespace DataExchange.Administration.ImportModuleTest
{
    [TestFixture]
    public class ErroneousImportsRepositoryTest
    {
        private Mock<IDataExchangeApi> _dataExchangeApiMock;
        private ErroneousImportsRepository _erroneousImportRepository;
        private Mock<IDataExchangeQueueTransaction> _dataExchangeQueueTransactionMock;

        [SetUp]
        public void SetUp()
        {
            _dataExchangeQueueTransactionMock = new Mock<IDataExchangeQueueTransaction>();
            
            _dataExchangeApiMock = new Mock<IDataExchangeApi>();
            _dataExchangeApiMock.Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>())).Returns(_dataExchangeQueueTransactionMock.Object);

            _erroneousImportRepository = new ErroneousImportsRepository(_dataExchangeApiMock.Object, new DateTimeUtility(new CultureInfo("sv-SE")), () => new RegionalCalendar());
        }

        [Test]
        public void GetAll_DataExchangeApiReturnsNull_ReturnsEmptyList()
        {
            // Assign

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => null);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual(0, result.Count);
        }

        [Test]
        public void GetAll_DataExchangeApiReturnsEmptyDictionary_ReturnsEmptyList()
        {
            // Assign

            var allErroneousImportMessages = new Dictionary<string, DataExchangeImportMessage>();

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => allErroneousImportMessages);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual(0, result.Count);
        }

        [Test]
        public void GetAll_DataExchangeApiReturnsOneResult_ReturnsOneResult()
        {
            // Assign

            var allErroneousImportMessages = new Dictionary<string, DataExchangeImportMessage>();
            var msg = new DataExchangeImportMessage {Country = "SWE", Protocol = "UTILTS"};
            msg.SetMessageData("0123456789",null);
            allErroneousImportMessages.Add("1", msg);

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => allErroneousImportMessages);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual(1, result.Count);
        }

        [Test]
        public void GetAll_DataExchangeApiReturnsOneResult_ResultIsConvertedToModelCorrectly()
        {
            // Assign
            const string msgDta = "0123456789";
            var allErroneousImportMessages = new Dictionary<string, DataExchangeImportMessage>();
            var msg = new DataExchangeImportMessage {Country = "SWE", Protocol = "UTILTS"};
            msg.SetMessageData(msgDta,null);
            allErroneousImportMessages.Add("1", msg);

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => allErroneousImportMessages);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual("1", result[0].InternalId);
            Assert.AreEqual(msgDta, result[0].ImportMessage);
            Assert.AreEqual("SWE", result[0].ImportProperties.Find(x => x.PropertyName == FailedImportPropertyName.Country).PropertyValue);
            Assert.AreEqual("UTILTS", result[0].ImportProperties.Find(x => x.PropertyName == FailedImportPropertyName.Protocol).PropertyValue);
        }
        
        [Test]
        public void GetAll_DataExchangeApiReturnsOneResult_TimeStampIsFormattedCorrectly()
        {
            // Assign
            var allErroneousImportMessages = new Dictionary<string, DataExchangeImportMessage>();
            var msg = new DataExchangeImportMessage
            {
                Country = "SWE",
                Protocol = "UTILTS",
                EnqueuedTimeUtc = new DateTime(2013, 11, 8, 10, 00, 00)
            };
            msg.SetMessageData("0123456789",null);
            allErroneousImportMessages.Add("1", msg);

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => allErroneousImportMessages);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual("2013-11-08 10:00", result[0].Timestamp);
        }

        [Test]
        public void GetAll_DataExchangeApiReturnsOneResultWithProtocol_ProtocolIsCorrect()
        {
            // Assign

            var allErroneousImportMessages = new Dictionary<string, DataExchangeImportMessage>();
            var msg = new DataExchangeImportMessage
            {
                Protocol = "UTILTS",
                EnqueuedTimeUtc = new DateTime(2013, 11, 8, 10, 00, 00)
            };
            msg.SetMessageData("0123456789",null);
            allErroneousImportMessages.Add("1", msg);

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => allErroneousImportMessages);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual("UTILTS", result[0].Protocol);
        }

        [Test]
        public void GetAll_DataExchangeApiReturnsOneResultWithCountry_CountryIsCorrect()
        {
            // Assign
            var allErroneousImportMessages = new Dictionary<string, DataExchangeImportMessage>();
            var msg = new DataExchangeImportMessage
            {
                Country = "SWE",
                EnqueuedTimeUtc = new DateTime(2013, 11, 8, 10, 00, 00)
            };
            msg.SetMessageData("0123456789",null);
            allErroneousImportMessages.Add("1", msg);

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => allErroneousImportMessages);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual("SWE", result[0].Country);
        }

        [Test]
        public void GetAll_DataExchangeApiReturnsOneResultWithSubAddress_SubAddressIsCorrect()
        {
            // Assign

            var allErroneousImportMessages = new Dictionary<string, DataExchangeImportMessage>();
            var msg = new DataExchangeImportMessage
            {
                SubAddress = "SUBADDRESS",
                EnqueuedTimeUtc = new DateTime(2013, 11, 8, 10, 00, 00)
            };
            msg.SetMessageData("0123456789",null);
            allErroneousImportMessages.Add("1", msg);

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => allErroneousImportMessages);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual("SUBADDRESS", result[0].SubAddress);
        }

        [Test]
        public void GetAll_DataExchangeApiReturnsOneResultWithProductCode_ProductCodeIsCorrect()
        {
            // Assign

            var allErroneousImportMessages = new Dictionary<string, DataExchangeImportMessage>();
            var msg = new DataExchangeImportMessage
            {
                ProductCode = "PCODE",
                EnqueuedTimeUtc = new DateTime(2013, 11, 8, 10, 00, 00)
            };
            msg.SetMessageData("0123456789",null);
            allErroneousImportMessages.Add("1", msg);

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => allErroneousImportMessages);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual("PCODE", result[0].ProductCode);
        }

        [Test]
        public void GetAll_DataExchangeApiReturnsOneResultWithSenderName_SenderNameIsCorrect()
        {
            // Assign

            var allErroneousImportMessages = new Dictionary<string, DataExchangeImportMessage>();
            var msg = new DataExchangeImportMessage
            {
                SenderName = "SENDER",
                EnqueuedTimeUtc = new DateTime(2013, 11, 8, 10, 00, 00)
            };
            msg.SetMessageData("0123456789",null);
            allErroneousImportMessages.Add("1", msg);

            _dataExchangeApiMock.Setup(x => x.GetAllErroneousImportMessages(It.IsAny<IDataExchangeQueueTransaction>())).Returns(() => allErroneousImportMessages);

            // Act

            var result = _erroneousImportRepository.GetAll();

            // Assert

            Assert.AreEqual("SENDER", result[0].SenderName);
        }

        [Test]
        public void Delete_OneFailedImportIsDeleted_OneImportIsRemovedUsingDataExchangeApi()
        {
            // Assign

            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                                       {
                                           new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                           new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                       },
                    ImportMessage = "0123456789"
                };

            var failedImportModels = new List<FailedImportModel> {failedImportModel};

            // Act

            _erroneousImportRepository.Delete(failedImportModels);

            // Assert

            _dataExchangeApiMock.Verify(x => x.RemoveErroneousImportMessages(It.Is<IList<string>>(i => i.Count == 1 && i.Contains("1")), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void Delete_OneFailedImportIsDeleted_TransactionIsCompleted()
        {
            // Assign

            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                                       {
                                           new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                           new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                       },
                    ImportMessage = "0123456789"
                };

            var failedImportModels = new List<FailedImportModel> {failedImportModel};

            // Act

            _erroneousImportRepository.Delete(failedImportModels);

            // Assert

            _dataExchangeQueueTransactionMock.Verify(x => x.Begin());
            _dataExchangeQueueTransactionMock.Verify(x => x.Commit());
        }

        [Test]
        public void Delete_ThreeFailedImportIsDeleted_ThreeImportsAreRemovedUsingDataExchangeApi()
        {
            // Assign

            var failedImportModel1 = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                                       {
                                           new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                           new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                       },
                    ImportMessage = "0123456789"
                };
            var failedImportModel2 = new FailedImportModel
                {
                    InternalId = "2",
                    ImportProperties = new List<FailedImportProperty>
                                       {
                                           new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                           new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                       },
                    ImportMessage = "0123456789"
                };
            var failedImportModel3 = new FailedImportModel
                {
                    InternalId = "3",
                    ImportProperties = new List<FailedImportProperty>
                                       {
                                           new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                           new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                       },
                    ImportMessage = "0123456789"
                };

            var failedImportModels = new List<FailedImportModel>
                {
                    failedImportModel1,
                    failedImportModel2,
                    failedImportModel3
                };

            // Act

            _erroneousImportRepository.Delete(failedImportModels);

            // Assert

            _dataExchangeApiMock.Verify(x => x.RemoveErroneousImportMessages(It.Is<IList<string>>(i => i.Count == 3 && i.Contains("1") && i.Contains("2") && i.Contains("3")), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImported_OneImportIsDequeuedUsingDataExchangeApi()
        {
            // Assign
            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                                       {
                                           new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                           new FailedImportProperty(FailedImportPropertyName.Protocol, "MSCONS")
                                       },
                    ImportMessage = "ABCDEFGHIJK"
                };
            var msg = new DataExchangeImportMessage
            {
                Protocol = "UTILTS",
                Country = "SWE",
            };
            msg.SetMessageData("0123456789",null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeApiMock.Verify(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImportedWithExternalReference_ExternalReferenceIsCorrectInTheSavedAndImportedMessage()
        {
            // Assign
            var failedImportModel = new FailedImportModel
            {
                InternalId = "1",
                ImportProperties = new List<FailedImportProperty>
                        {
                            new FailedImportProperty(FailedImportPropertyName.ExternalReference, "12345")
                        },
                ImportMessage = "ABCDEFGHIJK"
            };
            var msg = new DataExchangeImportMessage();
            msg.SetMessageData("0123456789",null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(y => y.ExternalReference == "12345"), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImportedWithProtocol_ProtocolIsCorrectInTheSavedAndImportedMessage()
        {
            // Assign

            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                        {
                            new FailedImportProperty(FailedImportPropertyName.Protocol, "MSCONS")
                        },
                    ImportMessage = "ABCDEFGHIJK"
                };
            var msg = new DataExchangeImportMessage();
            msg.SetMessageData("0123456789",null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(y => y.Protocol == "MSCONS"), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImportedWithCountry_CountryIsCorrectInTheSavedAndImportedMessage()
        {
            // Assign

            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                            {
                                new FailedImportProperty(FailedImportPropertyName.Country, "SWE")
                            },
                    ImportMessage = "ABCDEFGHIJK"
                };
            var msg = new DataExchangeImportMessage();
            msg.SetMessageData("0123456789",null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(y => y.Country == "SWE"), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImportedWithSubAddress_SubAddressIsCorrectInTheSavedAndImportedMessage()
        {
            // Assign

            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                            {
                                new FailedImportProperty(FailedImportPropertyName.SubAddress, "SUBADDRESS")
                            },
                    ImportMessage = "ABCDEFGHIJK"
                };
            var msg = new DataExchangeImportMessage();
            msg.SetMessageData("0123456789",null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(y => y.SubAddress == "SUBADDRESS"), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImportedWithProductCode_ProductCodeIsCorrectInTheSavedAndImportedMessage()
        {
            // Assign

            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                                {
                                    new FailedImportProperty(FailedImportPropertyName.ProductCode, "PCODE")
                                },
                    ImportMessage = "ABCDEFGHIJK"
                };
            var msg = new DataExchangeImportMessage();
            msg.SetMessageData("0123456789",null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(y => y.ProductCode == "PCODE"), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImportedWithSenderName_SenderNameIsCorrectInTheSavedAndImportedMessage()
        {
            // Assign

            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                                    {
                                        new FailedImportProperty(FailedImportPropertyName.SenderName, "SENDER")
                                    },
                    ImportMessage = "ABCDEFGHIJK"
                };
            var msg = new DataExchangeImportMessage();
            msg.SetMessageData("0123456789",null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(y => y.SenderName == "SENDER"), It.IsAny<IDataExchangeQueueTransaction>()));
        }


        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImportedWithExternalText_ExternalTextIsCorrectInTheSavedAndImportedMessage()
        {
            // Assign

            var failedImportModel = new FailedImportModel
            {
                InternalId = "1",
                ImportProperties = new List<FailedImportProperty>
                        {
                            new FailedImportProperty(FailedImportPropertyName.ExternalText, "12345")
                        },
                ImportMessage = "ABCDEFGHIJK"
            };
            var msg = new DataExchangeImportMessage();
            msg.SetMessageData("0123456789",null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(y => y.ExternalText == "12345"), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImported_OneImportIsEnqueuedUsingDataExchangeApi()
        {
            // Assign
            const string msgDta = "0123456789";
            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                                       {
                                           new FailedImportProperty(FailedImportPropertyName.Country, "NOR"),
                                           new FailedImportProperty(FailedImportPropertyName.Protocol, "MSCONS")
                                       },
                    ImportMessage = "ABCDEFGHIJK"
                };
            var msg = new DataExchangeImportMessage
            {
                Protocol = "UTILTS",
                Country = "SWE"
            };
            msg.SetMessageData(msgDta,null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(m => m.GetMessageData(true) == msgDta), It.IsAny<IDataExchangeQueueTransaction>()));
            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(m => m.Protocol == "MSCONS"), It.IsAny<IDataExchangeQueueTransaction>()));
            _dataExchangeApiMock.Verify(x => x.EnqueueImportMessage(It.Is<DataExchangeImportMessage>(m => m.Country == "NOR"), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void SaveAndImport_OneFailedImportIsSavedAndImported_TransactionIsCompleted()
        {
            // Assign

            var failedImportModel = new FailedImportModel
                {
                    InternalId = "1",
                    ImportProperties = new List<FailedImportProperty>
                                       {
                                           new FailedImportProperty(FailedImportPropertyName.Country, "NOR"),
                                           new FailedImportProperty(FailedImportPropertyName.Protocol, "MSCONS")
                                       },
                    ImportMessage = "ABCDEFGHIJK"
                };
            var msg = new DataExchangeImportMessage
            {
                Protocol = "UTILTS",
                Country = "SWE"
            };
            msg.SetMessageData("0123456789",null);
            _dataExchangeApiMock.Setup(x => x.DequeueErroneousImportMessage("1", It.IsAny<IDataExchangeQueueTransaction>())).Returns(msg);

            // Act

            _erroneousImportRepository.SaveAndImport(failedImportModel);

            // Assert

            _dataExchangeQueueTransactionMock.Verify(x => x.Begin());
            _dataExchangeQueueTransactionMock.Verify(x => x.Commit());
        }
    }
}
