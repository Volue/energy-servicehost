using System;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck
{
    public class ClaimHandlerException : Exception
    {
        public IClaimTicket ClaimTicket { get; set; }

        public ClaimHandlerException(IClaimTicket Ticket) : base(Ticket.HandlerName + " failure")
        {
            ClaimTicket = Ticket;
        }
        public ClaimHandlerException(IClaimTicket Ticket,Exception Inner) : base(Ticket.HandlerName + " failure",Inner)
        {
            ClaimTicket = Ticket;
        }
    }
}
