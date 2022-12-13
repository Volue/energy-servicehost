using System;
using System.Runtime.Serialization;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck
{
    [DataContract]
    public class CloudBlobClaimTicket : ClaimTicket
    {
        public string Blobname { get; set; }

        public CloudBlobClaimTicket() { }
        public CloudBlobClaimTicket(string handlerName)
        {
            HandlerName = handlerName;
        }
    }
}
