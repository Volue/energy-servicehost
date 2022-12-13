using System;
using System.IO;
using System.Text;
using log4net;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO
{
    public class FileUtility : IFileUtility
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public string ImportFilePrefix { get; set; }
        public string ImportFileExtension { get; set; }

        private readonly IEventLogModuleItem _iccLog;

        public FileUtility(IEventLogModuleItem iccLog)
        {
            _iccLog = iccLog;
        }

        public void DeleteFile(string fullPath)
        {
            try
            {
                if (File.Exists(fullPath))
                {
                    File.Delete(fullPath);
                }
            }
            catch (Exception e)
            {
                LogError(string.Format("Unable to delete the file with path {0}", fullPath));
                Log.Error(e.Message);
            }
        }

        public string SaveImportToFile(DataExchangeImportMessage message, string importDir,Encoding encoding)
        {
            if (!Directory.Exists(importDir))
            {
                Directory.CreateDirectory(importDir);
            }

            string importFileName = String.Format("{0}{1:yyyyMMddHHmmss}_{2}.{3}", ImportFilePrefix, DateTime.Now, Path.GetRandomFileName(), ImportFileExtension);

            using (var fStream = new FileStream(Path.Combine(importDir, importFileName), FileMode.Create, FileAccess.Write))
            {
                // Use encoding Windows-1252 (code page for western europe), since this format is presently used for export and import files
                using (var sWriter = encoding == null ? new StreamWriter(fStream) : new StreamWriter(fStream, encoding))
                {
                    sWriter.Write(message.GetMessageData());
                }
            }

            Log.Debug($"Message written to {importFileName}");
            return importFileName;
        }


        private void LogError(string message)
        {
            if (_iccLog != null)
            {
                _iccLog.LogMessage(8000, message);
            }

            Log.Error(message);
        }
    }
}