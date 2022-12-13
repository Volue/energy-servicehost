using System.Collections.Generic;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerService.Settings
{
    internal class ImportSettings
    {
        public bool TradeHandlesInnmating { get; set; }

        public bool TradeAvregnToEdkin { get; set; }

        public bool TradeTimerToEdkin { get; set; }

        public string TradeStatnettAddress { get; set; }

        public string TradeSvkAddress { get; set; }

        public IList<string> TradeEdiimpProductCodes { get; set; }

        public IList<string> TradeDelforProductCodes { get; set; }

        public string EdkFilesDirectory { get; set; }

        public string EdiImportDirectory { get; set; }
    }
}
