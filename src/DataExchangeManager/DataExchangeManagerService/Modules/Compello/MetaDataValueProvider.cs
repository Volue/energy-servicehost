using System.Collections.Generic;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public class MetaDataValueProvider : IMetaDataValueProvider
    {
        public string GetMetadataValue(Dictionary<string, object> caseInsensitiveMetadata, string keyName, bool mandatory)
        {
            if (caseInsensitiveMetadata == null && mandatory)
            {
                throw new DataExchangeInvalidMetadataException("No metadata available.");
            }

            if (caseInsensitiveMetadata==null)
            {
                return string.Empty;
            }

            string value = null;

            if (caseInsensitiveMetadata.ContainsKey(keyName))
            {
                value = caseInsensitiveMetadata[keyName].ToString();            
            }

            if(mandatory && string.IsNullOrEmpty(value))
            {
                throw new DataExchangeInvalidMetadataException(string.Format("Field with name '{0}' was empty in the metadata, this field is required.", keyName));
            }

            return value;
        }
    }
}
