using System;
using System.Collections;
using System.Data;
using System.Text;
using Powel.Icc.Services.Time;
using Powel.Icc.Common;
using Powel.Icc.Data.Entities;
using Powel.Icc.Data.Entities.Metering;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;


namespace Powel.Icc.Data.Metering
{
	/// <summary>
	/// Summary description for MeteringOrderData.
	/// </summary>
	public class MeteringOrderData
	{
		public static ArrayList GetNewOnDemandMeteringOrders(IDbConnection connection)
		{			
			OracleCommand cmd = new OracleCommand("ICC_METERING.GetNewOnDemandExportOrders");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
						
			Util.ExecuteCommand(cmd, connection);

			return GetOnDemandMeteringOrdersFromCursor((OracleRefCursor)cmd.Parameters[0].Value, connection);
		}

		public static ArrayList GetDifferentialOnDemandMeteringOrders(UtcTime before, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.GetDiffOnDemandExportOrders");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("iTransferBefore", OracleDbType.Date, cal.ToDateTime(before), ParameterDirection.Input);
						
			Util.ExecuteCommand(cmd, connection);

			return GetOnDemandMeteringOrdersFromCursor((OracleRefCursor)cmd.Parameters[0].Value, connection);
		}

		private static ArrayList GetOnDemandMeteringOrdersFromCursor(OracleRefCursor refCursor, IDbConnection connection)
		{
			ArrayList alMeteringOrdersOnDemandCollections = new ArrayList();
			MeteringOrderOnDemandCollection moodCol = null;
			UtcTime transferTime, lastTransferTime = UtcTime.Null;
			UtcTime dataFromTime, lastDataFromTime = UtcTime.Null;
			UtcTime dataToTime, lastDataToTime = UtcTime.Null;
			RegistrationReason reason, lastReason = RegistrationReason.UNDEFINED;
			int timsKey, opunKey, seqno;
			bool first = true;

			using (refCursor)
			{
				using (OracleDataReader reader = refCursor.GetDataReader())
				{
					while (reader.Read())
					{
						//already grouped during query
						transferTime = Util.GetDate(reader, "transfer_time");
						dataFromTime = Util.GetDate(reader, "datafrom_time");
						dataToTime = Util.GetDate(reader, "datato_time");
						reason = RegistrationReason.UNDEFINED;  //This functionality is no longer supported! (RegistrationReason) Util.GetInt32(reader, "reas_key");
						timsKey = Util.GetInt32(reader, "tims_key");
						opunKey = Util.GetInt32(reader, "opun_key");
						seqno = Util.GetInt32(reader, "seqno");			
						if( first)
						{
							moodCol = new MeteringOrderOnDemandCollection(transferTime, dataFromTime, dataToTime, reason);
							first = false;
						}
						else if(transferTime != lastTransferTime || dataFromTime != lastDataFromTime ||
							dataToTime != lastDataToTime || reason != lastReason)
						{
							//store current meteringOrderOnDemand
							alMeteringOrdersOnDemandCollections.Add(moodCol);
							moodCol = new MeteringOrderOnDemandCollection(transferTime, dataFromTime, dataToTime, reason);
							lastTransferTime = transferTime;
							lastDataFromTime = dataFromTime;
							lastDataToTime = dataToTime;
							lastReason = reason;
						}

						moodCol.AddMeteringOrderOnDemand(new MeteringOrderOnDemand(timsKey, opunKey, seqno));
					}
					//add last metering order on demand if existing
					if( moodCol != null)
						alMeteringOrdersOnDemandCollections.Add(moodCol);
				}
			}
			return alMeteringOrdersOnDemandCollections;
		}

		public static void MarkNewOnDemandMeteringOrders(MeteringOrderOnDemandCollection meteringOrdersOnDemandCollection, MeteringOrderOnDemand.Status status, IDbConnection connection)
		{
			MeteringOrderOnDemandCollection[] moodCols= new MeteringOrderOnDemandCollection[1];
			moodCols[0] = meteringOrdersOnDemandCollection;
			MarkNewOnDemandMeteringOrders(moodCols, status, connection);

		}

		public static void MarkNewOnDemandMeteringOrders(MeteringOrderOnDemandCollection[] meteringOrdersOnDemandCollections, MeteringOrderOnDemand.Status status, IDbConnection connection)
		{
			ArrayList alTimsKeys = new ArrayList();
			ArrayList alOpunKeys = new ArrayList();
			ArrayList alSeqnos = new ArrayList();
			int numbersOfOrders = 0;
			foreach(MeteringOrderOnDemandCollection moodCol in meteringOrdersOnDemandCollections)
			{
				foreach(MeteringOrderOnDemand mood in moodCol.MeteringOrdersOnDemand)
				{
					alTimsKeys.Add(mood.timsKey);
					alOpunKeys.Add(mood.opunKey);
					alSeqnos.Add(mood.seqno);
					numbersOfOrders++;
				}
			}
			if( numbersOfOrders > 0)
			{
				OracleCommand cmd = new OracleCommand("ICC_METERING.MarkOnDemandExportOrders");
				cmd.CommandType = CommandType.StoredProcedure;
				OracleParameter par;

				par = cmd.Parameters.Add("iTimsKeys", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = (int[]) alTimsKeys.ToArray(typeof(int));

				par = cmd.Parameters.Add("iOpunKeys", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = (int[]) alOpunKeys.ToArray(typeof(int));

				par = cmd.Parameters.Add("iSeqnos", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = (int[]) alSeqnos.ToArray(typeof(int));

				par = cmd.Parameters.Add("iNumberOfOrders", OracleDbType.Int32);
				par.Value = numbersOfOrders;
			
				par = cmd.Parameters.Add("iStatus", OracleDbType.Int32);
				par.Value = (int) status;

				Util.ExecuteCommand(cmd, connection);
			}
		}

		public static void SaveOnDemandMeteringOrder(int timsKey, string receiver, WayOfRegistrationType diffOrAcc, UtcTime transferDate, UtcTime dataFromTime,
			UtcTime dataToTime, RegistrationReason reasonKey, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			
			OracleCommand cmd = new OracleCommand("ICC_METERING.SaveOnDemandExportOrder");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("iTimsKey", OracleDbType.Int32, timsKey, ParameterDirection.Input);
			cmd.Parameters.Add("iRecevier", OracleDbType.Varchar2, receiver, ParameterDirection.Input);
			cmd.Parameters.Add("iWayOfRegistration", OracleDbType.Int32, diffOrAcc, ParameterDirection.Input);
			cmd.Parameters.Add("iTransferDate", OracleDbType.Date, cal.ToDateTime(transferDate), ParameterDirection.Input);
			cmd.Parameters.Add("iDataFromTime", OracleDbType.Date, cal.ToDateTime(dataFromTime), ParameterDirection.Input);
			cmd.Parameters.Add("iDataToTime", OracleDbType.Date, cal.ToDateTime(dataToTime), ParameterDirection.Input);
			cmd.Parameters.Add("iReasKey", OracleDbType.Int32, reasonKey, ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);  
		}

		public static void EndAccumulativeExportOrders(int[] timsKeys, UtcTime validFrom, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			if (timsKeys.Length > 0)
			{
				OracleCommand cmd;
				OracleParameter par;

				//end any active export orders
				cmd = new OracleCommand("ICC_METERING.EndAccumulativeExportOrders");
				cmd.CommandType = CommandType.StoredProcedure;

				par = cmd.Parameters.Add("iTimsKeys", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = timsKeys;

				cmd.Parameters.Add("iValidFrom", OracleDbType.Varchar2, cal.ToDatabaseString(validFrom), ParameterDirection.Input);
				cmd.Parameters.Add("iNumberOfOrders", OracleDbType.Int32, timsKeys.Length, ParameterDirection.Input);

				Util.ExecuteCommand(cmd, connection);
			}
		}

        public static void EndAccumulativeExportOrder(int timsKey, string receiver, UtcTime validFrom, IDbConnection connection)
        {
            RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

            //end any active export orders
            var cmd = new OracleCommand("ICC_METERING.EndAccumulativeExportOrder");
            cmd.CommandType = CommandType.StoredProcedure;
            cmd.Parameters.Add("iTimsKey", OracleDbType.Decimal, timsKey, ParameterDirection.Input);
            cmd.Parameters.Add("iReceiver", OracleDbType.Varchar2, receiver, ParameterDirection.Input);
            cmd.Parameters.Add("iValidFrom", OracleDbType.Varchar2, cal.ToDatabaseString(validFrom), ParameterDirection.Input);

            Util.ExecuteCommand(cmd, connection);
        }

        public static void SaveAccumulativeExportOrder(int[] timsKeys, string receiver, UtcTime timestamp, int hoursOffsetBackward, int hoursOffsetForward, TsExpDefInterval interval, UtcTime validFrom, UtcTime validTo, RegistrationReason reason, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			if( timsKeys.Length > 0)
			{
				OracleCommand cmd;
				OracleParameter par;

				cmd = new OracleCommand("ICC_METERING.SaveAccumulativeExportOrders");
				cmd.CommandType = CommandType.StoredProcedure;

				par = cmd.Parameters.Add("iTimsKeys", OracleDbType.Decimal);
				par.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
				par.Value = timsKeys;
				
				cmd.Parameters.Add("iReceiver", OracleDbType.Varchar2, receiver, ParameterDirection.Input);
				cmd.Parameters.Add("iDateTime", OracleDbType.Varchar2, cal.ToDatabaseString(timestamp), ParameterDirection.Input);
				cmd.Parameters.Add("iHoursOffsetBackward", OracleDbType.Int32, hoursOffsetBackward, ParameterDirection.Input);
				cmd.Parameters.Add("iHoursOffsetForward", OracleDbType.Int32, hoursOffsetForward, ParameterDirection.Input);
				cmd.Parameters.Add("iInterval", OracleDbType.Int32, (int) interval, ParameterDirection.Input);
				cmd.Parameters.Add("iValidFrom", OracleDbType.Varchar2, cal.ToDatabaseString(validFrom), ParameterDirection.Input);
				cmd.Parameters.Add("iValidTo", OracleDbType.Varchar2, cal.ToDatabaseString(validTo), ParameterDirection.Input);
				cmd.Parameters.Add("iReasKey", OracleDbType.Int32, (int) reason, ParameterDirection.Input);
				cmd.Parameters.Add("iNumberOfOrders", OracleDbType.Int32, timsKeys.Length, ParameterDirection.Input);
				
				Util.ExecuteCommand(cmd, connection);
			}
		}
		
	}
}
