namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public interface ICompelloSettings
    {
        string Address { get; }
        int Port { get; }
        string ApiKey { get; }
    }
}