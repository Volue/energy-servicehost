using Powel.Icc.Messaging.DataExchangeCommon.Settings;

namespace Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService.Settings
{
    public class AzureModuleSettings : DataExchangeModuleSettings
    {
        internal const string Modulename = "AzureDataExchangeManager";
        public override string ModuleName => Modulename;

        private const string ImportParametersFileFilterDefault = "AzureImportParametersFile.json";
        private const string ImportParametersFilePathDefault = ".";
        private const int MaxConcurrentImportsDefault = 2;

        public string ImportParametersFileFilter => GetStringFromConfig(() => ImportParametersFileFilter) ?? ImportParametersFileFilterDefault;
        public string ImportParametersFilePath => GetStringFromConfig(() => ImportParametersFilePath) ?? ImportParametersFilePathDefault;
        public int MaxConcurrentImports => GetIntFromConfig(() => MaxConcurrentImports, MaxConcurrentImportsDefault);
        public bool UsePumpingReceiver => GetBoolFromConfig(() => UsePumpingReceiver, false);
    }
}
