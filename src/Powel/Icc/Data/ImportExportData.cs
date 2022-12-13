using System;
using System.Collections.Generic;
using System.Data;
using System.Text;
using Oracle.ManagedDataAccess.Client;
using Powel.Icc.Common;
using Powel.Icc.Services.Time;
using Powel.Icc.Data.Entities;
using Powel.Icc.Data.Entities.Metering;
//using Powel.Icc.TimeSeries;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for ImportExportData.
	/// </summary>
	public class ImportExportData
	{
		private const int OBJECT_TYPE_IMPORT_FREQUENCY_OBJECT_CHANGED = 900;
		public const string ImportDefinitionIgnore = "##PDCONST##ImportDefinitionIgnore##"; //must be special, to avoid accidental identicalness to a time series name

		private ImportExportData()
		{
		}

		/// <summary>
		/// Will return valid import definitions for the given external reference and protocol.
		/// For every single point in time there shall be only one valid import definition pr external reference and protocol.
		/// Same external reference may be valid for different protocols.
		/// </summary>
		public static DataTable FetchImportDefinitions(string externalReference, TsProtocol protocol, IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand();
			cmd.CommandText = "select ie.tims_key, ts.filename, ts.tscode, ie.valftime, ie.valttime, ie.factor, ie.offset, ie.rval1, ie.rval2 from impexpts ie left join timeser ts on ie.tims_key = ts.tims_key join tstradef tr on ie.tstr_key = tr.tstr_key where ie.direct = 0 and ie.globeref = :extRef and tr.prot_key =:protocol";
			cmd.Parameters.Add(":extRef", externalReference);
			cmd.Parameters.Add(":protocol", (int)protocol);
			
			return Util.CommandToDataTable(cmd, connection);
		}

		/// <summary>
		/// Will return valid import definitions for the given external reference and protocol in a given time period.
		/// For every single point in time there shall be only one valid import definition pr external reference and protocol.
		/// Same external reference may be valid for different protocols.
		/// </summary>
		public static DataTable FetchImportDefinitions(string externalReference, UtcTime from, UtcTime to, TsProtocol protocol)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			
			OracleCommand cmd = new OracleCommand();
			cmd.CommandText = "select ie.tims_key, ts.tscode, ie.valftime, ie.valttime, ie,factor, ie.offset, ie.rval1, ie.rval2 from impexpts ie left join timeser ts on ie.tims_key = ts.tims_key join tstradef tr on ie.tstr_key = tr.tstr_key where ie.direct = 0 and ie.globeref = :extRef and tr.prot_key =:protocol and ie.valftime < :to and ie.valttime > :from";
			cmd.Parameters.Add(":extRef", externalReference);
			cmd.Parameters.Add(":protocol", (int)protocol);
			cmd.Parameters.Add(":from", cal.ToDatabaseString(from));
			cmd.Parameters.Add(":to", cal.ToDatabaseString(to));
			
			return Util.CommandToDataTable(cmd);
		}

        [Obsolete("Not tested at all", true)]
		public static bool CreateImportDefinition(int timsKey, int opunKey, string externalReference, float offset, float factor, int readOff, UtcTime validFrom, UtcTime validTo, int tstrKey, int digits, int decimals, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			
			OracleCommand cmd = new OracleCommand("ICC_METERING.ImportDefinitionInsert");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IopunKey", OracleDbType.Int32,opunKey, ParameterDirection.Input);
			cmd.Parameters.Add("ItimsKey", OracleDbType.Int32,timsKey, ParameterDirection.Input);
			cmd.Parameters.Add("Igloberef", OracleDbType.Varchar2, externalReference, ParameterDirection.Input);
			cmd.Parameters.Add("Ioffset", OracleDbType.Single, offset, ParameterDirection.Input);
			cmd.Parameters.Add("Ifactor", OracleDbType.Single, factor, ParameterDirection.Input);
			cmd.Parameters.Add("IreadOff", OracleDbType.Int32, readOff, ParameterDirection.Input);
			cmd.Parameters.Add("Ivalftime", OracleDbType.Varchar2, cal.ToDatabaseString(validFrom), ParameterDirection.Input);
			cmd.Parameters.Add("Ivalttime", OracleDbType.Varchar2, cal.ToDatabaseString(validTo), ParameterDirection.Input);
			cmd.Parameters.Add("ItstrKey", OracleDbType.Int32, tstrKey, ParameterDirection.Input);
			cmd.Parameters.Add("InDigits", OracleDbType.Int32, digits, ParameterDirection.Input);
			cmd.Parameters.Add("InDecimals", OracleDbType.Int32, decimals, ParameterDirection.Input);
			
			Util.ExecuteCommand(cmd, connection);

			int n = (int) cmd.Parameters["nError"].Value;
			if (n >= 0) return true;
			else return false;
		}
		
		[Obsolete("Not tested at all", true)]
		public static bool CreateExportDefinition(int timsKey, int opunKey, string externalReference, float offset, float factor, int readOff, UtcTime validFrom, UtcTime validTo, int tstrKey, int digits, int decimals, int exmtKey, int sender, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			
			OracleCommand cmd = new OracleCommand("ICC_METERING.ImportDefinitionInsert");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IopunKey", OracleDbType.Int32,opunKey, ParameterDirection.Input);
			cmd.Parameters.Add("ItimsKey", OracleDbType.Int32,timsKey, ParameterDirection.Input);
			cmd.Parameters.Add("Igloberef", OracleDbType.Varchar2, externalReference, ParameterDirection.Input);
			cmd.Parameters.Add("Ioffset", OracleDbType.Single, offset, ParameterDirection.Input);
			cmd.Parameters.Add("Ifactor", OracleDbType.Single, factor, ParameterDirection.Input);
			cmd.Parameters.Add("IreadOff", OracleDbType.Int32, readOff, ParameterDirection.Input);
			cmd.Parameters.Add("Ivalftime", OracleDbType.Varchar2, cal.ToDatabaseString(validFrom), ParameterDirection.Input);
			cmd.Parameters.Add("Ivalttime", OracleDbType.Varchar2, cal.ToDatabaseString(validTo), ParameterDirection.Input);
			cmd.Parameters.Add("ItstrKey", OracleDbType.Int32, tstrKey, ParameterDirection.Input);
			cmd.Parameters.Add("InDigits", OracleDbType.Int32, digits, ParameterDirection.Input);
			cmd.Parameters.Add("InDecimals", OracleDbType.Int32, decimals, ParameterDirection.Input);
			cmd.Parameters.Add("IexmtKey", OracleDbType.Int32, exmtKey, ParameterDirection.Input);
			cmd.Parameters.Add("Isender", OracleDbType.Int32, sender, ParameterDirection.Input);
			
			Util.ExecuteCommand(cmd, connection);

			int n = (int) cmd.Parameters["nError"].Value;
			if (n >= 0) return true;
			else return false;
		}

	    /// <summary>
	    /// Fetches the import definition for a time series at a given time
	    /// </summary>
	    /// <param name="timsKey">tims_key of the given time series</param>
	    /// <param name="validAt">point in time when the import definition must be valid</param>
	    /// <param name="connection"></param>
	    /// <returns>An ImportDefinition</returns>
	    public static ImportDefinition FetchImportDefinition(int timsKey, UtcTime validAt, IDbConnection connection)
	    {
	        return FetchImportDefinition(timsKey, validAt, null, connection);
	    }

	    /// <summary>
		/// Fetches the import definition for a time series at a given time
		/// </summary>
		/// <param name="timsKey">tims_key of the given time series</param>
		/// <param name="validAt">point in time when the import definition must be valid</param>
		/// <param name="connection"></param>
		/// <returns>An ImportDefinition</returns>
		public static ImportDefinition FetchImportDefinition(int timsKey, UtcTime validAt, TsProtocol? protocol, IDbConnection connection)
		{
			ImportDefinition impDef = null;
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			DataTable dtImpDef = FetchImportDefinitionDT(timsKey, validAt, protocol, connection);
			if (dtImpDef.Rows.Count > 1)
			{
				throw new ApplicationException("There are more than one import definition for tims_key " + timsKey + " at time " + validAt); //TODO Type of exception
			}
			else if (dtImpDef.Rows.Count == 1)
			{
				DataRow dr = dtImpDef.Rows[0];
				int digits = Int32.MinValue;
				int decimals = Int32.MinValue;
				string internalName = "";

				if ((int)dr["tims_key"] == -1) //ignore ts
					internalName = ImportDefinitionIgnore;
				else
					internalName = (string)dr["tscode"];

				int opunKey = Util.GetInt32(dr, "opun_key");
				int seqno = Util.GetInt32(dr, "seqno");
				string globeref = Util.GetString(dr, "globeref");
				string valftime = Util.GetString(dr, "valftime");
				string valttime = Util.GetString(dr, "valttime");
				double offset = Util.GetFloat(dr, "offset");
				double scale = Util.GetFloat(dr, "factor");
				ReadOff readOff = (ReadOff)Util.GetInt16(dr, "vstaint");
				Agreement.Frequency impFrequency = Agreement.Frequency.NEVER;
				if (dr["imp_frequency"] != DBNull.Value)
					impFrequency = (Agreement.Frequency)Util.GetInt16(dr, "imp_frequency");
				int tstrKey = Util.GetInt32(dr, "tstr_key");
				if (dr["rval1"] != DBNull.Value)
					digits = Convert.ToInt32(Util.GetDecimal(dr, "rval1")); ;
				if (dr["rval2"] != DBNull.Value)
					decimals = Convert.ToInt32(Util.GetDecimal(dr, "rval2"));
			    string opunName = Util.GetString(dr, "shorname");
				//				TsProtocol protocol = (TsProtocol) Util.GetInt16(dr,"PROT_KEY");

				impDef = new ImportDefinition(opunKey, opunName, timsKey, seqno, internalName, globeref,
					new TimePeriod(cal.ToUtcTime(valftime), cal.ToUtcTime(valttime)),
					tstrKey, offset, scale, readOff, digits, decimals, impFrequency, protocol);
			}
            
			return impDef;
		}

	    /// <summary>
        /// Fetches the import definition for a time series at a given time
        /// </summary>
        /// <param name="timsKey">tims_key of the given time series</param>
        /// <param name="validAt">point in time when the import definition must be valid</param>
        /// <param name="protocol"></param>
        /// <param name="connection"></param>
        /// <returns>A datatable describing the import definition fields (IMPEXPTS)</returns>
        public static DataTable FetchImportDefinitionDT(int timsKey, UtcTime validAt, TsProtocol? protocol, IDbConnection connection)
        {
            DataTable importDefinitions = FetchImportDefinitionsDT(timsKey, validAt, validAt, protocol, connection);
            return importDefinitions; //should be only one
        }

		public static DataTable FetchImportDefinitionsDT(int timsKey, UtcTime from, UtcTime to, TsProtocol? protocol, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			
            var commandText = new StringBuilder();
            commandText.Append("select ie.opun_key, ie.tims_key, ie.seqno, ie.globeref, ie.valftime, ie.valttime, ");
            commandText.Append("ie.factor, ie.offset, ie.rval1, ie.rval2, ie.vstaint, ie.tstr_key, ie.imp_frequency, ");
            commandText.Append("ts.tscode, td.prot_key, ou.shorname ");
            commandText.Append("from impexpts ie ");
            commandText.Append("left join timeser ts on ie.tims_key = ts.tims_key ");
            commandText.Append("left join tstradef td on ie.tstr_key = td.tstr_key ");
            commandText.Append("left join oprtunit ou on ie.opun_key = ou.opun_key ");
            commandText.Append("where ie.direct = 0 ");
            commandText.Append("and ie.tims_key = :timsKey ");
            if (protocol.HasValue)
            {
                commandText.Append("and td.prot_key = :prot ");
            }
            commandText.Append("and ie.valftime <= :fromTime ");
            commandText.Append("and ie.valttime > :toTime");

            var cmd = new OracleCommand();
            cmd.CommandText = commandText.ToString();

            cmd.Parameters.Add(":timsKey", OracleDbType.Int32, (int) timsKey, ParameterDirection.Input);
            if (protocol.HasValue)
            {
                cmd.Parameters.Add(":prot", OracleDbType.Int32, (int) protocol, ParameterDirection.Input);
            }
		    cmd.Parameters.Add(":fromTime", OracleDbType.Varchar2, cal.ToDatabaseString(from), ParameterDirection.Input);
			cmd.Parameters.Add(":toTime", OracleDbType.Varchar2, cal.ToDatabaseString(to), ParameterDirection.Input);

			return Util.CommandToDataTable(cmd, connection);
		}	

		/// <summary>
		/// Fetches the import definitions for a time series at a given time interval
		/// </summary>
		/// <param name="timsKey">tims_key of the given time series</param>
		/// <param name="from">time interval start</param>
		/// <param name="from">time interval end</param>
		/// <param name="connection"></param>
		/// <returns>ImportDefinition[]</returns>
		public static ImportDefinition[] FetchImportDefinitions(int timsKey, UtcTime from, UtcTime to, TsProtocol protocol, IDbConnection connection)
		{
			ImportDefinition[] impDefs = null;
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			DataTable dtImpDefs = FetchImportDefinitionsDT(timsKey, from, to, protocol, connection);
			int i = 0;
			if( dtImpDefs.Rows.Count > 0)
			{
				impDefs = new ImportDefinition[dtImpDefs.Rows.Count];
							
				DataRow dr = dtImpDefs.Rows[i];
				int digits = Int32.MinValue;
				int decimals = Int32.MinValue;
				string internalName = "";
				
				if( (int)dr["tims_key"] == -1) //ignore ts
					internalName = ImportDefinitionIgnore;
				else
					internalName = (string)dr["tscode"];

				int opunKey = Util.GetInt32(dr, "opun_key");
				int seqno = Util.GetInt32(dr, "seqno");
				string globeref = Util.GetString(dr, "globeref");
				string valftime = Util.GetString(dr, "valftime");
				string valttime = Util.GetString(dr, "valttime");
				double offset = Util.GetFloat(dr, "offset");
				double scale = Util.GetFloat(dr, "factor");
				ReadOff readOff = (ReadOff) Util.GetInt16(dr, "vstaint");
				Agreement.Frequency impFrequency = (Agreement.Frequency) Util.GetInt16(dr, "imp_frequency");
				int tstrKey = Util.GetInt32(dr, "tstr_key");
				if (dr["rval1"] != DBNull.Value)
					digits = Convert.ToInt32(Util.GetDecimal(dr, "rval1"));;
				if (dr["rval2"] != DBNull.Value)
					decimals = Convert.ToInt32(Util.GetDecimal(dr, "rval2"));
			    string opunName = Util.GetString(dr, "shorname");
				//TsProtocol protocol = (TsProtocol) Util.GetInt16(dr, "PROT_KEY");
								
				impDefs[i] = new ImportDefinition(opunKey, opunName, timsKey, seqno, internalName, globeref,
					new TimePeriod(cal.ToUtcTime(valftime), cal.ToUtcTime(valttime)),
					tstrKey, offset, scale, readOff, digits, decimals, impFrequency,protocol);

			}
		    
			return impDefs;
		}

        private static DataTable FetchImportDefinitionsDT(string bankacc, DateTime validTime, IDbConnection connection)
        {
            RegionalCalendar cal = RegionalCalendar.GetCalendar(RegionalCalendar.dbName);

            var cmd = new OracleCommand();
            cmd.CommandText = 
@"select ie.opun_key, ie.tims_key, ie.seqno, ie.globeref, ie.valftime, ie.valttime, ie.factor, ie.offset, ie.rval1, ie.rval2, ie.vstaint, ie.tstr_key, ie.imp_frequency 
from impexpts ie 
left join oprtunit ou on ie.opun_key = ou.opun_key 
where ie.direct = 0 
and ou.bankacc = :bankacc 
and ie.valftime <= :validTime 
and ie.valttime > :validTime";

            cmd.Parameters.Add(":bankacc", OracleDbType.Varchar2, bankacc, ParameterDirection.Input);
            cmd.Parameters.Add(":validTime", OracleDbType.Varchar2, cal.ToDatabaseString(cal.ToUtcTime(validTime)), ParameterDirection.Input);

            return Util.CommandToDataTable(cmd, connection);
        }

        /// <summary>
        /// Fetches the import definitions for a time series at a given time interval
        /// </summary>
        /// <param name="bankacc">EDIEL address for party owning the import definitions</param>
        /// <param name="validTime">Read import definitions valid at this time.</param>
        /// <param name="protocol"></param>
        /// <param name="connection"></param>
        /// <returns>ImportDefinition[]</returns>
        public static ImportDefinition[] FetchImportDefinitions(string bankacc, DateTime validTime)
        {
            DataTable dtImpDefs = FetchImportDefinitionsDT(bankacc, validTime, Util.OpenConnection());
            ImportDefinition[] impDefs = new ImportDefinition[dtImpDefs.Rows.Count];
            int i = 0;
            foreach (DataRow row in dtImpDefs.Rows)
            {
                impDefs[i++] = new ImportDefinition(Util.GetString(row,"globeref"));
            }

            return impDefs;
        }

        public static DataTable FetchExportDefinitionsDT(List<int> exmtLst, UtcTime from, UtcTime to, IDbConnection connection)
        {
            RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

            string exmtKeys = "";
            foreach (int exmtKey in exmtLst)
            {
                if (exmtKeys.Length > 0)
                    exmtKeys += ',';

                exmtKeys += exmtKey.ToString();
            }

            var commandText = new StringBuilder();
            commandText.Append("select ie.opun_key, ie.tims_key, ie.seqno, ie.globeref, ie.valftime, ie.valttime, ");
            commandText.Append("ie.factor, ie.offset, ie.rval1, ie.rval2, ie.vstaint, ie.tstr_key,  ");
            commandText.Append("ts.tscode, td.prot_key, ou.shorname, et.valtran ");
            commandText.Append("from impexpts ie ");
            commandText.Append("left join timeser ts on ie.tims_key = ts.tims_key ");
            commandText.Append("left join tstradef td on ie.tstr_key = td.tstr_key ");
            commandText.Append("left join oprtunit ou on ie.opun_key = ou.opun_key ");
            commandText.Append("left join exchtask et on ie.exmt_key in (" + exmtKeys + ") ");
            commandText.Append("where ie.direct = 1 ");
            commandText.Append("and ie.valftime <= :fromTime ");
            commandText.Append("and ie.valttime > :toTime");

            var cmd = new OracleCommand();
            cmd.CommandText = commandText.ToString();

            cmd.Parameters.Add(":fromTime", OracleDbType.Varchar2, cal.ToDatabaseString(from), ParameterDirection.Input);
            cmd.Parameters.Add(":toTime", OracleDbType.Varchar2, cal.ToDatabaseString(to), ParameterDirection.Input);

            return Util.CommandToDataTable(cmd, connection);
        }

        public static DataTable FetchExportDefinitionsDT(int exmtKey, int sequenceKey, IDbConnection connection)
        {
            var commandText = new StringBuilder();
            commandText.Append("select ie.opun_key, ie.mesh_guid, et.fromtime, et.totime ");
            commandText.Append("from impexpts ie ");
            commandText.Append("left join exchtask et on ie.exmt_key = et.exmt_key ");
            commandText.Append("where ie.direct = 1 and ie.exmt_key = " + exmtKey + " and et.seqno = " + sequenceKey + " ");
            commandText.Append("and ie.valftime < et.totime ");
            commandText.Append("and ie.valttime > et.fromtime order by ie.opun_key");

            var cmd = new OracleCommand();
            cmd.CommandText = commandText.ToString();
            return Util.CommandToDataTable(cmd, connection);
        }

        /// <summary>
        /// Fetches the export definitions for a time series at a given time interval
        /// </summary>
        /// <param name="exportDefinitionGroups">list with exportdefinitiongroups</param>
        /// <param name="from">time interval start</param>
        /// <param name="from">time interval end</param>
        /// <param name="connection"></param>
        /// <returns>ExportDefinition[]</returns>
        public static ExportDefinition[] FetchExportDefinitions(List<int> exmtLst,UtcTime from, UtcTime to, IDbConnection connection)
        {
            ExportDefinition[] expDefs = null;
            RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

            DataTable dtExpDefs = FetchExportDefinitionsDT(exmtLst, from, to, connection);
            int i = 0;
            if (dtExpDefs.Rows.Count > 0)
            {
                expDefs = new ExportDefinition[dtExpDefs.Rows.Count];

                DataRow dr = dtExpDefs.Rows[i];
                int digits = Int32.MinValue;
                int decimals = Int32.MinValue;
                string internalName = "";

                if ((int)dr["tims_key"] == -1) //ignore ts
                    internalName = ImportDefinitionIgnore;
                else
                    internalName = (string)dr["tscode"];

                int opunKey = Util.GetInt32(dr, "opun_key");
                int seqno = Util.GetInt32(dr, "seqno");
                string globeref = Util.GetString(dr, "globeref");
                string valftime = Util.GetString(dr, "valftime");
                string valttime = Util.GetString(dr, "valttime");
                double offset = Util.GetFloat(dr, "offset");
                double scale = Util.GetFloat(dr, "factor");
                ReadOff readOff = (ReadOff)Util.GetInt16(dr, "vstaint");
                int valtran = Util.GetInt16(dr, "valtran");
                int tstrKey = Util.GetInt32(dr, "tstr_key");
                int timsKey = Util.GetInt32(dr, "tims_key");
                if (dr["rval1"] != DBNull.Value)
                    digits = Convert.ToInt32(Util.GetDecimal(dr, "rval1")); ;
                if (dr["rval2"] != DBNull.Value)
                    decimals = Convert.ToInt32(Util.GetDecimal(dr, "rval2"));
                string opunName = Util.GetString(dr, "shorname");
                TsProtocol protocol = (TsProtocol) Util.GetInt16(dr, "PROT_KEY");


                expDefs[i] = new ExportDefinition(opunKey, opunName, timsKey, seqno, internalName, globeref,
                    new TimePeriod(cal.ToUtcTime(valftime), cal.ToUtcTime(valttime)),
                    tstrKey, offset, scale, readOff, digits, decimals, protocol, valtran);

            }

            return expDefs;
        }


		///<summary>
		///Deletes rows in OBJECT_CHANGED that indicates changed import frequencies.
		///</summary>
		public static void DeleteChangedImportDefinitions(ImportDefinition[] impDefs, UtcTime beforeTime, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			if (impDefs.Length == 0)
				return;

			StringBuilder timsKeys = new StringBuilder();
			bool first = true;
			foreach(ImportDefinition impDef in impDefs)
			{
				if(!first) timsKeys.Append(",");
				timsKeys.Append(impDef.TimsKey);
				if(first) first = false;
			}
			
			OracleCommand cmd = new OracleCommand();
			cmd.CommandText = "delete from object_changed o where object_type = :objectType and o.fromtime <= :fromTime and o.object_key in (" + timsKeys + ")"; 
			cmd.Parameters.Add(":objectType", OBJECT_TYPE_IMPORT_FREQUENCY_OBJECT_CHANGED);
			cmd.Parameters.Add(":fromTime", cal.ToDatabaseString(beforeTime));
			
			Util.ExecuteCommand(cmd, connection);
		}

		///<summary>
		///Method that checks OBJECT_CHANGED for new or updated import frequencies of import definitions
		///</summary> 
		public static DataTable GetChangedImportDefinitions(UtcTime before, IDbConnection connection)
		{
			return GetChangedImportDefinitions(before, false, connection);
		}

		public static DataTable GetChangedImportDefinitions(UtcTime before, bool grouped, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			
			OracleCommand cmd = new OracleCommand();
			if (grouped)
				cmd.CommandText = "select max(o.object_key) as tims_key, mt.meas_key, o.fromtime from object_changed o, meas_ts mt where object_type = :objectType and o.object_key = mt.tims_key and fromtime <= :fromTime group by (meas_key, fromtime)";
			else
				cmd.CommandText = "select distinct o.object_key as tims_key, o.fromtime from object_changed o where object_type = :objectType and fromtime <= :fromTime";
			
			cmd.Parameters.Add(":objectType", OBJECT_TYPE_IMPORT_FREQUENCY_OBJECT_CHANGED);
			cmd.Parameters.Add(":fromTime", cal.ToDatabaseString(before));
			
			return Util.CommandToDataTable(cmd, connection);
		}

		public static DataTable GetTransferFormats(TransferDirection direction, IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand();
			cmd.CommandText = "select tstr_key, name, prot_key, descript, direct from tstradef ";
			if (direction >= 0)
				cmd.CommandText += "where direct =" + (int) direction;

			return Util.CommandToDataTable(cmd, connection);
		}
	}

	public enum TransferDirection
	{
		BOTH = -1,
		IMPORT = 0,
		EXPORT = 1
	}
}
