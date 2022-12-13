using System;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract
{
    public abstract class PushExportModule : TaskBasedModule
    {
        private readonly MessageExporter _messageExporter;
        private readonly TimeSpan _sleepTime;
        private readonly TimeSpan _dequeueTimeout;

        protected PushExportModule(
            IDataExchangeManagerServiceSettingsFactory dataExchangeManagerServiceSettingsFactory,
            MessageExporter messageExporter, 
            IServiceEventLogger serviceEventLogger) 
            : base(serviceEventLogger)
        {
            _messageExporter = messageExporter;
            
            _sleepTime = TimeSpan.FromMilliseconds(
                dataExchangeManagerServiceSettingsFactory.GetSettings().TimeoutBeforeSendExportRetryInMs);
            _dequeueTimeout = TimeSpan.FromSeconds(5);
        }

        protected override TimeSpan SleepTime
        {
            get { return _sleepTime; }
        }

        /// <returns>externalReference</returns>
        protected abstract string SendExportMessage(DataExchangeExportMessage export);

        protected sealed override bool TryExecuteSingleTask()
        {
            return _messageExporter.ExportMessage(
                ModuleName,
                _dequeueTimeout,
                SendExportMessage);
        }
    }
}
