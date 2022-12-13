namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public interface IDataExchangeSettingsFactory
    {
        int ExportIndex { get; set; }
        int ImportIndex { get; set; }
        DataExchangeSettings GetSettings();
    }
}