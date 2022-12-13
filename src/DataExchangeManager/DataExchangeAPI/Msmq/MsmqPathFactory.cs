namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq
{
    public class MsmqPathFactory
    {
        private const string PRIVATE_QUEUE_PREFIX = @"Private$\";
        private const string DEFAULT_EXPORT_QUEUE_PREFIX = @"ICC_EXPORT_";
        private const string DEFAULT_IMPORT_QUEUE_PREFIX = @"ICC_IMPORT_";

        private const string OS_PATH_FORMAT = @"FormatName:Direct=OS:{0}\{1}{2}";
        private const string TCP_PATH_FORMAT = @"FormatName:Direct=TCP:{0}\{1}{2}";
        private const string LOCAL_PATH_FORMAT = @".\{0}{1}";

        private readonly IDataExchangeSettingsFactory _settingsFactory;

        public MsmqPathFactory(IDataExchangeSettingsFactory settingsFactory)
        {
            _settingsFactory = settingsFactory;
        }

        public MsmqPath CreateInternalExportQueuePath(string queueIdentifier)
        {
            const string prefix = PRIVATE_QUEUE_PREFIX + DEFAULT_EXPORT_QUEUE_PREFIX;
            string machineName = (_settingsFactory.GetSettings().ExportQueueMachineName ?? string.Empty).Trim();
            return CreateQueuePath(prefix, machineName, queueIdentifier);
        }

        public MsmqPath CreateInternalImportQueuePath()
        {
            const string prefix = PRIVATE_QUEUE_PREFIX + DEFAULT_IMPORT_QUEUE_PREFIX;
            string machineName = (_settingsFactory.GetSettings().ImportQueueMachineName ?? string.Empty).Trim();
            return CreateQueuePath(prefix, machineName, string.Empty);
        }

        public MsmqPath CreateCustomImportQueuePath(string machineName, string queueIdentifier)
        {
            // We do not want the ICC_ prefix on custom queues
            const string prefix = PRIVATE_QUEUE_PREFIX;
            machineName = (machineName ?? string.Empty).Trim();
            return CreateQueuePath(prefix, machineName, queueIdentifier);
        }

        private MsmqPath CreateQueuePath(string prefix, string machineName, string queueIdentifier)
        {
            var machineNameType = MachineNameTypeUtility.DetermineMachineNameType(machineName);

            MsmqPath msmqPath = new MsmqPath
                {
                    FullPath = GetFullPath(machineNameType, machineName, prefix, queueIdentifier),
                    MachineNameType = machineNameType
                };

            return msmqPath;
        }

        private static string GetFullPath(MachineNameType machineNameType, string machineName, string prefix, string queueIdentifier)
        {
            string fullPath = null;

            switch (machineNameType)
            {
                case MachineNameType.Os:
                    fullPath = string.Format(OS_PATH_FORMAT, machineName, prefix, queueIdentifier);
                    break;
                case MachineNameType.Tcp:
                    fullPath = string.Format(TCP_PATH_FORMAT, machineName, prefix, queueIdentifier);
                    break;
                case MachineNameType.Local:
                    fullPath = string.Format(LOCAL_PATH_FORMAT, prefix, queueIdentifier);
                    break;
            }
 
            if (fullPath != null && fullPath.Substring(fullPath.Length - 1) == "_")
            {
                fullPath = fullPath.Substring(0, fullPath.Length - 1);
            }

            return fullPath;
        }
    }
}
