using System;
using System.Collections.Generic;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;

namespace Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager
{
    public class MassTransitFileWatcherDataExchangeManagerService : DataExchangeManagerServiceBase
    {
        public MassTransitFileWatcherDataExchangeManagerService(IServiceEventLogger serviceEventLogger, Func<IEnumerable<IDataExchangeModule>> dataExchangeModuleFactory)
            : base(serviceEventLogger,dataExchangeModuleFactory)
        { }

        public override string ServiceIterationName => "MassTransit to File Watcher Data Exchange Manager Service";

        public override IccModule ServiceIterationModule => IccModule.M_MASSTRANSIT_FILEWATCHER_DATA_EXCHANGE_MANAGER;
    }
}
