using System;
using System.Data;
using Powel.Icc.Common;
using Oracle.ManagedDataAccess.Client;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for MeteringData.
	/// </summary>
	public class ErrorData
	{
		internal static string GetErrorText(IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ERROR.GetErrorText");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("sError", OracleDbType.Varchar2, 1000, null, ParameterDirection.ReturnValue);

			Util.ExecuteCommand(cmd, connection);
			return Convert.ToString(cmd.Parameters["sError"].Value);
		}

		internal static int GetErrorNumber(IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ERROR.GetErrorNum");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);

			Util.ExecuteCommand(cmd, connection);
			
			return Math.Abs(Util.GetInt32(cmd.Parameters["nError"]));
		}
	}

	
}
