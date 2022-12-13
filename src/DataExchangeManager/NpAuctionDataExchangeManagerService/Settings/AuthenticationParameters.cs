using NP.Auction.Client.Configuration;

namespace Powel.Icc.Messaging.NpAuctionDataExchangeManager.Settings
{
    class AuthenticationParameters : IAuthConfig
    {
        public string Username { get; set; }
        public string Password { get; set; }
        public string ClientId { get; set; }
        public string ClientSecret { get; set; }

        public bool Equals(IAuthConfig other)
        {
            return Username == other.Username && Password == other.Password && ClientId == other.ClientId && ClientSecret == other.ClientSecret;
        }
    }
}
