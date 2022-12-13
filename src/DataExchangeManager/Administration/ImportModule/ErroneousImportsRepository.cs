using System;
using System.Collections.Generic;
using System.Linq;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Services.Time;
using Powel.Wpf.Common;

namespace DataExchange.Administration.ImportModule
{
    public class ErroneousImportsRepository : IErroneousImportsRepository
    {
        private readonly IDataExchangeApi _dataExchangeApi;
        private readonly IDateTimeUtility _dateTimeUtility;
        private readonly Func<RegionalCalendar> _regionalCalendarFactory;

        public ErroneousImportsRepository(IDataExchangeApi dataExchangeApi, IDateTimeUtility dateTimeUtility, Func<RegionalCalendar> regionalCalendarFactory)
        {
            _dataExchangeApi = dataExchangeApi;
            _dateTimeUtility = dateTimeUtility;
            _regionalCalendarFactory = regionalCalendarFactory;
        }

        public IList<FailedImportModel> GetAll()
        {
            IList<FailedImportModel> failedImports = new List<FailedImportModel>();
            IDictionary<string, DataExchangeImportMessage> erronousImportMessages;

            using (var transaction = _dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.Dequeue))
            {
                transaction.Begin();
                erronousImportMessages = _dataExchangeApi.GetAllErroneousImportMessages(transaction);
                transaction.Commit();
            }

            if (erronousImportMessages != null)
            {
                foreach (string messageId in erronousImportMessages.Keys)
                {
                    DataExchangeImportMessage importMessage = erronousImportMessages[messageId];

                    DateTime enqueuedTimePresentation = _regionalCalendarFactory().ToDateTime(new UtcTime(importMessage.EnqueuedTimeUtc.Ticks));

                    var item = new FailedImportModel
                                   {
                                       ImportProperties = new List<FailedImportProperty>
                                           {
                                               new FailedImportProperty(FailedImportPropertyName.ExternalReference, importMessage.ExternalReference),
                                               new FailedImportProperty(FailedImportPropertyName.Country, importMessage.Country),
                                               new FailedImportProperty(FailedImportPropertyName.Protocol, importMessage.Protocol),
                                               new FailedImportProperty(FailedImportPropertyName.SubAddress, importMessage.SubAddress),
                                               new FailedImportProperty(FailedImportPropertyName.ProductCode, importMessage.ProductCode),
                                               new FailedImportProperty(FailedImportPropertyName.SenderName, importMessage.SenderName),
                                               new FailedImportProperty(FailedImportPropertyName.ExternalText, importMessage.ExternalText)
                                           },
                                       Timestamp = _dateTimeUtility.ConvertDateTimeToDateTimeString(enqueuedTimePresentation),
                                       ImportMessage = importMessage.GetMessageData(),
                                       InternalId = messageId
                                   };

                    failedImports.Add(item);
                }
            }

            return failedImports;
        }

        public void Delete(IList<FailedImportModel> failedImportsToDelete)
        {
            var ids = failedImportsToDelete.Select(failedImportModel => failedImportModel.InternalId).ToList();

            using (var transaction = _dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.EnqueueAndRemoteDequeue))
            {
                transaction.Begin();

                _dataExchangeApi.RemoveErroneousImportMessages(ids, transaction);

                transaction.Commit();
            }
        }
        
        public void SaveAndImport(FailedImportModel failedImport)
        {
            using (var transaction = _dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.EnqueueAndDequeue))
            {
                transaction.Begin();
                DataExchangeImportMessage importMessage = _dataExchangeApi.DequeueErroneousImportMessage(failedImport.InternalId, transaction);
                if (importMessage != null)
                {
                    importMessage.ExternalReference = failedImport.ExternalReference;
                    importMessage.Country = failedImport.Country;
                    importMessage.Protocol = failedImport.Protocol;
                    importMessage.SubAddress = failedImport.SubAddress;
                    importMessage.ProductCode = failedImport.ProductCode;
                    importMessage.SenderName = failedImport.SenderName;
                    importMessage.ExternalText = failedImport.ExternalText;
                    _dataExchangeApi.EnqueueImportMessage(importMessage, transaction);
                    transaction.Commit();
                }
            }
        }
    }
}
