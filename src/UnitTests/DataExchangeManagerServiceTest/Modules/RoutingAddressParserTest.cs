using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules
{
    [TestFixture]
    public class RoutingAddressParserTest
    {
        private const string ROUTING_ADDRESS_WITH_PORT = "compello:localhost:25";
        private const string ROUTING_ADDRESS_WITH_WHITE_SPACES = "compello :localhost :25";
        private const string ROUTING_ADDRESS_WITHOUT_PORT = "compello:localhost";

        [Test]
        public void ParsePortFromRoutingAddress_EnterRoutingAddressWithPort_PortIsReturned()
        {
            int result = RoutingAddressParser.ParsePortFromRoutingAddress(ROUTING_ADDRESS_WITH_PORT);

            Assert.AreEqual(result, 25);
        }

        [Test]
        public void ParsePortFromRoutingAddress_EnterWhiteSpacedRoutingAddress_PortIsReturned()
        {
            int result = RoutingAddressParser.ParsePortFromRoutingAddress(ROUTING_ADDRESS_WITH_WHITE_SPACES);

            Assert.AreEqual(result, 25);
        }

        [Test]
        public void ParsePortFromRoutingAddress_EnterRoutingAddressWithoutPort_ZeroIsReturned()
        {
            int result = RoutingAddressParser.ParsePortFromRoutingAddress(ROUTING_ADDRESS_WITHOUT_PORT);

            Assert.AreEqual(result, 0);
        }

        [Test]
        public void ParseMachineNameFromRoutingAddress_EnterRoutingAddressWithPort_MachineNameIsReturned()
        {
            string result = RoutingAddressParser.ParseMachineNameFromRoutingAddress(ROUTING_ADDRESS_WITH_PORT);

            Assert.AreEqual(result, "localhost");
        }

        [Test]
        public void ParseMachineNameFromRoutingAddress_EnterWhiteSpacedRoutingAddressWithPort_MachineNameIsReturned()
        {
            string result = RoutingAddressParser.ParseMachineNameFromRoutingAddress(ROUTING_ADDRESS_WITH_WHITE_SPACES);

            Assert.AreEqual(result, "localhost");

        }

        [Test]
        public void ParseMachineNameFromRoutingAddress_EnterRoutingAddressWithoutPort_MachineNameIsReturned()
        {
            string result = RoutingAddressParser.ParseMachineNameFromRoutingAddress(ROUTING_ADDRESS_WITHOUT_PORT);

            Assert.AreEqual(result, "localhost");
        }

    }
}
