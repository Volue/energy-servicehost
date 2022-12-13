using System;
using System.Threading.Tasks;
using log4net;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.ServiceBusContracts.ExternalMessage;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners
{
    public class AncitraQueueRunner : IImportApplicationRunner, IDisposable
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private INServiceBusEndpoint _endpoint;
        private IImportEventLogger _importEventLogger;

        public AncitraQueueRunner(IImportEventLogger importEventLogger, INServiceBusEndpoint endpoint)
        {
            _importEventLogger = importEventLogger;
            _endpoint = endpoint;
        }
        public virtual void Run(DataExchangeImportMessage message)
        {
            Send(message).Wait();
        }

        public async Task Send(DataExchangeImportMessage message)
        {
            _endpoint.Start();

            // Clone message, but include the payload in the message not as a Claim.
            var externalMessage = new ExternalMessageCommand
            {
                Country = message.Country,
                ExternalReference = message.ExternalReference,
                EnqueuedTimeUtc = message.EnqueuedTimeUtc,
                ExternalText = message.ExternalText,
                Format = message.Format,
                Priority = message.Priority,
                ProductCode = message.ProductCode,
                Protocol = message.Protocol,
                ProtocolId = message.ProtocolId,
                ReceiverId = message.ReceiverId,
                ReceiverName = message.ReceiverName,
                RoutingAddress = message.RoutingAddress,
                SenderId = message.SenderId,
                SenderName = message.SenderName,
                SubAddress = message.SubAddress,
                MessageData = message.GetMessageData()
            };

            Log.Debug($"Sending message: {externalMessage.ExternalReference}");
            await _endpoint.Send(externalMessage).ConfigureAwait(false);
            _importEventLogger.LogSuccessfulImport(message);
        }

        public void Dispose()
        {
            _endpoint.Dispose();
        }
    }
}
