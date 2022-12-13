using System.Text;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract
{
    public interface IFileUtility
    {
        string ImportFileExtension { get; set; }

        void DeleteFile(string fullPath);
        string SaveImportToFile(DataExchangeImportMessage message, string importDir,Encoding encoding);
    }
}
