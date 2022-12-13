using System;
using System.Threading.Tasks;
using NServiceBus;
using NServiceBus.Logging;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.MessageBus.Contracts.TimeSeries.Contract;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners
{
    public class ServiceBus2TssRunner : IImportApplicationRunner, IDisposable
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private INServiceBusEndpoint _endpoint;
        private IImportEventLogger _importEventLogger;

        public ServiceBus2TssRunner(IImportEventLogger importEventLogger, INServiceBusEndpoint endpoint)
        {
            NServiceBus.Logging.LogManager.Use<Log4NetFactory>();   // Execute as early as possible.
            _importEventLogger = importEventLogger;
            _endpoint = endpoint;
        }
        public virtual void Run(DataExchangeImportMessage message)
        {
            Send(message).Wait();
        }

        public async Task Send(DataExchangeImportMessage message)
        {
            var started = await _endpoint.Start();
            if (!started)
            {
                Log.Error("Endpoint not started.");
                return;
            }

            // Clone message, but include the payload in the message not as a Claim.
            var externalMessage = Newtonsoft.Json.JsonConvert.DeserializeObject<TimeSeriesContract>(message.GetMessageData());

            Log.Debug($"Sending message: {message.GetMessageData()}");
            await _endpoint.Send(externalMessage).ConfigureAwait(false);
            _importEventLogger.LogSuccessfulImport(message);
        }

        public void Dispose()
        {
            _endpoint.Dispose();
        }
    }
}
