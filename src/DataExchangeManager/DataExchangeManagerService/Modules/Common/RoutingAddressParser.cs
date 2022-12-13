using System;
using System.Globalization;
using Powel.Icc.Common;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common
{
    public static class RoutingAddressParser
    {
        public static string[] SplitString(string str)
        {
            return str.Replace(" ", "").Split(new char[] { ':' });
        }

        public static int ParsePortFromRoutingAddress(string routingAddress)
        {
            int retVal = 0;

            string[] splitAddress = SplitString(routingAddress ?? string.Empty);

            // Should be value "module:machine:port"
            if (splitAddress.Length == 3)
            {
                if(!Int32.TryParse(splitAddress[2], NumberStyles.Integer, NumberFormatInfo.InvariantInfo, out retVal))
                {
                    throw new DataExchangeConfigurationException("Expected the port specified in the routing address to be an integer.");
                }
            }
            // module:machine:port
            //                 ^
            return retVal;
        }

        public static string ParseMachineNameFromRoutingAddress(string routingAddress)
        {
            string retVal = String.Empty;

            string[] splitAddress = SplitString(routingAddress ?? string.Empty);

            // Should be value "module:machine:port" or "module:machine"
            if (splitAddress.Length == 2 || splitAddress.Length == 3)
            {
                // module:machine:port 
                //          ^
                retVal = splitAddress[1];
            }

            return retVal;
        }

        public static string ParseModuleName(string routingAddress)
        {
            if(string.IsNullOrWhiteSpace(routingAddress))
            {
                return null;
            }

            var splitted = SplitString(routingAddress);
            if(splitted.Length > 0)
            {
                return splitted[0];
            }

            return null;
        }

        public static string ParseBlobName(string RoutingAddress)
        {
            string[] splitAddress = SplitString(RoutingAddress ?? string.Empty);

            // Should be value "module:BlobUri:"
            return splitAddress.Length == 3 ? splitAddress[2] : string.Empty;
        }
    }
}
