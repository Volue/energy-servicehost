namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService
{
    public interface IExternalEventLogger
    {
        void Info(string moduleName, string message);
        void Warning(string moduleName, string message);
        void Error(string moduleName, string message);
        void Fatal(string moduleName, string message);
    }
}