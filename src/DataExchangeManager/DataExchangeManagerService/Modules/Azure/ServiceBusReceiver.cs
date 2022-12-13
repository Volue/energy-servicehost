using System;
using Microsoft.ServiceBus.Messaging;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure
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

        public void OnMessage(string QueueName, Action<BrokeredMessage> OnMessageAction,OnMessageOptions Opts)
        {
            var msgReceiver = _messagingFactory.CreateMessageReceiver(QueueName, ReceiveMode.PeekLock);
            msgReceiver.OnMessage(OnMessageAction, Opts);
        }
    }
}
