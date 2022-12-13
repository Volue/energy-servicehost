namespace Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService.Modules.Common.Abstract
{
    public interface IFactory<T>
    {
        T Create();
    }
}