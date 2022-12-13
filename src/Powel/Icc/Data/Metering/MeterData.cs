using System;
using System.Collections;
using System.Data;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Services.Time;
using Powel.Icc.Common;


namespace Powel.Icc.Data.Metering
{
	/// <summary>
	/// Summary description for MeterData.
	/// </summary>
	public class MeterData
	{
		
		public static bool AttachToTerminal(Meter meter, Terminal terminal, UtcTime timeOfUpdate, IDbConnection connection)
		{
			return UpdateConnectionToTerminal(meter, terminal, timeOfUpdate, connection);
		}
		public static void DisconnectFromTerminal(Meter meter, UtcTime timeOfDisconnection, IDbConnection connection)
		{
			FinishConnectionToTerminal(meter, timeOfDisconnection, connection);
		}

		private static bool UpdateConnectionToTerminal(Component component, Terminal terminal, UtcTime timeOfUpdate, IDbConnection connection)
		{
			
			if( !(component is Meter))
				throw new ArgumentException("AttachToTerminal: Only meters can be attached to a terminal");
			if( component.Key == 0)
				throw new ArgumentException("AttachToTerminal: Meter must have a key.");

			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.RegisterUpdate");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32,terminal.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IinstReg", OracleDbType.Int32, -1, ParameterDirection.Input); //constant input parameter to represent an relation to a terminal
			cmd.Parameters.Add("Istate", OracleDbType.Int32, RegisterState.ACTIVE, ParameterDirection.Input);
			cmd.Parameters.Add("ItserKey", OracleDbType.Int32, DBNull.Value, ParameterDirection.Input);
			cmd.Parameters.Add("IunmeKey", OracleDbType.Int32, DBNull.Value, ParameterDirection.Input);
			cmd.Parameters.Add("ItsinKey", OracleDbType.Int32, DBNull.Value, ParameterDirection.Input);
			cmd.Parameters.Add("IinstKea", OracleDbType.Int32, component.Key, ParameterDirection.Input);
			cmd.Parameters.Add("InDigits", OracleDbType.Int32, DBNull.Value, ParameterDirection.Input);
			cmd.Parameters.Add("InDecimals", OracleDbType.Int32, DBNull.Value, ParameterDirection.Input);
			cmd.Parameters.Add("Ifactor", OracleDbType.Decimal, DBNull.Value, ParameterDirection.Input);
			cmd.Parameters.Add("Idirection", OracleDbType.Int32, DBNull.Value, ParameterDirection.Input);
			cmd.Parameters.Add("Iautomatic", OracleDbType.Int32, 0, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfUpdate), ParameterDirection.Input);
			cmd.Parameters.Add("IvltpKey", OracleDbType.Int32, PrimaryValueType.Unknown, ParameterDirection.Input);//TODO riktig?
			cmd.Parameters.Add("IusedPower", OracleDbType.Decimal, DBNull.Value, ParameterDirection.Input);
		
			MeteringData.ExecuteMeteringDBCommand(cmd, connection);

			return true;
		}


		private static void FinishConnectionToTerminal(Component component, UtcTime timeOfFinish, IDbConnection connection)
		{
			Terminal terminal = GetTerminal(component, timeOfFinish, connection);
			if( terminal == null)
				return;
			
			if( !(component is Meter))
				throw new ArgumentException("FinishConnectionToTerminal: Only meters can be attached/disconnected to a terminal");
			if( component.Key == 0)
				throw new ArgumentException("FinishConnectionToTerminal: Meter must have a key.");

			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.RegisterFinish");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32,terminal.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IinstReg", OracleDbType.Int32, -1, ParameterDirection.Input); //constant input parameter to represent an relation to a terminal
			cmd.Parameters.Add("IinstKea", OracleDbType.Int32, component.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfFinish), ParameterDirection.Input);
		
			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}


		public static Terminal GetTerminal(Component component, UtcTime validAtTime, IDbConnection connection)
		{
			Terminal terminal = null;
			if( !(component is Meter))
				return null; //throw new ArgumentException("GetTerminal: Only meters can be attached to a terminal");
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.TerminalGetForComponent");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IinstKea", OracleDbType.Int32, component.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime), ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			int terminalKey = 0;
			using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value)
			{
				using (OracleDataReader reader = cursor.GetDataReader())
				{
					if (reader.Read())
					{
						terminalKey = Util.GetInt32(reader, "inst_key");
					}
				}
			}
			if( terminalKey > 0)
			{
				ArrayList alTerminals = ComponentData.GetByKey(terminalKey, validAtTime, connection);
				if( alTerminals.Count > 0)
					terminal = (Terminal) alTerminals[0];
			}

			return terminal;
		}		
	}
}
