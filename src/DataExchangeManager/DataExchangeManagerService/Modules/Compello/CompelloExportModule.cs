using System.Collections.Generic;
using System.Globalization;
using System.IO;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Api.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Builders.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Exceptions;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Validators.Abstract;
using System.Text;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public class CompelloExportModule : PushExportModule
    {
        public const string MODULE_NAME = "COMPELLO";

        private readonly IApiWrapperFactory _compelloApiFactory;
        private readonly IDataExchangeExportMessageValidator _messageValidator;
        private readonly IExportSettingsBuilder _settingsBuilder;

        public CompelloExportModule(
            IApiWrapperFactory compelloApiFactory,
            IDataExchangeManagerServiceSettingsFactory dataExchangeManagerServiceSettingsFactory,
            MessageExporter messageExporter, 
            IServiceEventLogger serviceEventLogger, 
            IExportSettingsBuilder settingsBuilder, 
            IDataExchangeExportMessageValidator messageValidator)
            : base(dataExchangeManagerServiceSettingsFactory, messageExporter, serviceEventLogger)
        {
            _compelloApiFactory = compelloApiFactory;
            _settingsBuilder = settingsBuilder;
            _messageValidator = messageValidator;
        }

        public override string ModuleName
        {
            get { return MODULE_NAME; }
        }

        protected override string SendExportMessage(DataExchangeExportMessage exportMessage)
        {
            Validate(exportMessage);

            var settings = _settingsBuilder.Build(exportMessage);
            var metaData = new Dictionary<string, object>();

            using (var stream = new MemoryStream())
            using (var writer = new StreamWriter(stream,Encoding.GetEncoding("Windows-1252")))
            {
                writer.Write(exportMessage.GetMessageData());
                writer.Flush();
                stream.Seek(0, SeekOrigin.Begin);

                var compelloClient = _compelloApiFactory.Create(settings);
                var compelloExternalReference = compelloClient.SendMessage(stream, metaData).ToString(CultureInfo.InvariantCulture);
                Log.Info($"{MODULE_NAME}: Sent ExternalReference: {exportMessage.ExternalReference}, Compello ID: {compelloExternalReference}");
                return compelloExternalReference;
            }
        }

        private void Validate(DataExchangeExportMessage exportMessage)
        {
            try
            {
                _messageValidator.Validate(exportMessage);
            }
            catch (DataExchangeInvalidRoutingAddressException ex)
            {
                LogError(ex.Message);

                throw;
            }
        }
    }
}