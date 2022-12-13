using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;

namespace DataExchangeTestClient
{
    public class EventLevelComboboxItem
    {
        public EventLevelComboboxItem(EventLevelTypeEnum level)
        {
            _eventLevel = level;
        }

        private readonly EventLevelTypeEnum _eventLevel;

        public EventLevelTypeEnum EventLevel { get { return _eventLevel; } }

        public override string ToString()
        {
            switch (_eventLevel)
            {
                case EventLevelTypeEnum.Info:
                    return "info";
                case EventLevelTypeEnum.Warning:
                    return "warning";
                case EventLevelTypeEnum.Error:
                    return "error";
                case EventLevelTypeEnum.Fatal:
                    return "fatal";
                default:
                    return "<invalid event level type>";
            }
        }
    }
}
