using System;
using System.IO;
using System.Text;
using Powel.Icc.Common;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.FileWriter
{
    public class DataExchangeFileWriter : IDataExchangeFileWriter
    {
        public void LogExportMessageToFile(DataExchangeExportMessage message)
        {
            string path = IccConfiguration.ImportExport.IccExportDir + Path.PathSeparator + "DataExchange";

            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }

            string filename = String.Format("{0:yyyyMMddHHmmss}_{1}.exp", DateTime.Now, Path.GetRandomFileName());
            string fullPath = Path.Combine(path,filename);

            using (var fStream = new FileStream(fullPath, FileMode.Create, FileAccess.Write))
            {
                // Write UTF-8 without BOM
                using (var sWriter = new StreamWriter(fStream, new UTF8Encoding(false)))
                {
                    sWriter.Write(message.GetMessageData());
                }
            }

            // TODO: should we update MESSAGE_HEADER with the filename?
        }
    }
}
