using System;
using System.Collections;
using System.Data;
using Powel.Icc.Common;
using Oracle.ManagedDataAccess.Client;
using System.Collections.Generic;

namespace Powel.Icc.Data.Metering
{
	/// <summary>
	/// Summary description for MeteringData.
	/// </summary>
	public class MeteringData
	{
		private static string GetErrorText(IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.GetErrorText");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("sError", OracleDbType.Varchar2, 1000, null, ParameterDirection.ReturnValue);

			Util.ExecuteCommand(cmd, connection);
			return Convert.ToString(cmd.Parameters["sError"].Value);
		}

		private static int GetErrorNumber(IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.GetErrorNum");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);

			Util.ExecuteCommand(cmd, connection);
			
			return Math.Abs(Util.GetInt32(cmd.Parameters["nError"]));
		}

		private static string[] GetErrorParams(IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.GetErrorParams");
			cmd.CommandType = CommandType.StoredProcedure;
			List<string> args = new List<string>();
			OracleParameter  returnValue  = new OracleParameter();
			returnValue.Size = 10;
			returnValue.ArrayBindSize = new int[10] {
				200, 200, 200, 200, 200, 200, 200, 200, 200, 200/*,
				200, 200, 200, 200, 200, 200, 200, 200, 200, 200*/
			};
			returnValue.CollectionType = OracleCollectionType.PLSQLAssociativeArray;			
      returnValue.Direction = ParameterDirection.ReturnValue;			
			cmd.Parameters.Add(returnValue);
			Util.ExecuteCommand(cmd, connection);
			object o = returnValue.Value;
			IEnumerable enumerable = o as IEnumerable;
			if( enumerable != null)
			{
				foreach(object element in enumerable)
				{
					IConvertible s = element as IConvertible;
					if(s != null)					
						args.Add(s.ToString());
          else args.Add(null);						
				}
			}
			return args.ToArray();
		}
		public static bool IsMeteringDBException(OracleException exception)
		{
			if(exception.Message.IndexOf("ORA-20111") != -1)
				return true;
			return false;
		}

		internal static void ThrowMeteringDBException(IDbConnection connection)
		{
			throw new IccException(GetErrorText(connection), GetErrorNumber(connection), GetErrorParams(connection));
		}

		internal static void ExecuteMeteringDBCommand (OracleCommand cmd, IDbConnection connection)
		{
			try
			{
				Util.ExecuteCommand(cmd, connection);
			}
			catch(OracleException ex)
			{
				if( MeteringData.IsMeteringDBException(ex))
					MeteringData.ThrowMeteringDBException(connection);
				else
					throw;
			}
		}

		public static void PrepareMeteringService(IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.MeteringDBPrepare");
			cmd.CommandType = CommandType.StoredProcedure;
			Util.ExecuteCommand(cmd, connection);
		}

		public static void ConfigMeteringService(int importActorKey, int exportActorKey, string importFormat, string exportFormatDifferential, string exportFormatAccumulative, bool createDefaultExchangeMethods, IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.MeteringDBConfig");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("iImportActor", OracleDbType.Int32, importActorKey, ParameterDirection.Input);
			cmd.Parameters.Add("iExportActor", OracleDbType.Int32, exportActorKey, ParameterDirection.Input);
			cmd.Parameters.Add("iImportFormat", OracleDbType.Varchar2, 255, importFormat, ParameterDirection.Input);
			cmd.Parameters.Add("iExportFormatDifferential", OracleDbType.Varchar2, 255, exportFormatDifferential, ParameterDirection.Input);
			cmd.Parameters.Add("iExportFormatAccumulative", OracleDbType.Varchar2, 255, exportFormatAccumulative, ParameterDirection.Input);
			if(!createDefaultExchangeMethods)
				cmd.Parameters.Add("iCreateExchMeths", OracleDbType.Int32, 0, ParameterDirection.Input); //default true/1

			Util.ExecuteCommand(cmd, connection);
		}

		public static void MeteringTestCleanAgreement(string measurePointID, IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.MeteringTestCleanAgreement");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("iMeasurePointID", OracleDbType.Varchar2, 1000, measurePointID, ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);
		}

		public static void MeteringTestCleanOrders(IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.MeteringTestCleanOrders");
			cmd.CommandType = CommandType.StoredProcedure;

			Util.ExecuteCommand(cmd, connection);
		}

		public static void MeteringTestCleanTimeSeriesValues(string measurePointID, IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.MeteringTestCleanValues");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("iMeasurePointID", OracleDbType.Varchar2, 1000, measurePointID, ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);
		}

		public static void MeteringTestCleanAll(string measurePointID, IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.MeteringTestCleanUp");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("iMeasurePointID", OracleDbType.Varchar2, 1000, measurePointID, ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);
		}
	}

	
}
