using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model
{
    public class Settings : IEquatable<Settings>
    {
        public readonly string HostAddress;
        public readonly int Port;
        public readonly string ApiKey;
        public readonly int HeartbeatInterval;
        public readonly int RestartInterval;        

        public Settings(string hostAddress, int port, string apiKey, int heartbeatInterval, int restartInterval)
        {
            HostAddress = hostAddress;
            Port = port;
            ApiKey = apiKey;
            HeartbeatInterval = heartbeatInterval;
            RestartInterval = restartInterval;
        }

        public override bool Equals(object obj)
        {
            Settings other = obj as Settings;

            return other != null && Equals(other);
        }

        public bool Equals(Settings other)
        {
            return HostAddress == other.HostAddress
                   && Port == other.Port
                   && ApiKey == other.ApiKey
                   && HeartbeatInterval == other.HeartbeatInterval
                   && RestartInterval == other.RestartInterval;
        }
    }
}
