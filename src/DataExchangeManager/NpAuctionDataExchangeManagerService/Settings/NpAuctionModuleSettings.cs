using Powel.Icc.Messaging.DataExchangeCommon.Settings;

namespace Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService.Settings
{
    public class NpAuctionModuleSettings : PushExportModuleSettings
    {
        // Default values and functions for fetching ConfigurationManager.AppSettings[Modulename.*] values.
        internal const string Modulename = "NpAuctionDataExchangeManager";
        public override string ModuleName => Modulename;
        private const string ExportParametersFileDefault = "NpAuctionExportParametersFile.json";
        private const string ExportParametersFileFilterDefault = "NpAuctionExportParametersFile.json";
        private const string ExportParametersFilePathDefault = @"%ICC_HOME%\bin";
        private const string AuctionUrlDefault = "https://auctions-api.test.nordpoolgroup.com/api/v1";    // https://auctions-api.nordpoolgroup.com/api/v1
        private const string SsoUrlDefault = "https://sts.test.nordpoolgroup.com";    // https://sts.nordpoolgroup.com/connect/token
        public string ExportParametersFile => GetStringFromConfig(() => ExportParametersFile) ?? ExportParametersFileDefault;
        public string ExportParametersFileFilter => GetStringFromConfig(() => ExportParametersFileFilter) ?? ExportParametersFileFilterDefault;
        public string ExportParametersFilePath => GetStringFromConfig(() => ExportParametersFilePath) ?? ExportParametersFilePathDefault;
        public string AuctionUrl => GetStringFromConfig(() => AuctionUrl) ?? AuctionUrlDefault;
        public string SsoUrl => GetStringFromConfig(() => SsoUrl) ?? SsoUrlDefault;
    }
}
