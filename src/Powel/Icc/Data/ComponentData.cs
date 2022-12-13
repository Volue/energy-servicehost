/*
$Header$
*/
using System;
using System.Data;
using System.Collections;
using System.Text;
using System.Globalization;
using Oracle.ManagedDataAccess.Client;
using Powel.Icc.Services.Time;
using Powel.Icc.Common;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for ComponentData.
	/// </summary>
	public class ComponentData
	{
		static public RegionalCalendar Clocal;
		static public RegionalCalendar Cdb;

		public ComponentData()
		{
			Clocal = RegionalCalendar.CreateFromTimeZone(TimeZone.CurrentTimeZone, WeekOneRule.First4DayWeek, DayOfWeek.Monday, true);
			Cdb = RegionalCalendar.CreateFromTimeZone(TimeZone.CurrentTimeZone, WeekOneRule.First4DayWeek, DayOfWeek.Monday, false);
		}

		static public DateTime LocalDateTimeToDbDateTime(DateTime dtLocal)
		{
			return Cdb.ToDateTime(Clocal.ToUtcTime(dtLocal));
		}
		static public DateTime DbDateTimeToLocalDateTime(DateTime dtDb)
		{
			return Clocal.ToDateTime(Cdb.ToUtcTime(dtDb));
		}

		public DataTable GetComponentTypes(string compKeyList)
		{

			string query = "SELECT CMPC_KEY AS INST_TYP, NAME FROM VIEW_INSTR_TP";
			if (compKeyList.Length > 0)
				query += string.Format(" WHERE CMPC_KEY IN ({0})", compKeyList);
			query +=  " ORDER BY NAME";

			DataTable componentType = Util.SqlToDataTable(query);   
			componentType.TableName = "componentType";
			return componentType;
		}

		public DataTable GetComponentType(string inst_key)
		{
			StringBuilder query = new StringBuilder();
			query.Append("SELECT I.INST_TYP, TP.NAME FROM VIEW_INSTR_TP TP, INSTRUMENT I");
			query.AppendFormat(" WHERE I.INST_KEY = {0} AND TP.CMPC_KEY = I.INST_TYP",inst_key );

			DataTable componentType = Util.SqlToDataTable(query.ToString());   
			componentType.TableName = "componentType";
			return componentType;
		}


		public DataTable GetLots()
		{
			string query = "SELECT LOT_KEY,NAME FROM LOT ORDER BY NAME,LOT_KEY";
			DataTable lot = Util.SqlToDataTable(query);
			lot.TableName = "lot";
			return lot;
		}

		public DataTable GetMasters()
		{
			string query = "SELECT * FROM MASTER ORDER BY NAME";

			DataTable master = Util.SqlToDataTable(query);
			master.TableName = "master";
			return master;
		}

		public int GetMasterKey(string masterName, IDbConnection connection)
		{
		    OracleCommand cmd = new OracleCommand("SELECT mstr_key FROM master WHERE name = '" + masterName + "'");
			DataTable dt = Util.CommandToDataTable(cmd);
			if (dt.Rows.Count == 1)
				return (int) dt.Rows[0][0];
			else if(dt.Rows.Count < 1)
				throw new ApplicationException("Master: '" + masterName + "' not found.");
			else
				throw new ApplicationException("Ambiguoous masters '" + masterName + "' found.");
		}

		public DataTable GetMasters(string componentTypes)
		{
			string query = "";
			if (componentTypes.Length > 0)
				query = string.Format("SELECT * FROM MASTER WHERE INST_TYP IN ({0}) ORDER BY NAME", componentTypes);
			else
				query = "SELECT * FROM MASTER ORDER BY NAME";

			DataTable master = Util.SqlToDataTable(query);
			master.TableName = "master";
			return master;
		}

		public DataTable GetMaster(string mstr_key)
		{
			StringBuilder query = new StringBuilder();
			query.AppendFormat("SELECT * FROM MASTER WHERE MSTR_KEY = {0}", mstr_key);

			DataTable master = Util.SqlToDataTable(query.ToString());
			master.TableName = "master";
			return master;
		}

		public DataTable GetMasterByName(string name, string componentTypes)
		{
			string query = string.Format("SELECT * FROM MASTER WHERE NAME = '{0}'", name);
			if (componentTypes.Length > 0)
				query += string.Format(" AND INST_TYP IN ({0})", componentTypes);

			DataTable master = Util.SqlToDataTable(query);
			master.TableName = "master";
			return master;
		}

		public DataTable GetMasterByInstKey(string inst_key)
		{
			StringBuilder query = new StringBuilder();
			query.AppendFormat("SELECT m.* FROM MASTER M, INSTRUMENT I WHERE I.INST_KEY = {0} AND M.MSTR_KEY = I.MSTR_KEY", inst_key);

			DataTable master = Util.SqlToDataTable(query.ToString());
			master.TableName = "master";
			return master;
		}

		public DataTable GetComponentAttribute(string inst_key, string attribute)
		{
			StringBuilder query = new StringBuilder();
			query.Append(" SELECT v.inst_key, v.name, v.cvalue, v.rvalue, v.ivalue");
			query.Append(" FROM view_instr_vl v");
			query.AppendFormat(" WHERE v.inst_key = {0}", inst_key);
			query.AppendFormat(" AND v.name = '{0}'", attribute);

			DataTable meterAttr = Util.SqlToDataTable(query.ToString());   
			meterAttr.TableName = "meterAttr";
			return meterAttr;
		}

		public DataTable GetMasterAttribute(string mstr_key, string attribute)
		{
			StringBuilder query = new StringBuilder();
			query.Append(" SELECT v.mstr_key, v.name, v.cvalue, v.rvalue, v.ivalue");
			query.Append(" FROM view_mastr_vl v");
			query.AppendFormat(" WHERE v.mstr_key = {0}", mstr_key);
			query.AppendFormat(" AND v.name = '{0}'", attribute);

			DataTable masterAttr = Util.SqlToDataTable(query.ToString());   
			masterAttr.TableName = "masterAttr";
			return masterAttr;
		}

		public DataTable GetMasterAttributeByInstKey(string inst_key, string attribute)
		{
			StringBuilder query = new StringBuilder();
			query.Append(" SELECT v.mstr_key, v.name, v.cvalue, v.rvalue, v.ivalue");
			query.Append(" FROM view_mastr_vl v, instrument i");
            if (inst_key=="")
                query.Append(" WHERE i.inst_key = -1");
            else
			    query.AppendFormat(" WHERE i.inst_key = {0}", inst_key);
			query.Append(" AND v.mstr_key = i.mstr_key");
			query.AppendFormat(" AND v.name = '{0}'", attribute);

			DataTable masterAttr = Util.SqlToDataTable(query.ToString());   
			masterAttr.TableName = "masterAttr";
			return masterAttr;
		}

		public DataTable GetComponentsAttributes(string inst_key, string mstr_key)
		{
			StringBuilder query = new StringBuilder();
			query.Append("SELECT name, cvalue, rvalue, ivalue");
			query.Append(" FROM  view_mastr_vl");
			query.AppendFormat(" WHERE mstr_key = {0}", mstr_key);
			query.Append(" UNION");
			query.Append(" SELECT name, cvalue, rvalue, ivalue");
			query.Append(" FROM  view_instr_vl");
			query.AppendFormat(" WHERE inst_key = {0}", inst_key);

			DataTable compAttr = Util.SqlToDataTable(query.ToString());
			compAttr.TableName = "compAttr";
			return compAttr;
		}

		public DataTable GetComponentsAttributes(string inst_key)
		{
			StringBuilder query = new StringBuilder();
			query.Append("SELECT i.inst_key, v.name, v.cvalue, v.rvalue, v.ivalue");
			query.Append(" FROM instrument i, view_mastr_vl v");
			query.AppendFormat(" WHERE i.inst_key = {0}", inst_key);
			query.Append(" AND i.mstr_key = v.mstr_key"); 
			query.Append(" UNION");
			query.Append(" SELECT v.inst_key, v.name, v.cvalue, v.rvalue, v.ivalue");
			query.Append(" FROM view_instr_vl v");
			query.AppendFormat(" WHERE v.inst_key = {0}", inst_key);

			DataTable compAttr = Util.SqlToDataTable(query.ToString());
			compAttr.TableName = "compAttr";
			return compAttr;
		}

		public DataTable GetComponent(string inst_key)
		{
			StringBuilder query = new StringBuilder();
			query.AppendFormat("SELECT i.*, m.name AS master_name FROM instrument i, master m WHERE inst_key = {0} AND m.mstr_key = i.mstr_key", inst_key);

			DataTable component = Util.SqlToDataTable(query.ToString());
			component.TableName = "component";
			return component;
		}

		public DataTable GetComponent(string instr_no, string date)
		{
			StringBuilder query = new StringBuilder();
			// the view_instrument view allready ignores records with state 4 and 8 so that part could be removed
			query.AppendFormat("SELECT * FROM view_instrument WHERE instr_no = '{0}' AND valftime <= '{1}' AND valttime > '{1}' AND state NOT IN (4,8)", instr_no, date);

			DataTable component = Util.SqlToDataTable(query.ToString());
			component.TableName = "component";
			return component;
		}
		
		public DataTable GetComponentByKey(string inst_key, string date)
		{
			StringBuilder query = new StringBuilder();
			// the view_instrument view allready ignores records with state 4 and 8 so that part could be removed
			query.AppendFormat("SELECT * FROM view_instrument WHERE inst_key = {0} AND valftime <= '{1}' AND valttime > '{1}' AND state NOT IN (4,8)", inst_key, date);

			DataTable component = Util.SqlToDataTable(query.ToString());
			component.TableName = "component";
			return component;
		}

		public DataTable GetComponentByKeyOpenEnd(string inst_key, string date)
		{
			StringBuilder query = new StringBuilder();
			query.AppendFormat("SELECT * FROM view_instrument WHERE inst_key = {0} AND valftime <= '{1}' AND valttime >= '20360101000000000' AND state NOT IN (4,8)", inst_key, date);

			DataTable component = Util.SqlToDataTable(query.ToString());
			component.TableName = "component";
			return component;
		}

        public DataTable GetMeterReadRegisters(string inst_key, string date, bool justActiveReadings)
		{
			StringBuilder query = new StringBuilder();
			query.Append("SELECT *");
			query.Append(" FROM instr_rg i");
			query.AppendFormat(" WHERE i.inst_key = {0}", inst_key);
			query.AppendFormat(" AND i.valftime <= '{0}'AND i.valttime > '{0}'", date);
            if (justActiveReadings)
            {
			query.Append(" AND i.tsin_key = -1");
			query.Append(" AND i.state = 1");
            }
			query.Append(" ORDER BY i.inst_reg");

			DataTable compReg = Util.SqlToDataTable(query.ToString());
			compReg.TableName = "compReg";
			return compReg;
		}

		public DataRow[] ActivateMeterReadRegister(string inst_key, string date, string regNo, string resolution, string valueType, bool activate)
		{
			StringBuilder query = new StringBuilder();
			query.AppendFormat("SELECT * FROM instr_rg i WHERE i.inst_key = {0}", inst_key);
			query.AppendFormat(" AND i.inst_reg = {0}", regNo);
			query.AppendFormat(" AND i.valftime <= '{0}' AND i.valttime > '{0}'", date);
            if (String.IsNullOrEmpty(resolution))
            {
                query.Append(" AND i.tsin_key = -1");
            }
            else
            {
                query.AppendFormat(" AND i.tsin_key = (select tsin_key from tsint_tp where upper(code)=upper('{0}'))",
                                   resolution);
            }
            if (!String.IsNullOrEmpty(valueType))
            {
                query.AppendFormat(
                    " AND NVL(i.vltp_key,0) = (select tp_key from language_tp where upper(name)=upper('{0}'))",
                    valueType);
            }
		    query.Append(" ORDER BY i.inst_reg");

		    DataTable regList = Util.SqlToDataTable(query.ToString());
			if (regList.Rows.Count == 1)
			{
			    String cmdStr = "update instr_rg set valttime = '" + date + "' where inst_key = " + inst_key +
			                    " and inst_reg = " + regNo + " and valftime <= '" + date + "' and valttime > '" +
			                    date + "'";
                if (!String.IsNullOrEmpty(resolution))
                    cmdStr += " and tsin_key = (select tsin_key from tsint_tp where upper(code)=upper('" + resolution +
                              "'))";
                if (!String.IsNullOrEmpty(valueType))
                    cmdStr += " and nvl(vltp_key,0) = (select tp_key from language_tp where upper(name)=upper('" +
                              valueType + "'))";
				// terminate the actual version of register
				OracleCommand cmd = new OracleCommand(cmdStr);
				Util.ExecuteCommand(cmd);
				int seqno = Convert.ToInt32(regList.Rows[0]["seqno"]) + 1;
				string state = "0";
				if (activate)
					state = "1";
				// create a new version of register with a new state
				OracleCommand cmd2 = new OracleCommand("insert into instr_rg (inst_key,inst_reg,seqno,valftime,valttime," +
                                                       "state,tser_key,unme_key,tsin_key,ndigits,ndecimals,factor,direction," +
                                                       "automatic,vltp_key) values(" + inst_key + "," + regNo + "," + seqno +
													   ",'" + date + "','" + Convert.ToString(regList.Rows[0]["valttime"]) + "'," +
													   state + "," + Convert.ToString(regList.Rows[0]["tser_key"]) + "," +
													   Convert.ToString(regList.Rows[0]["unme_key"]) + "," +
													   Convert.ToString(regList.Rows[0]["tsin_key"]) + "," + 
													   Convert.ToString(regList.Rows[0]["ndigits"]) + "," +
													   Convert.ToString(regList.Rows[0]["ndecimals"]) + "," +
													   Convert.ToString(regList.Rows[0]["factor"]) + "," +
													   Convert.ToString(regList.Rows[0]["direction"]) + "," +
													   Convert.ToString(regList.Rows[0]["automatic"]) + "," +
                                                       Convert.ToString(regList.Rows[0]["vltp_key"]) + ")");
				Util.ExecuteCommand(cmd2);

				DataTable tbl = GetMeterReadRegisters(inst_key,date,true);
				return tbl.Select("inst_reg = " + regNo);
			}
			return regList.Select("inst_reg = " + regNo);
		}

		public DataSet GetMeterStockInfo(long instrOwnerKey, string spotCheckCountry)
		{
			string meterKeys = DbConfiguration.Component.IccCompMeterKey;
			string now = LocalDateTimeToDbDateTime(DateTime.Now).ToString("yyyyMMddHHmmss") + "000";

			StringBuilder query = new StringBuilder();
			query.Append("SELECT DISTINCT i.inst_key AS inst_key, i.instr_no, i.fabr_no, i.state, i.valftime");
			query.Append(", i.descript, m.mstr_key AS mstr_key"); //!!!, m.name AS master_name
			if (instrOwnerKey <= 0) //owner not specified
				query.Append(", v.ivalue as owner_key");
			if (spotCheckCountry == "sweden")
				query.Append(", na.svk_id as net_id");
			query.Append(" FROM view_instrument i, master m, instr_vl v");
			if (spotCheckCountry == "sweden")
				query.Append(", meas_pnt mp, installation il, netarea na");
			query.AppendFormat(" WHERE i.mstr_key = m.mstr_key AND i.inst_typ IN ({0})", meterKeys);
			query.AppendFormat(" AND i.valftime <= '{0}' AND i.valttime > '{0}'", now);
			if (instrOwnerKey > 0) //owner specified
				query.AppendFormat(" AND i.inst_key = v.inst_key AND v.val_key=1049 AND v.ivalue={0}", instrOwnerKey); //instr_owner
			else //owner not specified
				query.Append(" AND i.inst_key = v.inst_key(+) AND v.val_key(+)=1049"); //instr_owner
			if (spotCheckCountry == "sweden")
			{
				query.Append(" AND i.meas_key = mp.meas_key(+) AND mp.insl_key = il.insl_key(+)");
				query.Append(" AND il.neta_key = na.neta_key(+)");
				query.AppendFormat(" AND mp.valftime(+) <= '{0}' AND mp.valttime(+) > '{0}'", now);
				query.AppendFormat(" AND il.valftime(+) <= '{0}' AND il.valttime(+) > '{0}'", now);
			}
			if (spotCheckCountry == "norway")
				query.Append(" AND i.state in (1,2)"); //active/in stock
			query.Append(" ORDER BY i.instr_no");
			DataTable dtMeterInfo = Util.SqlToDataTable(query.ToString());
			dtMeterInfo.TableName = "meterInfo";

			DataSet ds = new DataSet();
			if (dtMeterInfo.Rows.Count == 0)
				return ds;

			ArrayList mstrKeyList = new ArrayList();

			// Get state text info
			if (spotCheckCountry == "sweden")
			{
				// Append state text column to the meter table
				dtMeterInfo.Columns.Add("state_text", System.Type.GetType("System.String"));
				// Look up revision state
				query.Remove(0, query.Length);
				query.Append("SELECT DISTINCT i.inst_key AS inst_key FROM instrument i, instr_st s");
				if (instrOwnerKey > 0) //owner specified
					query.Append(", instr_vl v");
				query.AppendFormat(" WHERE i.inst_key = s.inst_key AND i.inst_typ IN ({0})", meterKeys);
				query.Append(" AND s.state = 4");
				if (instrOwnerKey > 0) //owner specified
				{
					query.Append(" AND i.inst_key = v.inst_key");
					query.AppendFormat(" AND v.val_key=1049 AND v.ivalue={0}", instrOwnerKey); //instr_owner
				}
				query.Append(" ORDER BY i.inst_key");
				DataTable dtMeterRevState = Util.SqlToDataTable(query.ToString());
				// Loop over meters and append state info (sweden)
				foreach (DataRow dr in dtMeterInfo.Rows)
				{
					string stateText;
					int state = Int32.Parse(dr["state"].ToString());
					if (state == 1) stateText = "U"; //1=Active/Uppsatt/Aktiv
					else if (state == 2) stateText = "L"; //2=In stock/På lager
					else if (state == 5) stateText = "S"; //5=Destroyed/Skrotad
					else stateText = "Z"; //Other
					string instKey = dr["inst_key"].ToString();
					DataRow[] draMeterRevState = dtMeterRevState.Select("inst_key=" + instKey);
					if (draMeterRevState.Length > 0) stateText += "R"; //Revised
					dr["state_text"] = stateText;
				}
			}
			// Get meter attribute info
			dtMeterInfo.Columns.Add("production_year", System.Type.GetType("System.Int32"));
			dtMeterInfo.Columns.Add("instr_product", System.Type.GetType("System.String"));
			dtMeterInfo.Columns.Add("instr_tp", System.Type.GetType("System.String"));
			// Look up attributes for production year, instr_product, instr_tp
			query.Remove(0, query.Length);
			query.Append("SELECT DISTINCT i.inst_key AS inst_key, va.val_key, va.ivalue, va.cvalue");
			query.Append(" FROM view_instrument i, instr_vl va");
			if (instrOwnerKey > 0) //owner specified
				query.Append(", instr_vl vo");
			query.AppendFormat(" WHERE i.inst_typ IN ({0})", meterKeys);
			query.Append(" AND i.inst_key = va.inst_key AND va.val_key IN (704,702,701)");
			if (instrOwnerKey > 0) //owner specified
			{
				query.Append(" AND i.inst_key = vo.inst_key");
				query.AppendFormat(" AND vo.val_key=1049 AND vo.ivalue={0}", instrOwnerKey); //instr_owner
			}
			query.Append(" ORDER BY i.inst_key");
			DataTable dtMeterAttr = Util.SqlToDataTable(query.ToString());
			// Loop over meters and append attribute info, also keep track of master keys used
			foreach (DataRow dr in dtMeterInfo.Rows)
			{
				dr["production_year"] = DBNull.Value;
				dr["instr_product"] = DBNull.Value;
				dr["instr_tp"] = DBNull.Value;
				string instKey = dr["inst_key"].ToString();
				DataRow[] draMeterAttr = dtMeterAttr.Select("inst_key=" + instKey);
				foreach (DataRow drAttr in draMeterAttr)
				{
					if (drAttr["val_key"].ToString() == "704") //production year
					{
						if (drAttr["ivalue"] != DBNull.Value)
							dr["production_year"] = Int32.Parse(drAttr["ivalue"].ToString());
					}
					else if (drAttr["val_key"].ToString() == "702") //instr_product
					{
						if (drAttr["cvalue"] != DBNull.Value)
							dr["instr_product"] = drAttr["cvalue"].ToString();
					}
					else if (drAttr["val_key"].ToString() == "701") //instr_tp
					{
						if (drAttr["cvalue"] != DBNull.Value)
							dr["instr_tp"] = drAttr["cvalue"].ToString();
					}
				}
				// add master key to list if it is not already added
				string mstrKey = dr["mstr_key"].ToString();
				if (!mstrKeyList.Contains(mstrKey))
					mstrKeyList.Add(mstrKey);
			}
			// Get master info
			string mstrKeyListStr = "";
			foreach (object mstrKeyObj in mstrKeyList)
			{
				if (mstrKeyListStr.Length > 0)
					mstrKeyListStr += ",";
				mstrKeyListStr += mstrKeyObj.ToString();
			}
			query.Remove(0, query.Length);
			query.Append("SELECT m.mstr_key, m.name, v1.cvalue AS master_prod, v2.cvalue AS master_type");
			query.Append(" FROM master m, mastr_vl v1, mastr_vl v2");
			query.Append(" WHERE m.mstr_key = v1.mstr_key(+) AND m.mstr_key = v2.mstr_key(+)");
			query.AppendFormat(" AND m.mstr_key IN ({0})", mstrKeyListStr);
			query.Append(" AND v1.val_key(+) = 702"); //instr_product
			query.Append(" AND v2.val_key(+) = 701"); //instr_tp
			query.Append(" ORDER BY m.mstr_key");
			DataTable dtMasterInfo = Util.SqlToDataTable(query.ToString());
			dtMasterInfo.TableName = "masterInfo";
			// Get master attribute info
			if (spotCheckCountry == "norway")
			{
				// Append extra columns to the master table
				dtMasterInfo.Columns.Add("impulse", System.Type.GetType("System.Double"));
				dtMasterInfo.Columns.Add("current", System.Type.GetType("System.String"));
				dtMasterInfo.Columns.Add("max_current", System.Type.GetType("System.String"));
				// Look up attributes for impulse, current, max current
				query.Remove(0, query.Length);
				query.Append("SELECT mstr_key, val_key, rvalue, cvalue FROM mastr_vl");
				query.AppendFormat(" WHERE mstr_key IN ({0})", mstrKeyListStr);
				query.Append(" AND val_key IN (717,706,804) ORDER BY mstr_key");
				DataTable dtMasterAttr = Util.SqlToDataTable(query.ToString());
				// Loop over masters and append attribute info
				foreach (DataRow dr in dtMasterInfo.Rows)
				{
					dr["impulse"] = DBNull.Value;
					dr["current"] = DBNull.Value;
					dr["max_current"] = DBNull.Value;
					string mstrKey = dr["mstr_key"].ToString();
					DataRow[] draMasterAttr = dtMasterAttr.Select("mstr_key=" + mstrKey);
					foreach (DataRow drAttr in draMasterAttr)
					{
						if (drAttr["val_key"].ToString() == "717") //instr_impulse
							dr["impulse"] = Double.Parse(drAttr["rvalue"].ToString());
						else if (drAttr["val_key"].ToString() == "706") //instr_current
							dr["current"] = drAttr["cvalue"].ToString();
						else if (drAttr["val_key"].ToString() == "804") //instr_max_current
							dr["max_current"] = drAttr["cvalue"].ToString();
					}
				}
			}
			// Get net info
			DataTable dtNetAreaInfo = null;
			DataTable dtNetOwnerInfo = null;
			if (spotCheckCountry == "sweden")
			{
				// Get net area info
				bool netIdMissing = false;
				ArrayList netIdList = new ArrayList();
				foreach (DataRow dr in dtMeterInfo.Rows)
				{
					if (dr["net_id"] != DBNull.Value)
					{
						if (!netIdList.Contains(dr["net_id"].ToString()))
							netIdList.Add(dr["net_id"].ToString());
					}
					else
						netIdMissing = true;
				}
				if (netIdList.Count > 0)
				{
					string netIdStr = "";
					for (int i = 0; i < netIdList.Count; i++)
					{
						if (netIdStr.Length > 0)
							netIdStr += ",";
						netIdStr += "'" + netIdList[i].ToString() + "'";
					}
					query.Remove(0, query.Length);
					query.Append("SELECT neta_key, svk_id, name FROM netarea");
					query.AppendFormat(" WHERE svk_id in ({0})", netIdStr);
					query.Append(" ORDER BY svk_id");
					dtNetAreaInfo = Util.SqlToDataTable(query.ToString());
					dtNetAreaInfo.TableName = "netAreaInfo";
				}
				// Get instrument owner info to be used as default when net area is missing
				if (netIdMissing)
				{
					if (instrOwnerKey <= 0) //owner not specified
					{
						ArrayList ownKeyList = new ArrayList();
						foreach (DataRow dr in dtMeterInfo.Rows)
						{
							if (dr["owner_key"] != DBNull.Value)
							{
								if (!ownKeyList.Contains(dr["owner_key"].ToString()))
									ownKeyList.Add(dr["owner_key"].ToString());
							}
						}
						//add ICC_SENDER (used as default value)
						string iccSenderKey = IccConfiguration.ImportExport.IccSender.ToString();
						if (!ownKeyList.Contains(iccSenderKey))
							ownKeyList.Add(iccSenderKey);
						string ownKeyStr = "";
						for (int i = 0; i < ownKeyList.Count; i++)
						{
							if (ownKeyStr.Length > 0)
								ownKeyStr += ",";
							ownKeyStr += ownKeyList[i].ToString();
						}
						query.Remove(0, query.Length);
						query.Append("SELECT opun_key, shorname, name FROM oprtunit");
						query.AppendFormat(" WHERE opun_key IN ({0})", ownKeyStr);
						query.Append(" ORDER BY shorname");
						dtNetOwnerInfo = Util.SqlToDataTable(query.ToString());
						dtNetOwnerInfo.TableName = "netOwnerInfo";
						foreach (DataRow dr in dtMeterInfo.Rows)
						{
							if (dr["net_id"] == DBNull.Value)
							{
								string ownKey = dr["owner_key"].ToString();
								if (ownKey == "" || ownKey == "0")
									ownKey = iccSenderKey;
								DataRow[] draNetOwnerInfo = dtNetOwnerInfo.Select("opun_key=" + ownKey);
								if (draNetOwnerInfo.Length > 0)
									dr["net_id"] = draNetOwnerInfo[0]["shorname"].ToString();
							}
						}
					}
					else //owner specified
					{
						query.Remove(0, query.Length);
						query.Append("SELECT opun_key, shorname, name FROM oprtunit");
						query.AppendFormat(" WHERE opun_key = {0}", instrOwnerKey);
						query.Append(" ORDER BY shorname");
						dtNetOwnerInfo = Util.SqlToDataTable(query.ToString());
						dtNetOwnerInfo.TableName = "netOwnerInfo";
						if (dtNetOwnerInfo.Rows.Count > 0)
						{
							foreach (DataRow dr in dtMeterInfo.Rows)
							{
								if (dr["net_id"] == DBNull.Value)
									dr["net_id"] = dtNetOwnerInfo.Rows[0]["shorname"].ToString();
							}
						}
					}
					dtMeterInfo.AcceptChanges();
				}
			}

			ds.Tables.Add(dtMeterInfo.Copy()); //...copy...!!!
			ds.Tables.Add(dtMasterInfo.Copy());
			if (dtNetAreaInfo != null)
				ds.Tables.Add(dtNetAreaInfo.Copy());
			if (dtNetOwnerInfo != null)
				ds.Tables.Add(dtNetOwnerInfo.Copy());
			return ds;
		}

		public DataSet GetMeterErrorInfo(long keyTabKey, string fromTime)
		{
			StringBuilder query = new StringBuilder();
			query.Append("SELECT DISTINCT i.inst_key AS inst_key, i.instr_no, i.fabr_no");
			query.Append(", m.mstr_key AS mstr_key, m.name AS master_name");
			query.Append(", e.error_date, e.error_descript, e.reason_key, e.cat_key");
			query.Append(", e.high_consumption, e.contact_info");
			query.Append(" FROM instrument i, master m, instr_nt e, keytab k");
			query.Append(" WHERE i.mstr_key = m.mstr_key AND i.inst_key = e.inst_key");
			query.AppendFormat(" AND i.inst_key = k.ivalue1 AND k.rcount={0}", keyTabKey);
			if (fromTime.Length > 0)
				query.AppendFormat(" AND e.error_date >= '{0}'", fromTime);
			query.Append(" ORDER BY i.instr_no");
			DataTable dtMeterInfo = Util.SqlToDataTable(query.ToString());
			dtMeterInfo.TableName = "meterInfo";

			DataSet ds = new DataSet();
			if (dtMeterInfo.Rows.Count == 0)
				return ds;

			ArrayList mstrKeyList = new ArrayList();

			// Get meter attribute info
			dtMeterInfo.Columns.Add("production_year", System.Type.GetType("System.Int32"));
			dtMeterInfo.Columns.Add("instr_product", System.Type.GetType("System.String"));
			dtMeterInfo.Columns.Add("instr_tp", System.Type.GetType("System.String"));
			// Look up attributes for production year, instr_product, instr_tp
			query.Remove(0, query.Length);
			query.Append("SELECT v.inst_key, v.val_key, v.ivalue, v.cvalue FROM instr_vl v, keytab k");
			query.AppendFormat(" WHERE v.inst_key = k.ivalue1 AND k.rcount={0}", keyTabKey);
			query.Append(" AND v.val_key IN (704,702,701) ORDER BY v.inst_key");
			DataTable dtMeterAttr = Util.SqlToDataTable(query.ToString());
			// Loop over meters and append attribute info, also keep track of master keys used
			foreach (DataRow dr in dtMeterInfo.Rows)
			{
				dr["production_year"] = DBNull.Value;
				dr["instr_product"] = DBNull.Value;
				dr["instr_tp"] = DBNull.Value;
				string instKey = dr["inst_key"].ToString();
				DataRow[] draMeterAttr = dtMeterAttr.Select("inst_key=" + instKey);
				foreach (DataRow drAttr in draMeterAttr)
				{
					if (drAttr["val_key"].ToString() == "704") //production year
					{
						if (drAttr["ivalue"] != DBNull.Value)
							dr["production_year"] = Int32.Parse(drAttr["ivalue"].ToString());
					}
					else if (drAttr["val_key"].ToString() == "702") //instr_product
					{
						if (drAttr["cvalue"] != DBNull.Value)
							dr["instr_product"] = drAttr["cvalue"].ToString();
					}
					else if (drAttr["val_key"].ToString() == "701") //instr_tp
					{
						if (drAttr["cvalue"] != DBNull.Value)
							dr["instr_tp"] = drAttr["cvalue"].ToString();
					}
				}
				// add master key to list if it is not already added
				string mstrKey = dr["mstr_key"].ToString();
				if (!mstrKeyList.Contains(mstrKey))
					mstrKeyList.Add(mstrKey);
			}
			// Get master info
			string mstrKeyListStr = "";
			foreach (object mstrKeyObj in mstrKeyList)
			{
				if (mstrKeyListStr.Length > 0)
					mstrKeyListStr += ",";
				mstrKeyListStr += mstrKeyObj.ToString();
			}
			query.Remove(0, query.Length);
			query.Append("SELECT m.mstr_key, m.name, v1.cvalue AS master_prod, v2.cvalue AS master_type");
			query.Append(" FROM master m, mastr_vl v1, mastr_vl v2");
			query.Append(" WHERE m.mstr_key = v1.mstr_key(+) AND m.mstr_key = v2.mstr_key(+)");
			query.AppendFormat(" AND m.mstr_key IN ({0})", mstrKeyListStr);
			query.Append(" AND v1.val_key(+) = 702"); //instr_product
			query.Append(" AND v2.val_key(+) = 701"); //instr_tp
			query.Append(" ORDER BY m.mstr_key");
			DataTable dtMasterInfo = Util.SqlToDataTable(query.ToString());
			dtMasterInfo.TableName = "masterInfo";

			ds.Tables.Add(dtMeterInfo.Copy()); //...copy...!!!
			ds.Tables.Add(dtMasterInfo.Copy());
			return ds;
		}

		public string GetComponentOwnerKeysAsString()
		{
			string ownerKeys = IccConfiguration.Component.IccInstrOwner;
			if (ownerKeys.Length == 0)
				ownerKeys = DbConfiguration.Component.IccInstrOwner;
			if (ownerKeys.Length == 0)
				ownerKeys = IccConfiguration.ImportExport.IccSender.ToString();
			if (ownerKeys.Length == 0)
				throw new ApplicationException("Unable to find environment info for component owner list.");
			return ownerKeys;
		}

		public DataTable GetComponentOwners(string ownerKeys)
		{
			string query = string.Format("SELECT OPUN_KEY, NAME, SHORNAME, OPRSTATU FROM OPRTUNIT WHERE OPUN_KEY IN ({0})", ownerKeys);
			query += " ORDER BY NAME";

			DataTable componentOwners = Util.SqlToDataTable(query);
			componentOwners.TableName = "componentOwners";
			return componentOwners;
		}
	}
}
