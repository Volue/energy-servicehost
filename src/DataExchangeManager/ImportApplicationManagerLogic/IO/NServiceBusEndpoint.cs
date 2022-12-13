using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using NServiceBus;
using NServiceBus.Features;
using NServiceBus.Persistence.Legacy;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.ServiceBusContracts.ExternalMessage;
using Powel.MessageBus.Contracts.TimeSeries.Contract;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO
{
    public class NServiceBusEndpoint : INServiceBusEndpoint
    {
        private EndpointConfiguration _configuration;
        private IEndpointInstance _endpointInstance;

        public NServiceBusEndpoint()
        {
            NServiceBus.Logging.LogManager.Use<Log4NetFactory>();   // Execute as early as possible.
            var endpointName = ConfigurationManager.AppSettings["ImportApplicationManagerLogic.EndpointName"] ?? "ImportApplicationManagerLogicEndpoint";
            var errorEndpointName = endpointName + ".Error";

            _configuration = new EndpointConfiguration(endpointName);
            _configuration.SendOnly();
            _configuration.DisableFeature<TimeoutManager>();
            _configuration.UseSerialization<JsonSerializer>();
            var transport = _configuration.UseTransport<MsmqTransport>();
            var routing = transport.Routing();

            // ExternalMessageCommand is sent to ancitra (AncitraQueueRunner).
            var ancitraEndpointName = ConfigurationManager.AppSettings["Ancitra.EndpointName"] ?? "Ancitra.Import";
            routing.RouteToEndpoint(typeof(ExternalMessageCommand), ancitraEndpointName);

            // TimeSeriesContract is sent to ServiceBus2TssService (ServiceBus2TssRunner).
            var timeseriesReceiverEndpointName = ConfigurationManager.AppSettings["TimeSeriesContractReceiver.EndpointName"] ?? "Timeseries2Tss";
            routing.RouteToEndpoint(typeof(TimeSeriesContract), timeseriesReceiverEndpointName);

            _configuration.UsePersistence<MsmqPersistence>();
            _configuration.SendFailedMessagesTo(errorEndpointName);
            _configuration.EnableInstallers();
            ExcludeAssemblyScanning(_configuration);
        }

        public async Task<bool> Start()
        {
            if (_endpointInstance == null)
                _endpointInstance = await Endpoint.Start(_configuration).ConfigureAwait(false);
            return _endpointInstance != null;
        }

        public void Stop()
        {
            _endpointInstance?.Stop();
            _endpointInstance = null;
        }

        public Task Send(ICommand message)
        {
            return _endpointInstance?.Send(message);
        }

        #region AssemblyScanning

        private static void ExcludeAssemblyScanning(EndpointConfiguration endpointConfiguration)
        {
            // EDI\.Server\.API\.Client.+\.dll: Don't scan Compello dlls for net_tcp and https.
            var excludeRegexs = (ConfigurationManager.AppSettings["ServiceHostExcludedDllAssembliesRegexList"] ??
                                 @"EDI\.Server\.API\.Client.+\.dll").Split(',');
            ExcludeAssemblyScanning(endpointConfiguration, excludeRegexs, "*.dll");
            var cfgExclExe = ConfigurationManager.AppSettings["ServiceHostExcludedExeAssembliesRegexList"];
            if (!string.IsNullOrEmpty(cfgExclExe))
            {
                excludeRegexs = cfgExclExe.Split(',');
                ExcludeAssemblyScanning(endpointConfiguration, excludeRegexs, "*.exe");
            }
        }

        private static void ExcludeAssemblyScanning(EndpointConfiguration endpointConfiguration, string[] excludeRegexs, string directoryFileTypePattern)
        {
            var scanner = endpointConfiguration.AssemblyScanner();
            var baseDirectory = AppDomain.CurrentDomain.BaseDirectory;
            var excludeAssemblies = new List<string>();
            foreach (var filename in Directory.EnumerateFiles(baseDirectory, directoryFileTypePattern).Select(Path.GetFileName))
            {
                foreach (var pattern in excludeRegexs)
                {
                    if (Regex.IsMatch(filename, pattern, RegexOptions.IgnoreCase))
                        excludeAssemblies.Add(filename);
                }
            }
            scanner.ExcludeAssemblies(excludeAssemblies.ToArray());
        }

        #endregion

        public void Dispose()
        {
            _endpointInstance?.Stop().ConfigureAwait(false);
        }
    }
}
