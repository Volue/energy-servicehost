namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract
{
    public interface IClaimHandler
    {
        string Name { get; }
        ClaimTicket checkIn(string Payload);
        string checkOut(ClaimTicket Ticket, bool Delete);
        void clear(ClaimTicket Ticket);
    }
}
