using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using System.Text;
using Oracle.ManagedDataAccess.Client;
using Powel.Icc.Common;
using Powel.Icc.Data.Entities.MessageLog;
using Powel.Icc.Data.Interfaces;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data
{
    public class MessageLogData : IMessageLogData
    {
        public static long GetNextMessageId()
        {
            var cmd = new OracleCommand("select message_header_seq.nextval from dual");

            return Convert.ToInt64(Util.CommandToScalar(cmd));
        }

        public static int GetOpunKeyByBankAcc(string bankAcc)
        {
            var cmd = new OracleCommand(
                "select " +
                "opun_key from " +
                "oprtunit " +
                "where " +
                "bankacc = :bankAcc");

            cmd.Parameters.Add(":bankAcc", bankAcc);

            return Convert.ToInt32(Util.CommandToScalar(cmd));
        }

        public static void CreateNewMessageHeader(long msgId, int protKey, string messageReference, int messageStatus,
            int sender, int receiver, int user_key, int osuser_key, string externalReference, string routingAddress)
        {
            var cmd = new OracleCommand(
                "insert into message_header(" +
                "msg_id, " +
                "prot_key, " +
                "direct, " +
                "date_created, " +
                "message_reference, " +
                "message_status, " +
                "sender, " +
                "receiver, " +
                "user_key, " +
                "osuser_key, " +
                "external_reference, " +
                "routing_address)" +
                "values (:msgId, :protKey, 1, :dateCreated, :messageReference, :messageStatus, :sender, :receiver, :userKey, :osUserKey, :externalReference, :routingAddress)");

            var currentTime = IccConfiguration.Time.DatabaseCalendar.ToDateTime(UtcTime.Now);

            cmd.Parameters.Add(":msgId", msgId);
            cmd.Parameters.Add(":protKey", protKey);
            cmd.Parameters.Add(":dateCreated", OracleDbType.Date, currentTime, ParameterDirection.Input);
            cmd.Parameters.Add(":messageReference", messageReference);
            cmd.Parameters.Add(":messageStatus", messageStatus);
            cmd.Parameters.Add(":sender", sender);
            cmd.Parameters.Add(":receiver", receiver);
            cmd.Parameters.Add(":userKey", user_key);
            cmd.Parameters.Add(":osUserKey", osuser_key);
            cmd.Parameters.Add(":externalReference", externalReference);
            cmd.Parameters.Add(":routingAddress", routingAddress);

            Util.ExecuteCommand(cmd);
        }

        public List<MessageHeaderStatus> GetMessageHeaderStatuses(int roleKey, List<string> messageReferences)
        {
            var messageReferencesSql = string.Empty;

            for (var i = 0; i < messageReferences.Count; i++)
            {
                if (messageReferencesSql.Length > 0)
                {
                    messageReferencesSql += ",";
                }

                messageReferencesSql += ":" + i;
            }

            const string sql = @"
                select
                    mh.msg_id,
                    mh.message_reference,
                    mh.message_status,
                    ms.name as message_status_name,
                    mh.date_created,
                    mh.transfer_date,
                    mh.date_ack_received,
                    td.transaction_reference,
                    td.transaction_status,
                    td.error_text,
                    mh.direct,
                    ops.country_key as sender_country_key,
                    opr.country_key as receiver_country_key
                from
                    message_header mh,
                    message_status_tp ms,
                    transaction_details td,
                    oprtunit ops,
                    oprtunit opr
                where
                    mh.message_reference in ({0}) and
                    mh.message_status = ms.message_status and
                    mh.msg_id(+) = td.msg_id and
                    ops.opun_key(+) = mh.sender and
                    opr.opun_key(+) = mh.receiver";

            var cmd = new OracleCommand(string.Format(sql, messageReferencesSql));

            foreach (string messageReference in messageReferences)
            {
                cmd.Parameters.Add(new OracleParameter(null, messageReference));
            }

            var cal = IccConfiguration.Time.DatabaseCalendar;

            var result = new List<MessageHeaderStatus>();
            using (var connection = Util.OpenConnectionByRole(roleKey))
            using (var reader = Util.CommandToDataReader(cmd, connection))
            {
                while (reader.Read())
                {
                    var messageId = reader.GetInt64(reader.GetOrdinal("msg_id"));
                    var messageHeaderStatus = result.Find(x => x.MessageId == messageId);

                    if (messageHeaderStatus == null)
                    {
                        messageHeaderStatus = new MessageHeaderStatus();
                        messageHeaderStatus.MessageId = messageId;
                        messageHeaderStatus.MessageReference = reader["message_reference"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("message_reference"))
                            : string.Empty;
                        messageHeaderStatus.Status = reader["message_status"] != DBNull.Value
                            ? reader.GetInt16(reader.GetOrdinal("message_status"))
                            : 0;
                        messageHeaderStatus.StatusName = reader["message_status_name"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("message_status_name"))
                            : string.Empty;
                        messageHeaderStatus.TransferDate = reader["transfer_date"] == DBNull.Value
                            ? UtcTime.MinValue
                            : cal.ToUtcTime(reader.GetDateTime(reader.GetOrdinal("transfer_date")));
                        messageHeaderStatus.DateAckReceived = reader["date_ack_received"] == DBNull.Value
                            ? UtcTime.MinValue
                            : cal.ToUtcTime(reader.GetDateTime(reader.GetOrdinal("date_ack_received")));
                        messageHeaderStatus.DateCreated = reader["date_created"] == DBNull.Value
                            ? UtcTime.MinValue
                            : cal.ToUtcTime(reader.GetDateTime(reader.GetOrdinal("date_created")));
                        messageHeaderStatus.Direction = reader["direct"] != DBNull.Value
                            ? reader.GetInt32(reader.GetOrdinal("direct"))
                            : 0;
                        messageHeaderStatus.SenderCountryKey = reader["sender_country_key"] == DBNull.Value
                            ? -1
                            : reader.GetInt32(reader.GetOrdinal("sender_country_key"));
                        messageHeaderStatus.ReceiverCountryKey = reader["receiver_country_key"] == DBNull.Value
                            ? -1
                            : reader.GetInt32(reader.GetOrdinal("receiver_country_key"));
                        result.Add(messageHeaderStatus);
                    }

                    if (reader["transaction_reference"] != DBNull.Value && reader["transaction_status"] != DBNull.Value)
                    {
                        var messageHeaderTransaction = new MessageHeaderTransaction();
                        messageHeaderTransaction.TransactionReference =
                            reader.GetString(reader.GetOrdinal("transaction_reference"));
                        messageHeaderTransaction.TransactionStatus =
                            reader.GetInt16(reader.GetOrdinal("transaction_status"));
                        messageHeaderTransaction.ErrorText = reader["error_text"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("error_text"))
                            : string.Empty;
                        messageHeaderStatus.Transactions.Add(messageHeaderTransaction);
                    }
                }
            }
            return result;
        }

        public List<MessageHeader> GetMessageHeaders(int roleKey, MessageLogQueryParameters parameters)
        {
            var query = new MessageLogQueryBuilder();
            query.FetchTransactions = true;
            query.Parameters = parameters;

            var cmd = new OracleCommand(query.SqlStatement);

            foreach (DbParameter parameter in query.SqlParameters.DbParameters)
            {
                var oracleParameter = new OracleParameter(parameter.ParameterName, parameter.Value);

                cmd.Parameters.Add(oracleParameter);
            }

            var result = new List<MessageHeader>();

            var cal = IccConfiguration.Time.DatabaseCalendar;

            using (var connection = Util.OpenConnectionByRole(roleKey))
            using (var reader = Util.CommandToDataReader(cmd, connection))
            {
                while (reader.Read())
                {
                    var messageId = reader.GetInt64(reader.GetOrdinal("msg_id"));
                    var e = result.Find(x => x.MessageId == messageId);

                    if (e == null)
                    {
                        e = new MessageHeader();
                        e.MessageId = messageId;
                        e.Direction = reader["direct"] != DBNull.Value
                            ? reader.GetInt32(reader.GetOrdinal("direct"))
                            : 0;
                        e.ProtocolId = reader["prot_key"] != DBNull.Value
                            ? reader.GetInt32(reader.GetOrdinal("prot_key"))
                            : 0;
                        e.DateCreated = ReadNullableUtcTime(reader, "date_created", cal);
                        e.MessageReference = reader["message_reference"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("message_reference"))
                            : string.Empty;
                        e.StartDate = ReadNullableUtcTime(reader, "start_date", cal);
                        e.EndDate = ReadNullableUtcTime(reader, "end_date", cal);
                        e.SenderId = reader["sender"] != DBNull.Value ? reader.GetInt32(reader.GetOrdinal("sender")) : 0;
                        e.ReceiverId = reader["receiver"] != DBNull.Value
                            ? reader.GetInt32(reader.GetOrdinal("receiver"))
                            : 0;
                        e.TransferDate = ReadNullableUtcTime(reader, "transfer_date", cal);
                        e.Status = reader["message_status"] != DBNull.Value
                            ? reader.GetInt32(reader.GetOrdinal("message_status"))
                            : 0;
                        e.DateAckReceived = ReadNullableUtcTime(reader, "date_ack_received", cal);
                        e.Filesize = ReadNullableInt32(reader, "filesize");
                        e.ProtocolName = reader["proto_name"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("proto_name"))
                            : string.Empty;
                        e.SenderName = reader["sender_name"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("sender_name"))
                            : string.Empty;
                        e.SenderShortName = reader["sender_shorname"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("sender_shorname"))
                            : string.Empty;
                        e.SenderCountryKey = reader["sender_country_key"] != DBNull.Value
                            ? reader.GetInt32(reader.GetOrdinal("sender_country_key"))
                            : -1;
                        e.ReceiverName = reader["receiver_name"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("receiver_name"))
                            : string.Empty;
                        e.ReceiverShortName = reader["receiver_shorname"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("receiver_shorname"))
                            : string.Empty;
                        e.ReceiverCountryKey = reader["receiver_country_key"] != DBNull.Value
                            ? reader.GetInt32(reader.GetOrdinal("receiver_country_key"))
                            : -1;
                        e.StatusName = reader["message_status_name"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("message_status_name"))
                            : string.Empty;
                        e.SortDate = ReadNullableUtcTime(reader, "sort_date", cal);
                        e.ExchangeMethodName = reader["exchange_method_name"] == DBNull.Value
                            ? ""
                            : reader.GetString(reader.GetOrdinal("exchange_method_name"));
                        result.Add(e);
                    }

                    if (reader["transaction_reference"] != DBNull.Value && reader["transaction_status"] != DBNull.Value)
                    {
                        var transaction = new MessageHeaderTransaction();
                        transaction.TransactionReference = reader.GetString(reader.GetOrdinal("transaction_reference"));
                        transaction.TransactionStatus = reader.GetInt16(reader.GetOrdinal("transaction_status"));
                        e.Transactions.Add(transaction);
                    }
                }
            }
            return result;
        }

        public static string GetFilename(int AppKey)
        {
            var cmd = new OracleCommand("select direct,transfer_date,date_created,filename from message_header where app_key = :appKey");
            cmd.Parameters.Add(":appKey", OracleDbType.Int32).Value = AppKey;
            var dt = Util.CommandToDataTable(cmd);
            if (dt.Rows.Count == 0)
                return "";
            var row = dt.Rows[0];
            string column = "date_created";
            if (Convert.ToInt32(row["direct"]) == 0)
                column = "transfer_date";
            var date = Convert.ToDateTime(row[column]).ToString("yyyyMMdd");
            return System.IO.Path.Combine(date, row["filename"].ToString());
        }

        public static List<string> GetMessageReferences(int AppKey)
        {
            var ret = new List<string>();
            var cmd = new OracleCommand("select message_reference from message_header where app_key = :appKey");
            cmd.Parameters.Add(":appKey", OracleDbType.Int32).Value = AppKey;
            var dt = Util.CommandToDataTable(cmd);
            foreach (DataRow row in dt.Rows)
            {
                ret.Add(row[0].ToString());
            }
            return ret;
        }

        public static bool SetMessageHeaderStatus(string externalReference, int status, IDbTransaction transaction)
        {
            var cmd = new OracleCommand("update message_header set message_status = :1 where external_reference = :2");
            cmd.Parameters.Add(null, status);
            cmd.Parameters.Add(null, externalReference);
            return Util.ExecuteCommand(cmd, transaction) > 0;
        }

        public static void SetMessageHeaderStatus(long messageId, int newStatus, int currentStatus)
        {
            var cmd =
                new OracleCommand(
                    "update message_header set message_status = :1 where msg_id = :2 and message_status = :3");
            cmd.Parameters.Add(null, newStatus);
            cmd.Parameters.Add(null, messageId);
            cmd.Parameters.Add(null, currentStatus);
            Util.ExecuteCommand(cmd);
        }

        public static void SetExternalReference(long messageId, string externalReference)
        {
            var cmd = new OracleCommand("update message_header set external_reference = :1 where msg_id = :2");
            cmd.Parameters.Add(null, externalReference);
            cmd.Parameters.Add(null, messageId);
            Util.ExecuteCommand(cmd);
        }

        public static void SetMessageHeaderStatus(long messageId, int status, IDbTransaction transaction)
        {
            var cmd = new OracleCommand("update message_header set message_status = :1 where msg_id = :2");
            cmd.Parameters.Add(null, status);
            cmd.Parameters.Add(null, messageId);
            Util.ExecuteCommand(cmd, transaction);
        }

        public static void SetMessageHeaderStatusText(string externalReference, string statusText,
            IDbTransaction transaction)
        {
            var cmd =
                new OracleCommand(
                    "update transaction_details set error_text = :1 where msg_id in (select msg_id from message_header where external_reference = :2)");
            cmd.Parameters.Add(null, statusText);
            cmd.Parameters.Add(null, externalReference);
            Util.ExecuteCommand(cmd, transaction);
        }

        public static void SetMessageHeaderStatusText(long externalReference, string statusText,
            IDbTransaction transaction)
        {
            var cmd = new OracleCommand("update transaction_details set error_text = :1 where msg_id = :2");
            cmd.Parameters.Add(null, statusText);
            cmd.Parameters.Add(null, externalReference);
            Util.ExecuteCommand(cmd, transaction);
        }

        public static void SetMessageHeaderStatusAndUpdateTransferDate(long messageId, int newStatus, int currentStatus,
            string externalReference, string routingAddress)
        {
            var now = IccConfiguration.Time.DatabaseCalendar.ToDateTime(UtcTime.Now).ToString("yyyy-MM-dd HH:mm:ss");
            var cmd =
                new OracleCommand(
                    "update message_header set message_status = :1, external_reference = :2, routing_address = :3, transfer_date = to_date(:4, 'YYYY-MM-DD HH24:MI:SS') where msg_id = :5 and message_status = :6");
            cmd.Parameters.Add(null, newStatus);
            cmd.Parameters.Add(null, externalReference);
            cmd.Parameters.Add(null, routingAddress);
            cmd.Parameters.Add(null, now);
            cmd.Parameters.Add(null, messageId);
            cmd.Parameters.Add(null, currentStatus);
            //if no rows where affected, we still need to update external_reference, routing_address and transfer_date
            if (Util.ExecuteCommand(cmd) == 0)
            {
                cmd =
                    new OracleCommand(
                        "update message_header set external_reference = :1, routing_address = :2, transfer_date = to_date(:3, 'YYYY-MM-DD HH24:MI:SS') where msg_id = :4");
                cmd.Parameters.Add(null, externalReference);
                cmd.Parameters.Add(null, routingAddress);
                cmd.Parameters.Add(null, now);
                cmd.Parameters.Add(null, messageId);
                Util.ExecuteCommand(cmd);
            }
        }

        public static void SetMessageHeaderStatusAndUpdateTransferDate(long messageId, int newStatus, int currentStatus,
            string externalReference)
        {
            var now = IccConfiguration.Time.DatabaseCalendar.ToDateTime(UtcTime.Now).ToString("yyyy-MM-dd HH:mm:ss");
            var cmd =
                new OracleCommand(
                    "update message_header set message_status = :1, external_reference = :2, transfer_date = to_date(:3, 'YYYY-MM-DD HH24:MI:SS') where msg_id = :4 and message_status = :5");
            cmd.Parameters.Add(null, newStatus);
            cmd.Parameters.Add(null, externalReference);
            cmd.Parameters.Add(null, now);
            cmd.Parameters.Add(null, messageId);
            cmd.Parameters.Add(null, currentStatus);
            //if no rows where affected, we still need to update external_reference and transfer_date
            if (Util.ExecuteCommand(cmd) == 0)
            {
                cmd =
                    new OracleCommand(
                        "update message_header set external_reference = :1, transfer_date = to_date(:2, 'YYYY-MM-DD HH24:MI:SS') where msg_id = :3");
                cmd.Parameters.Add(null, externalReference);
                cmd.Parameters.Add(null, now);
                cmd.Parameters.Add(null, messageId);
                Util.ExecuteCommand(cmd);
            }
        }

        public static int GetMessageHeaderStatus(long messageId)
        {
            var cmd = new OracleCommand("select message_status from message_header where msg_id = :1");
            cmd.Parameters.Add(null, messageId);
            return Convert.ToInt32(Util.CommandToScalar(cmd));
        }

        public static Dictionary<long, int> GetMessageHeaderStatuses(IList<long> messageIds)
        {
            var sql = string.Format("select msg_id, message_status from message_header where msg_id in ({0})",
                string.Join(",", messageIds));
            var cmd = new OracleCommand(sql);

            var result = new Dictionary<long, int>();

            using (IDbConnection connection = Util.OpenConnection())
            using (var reader = Util.CommandToDataReader(cmd, connection))
            {
                while (reader.Read())
                {
                    var messageId = reader.GetInt64(reader.GetOrdinal("msg_id"));
                    var messageStatus = reader.GetInt32(reader.GetOrdinal("message_status"));
                    result.Add(messageId, messageStatus);
                }
            }

            return result;
        }

        public static string GetMessageReceiver(long messageId)
        {
            var cmd = new OracleCommand(@"select o.name from message_header mh
                                            left join oprtunit o on mh.receiver = o.opun_key
                                          where mh.msg_id = :1");
            cmd.Parameters.Add(null, messageId);
            return Util.CommandToScalar(cmd).ToString();
        }

        public static void DeleteMessage(long messageId)
        {
            var cmd = new OracleCommand(@"DELETE FROM message_header WHERE msg_id = :1");
                cmd.Parameters.Add(null, messageId);
                Util.ExecuteCommand(cmd);
        }

        private static UtcTime ReadNullableUtcTime(OracleDataReader reader, string columnName, RegionalCalendar cal)
        {
            return reader[columnName] == DBNull.Value
                ? UtcTime.MinValue
                : cal.ToUtcTime(reader.GetDateTime(reader.GetOrdinal(columnName)));
        }

        private static int ReadNullableInt32(OracleDataReader reader, string columnName)
        {
            return reader[columnName] == DBNull.Value ? 0 : reader.GetInt32(reader.GetOrdinal(columnName));
        }

        public List<MessageLogExportStatus> GetExportStatuses(int roleKey, List<string> exchangeMethodNames,
            TimePeriod timePeriod, int maximumNumberOfMessageLogEntries)
        {
            var periodStart = RegionalCalendar.CurrentNT.ToDateTime(timePeriod.From);
            var periodEnd = RegionalCalendar.CurrentNT.ToDateTime(timePeriod.Until);

            var cmd = new OracleCommand();

            const string sql = @"
            select * from (    
                select 
                    mh.msg_id,
                    mh.message_reference,
                    mh.message_status,
                    ms.name as message_status_name,
                    mh.start_date,
                    mh.end_date,
                    mh.transfer_date,
                    mh.date_ack_received,
                    mh.prot_key,
                    mh.sender,
                    mh.receiver,
                    mh.date_created,
                    em.name as exchange_method_name,
                    ops.name as sender_name,
                    opr.name as receiver_name,
                    td.transaction_reference,
                    td.transaction_status,
                    td.error_text,
                    ops.country_key as sender_country_key,
                    opr.country_key as receiver_country_key
                from
                    message_header mh,
                    message_status_tp ms,
                    transaction_details td,
                    exchmeth em,
                    oprtunit ops,
                    oprtunit opr
                where 
                    mh.message_status = ms.message_status
                and 
                    mh.msg_id(+) = td.msg_id
                and
                    mh.exmt_key = em.exmt_key
                and 
                    mh.sender = ops.opun_key
                and 
                    mh.receiver = opr.opun_key
                and
                    mh.start_date < :end_date
                and
                    mh.end_date > :start_date
                and
                    mh.exmt_key in (select exmt_key from exchmeth where name in {0})
                order by mh.date_created desc
            ) where rownum < :max";

            cmd.Parameters.Add(new OracleParameter(":end_date", OracleDbType.TimeStamp, ParameterDirection.Input)
                {
                    Value = periodEnd
                });

            cmd.Parameters.Add(new OracleParameter(":start_date", OracleDbType.TimeStamp, ParameterDirection.Input)
            {
                Value = periodStart
            });
            cmd.CommandText = string.Format(sql, GetOracleCommandByVarChar2Parameters(cmd, exchangeMethodNames));

            cmd.Parameters.Add(new OracleParameter(":max", OracleDbType.Int32, ParameterDirection.Input)
                {
                    Value = maximumNumberOfMessageLogEntries
                });

            var result = new List<MessageLogExportStatus>();

            var cal = IccConfiguration.Time.DatabaseCalendar;

            using (var connection = Util.OpenConnectionByRole(roleKey))
            using (var reader = Util.CommandToDataReader(cmd, connection))
            {
                while (reader.Read())
                {
                    var messageId = reader.GetInt64(reader.GetOrdinal("msg_id"));
                    var e = result.Find(x => x.MessageId == messageId);

                    if (e == null)
                    {
                        e = new MessageLogExportStatus();
                        e.MessageId = messageId;
                        e.MessageReference = reader.GetString(reader.GetOrdinal("message_reference"));
                        e.Status = reader.GetInt32(reader.GetOrdinal("message_status"));
                        e.StatusName = reader.GetString(reader.GetOrdinal("message_status_name"));
                        e.StartDate = reader["start_date"] == DBNull.Value
                            ? UtcTime.MinValue
                            : cal.ToUtcTime(reader.GetDateTime(reader.GetOrdinal("start_date")));
                        e.EndDate = reader["end_date"] == DBNull.Value
                            ? UtcTime.MinValue
                            : cal.ToUtcTime(reader.GetDateTime(reader.GetOrdinal("end_date")));
                        e.TransferDate = reader["transfer_date"] == DBNull.Value
                            ? UtcTime.MinValue
                            : cal.ToUtcTime(reader.GetDateTime(reader.GetOrdinal("transfer_date")));
                        e.DateAckReceived = reader["date_ack_received"] == DBNull.Value
                            ? UtcTime.MinValue
                            : cal.ToUtcTime(reader.GetDateTime(reader.GetOrdinal("date_ack_received")));
                        e.ProtocolId = reader.GetInt32(reader.GetOrdinal("prot_key"));
                        e.SenderId = reader.GetInt32(reader.GetOrdinal("sender"));
                        e.ReceiverId = reader.GetInt32(reader.GetOrdinal("receiver"));
                        e.DateCreated = reader["date_created"] == DBNull.Value
                            ? UtcTime.MinValue
                            : cal.ToUtcTime(reader.GetDateTime(reader.GetOrdinal("date_created")));
                        e.ExchangeMethodName = reader.GetString(reader.GetOrdinal("exchange_method_name"));
                        e.SenderName = reader.GetString(reader.GetOrdinal("sender_name"));
                        e.ReceiverName = reader.GetString(reader.GetOrdinal("receiver_name"));
                        result.Add(e);
                    }

                    if (reader["transaction_reference"] != DBNull.Value && reader["transaction_status"] != DBNull.Value)
                    {
                        var transaction = new MessageHeaderTransaction();
                        transaction.TransactionReference = reader.GetString(reader.GetOrdinal("transaction_reference"));
                        transaction.TransactionStatus = reader.GetInt16(reader.GetOrdinal("transaction_status"));
                        transaction.ErrorText = reader["error_text"] != DBNull.Value
                            ? reader.GetString(reader.GetOrdinal("error_text"))
                            : string.Empty;

                        e.Transactions.Add(transaction);
                    }
                }
            }
            return result;
        }

        private static string GetOracleCommandByVarChar2Parameters(OracleCommand cmd, List<string> parameters)
        {
            var stringBuilder = new StringBuilder();
            stringBuilder.Append("(");
            for (var i = 0; i < parameters.Count; i++)
            {
                if (i > 0)
                {
                    stringBuilder.Append(",");
                }
                var paramName = ":param" + i;
                stringBuilder.Append(paramName);
                cmd.Parameters.Add(new OracleParameter(paramName, OracleDbType.Varchar2, ParameterDirection.Input)
                    {
                        Value = parameters[i]
                    });
            }
            stringBuilder.Append(")");
            return stringBuilder.ToString();
        }
    }
}