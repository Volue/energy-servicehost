namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
{
    public interface IFactory<T>
    {
        T Create();
    }
}