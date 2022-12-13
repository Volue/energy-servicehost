namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService
{
    public class DataExchangeManagerServiceSettingsFactory:IDataExchangeManagerServiceSettingsFactory
    {
        public DataExchangeManagerServiceSettings GetSettings()
        {
            var settings = new DataExchangeManagerServiceSettings();
            settings.TimeoutBeforeSendExportRetryInMs = 5000;   // Should be smaller than Stop timeout.

            return settings;
        }
    }
}
