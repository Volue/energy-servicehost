using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using log4net;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public class MessageImporter : IMessageImporter
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        private readonly IDataExchangeApi _dataExchangeApi;
        private readonly IImportMessageTranslator _importMessageTranslator;
        private readonly IServiceEventLogger _serviceEventLogger;

        public MessageImporter(IDataExchangeApi dataExchangeApi, IImportMessageTranslator importMessageTranslator, IServiceEventLogger eventLogger)
        {
            _dataExchangeApi = dataExchangeApi;
            _importMessageTranslator = importMessageTranslator;
            _serviceEventLogger = eventLogger;
        }

        public SubmitImportResponse Import(ImportMessage message)
        {
            Log.Debug($"Metadata:{(message.Metadata == null ? "null" : string.Join(";",message.Metadata.Select(md => $"Key:{md.Key}, Value:{md.Value ?? "null"}")))}");
            var caseInsensitiveMetadata = GetCaseInsensitiveMetadata(message.Metadata);
            var caseInsensitiveMessage = new ImportMessage(message.MessageId, message.Data, caseInsensitiveMetadata);

            SubmitImportResponse response;
            try
            {
                var dataExchangeImportMessage = _importMessageTranslator.Translate(caseInsensitiveMessage);

                response = AddMessageToImportQueue(dataExchangeImportMessage);
            }
            catch (DataExchangeInvalidMetadataException ex)
            {
                var dataExchangeImportMessage = _importMessageTranslator.Translate(caseInsensitiveMessage, false);
                AddMessageToImportErrorQueue(dataExchangeImportMessage);

                response = CreateImportResponse(false, ex.Message);

                Log.Error("Message from Compello server has arrived with insufficient metadata and placed in the import error queue.", ex);
               _serviceEventLogger.LogMessage(30264);
            }
            catch (Exception ex)
            {
                Log.Error("Failed to enqueue import message", ex);

                response = CreateImportResponse(false, string.Format("Internal error ('{0}')", ex.Message));
            }

            return response;
        }

        private static Dictionary<string, object> GetCaseInsensitiveMetadata(Dictionary<string, object> metadata)
        {
            if (metadata == null)
            {
                return null;
            }

            return new Dictionary<string, object>(metadata, StringComparer.OrdinalIgnoreCase);
        }

        private void AddMessageToImportErrorQueue(DataExchangeImportMessage dataExchangeImportMessage)
        {
            using (var transaction = _dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
            {
                transaction.Begin();

                _dataExchangeApi.EnqueueErroneousImportMessage(dataExchangeImportMessage, transaction);

                transaction.Commit();
            }          
        }

        private SubmitImportResponse AddMessageToImportQueue(DataExchangeImportMessage dataExchangeImportMessage)
        {
            using (var transaction = _dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
            {
                transaction.Begin();

                _dataExchangeApi.EnqueueImportMessage(dataExchangeImportMessage, transaction);

                transaction.Commit();
                Log.Debug("Message transaction committed");
            }

            return CreateImportResponse(true, null);
        }

        private SubmitImportResponse CreateImportResponse(bool isOk, string response)
        {
            return new SubmitImportResponse
                {
                    RequestOk = isOk,
                    RequestNotOkReasonText = response
                };
        }
    }
}