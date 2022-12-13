using System;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Settings;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeCommon.Abstract
{
    public abstract class PushExportModule : TaskBasedModule
    {
        private readonly MessageExporter _messageExporter;
        private readonly TimeSpan _sleepTime;
        private readonly TimeSpan _dequeueTimeout;
        protected readonly PushExportModuleSettings _settings;

        protected PushExportModule(PushExportModuleSettings settings, MessageExporter messageExporter, IServiceEventLogger serviceEventLogger)
            : base(serviceEventLogger)
        {
            _settings = settings;
            _messageExporter = messageExporter;

            _sleepTime = settings.ExportPollingInterval; // 10000
            _dequeueTimeout = settings.MsmqDequeueTimeout;    // 5000
        }

        protected override TimeSpan SleepTime => _sleepTime;

        /// <returns>externalReference</returns>
        protected abstract string SendExportMessage(DataExchangeExportMessage export);

        protected sealed override bool TryExecuteSingleTask()
        {
            return _messageExporter.ExportMessage(ModuleName,_dequeueTimeout,SendExportMessage);
        }
    }
}
