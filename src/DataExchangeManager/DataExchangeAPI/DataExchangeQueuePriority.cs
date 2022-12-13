using System;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public enum DataExchangeQueuePriority
    {
        High,
        Normal,
        Low,
        Undefined
    }

    public static class DataExchangeQueuePriorityConverter
    {
        public static DataExchangeQueuePriority FromString(string value)
        {
            DataExchangeQueuePriority priority;

            if(!Enum.TryParse(value, true, out priority))
            {
                priority = DataExchangeQueuePriority.Undefined;
            }

            return priority;
        }
    }
}
