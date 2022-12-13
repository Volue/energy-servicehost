using System;
using System.Data;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using Powel.Icc.Common;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Services.Time;
using System.Collections.Generic;



namespace Powel.Icc.Data.Metering
{
	/// <summary>
	/// Class for accessing Installation-related data tables
	/// </summary>
	public class InstallationData
	{
		public InstallationData()
		{
		}

		
		public static int Update(Installation inst, IDbConnection connection)
		{
			if(inst.ValidFromDate == UtcTime.Null)
				throw new ArgumentException("InstallationData.Update() needs installation.ValidFromDate to be set.");
				
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.InstallationUpdate");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("Iinsl_key", OracleDbType.Int32, inst.Key, ParameterDirection.Input);
			cmd.Parameters.Add("seqno", OracleDbType.Int32, inst.Seqno, ParameterDirection.Input);
			cmd.Parameters.Add("Iinstname", OracleDbType.Varchar2,inst.Id, ParameterDirection.Input);
			cmd.Parameters.Add("Iinstname2", OracleDbType.Varchar2, inst.SecondaryId, ParameterDirection.Input);
			cmd.Parameters.Add("Idescript", OracleDbType.Varchar2, inst.Description, ParameterDirection.Input);
			cmd.Parameters.Add("Iinstallation_code", OracleDbType.Varchar2, inst.Code, ParameterDirection.Input);
			cmd.Parameters.Add("Ivalftime", OracleDbType.Varchar2, cal.ToDatabaseString(inst.ValidFromDate), ParameterDirection.Input);
			cmd.Parameters.Add("Ivalttime", OracleDbType.Varchar2, cal.ToDatabaseString(inst.ValidToDate), ParameterDirection.Input);
			cmd.Parameters.Add("Icustomer_name", OracleDbType.Varchar2, inst.CustomerName, ParameterDirection.Input);
			cmd.Parameters.Add("Icustomer_id", OracleDbType.Varchar2, inst.CustomerId, ParameterDirection.Input);
			cmd.Parameters.Add("IapartmentNo", OracleDbType.Varchar2, inst.ApartmentNo, ParameterDirection.Input);
            cmd.Parameters.Add("IhouseNo", OracleDbType.Varchar2, inst.HouseNo, ParameterDirection.Input);
			cmd.Parameters.Add("Iaddress", OracleDbType.Varchar2, inst.AddressMain, ParameterDirection.Input);
			cmd.Parameters.Add("Ico_info", OracleDbType.Varchar2, inst.CoInfoMain, ParameterDirection.Input);
			cmd.Parameters.Add("Ipostcode", OracleDbType.Varchar2, inst.PostCodeMain, ParameterDirection.Input);
			cmd.Parameters.Add("Iposttown", OracleDbType.Varchar2, inst.CityMain, ParameterDirection.Input);
			cmd.Parameters.Add("Iaddress2", OracleDbType.Varchar2, inst.AddressContact, ParameterDirection.Input);
			cmd.Parameters.Add("Ico_info2", OracleDbType.Varchar2, inst.CoInfoContact, ParameterDirection.Input);
			cmd.Parameters.Add("Ipostcode2", OracleDbType.Varchar2, inst.PostCodeContact, ParameterDirection.Input);
			cmd.Parameters.Add("Iposttown2", OracleDbType.Varchar2, inst.CityContact, ParameterDirection.Input);
			cmd.Parameters.Add("Iphone_home", OracleDbType.Varchar2, inst.PhoneHome, ParameterDirection.Input);
			cmd.Parameters.Add("Iphone_work", OracleDbType.Varchar2, inst.PhoneWork, ParameterDirection.Input);
			cmd.Parameters.Add("Iphone_mobile", OracleDbType.Varchar2, inst.PhoneMobile, ParameterDirection.Input);


			cmd = ParameterNullifier(cmd);

            try
            {
                MeteringData.ExecuteMeteringDBCommand(cmd, connection);
            }
            catch(OracleException oe)
            {
                if(oe.Number==1) //unique constraint
                {
                    IccException ie = new IccException(1012, new string[] {oe.Message});
                    throw ie;
                }
                
                throw oe;
            }
            
            int n = Util.GetInt32(cmd.Parameters["nError"]); //returns insl_key of updated installation or NEGATIVE error-key

			if( n > 0)
			{
				if( inst.Key == 0)
					inst.Key = n; //update installation if new
			}
			return n;
		}

		public static Installation GetByKey(int insl_key)
		{
			return GetByKey(insl_key, UtcTime.Null, null);
		}

		public static Installation GetByKey(int insl_key, UtcTime validAtTime)
		{
			return GetByKey(insl_key, validAtTime, null);
		}

		public static Installation GetByKey(int insl_key, UtcTime validAtTime, IDbConnection connection)
		{
			Installation inst = null;
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.InstallationGetByKey");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("Iinsl_key", OracleDbType.Int32,insl_key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime), ParameterDirection.Input);
			
			Util.ExecuteCommand(cmd, connection);

			using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value)
			{
				using (OracleDataReader reader = cursor.GetDataReader())
				{
					if (reader.Read())
						inst = new Installation(Util.GetInt32(reader, "insl_key"),
												Util.GetInt32(reader, "seqno"),
												Util.GetString(reader, "name"),
												Util.GetString(reader, "name2"),
												Util.GetString(reader, "descript"),
												Util.GetString(reader, "code"),
												cal.ToUtcTime(Util.GetString(reader, "valftime")),
                            					cal.ToUtcTime(Util.GetString(reader, "valttime")),							
												Util.GetString(reader, "custname"),
												Util.GetString(reader, "customer_id"),
												Util.GetString(reader, "apartNo"),
                                                Util.GetString(reader, "houseNo"),
												Util.GetString(reader, "address"),
												Util.GetString(reader, "co_info"),
												Util.GetString(reader, "postcode"),
												Util.GetString(reader, "posttown"),
												Util.GetString(reader, "address2"),
												Util.GetString(reader, "co_info2"),
												Util.GetString(reader, "postcode2"),
												Util.GetString(reader, "posttown2"),
												Util.GetString(reader, "phone_home"),
												Util.GetString(reader, "phone_work"),
												Util.GetString(reader, "phone_mobile"));
				}
			}

			return inst;			
		}

		public static Installation GetByID(string installationID)
		{
			using (IDbConnection connection = Util.OpenConnection())
			{
				return GetByID(installationID, UtcTime.Null, null);
			}
		}

		public static Installation GetByID(string installationID, IDbConnection connection)
		{
			return GetByID(installationID, UtcTime.Null, connection);			
		}

		public static Installation GetByID(string installationID, UtcTime validAtTime)
		{
			using( IDbConnection connection = Util.OpenConnection())
			{
				return GetByID(installationID, validAtTime, null);
			}
		}

		public static Installation GetByID(string installationID, UtcTime validAtTime, IDbConnection connection)
		{
			if (connection == null)
				throw new ApplicationException("An open connection is required as input to InstallationData.GetByID(...,...,...)");
			Installation inst = null;
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.InstallationGetByName");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("Iname", OracleDbType.Varchar2,installationID, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2,cal.ToDatabaseString(validAtTime), ParameterDirection.Input);
			
			Util.ExecuteCommand(cmd, connection);

			using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value)
			{
				using (OracleDataReader reader = cursor.GetDataReader())
				{
					if (reader.Read())
						inst = new Installation(
							Util.GetInt32(reader, "insl_key"),
							Util.GetInt32(reader, "seqno"),
							Util.GetString(reader, "name"),
							Util.GetString(reader, "name2"),
							Util.GetString(reader, "descript"),
							Util.GetString(reader, "code"),
							cal.ToUtcTime(Util.GetString(reader, "valftime")),
							cal.ToUtcTime(Util.GetString(reader, "valttime")),							
							Util.GetString(reader, "custname"),
							Util.GetString(reader, "customer_id"),
							Util.GetString(reader, "apartNo"),
                            Util.GetString(reader, "houseNo"),
							Util.GetString(reader, "address"),
							Util.GetString(reader, "co_info"),
							Util.GetString(reader, "postcode"),
							Util.GetString(reader, "posttown"),
							Util.GetString(reader, "address2"),
							Util.GetString(reader, "co_info2"),
							Util.GetString(reader, "postcode2"),
							Util.GetString(reader, "posttown2"),
							Util.GetString(reader, "phone_home"),
							Util.GetString(reader, "phone_work"),
							Util.GetString(reader, "phone_mobile"));
				}
			}

			return inst;			
		}


		/// <summary>
		/// Gets all valid installations of a given grid area at a given time
		/// </summary>
		public static List<Installation> GetForGridArea(string gridAreaName, UtcTime validAtTime)
		{
			using (IDbConnection connection = Util.OpenConnection())
			{
				return GetForGridArea(gridAreaName, validAtTime, connection);
			}
		}

		/// <summary>
		/// Gets all valid installations of a given grid area at a given time
		/// </summary>
		public static List<Installation> GetForGridArea(string gridAreaName, UtcTime validAtTime, IDbConnection connection)
		{
			if (connection == null)
				throw new ApplicationException("An open connection is required as input to InstallationData.GetForGridArea(...,...,...)");

			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			OracleCommand cmd = new OracleCommand(
	@"SELECT I.* FROM NETAREA NA, VIEW_INSTALLATION I 
WHERE NA.NAME = :1 
AND NA.NETA_KEY = I.NETA_KEY 
AND I.VALFTIME <= :2 AND I.VALTTIME > :3 ");
			cmd.Parameters.Add(null, gridAreaName);
			cmd.Parameters.Add(null, cal.ToDatabaseString(validAtTime));
			cmd.Parameters.Add(null, cal.ToDatabaseString(validAtTime));

			DataTable dt = Util.CommandToDataTable(cmd, connection);
			dt.TableName = "installation";

			List<Installation> installationList = new List<Installation>();
			Installation inst;
			foreach (DataRow dataRow in dt.Rows)
			{
				inst = new Installation(
					Util.GetInt32(dataRow, "insl_key"),
					Util.GetInt32(dataRow, "seqno"),
					Util.GetString(dataRow, "name"),
					Util.GetString(dataRow, "name2"),
					Util.GetString(dataRow, "descript"),
					Util.GetString(dataRow, "code"),
					cal.ToUtcTime(Util.GetString(dataRow, "valftime")),
					cal.ToUtcTime(Util.GetString(dataRow, "valttime")),
					Util.GetString(dataRow, "custname"),
					Util.GetString(dataRow, "customer_id"),
					Util.GetString(dataRow, "apartNo"),
                    Util.GetString(dataRow, "houseNo"),
					Util.GetString(dataRow, "address"),
					Util.GetString(dataRow, "co_info"),
					Util.GetString(dataRow, "postcode"),
					Util.GetString(dataRow, "posttown"),
					Util.GetString(dataRow, "address2"),
					Util.GetString(dataRow, "co_info2"),
					Util.GetString(dataRow, "postcode2"),
					Util.GetString(dataRow, "posttown2"),
					Util.GetString(dataRow, "phone_home"),
					Util.GetString(dataRow, "phone_work"),
					Util.GetString(dataRow, "phone_mobile"));
				installationList.Add(inst);
			}
			return installationList;

		}

		public static void Finish(Installation installation, UtcTime finishTime,bool finishMeasurePoints, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.InstallationFinish");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("Iinsl_key", OracleDbType.Int32, installation.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IfinishTime", OracleDbType.Varchar2, cal.ToDatabaseString(finishTime), ParameterDirection.Input);
			cmd.Parameters.Add("IfinishMeasurePoints", OracleDbType.Int32, finishMeasurePoints, ParameterDirection.Input);
			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}

		/// <summary>
		/// Creates a new version of an inactive installation.
		/// </summary>
		public static Installation CreateNewActiveVersion(Installation installation, UtcTime timeOfChange, IDbConnection connection)
		{	
			installation.Seqno = 0;
			installation.ValidFromDate = timeOfChange;
			installation.ValidToDate = UtcTime.MaxValue;
			Update(installation, connection);

			return GetByID(installation.Id,timeOfChange, connection);

		}
        /// <summary>
        /// Creates a new version of the installation. Includes finishing current version, creating new version and copying of measure point connections from old to new version.
        /// </summary>
        public static Installation CreateNewVersion(Installation installation, UtcTime timeOfChange, IDbConnection connection)
        {

            installation.MeasurePoints = new MeasurePoint[0];
            Installation oldInstallation = new Installation(installation);

            installation.Seqno = 0;//Is this really needed?
            installation.ValidFromDate = timeOfChange;
            installation.ValidToDate = UtcTime.MaxValue;
            //Finish installation, but not the MPs
            Finish(oldInstallation, timeOfChange, false, connection);
            Update(installation, connection);

            return GetByID(installation.Id, timeOfChange, connection);

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
				}

			}
			return cmd;
		}

        public static int FetchNewKey(string installationID,UtcTime validFrom, IDbConnection connection)
        {

            RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

            OracleCommand cmd = new OracleCommand("ICC_METERING.InstallationFetchNewKey");
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.Add("inslKey", OracleDbType.Int32, ParameterDirection.ReturnValue);
            cmd.Parameters.Add("Iinstname", OracleDbType.Varchar2, installationID, ParameterDirection.Input);
            cmd.Parameters.Add("Ivalftime", OracleDbType.Varchar2, cal.ToDatabaseString(validFrom), ParameterDirection.Input);

            cmd = ParameterNullifier(cmd);

            try
            {
                MeteringData.ExecuteMeteringDBCommand(cmd, connection);
            }
            catch (OracleException oe)
            {
                if (oe.Number == 1)//unique constraint
                {
                    IccException ie = new IccException(1012, new string[] { oe.Message });
                    throw ie;
                }

                throw oe;
            }

            return Util.GetInt32(cmd.Parameters["inslKey"]);
        }

        public static void DeleteByKey(int installationKey, IDbConnection connection)
        {
            OracleCommand cmd = new OracleCommand("ICC_METERING.InstallationDeleteByKey");
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.Add("Iinsl_key", OracleDbType.Int32,installationKey, ParameterDirection.Input);

            cmd = ParameterNullifier(cmd);

            MeteringData.ExecuteMeteringDBCommand(cmd, connection);
        }
	}
}
