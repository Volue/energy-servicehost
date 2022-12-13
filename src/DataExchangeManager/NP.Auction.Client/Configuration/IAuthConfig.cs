using System;

namespace NP.Auction.Client.Configuration
{
    public interface IAuthConfig : IEquatable<IAuthConfig>
    {
        string Username { get; set; }
        string Password { get; set; }
        string ClientId { get; set; }
        string ClientSecret { get; set; }
        //string SsoUrl { get; set; }
    }
}