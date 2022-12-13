using System.Data;
using Oracle.ManagedDataAccess.Client;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for UnitData.
	/// </summary>
	public class UnitData
	{
		public static bool IsUnit(string unitcode, ref int unit_key)
		{
            var cmd = new OracleCommand();
			cmd.CommandText = "select unme_key from unmea_tp where code = :1";
			cmd.Parameters.Add(":1",unitcode);
			object o = Util.CommandToScalar(cmd);
			if( o != null)
			{
				unit_key = (int) o;
				return true;
			}
			else
            {
				return false;
		    }
        }

		public static bool IsScalable(int fromUnitKey, int toUnitKey, ref decimal factor)
		{
            var cmd = new OracleCommand();
			cmd.CommandText = "select factor from unmea_rescale_tp where from_unme_key = :1 and to_unme_key = :2";
			cmd.Parameters.Add(":1",fromUnitKey);
			cmd.Parameters.Add(":1",toUnitKey);
			object o = Util.CommandToScalar(cmd);
			if( o != null)
			{
				factor = (decimal) o;
				return true;
			}
			else
            {
				return false;	
	    	}
        }

        public static string GetUnit(int unitKey)
        {
            using (IDbConnection connection = Util.OpenConnection())
            {
                return GetUnit(unitKey, connection);
            }
        }

		public static string GetUnit(int unitKey, IDbConnection connection)
		{
            var cmd = new OracleCommand();
			cmd.CommandText = "select code from unmea_tp where unme_key = :1";
			cmd.Parameters.Add(":1",unitKey);
			object o = connection!=null?Util.CommandToScalar(cmd,connection):Util.CommandToScalar(cmd);
			if( o != null)
            {
				return (string) o;
            }
			else
            {
				return null;
		    }
        }

		public static DataTable FetchUnmeaTP() 
		{
            var oCmd = new OracleCommand();
			oCmd.CommandText = "select unme_key,code,name from unmea_tp";
			return Util.CommandToDataTable(oCmd);
		}

		public static DataTable FetchUnmeaRescale()
		{
            var oCmd = new OracleCommand();
			oCmd.CommandText = "select from_unme_key,to_unme_key,factor from unmea_rescale_tp";
			return Util.CommandToDataTable(oCmd);
		}

	}
}
