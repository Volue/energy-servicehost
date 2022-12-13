using System;
using Microsoft.ServiceBus.Messaging;

namespace Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService.Modules.Azure
{
    public class ServiceBusReceiver
    {
        private readonly MessagingFactory _messagingFactory;

        public string ConnectionString { get; private set; }

        public ServiceBusReceiver(string connectionString)
        {
            ConnectionString = connectionString;
            _messagingFactory = MessagingFactory.CreateFromConnectionString(connectionString);
        }

        public void OnMessage(string queueName, Action<BrokeredMessage> onMessageAction, OnMessageOptions opts)
        {
            GetMessageReceiver(queueName).OnMessage(onMessageAction, opts);
        }

        public MessageReceiver GetMessageReceiver(string queueName)
        {
            return _messagingFactory.CreateMessageReceiver(queueName, ReceiveMode.PeekLock);
        }
    }
}
