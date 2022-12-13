using System.Collections.Generic;

namespace DataExchange.Administration.ImportModule
{
    public interface IErroneousImportsRepository
    {
        IList<FailedImportModel> GetAll();
        void Delete(IList<FailedImportModel> failedImportsToDelete);
        void SaveAndImport(FailedImportModel failedImport);
    }
}
