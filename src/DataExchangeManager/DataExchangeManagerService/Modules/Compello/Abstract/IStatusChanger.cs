using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract
{
    public interface IStatusChanger
    {
        bool TryChangeStatus(StatusChangeEventArgs newStatus);
    }
}