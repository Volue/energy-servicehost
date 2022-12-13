using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerService.Settings
{
    internal class SettingsRepository
    {
        public ImportSettings GetSettingsFromEnvironmentVariables()
        {
            return new ImportSettings
                {
                    TradeHandlesInnmating = GetBoolFromEnvironmentVariable("TRADE_HANDLES_INNMATING"),
                    TradeAvregnToEdkin = GetBoolFromEnvironmentVariable("TRADE_AVREGN_TO_EDKIN"),
                    TradeTimerToEdkin = GetBoolFromEnvironmentVariable("TRADE_TIMER_TO_EDKIN"),
                    TradeStatnettAddress = GetStringFromEnvironmentVariable("TRADE_STATNETT_ADDRESS"),
                    TradeSvkAddress = GetStringFromEnvironmentVariable("TRADE_SVK_ADDRESS"),
                    TradeEdiimpProductCodes = GetStringListFromCommaSeparatedEnvironmentVariable("TRADE_EDIIMP_PCODES"),
                    TradeDelforProductCodes = GetStringListFromCommaSeparatedEnvironmentVariable("TRADE_DELFOR_PCODES"),
                    EdkFilesDirectory = GetStringFromEnvironmentVariable("EDK_FILES_DIR"),
                    EdiImportDirectory = GetStringFromEnvironmentVariable("ICC_IMPORT_DIR"),
                    //@"C:\Powel\IccData\import\EDI\new"//Path.GetTempPath()
                };            
        }

        private string GetStringFromEnvironmentVariable(string name)
        {
            return Environment.GetEnvironmentVariable(name) ?? string.Empty;
        }

        private IList<string> GetStringListFromCommaSeparatedEnvironmentVariable(string name)
        {
            // Split the string on comma, add all non-empty tokens to the result
            var temp = new List<string>(GetStringFromEnvironmentVariable(name).Split(','));
            return temp.Select(value => value.Trim()).Where(trimmedValue => !string.IsNullOrEmpty(trimmedValue)).ToList();
        }

        private bool GetBoolFromEnvironmentVariable(string name)
        {
            if((Environment.GetEnvironmentVariable(name) ?? string.Empty).ToUpper() == "TRUE")
            {
                return true;
            }

            return false;
        }
    }
}
