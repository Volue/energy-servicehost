namespace Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService.Settings.Abstract
{
    public interface IAuthConfig
    {
        string Username { get; }
        string Password { get; }
        string ClientId { get; }
        string ClientSecret { get; }
        string SsoUrl { get; }
    }
}
