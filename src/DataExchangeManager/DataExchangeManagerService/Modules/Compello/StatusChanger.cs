using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model.Events;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello
{
    public class StatusChanger : IStatusChanger
    {
        private readonly IDataExchangeMessageLog _dataExchangeMessageLog;
        private readonly IServiceEventLogger _serviceEventLogger;
        private List<Restriction> _restrictions;

        public StatusChanger(IDataExchangeMessageLog dataExchangeMessageLog, IServiceEventLogger serviceEventLogger)
        {
            _dataExchangeMessageLog = dataExchangeMessageLog;
            _serviceEventLogger = serviceEventLogger;
            CreateRestrictions();
        }

        private void CreateRestrictions()
        {
            _restrictions = new List<Restriction>
                {
                    new Restriction(
                        new HashSet<TransLogMessageStatus>
                            {
                                TransLogMessageStatus.ExportSentWaitingForMoreAcknowledgement,
                                TransLogMessageStatus.ExportSentSuccessful,
                                TransLogMessageStatus.ExportSentPartlySuccessful,
                                TransLogMessageStatus.ExportSentFailed,
                                TransLogMessageStatus.ExportTransferredError
                            }, null),
                    new Restriction(
                        new HashSet<TransLogMessageStatus>
                            {
                                TransLogMessageStatus.ExportSentWaitingForAcknowledgement,
                                TransLogMessageStatus.ExportSentToRemoteSystem
                            },
                        new HashSet<TransLogMessageStatus>
                            {
                                TransLogMessageStatus.ExportSentReceivedOk,
                                TransLogMessageStatus.ExportSentReceivedError
                            }),
                    new Restriction(
                        new HashSet<TransLogMessageStatus>
                            {
                                TransLogMessageStatus.ExportTransferredProcessingStarted
                            },
                        new HashSet<TransLogMessageStatus>
                            {
                                TransLogMessageStatus.ExportTransferredProcessingOk,
                                TransLogMessageStatus.ExportTransferredError,
                                TransLogMessageStatus.ExportSentReceivedOk,
                                TransLogMessageStatus.ExportSentReceivedError
                            })
                };
        }

        public bool TryChangeStatus(StatusChangeEventArgs newStatus)
        {
            var statusChanged = false;

            if (CanChangeStatus(newStatus))
            {
                _dataExchangeMessageLog.SetStatus(newStatus.TransactionId, newStatus.Status);

                statusChanged = true;
            }

            if (NewStatusIsError(newStatus.Status))
            {
                if (MessageExists(newStatus.TransactionId))
                {
                    _serviceEventLogger.LogMessage(30213, GetMessageStatus(newStatus.TransactionId).ToString(),
                        newStatus.Status.ToString(), _dataExchangeMessageLog.GetMessageReceiver(newStatus.TransactionId));
                }
                else
                {
                    _serviceEventLogger.LogMessage(30214, newStatus.Status.ToString(),
                        newStatus.TransactionId.ToString(CultureInfo.InvariantCulture));
                }
            }

            return statusChanged;
        }

        private bool MessageExists(long transactionId)
        {
            var messageId = _dataExchangeMessageLog.GetStatus(transactionId);

            return messageId != default(int);
        }

        private bool NewStatusIsError(TransLogMessageStatus status)
        {
            return status == TransLogMessageStatus.ExportTransferredError
                   || status == TransLogMessageStatus.ExportSentReceivedError;
        }

        private bool CanChangeStatus(StatusChangeEventArgs newStatus)
        {
            var currentStatus = GetMessageStatus(newStatus.TransactionId);

            return _restrictions.All(x => x.IsFulfilled(currentStatus, newStatus.Status));
        }

        private TransLogMessageStatus GetMessageStatus(long transactionId)
        {
            return (TransLogMessageStatus) _dataExchangeMessageLog.GetStatus(transactionId);
        }
    }
}