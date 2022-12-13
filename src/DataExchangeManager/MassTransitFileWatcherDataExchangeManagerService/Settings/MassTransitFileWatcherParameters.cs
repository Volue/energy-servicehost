using System;
using System.Security.AccessControl;

namespace Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Settings
{
    public class MassTransitFileWatcherParameters
    {
        public class MessageFileParameters : IEquatable<MessageFileParameters>
        {
            public string MessageType { get; set; }
            public string Directory { get; set; }
            public string FileNameTemplate { get; set; }
            public string Encoding { get; set; } = "utf-8"; // See: https://docs.microsoft.com/en-us/dotnet/api/system.text.encoding?view=netframework-4.8

            public bool Equals(MessageFileParameters other)
            {
                return MessageType == other.MessageType;
            }
        }

        public MessageFileParameters[] MessageFileParametersArray;

        public class FileWatcherParameters : IEquatable<FileWatcherParameters>
        {
            public string Directory { get; set; }
            public string FileNameTemplate { get; set; }
            public string Encoding { get; set; }
            public string Host { get; set; }
            public string MessageType { get; set; }

            public bool Equals(FileWatcherParameters other)
            {
                return Directory == other.Directory && FileNameTemplate == other.FileNameTemplate;
            }
        }

        public FileWatcherParameters[] FileWatcherParametersArray;
    }
}
