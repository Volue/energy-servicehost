using System.Net;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public enum MachineNameType
    {
        Os, Tcp, Local
    }

    public static class MachineNameTypeUtility
    {
        public static MachineNameType DetermineMachineNameType(string machineName)
        {
             MachineNameType machineNameType;

            if (string.IsNullOrEmpty(machineName) || machineName == "." || machineName == "localhost" || machineName == Dns.GetHostName())
            {
                machineNameType = MachineNameType.Local;
            }
            else
            {
                IPAddress ipAddress;
                if (IPAddress.TryParse(machineName, out ipAddress))
                {
                    machineNameType = IPAddress.IsLoopback(ipAddress) ? MachineNameType.Local : MachineNameType.Tcp;
                }
                else
                {
                    machineNameType = MachineNameType.Os;
                }
            }

            return machineNameType;
        }
    }
}
