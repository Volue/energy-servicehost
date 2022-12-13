using System;
using System.Data;
using System.Globalization;
using Oracle.ManagedDataAccess.Client;
using System.Collections;
using System.Text;
using System.Text.RegularExpressions;
using System.Collections.Generic;

namespace Powel.Icc.Data
{
	#region Structs
	public struct Message
	{
		public string id,measpnt,instime,updtime,group_id,status,construction,net_area,time,old_meter,new_meter,row_id,time_raw,valftime_raw,valttime_raw, message, version, msgtype,bytesDokNr,beteckningBytesTyp,kund,systemID;
		public int count;
	}

	public struct EventLogEntry
	{
		public string rcount,datetim,descript;
	}

	public struct MessageIdAndStatus
	{
		public Int64 Id;
		public string Status;
		public string Type;
	}

	public struct MessageIdGridAreaAndInstallations
	{
		public string GroupId;
		public string InstallationId;
		public string GridId;
		
	}

	

	#endregion

	/// <summary>
	/// Summary description for MessageData.
	/// </summary>
	public class MessageData
	{
#region Enums
		public enum MessageType
		{
			UNDEF,
			READING,
			READBTCH,
			METERCNG,
			METERCN2,
			MPOINT,
			CISUPDAT,
			POSITION,
			WRKORDER,
			SUPPLCNG,
			CISEXPDF
		}
#endregion

		public MessageData()
		{
			MessageStatus = "";
			MsgType = "";
			MessageTypes = new ArrayList();
			StartDate = new DateTime(0);
			EndDate = new DateTime(0);
			Debug = false;
		}
		
#region Properties

		public ArrayList MessageTypes{ get; set; }
		public string GridArea { get; set; }
		public string Installation { get; set; }
		public string MsgType { get; set; }
		public string MessageStatus{ get; set; }
		public DateTime StartDate{ get; set; }
		public DateTime EndDate{ get; set; }
		public bool Debug{ get; set; }
#endregion

		public MessageType GetType(string type)
		{
			string tp = type.ToUpper();
			switch (tp)
				{
				case "CISUPDAT": return MessageType.CISUPDAT;
				case "CISEXPDF": return MessageType.CISEXPDF;
				case "METERCN2": return MessageType.METERCN2;
				case "METERCNG": return MessageType.METERCNG;
				case "MPOINT":   return MessageType.MPOINT;
				case "POSITION": return MessageType.POSITION;
				case "READBTCH": return MessageType.READBTCH;
				case "READING":  return MessageType.READING;
				case "SUPPLCNG": return MessageType.SUPPLCNG;
				case "WRKORDER": return MessageType.WRKORDER;
				default: return MessageType.UNDEF;
				}
		}

		public DataTable FetchMessages()
		{
			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = BuildQuery();
			try
				{
					return Util.CommandToDataTable(oCmd);
				}
			catch (Exception)
				{

					return new DataTable();
				}
			
		}

		//public DataTable FetchMessages(ArrayList ids)
		//{
		//    OracleCommand oCmd = new OracleCommand();
		//    oCmd.CommandText = BuildQuery(ids);
		//    return Util.CommandToDataTable(oCmd);
		//}

		public ArrayList GetMessages(DataTable dt)
		{
			string group_id = "";
			Message msg = new Message();
			ArrayList msgs = new ArrayList();
			foreach (DataRow dr in dt.Rows)
			{
				// is it possible for two rows with different ids (ie types) have the same group_id?
				// if so, below has to be changed
				if (dr["group_id"].ToString() != group_id)
				{
					group_id = dr["group_id"].ToString();
					msg.id = dr["id"].ToString();
					msg.group_id = group_id;
					msg.instime = dr["instime"].ToString();
					msg.updtime = dr["updtime"].ToString();
					msg.status = dr["status"].ToString();
					msg.new_meter = ""; //clear new_meter
					msg.old_meter = ""; //clear old_meter
					msg.bytesDokNr = string.Empty;
					msg.kund = string.Empty;
					msg.beteckningBytesTyp = string.Empty;
					msg.message = dr["message"].ToString();
					if (msg.id.ToUpper() == "METERCN2")
					{
						msg.construction = dr["c001"].ToString();
						msg.measpnt = dr["c023"].ToString();
						msg.net_area = dr["c002"].ToString();
						msg.time = this.From17ToDate(dr["c008"].ToString());
						msg.time_raw = dr["c008"].ToString();
						if (dr["c004"].ToString().ToUpper() == "NEDTAGEN")
							msg.old_meter = dr["c003"].ToString();
						else if (dr["c004"].ToString().ToUpper() == "UPPSATT")
							msg.new_meter = dr["c003"].ToString();
					}
					else if (msg.id.ToUpper() == "SUPPLCNG")
					{

						msg.construction = dr["c002"].ToString();
						msg.measpnt = "";
						msg.net_area = dr["c001"].ToString();
						msg.time = this.From17ToDate(dr["c003"].ToString());
						msg.time_raw = dr["c003"].ToString();

						msg.bytesDokNr = dr["c017"].ToString() +" "+ dr["c019"].ToString();
						msg.beteckningBytesTyp = dr["c016"].ToString();
						string sign = string.IsNullOrEmpty(dr["c005"].ToString()) || string.IsNullOrEmpty(dr["c006"].ToString())
							? ""
							: " - ";

						msg.old_meter = dr["c005"].ToString() + sign + dr["c006"].ToString();

						sign = string.IsNullOrEmpty(dr["c011"].ToString()) || string.IsNullOrEmpty(dr["c012"].ToString())
							? ""
							: " - ";
						msg.new_meter = dr["c011"].ToString() + sign + dr["c012"].ToString();
						msg.time = this.From8ToDate(dr["c003"].ToString());
						msg.time_raw = dr["c003"].ToString();

						switch (dr["c016"].ToString())
						{
							case "1 - Leverantörsbyte":
								msg.kund = dr["c010"].ToString();
								break;
							case "3 - Leveransstart":
								msg.kund = dr["c010"].ToString();
								break;
							case "4 - Leveransslut":
								msg.kund = dr["c004"].ToString();
								break;
							case "1 - Hävning av leverans":
								msg.kund = dr["c004"].ToString();
								break;
							case "2 - Balansansvarsbyte":
								msg.kund = dr["c010"].ToString();
								break;
							default:
								msg.kund = "";
								break;
						}
					}
					else if (msg.id.ToUpper() == "CISUPDAT")
					{
						msg.construction = dr["c001"].ToString();
						msg.measpnt = "";
						msg.net_area = dr["c002"].ToString();
						msg.time = this.From8ToDate(dr["c026"].ToString()) + " - " + this.From8ToDate(AddDays(dr["c027"].ToString(),1));
						msg.time_raw = dr["c026"].ToString() + " - " + this.From8ToDate(AddDays(dr["c027"].ToString(), 1));
					}
					else if (msg.id.ToUpper() == "CISEXPDF")
					{
						msg.construction = dr["c001"].ToString();
						msg.measpnt = "";
						msg.net_area = dr["c002"].ToString();
						msg.time = this.From8ToDate(dr["c003"].ToString()) + " - " + this.From8ToDate(dr["c004"].ToString());
						msg.time_raw = dr["c003"].ToString() + " - " + dr["c004"].ToString();

					}
					msg.row_id = dr["row_id"].ToString();
					msg.valftime_raw = dr["valftime"].ToString()=="NULL" ? "" : dr["valftime"].ToString();
					msg.valttime_raw = dr["valttime"].ToString()=="NULL" ? "" : dr["valttime"].ToString();
					msgs.Add(msg);
				}
				else
				{
					if (msg.id.ToUpper() == "METERCN2")
					{
						msg = (Message)msgs[msgs.Count - 1];
						if (dr["c004"].ToString().ToUpper() == "NEDTAGEN")
							msg.old_meter = dr["c003"].ToString();
						else if (dr["c004"].ToString().ToUpper() == "UPPSATT")
							msg.new_meter = dr["c003"].ToString();
						msgs[msgs.Count - 1] = msg;
					}
				}
			}
			return msgs;
		}

		public string AddDays(string time, int days)
		{
			if (time.Equals("20360101"))
				return time;
			try
			{
				return DateTime.ParseExact(time, "yyyyMMdd",
				                           CultureInfo.InvariantCulture).AddDays(days).ToString("yyyyMMdd");
			}
			catch (Exception)
			{

				return time;
			}
            
            
		}

		public string From17ToDate(string date)
		{
			try
			{
				if (date.Length > 13 && this.IsInteger(date))
				{
					DateTime dt = new DateTime(Int32.Parse(date.Substring(0, 4)), Int32.Parse(date.Substring(4, 2)), Int32.Parse(date.Substring(6, 2)), Int32.Parse(date.Substring(8, 2)), Int32.Parse(date.Substring(10, 2)), Int32.Parse(date.Substring(12, 2)));
					return dt.ToShortDateString() + " " + dt.ToLongTimeString();
				}
				else
					return "";
			}
			catch (Exception)
			{

				return "";
			}
			
		}

		public string From8ToDate(string date)
		{
			try
			{
				if (date.Equals("20360101"))
				{
					return "->";
				}
				if (date.Length == 8 && this.IsInteger(date))
				{
					DateTime dt = new DateTime(Int32.Parse(date.Substring(0, 4)), Int32.Parse(date.Substring(4, 2)), Int32.Parse(date.Substring(6, 2)));
					return dt.ToShortDateString();
				}
				else
					return "";

			}
			catch (Exception)
			{

				return "";
			}
            
		}

		public string BuildQuery()
		{
			StringBuilder query = new StringBuilder();
			query.Append("select distinct id,instime,updtime,group_id,group_seqno,status,c001,c023,c002,c003,c004,c005,c006,c008,c010,c011,c012,c016,c017,c019,c026,c027,row_id,valftime,valttime,message from view_vf_input");
			StringBuilder where = new StringBuilder();
		    
			if (this.MessageStatus != "")
			{
				string[] statuses = MessageStatus.Split(',');
				for (int i = 0; i < statuses.Length; i++)
				{
					if (where.Length == 0) where.Append(" where ");
					if (i == 0)
						where.Append("(status='" + statuses[i]  + "'");
					else
						where.Append(" or status='" + statuses[i] + "'");
				}
				where.Append(")");
                
			}
			if (this.StartDate.Ticks != 0)
			{
				if (where.Length == 0) where.Append(" where ");
				else where.Append(" and ");
				where.Append("updtime > " + ToOracleDate(this.StartDate));
			}
			if (this.EndDate.Ticks != 0)
			{
				if (where.Length == 0) where.Append(" where ");
				else where.Append(" and ");
				where.Append(" updtime < " + ToOracleDate(this.EndDate));
			}
			if (!string.IsNullOrEmpty(this.GridArea))
			{
				if (where.Length == 0) where.Append(" where ");
				else where.Append(" and ");
				where.Append(" (C001 like '" + GridArea + "' OR " + "C002 like '" + GridArea + "')");
			}
			if (!string.IsNullOrEmpty(this.Installation))
			{
				if (where.Length == 0) where.Append(" where ");
				else where.Append(" and ");
				where.Append(" (C001 like '" + Installation + "' OR " + "C002 like '" + Installation + "')");
			}
			if (where.Length == 0) where.Append(" where ");
			else where.Append(" and ");
			if (this.MsgType != "")
			{
				where.Append("id='" + this.MsgType + "'");
			}
			else
			{
				for (int i=0;i<this.MessageTypes.Count;i++)
				{
					if (i == 0)
						where.Append("(id='" + this.MessageTypes[i].ToString() + "'");
					else
						where.Append(" or id='" + this.MessageTypes[i].ToString() + "'");
				}
				where.Append(")");
			}
			query.Append(where);
			query.Append(" ORDER BY group_id");
			return query.ToString();
		}

		private string DateTo17(DateTime date)
		{
			StringBuilder newDate = new StringBuilder();
			newDate.Append(date.ToString("yyyyMMddHHmmss"));
			newDate.Append("000");
			return newDate.ToString();
		}

		public DataTable GetRawData(ArrayList ids)
		{
			OracleCommand oCmd = new OracleCommand();
			StringBuilder cmd = new StringBuilder();
			cmd.Append("select instime,id,event,msgid,status,updtime,group_id,group_seqno,group_count,version");
			for (int i=1;i<61;i++)
				cmd.Append("," + this.FormatNumber(i)); //this.from vf_input where group_id='" + ids[0] + "'");
			cmd.Append(" from vf_input where group_id='" + ids[0] + "'");
			for (int i=1;i<ids.Count;i++)
				cmd.Append(" or group_id='" + ids[i] + "'");
			oCmd.CommandText = cmd.ToString();
			return Util.CommandToDataTable(oCmd);
		}

		public DataTable GetLogData(ArrayList ids)
		{
			OracleCommand oCmd = new OracleCommand();
			StringBuilder cmd = new StringBuilder();
			cmd.Append("select id,c001,c002,c003,c004,c008,c026,c027,msgid,group_id,group_seqno");
			//for (int i = 1; i < 51; i++)
			//    cmd.Append("," + this.FormatNumber(i)); //this.from vf_input where group_id='" + ids[0] + "'");
			cmd.Append(" from vf_input where group_id='" + ids[0] + "'");
			for (int i = 1; i < ids.Count; i++)
				cmd.Append(" or group_id='" + ids[i] + "'");
			oCmd.CommandText = cmd.ToString();
			return Util.CommandToDataTable(oCmd);
		}   

		private void Identify(IDbConnection connection, string osUser, string dbLang, string progKey, string appKey)
		{
			StringBuilder myStringBuilder = new StringBuilder();
			myStringBuilder.Append("BEGIN");
			myStringBuilder.Append(" pdmisc.identify(");
			myStringBuilder.AppendFormat("initials => '{0}'", osUser);
			myStringBuilder.AppendFormat(", language_in_ISO_639_1 => '{0}'", dbLang);
			myStringBuilder.AppendFormat(", prog_key => {0}", progKey);
			if (appKey.Length > 0)
				myStringBuilder.AppendFormat(", app_key => {0}", appKey);
			myStringBuilder.Append(");");
			myStringBuilder.Append("END;");
			OracleCommand cmd = new OracleCommand(myStringBuilder.ToString());
			Util.ExecuteCommand(cmd, connection);
		}

		public ArrayList GetEventLogEntries(ArrayList ids)
		{

			EventLogEntry entry = new EventLogEntry();
			ArrayList entries = new ArrayList();
			if (ids.Count == 0)
				return entries;
			string tmp;

			StringBuilder sbCmd = new StringBuilder();
			OracleCommand oCmd = new OracleCommand();

			// first get row id's and app_key
			sbCmd.Append("select c048 as \"app_key\",min(c049) as \"start\",max(c050) as \"end\" from vf_input where group_id in (");
			for (int i = 0; i < ids.Count; i++)
			{
				tmp = ids[i].GetType().ToString();
				if (ids[i].GetType().ToString() == "Powel.Icc.Data.MessageIdAndStatus")
					sbCmd.Append(((MessageIdAndStatus)ids[i]).Id + ",");
				else
					sbCmd.Append(ids[i] + ",");
			}
			sbCmd.Remove(sbCmd.Length - 1, 1);//get rid of last comma
			sbCmd.Append(") and c048>' ' and c049>' ' and c050>' ' group by c048");
			oCmd.CommandType = CommandType.Text;
			oCmd.CommandText = sbCmd.ToString();
			DataTable dt = Util.CommandToDataTable(oCmd);
			// now get entries from the eventlog
			//are there any entries?
			if (dt.Rows.Count > 0)
			{
				if (!dt.Rows[0].IsNull(0))
				{
					sbCmd.Remove(0, sbCmd.Length);
					sbCmd.Append("select rcount,pdmisc.toDateFrom17(datetim) as tim ,descript from icc_appl where ");
					foreach (DataRow dr in dt.Rows)
					{
						sbCmd.Append("(app_key=" + dr["app_key"].ToString() + " and rcount between " + dr["start"].ToString() + " and " + dr["end"].ToString() + ") or ");
					}
					sbCmd.Remove(sbCmd.Length - 4, 4); //get rid of last " or "
					////Reject entries for selected messages
					//sbCmd.Append(") OR (message_key=6900 and(");
					//for (int i = 0; i < ids.Count; i++)
					//{

					//    if (ids[i].GetType().ToString() == "Powel.Icc.Data.MessageIdAndStatus")
					//        sbCmd.Append("descript like '%" + ((MessageIdAndStatus)ids[i]).Id + "%' or ");
					//    else
					//        sbCmd.Append("descript like '%" + ids[i] + "%' or ");

					//}
					//sbCmd.Remove(sbCmd.Length - 4, 4); //get rid of last " or "
					//sbCmd.Append("))");

					sbCmd.Append(" order by rcount");
					oCmd.CommandText = sbCmd.ToString();
					dt.Clear();
					dt = Util.CommandToDataTable(oCmd);
					foreach (DataRow dr in dt.Rows)
					{
						entry.rcount = dr["rcount"].ToString();
						entry.datetim = dr["tim"].ToString();
						entry.descript = dr["descript"].ToString();
						entries.Add(entry);
					}

				}



			}
			//else
			//{
			//    sbCmd.Remove(0, sbCmd.Length);
			//    sbCmd.Append("select rcount,pdmisc.toDateFrom17(datetim) as tim ,descript from icc_appl where (");
			//    sbCmd.Append("message_key=6900 and(");
			//    for (int i = 0; i < ids.Count; i++)
			//    {

			//        if (ids[i].GetType().ToString() == "Powel.Icc.Data.MessageIdAndStatus")
			//            sbCmd.Append("descript like '%" + ((MessageIdAndStatus)ids[i]).Id + "%' or ");
			//        else
			//            sbCmd.Append("descript like '%" + ids[i] + "%' or ");

			//    }
			//    sbCmd.Remove(sbCmd.Length - 4, 4); //get rid of last " or "
			//    sbCmd.Append("))");

			//    sbCmd.Append(" order by rcount");
			//    oCmd.CommandText = sbCmd.ToString();
			//    dt.Clear();
			//    dt = Util.CommandToDataTable(oCmd);
			//    if (dt.Rows.Count > 0)
			//    {
			//        foreach (DataRow dr in dt.Rows)
			//        {
			//            entry.rcount = dr["rcount"].ToString();
			//            entry.datetim = dr["tim"].ToString();
			//            entry.descript = dr["descript"].ToString();
			//            entries.Add(entry);
			//        }
			//    }

			//}

			return entries;
		}

		public void RejectMessages(ArrayList ids)
		{
			SkipMessages(FindMessagesToSkip(ids));
			OracleCommand oCmd = new OracleCommand();
			StringBuilder cmd = new StringBuilder();
			cmd.Append("update vf_input set status='REJECTED',updtime=" + ToOracleDate(DateTime.Now) +  " where group_id='" + ids[0] + "'");
			for (int i = 1; i < ids.Count; i++)
				cmd.Append(" or group_id='" + ids[i] + "'");
			oCmd.CommandText = cmd.ToString();
			Util.ExecuteCommand(oCmd);



		}

		public static void CommentMessages(ArrayList ids,string comment)
		{
			OracleCommand oCmd = new OracleCommand();
			StringBuilder cmd = new StringBuilder();
			cmd.Append("update vf_input set message='"+comment+"' where group_id='" + ids[0] + "'");
			for (int i = 1; i < ids.Count; i++)
				cmd.Append(" or group_id='" + ids[i] + "'");
			oCmd.CommandText = cmd.ToString();
			Util.ExecuteCommand(oCmd);

		}

		public void UpdateVfInputWith048c049AndC050Data(string id)
		{
			string rcount ;
			string app_key;

			//select max(rcount) from icc_appl where message_key=6900 and app_key in (select app_key from icc_app a where a.name='message_control' and osuser_key=62 and user_key=1);
			//select app_key from icc_appl where rcount=1129532;
			using (IDbConnection connection = Util.OpenConnection())
			{
				OracleCommand cmd =
					new OracleCommand("select max(rcount) from icc_appl where message_key=6900 and app_key in (select app_key from icc_app a where a.name='message_control')");
				rcount = Convert.ToString(Powel.Icc.Data.Util.CommandToScalar(cmd));
				cmd.CommandText = "select app_key from icc_appl where rcount="+rcount;
				app_key = Convert.ToString(Powel.Icc.Data.Util.CommandToScalar(cmd));

				cmd.CommandText = "update vf_input set C048=" + app_key + "," + "C049=" + rcount + "," + "C050=" + rcount + " where group_id='" + id + "'";
				Util.ExecuteCommand(cmd);
			}
            
		}

		private ArrayList FindMessagesToSkip(ArrayList ids)
		{
			ArrayList messagesToSkip=new ArrayList();
			StringBuilder query = new StringBuilder();
			query.Append("select id,group_id,c001,c002 from view_vf_input where group_id in ('" + ids[0] + "'");
			for (int i = 1; i < ids.Count; i++)
			{
				query.Append(",'" + ids[i] + "'");
			}
			query.Append(") and ID not like 'METERCN2' and (status='INVALID' OR status='ERROR' OR status='STOP_RETRO')");

			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = query.ToString();
			DataTable dt= Util.CommandToDataTable(oCmd);
			MessageIdGridAreaAndInstallations msg = new MessageIdGridAreaAndInstallations();
			foreach (DataRow dr in dt.Rows)
			{
				if (StatusInvalidForMoreThanOneMessage(dr["c001"].ToString(),dr["c002"].ToString()))
					break;
                
				msg=new MessageIdGridAreaAndInstallations();
                
				msg.GroupId = dr["group_id"].ToString();
				if (dr["id"].ToString() == "SUPPLCNG")
				{
					msg.InstallationId = dr["c002"].ToString();
					msg.GridId = dr["c001"].ToString();
				}
				else
				{
					msg.InstallationId = dr["c001"].ToString();
					msg.GridId = dr["c002"].ToString();
				}

				messagesToSkip.Add(msg);
			}

			return messagesToSkip;

		}

		private void SkipMessages(ArrayList ids)
		{
			OracleCommand oCmd = new OracleCommand();
			StringBuilder cmd = new StringBuilder();

			MessageIdGridAreaAndInstallations param;
			for (int i = 0; i < ids.Count; i++)
			{
				cmd = new StringBuilder();
				param = (MessageIdGridAreaAndInstallations)ids[i];
				cmd.Append(" DELETE FROM VF_INPUT_ON_HOLD WHERE (installation_id='" + param.InstallationId + "' AND grid_id='" + param.GridId + "')");
				oCmd.CommandText = cmd.ToString();
				Util.ExecuteCommand(oCmd);
			}

            
			for (int i = 0; i < ids.Count; i++)
			{
				cmd = new StringBuilder();
				param = (MessageIdGridAreaAndInstallations)ids[i];
				cmd.Append(" UPDATE vf_input SET status='NEW',updtime="+ ToOracleDate(DateTime.Now)+" WHERE ((id != 'SUPPLCNG' AND c001='" + param.InstallationId + "' AND c002='" + param.GridId + "')");
				cmd.Append(" OR (id = 'SUPPLCNG' AND c002='" + param.InstallationId + "' AND c001='" + param.GridId + "'))");
				cmd.Append(" AND status = 'ON_HOLD'");
				oCmd.CommandText = cmd.ToString();
				Util.ExecuteCommand(oCmd);
			} 
            
		}

		public ArrayList ImportMessages(ArrayList ids, Boolean import, string osUser, string dbLang, string progKey)
		{
			using (IDbConnection connection = Util.OpenConnection())
			{
				Identify(connection, osUser, dbLang, progKey, "");
				int iImport = 0;
				StringBuilder sbCmd = new StringBuilder();
				OracleCommand oCmd = new OracleCommand();
				oCmd.CommandType = CommandType.StoredProcedure;
				OracleParameter par;
				iImport = import ? 1 : 0;
				int iDebug = this.Debug ? 1 : 0;
				ArrayList ids_and_stats = this.GetIdsAndStatuses(ids);
				MessageIdAndStatus msg;
				for (int i = 0; i < ids_and_stats.Count; i++)
				{
					msg = (MessageIdAndStatus)ids_and_stats[i];
					switch (GetType(msg.Type))
					{
						case MessageType.METERCN2:
							oCmd.CommandText = "icc_vipxpress_metercn2.meterUpdate";
							break;
						case MessageType.SUPPLCNG:
							oCmd.CommandText = "icc_vipxpress_supplcng.supplierUpdate";
							break;
						case MessageType.CISUPDAT:
							oCmd.CommandText = "icc_vipxpress_cisupdat.cisUpdate";
							break;
						case MessageType.CISEXPDF:
							oCmd.CommandText = "icc_vipxpress_cisexpdf.expdefUpdate";
							break;
						default:
							//TODO: warning
							oCmd.CommandText = "";
							break;
					}
					par = oCmd.Parameters.Add("groupId", OracleDbType.Int64);
					par.Direction = ParameterDirection.Input;
					par.Value = msg.Id;

					par = oCmd.Parameters.Add("doCommit", OracleDbType.Int32);
					par.Value = iImport;
					par = oCmd.Parameters.Add("oldStatus", OracleDbType.Varchar2);
					par.Value = msg.Status;
					par = oCmd.Parameters.Add("tmpStatus", OracleDbType.Varchar2);
					par.Value = "WORKING";
					par = oCmd.Parameters.Add("newStatus", OracleDbType.Varchar2);
					if (import)
						par.Value = "UPDATED";
					else
						par.Value = msg.Status;

					par = oCmd.Parameters.Add("iDebug", OracleDbType.Int32);
					par.Value = iDebug;
					Util.ExecuteCommand(oCmd, connection);
					oCmd.Parameters.Clear();
				}
			}
			return GetEventLogEntries(ids);
		}

		/// <summary>
		/// Sets the DB environment variable to a defined value.
		/// </summary>
		/// <param name="variable">The wanted variable.</param>
		/// <param name="value">The new value of the variable.</param>
		public static void SetDBEnvironment(string variable, string value)
		{
			using (IDbConnection connection = Util.OpenConnection())
			{
				try
				{
					//string querry = "update pddbenv set value = '" + value + "' where name = '" + variable + "'";
					//OracleCommand cmd = new OracleCommand(querry);
					//Util.ExecuteCommand(cmd, connection);

					OracleCommand cmd = new OracleCommand("pdmisc.setenv");
					cmd.CommandType = CommandType.StoredProcedure;
					cmd.Parameters.Add("p_name", OracleDbType.Varchar2, 64, variable, ParameterDirection.Input);
					cmd.Parameters.Add("p_value", OracleDbType.Varchar2, 256, value, ParameterDirection.Input);
					Powel.Icc.Data.Util.ExecuteCommand(cmd, connection);
				}
				catch (Exception)
				{
				}


			}
		}

		/// <summary>
		/// Get the DB environment variable to a defined value.
		/// </summary>
		/// <param name="name">The wanted variable.</param>
		/// <param name="defaultValue">return value if not found value.</param>
		public static string GetDBEnvironment(string name, string defaultValue)
		{
			string value="";
			using (IDbConnection connection = Util.OpenConnection())
			{
				OracleCommand cmd =
					new OracleCommand("select nvl(max(value),-1) as value  from pddbenv where name= '" + name + "'");
				value = Convert.ToString(Powel.Icc.Data.Util.CommandToScalar(cmd));
			}

			return value == "-1" ? defaultValue : value;
		}

		


		public ArrayList GetMessageStatuses()
		{
			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = "select distinct status from vf_input";
            ArrayList stats = new ArrayList();

            using (IDbConnection connection = Util.OpenConnection())
            using (var reader = Util.CommandToDataReader(oCmd, connection))
            {
                while (reader.Read())
                {
                    stats.Add(reader.GetString(0));
                }
            }

			return stats;
		}

		public ArrayList GetMessageTypes()
		{
			ArrayList types = new ArrayList();
			types.Add("CISUPDAT");
			types.Add("CISEXPDF");
			types.Add("METERCN2");
			types.Add("SUPPLCNG");
			return types;
		}

		private string ToOracleDate(DateTime date)
		{
			return "to_date('" + date.ToString("yyyyMMddHHmmss") + "','YYYYMMDDHH24MISS')";
		}

		private string FormatNumber(int i)
		{
			if (i<10)
				return "C00" + i;
			else
				return "C0" + i;
		}

		public bool IsInteger(String strNumber)
		{
			Regex objNotIntPattern=new Regex("[^0-9-]");
			Regex objIntPattern=new Regex("^-[0-9]+$|^[0-9]+$");
			return  !objNotIntPattern.IsMatch(strNumber) &&  objIntPattern.IsMatch(strNumber);
		}

		public int ReserveLock(string appName, int timeOut)
		{
			int iLock = -10;
			//			int i = 0;
			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = "select icc_lock.reserve_lock('PD$MESSAGE_CONTROL','" + appName + "'," + timeOut.ToString() + ") from dual";
			//			while (iLock < 0 && i < 5)
			//			{
			iLock = Int32.Parse(Util.CommandToScalar(oCmd).ToString());
			//				System.Threading.Thread.Sleep(250);
			//				i++;
			//			}
			return iLock;
		}

		public int ReleaseLock(int lockId)
		{
			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = "select icc_lock.release_lock(" + lockId.ToString() + ") from dual";
			return Int32.Parse(Util.CommandToScalar(oCmd).ToString());
		}

		private int MaintainLock(int lockId)
		{
			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = "select icc_lock.maintain_lock(" + lockId.ToString() + ") from dual";
			return Int32.Parse(Util.CommandToScalar(oCmd).ToString());
		}

		private ArrayList GetIdsAndStatuses(ArrayList ids)
		{
			OracleCommand oCmd = new OracleCommand();
			StringBuilder sbCmd = new StringBuilder();
			sbCmd.Append("select distinct group_id, status, id from vf_input where group_id in (");
			for (int i=0;i<ids.Count;i++)
			{
				sbCmd.Append(ids[i].ToString() + ",");
			}
			//remove last comma
			sbCmd.Remove(sbCmd.Length - 1,1);
			sbCmd.Append(") order by group_id");
			oCmd.CommandText = sbCmd.ToString();
			DataTable dt = Util.CommandToDataTable(oCmd);

			MessageIdAndStatus msg;
			ArrayList msgs = new ArrayList();
			if (dt.Rows.Count > 0)
			{
				if (!dt.Rows[0].IsNull(0))
				{
					foreach (DataRow dr in dt.Rows)
					{
						msg = new MessageIdAndStatus();
						msg.Id = Int64.Parse(dr["group_id"].ToString());
						msg.Status = dr["status"].ToString();
						msg.Type = dr["id"].ToString();
						msgs.Add(msg);
					}
				}
			}
			return msgs;
		}

		public bool StatusChangedToUpdated(ArrayList ids)
		{
			OracleCommand oCmd = new OracleCommand();
			StringBuilder sbCmd = new StringBuilder();
			if (ids.Count > 0)
			{
				sbCmd.Append("select count(*) from vf_input where status='UPDATED' and group_id in (");
				for (int i=0;i<ids.Count;i++)
				{
					sbCmd.Append(ids[i] + ",");
				}
				sbCmd.Remove(sbCmd.Length - 1,1); 
				sbCmd.Append(")");
				oCmd.CommandText = sbCmd.ToString();
				if (Int32.Parse(Util.CommandToScalar(oCmd).ToString()) > 0)
					return true;
			}
			return false;
		}


		public bool StatusInvalidForMoreThanOneMessage(string c001, string c002)
		{
			OracleCommand oCmd = new OracleCommand();
			StringBuilder sbCmd = new StringBuilder();
			sbCmd.Append("select count(*) from vf_input where status='INVALID' and ((c001 = '" + c001 + "' and c002='" 
			             + c002 + "') or (c001 = '" + c002 + "' and c002='" + c001 + "'))");
			oCmd.CommandText = sbCmd.ToString();
			if (Int32.Parse(Util.CommandToScalar(oCmd).ToString()) > 1)
				return true;
           
			return false;
		}

		public bool StatusInvalid(ArrayList ids)
		{
			OracleCommand oCmd = new OracleCommand();
			StringBuilder sbCmd = new StringBuilder();
			if (ids.Count > 0)
			{
				sbCmd.Append("select count(*) from vf_input where status='INVALID' and group_id in (");
				for (int i = 0; i < ids.Count; i++)
				{
					sbCmd.Append(ids[i] + ",");
				}
				sbCmd.Remove(sbCmd.Length - 1, 1);
				sbCmd.Append(")");
				oCmd.CommandText = sbCmd.ToString();
				if (Int32.Parse(Util.CommandToScalar(oCmd).ToString()) > 0)
					return true;
			}
			return false;
		}

		public bool StatusManual(ArrayList ids)
		{
			OracleCommand oCmd = new OracleCommand();
			StringBuilder sbCmd = new StringBuilder();
			if (ids.Count > 0)
			{
				sbCmd.Append("select count(*) from vf_input where status='MANUAL' and group_id in (");
				for (int i = 0; i < ids.Count; i++)
				{
					sbCmd.Append(ids[i] + ",");
				}
				sbCmd.Remove(sbCmd.Length - 1, 1);
				sbCmd.Append(")");
				oCmd.CommandText = sbCmd.ToString();
				if (Int32.Parse(Util.CommandToScalar(oCmd).ToString()) > 0)
					return true;
			}
			return false;
		}

		public static void EnqueueInputMessage(string message)
		{
			QueueMessageType messageType = GetMessageType(message);
			EnqueueInputMessage(message, messageType, null, null);
		}
		public static void EnqueueInputMessage(string message, string systemID)
        {
			QueueMessageType messageType = GetMessageType(message);
			EnqueueInputMessage(message, messageType, null, systemID);
        }
		public static void EnqueueInputMessage(string message, QueueMessageType messageType)
		{
			EnqueueInputMessage(message, messageType, null, null);
		}
		public static void EnqueueInputMessage(string message, QueueMessageType messageType, string version)
        {
			EnqueueInputMessage(message, messageType, version, null);
        }
		public static void EnqueueInputMessage(string message, QueueMessageType messageType, string version, string systemID)
		{
			OracleCommand cmd;

			using (IDbConnection connection = Util.OpenConnection())
			{
				string id = Guid.NewGuid().ToString().Substring(0, 8);
				string query;
				if (version == null && String.IsNullOrEmpty(systemID))
					query = "insert into vf_input (id, msgtype, group_seqno, message, event, status) values (:1, :2, :3, :4, 'N/A', 'N/A')";
				else if (version == null)
					query = "insert into vf_input (id, msgtype, group_seqno, message, event, status, c002) values (:1, :2, :3, :4, 'N/A', 'N/A', :5)";
				else if (String.IsNullOrEmpty(systemID))
					query = "insert into vf_input (id, msgtype, group_seqno, message, version, event, status) values (:1, :2, :3, :4, :5,'N/A', 'N/A')";
				else
					query = "insert into vf_input (id, msgtype, group_seqno, message, version, event, status, C002) values (:1, :2, :3, :4, :5,'N/A', 'N/A', :6)";
				cmd = new OracleCommand(query);
				cmd.Parameters.Add(null, id);
				cmd.Parameters.Add(null, messageType.ToString());
				cmd.Parameters.Add(null, (int)messageType);
				cmd.Parameters.Add(null, message);
				if (version != null)
					cmd.Parameters.Add(null, version);
				if (!String.IsNullOrEmpty(systemID))
					cmd.Parameters.Add(null, systemID);

				Util.ExecuteCommand(cmd, connection);
			}
		}

		public static Message DequeueInputMessage(QueueMessageType messageType,
                                                  IDbConnection connection, string systemID, string status = "claimed")
		{
			List<QueueMessageType> messageTypeList = new List<QueueMessageType>();
			messageTypeList.Add(messageType);
			return DequeueInputMessage(messageTypeList, connection, systemID, status);
		}


        public static Message DequeueInputMessage(List<QueueMessageType> messageTypeList, IDbConnection connection, string systemID, string status = "claimed")
		{
			Message msg = GetInputMessage(messageTypeList, connection, status, systemID);

			if (msg.message != null)
			{
				OracleCommand cmd;
				cmd = new OracleCommand("delete from vf_input where id = :1");
				cmd.Parameters.Add(null, msg.id);

				Util.ExecuteCommand(cmd, connection);
			}
            
			return msg;

		}

        private static Message GetInputMessage(List<QueueMessageType> messageTypeList, IDbConnection connection, string status, string systemID)
		{
			OracleCommand cmd;
			Message msg = new Message();
			string messageTypes = string.Empty;
			foreach(QueueMessageType messageType in messageTypeList)
			{
				messageTypes += ",'" + messageType + "'";
			}

			messageTypes = messageTypes.Substring(1);
		tryOnceMore:
			//ToDo: Parameterized query could be more efficient.
			cmd =
				new OracleCommand(
					"select id,message,msgtype,version from vf_input where instime=(select min(instime) from vf_input where msgtype in (" +
					messageTypes + ") and status <> '" + status + (String.IsNullOrEmpty(systemID) ? "" : "' and c002='" + systemID) + "') and msgtype in (" + messageTypes +
					") and status <> '" + status + (String.IsNullOrEmpty(systemID) ? "" : "' and c002='" + systemID) + "' and rownum<=1");

			DataTable dt = Util.CommandToDataTable(cmd, connection);

			if (dt.Rows.Count == 0)
			{
				msg.message = null;
				return msg;
			}
			msg.id = (string)dt.Rows[0]["id"];
            Console.WriteLine("Msg.id=" + msg.id + ", nRows=" + dt.Rows.Count.ToString());

            cmd = new OracleCommand("update vf_input set status='" + status + "' where id='" + msg.id + "' and status <> '" + status + "'");
            int rowsAffected = Util.ExecuteCommand(cmd, connection);
            Console.WriteLine("rowsAffected=" + rowsAffected);
            if (rowsAffected < 1)
                goto tryOnceMore;
            cmd = new OracleCommand("commit");
            Util.ExecuteCommand(cmd, connection);

			msg.msgtype = (string) dt.Rows[0]["msgtype"];
			object v = dt.Rows[0]["version"];
			object o = dt.Rows[0]["message"];

			msg.version = (v == DBNull.Value) ? null : (string)v;
			msg.message = (o == DBNull.Value) ? null : (string)o;
			msg.systemID = systemID;
			return msg;
		}
		public static Message ClaimInputMessage(QueueMessageType messageType,
		                                        IDbConnection connection, string status, string systemID)
		{
			List<QueueMessageType> messageTypeList = new List<QueueMessageType>();
			messageTypeList.Add(messageType);
			return ClaimInputMessage(messageTypeList, connection, status, systemID);
		}

		public static Message ClaimInputMessage(List<QueueMessageType> messageTypeList, 
		                                        IDbConnection connection, string status, string systemID)
		{
		    Message msg;
			msg = GetInputMessage(messageTypeList, connection, status, systemID);
			return msg;
		}

		public static DataTable FetchNewReadings()
		{
			OracleCommand cmd;

			string sql =
				@"select v.rowid, c017 globeref, c005 time, c006 value, u.code unit, c020 as status, c024 UsedForInterpol 
from vf_output v 
join timeser t on v.c016 = t.tims_key 
join unmea_tp u on t.unme_key = u.unme_key 
where status = 'NEW' and id in ('READING', 'READBTCH')
order by globeref,time desc";

			//@"select v.rowid, c017 globeref, c005 time, c006 value, u.code unit, c020 as status, i.instr_no as meterID 
			//from vf_output v 
			//join timeser t on v.c016 = t.tims_key 
			//join unmea_tp u on t.unme_key = u.unme_key 
			//join meas_ts mt on mt.tims_key = v.c016 
			//join meas_pnt mp on mp.meas_key = mt.meas_key 
			//join instr_st s on s.meas_key = mt.meas_key 
			//join instrument i on i.inst_key = s.inst_key and i.state=1 
			//join instr_rg ir on i.inst_key = ir.inst_key
			//join master m on m.mstr_key = i.mstr_key 
			//and s.valftime <= v.c005 
			//and s.valttime > v.c005 
			//and mp.valftime <= v.c005 
			//and mp.valttime > v.c005 
			//and ir.valftime <= v.c005 
			//and ir.valttime > v.c005 
			//where status = 'NEW' and id in ('READING', 'READBTCH')
			//and ir.tser_key = t.tser_key and ir.unme_key = t.unme_key and ir.tsin_key = t.tsin_key
			//order by instime";
			cmd = new OracleCommand(sql);
			return Util.CommandToDataTable(cmd);
		}

		public static void UpdateRowStatus(string[] rowids, string status)
		{
			StringBuilder sql = new StringBuilder();
			sql.Append("update vf_output set status = :1 where rowid in (");

			for (int i=0; i<rowids.Length; i++)
			{
				if (i > 0)
					sql.Append(", ");

				sql.Append("'" + rowids[i] + "'");
			}

			sql.Append(")");

			OracleCommand cmd = new OracleCommand(sql.ToString());
			cmd.Parameters.Add(null, status);

			Util.ExecuteCommand(cmd);
		}

		public static void EnqueueOutputMessage(QueueMessageType messageType, string message)
		{
			EnqueueOutputMessage(messageType, message, null, null);
		}

		public static void EnqueueOutputMessage(QueueMessageType messageType, string message, string version)
        {
			EnqueueOutputMessage(messageType, message, version, null);
        }
		public static void EnqueueOutputMessage(QueueMessageType messageType, string message, string version, string systemID)

		{
			OracleCommand cmd;

			using (IDbConnection connection = Util.OpenConnection())
			{
				string id = Guid.NewGuid().ToString().Substring(0, 8);

				string query;
				if (version == null && String.IsNullOrEmpty(systemID))
					query = "insert into vf_output (instime, id, msgtype, message, event, status, c001) values (sysdate,:1, :2, :3, 'N/A', 'N/A', '0')";
				else if (version == null)
					query = "insert into vf_output (instime, id, msgtype, message, event, status, c001, c002) values (sysdate,:1, :2, :3, 'N/A', 'N/A', '0', :4)";
				else if (String.IsNullOrEmpty(systemID))
					query = "insert into vf_output (instime, id, msgtype, message, event, status, version, c001) values (sysdate,:1, :2, :3, 'N/A', 'N/A', :4, '0')";
				else
					query = "insert into vf_output (instime, id, msgtype, message, event, status, version, c001, c002) values (sysdate,:1, :2, :3, 'N/A', 'N/A', :4, '0', :5)";
				cmd = new OracleCommand(query);
				cmd.Parameters.Add(null, id);
				cmd.Parameters.Add(null, messageType.ToString());
				cmd.Parameters.Add(null, message);
				if (version != null)
					cmd.Parameters.Add(null, version);
				if (!String.IsNullOrEmpty(systemID))
					cmd.Parameters.Add(null, systemID);
				Util.ExecuteCommand(cmd, connection);
			}
		}
        
		public static Message DequeueOutputMessage( QueueMessageType messageType, IDbConnection connection, string systemID)
		{
			List<QueueMessageType> messageTypeList = new List<QueueMessageType>();
			messageTypeList.Add(messageType);
			return DequeueOutputMessage(messageTypeList, connection, systemID);
		}
		public static Message DequeueOutputMessage(List<QueueMessageType> messageTypeList,
		                                           IDbConnection connection, string systemID)
		{
			OracleCommand cmd;

			Message msg = new Message();

			string messageTypes = string.Empty;
			foreach (QueueMessageType messageType in messageTypeList)
			{
				messageTypes += ",'" + messageType + "'";
			}
			messageTypes = messageTypes.Substring(1);

			cmd = new OracleCommand(
			                        "select * from (select /*+ index( VF_OUTPUT IX_VF_OUTPUT_MSGTYPE) */ id, version, message, msgtype, nvl(c001,0) as c001 from vf_output where msgtype in (" +
			                        messageTypes + ")" + (!String.IsNullOrEmpty(systemID)?" and c002='" + systemID + "'":"") + " order by instime) where rownum <= 1");

			DataTable dt = Util.CommandToDataTable(cmd, connection);

			if (dt.Rows.Count == 0)
			{
				msg.message = null;
				return msg;
			}

			msg.id = (string) dt.Rows[0]["id"];
			msg.msgtype = (string) dt.Rows[0]["msgtype"];
			try
			{
				msg.count = (dt.Rows[0]["c001"] == DBNull.Value ? 0 : Convert.ToInt32((string)dt.Rows[0]["c001"]));
			}
			catch (Exception)
			{
				msg.count = 0;		        
			}
			var o = dt.Rows[0]["message"];
			var v = dt.Rows[0]["version"];
			msg.message = (o == DBNull.Value) ? null : (string)o;
			msg.version = (v == DBNull.Value) ? null : (string)v;
			msg.systemID = systemID;
			
			//version = Convert.ToInt32(msg.version);
			//version++;
			//msg.version = version.ToString();


			cmd = new OracleCommand("update vf_output set c001 = :1 where id = :2");
			cmd.Parameters.Add(null, ++msg.count);
			cmd.Parameters.Add(null, msg.id);	

			Util.ExecuteCommand(cmd, connection);

			return msg;
		}

		public static void DeleteOutputMessage(string id, IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("delete from vf_output where id = :1");
			cmd.Parameters.Add(null, id);

			Util.ExecuteCommand(cmd, connection);
		}
		public static void DeleteInputMessage(string id, IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("delete from vf_input where id = :1");
			cmd.Parameters.Add(null, id);

			Util.ExecuteCommand(cmd, connection);
		}

		public static QueueMessageType GetMessageType(string message)
		{
			if(message.IndexOf("submitTimeSeries")>1)
				return QueueMessageType.TIME_SERIES;
			else if (message.IndexOf("submitInstallationToQueue") > 1)
				return QueueMessageType.METERING_SERVICE_INSTALLATION;
			else if (message.IndexOf("submitComponentsToQueue") > 1)
				return QueueMessageType.METERING_SERVICE_COMPONENTS;
			else if (message.IndexOf("submitAgreementToQueue") > 1)
				return QueueMessageType.METERING_SERVICE_AGREEMENT;
			else if (message.IndexOf("submitTimeSeriesResponse") > 1)
				return QueueMessageType.TIME_SERIES_RESPONSE;

			return QueueMessageType.UNKNOWN_MESSAGE;
		}
	}
}
