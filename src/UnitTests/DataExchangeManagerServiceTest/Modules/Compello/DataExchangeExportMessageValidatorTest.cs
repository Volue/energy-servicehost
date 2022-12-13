using System.Globalization;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Exceptions;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Validators;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class DataExchangeExportMessageValidatorTest
    {
        [Test]
        public void Validate_CorrectRoutingAddress_DoesNotThrow()
        {
            var message = new DataExchangeExportMessage()
            {
                MessageLogId = 1,
                MessageReference = "DummyMessageReference",
                RoutingAddress = "COMPELLO:"
            };
            message.SetMessageData("DummyMessageData",null);
            var sut = new DataExchangeExportMessageValidator();

            Assert.DoesNotThrow(() => sut.Validate(message));
            message.DeleteMessageData();
        }

        [Test]
        [TestCase(null)]
        [TestCase("")]
        [TestCase("   ")]
        public void Validate_EmptyRoutingAddress_ThrowsDataExchangeInvalidRoutingAddressException(string routingAddress)
        {
            TestValidationForThrow(routingAddress);
        }

        [Test]
        public void Validate_LackOfModuleName_ThrowsDataExchangeInvalidRoutingAddressException()
        {
            TestValidationForThrow(":");
        }

        [Test]
        public void Validate_IncorrectModuleName_ThrowsDataExchangeInvalidRoutingAddressException()
        {
            TestValidationForThrow("NOTCOMPELLO:");
        }

        [TestCase("COMPELLO;")]
        [TestCase("COMPELLO#")]
        public void Validate_DelimiterNotAColon_ThrowsDataExchangeInvalidRoutingAddressException(string routingAddress)
        {
            TestValidationForThrow(routingAddress);
        }

        private static void TestValidationForThrow(string routingAddress)
        {
            var message = new DataExchangeExportMessage
                {
                    MessageLogId = 1,
                    MessageReference = "DummyMessageReference",
                    RoutingAddress = routingAddress
                };
            message.SetMessageData("DummyMessageData",null);
            var sut = new DataExchangeExportMessageValidator();

            var ex = Assert.Catch<DataExchangeInvalidRoutingAddressException>(() => sut.Validate(message));

            Assert.AreEqual(message.MessageLogId.ToString(CultureInfo.InvariantCulture), ex.MessageId);
            StringAssert.AreEqualIgnoringCase(DataExchangeInvalidRoutingAddressException.GetInvalidRoutingAddressMessage(routingAddress), ex.Message);
            message.DeleteMessageData();
        }
    }
}