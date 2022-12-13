using System;
using Microsoft.ServiceBus.Messaging;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService.Modules.Azure;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace AzureDataExchangeManagerService.UnitTests
{
    [TestFixture]
    public class AzureMessageToImportTests
    {
        //    private IUnityContainer _container;
        //    private Dictionary<string, IUnityContainer> _childContainers;
        private AzureLogic _instance;
        private DataExchangeSettings _dataExchangeSettings;
        private Mock<IServiceEventLogger> _mockServiceEventLogger;

        private const string RoutingAddress = "AZURE:";
        private const string OutputFormatProperty = "Output_Format";
        private const string SendercodeProperty = "SenderCode";
        private const string ReceiverCodeProperty = "ReceiverCode";
        private const string BusinessTypeProperty = "BusinessType";
        private const string ProductCodeProperty = "ProductCode";
        private const string SenderCountryProperty = "SenderCountry";
        private const string PriorityProperty = "Priority";

        [SetUp]
        public void SetUp()
        {
            _dataExchangeSettings = new DataExchangeSettings
            {
                ImportQueueMachineName = ".",
                ExportQueueMachineName = ".",
                StoreExportMessages = false
            };
            _mockServiceEventLogger = new Mock<IServiceEventLogger>();
            var mockImportApplication = new Mock<IImportApplication>();
            _instance = new AzureLogic(_mockServiceEventLogger.Object, mockImportApplication.Object);
            var dataExchangeSettingsFactory = new Mock<IDataExchangeSettingsFactory>();
            dataExchangeSettingsFactory.Setup(x => x.GetSettings()).Returns(_dataExchangeSettings);
        }

        [TestCase("OutputFormatProperty", "Hello World!", "OutputFormatProperty")]
        [TestCase("APERAK", "NP:Hello World!", "EDIELAPERAK")]
        [TestCase("APERAK", "Hello World!", "UTILTSAPERAK")]
        public void BrokeredMessageMapsToDataExchangeImportMessage(string outpFormat, string payload, string expectedProtocol)
        {
            var azurebMessage = BuildBrokeredMessage(outpFormat);
            var dataExchImportMessage = _instance.BuildDataExchangeImportMessage(azurebMessage, payload);
            Assert.IsNotNull(dataExchImportMessage);
            Assert.AreEqual(payload, dataExchImportMessage.GetMessageData());
            Assert.AreEqual(expectedProtocol, dataExchImportMessage.Protocol);
            Assert.AreEqual(RoutingAddress, dataExchImportMessage.RoutingAddress);
            Assert.AreEqual(azurebMessage.Properties[SenderCountryProperty], dataExchImportMessage.Country);
            Assert.AreEqual(azurebMessage.Properties[PriorityProperty], dataExchImportMessage.Priority);
            Assert.AreEqual(azurebMessage.CorrelationId, dataExchImportMessage.ExternalReference);
            Assert.AreEqual(azurebMessage.Properties[SendercodeProperty], dataExchImportMessage.SenderName);
            Assert.AreEqual(azurebMessage.Properties[ProductCodeProperty], dataExchImportMessage.ProductCode);
            Assert.AreEqual(azurebMessage.Properties[BusinessTypeProperty], dataExchImportMessage.SubAddress);
            Assert.AreEqual(azurebMessage.Properties[ReceiverCodeProperty], dataExchImportMessage.ReceiverName);
        }

        [Test]
        public void BrokeredMessageMappingToDataExchangeImportMessageThrowsExceptionOnMissingProperty()
        {
            foreach (var propName in new[] { OutputFormatProperty })
            {
                AssertMissingPropertyThrowsException(propName);
            }
        }

        [Test]
        public void BrokeredMessageMappingToDataExchangeImportMessageDoesNotThrowExceptionOnNonRequiredProperties()
        {
            foreach (var propName in new[]
            {
                SendercodeProperty,
                ReceiverCodeProperty,
                BusinessTypeProperty,
                ProductCodeProperty,
                SenderCountryProperty,
                PriorityProperty
            })
            {
                AssertMissingPropertyThrowsException(propName, false);
            }
        }

        private void AssertMissingPropertyThrowsException(string propName, bool shouldThrow = true)
        {
            var faultyMessage = BuildBrokeredMessage("APERAK");
            faultyMessage.Properties.Remove(propName);
            if (shouldThrow)
            {
                Assert.That(() => _instance.BuildDataExchangeImportMessage(faultyMessage, "Hello world"), Throws.TypeOf<MissingFieldException>());
            }
            else
            {
                Assert.That(() => _instance.BuildDataExchangeImportMessage(faultyMessage, "Hello world"), Throws.Nothing);
            }
        }

        private static BrokeredMessage BuildBrokeredMessage(string outpFormat, string priority = null)
        {
            var azurebMessage = new BrokeredMessage()
            {
                CorrelationId = "CorrelationId"
            };
            azurebMessage.Properties[OutputFormatProperty] = outpFormat;
            azurebMessage.Properties[SendercodeProperty] = "Sendercode";
            azurebMessage.Properties[ReceiverCodeProperty] = "ReceiverCode";
            azurebMessage.Properties[BusinessTypeProperty] = "BusinessType";
            azurebMessage.Properties[ProductCodeProperty] = "ProductCode";
            azurebMessage.Properties[SenderCountryProperty] = "SenderCountry";
            azurebMessage.Properties[PriorityProperty] = priority ?? DataExchangeQueuePriority.High.ToString();
            return azurebMessage;
        }
    }
}