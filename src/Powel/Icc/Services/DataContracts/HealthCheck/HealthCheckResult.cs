using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.Serialization;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;

namespace Powel.Icc.Services.DataContracts.HealthCheck
{
    [DataContract]
    public class HealthCheckResult
    {
        private HealthCheckStatus _status;

        public static HealthCheckResult Healthy(Uri address)
        {
            return new HealthCheckResult
            {
                Status = HealthCheckStatus.Healthy,
                Address = ResolveAddressWithFullDnsName(address)
            };
        }

        public static HealthCheckResult Failed(Uri address)
        {
            return new HealthCheckResult
            {
                Status = HealthCheckStatus.Failed,
                Address = ResolveAddressWithFullDnsName(address)
            };
        }

        [DataMember(Name = "status")]
        [JsonConverter(typeof(StringEnumConverter))]
        public HealthCheckStatus Status {
            get
            {
                if (_status == HealthCheckStatus.Healthy && Dependencies != null)
                {
                    return Dependencies.Values.All(d => d.Status == HealthCheckStatus.Healthy)
                        ? HealthCheckStatus.Healthy
                        : HealthCheckStatus.Unhealthy;
                }

                return _status;
            }
            set => _status = value;
        }

        [DataMember(Name = "address", EmitDefaultValue = false)]
        public string Address { get; set; }

        [DataMember(Name = "statusCode", EmitDefaultValue = false)]
        public int StatusCode { get; set; }

        [DataMember(Name = "description", EmitDefaultValue = false)]
        public string Description { get; set; }

        [DataMember(Name = "settings", EmitDefaultValue = false)]
        public Dictionary<string, string> Settings { get; set; }

        [DataMember(Name = "counters", EmitDefaultValue = false)]
        public Dictionary<string, int> Counters { get; set; }

        [DataMember(Name = "dependencies", EmitDefaultValue = false)]
        public Dictionary<string, HealthCheckResult> Dependencies { get; set; }

        [DataMember(Name = "data", EmitDefaultValue = false)]
        public Dictionary<string, object> Data { get; set; }

        private static string ResolveAddressWithFullDnsName(Uri address)
        {
            try
            {
                var uriBuilder = new UriBuilder(address);
                uriBuilder.Host = Dns.GetHostEntry(uriBuilder.Host).HostName;
                return uriBuilder.ToString();
            }
            catch (Exception)
            {
                // Unable to resolve DNS name
                return address.ToString();
            }
        }
    }
}