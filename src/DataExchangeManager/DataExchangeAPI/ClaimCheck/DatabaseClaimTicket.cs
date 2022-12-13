using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck
{
    [DataContract]
    public class DatabaseClaimTicket : ClaimTicket
    {
        public string Id { get; set; }

        public DatabaseClaimTicket() { }
        public DatabaseClaimTicket(string handlerName)
        {
            HandlerName = handlerName;
        }
    }
}
