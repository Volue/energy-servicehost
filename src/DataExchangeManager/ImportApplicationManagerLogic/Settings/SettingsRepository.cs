using System;
using System.Collections.Generic;
using System.Linq;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings
{
    public class SettingsRepository
    {
        public ImportSettings GetSettingsFromEnvironmentVariables()
        {
            return new ImportSettings
                {
                    TradeHandlesInnmating = GetBoolFromValue(GetStringFromEnvironmentVariable("TRADE_HANDLES_INNMATING")),
                    TradeAvregnToEdkin = GetBoolFromValue(GetStringFromEnvironmentVariable("TRADE_AVREGN_TO_EDKIN")),
                    TradeTimerToEdkin = GetBoolFromValue(GetStringFromEnvironmentVariable("TRADE_TIMER_TO_EDKIN")),
                    TradeDelforToEdkin = GetBoolFromValue(GetStringFromEnvironmentVariable("TRADE_DELFOR_TO_EDKIN")),
                    TradeStatnettAddress = GetStringFromEnvironmentVariable("TRADE_STATNETT_ADDRESS"),
                    TradeSvkAddress = GetStringFromEnvironmentVariable("TRADE_SVK_ADDRESS"),
                    TradeEdiimpProductCodes = GetStringListFromCommaSeparatedValue(GetStringFromEnvironmentVariable("TRADE_EDIIMP_PCODES")),
                    TradeEdkinProductCodes = GetStringListFromCommaSeparatedValue(GetStringFromEnvironmentVariable("TRADE_DELFOR_PCODES")),
                    EdkFilesDirectory = GetStringFromEnvironmentVariable("EDK_FILES_DIR"),
                    EdiImportDirectory = GetStringFromEnvironmentVariable("ICC_IMPORT_DIR"),
                    AncitraMessageTypes = GetStringListFromCommaSeparatedValue(GetStringFromEnvironmentVariable("ICC_ANCITRA_MESSAGE_TYPES")),
                    AncitraMessageTypeUseRe = GetBoolFromValue(GetStringFromEnvironmentVariable("ICC_ANCITRA_MESSAGE_TYPE_USE_RE"))
            };            
        }

        private static string GetStringFromEnvironmentVariable(string name)
        {
            return Environment.GetEnvironmentVariable(name) ?? string.Empty;
        }

        public static IList<string> GetStringListFromCommaSeparatedValue(string value)
        {
            // Split the string on comma, add all non-empty tokens to the result
            var temp = new List<string>(value.Split(','));
            return temp.Select(v => v.Trim()).Where(trimmedValue => !string.IsNullOrEmpty(trimmedValue)).ToList();
        }

        public static bool GetBoolFromValue(string value)
        {
            return value?.ToUpper() == "TRUE" ? true : false;
        }
    }
}
