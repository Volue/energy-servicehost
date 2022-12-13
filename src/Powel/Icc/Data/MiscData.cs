/*
$Header$
*/
using System;
using System.Data;
using System.Collections;
using System.Text;
using Oracle.ManagedDataAccess.Client;

namespace Powel.Icc.Data
{
	/// <summary>
	/// Summary description for MiscData.
	/// </summary>
	public class MiscData
	{
		public MiscData()
		{
		}

		public DataTable GetDebitTypes()
		{
			string query = "SELECT TP_KEY AS DEBT_TYP, NAME FROM LANGUAGE_TP WHERE TAG = 'DEBIT_TYPE_TP' ORDER BY DEBT_TYP";

			DataTable debitType = Util.SqlToDataTable(query);
			debitType.TableName = "debitType";
			return debitType;
		}

		public DataTable GetErrorReasons()
		{
			string query = "SELECT * FROM ERROR_REASON_TP";

			DataTable errorReason = Util.SqlToDataTable(query);
			errorReason.TableName = "errorReason";
			return errorReason;
		}

        public DataTable GetErrorReasons(string cmpc_key)
		{
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT * FROM ERROR_REASON_TP WHERE CMPC_KEY = :cmpcKey";
            cmd.Parameters.Add(":cmpcKey", OracleDbType.Int32, Convert.ToInt32(cmpc_key), ParameterDirection.Input);

            DataTable errorReason = Util.CommandToDataTable(cmd);
			errorReason.TableName = "errorReason";
			return errorReason;
		}

		public DataTable GetErrorReason(string code)
		{
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT * FROM ERROR_REASON_TP WHERE CODE = :code";
            cmd.Parameters.Add(":code", OracleDbType.Varchar2, code, ParameterDirection.Input);

			DataTable errorReason = Util.CommandToDataTable(cmd);
			errorReason.TableName = "errorReason";
			return errorReason;
		}

		public DataTable GetErrorCategories()
		{
			string query = "SELECT * FROM ERROR_CATEGORY_TP";

			DataTable errorCategory = Util.SqlToDataTable(query);
			errorCategory.TableName = "errorCategory";
			return errorCategory;
		}

		public DataTable GetErrorCategories(string cmpc_key)
		{
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT * FROM ERROR_CATEGORY_TP WHERE CMPC_KEY = :cmpcKey";
            cmd.Parameters.Add(":cmpcKey", OracleDbType.Int32, Convert.ToInt32(cmpc_key), ParameterDirection.Input);

            DataTable errorCategory = Util.CommandToDataTable(cmd);
			errorCategory.TableName = "errorCategory";
			return errorCategory;
		}

		public DataTable GetErrorCategory(string code)
		{
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT * FROM ERROR_CATEGORY_TP WHERE CODE = :code";
            cmd.Parameters.Add(":code", OracleDbType.Varchar2, code, ParameterDirection.Input);

            DataTable errorCategory = Util.CommandToDataTable(cmd);
			errorCategory.TableName = "errorCategory";
			return errorCategory;
		}

		public DataTable GetMsaAreas()
		{
			string query = "SELECT DISTINCT MSA_KEY, NAME FROM MILESTONEAREA WHERE MSA_KEY>0 ORDER BY NAME";

			DataTable msaArea = Util.SqlToDataTable(query);
			msaArea.TableName = "msaArea";
			return msaArea;
		}

		public DataTable GetMsaArea(string msa_key)
		{
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT DISTINCT NAME FROM MILESTONEAREA WHERE MSA_KEY = :msaKey";
            cmd.Parameters.Add(":msaKey", OracleDbType.Int32, Convert.ToInt32(msa_key), ParameterDirection.Input);

            DataTable msaArea = Util.CommandToDataTable(cmd);
			msaArea.TableName = "msaArea";
			return msaArea;
		}

		public DataTable GetCollSystems()
		{
			string query = "SELECT OPUN_KEY, NAME, SHORNAME FROM OPRTUNIT WHERE OPRSTATU=12 ORDER BY NAME";

			DataTable collSystem = Util.SqlToDataTable(query);
			collSystem.TableName = "collSystem";
			return collSystem;
		}

		public DataTable GetOrgUnits()
		{
			string query = "SELECT DISTINCT ORG_UNIT FROM INSTALLATION ORDER BY ORG_UNIT";

			DataTable orgUnit = Util.SqlToDataTable(query);
			orgUnit.TableName = "orgUnit";
			return orgUnit;
		}

		public DataTable GetUnit(string unme_key)
		{
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT * FROM UNMEA_TP WHERE UNME_KEY = :unmeKey";
            cmd.Parameters.Add(":unmeKey", OracleDbType.Int32, Convert.ToInt32(unme_key), ParameterDirection.Input);

            DataTable unit = Util.CommandToDataTable(cmd);
			unit.TableName = "unit";
			return unit;
		}

		public DataTable GetRegisterResolution(string tsin_key)
		{
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT * FROM TSINT_TP WHERE TSIN_KEY = :tsinKey";
            cmd.Parameters.Add(":tsinKey", OracleDbType.Int32, Convert.ToInt32(tsin_key), ParameterDirection.Input);

            DataTable unit = Util.CommandToDataTable(cmd);
			unit.TableName = "resolution";
			return unit;
		}

		public DataTable GetRegisterType(string tser_key)
		{
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT * FROM TSERI_TP WHERE TSER_KEY = :tserKey";
            cmd.Parameters.Add(":tserKey", OracleDbType.Int32, Convert.ToInt32(tser_key), ParameterDirection.Input);

            DataTable unit = Util.CommandToDataTable(cmd);
			unit.TableName = "type";
			return unit;
		}

        public DataTable GetOprtunitBasicInfo(string opun_key)
		{
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT OPUN_KEY, NAME, SHORNAME, OPRSTATU FROM OPRTUNIT WHERE OPUN_KEY = :opunKey";
            cmd.Parameters.Add(":opunKey", OracleDbType.Int32, Convert.ToInt32(opun_key), ParameterDirection.Input);

            DataTable oprtunitBasicInfo = Util.CommandToDataTable(cmd);
			oprtunitBasicInfo.TableName = "oprtunitBasicInfo";
			return oprtunitBasicInfo;
		}

		public static DataTable GetOprtunitsBasicInfo(string nameCriteriaPrefix)
		{
            string nameCriteria = nameCriteriaPrefix + "%";
            
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT OPUN_KEY, NAME, SHORNAME, OPRSTATU FROM OPRTUNIT WHERE NAME LIKE :nameCriteria";
            cmd.Parameters.Add(":nameCriteria", OracleDbType.Varchar2, nameCriteria, ParameterDirection.Input);

            DataTable oprtunitsBasicInfo = Util.CommandToDataTable(cmd);
			oprtunitsBasicInfo.TableName = "oprtunitsBasicInfo";
			return oprtunitsBasicInfo;
		}

        public static DataTable GetOprtunitImpExpInfo(int opunKey)
        {
            // Used for finding sender and receiver info in DataExchangeAPI
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "SELECT O.OPUN_KEY, O.NAME, O.OPRSTATU, O.BANKACC, O.COUNTRY_KEY, C.CODE COUNTRY_CODE";
            cmd.CommandText += " FROM OPRTUNIT O, COUNTRY C WHERE O.COUNTRY_KEY = C.COUNTRY_KEY (+) AND O.OPUN_KEY = :opunKey";
            cmd.Parameters.Add(":opunKey", OracleDbType.Int32, Convert.ToInt32(opunKey), ParameterDirection.Input);

            DataTable oprtunitBasicInfo = Util.CommandToDataTable(cmd);
            oprtunitBasicInfo.TableName = "oprtunitImpExpInfo";
            return oprtunitBasicInfo;
        }

        public int CreateKeytabIndex(DataTable keyTable)
		{
			// first build the PDKeyTab with a list of keys
			PDKeyTab pkt = new PDKeyTab(keyTable.Rows.Count, Util.OpenConnection());
			int rcount = pkt.fetchSequenceNumber();
			for (int row=0; row<keyTable.Rows.Count; row++)
			{
				if (keyTable.Rows[row]["key1"] != DBNull.Value)
					pkt.AddLongValue1(row, Convert.ToInt32(keyTable.Rows[row]["key1"].ToString()));
				if (keyTable.Rows[row]["key2"] != DBNull.Value)
				  pkt.AddLongValue2(row, Convert.ToInt32(keyTable.Rows[row]["key2"].ToString()));
			}
			if (keyTable.Rows.Count > 0)
			{
				pkt.Execute();
				return rcount;
			}
			else
				return -1;
		}

		public DataTable GetListNums()
		{
			string query = "SELECT DISTINCT LISTNO FROM MEAS_PNT WHERE LISTNO != '-' AND LISTNO IS NOT NULL ORDER BY LISTNO";

			DataTable listNo = Util.SqlToDataTable(query);
			listNo.TableName = "listNo";
			return listNo;
		}

        public static void RecompileTsStore()
        {
            Util.ExecuteSql("alter package icc_tsstore compile body");
        }

        public static void ReloadPdDbEnv()
        {
            Console.WriteLine("ReloadPdDbEnv");

            OracleCommand cmd = new OracleCommand("icc_tsstore.reloadOverwriteCcPeriods");
            cmd.CommandType = CommandType.StoredProcedure;
            Util.ExecuteCommand(cmd);
        }

        public static void SetProgNumber(int iccProgram, IDbConnection connection)
        {
            OracleCommand cmd = new OracleCommand("pdmisc.set_prog_number");
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.Add("prog_key_value", iccProgram);

            Util.ExecuteCommand(cmd, connection);
        }

        public static string GetDbSchemaRelease(IDbConnection connection)
        {
            DataTable val = Util.SqlToDataTable("select value from pddbenv where name = '_ICC_SCHEMA_RELEASE'", connection);
            if (val.Rows.Count == 0)
                return "";
            return val.Rows[0][0].ToString();
        }
    }
}
