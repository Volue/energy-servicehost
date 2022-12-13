/*
$Header$
*/
using System;
using System.Data;
using Oracle.ManagedDataAccess.Client;
using Powel.Icc.Interop;

namespace Powel.Icc.Data
{
	/// <summary>
	/// This class is similar in structure to IccConfiguration.
	/// But instead of encapsulating environment variables, it encapsulates
	/// state stored in the database, in the tables pddbenv, dbenv_common
	/// and dbenv_user. 
	/// pddbenv is used for read-only system settings, dbenv_common is for
	/// read/write system settings and dbenv_user is for user specific
	/// settings.
	/// </summary>
	public sealed class DbConfiguration
	{
		sealed class DbConfigurationHandler
		{
			string tableName;

			public DbConfigurationHandler(string tableName)
			{
				this.tableName = tableName;
			}

			public string ReadOptionalString(string key, string defaultValue)
			{
				try
				{
					return ReadString(key);
				}
				catch
				{
					return defaultValue;
				}
			}

			public int ReadOptionalInt32(string key, int defaultValue)
			{
				try
				{
					return ReadInt32(key);
				}
				catch
				{
					return defaultValue;
				}
			}

			public decimal ReadOptionalDecimal(string key, 
				decimal defaultValue)
			{
				try
				{
					return ReadDecimal(key);
				}
				catch
				{
					return defaultValue;
				}
			}

			public bool ReadOptionalBoolean(string key, bool defaultValue)
			{
				try
				{
					return ReadBoolean(key);
				}
				catch
				{
					return defaultValue;
				}
			}

			public string ReadString(string key)
			{
				return (string)Read(key, 'C');
			}

			public int ReadInt32(string key)
			{
				return Convert.ToInt32(Read(key, 'I'));
			}

			public decimal ReadDecimal(string key)
			{
				return Convert.ToDecimal(Read(key, 'F'));
			}

			public double ReadDouble(string key)
			{
				return Convert.ToDouble(Read(key, 'F'));
			}

			bool ReadBoolean(string key)
			{
				object o = Read(key, 'B');

			    var s = o.ToString().ToUpper();
                return s != "FALSE" && s != "0";
			}

			object Read(string key, char type)
			{
				string sql = String.Format(
					"SELECT {0} FROM {1} WHERE ENVTYPE = '{2}' AND NAME = :1", 
					GetColumnName(type), tableName, type);

				OracleCommand cmd = new OracleCommand(sql);
				cmd.Parameters.Add(null, key);

				var o = Util.CommandToScalar(cmd);

			    if (o == null)
#if DEBUG
                    //SIV. ConfigurationException takes very long time in debugging if not caught and handled properly.
                    //Not quite sure that this is the right place to fix it, but anyway:
                    o = "";
#else
                    throw new ConfigurationException(String.Format(
						"The database configuration parameter '{0}' is unknown.", 
						key));
#endif

				return o;
			}

			public void Write(string key, string value)
			{
				Write (key, 'C', value);
			}

			public void Write(string key, int value)
			{
				Write (key, 'I', value);
			}

			public void Write(string key, decimal value)
			{
				Write (key, 'F', value);
			}

			public void Write(string key, bool value)
			{
				Write(key, 'B', value ? 1 : 0);
				//Write(key, 'B', value ? "TRUE" : "FALSE");
			}

			void Write(string key, char type, object value)
			{
				string sql = String.Format(
					@"declare envcount number;
begin
select count(*) into envcount from {0} where name = :1;
if envcount > 0 then
	update {0} set {2} = :2 where name = :3;
else
	insert into {0} (name, envtype, {2}) values (:4, '{1}', :5);
end if;
end;", tableName, type, GetColumnName(type));

/*				int valueBool = -1;
				if(type=='B')
					valueBool = value?1:0;
				*/

				OracleCommand cmd = new OracleCommand(sql);
				cmd.Parameters.Add(null, key);
				cmd.Parameters.Add(null, value);
				cmd.Parameters.Add(null, key);
				cmd.Parameters.Add(null, key);
				cmd.Parameters.Add(null, value);

				Util.ExecuteCommand(cmd);
			}

			string GetColumnName(char type)
			{
				if (tableName.Equals("pddbenv"))
				{
					return "value";
				}
				else
				{
					switch (type)
					{
						case 'C':
							return "cval";
						case 'I':
							return "ival";
						case 'F':
							return "rval";
						case 'B':
							return "ival";
						default:
							throw new ApplicationException("Unknown data type.");
					}
				}
			}
		}

		static DbConfigurationHandler systemHandler = 
			new DbConfigurationHandler("pddbenv");

		static DbConfigurationHandler commonHandler = 
			new DbConfigurationHandler("dbenv_common");

		static DbConfigurationHandler userHandler = 
			new DbConfigurationHandler("dbenv_user");

		DbConfiguration()
		{
		}

		public sealed class Component 
		{
			static string iccCompMeterKey = "zz";
			static string iccCompTermKey = "zz";
			static string iccCompCurrTrafoKey = "zz";
			static string iccCompVoltTrafoKey = "zz";
			static string iccCompConsKey = "zz";
			static string iccCompReptKey ="zz";
			static string iccCompModemKey = "zz";
			static string iccCompSimCardKey = "zz";
			static string iccCompAntennaKey = "zz";
			static string iccCompCableKey = "zz";
			static string iccCompFuseKey = "zz";
			static string iccCompTimeSwitchKey = "zz";
			static string iccCompExtSwitchKey = "zz";
			static string iccCompTempTransmitterKey = "zz";
			static string iccCompFlowTransmitterKey = "zz";
			static int maxInstrumentRows = Int32.MinValue;
			static int iccCompMeterStockReportMaxInFile = Int32.MinValue;
			static int iccCompMeterErrorReportMaxInFile = Int32.MinValue;

			public static bool IccRegListsLimited
			{
				get { return systemHandler.ReadOptionalBoolean("ICC_INSTRREG_REG_LISTS_LIMITED", false); }
			}
			public static string IccCompMeterKey
			{
				get
				{
					if (iccCompMeterKey == "zz") iccCompMeterKey = systemHandler.ReadOptionalString("ICC_COMP_METER_KEY", "");
					return iccCompMeterKey;
				}
			}
			public static string IccCompTermKey
			{
				get
				{
					if (iccCompTermKey == "zz") iccCompTermKey = systemHandler.ReadOptionalString("ICC_COMP_TERM_KEY", "");
					return iccCompTermKey;
				}
			}
			public static string IccCompCurrTrafoKey
			{
				get
				{
					if (iccCompCurrTrafoKey == "zz") iccCompCurrTrafoKey = systemHandler.ReadOptionalString("ICC_COMP_CURR_TRAFO_KEY", "");
					return iccCompCurrTrafoKey;
				}
			}
			public static string IccCompVoltTrafoKey
			{
				get
				{
					if (iccCompVoltTrafoKey == "zz") iccCompVoltTrafoKey = systemHandler.ReadOptionalString("ICC_COMP_VOLT_TRAFO_KEY", "");
					return iccCompVoltTrafoKey;
				}
			}
			public static string IccCompConsKey
			{
				get 
				{ 
					if (iccCompConsKey == "zz") iccCompConsKey = systemHandler.ReadOptionalString("ICC_COMP_CONS_KEY", ""); 
					return iccCompConsKey;
				}
			}
			public static string IccCompReptKey
			{
				get
				{
					if (iccCompReptKey == "zz") iccCompReptKey = systemHandler.ReadOptionalString("ICC_COMP_REPT_KEY", "");
					return iccCompReptKey;
				}
			}
			public static string IccCompModemKey
			{
				get
				{
					if (iccCompModemKey == "zz") iccCompModemKey = systemHandler.ReadOptionalString("ICC_COMP_MODEM_KEY", "");
					return iccCompModemKey;
				}
			}
			public static string IccCompSimCardKey
			{
				get
				{
					if (iccCompSimCardKey == "zz") iccCompSimCardKey = systemHandler.ReadOptionalString("ICC_COMP_SIM_CARD_KEY", "");
					return iccCompSimCardKey;
				}
			}
			public static string IccCompAntennaKey
			{
				get
				{
					if (iccCompAntennaKey == "zz") iccCompAntennaKey = systemHandler.ReadOptionalString("ICC_COMP_ANTENNA_KEY", "");
					return iccCompAntennaKey;
				}
			}
			public static string IccCompCableKey
			{
				get
				{
					if (iccCompCableKey == "zz") iccCompCableKey = systemHandler.ReadOptionalString("ICC_COMP_CABLE_KEY", "");
					return iccCompCableKey;
				}
			}
			public static string IccCompFuseKey
			{
				get
				{
					if (iccCompFuseKey == "zz") iccCompFuseKey = systemHandler.ReadOptionalString("ICC_COMP_FUSE_KEY", "");
					return iccCompFuseKey;
				}
			}
			public static string IccCompTimeSwitchKey
			{
				get
				{
					if (iccCompTimeSwitchKey == "zz") iccCompTimeSwitchKey = systemHandler.ReadOptionalString("ICC_COMP_TIME_SWITCH_KEY", "");
					return iccCompTimeSwitchKey;
				}
			}
			public static string IccCompExtSwitchKey
			{
				get
				{
					if (iccCompExtSwitchKey == "zz") iccCompExtSwitchKey = systemHandler.ReadOptionalString("ICC_COMP_EXT_SWITCH_KEY", "");
					return iccCompExtSwitchKey;
				}
			}

			public static string IccCompTempTransmitterKey
			{
				get
				{
					if (iccCompTempTransmitterKey == "zz") iccCompTempTransmitterKey = systemHandler.ReadOptionalString("ICC_COMP_TEMP_TRANSMITTER_KEY", "");
					return iccCompTempTransmitterKey;
				}
			}

			public static string IccCompFlowTransmitterKey
			{
				get
				{
					if (iccCompFlowTransmitterKey == "zz") iccCompFlowTransmitterKey = systemHandler.ReadOptionalString("ICC_COMP_FLOW_TRANSMITTER_KEY", "");
					return iccCompFlowTransmitterKey;
				}
			}

			public static string IccCompCollSysKey
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_COLL_SYS_KEY", ""); }
			}
			public static string IccCompCompanies
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_COMPANIES", ""); }
			}
			public static string IccCompSignatures
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_SIGNATURES", ""); }
			}
			public static string IccCompLocations
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_LOCATIONS", ""); }
			}
			public static string InstrRegReadingsOnHourMeterIsAuto
			{
				get { return systemHandler.ReadOptionalString("ICC_INSTRREG_READINGS_ON_HOUR_METER_IS_AUTO", ""); }
			}
			public static string OOInstrRegQ1
			{
				get { return systemHandler.ReadOptionalString("OO_INSTRREG_Q1", ""); }
			}
			public static string IccCompMeterTpKey
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_METER_TP_KEY", ""); }
			}
			public static string IccCompTermTpKey
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_TERM_TP_KEY", ""); }
			}
			public static string IccCompCurrTrafoTpKey
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_CURR_TRAFO_TP_KEY", ""); }
			}
			public static string IccCompVoltTrafoTpKey
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_VOLT_TRAFO_TP_KEY", ""); }
			}
			public static string IccCompConsTpKey
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_CONS_TP_KEY", ""); }
			}
			public static string IccCompCollSysTpKey
			{
				get { return systemHandler.ReadOptionalString("ICC_COMP_COLL_SYS_TP_KEY", ""); }
			}
			public static string IccInstrOwner
			{
				get { return systemHandler.ReadOptionalString("ICC_INSTR_OWNER", ""); }
			}
			public static int MaxInstrumentRows
			{
				get
				{
					if (maxInstrumentRows == Int32.MinValue) 
						maxInstrumentRows = systemHandler.ReadOptionalInt32(
							"ICC_MAX_INSTRUMENT_ROWS", -1);
					
					return maxInstrumentRows;
				}
			}
			public static int IccCompMeterStockReportMaxInFile
			{
				get
				{
					if (iccCompMeterStockReportMaxInFile == Int32.MinValue)
						iccCompMeterStockReportMaxInFile = systemHandler.ReadOptionalInt32(
							"ICC_COMP_METER_STOCK_REPORT_MAX_IN_FILE", -1);

					return iccCompMeterStockReportMaxInFile;
				}
			}
			public static int IccCompMeterErrorReportMaxInFile
			{
				get
				{
					if (iccCompMeterErrorReportMaxInFile == Int32.MinValue)
						iccCompMeterErrorReportMaxInFile = systemHandler.ReadOptionalInt32(
							"ICC_COMP_METER_ERROR_REPORT_MAX_IN_FILE", -1);

					return iccCompMeterErrorReportMaxInFile;
				}
			}
		}

		public sealed class WorkOrder 
		{
			static string expFormat = "zz";
			static string vipXpress = "zz";
			static string collSys = "zz";
			static bool vipXpressCisNoExpCurrTrafo = false;
			static string vipXpressCisNoExpCurrTrafoStr = "zz";
			static bool vipXpressCisNoExpHourReg = false;
			static string vipXpressCisNoExpHourRegStr = "zz";
			static bool vipXpressCisExpNewAndTerminate = false;
			static string vipXpressCisExpNewAndTerminateStr = "zz";
			static bool vipXpressMDMS = false;
			static string vipXpressMDMSStr = "zz";
			static bool updateMeasPntName = false;
			static string updateMeasPntNameStr = "zz";
			static bool updateMeasPntFusesz = false;
			static string updateMeasPntFuseszStr = "zz";
			static bool impNoCheckLocation = false;
			static string impNoCheckLocationStr = "zz";
			static string verificationInterval = "zz";
			static int maxWorkOrderRows = Int32.MinValue;
			static int expMaxInFile = Int32.MinValue;
			static string externalWOsystem = "zz";
			static string xmlVersion = "zz";
			static string docDirImp = "zz";
			static string docDirExp = "zz";
			static string spotCheckCountry = "zz";
			static string spotCheckLabId = "zz";
			static bool activateRegisters = false;
			static string activateRegistersStr = "zz";

			public static string ExpFormat
			{
				get
				{
					if (expFormat == "zz") expFormat = systemHandler.ReadOptionalString("ICC_WO_EXP_FORMAT", "");
					return expFormat;
				}
			}
			public static string VipXpress
			{
				get
				{
					if (vipXpress == "zz") vipXpress = systemHandler.ReadOptionalString("ICC_WO_VIP_XPRESS", "");
					return vipXpress;
				}
			}
			public static string CollSys
			{
				get 
				{
					if (collSys == "zz") collSys = systemHandler.ReadOptionalString("ICC_WO_COLL_SYS", "");
					return collSys;
				}
			}
			public static bool VipXpressCisNoExpCurrTrafo
			{
				get 
				{
					if (vipXpressCisNoExpCurrTrafoStr == "zz")
					{
						vipXpressCisNoExpCurrTrafo = systemHandler.ReadOptionalBoolean("ICC_WO_VIP_XPRESS_CIS_NO_EXP_CURR_TRAFO", false);
						vipXpressCisNoExpCurrTrafoStr = vipXpressCisNoExpCurrTrafo.ToString();
					}
					return vipXpressCisNoExpCurrTrafo;
				}
			}
			public static bool VipXpressCisNoExpHourReg
			{
				get 
				{
					if (vipXpressCisNoExpHourRegStr == "zz")
					{
						vipXpressCisNoExpHourReg = systemHandler.ReadOptionalBoolean("ICC_WO_VIP_XPRESS_CIS_NO_EXP_HOUR_REG", false);
						vipXpressCisNoExpHourRegStr = vipXpressCisNoExpHourReg.ToString();
					}
					return vipXpressCisNoExpHourReg;
				}
			}
			public static bool VipXpressCisExpNewAndTerminate
			{
				get 
				{
					if (vipXpressCisExpNewAndTerminateStr == "zz")
					{
						vipXpressCisExpNewAndTerminate = systemHandler.ReadOptionalBoolean("ICC_WO_VIP_XPRESS_CIS_EXP_NEW_AND_TERMINATE", false);
						vipXpressCisExpNewAndTerminateStr = vipXpressCisExpNewAndTerminate.ToString();
					}
					return vipXpressCisExpNewAndTerminate;
				}
			}
			public static bool VipXpressMDMS
			{
				get 
				{
					if (vipXpressMDMSStr == "zz")
					{
						vipXpressMDMS = systemHandler.ReadOptionalBoolean("ICC_WO_VIP_XPRESS_MDMS", false);
						vipXpressMDMSStr = vipXpressMDMS.ToString();
					}
					return vipXpressMDMS;
				}
			}
			public static bool UpdateMeasPntName
			{
				get 
				{
					if (updateMeasPntNameStr == "zz")
					{
						updateMeasPntName = systemHandler.ReadOptionalBoolean("ICC_WO_UPDATE_MEAS_PNT_NAME", false);
						updateMeasPntNameStr = updateMeasPntName.ToString();
					}
					return updateMeasPntName;
				}
			}
			public static bool UpdateMeasPntFusesz
			{
				get 
				{
					if (updateMeasPntFuseszStr == "zz")
					{
						updateMeasPntFusesz = systemHandler.ReadOptionalBoolean("ICC_WO_UPDATE_MEAS_PNT_FUSESZ", false);
						updateMeasPntFuseszStr = updateMeasPntFusesz.ToString();
					}
					return updateMeasPntFusesz;
				}
			}
			public static bool ImpNoCheckLocation
			{
				get 
				{
					if (impNoCheckLocationStr == "zz")
					{
						impNoCheckLocation = systemHandler.ReadOptionalBoolean("ICC_WO_IMP_NO_CHECK_LOCATION", false);
						impNoCheckLocationStr = impNoCheckLocation.ToString();
					}
					return impNoCheckLocation;
				}
			}
			public static string VerificationInterval
			{
				get
				{
					if (verificationInterval == "zz") verificationInterval = systemHandler.ReadOptionalString("ICC_WO_VERIFICATION_INTERVAL", "");
					return verificationInterval;
				}
			}
			public static int MaxWorkOrderRows
			{
				get
				{
					if (maxWorkOrderRows == Int32.MinValue) 
						maxWorkOrderRows = systemHandler.ReadOptionalInt32(
							"ICC_MAX_WORK_ORDER_ROWS", -1);

					return maxWorkOrderRows;
				}
			}
			public static int ExpMaxInFile
			{
				get
				{
					if (expMaxInFile == Int32.MinValue) 
						expMaxInFile = systemHandler.ReadOptionalInt32(
							"ICC_WO_EXP_MAX_IN_FILE", -1);

					return expMaxInFile;
				}
			}
			public static string ExternalWOsystem
			{
				get
				{
					if (externalWOsystem == "zz")
						externalWOsystem = systemHandler.ReadOptionalString("ICC_WO_EXTERNAL_WO_SYSTEM", "");
					return externalWOsystem;
				}
			}
			public static string XmlVersion
			{
				get
				{
					if (xmlVersion == "zz")
						xmlVersion = systemHandler.ReadOptionalString("ICC_WO_XML_VERSION", "V1");
					return xmlVersion;
				}
			}
			public static string DocDirImp
			{
				get
				{
					if (docDirImp == "zz")
						docDirImp = systemHandler.ReadOptionalString("ICC_WO_DOC_DIR_IMP", "");
					return docDirImp;
				}
			}
			public static string DocDirExp
			{
				get
				{
					if (docDirExp == "zz")
						docDirExp = systemHandler.ReadOptionalString("ICC_WO_DOC_DIR_EXP", "");
					return docDirExp;
				}
			}
			public static string SpotCheckCountry
			{
				get
				{
					if (spotCheckCountry == "zz")
						spotCheckCountry = systemHandler.ReadOptionalString("ICC_WO_SPOT_CHECK_COUNTRY", "norway");
					return spotCheckCountry;
				}
			}
			public static string SpotCheckLabId
			{
				get
				{
					if (spotCheckLabId == "zz")
						spotCheckLabId = systemHandler.ReadOptionalString("ICC_WO_SPOT_CHECK_LAB_ID", "");
					return spotCheckLabId;
				}
			}
			public static bool ActivateRegisters
			{
				get
				{
					if (activateRegistersStr == "zz")
					{
						activateRegisters = systemHandler.ReadOptionalBoolean("ICC_WO_ACTIVATE_REGISTERS", false);
						activateRegistersStr = activateRegisters.ToString();
					}
					return activateRegisters;
				}
			}
		}

		public sealed class MeasPnt 
		{
			static int maxMeasPntRows = Int32.MinValue;

			public static int MaxMeasPntRows
			{
				get
				{
					if (maxMeasPntRows == Int32.MinValue) 
						maxMeasPntRows = systemHandler.ReadOptionalInt32(
							"ICC_MAX_MEAS_PNT_ROWS", -1);

					return maxMeasPntRows;
				}
			}
		}

		public sealed class Capacity
		{
			public static decimal MinimumRegulationCapacity
			{
				get 
				{ 
					return commonHandler.ReadOptionalDecimal(
						"ICC_CAPACITY_MIN_REGULATION", 0); 
				}
				set 
				{ 
					commonHandler.Write("ICC_CAPACITY_MIN_REGULATION", value); 
				}
			}

			public static int SelectedGridAreaID
			{
				get 
				{ 
					return userHandler.ReadOptionalInt32(
						"ICC_CAPACITY_SELECTED_NETA_KEY", -1); 
				}
				set 
				{ 
					userHandler.Write("ICC_CAPACITY_SELECTED_NETA_KEY", value); 
				}
			}

			public static string BeginningMacro
			{
				get 
				{ 
					return userHandler.ReadOptionalString(
						"ICC_CAPACITY_BEGINNING_MACRO", "today"); 
				}
				set 
				{ 
					userHandler.Write("ICC_CAPACITY_BEGINNING_MACRO", value); 
				}
			}

			public static string EndMacro
			{
				get 
				{ 
					return userHandler.ReadOptionalString(
						"ICC_CAPACITY_END_MACRO", "today+1d"); 
				}
				set 
				{ 
					userHandler.Write("ICC_CAPACITY_END_MACRO", value); 
				}
			}

			public static int RecalculationIntervalMinutes
			{
				get 
				{ 
					return userHandler.ReadOptionalInt32(
						"ICC_CAPACITY_RECALCULATION_INTERVAL_MINUTES", 5); 
				}
				set 
				{ 
					userHandler.Write("ICC_CAPACITY_RECALCULATION_INTERVAL_MINUTES", value); 
				}
			}

			public static bool AutoRefresh
			{
				get 
				{ 
					return userHandler.ReadOptionalBoolean(
						"ICC_CAPACITY_AUTO_REFRESH", false); 
				}
				set 
				{ 
					userHandler.Write("ICC_CAPACITY_AUTO_REFRESH", value); 
				}
			}

			public static int StartCostHours
			{
				get 
				{ 
					return commonHandler.ReadOptionalInt32(
						"ICC_CAPACITY_START_COST_HOURS", 1); 
				}
				set 
				{ 
					commonHandler.Write("ICC_CAPACITY_START_COST_HOURS", value); 
				}
			}
		}

	    public sealed class WeatherUtility
	    {
	        public static bool HasIccTsHierarchyLicense => systemHandler.ReadOptionalBoolean("_ICC_TS_HIERARCHY_LICENSE", false);
	    }

	    public sealed class Alarm
		{
			public static int MaxItemsInAlarmList
			{
				get 
				{ 
					return userHandler.ReadOptionalInt32(
						"ICC_ALARM_MAX_ITEMS_IN_ALARM_LIST", 1000); 
				}
				set 
				{ 
					userHandler.Write("ICC_ALARM_MAX_ITEMS_IN_ALARM_LIST", value); 
				}
			}
			public static int IccDbAlarmSecondsPerLoop
			{
				get
				{
					return systemHandler.ReadOptionalInt32("ICC_DB_ALARM_SECONDS_PER_LOOP", 30);
				}
			}
		}

		public sealed class MeteringService
		{
			public static int ImportActor
			{
				get
				{
					return systemHandler.ReadOptionalInt32(
					"IMPORTACTOR_BREAKPOINT_1", -1);
				}
			}

			public static int ExportActor
			{
				get
				{
					return systemHandler.ReadOptionalInt32(
					"EXPORTACTOR_BREAKPOINT_1", -1);
				}
			}

			public static int ImportFormat
			{
				get
				{
					return systemHandler.ReadOptionalInt32(
					"IMPORTFORMAT_BREAKPOINT_1", -1);
				}
			}

			public static int ExportFormatDifferential
			{
				get
				{
					return systemHandler.ReadOptionalInt32(
					"EXPORTFORMAT_HOUR_1", -1);
				}
			}

			public static int ExportFormatAccumulative
			{
				get
				{
					return systemHandler.ReadOptionalInt32(
					"EXPORTFORMAT_BREAKPOINT_1", -1);
				}
			}

			public static bool UseBilling
			{
				get
				{
					return systemHandler.ReadOptionalBoolean(
					"USE_BILLING", false);
				}
			}

			public static int BillingExportActor
			{
				get
				{
					return systemHandler.ReadOptionalInt32(
					"BILLING_EXPORTACTOR", -1);
				}
			}


			public static bool AutoCreateExchangePoint
			{
				get
				{
					return systemHandler.ReadOptionalBoolean(
					"ICC_AUTO_CREATE_EXCH_PNT", false);
				}
                set
                {
                    systemHandler.Write("ICC_AUTO_CREATE_EXCH_PNT", value);
                }
            }

			public static bool ClearSupplierWhenNoCommidities
			{
				get
				{
					return systemHandler.ReadOptionalBoolean("SUBMIT_AGREEMENT_CLEAR_SUPPLIER_WITH_NO_COMMODITIES", true);
				}
			}

			public static double ValidationFactorConstant
			{
				get
				{
					return (Double)systemHandler.ReadOptionalDecimal("ICC_VAL_FACTOR_CONSTANT", (Decimal)1.0);
				}
			}
		};


		public sealed class TsStore
		{
			public static bool OverwriteCcPeriods
			{
				get
				{
					return systemHandler.ReadOptionalBoolean("ICC_TSSTORE_OVERWRITE_CC_PERIODS", true);
				}
				set
				{
					systemHandler.Write("ICC_TSSTORE_OVERWRITE_CC_PERIODS", value);
				}
			}
		}
	}

}
