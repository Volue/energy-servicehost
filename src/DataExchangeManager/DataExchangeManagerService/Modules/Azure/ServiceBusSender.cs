using Microsoft.ServiceBus.Messaging;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure
{
    public class ServiceBusSender
    {
        private readonly MessagingFactory _messagingFactory;
        public string ConnectionString { get; private set; }
        public ServiceBusSender(string connectionString)
        {
            ConnectionString = connectionString;
            _messagingFactory = MessagingFactory.CreateFromConnectionString(connectionString);
        }

        public void Send(BrokeredMessage Msg,string QueueName)
        {
            var sender = _messagingFactory.CreateMessageSender(QueueName);
            sender.Send(Msg);
        }
    }
}
