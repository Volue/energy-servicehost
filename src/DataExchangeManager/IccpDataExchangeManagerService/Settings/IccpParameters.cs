using System;
using System.Collections.Generic;
using System.Linq;
using Powel.Icc.Data;

namespace Powel.Icc.Messaging.IccpDataExchangeManager.Settings
{
    internal static class Extensions
    {
        internal static byte[] ToByteArray(this string str)
        {
            // The string contains hexadecimal numbers. E.g: "0 0 0 14" => {0x0, 0x0, 0x0, 0x14}
            return str.Split(' ').Select(b => Convert.ToByte(b,16)).ToArray();
        }
    }

    public class IccpParameters
    {
        public const string Default = "default";

        public class TlsSetup
        {
            public bool UseTls { get; set; } = false;
            public bool TlsChainValidation { get; set; } = true;
            public bool TlsAllowOnlyKnownCertificates { get; set; } = false;
            public string TlsOwnKeyFile { get; set; }
            public string TlsOwnKeyPassword { get; set; } = null;
            public string TlsOwnCertificateFile { get; set; }
            public string TlsCACertificateFiles { get; set; }
        }

        public class LocalEndpoint
        {
            public string ApTitle { get; set; } = "1.1.1.999";
            public int AeQualifier { get; set; } = 12;
            public string PSelector { get; set; } = "0 0 0 1";
            public string SSelector { get; set; } = "0 1";
            public string TSelector { get; set; } = "0 1";
            public TlsSetup Tls { get; set; } = new TlsSetup { UseTls = false, TlsChainValidation = true, TlsAllowOnlyKnownCertificates = false, TlsOwnKeyFile = "server-key.pem", TlsOwnCertificateFile = "server.cer", TlsCACertificateFiles = "root.cer" };
            public TimeSpan ListeningTimeout { get; set; } = TimeSpan.FromSeconds(1);
            public byte[] PSelectorArray => Extensions.ToByteArray(PSelector);
            public byte[] SSelectorArray => Extensions.ToByteArray(SSelector);
            public byte[] TSelectorArray => Extensions.ToByteArray(TSelector);
        }

        public class Client
        {
            public string Hostname { get; set; } = "localhost";
            public string ApTitle { get; set; } = "1.1.1.999";
            public int AeQualifier { get; set; } = 12;
            public string PSelector { get; set; } = "0 0 0 1";
            public string SSelector { get; set; } = "0 1";
            public string TSelector { get; set; } = "0 1";
            public TlsSetup Tls { get; set; } = new TlsSetup { UseTls = false, TlsChainValidation = true, TlsAllowOnlyKnownCertificates = false, TlsOwnKeyFile = "client1-key.pem", TlsOwnCertificateFile = "client1.cer", TlsCACertificateFiles = "root.cer" };
            public byte[] PSelectorArray => Extensions.ToByteArray(PSelector);
            public byte[] SSelectorArray => Extensions.ToByteArray(SSelector);
            public byte[] TSelectorArray => Extensions.ToByteArray(TSelector);
        }

        public class TimeserieMap : IEquatable<TimeserieMap>
        {
            public class VariableId : IEquatable<VariableId>
            {
                public string Domain { get; set; }
                public string Dataset { get; set; }
                public string VariableCode { get; set; }
                public bool Equals(VariableId other)
                {
                    return Domain == other.Domain && Dataset == other.Dataset && VariableCode == other.VariableCode;
                }
            }
            public VariableId Variable { get; set; }
            public string ExternalReference { get; set; }
            public string ValueUnit { get; set; }
            public bool Equals(TimeserieMap other)
            {
                return ExternalReference == other.ExternalReference && Variable == other.Variable;
            }
        }

        public class DatasetTimespan
        {
            public string Dataset { get; set; }
            public TimeSpan Timespan { get; set; }
        }

        public class DatasetBoolean
        {
            public string Dataset { get; set; }
            public bool Boolean { get; set; }
        }

        public class DatasetString
        {
            public string Dataset { get; set; }
            public string String { get; set; }
        }

        public string MessageOrigin { get; set; } = "SCADA";
        public LocalEndpoint LocalParameters { get; set; }  // Also used for local client parameters with Single association.
        // Array of servers with parameters. Usually: active and standby.
        public Client[] Clients { get; set; }
        public string DomainNames { get; set; }
        public string TlsCertificateDirectory { get; set; } = "certificates";
        public bool ReceiveInformationMessages { get; set; } = false;
        public TimeserieMap[] TimeseriesMaps { get; set; }
        public bool LogDomainDescription { get; set; } = false;
        public bool UseImportApplicationManagerService { get; set; } = true;
        // The "interval timeout" means the server sends the data for events during the interval. It will send an empty report when no changes happen during the interval.
        // You should use either "INTERVAL" or "CHANGE". Both options together make no sense.
        // But you can combine CHANGE with "INTEGRITY". "INTEGRITY" means that all data is sent when the integrity timeout expires.
        public DatasetTimespan[] TransferSetIntervals { get; set; } = { new DatasetTimespan { Dataset = Default, Timespan = TimeSpan.FromMinutes(0) }};
        public DatasetTimespan[] TransferSetReportIntegrityTimeouts { get; set; } = { new DatasetTimespan {Dataset = Default, Timespan = TimeSpan.FromMinutes(15) }};
        public DatasetBoolean[] TransferSetReportObjectChanges { get; set; } = { new DatasetBoolean {Dataset = Default, Boolean = false }};
        public DatasetString[] Priorities { get; set; } = { new DatasetString { Dataset = Default, String = "NORMAL" } };
        public bool LogMessageReceived { get; set; } = false;
        public TimeSpan ReconnectDelayTime { get;  set; } = TimeSpan.FromMinutes(1);
        public bool UseDynamicDatasets { get; set; } = false;
        public bool EnableTase2Logging { get; set; } = false;
        public bool UseDbTimeseriesMaps { get;  set; } = true;
        public char ExternalReferenceDelimiter { get; set; } = '.';

        public TimeserieMap[] GetTimeseriesMaps => UseDbTimeseriesMaps ? ReadTimeseriesMaps() : TimeseriesMaps;

        public TimeserieMap[] ReadTimeseriesMaps()
        {
            var impDefs = ImportExportData.FetchImportDefinitions(MessageOrigin, DateTime.Now);
            var mapList = new List<TimeserieMap>();    // domain, dataset, variable, extRef, valueUnit
            foreach (var impDef in impDefs)
            {
                // Decompose extRef into domain.dataset.variable.valueUnit
                // valueUnit is optional
                // domain may be blank. Leading separator must exist.
                var ids = impDef.ExtRef.Split(ExternalReferenceDelimiter);
                var valueUnit = ids.Length < 4 ? String.Empty : ids[3];

                var id = new TimeserieMap{Variable = new TimeserieMap.VariableId { Domain = ids[0], Dataset = ids[1], VariableCode = ids[2] }, ExternalReference = impDef.ExtRef, ValueUnit = valueUnit };
                if (!mapList.Contains(id))
                    mapList.Add(id);
            }

            return mapList.ToArray();
        }
    }
}
