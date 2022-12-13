using System.Collections.Generic;
using Moq;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class ImportMessageTranslatorTest
    {
        [Test]
        [TestCase(true)]
        [TestCase(false)]
        public void Translate_ReturnsMessageWithRoutingAddressFromSettingsSystem(bool areMetaDataMandatory)
        {
            var settingsProviderMock = new Mock<ISettingsProvider>();
            const string importRoutingAddress = "correctOne";
            settingsProviderMock
                .Setup(sp => sp.GetRoutingAddressForImport())
                .Returns(importRoutingAddress);
            var sut = new ImportMessageTranslator(new Mock<IMetaDataValueProvider>().Object, settingsProviderMock.Object);

            var result = sut.Translate(new ImportMessage(123, "test", new Dictionary<string, object>()), areMetaDataMandatory);

            StringAssert.AreEqualIgnoringCase(importRoutingAddress, result.RoutingAddress);
            settingsProviderMock
                .Verify(sp => sp.GetRoutingAddressForImport(), Times.Exactly(1));
        }
    }
}