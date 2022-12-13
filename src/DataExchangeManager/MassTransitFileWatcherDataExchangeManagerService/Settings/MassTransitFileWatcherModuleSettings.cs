using Powel.Icc.Messaging.DataExchangeCommon.Settings;

namespace Powel.Icc.Messaging.MassTransitFileWatcherDataExchangeManager.Settings
{
    public class MassTransitFileWatcherModuleSettings : PushExportModuleSettings
    {
        // Default values and functions for fetching ConfigurationManager.AppSettings[Modulename.*] values.
        internal const string Modulename = "MassTransitFileWatcherDataExchangeManager";
        public override string ModuleName => Modulename;
        private const string ExportParametersFileDefault = "MassTransitFileWatcherExportParametersFile.json";
        private const string ExportParametersFileFilterDefault = ExportParametersFileDefault;
        private const string ExportParametersFilePathDefault = @"%ICC_HOME%\bin";
        private const string ImportParametersFileDefault = "MassTransitFileWatcherImportParametersFile.json";
        private const string ImportParametersFileFilterDefault = ImportParametersFileDefault;
        private const string ImportParametersFilePathDefault = @"%ICC_HOME%\bin";
        private const string RabbitMqHostDefault = "amqp://guest:guest@localhost";
        public string ExportParametersFile => GetStringFromConfig(() => ExportParametersFile) ?? ExportParametersFileDefault;
        public string ExportParametersFileFilter => GetStringFromConfig(() => ExportParametersFileFilter) ?? ExportParametersFile;
        public string ExportParametersFilePath => GetStringFromConfig(() => ExportParametersFilePath) ?? ExportParametersFilePathDefault;
        public string ImportParametersFile => GetStringFromConfig(() => ImportParametersFile) ?? ImportParametersFileDefault;
        public string ImportParametersFileFilter => GetStringFromConfig(() => ImportParametersFileFilter) ?? ImportParametersFile;
        public string ImportParametersFilePath => GetStringFromConfig(() => ImportParametersFilePath) ?? ImportParametersFilePathDefault;
        public string RabbitMqHost => GetStringFromConfig(() => RabbitMqHost) ?? RabbitMqHostDefault;
    }
}
