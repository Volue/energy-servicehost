namespace Powel.Icc.Messaging.DataExchangeManager.NordPoolRequestCommand
{
    public class NordPoolErrorMessage
    {
        public int StatusCode { get; set; } = 0;    // Typically HTTP status codes.
        public string Message { get; set; } = string.Empty;
    }
}
