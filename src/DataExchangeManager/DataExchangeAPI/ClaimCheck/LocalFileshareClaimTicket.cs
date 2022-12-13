using System.Runtime.Serialization;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck
{
    [DataContract]
    public class LocalFileshareClaimTicket : ClaimTicket
    {
        public string Filename { get; set; }

        public LocalFileshareClaimTicket() { }
        public LocalFileshareClaimTicket(string handlerName)
        {
            HandlerName = handlerName;
        }
    }
}
