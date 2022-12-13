using System.Collections.Generic;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract
{
    public interface IMetaDataValueProvider
    {
        string GetMetadataValue(Dictionary<string, object> metaData, string keyName,
            bool mandatory);
    }
}