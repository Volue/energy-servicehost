using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using Powel.ConfigurationSystem.Cli;
using Powel.Icc.Interop;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Common
{
	/// <summary>
	/// Summary description for IccConfiguration.
	/// </summary>
	public sealed class IccConfiguration
	{
        //TOSA private static IccApplication iccApplication = null;

		private IccConfiguration()
		{
		}

        public static IConfigurationReader ConfigurationReader { get; set; } = new IccConfigurationReader();

        public static string IccAvlRestrAdvRights
		{
			get { return ConfigurationReader.ReadOptionalString("ICC_AVAILABILITY_ADVANCED_USER",string.Empty); }
		}
		public static string IccDir
		{
			get { return ConfigurationReader.ReadString("ICCDIR"); }
		}
		public static string UserName
		{
			get { return ConfigurationReader.ReadString("USERNAME"); }
		}

		public static string IccHome
		{
			get { return ConfigurationReader.ReadOptionalString("ICC_HOME", ""); }
		}

		public static string Home
		{
			get { return ConfigurationReader.ReadOptionalString("HOME", ""); }
		}

		public static string TmpDir
		{
			get { return ConfigurationReader.ReadString("TMPDIR"); }
		}
        public static string Tmp
        {
            get { return ConfigurationReader.ReadString("TMP"); }
        }

		public static string IccTmp
		{
			get { return ConfigurationReader.ReadOptionalString("ICC_TMP",Tmp);}
		}

		public static string IccSender
		{
			get { return ConfigurationReader.ReadOptionalString("ICC_SENDER", ""); }
		}

		public static string TestSourceFileDir
		{
			get { return ConfigurationReader.ReadOptionalString("TEST_SOURCE_FILEDIR", ""); }
		}

		public static int IccDefaultSpotArea
		{
			get { return ConfigurationReader.ReadInt32("ICC_DEFAULT_SPOT_AREA");}
		}

		public static string IccHourReconciliationCisSystem
		{
			get { return ConfigurationReader.ReadOptionalString("ICC_CORRECTION_SETTLEMENT_CIS_SYSTEM","");}
		}

		public static string IccCorrectionSettlementCisSystem
		{
			get { return ConfigurationReader.ReadOptionalString("ICC_CORRECTION_SETTLEMENT_CIS_SYSTEM", ""); }
		}

		public static bool IccExportIgnoreLockedCopySeries
		{
			get { return ConfigurationReader.ReadOptionalBoolean("ICC_EXPORT_IGNORE_LOCKED_COPY_SERIES", false); }
		}
		public static int IccProfileReconciliationBulksize
		{
			get { return ConfigurationReader.ReadOptionalInt32("ICC_PROFILE_RECONCILIATION_BULKSIZE", 1000); }
		}
		public static string IccWebBrowser
		{
			get { return ConfigurationReader.ReadOptionalString("ICC_WEB_BROWSER", "");}
		}

		public static string IccEditor
		{
			get { return ConfigurationReader.ReadOptionalString("ICC_EDITOR", "write"); }
		}

		public static string IccGribUtilHome
		{
			get { return ConfigurationReader.ReadOptionalString("ICC_GRIB_UTIL_HOME", ""); }
		}

        public static string IccImportDir
        {
            get { return ConfigurationReader.ReadOptionalString("ICC_IMPORT_DIR", ""); }
        }

        public static string IccXsdPath
        {
            get { return ConfigurationReader.ReadOptionalString("ICC_XSD_PATH", ""); }
        }

        public static string IccCompelloApiKey
        {
            get { return ConfigurationReader.ReadOptionalString("ICC_COMPELLO_API_KEY", ""); }
        }
        public static string IccCompelloAddress
        {
            get { return ConfigurationReader.ReadOptionalString("ICC_COMPELLO_ADDRESS", ""); }
        }
        public static int IccCompelloPort
        {
            get { return ConfigurationReader.ReadInt32("ICC_COMPELLO_PORT"); }
        }
        public static int IccCompelloHeartbeatInterval
        {
            get { return ConfigurationReader.ReadInt32("ICC_COMPELLO_HEARTBEAT_INTERVAL"); }
        }
        public static int IccCompelloRestartInterval
        {
            get { return ConfigurationReader.ReadInt32("ICC_COMPELLO_RESTART_INTERVAL"); }
        }
        public static string HelpDir
        {
            get { return ConfigurationReader.ReadOptionalString("HELP_DIR", ""); }
        }

	    public static string IccPomInstallPath
	    {
	        get { return ConfigurationReader.ReadOptionalString("ICC_POM_INSTALLPATH", string.Empty); }
	    }

        public static string IccPotInstallPath
        {
            get { return ConfigurationReader.ReadOptionalString("ICC_POT_INSTALLPATH", string.Empty); }
        }

        public static string IccPomaInstallPath
        {
            get { return ConfigurationReader.ReadOptionalString("ICC_POMA_INSTALLPATH", string.Empty); }
        }


		//TOSA      public static void SetLogonInfo(IccApplication iccApplication)
		//{
		//	IccConfiguration.iccApplication = iccApplication;
		//}

		public sealed class Time
		{
			private static RegionalCalendar m_rcDatabase = null;

			static RegionalCalendar ReadCalendar(string filename)
			{
				using (StreamReader reader = new StreamReader(filename))
				{
					return RegionalCalendar.CreateFromDefinition(
						reader.ReadToEnd());
				}
			}

			static RegionalCalendar ReadCalendar(string key, RegionalCalendar defaultCalendar)
			{
				//System.Diagnostics.Trace.WriteLine("Time.ReadCalendar");
				//System.Diagnostics.Trace.WriteLine("..key = " + key);
				//System.Diagnostics.Trace.WriteLine("..defaultCalendar = " + defaultCalendar.ToString() +
				//        " (" + defaultCalendar.Name + ")");

				RegionalCalendar calendar = defaultCalendar;

				string filename = ConfigurationReader.ReadOptionalString(key, "");

				//System.Diagnostics.Trace.WriteLine("..[Calendar] filename = " + filename);

				if (!string.IsNullOrEmpty(filename))
				{
					if (!File.Exists(filename))
					{
						string s = String.Format("The file {0} referred to by {1} does not exist.", filename, key);

						//System.Diagnostics.Trace.WriteLine(".." + s + " Default calendar '" + defaultCalendar.Name + "' is used");
						// throw new ConfigurationException(s);
					}
					else
					{
						try
						{
							calendar = ReadCalendar(filename);
						}
						catch (Exception ex)
						{
							System.Diagnostics.Trace.WriteLine(".." + ex.ToString() + " is suppressed. Default calendar '" +
								defaultCalendar.Name + "' is used");
						}
					}
				}

				//System.Diagnostics.Trace.WriteLine("Time.ReadCalendar. Exit");

				return calendar;
			}

			public static RegionalCalendar UserCalendar
			{
				get
				{
					return ReadCalendar("TZ_LOCAL", RegionalCalendar.Current);
				}
			}

			public static RegionalCalendar StandardCalendar
			{
				get
				{
					return ReadCalendar("TZ_STANDARD", RegionalCalendar.CurrentNT);
				}
			}

			public static RegionalCalendar DatabaseCalendar
			{
				get
				{
					return m_rcDatabase ?? (m_rcDatabase = RegionalCalendar.GetCalendar(RegionalCalendar.dbName));
				}
			}
		}

		public sealed class Data
		{
			private static Dictionary<int,string> connectionStrings = new Dictionary<int, string>();
			private static DateTime dictionaryLifetime = DateTime.MinValue;
			private static object lockObject = new object();
			static string connectionString = null;
			static string clientUser = null;
			static string nlsLang = null;
            static int externalAuthenticationLicense = -1; //0 = No, 1 = Yes, -1 = Unknown

            public static int ProgramKey { get; set; } = 0;

            public static string User
			{
				get
				{
					//TOSA if (IccConfiguration.iccApplication != null && iccApplication.Username != null)
					//	return iccApplication.Username;

                    if (ExternalAuthenticationLicense)
						return ConfigurationReader.ReadOptionalString("ICC_DBUSER", null);

                    return ConfigurationReader.ReadString("ICC_DBUSER");
				}
			}

			public static string Password
			{
				get
				{ 
					//TOSA if (IccConfiguration.iccApplication != null && iccApplication.Password != null)
					//	return iccApplication.Password;
					if (ExternalAuthenticationLicense)
						return ConfigurationReader.ReadOptionalString("ICC_DBPASSWD", null);
					return ConfigurationReader.ReadString("ICC_DBPASSWD");
				}
			}

			public static string Server
			{
				get
				{
					//TOSA if (IccConfiguration.iccApplication != null && iccApplication.Server != null)
					//	return iccApplication.Server;
                    return ConfigurationReader.ReadString("LOCAL");
                }
			}

			public static bool ExternalAuthenticationLicense
			{
				get
				{
					if (externalAuthenticationLicense == -1)
						externalAuthenticationLicense = ConfigurationReader.ReadOptionalBoolean("SMG_EXTERNAL_AUTH", false) ? 1 : 0;
					return externalAuthenticationLicense == 1;
				}
			}

			public static string OSUser
			{
				// Returning the configured value, which typically contains the value set by Windows
				// in the system environment variable with same name.
				// Note that the value as set by Windows shows the logged in user which do not reflect
				// thread impersonation etc, and also consider that the value can easily be manipulated
				// so for a more accurate value consider using System.Environment.UserName instead.
				get { return ConfigurationReader.ReadString("USERNAME"); }
			}

			public static string ClientUser
			{
				// The identity of the real client user, which may or may not correspond to the OS user.
				// E.g. for a service the client may inform the service about the client user, to get
				// auditing based on this instead of the OS user running the service host process.
				get
				{
					if (clientUser == null)
					{
						// If no value has been set, then fall-back to using a value stored in the
						// current process' environment - if any.
						// Note that we do not currently read any value set in configuration system, since the
						// intention is not to pre-configure an identity. We are just in some cases temporarily
						// storing the value in the process environment to be able to pass it on to spawned processes.
						// The primary use case for this is the GenericHost process which may spawn processes due
						// to actions on steps in Nimbus (see SimService::Init, and it's helper function SetProcessEnvironmentVariables).
						// But note also that although native applications do the same, they will have all configuration
						// variables loaded into the process environment, so for native applications it is possible
						// to configure a specific identity to be used in ConfigurationSystem. So maybe we should
						// do the same here, to be consistent? And possible it might be a feature that we actually want..?
						clientUser = Environment.GetEnvironmentVariable("ICC_CLIENTUSER"); // ConfigurationReader.ReadOptionalString("ICC_CLIENTUSER", null);
						if (clientUser == null) {
							// If no environment variable is set then final fall-back is to use the name
							// of the user associated with the current process - the OS user. Or in fact it
							// is the name of the user, or other security principal, associated with the current
							// thread, which e.g. takes impersonating into account.
							clientUser = Environment.UserDomainName + "\\" + Environment.UserName; // NB: Uses properties from System.Environment, not configuration variables typically with value from environment variables %USERNAME% and %USERDOMAIN% since those do not consider thread impersonation etc (and can be easily manipulated, although that can now be done by the ICC_CLIENTUSER variable anyway). This is the same way we do it in native code as well, so it is consistent!
						}
					}
					return clientUser;
				}
				set
				{
					// Explicit set a value to be used. Used e.g. by WCF services to set the value of
					// the client identity from the ServiceSecurityContext.
					clientUser = value;
				}
			}

			public static string NlsLang
			{
				get
				{
					if (nlsLang == null)
						nlsLang = ConfigurationReader.ReadString("NLS_LANG");
					return nlsLang;
				}
				set
				{
					nlsLang = value;
				}
			}

			public static string Language
			{
				get
				{
					return NlsLang.Split('_')[0];
				}
			}

			public static string Territory
			{
				get
				{
					return NlsLang.Split('_')[1].Split('.')[0];
				}
			}

		    // Customize connection pooling in ODP.NET. The default in ODP.NET 11g is to have connection pooling enabled,
		    // with pool size between 1 and 100 connections etc.
		    // Sample custom configuration:
		    //    "Pooling=true;Min Pool Size=2;Max Pool Size=10;Connection Lifetime=120;Incr Pool Size=2;Decr Pool Size=10"
			public static string OraclePoolingOptions => 
			    ConfigurationReader.ReadOptionalString("ICC_DBPOOLOPTIONS", "Pooling=true;");

			public static string OracleConnectionString
			{
				get
				{
					if (!string.IsNullOrEmpty(connectionString))
					{
						return connectionString;
					}

				    if (string.IsNullOrEmpty(Server))
					{
					    //Missing required connection data 
						return "";
					}

				    if (ExternalAuthenticationLicense && 
						       string.IsNullOrEmpty(User) && 
						       string.IsNullOrEmpty(Password))
					{
					    //No username/password - assuming external authentication
						return $"User Id=/;Password=;Data Source={Server};{OraclePoolingOptions}";
					}
				    
				    //Regular database (password) authentication (throws exception if username or password is missing)
					return $"User Id={User};Password=\"{Password}\";Data Source={Server};{OraclePoolingOptions}";
				}
				set => connectionString = value;
			}

			public static void ClearConnectionStringCache()
			{
				lock (lockObject)
				{
					connectionStrings.Clear();
				}
			}

            public static string GetOracleConnectionStringByRole(int role)
            {
                var conStr = string.Empty;

                if (role > -1)
                {
                    lock (lockObject)
                    {
                        // Reset dictionary or not
                        if (dictionaryLifetime < DateTime.UtcNow)
                        {
                            connectionStrings.Clear();
                        }

                        // Check if connection string exists in dictionary or not
                        if (connectionStrings.ContainsKey(role))
                        {
                            conStr = connectionStrings[role];
                        }
                        else
                        {
                            string machineName = System.Environment.MachineName;
                            string progName =
                                System.IO.Path.GetFileName(System.Reflection.Assembly.GetExecutingAssembly().Location);
                            Dictionary<string, string> parameters = null;
                            var csr = new ConfigurationSystemReaderCLIWrapper();
                            csr.GetExpandedConfigurationByRole(role, machineName, progName, out parameters);

                            string user = "";
                            string password = "";
                            string server = "";
                            string poolingOptions = "";
                            bool externalAuth = false;

                            if ((parameters != null) && (parameters.Count > 0))
                            {
                                user = (parameters.ContainsKey("ICC_DBUSER")) ? parameters["ICC_DBUSER"] : User;
                                password = (parameters.ContainsKey("ICC_DBPASSWD")) ? parameters["ICC_DBPASSWD"] : Password;
                                server = (parameters.ContainsKey("LOCAL")) ? parameters["LOCAL"] : Server;
                                poolingOptions = (parameters.ContainsKey("ICC_DBPOOLOPTIONS")) ? parameters["ICC_DBPOOLOPTIONS"] : OraclePoolingOptions;
                                externalAuth = (parameters.ContainsKey("SMG_EXTERNAL_AUTH")) ? (parameters["SMG_EXTERNAL_AUTH"].ToUpper() == "TRUE") : false;
                            }
                            else
                            {
                                user = User;
                                password = Password;
                                server = Server;
                                poolingOptions = OraclePoolingOptions;
                                externalAuth = ExternalAuthenticationLicense;
                            }

                            // Set life time when the first item is added
                            if (connectionStrings.Count < 1)
                            {
                                dictionaryLifetime = DateTime.UtcNow.AddHours(1);
                            }

                            // Build connection string ()
                            if (string.IsNullOrEmpty(Server)) 
                            {
                                // Missing required connection data
                                conStr = string.Empty;
                            }
                            else if (externalAuth && string.IsNullOrEmpty(user) && string.IsNullOrEmpty(password)) 
                            {
                                // No username/password - assuming external authentication
                                conStr = String.Format("User Id=/;Password=;Data Source={0};{1}", server, poolingOptions);
                            }
                            else 
                            {
                                // Regular database (password) authentication (throws exception if username or password is missing)
                                conStr = String.Format("User Id={0};Password=\"{1}\";Data Source={2};{3}", user, password, server, poolingOptions);
                            }

                            // Only add connection string to cache if it is nonempty
                            if (!string.IsNullOrEmpty(conStr))
                            {
                                connectionStrings[role] = conStr;
                            }
                        }
                    }
                }
                else
                {
                    conStr = OracleConnectionString;
                }

                return conStr;
            }
		}

	    public sealed class PDMail
	    {
            public static string SmtpServer => ConfigurationReader.ReadString("PDMAIL_SMTP_SERVER");
            public static string SenderName
            {
                get { return ConfigurationReader.ReadOptionalString("PDMAIL_SENDER_NAME","PDMail Powel AS"); }
            }
            public static string SenderEmail => ConfigurationReader.ReadString("PDMAIL_SENDER_EMAIL");
            public static int PortNumber => ConfigurationReader.ReadOptionalInt32("PDMAIL_SMTP_PORTNUMBER", 25); // 25: SMTP; 465: SMTP/SSL; 587: SMTP/TLS 

            public static bool EnableSSL
            {
                get { return ConfigurationReader.ReadOptionalBoolean("PDMAIL_SMTP_ENABLESSL", false); }
            }

            public static string Password
            {
                get { return ConfigurationReader.ReadOptionalString("PDMAIL_SENDER_PASSWORD", String.Empty); }
            }

	        public static bool Anonymous => ConfigurationReader.ReadOptionalBoolean("PDMAIL_SMTP_ANONYMOUS", true);
	    }

	    public sealed class WeatherUtility 
	    {

	        public static bool HasWeatherUtilityMepsLicense => ConfigurationReader.ReadOptionalBoolean("ICC_WEATHER_UTILITY_MEPS_LICENSE", false);
	    }

        public sealed class ImportExport
		{
			public static string IccExportDir
			{
				get { return ConfigurationReader.ReadString("ICC_EXPORT_DIR"); }
			}

			public static string IccImportDir
			{
				get { return ConfigurationReader.ReadString("ICC_IMPORT_DIR"); }
			}

			public static int IccSender
			{
				get { return ConfigurationReader.ReadInt32("ICC_SENDER"); }
			}

			public static string EdiCountry
			{
				get { return ConfigurationReader.ReadOptionalString("EDI_COUNTRY", ""); }
			}

			public static string IccTranslogDir
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_TRANSLOG_DIR", ""); }
			}

            public static bool IccExpressMessaging
	        {
                get { return ConfigurationReader.ReadOptionalBoolean("ICC_EXPRESS_MESSAGING", false); }
            }

            public static string IccDataExchangeManagerImportQueue
            {
                get { return ConfigurationReader.ReadOptionalString("ICC_DATAEXCHANGEMANAGER_IMPORTQUEUE", ""); }
            }

            public static string IccDataExchangeManagerExportQueue
            {
                get { return ConfigurationReader.ReadOptionalString("ICC_DATAEXCHANGEMANAGER_EXPORTQUEUE", ""); }
            }

            public static string IccDataExchangeManagerClaimHandler
            {
                get { return ConfigurationReader.ReadOptionalString("ICC_DATAEXCHANGEMANAGER_CLAIMHANDLER", @"LocalFileshareClaimHandler"); }
            }

            public static string IccLocalFileshareDir
            {
                get { return ConfigurationReader.ReadOptionalString("ICC_LOCALFILESHARE_DIR", IccTranslogDir + @"\DEM"); }
            }

            public static string IccCloudBlobUrlAndSas
            {
                get { return ConfigurationReader.ReadOptionalString("ICC_CLOUDBLOB_URLANDSAS", ""); }
            }

            public static string ServiceBusFtpUri => ConfigurationReader.ReadOptionalString("ICC_SERVICEBUS_FTP_URI", null);
            public static string ServiceBusFtpUser => ConfigurationReader.ReadOptionalString("ICC_SERVICEBUS_FTP_USER", null);
            public static string ServiceBusFtpPassword => ConfigurationReader.ReadOptionalString("ICC_SERVICEBUS_FTP_PASSWORD", null);
        }

        public sealed class Alarm
		{
			public class Email
			{
				public static string From
				{
					get { return ConfigurationReader.ReadString("ICC_ALARM_EMAIL_FROM"); }
				}

				public static string Subject
				{
					get { return ConfigurationReader.ReadOptionalString("ICC_ALARM_EMAIL_SUBJECT","Alarm!"); }
				}

				public static string BodyFooter
				{
					get { return ConfigurationReader.ReadOptionalString("ICC_ALARM_EMAIL_BODYFOOTER", "Best regards Powel Alarm Service"); }
				}

				public sealed class Smtp
				{
					public static string Server => ConfigurationReader.ReadOptionalString("ICC_ALARM_EMAIL_SMTP_SERVER",PDMail.SmtpServer);
					public static int Port => ConfigurationReader.ReadOptionalInt32("ICC_ALARM_EMAIL_SMTP_PORT",PDMail.PortNumber);
                    public static string Username => ConfigurationReader.ReadOptionalString("ICC_ALARM_EMAIL_SMTP_USERNAME", From);
                    public static string Password => ConfigurationReader.ReadOptionalString("ICC_ALARM_EMAIL_SMTP_PASSWORD", string.Empty);
                    public static bool EnableSsl => ConfigurationReader.ReadOptionalBoolean("ICC_ALARM_EMAIL_SMTP_ENABLESSL", PDMail.EnableSSL);
				}
			}
		}

		public sealed class Replication
		{
			public static string User
			{
				get { return ConfigurationReader.ReadString("ICC_REPLICATION_DBUSER"); }
			}

			public static string Password
			{
				get { return ConfigurationReader.ReadString("ICC_REPLICATION_DBPASSWD"); }
			}

			public static string Server
			{
				get { return ConfigurationReader.ReadString("ICC_REPLICATION_SERVER"); }
			}
			public static string OraclePoolingOptions {
				get {
					//Pooling=true;Min Pool Size=2;Max Pool Size=10;Connection Lifetime=120;Incr Pool Size=2;Decr Pool Size=10"
					return ConfigurationReader.ReadOptionalString("ICC_REPLICATION_DBPOOLOPTIONS", "Pooling=true;");
				}
			}
			public static string OracleConnectionString
			{
				get 
				{
					return String.Format(
							"User Id={0};Password=\"{1}\";Data Source={2};{3}",
					User, Password, Server, OraclePoolingOptions);
				}
			}

			public static int PollingSeconds
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalInt32(
						"ICC_REPLICATION_POLLING_SECONDS", 60); 
				}
			}

			public static int StatusMessageMinutes
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalInt32(
						"ICC_REPLICATION_STATUS_MESSAGE_MINUTES", 60); 
				}
			}

			public static int MaxBatchSize
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalInt32(
						"ICC_REPLICATION_MAX_BATCH_SIZE", 25); 
				}
			}

			public static bool AutoCreateExportedSeries
			{
				get
				{
					return ConfigurationReader.ReadOptionalBoolean(
						"ICC_REPLICATION_AUTO_CREATE_EXPORTED_SERIES", false); 
				}
			}

			public static bool ImportFlexibleManning
			{
				get 
				{
					return ConfigurationReader.ReadOptionalBoolean(
						"ICC_REPLICATION_IMPORT_FLEXIBLE_MANNING", false); 
				}
			}

			public static bool ExportFlexibleManning
			{
				get
				{
					return ConfigurationReader.ReadOptionalBoolean(
						"ICC_REPLICATION_EXPORT_FLEXIBLE_MANNING", false);
				}
			}

            public static bool ImportTsValues
            {
                get
                {
                    return ConfigurationReader.ReadOptionalBoolean(
                        "ICC_REPLICATION_IMPORT_TS_VALUES", true);
                }
            }

            public static int ReplstatusLocalToBeExported
            {
                get
                {
                    return ConfigurationReader.ReadOptionalInt32(
                        "ICC_REPLICATION_LOCAL_REPLSTATUS_TB_EXPORTED", 0);
                }
            }

            public static int ReplstatusLocalExported
            {
                get
                {
                    return ConfigurationReader.ReadOptionalInt32(
                        "ICC_REPLICATION_LOCAL_REPLSTATUS_EXPORTED", 1);
                }
            }
            public static int ReplstatusRemoteToBeExported
            {
                get
                {
                    return ConfigurationReader.ReadOptionalInt32(
                        "ICC_REPLICATION_REMOTE_REPLSTATUS_TB_EXPORTED", 0);
                }
            }

            public static int ReplstatusRemoteExported
            {
                get
                {
                    return ConfigurationReader.ReadOptionalInt32(
                        "ICC_REPLICATION_REMOTE_REPLSTATUS_EXPORTED", 1);
                }
            }
            public static bool ExportTsValues
            {
                get
                {
                    return ConfigurationReader.ReadOptionalBoolean(
                        "ICC_REPLICATION_EXPORT_TS_VALUES", true);
                }
            }


			public static bool ImportFromOracle9
			{
				get
				{
					return ConfigurationReader.ReadOptionalBoolean(
						"ICC_REPLICATION_IMPORT_FROM_ORACLE_9", false);
				}
			}

			public static bool ExportFromOracle9
			{
				get
				{
					return ConfigurationReader.ReadOptionalBoolean(
						"ICC_REPLICATION_EXPORT_FROM_ORACLE_9", false);
				}
			}

			public static bool ForcedGC {
				get {
					return ConfigurationReader.ReadOptionalBoolean(
						"ICC_REPLICATION_FORCED_GC", false);
		        }
			}
			public static bool UpdateSkippedStatus {
				get {
					return ConfigurationReader.ReadOptionalBoolean("ICC_REPLICATION_UPDATE_SKIPPED_STATUS", false);
				}
			}
			public static int SystemNo
            {
				get {
					return ConfigurationReader.ReadOptionalInt32("ICC_REPLICATION_SYSTEM_NUMBER", 1);
                }
            }
		}
		
		public sealed class Messaging
		{
			public static string LogPath
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalString(
						"ICC_MESSAGING_LOG_PATH", null); 
				}
			}

			public static int PollingSeconds
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalInt32(
						"ICC_MESSAGING_POLLING_SECONDS", 5); 
				}
			}

			public static int StatusMessageMinutes
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalInt32(
						"ICC_MESSAGING_STATUS_MESSAGE_MINUTES", 60); 
				}
			}

			public static string TimeSeriesResponseUrl
			{
				get 
				{ 
					return ConfigurationReader.ReadString(
						"ICC_MESSAGING_TIME_SERIES_RESPONSE_URL");
				}
			}

			public static string MeteringSystemServiceUrl
			{
				get 
				{ 
					return ConfigurationReader.ReadString(
						"ICC_MESSAGING_METERING_SYSTEM_SERVICE_URL");
				}
			}

			public static string MeteringSystemServiceDiskDump
			{
				get
				{
					return ConfigurationReader.ReadString(
						"ICC_MESSAGING_METERING_SYSTEM_SERVICE_DISK_DUMP");
				}
			}

			public static string MeteringSystemServiceDiskDumpPath
			{
				get
				{
					return ConfigurationReader.ReadString(
						"ICC_MESSAGING_METERING_SYSTEM_SERVICE_DISK_DUMP_PATH");
				}
			}

			public static string GS2To
			{
				get 
				{ 
					return ConfigurationReader.ReadString("ICC_MESSAGING_GS2_TO");
				}
			}

			public static string GS2From
			{
				get 
				{ 
					return ConfigurationReader.ReadString("ICC_MESSAGING_GS2_FROM");
				}
			}

			public static string GS2UnprocessedFilePath
			{
				get 
				{ 
					return ConfigurationReader.ReadString("ICC_MESSAGING_GS2_UNPROCESSED_FILE_PATH");
				}
			}

			public static string GS2ProcessedFilePath
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalString("ICC_MESSAGING_GS2_PROCESSED_FILE_PATH", null);
				}
			}

			public static string GS2ReceiverUrl
			{
				get 
				{ 
					return ConfigurationReader.ReadString("ICC_MESSAGING_GS2_RECEIVER_URL");
				}
			}
			public static string TimeSeriesReceiverUrl
			{
				get
				{
					return ConfigurationReader.ReadString("ICC_MESSAGING_TIMESERIES_RECEIVER_URL");
				}
			}
			public static int LogsAgeInMinutes
			{
				get
				{
					return ConfigurationReader.ReadOptionalInt32(
						"ICC_MESSAGING_SERVICE_LOG_AGE", 1440);
				}
			}
			public static int LogSendFailureThreshold
			{
				get
				{
					return ConfigurationReader.ReadOptionalInt32(
						"ICC_MESSAGING_LOG_SEND_FAILURE_THRESHOLD", 1000);
				}
			}
			public static string RegisterTemplate
			{
				get
				{
					return ConfigurationReader.ReadOptionalString("ICC_MESSAGING_REGISTER_TEMPLATE","");
				}
			}
			public static string MeteringServiceResponseUrl
			{
				get
				{
					return ConfigurationReader.ReadString("ICC_MESSAGING_METERING_SERVICE_RESPONSE_URL");
				}
			}
		}
		
		public sealed class Trace
		{
			public static string File
			{
				get { return ConfigurationReader.ReadString("ICC_TRACE_FILE"); }
			}
		}
		public sealed class Component 
		{
			public static string IccCompCompanies
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_COMP_COMPANIES", ""); }
			}
			public static string IccCompSignatures
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_COMP_SIGNATURES", ""); }
			}
			public static string IccCompLocations
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_COMP_LOCATIONS", ""); }
			}
			public static string IccInstrRegRole
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_INSTRREG_ROLE", ""); }
			}
			public static string IccCompRegSpotCheckLicense
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_COMPREG_SPOT_CHECK_LICENSE", ""); }
			}
			public static string IccInstrOwner
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_INSTR_OWNER", ""); }
			}
			public static string MasterKeyMeter
			{
				get{ return ConfigurationReader.ReadString("ICC_DEF_MASTER_METER");}
			}
			public static string MasterKeyTerminal
			{
				get{ return ConfigurationReader.ReadString("ICC_DEF_MASTER_TERMINAL");}
			}
			public static string MasterKeyTransformerCurrent
			{
				get{ return ConfigurationReader.ReadString("ICC_DEF_MASTER_TRANSFORMER_CURRENT");}
			}
			public static string MasterKeyTransformerVoltage
			{
				get{ return ConfigurationReader.ReadString("ICC_DEF_MASTER_TRANSFORMER_VOLTAGE");}
			}
		}

		public sealed class WorkOrder 
		{
			public static string ExpFormat
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_WO_EXP_FORMAT", ""); }
			}
			public static string ClientWebServer
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_WO_CLIENT_WEB_SERVER", ""); }
			}
			public static string ClientApplName
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_WO_CLIENT_APPL_NAME", ""); }
			}
			public static string ClientGroupName
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_WO_CLIENT_GROUP_NAME", ""); }
			}
			public static string EnableConvHtXmlImp
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_WO_ENABLE_CONV_HT_XML_IMP", ""); }
			}
			public static bool CreateVerificationFile
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_WO_CREATE_VERIFICATION_FILE", "TRUE").ToUpper()=="TRUE"; }
			}
		}

		public sealed class Globalization
		{
			public static string Language
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_LANGUAGE", ""); }
			}

			public static CultureInfo LanguageCulture
			{
				get { return Culture.LanguageToCulture(Language); }
			}
		}

		public sealed class Restrictions
		{
			public static bool UsePeriodCheck
			{
				get { return ConfigurationReader.ReadOptionalBoolean("ICC_RESTRICTION_PERIOD_CHECK", false); }
			}
			public static int DefaultEndPeriod
			{
				get { return ConfigurationReader.ReadOptionalInt32("ICC_AVAILBILITY_DEFAULT_END_PERIOD", 7); }
			}
			public static int DefaultGatePriority
			{
				get { return ConfigurationReader.ReadOptionalInt32("ICC_SIM_DEFAULT_GATE_PRIORITY", 999); }
			}
			public static int DefaultGenPriority
			{
				get { return ConfigurationReader.ReadOptionalInt32("ICC_SIM_DEFAULT_GEN_PRIORITY", 999); }
			}
		}

		public sealed class Scada
		{
			public static bool ExportEnabled
			{
				get { return ConfigurationReader.ReadOptionalBoolean("ICC_SCADA_EXPORT", false); }
			}

			public static string ExportStart
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_EXPORT_SCADA_START", null); }
			}

			public static string ExportEnd
			{
				get { return ConfigurationReader.ReadOptionalString("ICC_EXPORT_SCADA_END", null); }
			}
        }

        public sealed class Metering
		{
			public static int AccumulativeMeteringOrderOffset
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalInt32(
						"ICC_METERING_ACCUMULATIVE_ORDER_OFFSET_HOURS", 12);
				}
			}

			public static int AccumulativeMeteringOrderDuration
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalInt32(
						"ICC_METERING_ACCUMULATIVE_ORDER_DURATION_HOURS", 72);
				}
			}

			public static string AccumulativeMeteringOrderIntervalStartTime_Daily
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalString(
						"ICC_METERING_ACCUMULATIVE_ORDER_INTERVAL_START_DAILY", "20060901000000000");
				}
			}

			public static string AccumulativeMeteringOrderIntervalStartTime_Weekly
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalString(
						"ICC_METERING_ACCUMULATIVE_ORDER_INTERVAL_START_WEEKLY", "20060904000000000");
				}
			}

			public static string AccumulativeMeteringOrderIntervalStartTime_Monthly
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalString(
						"ICC_METERING_ACCUMULATIVE_ORDER_INTERVAL_START_MONTHLY", "20060901000000000");
				}
			}

			public static string AccumulativeMeteringOrderIntervalStartTime_Yearly
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalString(
						"ICC_METERING_ACCUMULATIVE_ORDER_INTERVAL_START_YEARLY", "20060101000000000");
				}
			}

			public static string EanNumberPrefix
			{
				get 
				{ 
					return ConfigurationReader.ReadOptionalString(
						"ICC_EAN_PREFIXES", "7070575000");
				}
			}

			public static string MeterKey
			{
				get 
				{ 
					return ConfigurationReader.ReadString(
						"ICC_COMP_METER_KEY");
				}
			}

			public static int TimeSeriesValidationOnSave(string hasMvsLicence)
			{
				if (hasMvsLicence.Equals("FALSE"))
				{
					return 0;
				}
				return ConfigurationReader.ReadOptionalInt32(
						"ICC_TS_VALIDATE_ON_SAVE", 0);
			}
		}

		public sealed class Trade
		{
            public static int DefaultOperator => ConfigurationReader.ReadInt32("TRD_DEFAULT_OPERATOR");
			public static string EdkFilesDir
			{
				get { return ConfigurationReader.ReadString("EDK_FILES_DIR"); }
			}

		    public sealed class ActivationRequest
		    {
                public static int MaximumNumberOfItemsInHistoryList
                {
                    get { return ConfigurationReader.ReadOptionalInt32("ICC_AR_MAX_ITEMS_IN_HISTORY", 10); }
                }
		    }

            public sealed class NpAuction
            {
                public sealed class Requester
                {
                    public static string Priorities => ConfigurationReader.ReadOptionalString("TRD_NPAUCTION_REQUESTER_PRIORITIES", "NewAuction:Low,GetContract:Normal");
                    public static string MarketCode => ConfigurationReader.ReadOptionalString("TRD_NPAUCTION_REQUESTER_MARKET_CODE", "ELSPOT_H");
                    public static string RoutingAddress => ConfigurationReader.ReadOptionalString("TRD_NPAUCTION_REQUESTER_ROUTING_ADDRESS", "NpAuction:");
                    public static string Operators => ConfigurationReader.ReadOptionalString("TRD_NPAUCTION_REQUESTER_OPERATORS",null); // List of comma separated operator keys (OPUN_KEY)
                    public static int Receiver => ConfigurationReader.ReadInt32("TRD_NPAUCTION_REQUESTER_RECEIVER");  // Receivers/Nord Pool operator key (OPUN_KEY)
                }
            }
		}

        public sealed class DataExchangeManager
        {
            public static string MsmqMachineExport => ConfigurationReader.ReadOptionalString("ICC_MSMQ_MACHINE_EXPORT",null);
            public static string MsmqMachineImport => ConfigurationReader.ReadOptionalString("ICC_MSMQ_MACHINE_IMPORT", null);
            public static bool MsmqStoreExportMessages => ConfigurationReader.ReadOptionalBoolean("ICC_MSMQ_STORE_EXPORT_MESSAGES", false);
            public static string DefaultRoutingAddress => ConfigurationReader.ReadOptionalString("ICC_DEFAULT_ROUTING_ADDRESS", null);
            public sealed class Mades
            {
                public static string EndpointConfigurationName(string Default) => ConfigurationReader.ReadOptionalString("ICC_MADES_ENDPOINT", Default);
                public static int ImportIdleWait(int Default) => ConfigurationReader.ReadOptionalInt32("ICC_MADES_IMPORT_IDLEWAIT", Default);
                public static string[] HeartbeatReceivers(string Default) => ConfigurationReader.ReadOptionalString("ICC_MADES_HEARTBEAT_RECEIVERS", Default).Split(new char[] {','});
                public static string[] HeartbeatBusinessTypes(string Default) => ConfigurationReader.ReadOptionalString("ICC_MADES_HEARTBEAT_BUSINESSTYPES", Default).Split(new char[] { ',' });
                public static int HeartbeatInterval(int Default) => ConfigurationReader.ReadOptionalInt32("ICC_MADES_HEARTBEAT_INTERVAL", Default);
                public static int RestartInterval(int Default) => ConfigurationReader.ReadOptionalInt32("ICC_MADES_RESTART_INTERVAL", Default);
                public static string[] ImportBusinessTypes(string Default)
                {
                    return ConfigurationReader.ReadOptionalString("ICC_MADES_IMPORT_BUSINESSTYPES", Default)
                        .Split(new char[] {','});
                }

                public static bool ImportBusinessTypesAddAll(bool Default) => ConfigurationReader.ReadOptionalBoolean("ICC_MADES_IMPORT_BUSINESSTYPES_ADD_ALL", Default);

                public static bool RunModule(bool Default)
                {
                    return ConfigurationReader.ReadOptionalBoolean("ICC_MADES_RUN_MODULE", Default);
                }
            }

            public static bool RunAzureModule(bool Default)
            {
                return ConfigurationReader.ReadOptionalBoolean("ICC_AZURE_RUN_MODULE", Default);
            }
        }
	}
}
