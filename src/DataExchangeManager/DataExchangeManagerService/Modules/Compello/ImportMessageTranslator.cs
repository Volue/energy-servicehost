using System.Globalization;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public class ImportMessageTranslator : IImportMessageTranslator
    {
        private readonly IMetaDataValueProvider _metaDataValueProvider;
        private readonly ISettingsProvider _settingsProvider;

        public ImportMessageTranslator(IMetaDataValueProvider metaDataValueProvider, ISettingsProvider settingsProvider)
        {
            _metaDataValueProvider = metaDataValueProvider;
            _settingsProvider = settingsProvider;
        }

        public DataExchangeImportMessage Translate(ImportMessage importMessage, bool areMetaDataMandatory = true)
        {
            var m =  new DataExchangeImportMessage()
            {
                Version = DataExchangeImportMessage.LatestVersion,
                Priority = _metaDataValueProvider.GetMetadataValue(importMessage.Metadata, "priority", areMetaDataMandatory),
                Protocol = _metaDataValueProvider.GetMetadataValue(importMessage.Metadata, "protocol", areMetaDataMandatory),
                Country = _metaDataValueProvider.GetMetadataValue(importMessage.Metadata, "country", false),
                RoutingAddress = _settingsProvider.GetRoutingAddressForImport(),
                ExternalReference = importMessage.MessageId.ToString(CultureInfo.InvariantCulture),
                ExternalText = _metaDataValueProvider.GetMetadataValue(importMessage.Metadata, "externaltext", false),
                SubAddress = _metaDataValueProvider.GetMetadataValue(importMessage.Metadata, "subaddress", false),
                SenderName = _metaDataValueProvider.GetMetadataValue(importMessage.Metadata, "sendername", false),
                ProductCode = _metaDataValueProvider.GetMetadataValue(importMessage.Metadata, "prodcode", false)
            };
            m.SetMessageData(importMessage.Data,null);
            return m;
        }
    }
}
