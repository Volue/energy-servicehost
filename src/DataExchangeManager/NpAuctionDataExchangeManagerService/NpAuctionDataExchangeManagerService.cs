using System;
using System.Collections.Generic;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;

namespace Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService
{
    public class NpAuctionDataExchangeManagerService : DataExchangeManagerServiceBase
    {
        public NpAuctionDataExchangeManagerService(IServiceEventLogger serviceEventLogger, Func<IEnumerable<IDataExchangeModule>> dataExchangeModuleFactory)
            : base(serviceEventLogger,dataExchangeModuleFactory)
        { }

        public override string ServiceIterationName => "Nord Pool's Auction Data Exchange Manager Service";

        public override IccModule ServiceIterationModule => IccModule.M_NP_AUCTION_DATA_EXCHANGE_MANAGER;

    }
}
