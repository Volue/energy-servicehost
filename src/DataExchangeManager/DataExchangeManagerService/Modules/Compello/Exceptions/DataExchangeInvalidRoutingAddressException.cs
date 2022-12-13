using Powel.Icc.Common;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Exceptions
{
    public class DataExchangeInvalidRoutingAddressException : DataExchangeInvalidMessageException
    {
        public DataExchangeInvalidRoutingAddressException(string internalMessageId, string routingAddress) 
            : base(internalMessageId, GetInvalidRoutingAddressMessage(routingAddress))
        {
        }

        public static string GetInvalidRoutingAddressMessage(string routingAddress)
        {
            return string.Format(
                "The value of the routing address is invalid, expected it to be of format \"MODULE:\" where \"MODULE\" must be a valid module name. The value was \"{0}\".",
                routingAddress);
        }
    }
}