using System;
using System.Collections.Generic;
using Powel.Icc.Data;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.MessageLog
{
    public class DataExchangeMessageLog : IDataExchangeMessageLog
    {
        public void SetExternalReference(long messageLogId, string externalReference)
        {
            MessageLogData.SetExternalReference(messageLogId, externalReference);
        }

        public void SetStatusToExportEnqueued(long messageLogId)
        {
            MessageLogData.SetMessageHeaderStatus(messageLogId, (int)TransLogMessageStatus.ExportEnqueued,
                (int)TransLogMessageStatus.ExportCreated);
        }

        public void SetStatusToExportTransferredToHub(long messageLogId, string externalReference, string routingAddress)
        {
            MessageLogData.SetMessageHeaderStatusAndUpdateTransferDate(messageLogId,
                (int)
                    TransLogMessageStatus.ExportTransferredProcessingStarted,
                (int)TransLogMessageStatus.ExportEnqueued,
                externalReference, routingAddress);
        }

        public void SetStatusToExportTransferredToHub(long messageLogId, string externalReference)
        {
            IList<long> ids = new List<long> { messageLogId };
            var statuses = MessageLogData.GetMessageHeaderStatuses(ids);

            if (statuses.ContainsKey(messageLogId))
            {
                MessageLogData.SetMessageHeaderStatusAndUpdateTransferDate(messageLogId,
                    (int)TransLogMessageStatus.ExportTransferredProcessingStarted,
                    (int)TransLogMessageStatus.ExportEnqueued,
                    externalReference);
            }
        }

        /// <param name="statusText">nulls are NOT allowed</param>
        /// <exception cref="ArgumentNullException">statusText must NOT be null</exception>
        public void SetStatusAndStatusText(string externalReference, TransLogMessageStatus status,
            string statusText)
        {
            if (statusText == null)
            {
                throw new ArgumentNullException("statusText");
            }

            using (var connection = Util.OpenConnection())
            using (var transaction = Util.OpenTransaction(connection))
            {
                MessageLogData.SetMessageHeaderStatus(externalReference, (int)status, transaction);

                MessageLogData.SetMessageHeaderStatusText(externalReference, statusText, transaction);

                transaction.Commit();
            }
        }

        public void SetStatusAndStatusText(long messageLogId, TransLogMessageStatus status, string statusText)
        {
            using (var connection = Util.OpenConnection())
            using (var transaction = Util.OpenTransaction(connection))
            {
                MessageLogData.SetMessageHeaderStatus(messageLogId, (int)status, transaction);

                if (statusText != null)
                {
                    MessageLogData.SetMessageHeaderStatusText(messageLogId, statusText, transaction);
                }

                transaction.Commit();
            }
        }

        public bool SetStatus(string externalReference, TransLogMessageStatus status)
        {
            using (var connection = Util.OpenConnection())
            using (var transaction = Util.OpenTransaction(connection))
            {
                var ret = MessageLogData.SetMessageHeaderStatus(externalReference, (int)status, transaction);

                transaction.Commit();
                return ret;
            }
        }

        public void SetStatus(long messageLogId, TransLogMessageStatus status)
        {
            using (var connection = Util.OpenConnection())
            using (var transaction = Util.OpenTransaction(connection))
            {
                MessageLogData.SetMessageHeaderStatus(messageLogId, (int)status, transaction);

                transaction.Commit();
            }
        }

        public int GetStatus(long messageLogId)
        {
            using (Util.OpenConnection())
            {
                return MessageLogData.GetMessageHeaderStatus(messageLogId);
            }
        }

        public string GetMessageReceiver(long messageLogId)
        {
            using (Util.OpenConnection())
            {
                return MessageLogData.GetMessageReceiver(messageLogId);
            }
        }

        public long GetNextMessageId()
        {
            using (Util.OpenConnection())
            {
                return MessageLogData.GetNextMessageId();
            }
        }

        public void CreateNewMessageHeader(long msgId, string routingAddress, int protKey, int receiver, string messageReference, int messageStatus, int sender, string externalReference, int osuserKey, int userKey)
        {
            using (Util.OpenConnection())
            {
                MessageLogData.CreateNewMessageHeader(msgId, protKey, messageReference, messageStatus, sender,
                    receiver, userKey, osuserKey, externalReference, routingAddress);
            }
        }

        public void DeleteMessage(long messageId)
        {
            using (Util.OpenConnection())
            {
                MessageLogData.DeleteMessage(messageId);
            }
        }
    }
}