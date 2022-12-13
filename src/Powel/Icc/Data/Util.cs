using Powel.Icc.Common;
using System.Data;
using System.Globalization;
using System.Linq;
using System.Text;
using System;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using System.Diagnostics;
using System.Collections.Generic;
using log4net;
#if !TSS_Availability
using System.IO;
using System.Threading;
using System.Reflection;
using Powel.Icc.Interop;
#endif
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for Util.
	/// </summary>
	public static class Util
	{
#if TSS_Availability
	    private static readonly ConsoleLogger Log = new ConsoleLogger(typeof(Util));
#else
        private static readonly ILog Log = LogManager.GetLogger(typeof(Util));
        //private static RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar; Don't want this initialisation at startup.
#endif
        public const string LogSeparator = "\n_______________________________________________________________________";

        public static int DefaultParameterSize => 4096;

        #region Get connection strings

        public static string OracleConnectString => 
            $"User Id={DbUser};Password=\"{DbPassword}\";Data Source={TnsName};Pooling=true;";
	    public static string DbUser { get; set; }
	    public static string DbPassword { get; set; }
	    public static string TnsName { get; set; }
	    public static int ProgKey { get; set; }
	    public static string Language { get; set; }
	    public static string Territory { get; set; }

        public static string GetConfiguredConnectionString(bool includePassword)
		{
			return GetConfiguredConnectionString(
#if TSS_Availability
                OracleConnectString
#else
                IccConfiguration.Data.OracleConnectionString
#endif
                , includePassword);
		}

		public static string GetConfiguredReplicationConnectionString(bool includePassword)
		{
			return GetConfiguredConnectionString(
#if TSS_Availability
                OracleConnectString
#else
				IccConfiguration.Replication.OracleConnectionString
#endif
                , includePassword);
		}

		static string GetConfiguredConnectionString(string connectionString, bool includePassword)
		{
			if (includePassword)
				return connectionString;

            using (OracleConnection connection = new OracleConnection(connectionString))
			{
				connection.OpenWithNewPassword("");
				return connection.ConnectionString;
			}
		}
#endregion

#region Open connection/transaction
		public static IDbConnection OpenConnection()
		{
			var connection = new OracleConnection();
			PrepareConnection(connection);

			return connection;
		}

		public static IDbConnection OpenConnection(string connectionString)
		{
			var connection = new OracleConnection(connectionString);
			PrepareConnection(connection);

			return connection;
		}

        public static IDbConnection OpenConnectionByRole(int role)
        {
            IDbConnection connection = new OracleConnection();
            PrepareConnectionByRole(connection, role);                

            return connection;
        }

		public static IDbTransaction OpenTransaction(IDbConnection connection)
		{
			return OpenTransaction(connection, false);
		}

		public static IDbTransaction OpenTransaction(IDbConnection connection, bool serializable)
		{
			if (serializable)
				return connection.BeginTransaction(IsolationLevel.Serializable);

            return connection.BeginTransaction();
		}
#endregion

#region Prepare connection/command
        public static void PrepareConnection(IDbConnection connection)
		{
			PrepareConnection((OracleConnection)connection, -1);
		}

        public static void PrepareConnection(OracleConnection connection)
        {
            PrepareConnection(connection, -1);
        }

        public static void PrepareConnectionByRole(IDbConnection connection, int role)
        {
            PrepareConnection((OracleConnection)connection, role);
        }

		public static void PrepareConnection(OracleConnection connection, int role)
		{
            //var stopWatch = new Stopwatch();
            //stopWatch.Start();
            //int getConnStringElapsed = 0;
            if (string.IsNullOrEmpty(connection.ConnectionString))
			{
                Profiler.Log(10030, "Getting connect string");
                connection.ConnectionString =
#if TSS_Availability
                    OracleConnectString;
#else
			        role < 0
			            ? (!string.IsNullOrEmpty(DbUser) && !string.IsNullOrEmpty(DbPassword) && !string.IsNullOrEmpty(TnsName)
                            ? OracleConnectString 
                            : IccConfiguration.Data.OracleConnectionString)
			            : IccConfiguration.Data.GetOracleConnectionStringByRole(role);
#endif
                //stopWatch.Stop();
                //getConnStringElapsed = (int)Math.Round(stopWatch.Elapsed.TotalMilliseconds);
                //stopWatch.Reset();
                //stopWatch.Start();
            }

            try
			{
                Profiler.Log(10040, "connection.Open");
                connection.Open();
			}
			catch (OracleException ex)
			{
                var passwordIndex = connection.ConnectionString.IndexOf("assword");
			    var connString = passwordIndex > 0
			        ? $"{connection.ConnectionString.Substring(0, passwordIndex)}***"
			        : connection.ConnectionString;
			    if (passwordIndex > 0)
			    {
			        var passwordEndIndex = connection.ConnectionString.IndexOf(";", passwordIndex);
			        if (passwordEndIndex > 0)
			        {
			            connString += connection.ConnectionString.Substring(passwordEndIndex);
			        }
			    }
				Log.Error($"RoleId {role} with Connection string {connString} : {ex}");

#if TSS_Availability
                //connString = connection.ConnectionString; //SIV. Uncomment for debugging
#else
                IccConfiguration.Data.ClearConnectionStringCache();

                if (ex.Message.IndexOf("ORA-12500") != -1)
					throw new DatabaseNotReadyException("The database is not running.", connString);
				if (ex.Message.IndexOf("ORA-01033") != -1)
					throw new DatabaseNotReadyException("The database is initializing.", connString);
				if (ex.Errors[0].Message.IndexOf("ORA-03113") != -1 || ex.Errors[0].Message.IndexOf("ORA-03114") != -1)
					throw new BrokenConnectionException("The database connection is broken", connString);
#endif

                throw new DatabaseConnectionException($"Attempt to connect to database with connect string {connString} resulted in exception: {ex.Message}", ex);
			}

            // Set ClientId property of the connection to the value of the configuration variable ClientUser.
            // It is important to always set a value, if nothing else then String.Empty just to avoid database
            // username being used as undocumented(?) default since we want pdmisc.fetchOSUser to fall-back to
            // OS_USER with it's NVL conditional instead. (Setting connection.ClientId to empty string or null
            // is different from not setting it: The documentation says the default value of ClientId property
            // is null, but setting it explicitly to null leads to client_identifier in v$session being empty while
            // leaving it with default results in a value which is the current database username).
            // In this case we use the configuration variable, and it will always have a value since the getter
            // has built-in fall-back to current OS user.
#if TSS_Availability
            connection.ClientId = Environment.UserName;
#else
            Profiler.Log(10050, "IccConfiguration.Data.ClientUser");
            connection.ClientId = IccConfiguration.Data.ClientUser;
            //stopWatch.Stop();
            //var connectionOpenElapsed = (int)Math.Round(stopWatch.Elapsed.TotalMilliseconds);
            //stopWatch.Reset();
            //stopWatch.Start();
#endif
#if false
// The new ClientId value will not be reflected in v$session until next query, so to speed up the process (for debugging) we could execute a dummy query:
			using (OracleCommand tmpCmd = new OracleCommand("select 1 from dual", connection))
			{
				tmpCmd.ExecuteNonQuery();
			}
#endif

#if !TSS_Availability
            try
            {
                Profiler.Log(10060, "IccConfiguration.Data.ProgramKey etc");
                ProgKey = IccConfiguration.Data.ProgramKey;
                if (string.IsNullOrEmpty(Language))
                    Language = IccConfiguration.Data.Language;
                if (string.IsNullOrEmpty(Territory))
                    Territory = IccConfiguration.Data.Territory;
#endif
                if (ProgKey <= 0 && string.IsNullOrEmpty(Language) && string.IsNullOrEmpty(Territory))
                    return;

                Profiler.Log(10070, "PDMISC.Identify");
                var identifySql = ProgKey > 0 
                    ? $"BEGIN PDMISC.Identify(prog_key=>{ProgKey}, language=>'{Language}', territory=>'{Territory}'); END;"
                    : $"BEGIN PDMISC.Identify(                     language=>'{Language}', territory=>'{Territory}'); END;"
                    ;

                using (var identifyCmd = new OracleCommand(identifySql, connection))
				{
					identifyCmd.ExecuteNonQuery();
				}
#if !TSS_Availability
			}
			catch (Powel.Icc.Interop.ConfigurationException)
			{
				// If NLS_LANG isn't set, that's okay, we won't alter any session then.
			}
			catch (IndexOutOfRangeException)
			{
				// if NLS_LANG is not formated the correct
				// <language>_<territory>.<codepage> we get this exception, treat is as it
				// was not set. 
			}
            //stopWatch.Stop();
            //var pdMicsIdentifyElapsed = (int)Math.Round(stopWatch.Elapsed.TotalMilliseconds);
            //    Log.Warn($"\n\tgetConnStringElapsed = {getConnStringElapsed}" +
            //        $"\n\tconnectionOpenElapsed = {connectionOpenElapsed}" +
            //        $", pdMicsIdentifyElapsed = {pdMicsIdentifyElapsed}{LogSeparator}");
#endif

            // The following simple operation fails on both ODP.NET 9.2.0.4
            // and 10.1.0.2 when the Windows Regional Options dictate comma
            // as decimal separator. Until we have a verison of ODP.NET that
            // works correctly, we need to use the identify proc as above.
            //
            // OracleGlobalization glob = connection.GetSessionInfo();
            // connection.SetSessionInfo(glob);
            //
            // When we have a working ODP.NET, we may replace the call to
            // pdmisc.identify with the following:
            //
            // glob.Language = IccConfiguration.Data.Language;
            // glob.Territory = IccConfiguration.Data.Territory;
            //
            // However, it is possible that this might not work, for the same
            // reason that alter session doesn't work (cannot change language
            // in a session where language has already been set). This will
            // have to be tested with the TestConnectionPooling unit test.
            Profiler.Log(10080);
        }

        static void PrepareCommand(IDbCommand cmd, IDbConnection connection)
		{
            Profiler.Log(10000 + 90, "PrepareCommand");
			cmd.Connection = connection;
			cmd.CommandText = cmd.CommandText.Replace("\r", "");
            //cmd.Prepare();
            Profiler.Log(10000 + 91, "Exit PrepareCommand");
        }
        #endregion

        #region Database operations
        public static DataTable SqlToDataTable(string sql)
		{
			return SqlToDataTable(sql, null);
		}

		public static DataTable SqlToDataTable(string sql, 
			IDbConnection connection)
		{
			using (OracleCommand cmd = new OracleCommand(sql))
			{
				return CommandToDataTable(cmd, connection);
			}
		}

		public static DataTable CommandToDataTable(OracleCommand cmd)
		{
			DataSet ds = new DataSet();
			CommandIntoDataSet(cmd, ds, null);
			return ds.Tables[0];
		}

		public static DataTable CommandToDataTable(OracleCommand cmd, 
			IDbConnection connection)
		{
			DataSet ds = new DataSet();
			CommandIntoDataSet(cmd, ds, null, connection);
			return ds.Tables[0];
		}

		public static DataTable CommandToDataTable(OracleCommand cmd, 
			IDbTransaction transaction)
		{
			DataSet ds = new DataSet();
			CommandIntoDataSet(cmd, ds, null, transaction);
			return ds.Tables[0];
		}

		public static void SqlIntoDataSet(string sql, DataSet ds, 
			string name)
		{
			using (OracleCommand cmd = new OracleCommand(sql))
			{
				CommandIntoDataSet(cmd, ds, name);
			}
		}

		public static void CommandIntoDataSet(OracleCommand cmd, DataSet ds)
		{
			CommandIntoDataSet(cmd, ds, null);
		}

		public static void CommandIntoDataSet(OracleCommand cmd, DataSet ds, 
			string name)
		{
			using (OracleConnection connection = new OracleConnection())
			{
				PrepareConnection(connection);
				CommandIntoDataSet(cmd, ds, name, connection);
			}
		}

		public static void CommandIntoDataSet(OracleCommand cmd, DataSet ds, 
			string name, IDbConnection connection)
		{
			if (connection == null)
			{
				CommandIntoDataSet(cmd, ds, name);
				return;
			}

            var start = DateTime.Now;

            PrepareCommand(cmd, connection);

			TraceCommand(cmd, start);

            Profiler.Log(10000 + 100, "OracleDataAdapter.Fill");
            //var stopWatch = new Stopwatch();
            //stopWatch.Start();
            var da = new OracleDataAdapter(cmd);
            if (string.IsNullOrEmpty(name))
            {
                da.Fill(ds);
            }
            else
            {
                da.Fill(ds, name);
            }
            Profiler.Log(10000 + 101, "After OracleDataAdapter.Fill");
            //stopWatch.Stop();
            //var fillDataSetElapsed = (int)Math.Round(stopWatch.Elapsed.TotalMilliseconds);
            //Log.Warn($"\nfillDataSetElapsed = {fillDataSetElapsed}");
        }

        public static void CommandIntoDataSet(OracleCommand cmd, DataSet ds, 
			string name, IDbTransaction transaction)
		{
			if (transaction == null)
			{
				CommandIntoDataSet(cmd, ds, name);
				return;
			}

			CommandIntoDataSet(cmd, ds, name, transaction.Connection);
		}

		public static void ExecuteSql(string sql)
		{
			using (OracleCommand cmd = new OracleCommand(sql))
			{
				ExecuteCommand(cmd);
			}
		}

		public static void ExecuteSql(string sql, IDbConnection connection)
		{
			using (OracleCommand cmd = new OracleCommand(sql))
			{
				ExecuteCommand(cmd, connection);
			}
		}

		public static void ExecuteSql(string sql, 
			IDbTransaction transaction)
		{
			using (OracleCommand cmd = new OracleCommand(sql))
			{
				ExecuteCommand(cmd, transaction);
			}
		}

		public static int ExecuteCommand(IDbCommand cmd)
		{
			using (OracleConnection connection = new OracleConnection())
			{
				PrepareConnection(connection);
                Profiler.Log(10000, "Util.ExecuteCommand");
                var code = ExecuteCommand(cmd, connection);
                Profiler.Log(10001, "After Util.ExecuteCommand");
                return code;
			}
		}

        public static int ExecuteCommand(IDbCommand cmd, IDbConnection connection)
		{
			if (connection == null)
			{
                return ExecuteCommand(cmd);
            }

            PrepareCommand(cmd, connection);

            var start = DateTime.Now;

            int nrOfAffectedRows = 0;
			try
			{
                Profiler.Log(10002, "ExecuteNonQuery");
                nrOfAffectedRows = cmd.ExecuteNonQuery();
			}
			catch (OracleException ex)
			{
			    Log.Error($"ExecuteNonQuery failed. Connection string {connection.ConnectionString}\nCommand:{cmd.CommandText}\nException:{ex}");

			    OracleConnection.ClearPool( (OracleConnection) connection );
				// We cannot just check for error number 8177, because sometimes this
				// error is wrapped inside another one.);
				if (ex.Errors.Count == 1 && 
					ex.Errors[0].Message.IndexOf("ORA-08177") != -1)
					throw new TransactionSerializationException(
						"Database could not serialize access for transaction.", 
						ex);
				if (ex.Errors.Count == 1 && 
					( ex.Errors[0].Message.IndexOf("ORA-03113") != -1 ||
                      ex.Errors[0].Message.IndexOf("ORA-03114") != -1) )
					throw new BrokenConnectionException(
						"The database connection is broken, most likely because of a temporary network problem.", 
						connection.ConnectionString);
				if (ex.Errors.Count == 1 &&
					ex.Errors[0].Message.IndexOf("ORA-20113") != -1)
					throw new IccException(ErrorData.GetErrorText(connection), ErrorData.GetErrorNumber(connection));
				throw;
			}

            Profiler.Log(10010);

            TraceCommand(cmd, start);

            Profiler.Log(10020);

            return nrOfAffectedRows;
		}

		public static int ExecuteCommand(IDbCommand cmd, 
			IDbTransaction transaction)
		{
			if (transaction == null)
				return ExecuteCommand(cmd);

            return ExecuteCommand(cmd, transaction.Connection);
		}

		public static object SqlToScalar(string sql)
		{
			using (OracleCommand cmd = new OracleCommand(sql))
			{
				return CommandToScalar(cmd);
			}
		}

		public static object SqlToScalar(string sql, 
			IDbConnection connection)
		{
			using (OracleCommand cmd = new OracleCommand(sql))
			{
				return CommandToScalar(cmd, connection);
			}
		}

		public static object CommandToScalar(IDbCommand cmd)
		{
			using (OracleConnection connection = new OracleConnection())
			{
				PrepareConnection(connection);
				return CommandToScalar(cmd, connection);
			}
		}

		public static object CommandToScalar(IDbCommand cmd, 
			IDbConnection connection)
		{
			if (connection == null)
				return CommandToScalar(cmd);

			PrepareCommand(cmd, connection);
			DateTime start = DateTime.Now;
			TraceCommand(cmd, start);
			object retval = cmd.ExecuteScalar();
			return retval;
		}

		public static object CommandToScalar(IDbCommand cmd, 
			IDbTransaction transaction)
		{
			return CommandToScalar(cmd, transaction.Connection);
		}

		public static OracleDataReader SqlToDataReader(string sql, 
			IDbConnection connection)
		{
			using (OracleCommand cmd = new OracleCommand(sql))
			{
				return CommandToDataReader(cmd, connection);
			}
		}

		public static OracleDataReader CommandToDataReader(
			OracleCommand cmd, IDbConnection connection)
		{
			// TEH 2015-05-21: This causes AccessViolationException on Oracle 12.1.0.2
			// return CommandToDataReader(cmd,CommandBehavior.CloseConnection,connection);
			
			return CommandToDataReader(cmd,CommandBehavior.Default,connection);
		}

		public static OracleDataReader CommandToDataReader(OracleCommand cmd,
		                                                   CommandBehavior commandBehavior,
		                                                   IDbConnection connection)
		{
			PrepareCommand(cmd, connection);
			DateTime start = DateTime.Now;
			OracleDataReader reader = cmd.ExecuteReader(commandBehavior);
			TraceCommand(cmd, start);
			return reader;
		}
#endregion

#region Diagnostics
		static void TraceCommand(IDbCommand cmd, DateTime start)
		{
#if !TSS_Availability
            if (Log.IsDebugEnabled)
            {
                var stop = DateTime.Now;
                var msg = $"\n\tCommandText:\n\t{cmd.CommandText}" +
                          $"\n\tElapsed {(int) Math.Round((stop - start).TotalMilliseconds)}\n\tParameters:";
                var parameters = cmd.Parameters;
                if (parameters != null)
                {
                    try
                    {
                        var i = 1;
                        foreach (var param in parameters)
                        {
                            var oraParam = param as OracleParameter;
                            if (oraParam != null)
                            {
                                msg += $"\n\t\t{i,3}. = {GetString(oraParam)}";
                            }

                            i++;
                        }
                    }
                    catch (Exception err)
                    {
                        msg += $"\n\tException reading parameters:\n{err.Message}";
                    }
                }

                msg += $"{LogSeparator}";
                Log.Debug(msg);
            }

            return;
#endif
		}

		public static string ReadDbmsOutput(IDbConnection connection)
		{
			const int numlines = 1000;

			using (OracleCommand cmd = new OracleCommand("dbms_output.get_lines"))
			{
				cmd.CommandType = CommandType.StoredProcedure;

				OracleParameter par;

				par = cmd.Parameters.Add("lines", OracleDbType.Varchar2);
				par.Direction = ParameterDirection.Output;
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;

				int[] arrayBindSize = new int[numlines];

				for (int i=0; i<numlines; i++)
					arrayBindSize[i] = 256;

				par.ArrayBindSize = arrayBindSize;
				par.Size = numlines;

				par = cmd.Parameters.Add("numlines", numlines);
				par.Direction = ParameterDirection.InputOutput;

				ExecuteCommand(cmd, connection);

				Array lines = (Array)cmd.Parameters["lines"].Value;

				StringBuilder stringBuilder = new StringBuilder();

				for (int i=0; i<lines.Length; i++)
					stringBuilder.Append(lines.GetValue(i) + "\n");

				return stringBuilder.ToString();
			}
		}
#endregion

#region DataReader methods
		public static float GetFloat(OracleDataReader reader, 
			string paramName)
		{
			if( reader.IsDBNull(reader.GetOrdinal(paramName)))
				return float.NaN;
			else
				return Convert.ToSingle(reader.GetDecimal(
					reader.GetOrdinal(paramName)));
		}

		public static decimal GetDecimal(OracleDataReader reader, string paramName)
		{
			if( reader.IsDBNull(reader.GetOrdinal(paramName)))
				return decimal.MinValue;

            return reader.GetDecimal(reader.GetOrdinal(paramName));
		}

		public static decimal GetDecimal(OracleParameter param)
		{
			var o = param.Value;

			if (o == null)
				throw new ArgumentNullException();

			var convertible = o as IConvertible;

			if (convertible != null)
				return convertible.ToDecimal(CultureInfo.CurrentCulture);

			if (o is OracleDecimal)
				return ((OracleDecimal)o).Value;

			throw new InvalidOperationException("The parameter is of a data type that cannot be converted to the desired data type.");
		}

		public static int GetInt32(OracleDataReader reader, 
			string paramName)
		{
			if( reader.IsDBNull(reader.GetOrdinal(paramName)))
				return Int32.MinValue;

			return reader.GetInt32(reader.GetOrdinal(paramName));
		}
			
		public static string GetString(OracleDataReader reader, string paramName)
		{
			if( reader.IsDBNull(reader.GetOrdinal(paramName)))
				return "";

            return reader.GetString(reader.GetOrdinal(paramName));
		}

#if !TSS_Availability
		public static UtcTime GetDate(OracleDataReader reader, string paramName)
		{
			if (reader.IsDBNull(reader.GetOrdinal(paramName)))
				return UtcTime.Null;

            return RegionalCalendar.GetCalendar(RegionalCalendar.dbName).ToUtcTime( reader.GetDateTime(reader.GetOrdinal(paramName)));
		}
#endif

#endregion

#region DataRow methods
		public static float GetFloat(DataRow dr, 
			string paramName)
		{
			if( dr.IsNull(paramName))
				return float.NaN;

            return Convert.ToSingle(GetDecimal(dr, paramName));
		}

		public static decimal GetDecimal(DataRow dr, 
			string paramName)
		{
			if( dr.IsNull(paramName))
				return decimal.MinValue;

            return (decimal) dr[paramName];
		}

		public static int GetInt32(DataRow dr, string paramName)
		{
			if (dr.IsNull(paramName))
				return Int32.MinValue;

			object o = dr[paramName];
			IConvertible convertible = o as IConvertible;

			if (convertible != null)
				return convertible.ToInt32(CultureInfo.CurrentCulture);

			if (o is OracleDecimal)
				return ((OracleDecimal)o).ToInt32();

			throw new InvalidOperationException("The cell is of a data type that cannot be converted to the desired data type.");
		}

		public static int GetInt16(DataRow dr, 
			string paramName)
		{
			if( dr.IsNull(paramName))
				return Int32.MinValue;

            return Convert.ToInt32((Int16) dr[paramName]);
		}
			
		public static string GetString(DataRow dr, 
			string paramName)
		{
			if( dr.IsNull(paramName))
				return "";

            return (string) dr[paramName];
		}

        public static DateTime ConvertFromIccDateTime(DataRow dr, string columnName, ref string errorMessage)
        {
            try
            {
                var value = (string)dr[columnName];
                return ConvertFromIccDateTime(value);
            }
            catch (InvalidCastException)
            {
                errorMessage = AppendText(errorMessage, string.Format("Invalid value in {0}", columnName));
            }
            catch (ArgumentNullException)
            {
                errorMessage = AppendText(errorMessage, string.Format("No value in {0}", columnName));
            }
            catch (FormatException)
            {
                errorMessage = AppendText(errorMessage, string.Format("Invalid format in {0}", columnName));
            }

            return default(DateTime);
        }

        public static TDataType ConvertTo<TDataType>(DataRow dr, string columnName)
        {
            return ConvertTo<TDataType>(dr, dr.Table.Columns.IndexOf(columnName));
        }

        public static TDataType ConvertTo<TDataType>(DataRow dr, int columnIndex)
        {
            try
            {
                return (TDataType)dr[columnIndex];
            }
            catch (InvalidCastException)
            {
            }

            return default(TDataType);
        }

        public static TDataType ConvertTo<TDataType>(DataRow dr, string columnName, ref string errorMessage)
	    {
	        return ConvertTo<TDataType>(dr, dr.Table.Columns.IndexOf(columnName), ref errorMessage);
	    }

	    public static TDataType ConvertTo<TDataType>(DataRow dr, int columnIndex, ref string errorMessage)
        {
            try
            {
                return (TDataType)dr[columnIndex];
            }
            catch (InvalidCastException Ex)
            {
                errorMessage = AppendText(errorMessage, string.Format("Invalid value in {0}, {1}", dr.Table.Columns[columnIndex], Ex.Message));
            }

            return default(TDataType);
        }

        public static string AppendText(string text, string appendText)
        {
            if (!string.IsNullOrEmpty(text))
            {
                text += ". ";
            }

            text += appendText;

            return text;
        }

#if !TSS_Availability
        public static UtcTime GetDate(DataRow dr, 
			string paramName)
		{
			if( dr.IsNull(paramName))
				return UtcTime.Null;

            return RegionalCalendar.GetCalendar(RegionalCalendar.dbName).ToUtcTime((DateTime) dr[paramName]);
		}
#endif

#endregion

#region Conversion classes
		public static DateTime ConvertFromIccDateTime(string s)
		{
			return DateTime.ParseExact(s, "yyyyMMddHHmmssfff",
				CultureInfo.InvariantCulture);
		}

		public static string ConvertToIccDateTime(DateTime dt)
		{
			return dt.ToString("yyyyMMddHHmmssfff");
		}

#if !TSS_Availability
		/// <summary>
		/// Generate Oracle Date based on UtcTime and a calendar for the database.
		/// Needs to be max performance.
		/// </summary>
		/// <param name="TzDb"></param>
		/// <param name="t"></param>
		/// <returns></returns>
		public static OracleDate OracleDateFromUtcTime(RegionalCalendar TzDb, UtcTime t)
		{
			int Y, M, D, h, m, s, ms; TzDb.asYMDhms_ms(t, out Y, out M, out D, out h, out m, out s, out ms);
			return new OracleDate(Y, M, D, h, m, s);
		}

		public static UtcTime UtcTimeFromOracleDate(OracleDate dt, RegionalCalendar TzDb)
		{
			return TzDb.ToUtcTime(dt.Value);
		}
#endif

		[Obsolete("Use Convert.ToInt32 instead.")]
		public static int BoolToInt(bool trueOrFalse)
		{
			if (trueOrFalse)
				return 1;

            return 0;
		}

		[Obsolete("Use Convert.ToBoolean instead.")]
		public static bool IntToBool(int dbTrueOrFalse)
		{
			if (dbTrueOrFalse == 0)
				return false;

            return true;
		}
#endregion

		public static void Commit(IDbTransaction transaction)
		{
			transaction.Commit();
		}

		public static bool IsNull(OracleParameter par)
		{
			if (IsNull((object)par))
				return true;

			return IsNull(par.Value);
		}

		public static bool IsNull(object o)
		{
			if (o == null || o == DBNull.Value)
				return true;

			var propertyInfo = o.GetType().GetProperty("IsNull");

			if (propertyInfo == null)
				return false;

			var methodInfo = propertyInfo.GetGetMethod();

            return (bool)methodInfo.Invoke(o, null);
		}

		/// <summary>
		/// Provides a safe way to read an integer from an output parameter.
		/// Some versions of ODP.NET use OracleDecimal where Int32 had
		/// previously been used.
		/// </summary>
		public static int GetInt32(OracleParameter param)
		{
			var o = param.Value;

			if (o == null)
				throw new ArgumentNullException();

			var convertible = o as IConvertible;

			if (convertible != null)
				return convertible.ToInt32(CultureInfo.CurrentCulture);

			if (o is OracleDecimal)
				return ((OracleDecimal)o).ToInt32();

			throw new InvalidOperationException("The parameter is of a data type that cannot be converted to the desired data type.");
		}

		public static string GetString(OracleParameter param)
		{
			var o = param.Value;

			if (o == null)
				throw new ArgumentNullException();

			var convertible = o as IConvertible;

			if (convertible != null)
				return convertible.ToString(CultureInfo.CurrentCulture);

            return "<cannot convert to string>";
			//throw new InvalidOperationException("The parameter is of a data type that cannot be converted to the desired data type.");
		}

		public static string GetSchemaRelease(string connectionString)
		{
			using (var connection = Util.OpenConnection(connectionString))
			{
				return (string)SqlToScalar(
					"select value from pddbenv where name = '_ICC_SCHEMA_RELEASE'",
					connection);
			}
		}

		public static void SetPLSQLAssociativeArrayValue<T>(OracleParameter param, T[] array)
		{
			param.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
			if (array == null || array.Length == 0)
			{
				param.Size = 0;
				param.ArrayBindStatus = new OracleParameterStatus[] { OracleParameterStatus.NullInsert };
				array = null;
			}
			param.Value = array;
		}

		public static string UnlimitedIn(string subject, int[] list)
		{
			return UnlimitedIn(subject, list, 1000);
		}

		public static string UnlimitedIn(string subject, int[] list, int limit)
		{
			if (list == null || list.Length == 0)
				throw new ArgumentException("List cannot be empty", "list");

			var s = new StringBuilder();

			for (int i=0; i<list.Length; i++)
			{
				if (i % limit == 0)
				{
					if (i > 0)
						s.Append(") or ");

					s.Append(subject);
					s.Append(" in (");
				}
				else if (i > 0)
					s.Append(", ");

				s.Append(list[i]);
			}

			s.Append(")");

			if (list.Length > limit)
			{
				s.Insert(0, "(");
				s.Append(")");
			}

			return s.ToString();
		}

		/// <summary>
		/// Extend access to TS interval units in TSINT_TP view. Shpuld be used for testing against DBs that have restricted access to interval units
		/// </summary>
		public static void Extend_TSINT_TP_Access(IDbConnection connection)
		{
			var cmd = new OracleCommand(
				"BEGIN\n\t" +
					"UPDATE TP_TSINT_TP SET InUse = 'Y'\n\t\t" +
						"WHERE InUse NOT IN ('Y', '?')\n\t\t\t" +
							"AND TSIN_KEY IN (131, 141)\n\t\t" +
							";\n" +
				"END;"
				);
			cmd.CommandType = CommandType.Text;
			ExecuteCommand(cmd, connection);
		}

		public static string GetUser()
		{
			//This will get the correct username according to current
			//configuration (IccConfiguration) even in case of external authentication.
			return (string)SqlToScalar("select code from users where dbusr_id = uid");
		}

        public static DataTable JoinDataTables(DataTable t1, DataTable t2, params Func<DataRow, DataRow, bool>[] joinOn)
        {
            DataTable result = new DataTable();
            foreach (DataColumn col in t1.Columns)
            {
                if (result.Columns[col.ColumnName] == null)
                    result.Columns.Add(col.ColumnName, col.DataType);
            }
            foreach (DataColumn col in t2.Columns)
            {
                if (result.Columns[col.ColumnName] == null)
                    result.Columns.Add(col.ColumnName, col.DataType);
            }
            foreach (DataRow row1 in t1.Rows)
            {
                var joinRows = t2.AsEnumerable().Where(row2 =>
                                                           { return joinOn.All(parameter => parameter(row1, row2)); });
                foreach (DataRow fromRow in joinRows)
                {
                    DataRow insertRow = result.NewRow();
                    foreach (DataColumn col1 in t1.Columns)
                    {
                        insertRow[col1.ColumnName] = row1[col1.ColumnName];
                    }
                    foreach (DataColumn col2 in t2.Columns)
                    {
                        insertRow[col2.ColumnName] = fromRow[col2.ColumnName];
                    }
                    result.Rows.Add(insertRow);
                }
            }
            return result;
        }

	    #region Extensions

	    public static string Dump(this DataTable table)
	    {
	        var t = CultureInfo.CurrentCulture.TextInfo.ListSeparator;
	        string tableName = table.TableName;
            if (tableName == "Table" && table.ExtendedProperties.Count == 1)
            {
                tableName = table.ExtendedProperties["BaseTable.0"].ToString();
            }
	        var ret = new StringBuilder(tableName);
	        ret.AppendLine();
	        foreach (DataColumn column in table.Columns)
	        {
	            ret.Append(column.ColumnName);
	            ret.Append(t);
	        }
	        ret.AppendLine();
	        foreach (DataRow row in table.Rows)
	        {
	            foreach (var value in row.ItemArray)
	            {
	                ret.Append(value);
	                ret.Append(t);
	            }
	        }
	        return ret.ToString();
	    }

        public static OracleCommand AddParameterCollection<TValue>(this OracleCommand command, string name, OracleDbType type, IEnumerable<TValue> collection)
        {
            var oraParams = new List<OracleParameter>();
            var counter = 0;
            var collectionParams = new StringBuilder(":");
            foreach (var obj in collection)
            {
                var param = name + counter;
                collectionParams.Append(param);
                collectionParams.Append(", :");
                oraParams.Add(new OracleParameter(param, type) { Value = obj });
                counter++;
            }
            collectionParams.Remove(collectionParams.Length - 3, 3);
            command.CommandText = command.CommandText.Replace(":" + name, collectionParams.ToString());
            command.Parameters.AddRange(oraParams.ToArray());
            return command;
        }

	    #endregion
    }

    public static class Profiler
    {
        private static readonly ILog Logger = LogManager.GetLogger(typeof(Profiler));
        struct ProfilerRec
        {
            public readonly int LogCount;
            public readonly int PrevStepId;
            public readonly int StepId;
            public readonly int PrevElapsed;
            public readonly int Elapsed;
            public readonly string Descr;
            public ProfilerRec(int logCount, int prevStepId, int stepId, int prevElapsed, int elapsed, string descr)
            {
                LogCount = logCount;
                PrevStepId = prevStepId;
                StepId = stepId;
                PrevElapsed = prevElapsed;
                Elapsed = elapsed;
                Descr = descr;
            }
        }
        static private List<ProfilerRec> _logList;
        static private Stopwatch _stopWatch;
        static int _logCount = 0;
        static int _prevStepId = -1;
        static int _prevElapsed = 0;
        const int LastStepId = int.MaxValue;
        const int ElapsedThreshold = 9;

        public static void StartProfiling(long priorElapsed)
        {
            if (!Logger.IsWarnEnabled)
            {
                return;
            }

            Logger.Warn($"\n\tProfiling started. priorElapsed = {priorElapsed}");

            _logList = new List<ProfilerRec>(100);
            _logCount = 0;
            _prevStepId = -1;
            _prevElapsed = 0;
            _stopWatch = new Stopwatch();
            _stopWatch.Start();
        }

        private static bool IsRunning()
        {
            return _logList != null && _stopWatch != null && _stopWatch.IsRunning;
        }
        public static void Log(int stepId, string descr = null)
        {
            if (!Logger.IsWarnEnabled)
            {
                return;
            }

            if (!IsRunning())
            {
                return;
            }

            Debug.Assert(_stopWatch != null);
            Debug.Assert(_logList != null);

            _stopWatch.Stop();

            //Logger.Warn($"\n\tstepId = {stepId}");

            _logCount++;

            var elapsed = (int)Math.Round(_stopWatch.Elapsed.TotalMilliseconds);
            if (elapsed - _prevElapsed > ElapsedThreshold || stepId == LastStepId || !string.IsNullOrEmpty(descr))
            {
                _logList.Add(new ProfilerRec(_logCount, _prevStepId, stepId, _prevElapsed, elapsed, descr));
            }

            _prevStepId = stepId;
            _prevElapsed = elapsed;
            _stopWatch.Start();
        }

        public static void StopProfiling()
        {
            if (!Logger.IsWarnEnabled)
            {
                return;
            }

            if (!IsRunning())
            {
                return;
            }

            Debug.Assert(_stopWatch != null);
            Debug.Assert(_logList != null);

            Logger.Debug($"\n\t_logList.Count = {_logList.Count}");

            Log(LastStepId);

            _stopWatch.Stop();
            var totalElapsed = _logList.Last().Elapsed;
            var report = $"\n{Util.LogSeparator}\ntotalElapsed = {totalElapsed}" +
                $"\n\t{"#",3}  {"Steps", 13}  {"elapsed ms", 11}  {"split time", 10}   {"From step description", -60}" +
                $"{Util.LogSeparator}";
            string prevDescr = null;
            foreach (var rec in _logList)
            {
                var elapsed = rec.Elapsed - rec.PrevElapsed;
                if (elapsed > ElapsedThreshold || rec.StepId == LastStepId)
                {
                    report +=
                        $"\n\t{rec.LogCount, 3}.  {rec.PrevStepId, 5}->" +
                        $"{(rec.StepId == LastStepId ? -1 : rec.StepId), 5} " +
                        $"  {elapsed, 10}  {rec.Elapsed, 10}" +
                        $"   {(!string.IsNullOrEmpty(prevDescr) ? prevDescr : " "), -60}";
                    for (var i = 1; i <= 10; i++)
                    {
                        if (elapsed < i * 400)
                            break;
                        if (i == 1)
                            report += " LONG";
                        report += "*";
                    }
                }
                prevDescr = rec.Descr;
            }
            Logger.Warn(report + Util.LogSeparator);
        }
    }
}
