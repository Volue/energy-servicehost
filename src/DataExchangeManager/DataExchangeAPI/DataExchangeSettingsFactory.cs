using log4net;
using log4net.Util;
using Powel.Icc.Common;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public class DataExchangeSettingsFactory : IDataExchangeSettingsFactory
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        
        public static bool Testing = false;
        public static string ExportMachineNames { get; set; } = null;
        public static string ImportMachineNames { get; set; } = null;


        private int _exportIndex = 0;
        public int ExportIndex
        {
            get => _exportIndex;
            set 
            { 
                _exportIndex = value;
                _currentSettings = null;
            }
        }

        private int _importIndex = 0;

        public int ImportIndex
        {
            get => _importIndex;
            set
            {
                _importIndex = value;
                _currentSettings = null;
            }
        }

        private readonly string _exportMachineNames;
        private readonly string _importMachineNames;
        private readonly bool _storeExportMessages;
        private DataExchangeSettings _currentSettings = null;

        public DataExchangeSettingsFactory()
        {
            _exportMachineNames = Testing ? ExportMachineNames : IccConfiguration.DataExchangeManager.MsmqMachineExport;
            _importMachineNames = Testing ? ImportMachineNames : IccConfiguration.DataExchangeManager.MsmqMachineImport;
            _storeExportMessages = IccConfiguration.DataExchangeManager.MsmqStoreExportMessages;
        }
        public DataExchangeSettings GetSettings()
        {
            return _currentSettings ?? GetSettings(_exportMachineNames, _importMachineNames, _storeExportMessages);
        }

        private DataExchangeSettings GetSettings(string exportQueueMachineName, string importQueueMachineName, bool storeExportMessages)
        {
            var currentExportMachine = exportQueueMachineName;
            var currentImportMachine = importQueueMachineName;
            if (!string.IsNullOrEmpty(exportQueueMachineName) && exportQueueMachineName.Contains(";"))
            {
                var exportMachineList = exportQueueMachineName.Split(';');
                if (ExportIndex >= exportMachineList.Length)
                    ExportIndex = 0;
                currentExportMachine = exportMachineList[ExportIndex];
            }

            if (!string.IsNullOrEmpty(importQueueMachineName) && importQueueMachineName.Contains(";"))
            {
                var importMachineList = importQueueMachineName.Split(';');
                if (ImportIndex >= importMachineList.Length)
                    ImportIndex = 0;
                currentImportMachine = importMachineList[ImportIndex];
            }
            Log.DebugExt($"Queue machine names: Export[{ExportIndex}]={(string.IsNullOrEmpty(currentExportMachine) ? "not specified" : currentExportMachine)}, Import[{ImportIndex}]={(string.IsNullOrEmpty(currentImportMachine) ? "not specified" : currentImportMachine)}, StoreExportMessages={storeExportMessages}");
            _currentSettings = new DataExchangeSettings
            {
                ExportQueueMachineName = currentExportMachine,
                ImportQueueMachineName = currentImportMachine,
                StoreExportMessages = storeExportMessages
            };

            return _currentSettings;
        }
    }
}
