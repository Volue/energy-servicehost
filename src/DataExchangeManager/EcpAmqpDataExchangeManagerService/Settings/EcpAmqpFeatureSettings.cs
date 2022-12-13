using Powel.Icc.Messaging.DataExchangeCommon.Settings;

namespace Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Settings
{
    public class EcpAmqpModuleSettings : PushExportModuleSettings
    {
        // Default values and functions for fetching ConfigurationManager.AppSettings[Modulename.*] values.
        internal const string Modulename = "EcpAmqpDataExchangeManager";
        public override string ModuleName => Modulename;

        private const string ExportParametersFileDefault = "EcpAmqpExportParametersFile.json";
        private const string ExportParametersFileFilterDefault = "EcpAmqpExportParametersFile.json";
        private const string ExportParametersFilePathDefault = @"%ICC_HOME%\bin";   // the service run in %WinDir%\SysWOW64 or System32. We cannot have our parameter files there.
        private const string ImportParametersFileDefault = "EcpAmqpImportParametersFile.json";
        private const string ImportParametersFileFilterDefault = "EcpAmqpImportParametersFile.json";
        private const string ImportParametersFilePathDefault = @"%ICC_HOME%\bin";
        private const string StatusParametersFileDefault = "EcpAmqpStatusParametersFile.json";
        private const string StatusParametersFileFilterDefault = "EcpAmqpStatusParametersFile.json";
        private const string StatusParametersFilePathDefault = @"%ICC_HOME%\bin";
        //private const int MaxConcurrentImportsDefault = 2;
        public string ExportParametersFile => GetStringFromConfig(() => ExportParametersFile) ?? ExportParametersFileDefault;
        public string ExportParametersFileFilter => GetStringFromConfig(() => ExportParametersFileFilter) ?? ExportParametersFileFilterDefault;
        public string ExportParametersFilePath => GetStringFromConfig(() => ExportParametersFilePath) ?? ExportParametersFilePathDefault;
        public string ImportParametersFile => GetStringFromConfig(() => ImportParametersFile) ?? ImportParametersFileDefault;
        public string ImportParametersFileFilter => GetStringFromConfig(() => ImportParametersFileFilter) ?? ImportParametersFileFilterDefault;
        public string ImportParametersFilePath => GetStringFromConfig(() => ImportParametersFilePath) ?? ImportParametersFilePathDefault;
        public string StatusParametersFile => GetStringFromConfig(() => StatusParametersFile) ?? StatusParametersFileDefault;
        public string StatusParametersFileFilter => GetStringFromConfig(() => StatusParametersFileFilter) ?? StatusParametersFileFilterDefault;
        public string StatusParametersFilePath => GetStringFromConfig(() => StatusParametersFilePath) ?? StatusParametersFilePathDefault;
        //public int MaxConcurrentImports => GetIntFromConfig(() => MaxConcurrentImports, MaxConcurrentImportsDefault);
        public bool UsePumpingReceiver => GetBoolFromConfig(() => UsePumpingReceiver,false);
    }
}
