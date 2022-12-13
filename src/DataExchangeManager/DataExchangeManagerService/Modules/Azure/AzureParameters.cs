namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure
{
    public class AzureParameters
    {
        //public string pipServiceBusBaseAddress { get; set; }
        //public string pipServiceBusNamespace { get; set; }
        //public string pipInputQueueKeyName { get; set; }
        //public string pipInputQueuePrimaryKey { get; set; }
        //public string pipInputQueueSecondaryKey { get; set; }
        public string pipInputQueueName { get; set; }
        public string pipInputQueuePrimaryConnectionString { get; set; }
        //public string pipInputQueueSecondaryConnectionString { get; set; }
        //public string pipOutputTopicKeyName { get; set; }
        //public string pipOutputTopicPrimaryKey { get; set; }
        //public string pipOutputTopicSecondaryKey { get; set; }
        public string pipOutputTopicName { get; set; }
        public string pipOutputTopicPrimaryConnectionString { get; set; }
        public string pipOutputTopicSecondaryConnectionString { get; set; }
        public string pipBlobStorageUrlAndSas { get; set; }

        public AzureParameters()
        { }
    }
}
