using System;
using System.Data.Common;

namespace Powel.Icc.Data
{
    /// <summary>
    /// Class to construct SQL query from QueryParameters instance
    /// </summary>
    public class MessageLogQueryBuilder
    {
        int[] ExcludedAckFiles = { 17, 64, 73, 96, 97, 106, 124, 127 };   // See enum TsProtocol.

        public MessageLogQueryParameters Parameters { get; set; }

        public SqlParameterCollection SqlParameters { get; set; }

        public bool FetchTransactions { get; set; }

        public MessageLogQueryBuilder()
        {
            SqlParameters = new SqlParameterCollection();
            FetchTransactions = false;
        }

        /// <summary>
        /// Add full day to DateTime value
        /// </summary>
        /// <param name="time"></param>
        /// <returns></returns>
        DateTime AddFullDay(DateTime time)
        {
            //return time.AddHours(23).AddMinutes(59).AddSeconds(59);
            return time + new TimeSpan(1,0,0,0);
        }

        public string SqlStatement
        {
            get
            {
                if (Parameters == null)
                {
                    return null;
                }

                SqlParameters.Clear();

                string select = "select " +
                                "mh.msg_id, " +
                                "mh.direct, " +
                                "mh.prot_key, " +
                                "mh.date_created, " +
                                "mh.message_reference, " +
                                "mh.start_date, " +
                                "mh.end_date, " +
                                "mh.sender, " +
                                "mh.receiver, " +
                                "mh.transfer_date, " +
                                "mh.message_status, " +
                                "mh.date_ack_received, " +
                                "mh.filesize, " +
                                "mh.routing_address, " +
                                "mh.external_reference, " +
                                "ptp.name as proto_name, " +
                                "ops.name as sender_name, " +
                                "ops.shorname as sender_shorname, " +
                                "ops.country_key as sender_country_key, " +
                                "opr.name as receiver_name, " +
                                "opr.shorname as receiver_shorname, " +
                                "opr.country_key as receiver_country_key, " +
                                "ms.name as message_status_name, " +
                                "(case mh.direct when 0 then mh.transfer_date else mh.date_created end) as sort_date, " +
                                "em.name as exchange_method_name";

                string from = "from " +
                              "message_header mh, " +
                              "oprtunit ops, " +
                              "oprtunit opr, " +
                              "proto_tp ptp, " +
                              "message_status_tp ms, " +
                              "exchmeth em";

				string where = "where " +
								"mh.sender = ops.opun_key (+) " +
								"and mh.receiver = opr.opun_key (+) " +
                                "and mh.exmt_key = em.exmt_key (+) " +
								"and mh.prot_key = ptp.prot_key " +
								"and mh.message_status = ms.message_status";

                if (FetchTransactions)
                {
                    select += ", td.transaction_status, td.transaction_reference";
                    from += ", transaction_details td";
                    where += " and td.msg_id(+) = mh.msg_id";
                }

				bool distinctRequired = false;

				if (Parameters.Import && !Parameters.Export)
                {
                    where += " and mh.direct = 0";
                }
                else if (Parameters.Export && !Parameters.Import)
                {
                    where += " and mh.direct = 1";
                }
                else if (!Parameters.Import && !Parameters.Export)
                {
                    // ok.... this is a "WTF" but stupid is as stupid does 
                    where += " and mh.direct = 0 and mh.direct = 1";
                }

                if (!Parameters.IncludeAck && ExcludedAckFiles.Length > 0)
                {
					where += " and mh.prot_key not in (" + string.Join(",",ExcludedAckFiles) + ")";
                }

                if (!Parameters.IncludeHandled)
                {
                    where += " and mh.handled is null";
                }

                if (Parameters.TransactionStatus.Length > 0 || !string.IsNullOrEmpty(Parameters.TransactionReference))
				{
				    if (!FetchTransactions) // QC9303: The first condition caused an ambiguously defined column
				    {
				        from += ", transaction_details td";
				        where += " and mh.msg_id = td.msg_id";
				    }

				    distinctRequired = true;
				}

				if (Parameters.TransactionStatus.Length > 0)
                {
					where += " and td.transaction_status in (";
                    for (int i = 0; i < Parameters.TransactionStatus.Length; i++)
                    {
						if (i > 0) where += ",";
						where += Parameters.TransactionStatus[i];
                    }
                    where += ")";
                }

                if (Parameters.ProtocolType.Length > 0)
                {
					where += " and mh.prot_key in (";
                    for (int i = 0; i < Parameters.ProtocolType.Length; i++)
                    {
						if (i > 0) where += ",";
                        where += Parameters.ProtocolType[i];
                    }
                    where += ")";
                }

                if (Parameters.MessageStatus.Length > 0)
                {
					where += " and mh.message_status in (";
                    for (int i = 0; i < Parameters.MessageStatus.Length; i++)
                    {
						if (i > 0) where += ",";
                        where += Parameters.MessageStatus[i];
                    }
                    where += ")";
                }

				if (Parameters.StartTime != DateTime.MinValue)
                {
                    DbParameter start_param = SqlParameters.CreateParameter(Parameters.StartTime);
                    DbParameter end_param = SqlParameters.CreateParameter(AddFullDay(Parameters.EndTime));

                    // TODO: Normal/user time, query parameters
                    if (Parameters.CreatedDate)
                    {
                        //DbParameter start_param2 = SqlParameters.CreateParameter(Parameters.StartTime);
                        //DbParameter end_param2 = SqlParameters.CreateParameter(AddFullDay(Parameters.EndTime));
                        // if the row is imported this is a search on transfer_date and if
						// this is an export row this is a search on date_created
						//where += " and ((mh.direct = 1 and mh.date_created >= " + start_param.ParameterName;
						//where += " and mh.date_created <= " + end_param.ParameterName + ")";
						//where += " or ";
						//where += " (mh.direct = 0 and mh.transfer_date >= " + start_param2.ParameterName;
						//where += " and mh.transfer_date <= " + end_param2.ParameterName + "))";
						where += " and mh.date_created >= " + start_param.ParameterName;
						where += " and mh.date_created <= " + end_param.ParameterName;
                    }
					else if (Parameters.TransferDate)
                    {
                        DbParameter start_param2 = SqlParameters.CreateParameter(Parameters.StartTime);
                        DbParameter end_param2 = SqlParameters.CreateParameter(AddFullDay(Parameters.EndTime));
                        // if the row is imported this is a search on date_created and if
                        // this is an export row this is a search on transfer_date
						//where += " and ((mh.direct = 0 and mh.date_created >= " + start_param.ParameterName;
						//where += " and mh.date_created <= " + end_param.ParameterName + ")";
						//where += " or ";
						//where += " (mh.direct = 1 and nvl(mh.transfer_date, mh.date_created) >= " + start_param2.ParameterName;
						//where += " and nvl(mh.transfer_date, mh.date_created) <= " + end_param2.ParameterName + "))";
						where += " and ((mh.direct = 0 and mh.transfer_date >= " + start_param.ParameterName;
						where += " and mh.transfer_date <= " + end_param.ParameterName + ")";
						where += " or ";
						where += " (mh.direct = 1 and nvl(mh.transfer_date, mh.date_created) >= " + start_param2.ParameterName;
						where += " and nvl(mh.transfer_date, mh.date_created) <= " + end_param2.ParameterName + "))";
					}
                    else if (Parameters.TransactionPeriod)
                    {
                        // Include every item which overlaps the selected period, i.e. all transactions which starts or
                        // stops in the given period
                        //where += " and (mh.start_date < " + end_param.ParameterName + " and mh.end_date > " + start_param.ParameterName + ")";
                        where += " and (mh.end_date > " + start_param.ParameterName + " and mh.start_date < " + end_param.ParameterName + ")";
                        //where += " and (mh.start_date < pdmisc.todatefrom17('20120207000000000') and mh.end_date > pdmisc.todatefrom17('20120206000000000'))";
                    }
                }

                if (!string.IsNullOrEmpty(Parameters.MeasurePointName))
                {
                    string paramValue = Parameters.MeasurePointName.EndsWith("%") ? Parameters.MeasurePointName.ToUpper() : Parameters.MeasurePointName.ToUpper() + "%";
					DbParameter mp_param = SqlParameters.CreateParameter(paramValue);
                    DbParameter ep_param = SqlParameters.CreateParameter(paramValue);
                    where += " and mh.msg_id in";
                    where += " (select tobj.msg_id from transaction_objects tobj, meas_pnt mp, exch_pnt ep";
                    where += " where ((tobj.object_key = mp.meas_key and tobj.link_type = 2 and tobj.object_type = 1 and upper(mp.name) like " 
                             + mp_param.ParameterName + ")";
                    where += " or (tobj.object_key = ep.exch_key and tobj.link_type = 3 and tobj.object_type = 4 and upper(ep.name) like "
                             + ep_param.ParameterName + ")))";
				}

                if (!string.IsNullOrEmpty(Parameters.MessageReference))
                {
                    string paramValue = Parameters.MessageReference.EndsWith("%") ? Parameters.MessageReference : Parameters.MessageReference + "%";
                    DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and mh.message_reference like " + param.ParameterName;
                }

                if (!string.IsNullOrEmpty(Parameters.TransactionReference))
                {
					string paramValue = Parameters.TransactionReference.EndsWith("%") ? Parameters.TransactionReference : Parameters.TransactionReference + "%";
					DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and td.transaction_reference like " + param.ParameterName;
                }

                if (!string.IsNullOrEmpty(Parameters.ComponentCode))
                {
					string paramValue = Parameters.ComponentCode.EndsWith("%") ? Parameters.ComponentCode.ToUpper() : Parameters.ComponentCode.ToUpper() + "%";
					DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and mh.msg_id in";
                    where += " (select tobj.msg_id from transaction_objects tobj";
                    where += " where tobj.link_type = 9";
                    where += " and upper(tobj.object_name) like " + param.ParameterName + ")";
				}

                if (!string.IsNullOrEmpty(Parameters.ProductCode))
                {
					string paramValue = Parameters.ProductCode.EndsWith("%") ? Parameters.ProductCode.ToUpper() : Parameters.ProductCode.ToUpper() + "%";
					DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and mh.msg_id in";
                    where += " (select tobj.msg_id from transaction_objects tobj";
                    where += " where tobj.link_type = 10";
                    where += " and upper(tobj.object_name) like " + param.ParameterName + ")";
				}

                if (Parameters.ActorType != -1)
                {
                    DbParameter param1 = SqlParameters.CreateParameter(Parameters.ActorType);
                    DbParameter param2 = SqlParameters.CreateParameter(Parameters.ActorType);
                    where += " and (opr.oprstatu = " + param1.ParameterName + " or ops.oprstatu = " + param2.ParameterName + ")";
                }

                if (!string.IsNullOrEmpty(Parameters.ActorName))
                {
					string paramValue = Parameters.ActorName.EndsWith("%") ? Parameters.ActorName.ToUpper() : Parameters.ActorName.ToUpper() + "%";
					DbParameter param1 = SqlParameters.CreateParameter(paramValue);
					DbParameter param2 = SqlParameters.CreateParameter(paramValue);
                    where += " and (upper(ops.name) like " + param1.ParameterName;
                    where += " or upper(opr.name) like " + param2.ParameterName + ")";
                }

                if (!string.IsNullOrEmpty(Parameters.SenderName))
                {
                    string paramValue = Parameters.SenderName.EndsWith("%") ? Parameters.SenderName.ToUpper() : Parameters.SenderName.ToUpper() + "%";
                    DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and upper(ops.name) like " + param.ParameterName;
                }

                if (!string.IsNullOrEmpty(Parameters.ReceiverName))
                {
					string paramValue = Parameters.ReceiverName.EndsWith("%") ? Parameters.ReceiverName.ToUpper() : Parameters.ReceiverName.ToUpper() + "%";
					DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and upper(opr.name) like " + param.ParameterName;
                }

                if (!string.IsNullOrEmpty(Parameters.ExchangeMethodName))
                {
                    string paramValue = Parameters.ExchangeMethodName.EndsWith("%") ? Parameters.ExchangeMethodName.ToUpper() : Parameters.ExchangeMethodName.ToUpper() + "%";
                    DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and upper(em.name) like " + param.ParameterName;
                }

                if (!string.IsNullOrEmpty(Parameters.RoutingAddress))
                {
                    string paramValue = Parameters.RoutingAddress.EndsWith("%") ? Parameters.RoutingAddress.ToUpper() : Parameters.RoutingAddress.ToUpper() + "%";
                    DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and upper(mh.routing_address) like " + param.ParameterName;
                }

                if (!string.IsNullOrEmpty(Parameters.ExternalReference))
                {
                    string paramValue = Parameters.ExternalReference.EndsWith("%") ? Parameters.ExternalReference : Parameters.ExternalReference + "%";
                    DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and mh.external_reference like " + param.ParameterName;
                }

                if (!string.IsNullOrEmpty(Parameters.FileName))
                {
                    string paramValue = Parameters.FileName.EndsWith("%") ? Parameters.FileName.ToUpper() : Parameters.FileName.ToUpper() + "%";
                    DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and upper(mh.filename) like " + param.ParameterName;
                }

                if (!string.IsNullOrEmpty(Parameters.GlobalReference))
                {
                    string paramValue = Parameters.GlobalReference.EndsWith("%") ? Parameters.GlobalReference.ToUpper() : Parameters.GlobalReference.ToUpper() + "%";
                    DbParameter param = SqlParameters.CreateParameter(paramValue);
                    where += " and mh.msg_id in";
                    where += " (select tobj.msg_id from transaction_objects tobj";
                    where += " where tobj.link_type = 11";
                    where += " and upper(tobj.object_name) like " + param.ParameterName + ")";
                }

                if (distinctRequired)
				{
					select = select.Insert(7, "distinct ");
				}
                // TODO: Actor (ID) - cf sender/receiver
                // TODO: Actor name - cf sender/receiver
				return select + " " + from + " " + where + " order by sort_date desc";
            }
        }
    }
}

