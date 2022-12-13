using System;
using System.Collections.Generic;
using Powel.Icc.Data;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck
{
    public class DatabaseClaimHandler : IClaimHandler
    {
        public static string HandlerId => "DatabaseClaimHandler";
        public string Name { get { return HandlerId; } }
        public ClaimTicket checkIn(string Payload)
        {
            var ret = new DatabaseClaimTicket(Name);
            Guid id = Guid.NewGuid();
            ret.Id = id.ToString();
            DemMessagePayload.InsertPayload(ret.Id,Payload);
            return ret;
        }

        public string checkOut(ClaimTicket Ticket, bool Delete)
        {
            string ret = String.Empty;
            var ticket = Ticket as DatabaseClaimTicket;
            if (ticket == null)
                return ret;

            try
            {
                ret = DemMessagePayload.SelectPayload(ticket.Id);
                if (Delete)
                    clear(Ticket);
            }
            catch (KeyNotFoundException e)
            {
                throw new ClaimHandlerException(Ticket,e);
            }
            return ret;
        }

        public void clear(ClaimTicket Ticket)
        {
            var ticket = Ticket as DatabaseClaimTicket;
            if (ticket == null)
                return;

            DemMessagePayload.DeletePayload(ticket.Id);
        }
    }
}
