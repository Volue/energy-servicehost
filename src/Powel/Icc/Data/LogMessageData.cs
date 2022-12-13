using System;
using System.Collections.Generic;
using System.Data;
using System.Threading;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using Powel.Icc.Common;
using Powel.Icc.Interop;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for LogMessageData.
	/// </summary>
	public sealed class LogMessageData
	{
		private LogMessageData()
		{
		}

		static string SelectMessageDefinitions
		{
			get
			{
				return "SELECT t.MESSAGE_KEY, t.MESSAGE_TEXT, d.application_key FROM ICC_MESSAGE_TEXT t JOIN ICC_MESSAGE_DEFINITION d ON t.MESSAGE_KEY = d.MESSAGE_KEY ";
			}
		}
					
		static string SelectLogMessages
		{
			get
			{
				return
					@"select appl.rcount, applic.application_key logged_application_key, 
appl.message_key, appl.datetim message_datetim, appl.descript, app.user_key, app.osuser_key 
from icc_appl appl 
join icc_app app on appl.app_key = app.app_key 
join icc_application applic  on app.name = applic.name ";
			}
		}

		static string SelectLogMessageArguments
		{
			get
			{
				return
					@"select iaa.rcount, arg_number, arg_value
from icc_appl_arg iaa
join icc_appl ia  on iaa.rcount = ia.rcount ";
			}
		}

		public static DataTable FetchLogMessages(int messageDefinitionID, UtcTime from, UtcTime until)
		{
			return FetchLog(SelectLogMessages, messageDefinitionID, from, until);
		}

		public static DataTable FetchLogMessage(int messageID) {
			using (OracleCommand cmd = new OracleCommand(SelectLogMessages + "where rcount = :1")) {
				cmd.Parameters.Add(null, messageID);
				return Util.CommandToDataTable(cmd);
			}
		}

		public static DataTable FetchLogMessageArguments(int messageDefinitionID, UtcTime from, UtcTime until) {
			return FetchLog(SelectLogMessageArguments, messageDefinitionID, from, until);
		}

		public static DataTable FetchLogMessageArguments(int messageID) {
			using (OracleCommand cmd = new OracleCommand("select rcount, arg_number, arg_value from icc_appl_arg where rcount = :1")) {
				cmd.Parameters.Add(null, messageID);
				return Util.CommandToDataTable(cmd);
			}
		}

		static DataTable FetchLog(string select, int messageDefinitionID, UtcTime from, UtcTime until) {
			RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;
			using (OracleCommand cmd = new OracleCommand(select + "where message_key = :1 and datetim >= :2 and datetim < :3")) {
				cmd.Parameters.Add(null, messageDefinitionID);
				cmd.Parameters.Add(null, calendar.ToDatabaseString(from));
				cmd.Parameters.Add(null, calendar.ToDatabaseString(until));
				return Util.CommandToDataTable(cmd);
			}
		}

		public static string FetchMessageDefinition(int logMessageId) {
			using (OracleCommand cmd = new OracleCommand("SELECT message_text from icc_message_text WHERE message_key = :messageKey")) {
				cmd.Parameters.Add(":messageKey", logMessageId);

				return (string)Util.CommandToScalar(cmd);
			}
		}

		public static DataTable FetchMessageDefinitionsForApplication(int applicationID) {
			using (OracleCommand cmd = new OracleCommand(SelectMessageDefinitions + "WHERE d.APPLICATION_KEY = :application_key")) {
				cmd.Parameters.Add(":application_key", applicationID);

				return Util.CommandToDataTable(cmd);
			}
		}

		public static DataTable FetchCommonMessageDefinitions() {
			return Util.SqlToDataTable(SelectMessageDefinitions + "where application_key in (select application_key from icc_application where native_name like '-%')");
		}

		public static bool AppKeyExists(int appKey, IDbConnection connection) {
			using (OracleCommand cmd = new OracleCommand(
			    "select app_key from icc_app where app_key = :1")) {
				cmd.Parameters.Add(null, appKey);
				return Util.CommandToScalar(cmd, connection) != null;
			}
		}

        // FetchRecentAppKeyForApplication simple variant
		public static DataTable FetchRecentAppKeyForApplication(string appName) {
			using (OracleCommand cmd = new OracleCommand(
                "select max(app_key) as app_key from icc_app where name = :1 and startime > pdmisc.date_format (pdmisc.time_local2normal(sysdate -1/24*60))")) {
				cmd.Parameters.Add(null, appName);
				return Util.CommandToDataTable(cmd);
			}
		}

        // FetchRecentAppKeyForApplication variant where it is used criteria in icc_app_info.descript and a few attempts
        // Should use this variant for import/export for safer correct match
        // Using a few attempts because the application may not have finished yet (e.g. it may have been started from a script process)
        public static DataTable FetchRecentAppKeyForApplication(string appName, string identifier, bool isReference = false) {
            string ident = (isReference ? "REFERENCE#" : "FILENAME#") + identifier;
            int attempts = 0;
            const int maxAttempts = 3;
            DataTable tbl = new DataTable();
            while (attempts++ < maxAttempts) {
                using (OracleCommand cmd = new OracleCommand(
                    "select max(ia.app_key) as app_key from icc_app ia, icc_app_info iai where ia.app_key = iai.app_key and ia.name = :1 and iai.descript like :2 and ia.startime > pdmisc.date_format (pdmisc.time_local2normal(sysdate -1/24*60))")) {
                    cmd.Parameters.Add(null, appName);
                    cmd.Parameters.Add(null, ident);
                    tbl = Util.CommandToDataTable(cmd);
                    if (tbl.Rows.Count > 0)
                        return tbl;
                }
                if (attempts < maxAttempts)
                    Thread.Sleep(1000); // 1 sec
            }
            return tbl;
        }

        public static List<string> FetchFilenames(int AppKey)
        {
            var ret = new List<string>();
            var cmd = new OracleCommand("select descript from icc_app_info where app_key = :appKey and descript like 'FILENAME#%'");
            cmd.Parameters.Add(":appKey", AppKey);
            var dt = Util.CommandToDataTable(cmd);
            foreach (DataRow row in dt.Rows)
            {
                ret.Add(row[0].ToString().Substring(9));
            }

            return ret;
        }

        public static DataTable FetchLogMessagesForAppKey(int appKey) {
			using (OracleCommand cmd = new OracleCommand("select * from icc_appl where app_key = :1")) {
				cmd.Parameters.Add(null, appKey);
				return Util.CommandToDataTable(cmd);
			}
		}

	    [Obsolete("Use InitLog, LogMessage and Exit instead.")]
		public static void Log(int messageID, bool requiresSigning, string message) {
			// For now we simply write to the database, but in the future
			// we might need to wrap or reimplement the full ActivityLog
			// functionality.
			//
			// We even connect to whichever icc_app is most recent, really a
			// quite horrible thing to do.

			using (OracleCommand cmd = new OracleCommand(
			                                             @"declare
	app_key number;
begin
select max(app_key) into app_key from icc_app;
INSERT INTO ICC_APPL (APP_KEY, MESSAGE_KEY, INFOTYPE, DESCRIPT, SIGN_THIS_EVENT, PROFILE_KEY) VALUES (app_key, :message_key, :infotype, :descript, :sign_this_event, :profile_key);
end;")) {
				cmd.Parameters.Add(":message_key", messageID);
				cmd.Parameters.Add(":infotype", new OracleDecimal(0));
				cmd.Parameters.Add(":descript", message);
				cmd.Parameters.Add(":sign_this_event", Convert.ToByte(requiresSigning));
				cmd.Parameters.Add(":profile_key", new OracleDecimal(0));

				Util.ExecuteCommand(cmd);
			}
		}

		/// <summary>
		/// Starts a new module log in the event log for the given module.
		/// </summary>
		/// <param name="moduleKey"></param>
		public static void InitLog(IccModule moduleKey, IDbConnection connection)
		{
			InitLog(moduleKey, 0, false, null, null, connection);
		}

		/// <summary>
		/// Starts a new module log in the event log
		/// </summary>
		/// <param name="moduleKey"></param>
		/// <param name="appKeyOwner"></param>
		/// <param name="useLogKey"></param>
		/// <param name="name"></param>
		/// <param name="appendix"></param>
		public static void InitLog(IccModule moduleKey, int appKeyOwner, 
		                           bool useLogKey, string name,
		                           string appendix, IDbConnection connection) {
			using (OracleCommand cmd = new OracleCommand("pdlog.init")) {
				cmd.CommandType = CommandType.StoredProcedure;

				cmd.Parameters.Add("moduleKey", (int)moduleKey);
				if (useLogKey) {
					cmd.Parameters.Add("appKeyOwner", appKeyOwner);
					cmd.Parameters.Add("useLogKey", (int)1);
				} else {
					cmd.Parameters.Add("appKeyOwner", null);
					cmd.Parameters.Add("useLogKey", null);
				}
				cmd.Parameters.Add("name", name);
				cmd.Parameters.Add("appendix", appendix);

				Util.ExecuteCommand(cmd, connection);
			}
		}

        /// <summary>
		/// Logs the given message to a module log of the event log. 
		/// Requires a connection (transaction) to an open (initialized) event log module item.
		/// Run InitLog before LogMessage.
		/// Run Exit to close a event log module item.
		/// </summary>
		/// <param name="moduleKey"></param>
		/// <param name="messageKey"></param>
		/// <param name="args">string[] - A list of arguments to the log message. Maximum 10 arguments. Additional arguments is ignored</param>
		public static void LogMessage(IccModule moduleKey, int messageKey, string[] args, IDbConnection connection) {
			using (OracleCommand cmd = new OracleCommand("pdlog.logMessage")) {
				cmd.CommandType = CommandType.StoredProcedure;

				cmd.Parameters.Add("moduleKey", (int)moduleKey);
				cmd.Parameters.Add("messageKey", messageKey);
				string argnr = "";
				if (args != null) {
					for (int i = 0; i < args.Length && i < 10; i++) { //ignores argument 11 and up

						if (args[i] != null) {
							argnr = "0";
							if (i > 8) argnr = "";
							argnr += (i + 1).ToString();
							cmd.Parameters.Add("arg" + argnr, args[i]);
						}
					}
				}

				Util.ExecuteCommand(cmd, connection);
            }
		}

		/// <summary>
		/// Returns the log message as it should look in log but does not write it, easy way to get exception translated
		/// Requires a connection (transaction) to an open (initialized) event log module item.
		/// Run InitLog before GetLogMessage.
		/// Run Exit to close a event log module item.
		/// </summary>
		/// <param name="moduleKey"></param>
		/// <param name="messageKey"></param>
		/// <param name="args">string[] - A list of arguments to the log message. Maximum 10 arguments. Additional arguments is ignored</param>
		public static string GetLogMessage(IccModule moduleKey, int messageKey, string[] args, IDbConnection connection) {
			using (OracleCommand cmd = new OracleCommand("pdlog.GetLogMessage")) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.Parameters.Add("moduleKey", (int)moduleKey);
				cmd.Parameters.Add("messageKey", messageKey);
				string argnr = "";
				if (args != null) {
					for (int i = 0; i < args.Length && i < 10; i++) { //ignores argument 11 and up

						if (args[i] != null) {
							argnr = "0";
							if (i > 8) argnr = "";
							argnr += (i + 1).ToString();
							cmd.Parameters.Add("arg" + argnr, args[i]);
						}
					}
				}

				object o = Util.CommandToScalar( cmd, connection);
				if (o == null)
					throw new ArgumentNullException();
				IConvertible convertible = o as IConvertible;

				if (convertible != null)
					return convertible.ToString();

				throw new InvalidOperationException("The parameter is of a data type that cannot be converted to the desired data type.");
			}
		}

		/// <summary>
		/// Logs a debug message to a event log module item
		/// </summary>
		/// <param name="message"></param>
		/// <param name="transaction"></param>
		public static void LogDebugMessage(string message, IDbConnection connection) {
			using (OracleCommand cmd = new OracleCommand("pdlog.logdebugmessage")) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.Parameters.Add("messageText", message);
				Util.ExecuteCommand(cmd, connection);
			}
		}

		public static int GetAppKey(IDbConnection connection) {
			using (OracleCommand cmd = new OracleCommand("pdlog.GetAppKey")) {
				cmd.CommandType = CommandType.StoredProcedure;

				OracleParameter par = cmd.Parameters.Add("retval", OracleDbType.Int32);
				par.Direction = ParameterDirection.ReturnValue;
				Util.ExecuteCommand(cmd, connection);
				//OracleDecimal appKey = (OracleDecimal).Value;
				return Util.GetInt32(par);
			}
		}

		public static void SetAppKey(int appKey, IDbConnection connection) {
			using (OracleCommand cmd = new OracleCommand("pdmisc.set_log_number")) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.Parameters.Add("app_key_value", appKey);
				Util.ExecuteCommand(cmd, connection);
			}
        }
		
		/// <summary>
		/// Ends a module log in the event log
		/// </summary>
		public static void Exit(IDbConnection connection) {
			using (OracleCommand cmd = new OracleCommand("pdlog.exit")) {
				cmd.CommandType = CommandType.StoredProcedure;
				Util.ExecuteCommand(cmd, connection);
			}
		}
	}
}