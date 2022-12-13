using Powel.Icc.Messaging.DataExchangeCommon.Settings;

namespace Powel.Icc.Messaging.IccpDataExchangeManager.Settings
{
    public class IccpModuleSettings : DataExchangeModuleSettings
    {
        // Default values and functions for fetching ConfigurationManager.AppSettings[Modulename.*] values.
        internal const string Modulename = "IccpDataExchangeManager";
        public override string ModuleName => Modulename;

        private const string ImportParametersFileDefault = "IccpImportParametersFile.json";
        private const string ImportParametersFileFilterDefault = "IccpImportParametersFile.json";
        private const string ImportParametersFilePathDefault = @"%ICC_HOME%\bin";
        public string ImportParametersFile => GetStringFromConfig(() => ImportParametersFile) ?? ImportParametersFileDefault;
        public string ImportParametersFileFilter => GetStringFromConfig(() => ImportParametersFileFilter) ?? ImportParametersFileFilterDefault;
        public string ImportParametersFilePath => GetStringFromConfig(() => ImportParametersFilePath) ?? ImportParametersFilePathDefault;
        public bool UseDualRole => GetBoolFromConfig(() => UseDualRole, false);
    }
}
