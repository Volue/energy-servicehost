using System;
using System.Data;
using System.Collections;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using Powel.Icc.Services.Time;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Common;

namespace Powel.Icc.Data.Metering
{
	/// <summary>
	/// Summary description for RegisterData.
	/// </summary>
	public class RegisterData
	{
		public static ArrayList GetForComponent(Component component, UtcTime validAtTime, IDbConnection connection)
		{
			if( component.Key == 0)
				throw new ArgumentException("RegisterData.GetForComponent: component.Key is 0");
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.RegisterGetForComponent");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32, component.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime), ParameterDirection.Input);
				
			Util.ExecuteCommand(cmd, connection);
			ArrayList alRegisters = GetRegistersFromCommand(cmd);

			return alRegisters;
		}

		public static ArrayList IECGetForComponent(Component component,
		                                           UtcTime validAtTime,
		                                           IDbConnection connection)
		{
			if (component.Key == 0)
				throw new ArgumentException("RegisterData.GetForComponent: component.Key is 0");
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.RegisterGetForComponent");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32,
			                   component.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2,
			                   cal.ToDatabaseString(validAtTime), ParameterDirection.Input);
			
			Util.ExecuteCommand(cmd, connection);
			ArrayList alRegisters = IECGetRegistersFromCommand(cmd);

			return alRegisters;
		}

		public static ArrayList GetForMeasurePoint(MeasurePoint measurePoint,
		                                           UtcTime validAtTime,
		                                           IDbConnection connection)
		{
			if (measurePoint.Key == 0)
				throw new ArgumentException("RegisterData.GetForMeasurePoint: measurePoint.Key is 0");
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.RegisterGetForMeasurePoint");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("ImeasKey", OracleDbType.Int32,
			                   measurePoint.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2,
			                   cal.ToDatabaseString(validAtTime), ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);
			ArrayList alRegisters = GetRegistersFromCommand(cmd);
			
			return alRegisters;
		}

		private static ArrayList GetRegistersFromCommand(OracleCommand cmd)
		{
			ArrayList alRegisters = new ArrayList();
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value)
			{
				using (OracleDataReader reader = cursor.GetDataReader())
				{
					while (reader.Read())
					{
						var r = new Register(
						Util.GetInt32(reader, "inst_key"),
						Util.GetInt32(reader, "inst_reg"),
						Util.GetInt32(reader, "ndigits"),
						Util.GetInt32(reader, "ndecimals"),
						(MeasuringUnitCodeType)Enum.ToObject(typeof(MeasuringUnitCodeType), Util.GetInt32(reader, "unme_key")),
						(FlowDirectionType)Enum.ToObject(typeof(FlowDirectionType), Util.GetInt32(reader, "direction")),
						"" /*TODO Metering constant*/,
						Util.GetFloat(reader, "factor"),
						(RemoteMeteringType)Enum.ToObject(typeof(RemoteMeteringType), Util.GetInt32(reader, "automatic")),
						(WayOfRegistrationType)Enum.ToObject(typeof(WayOfRegistrationType), Util.GetInt32(reader, "tsin_key")),
						(MeteringFunctionCodeType)Enum.ToObject(typeof(MeteringFunctionCodeType), Util.GetInt32(reader, "tser_key")),
						//TsOrigin.Undefined, //ToDo:This needs to be fixed
						(RegisterState)Enum.ToObject(typeof(RegisterState), Util.GetInt32(reader, "state")),
						cal.ToUtcTime(Util.GetString(reader, "valftime")),
						cal.ToUtcTime(Util.GetString(reader, "valttime")),
						Util.GetString(reader, "instr_no"),
							(PrimaryValueType)Enum.ToObject(typeof(PrimaryValueType), Util.GetInt32(reader, "vltp_key")),
                        Util.GetFloat(reader,"used_power")
						);
						r.timsKey = Util.GetInt32(reader, "tims_key");
						alRegisters.Add(r); 
					}
				}
			}

			return alRegisters;
		}

		private static ArrayList IECGetRegistersFromCommand(OracleCommand cmd)
		{
			ArrayList alRegisters = new ArrayList();
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value) {
					using (OracleDataReader reader = cursor.GetDataReader()) {
						while (reader.Read()) {
							Register tempReg = new Register(Util.GetInt32(reader, "inst_key"),
							                                Util.GetInt32(reader, "inst_reg"),
							                                Util.GetInt32(reader, "ndigits"),
							                                Util.GetInt32(reader, "ndecimals"),
							                                //Util.GetInt32(reader, "unme_key"),
							                                Util.GetInt32(reader, "direction"),
							                                "",
							                                Util.GetFloat(reader, "factor"),
							                                (RemoteMeteringType)
							                                Enum.ToObject(typeof (RemoteMeteringType),
							                                              Util.GetInt32(reader, "automatic")),
							                                (RegisterState)
							                                Enum.ToObject(typeof (RegisterState),
							                                              Util.GetInt32(reader, "state")),
							                                cal.ToUtcTime(Util.GetString(reader, "valftime")),
							                                cal.ToUtcTime(Util.GetString(reader, "valttime")),
							                                Util.GetString(reader, "instr_no"),
							                                (PrimaryValueType)
							                                Enum.ToObject(typeof (PrimaryValueType),
							                                              Util.GetInt32(reader, "vltp_key")),
                                                            Util.GetFloat(reader,"used_power"));

							tempReg.tsin_key = Util.GetInt32(reader, "tsin_key");
							tempReg.tser_key = Util.GetInt32(reader, "tser_key");
							tempReg.unme_key = Util.GetInt32(reader, "unme_key");
							tempReg.timsKey = Util.GetInt32(reader, "tims_key");

							alRegisters.Add(tempReg);
						}
					}
			}
			return alRegisters;
		}

		public static int Update(Register register, Component relatedComponent, UtcTime timeOfUpdate, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			if( register.Key == 0)
			{
				if( relatedComponent == null)
					throw new ArgumentNullException("relatedComponent", "RegisterUpdate: A register needs to be related to a component");
				register.key = relatedComponent.Key;
			}

			OracleCommand cmd = new OracleCommand("ICC_METERING.RegisterUpdate");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32,register.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IinstReg", OracleDbType.Int32,register.RegisterNumber, ParameterDirection.Input);
			cmd.Parameters.Add("Istate", OracleDbType.Int32, register.State, ParameterDirection.Input);
			cmd.Parameters.Add("ItserKey", OracleDbType.Int32, register.MeteringFunctionCode, ParameterDirection.Input);
			cmd.Parameters.Add("IunmeKey", OracleDbType.Int32, register.MeasureUnitCode, ParameterDirection.Input);
			cmd.Parameters.Add("ItsinKey", OracleDbType.Int32, register.WayOfRegistration, ParameterDirection.Input);
			cmd.Parameters.Add("IinstKea", OracleDbType.Int32, null, ParameterDirection.Input);
			cmd.Parameters.Add("InDigits", OracleDbType.Int32, register.CapacityDigits, ParameterDirection.Input);
			cmd.Parameters.Add("InDecimals", OracleDbType.Int32, register.CapacityDecimals, ParameterDirection.Input);
			cmd.Parameters.Add("Ifactor", OracleDbType.Decimal, register.PulseConstant, ParameterDirection.Input);
			cmd.Parameters.Add("Idirection", OracleDbType.Int32, register.DirectionOfFlow, ParameterDirection.Input);
			cmd.Parameters.Add("Iautomatic", OracleDbType.Int32, register.RemoteMetering, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfUpdate), ParameterDirection.Input);
			cmd.Parameters.Add("IvltpKey", OracleDbType.Int32, register.ValueType, ParameterDirection.Input);
		    cmd.Parameters.Add("IusedPower", OracleDbType.Decimal, register.AnnualConsumption, ParameterDirection.Input);

			cmd = ParameterNullifier(cmd);

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);

			return Util.GetInt32(cmd.Parameters["nError"]);			
		}

		public static int Update(Register register, ReadingType readingType,
		                         Component relatedComponent, UtcTime timeOfUpdate,
		                         IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			if (register.Key == 0) {
				if (relatedComponent == null)
					throw new ArgumentNullException("relatedComponent", "RegisterUpdate: A register needs to be related to a component");
				register.key = relatedComponent.Key;
			}

			OracleCommand cmd = new OracleCommand("ICC_METERING.RegisterUpdate");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32, register.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IinstReg", OracleDbType.Int32, register.RegisterNumber, ParameterDirection.Input);
			cmd.Parameters.Add("Istate", OracleDbType.Int32, register.State, ParameterDirection.Input);
			//cmd.Parameters.Add("ItserKey", OracleDbType.Int32, register.MeteringFunctionCode, ParameterDirection.Input);
			cmd.Parameters.Add("ItserKey", OracleDbType.Int32, readingType.TserKey, ParameterDirection.Input);
			//cmd.Parameters.Add("IunmeKey", OracleDbType.Int32, register.MeasureUnitCode, ParameterDirection.Input);
			cmd.Parameters.Add("IunmeKey", OracleDbType.Int32, readingType.UnmeKey, ParameterDirection.Input);
			//cmd.Parameters.Add("ItsinKey", OracleDbType.Int32, register.WayOfRegistration, ParameterDirection.Input);
			cmd.Parameters.Add("ItsinKey", OracleDbType.Int32, readingType.TsinKey, ParameterDirection.Input);
			cmd.Parameters.Add("IinstKea", OracleDbType.Int32, null, ParameterDirection.Input);
			cmd.Parameters.Add("InDigits", OracleDbType.Int32, register.CapacityDigits, ParameterDirection.Input);
			cmd.Parameters.Add("InDecimals", OracleDbType.Int32, register.CapacityDecimals, ParameterDirection.Input);
			cmd.Parameters.Add("Ifactor", OracleDbType.Decimal, register.PulseConstant, ParameterDirection.Input);
			//cmd.Parameters.Add("Idirection", OracleDbType.Int32, register.DirectionOfFlow, ParameterDirection.Input);
			cmd.Parameters.Add("Idirection", OracleDbType.Int32, readingType.FlowDirection, ParameterDirection.Input);
			cmd.Parameters.Add("Iautomatic", OracleDbType.Int32, register.RemoteMetering, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfUpdate), ParameterDirection.Input);
			//cmd.Parameters.Add("IvltpKey", OracleDbType.Int32, register.ValueType, ParameterDirection.Input);
			cmd.Parameters.Add("IvltpKey", OracleDbType.Int32, readingType.VltpKey, ParameterDirection.Input);
		    cmd.Parameters.Add("IusedPower", OracleDbType.Decimal, register.AnnualConsumption, ParameterDirection.Input);

			cmd = ParameterNullifier(cmd);

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);

			return Util.GetInt32(cmd.Parameters["nError"]);
		}


		public static void Finish(Register register, UtcTime timeOfFinish, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			if( register.Key == 0 || register.RegisterNumber == 0)
				throw new ArgumentException("RegisterFinish: Register without Key or RegisterNumber");
			
			OracleCommand cmd = new OracleCommand("ICC_METERING.RegisterFinish");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32,register.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IinstReg", OracleDbType.Int32,register.RegisterNumber, ParameterDirection.Input);
			cmd.Parameters.Add("IinstKea", OracleDbType.Int32, null, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfFinish), ParameterDirection.Input);
		
			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}

		private static OracleCommand ParameterNullifier(OracleCommand cmd)
		{
			foreach (OracleParameter param in cmd.Parameters)
			{
				if (param.Value != null)
				{
					if (param.OracleDbType == OracleDbType.Single)
						if (param.Value.ToString() == decimal.MinValue.ToString())
							param.Value = null;
					if (param.OracleDbType == OracleDbType.Int32)
						if (param.Value.ToString() == Int32.MinValue.ToString())
							param.Value = null;
                    if (param.OracleDbType == OracleDbType.Decimal)
                        if (param.Value.ToString() == decimal.MinValue.ToString() ||
                            param.Value.ToString() == float.NaN.ToString())
                            param.Value = null;
				}

			}
			return cmd;
		}

		public static int CreateTimeSeries(int tsKey, MeasurePoint measurePoint,
		                                   Register register, ReadingType readingType,
		                                   UtcTime timeOfUpdate, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.CreateTimeSeries");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("ItimsKey", OracleDbType.Int32, tsKey, ParameterDirection.Input);
			//cmd.Parameters.Add("Itscode", OracleDbType.Varchar2, tsName, ParameterDirection.Input);
			cmd.Parameters.Add("Itscode", OracleDbType.Varchar2, measurePoint.Id + readingType.TsExt, ParameterDirection.Input);
			cmd.Parameters.Add("ImeasKey", OracleDbType.Int32, measurePoint.Key, ParameterDirection.Input);
			cmd.Parameters.Add("ImeasSeqno", OracleDbType.Int32, measurePoint.Seqno, ParameterDirection.Input);
			//cmd.Parameters.Add("ItsinKey", OracleDbType.Int32, register.WayOfRegistration, ParameterDirection.Input);
			cmd.Parameters.Add("ItsinKey", OracleDbType.Int32, readingType.TsinKey, ParameterDirection.Input);
			//cmd.Parameters.Add("ItserKey", OracleDbType.Int32, Convert.ToInt32(tsType), ParameterDirection.Input);
			cmd.Parameters.Add("ItserKey", OracleDbType.Int32, readingType.TserKey, ParameterDirection.Input);
			//cmd.Parameters.Add("IunmeKey", OracleDbType.Int32, register.MeasureUnitCode, ParameterDirection.Input);
			cmd.Parameters.Add("IunmeKey", OracleDbType.Int32, readingType.UnmeKey, ParameterDirection.Input);
			//cmd.Parameters.Add("Idirection", OracleDbType.Int32, register.DirectionOfFlow, ParameterDirection.Input);
			cmd.Parameters.Add("Idirection", OracleDbType.Int32, readingType.FlowDirection, ParameterDirection.Input);
			cmd.Parameters.Add("IAutomatic", OracleDbType.Int32, register.RemoteMetering, ParameterDirection.Input);
			//cmd.Parameters.Add("IvltpKey", OracleDbType.Int32, register.ValueType, ParameterDirection.Input);
			cmd.Parameters.Add("IvltpKey", OracleDbType.Int32, readingType.VltpKey, ParameterDirection.Input);
			cmd.Parameters.Add("IvltpValftime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfUpdate), ParameterDirection.Input);
			cmd.Parameters.Add("Iparaflag", OracleDbType.Int32, readingType.Paraflag, ParameterDirection.Input);
			cmd.Parameters.Add("Iconvertusinglocaltime", OracleDbType.Int32, readingType.ConvertusingLocaltime, ParameterDirection.Input);
			cmd.Parameters.Add("Iuse_registration_handling", OracleDbType.Int32, readingType.UseRegistrationHandling, ParameterDirection.Input);
			cmd.Parameters.Add("Iaudit_level", OracleDbType.Int32, readingType.AuditLevel, ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			if (cmd.Parameters[0].Value == DBNull.Value)
				throw new DataException("Creation of time series failed");
			return Util.GetInt32(cmd.Parameters[0]);
		}

        public static int FindTimsKey(Register register, UtcTime date, IDbConnection connection)
        {
            RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
            var cmd = new OracleCommand("SELECT ICC_METERING.TimeSeriesGetForRegister(" + register.Key +
                "," + register.RegisterNumber + ",'" + cal.ToDatabaseString(date) + "') from dual");
            DataTable dt = Util.CommandToDataTable(cmd);
            if (dt.Rows.Count == 1)
            {
                DataRow row = dt.Rows[0];
                var val = row[0];
                string valStr = val.ToString();
                int timsKey = int.Parse(valStr);
                timsKey = int.Parse(dt.Rows[0][0].ToString());
                if (timsKey < 0) return int.MinValue;
                else return timsKey;
            }
            else
                return int.MinValue;
        }

		public static void SetRegisterTimsKey(Register register, int timsKey, IDbConnection connection)
		{
			var cmd = new OracleCommand("UPDATE instr_rg SET tims_key = :1 WHERE inst_key = :2 AND inst_reg = :3 AND state = :4 AND tsin_key=(SELECT tsin_key FROM timeser WHERE tims_key = :5)");
			cmd.Parameters.Add(":1", OracleDbType.Int32, timsKey, ParameterDirection.Input);
			cmd.Parameters.Add(":2", OracleDbType.Int32, register.Key, ParameterDirection.Input);
			cmd.Parameters.Add(":3", OracleDbType.Int32, register.RegisterNumber, ParameterDirection.Input);
			cmd.Parameters.Add(":4", OracleDbType.Int32, 1, ParameterDirection.Input);
		    cmd.Parameters.Add(":5", OracleDbType.Int32, timsKey, ParameterDirection.Input);
			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}

		public static bool UpdateImpExpDef(int timeSeriesKey, bool createNew, Register register,
		                                   string globeref, double importFactor, double exportFactor,
		                                   bool createExport, string impRef, string expRef,
		                                   Frequency importFrequency, Frequency exportMethod,
		                                   UtcTime timeOfUpdate, UtcTime exportStartTime,
                                           int exportDir, IDbConnection connection)
		{
			return UpdateImpExpDef(timeSeriesKey, createNew, register, globeref,
			                       importFactor, exportFactor, true,
			                       createExport, impRef, expRef, importFrequency,
			                       exportMethod, timeOfUpdate,
			                       exportStartTime, exportDir, connection);
		}

        public static bool UpdateImpExpDef(int timeSeriesKey, bool createNew, Register register,
                                           string globeref, double importFactor, double exportFactor,
                                           bool createImport, bool createExport, string impRef,
                                           string expRef, Frequency importFrequency,
                                           Frequency exportMethod, UtcTime timeOfUpdate,
                                           UtcTime exportStartTime, int exportDir, IDbConnection connection)
        {
            int newInstance = 0, doImport = 0, doExport = 0;
            string Lgloberef = globeref;
            if (createNew) newInstance = 1;
            if (createImport) doImport = 1;
            if (createExport) doExport = 1;

            RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
            OracleCommand cmd = new OracleCommand("ICC_METERING.CheckTimeSeries");

            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.Add("InewInstr", OracleDbType.Int32, newInstance, ParameterDirection.Input);
            cmd.Parameters.Add("ItimsKey", OracleDbType.Int32, timeSeriesKey, ParameterDirection.Input);
            cmd.Parameters.Add("Itype", OracleDbType.Int32, exportMethod, ParameterDirection.Input);
            cmd.Parameters.Add("Igloberef", OracleDbType.Varchar2, globeref, ParameterDirection.Input);
            cmd.Parameters.Add("InDigits", OracleDbType.Int32, register.CapacityDigits, ParameterDirection.Input);
            cmd.Parameters.Add("InDecimals", OracleDbType.Int32, register.CapacityDecimals, ParameterDirection.Input);
            cmd.Parameters.Add("Ivalftime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfUpdate),
                               ParameterDirection.Input);
            cmd.Parameters.Add("IhourMeter", OracleDbType.Int32, register.readingType.TsinKey == 101,
                               ParameterDirection.Input);
            cmd.Parameters.Add("IimportFactor", OracleDbType.Double, importFactor, ParameterDirection.Input);
            cmd.Parameters.Add("IexportFactor", OracleDbType.Double, exportFactor, ParameterDirection.Input);
            cmd.Parameters.Add("IdoExport", OracleDbType.Int32, doExport, ParameterDirection.Input);
            cmd.Parameters.Add("IimpRef", OracleDbType.Varchar2, impRef, ParameterDirection.Input);
            cmd.Parameters.Add("IexpRef", OracleDbType.Varchar2, expRef, ParameterDirection.Input);
            cmd.Parameters.Add("IimpFrequency", OracleDbType.Int32, importFrequency, ParameterDirection.Input);
            cmd.Parameters.Add("IimpActor", OracleDbType.Varchar2, register.senderID, ParameterDirection.Input);
            cmd.Parameters.Add("IexpActor", OracleDbType.Varchar2, register.receiverID, ParameterDirection.Input);
            cmd.Parameters.Add("IdoImport", OracleDbType.Int32, doImport, ParameterDirection.Input);
            cmd.Parameters.Add("Iexpvalftime", OracleDbType.Varchar2, cal.ToDatabaseString(exportStartTime),
                               ParameterDirection.Input);
            cmd.Parameters.Add("IimportTstr_name", OracleDbType.Varchar2, register.importFormat,
                               ParameterDirection.Input);
            cmd.Parameters.Add("IexportTstr_name", OracleDbType.Varchar2, register.exportFormat,
                               ParameterDirection.Input);
            cmd.Parameters.Add("IexportMethod", OracleDbType.Varchar2, register.exportMethod, ParameterDirection.Input);
            if (createExport &&
                exportDir == 1)
            {
                cmd.Parameters.Add("IproductCode", OracleDbType.Varchar2, "in", ParameterDirection.Input);
            }

            MeteringData.ExecuteMeteringDBCommand(cmd, connection);
            return true;
        }

	    public static void InactivateImpExpDef(int timeSeriesKey, int direction,
		                                       UtcTime timeOfInactivation, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			var cmd = new OracleCommand("UPDATE impexpts SET valttime = :1 WHERE tims_key = :2 AND direct = :3 AND valftime < :1 AND valttime > :1");
	        cmd.BindByName = true;
			cmd.Parameters.Add(":1", OracleDbType.Varchar2,
			                   cal.ToDatabaseString(timeOfInactivation), ParameterDirection.Input);
			cmd.Parameters.Add(":2", OracleDbType.Int32, timeSeriesKey, ParameterDirection.Input);
			cmd.Parameters.Add(":3", OracleDbType.Int32, direction, ParameterDirection.Input);

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}

		public static void InactivateImpExpDef(int timeSeriesKey, string actor, int direction, UtcTime timeOfInactivation, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			var cmd = new OracleCommand("UPDATE impexpts SET valttime = :1 WHERE tims_key = :2 AND opun_key = ICC_METERING.CHECKACTOR(:3, null, null) AND direct = :4");
			cmd.Parameters.Add(":1", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfInactivation), ParameterDirection.Input);
			cmd.Parameters.Add(":2", OracleDbType.Int32, timeSeriesKey, ParameterDirection.Input);
			cmd.Parameters.Add(":3", OracleDbType.Varchar2, actor, ParameterDirection.Input);
			cmd.Parameters.Add(":4", OracleDbType.Int32, direction, ParameterDirection.Input);

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}

		public static void DeleteByTSKey(int tsKey, IDbConnection connection)
		{
			var cmd = new OracleCommand("ICC_METERING.TimeSeriesDeleteByKey");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("ItsKey", OracleDbType.Int32, tsKey, ParameterDirection.Input);

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}

		public static ArrayList GetExportDefByMeter(Component component,
		                                            UtcTime validAtTime,
		                                            IDbConnection connection)
		{
			if (!(component is Meter))
				throw new ArgumentException("GetExportDefByMeter: Meter component must be of type Meter");

			var alExports = new ArrayList();

			if (component.Key == 0)
				throw new ArgumentException("RegisterData.GetExportDefForComponent: component.Key is 0");
			var cal = IccConfiguration.Time.DatabaseCalendar;
			var cmd = new OracleCommand("ICC_METERING.ExportDefGetForComponent");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32, component.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2,
			                   cal.ToDatabaseString(validAtTime), ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			using (var cursor = (OracleRefCursor) cmd.Parameters[0].Value) {
				using (OracleDataReader reader = cursor.GetDataReader())	{
					while (reader.Read()) {
						var e = new Register(
						                     Util.GetInt32(reader, "inst_reg"),
						                     Util.GetInt32(reader, "ndigits"),
						                     Util.GetInt32(reader, "ndecimals"),
						                     (MeasuringUnitCodeType)
						                     Enum.ToObject(typeof (MeasuringUnitCodeType),
						                                   Util.GetInt32(reader, "unme_key")),
						                     (FlowDirectionType)
						                     Enum.ToObject(typeof (FlowDirectionType),
						                                   Util.GetInt32(reader, "direction")),
						                     "" /*TODO Metering constant*/,
						                     Util.GetFloat(reader, "factor"),
						                     (RemoteMeteringType)
						                     Enum.ToObject(typeof (RemoteMeteringType),
						                                   Util.GetInt32(reader, "automatic")),
						                     (WayOfRegistrationType)
						                     Enum.ToObject(typeof (WayOfRegistrationType),
						                                   Util.GetInt32(reader, "tsin_key")),
						                     (MeteringFunctionCodeType)
						                     Enum.ToObject(typeof (MeteringFunctionCodeType),
						                                   Util.GetInt32(reader, "tser_key")),
						                     //TsOrigin.Undefined, //ToDo:This needs to be fixed
						                     (RegisterState)
						                     Enum.ToObject(typeof (RegisterState),
						                                   Util.GetInt32(reader, "state")),
						                     cal.ToUtcTime(Util.GetString(reader, "valftime")),
						                     cal.ToUtcTime(Util.GetString(reader, "valttime")),
						                     Util.GetString(reader, "instr_no"),
						                     (PrimaryValueType)
						                     Enum.ToObject(typeof (PrimaryValueType),
						                                   Util.GetInt32(reader, "vltp_key")),
                                             Util.GetFloat(reader,"used_power")
						                     );
						e.receiverID = Util.GetString(reader, "receiver");
						e.timsKey = Util.GetInt32(reader, "tims_key");
						e.exportReference = Util.GetString(reader, "globeref");
						e.exportMethod = Util.GetString(reader, "method");
						e.exportFormat = Util.GetString(reader, "format");
						GetAccumulativeExportDefByKeys(e.timsKey, e.receiverID, validAtTime, connection, ref e);
						alExports.Add(e);
					}
				}
			}
			return alExports;
		}

		public static void GetAccumulativeExportDefByKeys(int timsKey, string receiver,
		                                                  UtcTime validAtTime,
		                                                  IDbConnection connection,
		                                                  ref Register e)
		{
			var cal = IccConfiguration.Time.DatabaseCalendar;
			using (var cmd = new OracleCommand("ICC_METERING.GetAccumulativeExportOrders")) {
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
				cmd.Parameters.Add("iTimsKey", OracleDbType.Int32, timsKey, ParameterDirection.Input);
				cmd.Parameters.Add("iReceiver", OracleDbType.Varchar2, receiver, ParameterDirection.Input);
				cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2,
				                   cal.ToDatabaseString(validAtTime), ParameterDirection.Input);

				Util.ExecuteCommand(cmd, connection);

				using (var cursor = (OracleRefCursor)cmd.Parameters[0].Value)
				{
					using (OracleDataReader reader = cursor.GetDataReader())
					{
						while (reader.Read())
						{
							e.mrExportInterval = CalculateFrequency(Util.GetInt32(reader, "intrvl_typ"));
						}
					}
				}
			}
			return;
		}

		public static int CalculateFrequency(int exportInterval)
		{
			var tsint = (TsExpDefInterval)Enum.ToObject(typeof(TsExpDefInterval), exportInterval);
			Frequency interval;
			switch (tsint)
			{
			case TsExpDefInterval.ONCE_A_DAY:
				interval = Frequency.DAILY;
				break;
			case TsExpDefInterval.ONCE_A_WEEK:
				interval = Frequency.WEEKLY;
				break;
			case TsExpDefInterval.ONCE_A_MONTH:
				interval = Frequency.MONTHLY;
				break;
			case TsExpDefInterval.ONCE_A_YEAR:
				interval = Frequency.YEARLY;
				break;
			case TsExpDefInterval.NEVER:
				interval = Frequency.NEVER;
				break;
			default:
				interval = Frequency.NEVER;
				break;
			}
			return (int)interval;
		}

		public enum Frequency
		{
			NEVER = -1,
			HOURLY = 3,
			DAILY = 4,
			WEEKLY = 5,
			MONTHLY = 6,
			BIMONTHLY = 7,
			QUARTERLY = 8,
			SEMIYEARLY = 9,
			YEARLY = 99,
		}
	}
}
