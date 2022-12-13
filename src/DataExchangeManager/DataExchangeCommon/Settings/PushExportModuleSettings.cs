using System;

namespace Powel.Icc.Messaging.DataExchangeCommon.Settings
{
    public abstract class PushExportModuleSettings : DataExchangeModuleSettings
    {
        private const double ExportPollingIntervalDefault = 1000;
        private const double MsmqDequeueTimeoutDefault = 1000;
        public TimeSpan ExportPollingInterval => TimeSpan.FromMilliseconds(GetDoubleFromConfig(() => ExportPollingInterval,ExportPollingIntervalDefault));
        public TimeSpan MsmqDequeueTimeout => TimeSpan.FromMilliseconds(GetDoubleFromConfig(() => MsmqDequeueTimeout, MsmqDequeueTimeoutDefault));
    }
}
