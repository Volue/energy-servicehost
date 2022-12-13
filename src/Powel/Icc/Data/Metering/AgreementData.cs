using System;
using System.Collections;
using System.Data;
using Oracle.ManagedDataAccess.Client;
using Powel.Icc.Data.Entities.Metering;
using Oracle.ManagedDataAccess.Types;
using Powel.Icc.Common;
using Powel.Icc.Services.Time;
using log4net;
using log4net.Config;

namespace Powel.Icc.Data.Metering
{
	/// <summary>
	/// Class for accessing Agreement-related data tables
	/// </summary>
	public abstract class AgreementData
	{
		private static ILog m_l4n = LogManager.GetLogger(typeof(AgreementData));

		public static int CreateTimeSeries(
			 int tsKey
			,string tsName
			,TsType tsType
			,MeasurePoint measurePoint
			,Register register
			,UtcTime timeOfUpdate
			,IDbConnection connection
			)
		{
			XmlConfigurator.Configure();
			m_l4n.Info("CreateTimeSeries");
			m_l4n.Info("..tsKey = " + tsKey);
			m_l4n.Info("..tsName = " + tsName);
			m_l4n.Info("..tsType = " + tsType);
			m_l4n.Info("..measurePoint = " + measurePoint);
			m_l4n.Info("..register = " + register);
			m_l4n.Info("..timeOfUpdate = " + timeOfUpdate);

			int nTS_Key_out = 0;

			try
			{

				RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
				OracleCommand cmd = new OracleCommand("ICC_METERING.CreateTimeSeries");
				
				cmd.CommandType = CommandType.StoredProcedure;
				
				cmd.Parameters.Add("nTS_Key_out", OracleDbType.Int32, ParameterDirection.ReturnValue);
				cmd.Parameters.Add("ItimsKey", OracleDbType.Int32, tsKey, ParameterDirection.Input);
				cmd.Parameters.Add("Itscode", OracleDbType.Varchar2,tsName, ParameterDirection.Input);
				cmd.Parameters.Add("ImeasKey", OracleDbType.Int32,measurePoint.Key, ParameterDirection.Input);
				cmd.Parameters.Add("ImeasSeqno", OracleDbType.Int32,measurePoint.Seqno, ParameterDirection.Input);
				cmd.Parameters.Add("ItsinKey", OracleDbType.Int32, register.WayOfRegistration, ParameterDirection.Input);
				cmd.Parameters.Add("ItserKey", OracleDbType.Int32, Convert.ToInt32(tsType), ParameterDirection.Input);
				cmd.Parameters.Add("IunmeKey", OracleDbType.Int32, register.MeasureUnitCode, ParameterDirection.Input);
				cmd.Parameters.Add("Idirection", OracleDbType.Int32, register.DirectionOfFlow, ParameterDirection.Input);
				cmd.Parameters.Add("IAutomatic", OracleDbType.Int32, register.RemoteMetering, ParameterDirection.Input);
				cmd.Parameters.Add("IvltpKey", OracleDbType.Int32, register.ValueType, ParameterDirection.Input);
				cmd.Parameters.Add("IvltpValftime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfUpdate), ParameterDirection.Input);
							
				Util.ExecuteCommand(cmd, connection);
							
				if(cmd.Parameters[0].Value == DBNull.Value)
					throw new DataException("Creation of time series failed");

				nTS_Key_out = Util.GetInt32(cmd.Parameters[0]);
				m_l4n.Info("..nTS_Key_out = " + nTS_Key_out);
			}
			finally
			{
				m_l4n.Info("CreateTimeSeries. Exit");
			}
			
			return nTS_Key_out;
		}

		public static ArrayList GetTimeSeries(MeasurePoint measurePoint, UtcTime validAtTime, IDbConnection connection)
		{
			if( measurePoint.Key == 0)
				throw new ArgumentException("AgreementData.GetTimeSeries: measurePoint.Key is 0");
			ArrayList timeSeries = new ArrayList();
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.TimeSeriesGetForMeasurePoint");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("ImeasKey", OracleDbType.Int32, measurePoint.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime), ParameterDirection.Input);
				
			Util.ExecuteCommand(cmd, connection);

			using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value)
			{
				using (OracleDataReader reader = cursor.GetDataReader())
				{
					while (reader.Read())
					{
						timeSeries.Add(Util.GetInt32(reader, "tims_key"));
					}
				}
			}
			return timeSeries;
		}

		public static bool FinishImpExpDef(int timeSeriesKey, Register register, string globeref, bool finishExport, string impRef, string expRef, UtcTime timeOfUpdate,	IDbConnection connection)
		{
				bool success = UpdateImpExpDef(timeSeriesKey, false, register, globeref, 1, 1, finishExport, impRef, expRef, Agreement.Frequency.NEVER, Agreement.Frequency.NEVER, null, timeOfUpdate, timeOfUpdate, connection);
				return success;
		}

		public static bool UpdateImpExpDef(int timeSeriesKey, Register register, string globeref, double importFactor, double exportFactor, bool createExport, string impRef, string expRef, Agreement.Frequency importFrequency, Agreement.Frequency exportMethod, Agreement agreement, UtcTime timeOfUpdate, UtcTime exportStartTime, IDbConnection connection)
		{
				bool success = UpdateImpExpDef(timeSeriesKey, true, register, globeref, importFactor, exportFactor, createExport, impRef, expRef, importFrequency, exportMethod, agreement, timeOfUpdate, exportStartTime, connection);
				return success;
		}

		private static bool UpdateImpExpDef(int timeSeriesKey, bool createNew, Register register, 
			string globeref, double importFactor, double exportFactor, 
			bool createExport, string impRef, string expRef, 
			Agreement.Frequency importFrequency, Agreement.Frequency exportMethod, 
			Agreement agreement, UtcTime timeOfUpdate, UtcTime exportStartTime, IDbConnection connection)
		{
			int newInstance = (createNew ? 1 : 0);
			int doExport = (createExport ? 1 : 0);
		
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.CheckTimeSeries");
			
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("InewInstr", OracleDbType.Int32, newInstance, ParameterDirection.Input);
			cmd.Parameters.Add("ItimsKey", OracleDbType.Int32, timeSeriesKey, ParameterDirection.Input);
			cmd.Parameters.Add("Itype", OracleDbType.Int32,exportMethod, ParameterDirection.Input);
			cmd.Parameters.Add("Igloberef", OracleDbType.Varchar2, globeref, ParameterDirection.Input);
			cmd.Parameters.Add("InDigits", OracleDbType.Int32, register.CapacityDigits, ParameterDirection.Input);
			cmd.Parameters.Add("InDecimals", OracleDbType.Int32, register.CapacityDecimals, ParameterDirection.Input);
			cmd.Parameters.Add("Ivalftime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfUpdate), ParameterDirection.Input);
			cmd.Parameters.Add("IhourMeter", OracleDbType.Int32, (register.WayOfRegistration==WayOfRegistrationType.DIFFERENTIAL?1:0), ParameterDirection.Input);
			cmd.Parameters.Add("IimportFactor", OracleDbType.Double, importFactor, ParameterDirection.Input);
			cmd.Parameters.Add("IexportFactor", OracleDbType.Double, exportFactor, ParameterDirection.Input);
			cmd.Parameters.Add("IdoExport", OracleDbType.Int32, doExport, ParameterDirection.Input);
			cmd.Parameters.Add("IimpRef", OracleDbType.Varchar2, impRef, ParameterDirection.Input);
			cmd.Parameters.Add("IexpRef", OracleDbType.Varchar2, expRef, ParameterDirection.Input);
			cmd.Parameters.Add("IimpFrequency", OracleDbType.Int32, importFrequency, ParameterDirection.Input);
			cmd.Parameters.Add("IimpActor", OracleDbType.Varchar2, agreement != null?agreement.SenderId:null, ParameterDirection.Input);
			cmd.Parameters.Add("IexpActor", OracleDbType.Varchar2, agreement != null?agreement.ReceiverId:null, ParameterDirection.Input);
			cmd.Parameters.Add("IdoImport", OracleDbType.Int32, 1, ParameterDirection.Input);
			cmd.Parameters.Add("Iexpvalftime", OracleDbType.Varchar2, cal.ToDatabaseString(exportStartTime), ParameterDirection.Input);

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);

			return true;
		}

		public static void SaveAgreement(Agreement agreement, MeasurePoint measurePoint, UtcTime validFrom, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.AgreementInsert");
			
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("iMeasKey", OracleDbType.Int32, measurePoint.Key, ParameterDirection.Input);
			cmd.Parameters.Add("iSeqno", OracleDbType.Int32, measurePoint.Seqno, ParameterDirection.Input);
			cmd.Parameters.Add("iDebitingType", OracleDbType.Int32, 0, ParameterDirection.Input); //NOT IN USE
			cmd.Parameters.Add("iReceivedDate", OracleDbType.Date, cal.ToDateTime(UtcTime.Now), ParameterDirection.Input);
			cmd.Parameters.Add("iValidFromDate", OracleDbType.Date, cal.ToDateTime(validFrom), ParameterDirection.Input);
			cmd.Parameters.Add("iSupplier", OracleDbType.Varchar2, agreement.SupplierId, ParameterDirection.Input);
			cmd.Parameters.Add("iSender", OracleDbType.Varchar2, agreement.SenderId, ParameterDirection.Input);
			cmd.Parameters.Add("iReceiver", OracleDbType.Varchar2, agreement.ReceiverId, ParameterDirection.Input);
			cmd.Parameters.Add("iPulseImp", OracleDbType.Int32, Convert.ToInt32(agreement.PulseImport), ParameterDirection.Input);
			cmd.Parameters.Add("iPulseExp", OracleDbType.Int32, Convert.ToInt32(agreement.PulseExport), ParameterDirection.Input);
			cmd.Parameters.Add("iTrafoImp", OracleDbType.Int32, Convert.ToInt32(agreement.TrafoImport), ParameterDirection.Input);
			cmd.Parameters.Add("iTrafoExp", OracleDbType.Int32, Convert.ToInt32(agreement.TrafoExport), ParameterDirection.Input);
			cmd.Parameters.Add("iIntervalAcc", OracleDbType.Int32, 0, ParameterDirection.Input); //NOT IN USE
			cmd.Parameters.Add("iIntervalDif", OracleDbType.Int32, 0, ParameterDirection.Input); //NOT IN USE
			
			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}

		public static Agreement[] GetForMeasurePoint(MeasurePoint measurePoint, TimePeriod validAt, IDbConnection connection)
		{
			ArrayList agreements = new ArrayList();
			Agreement a;
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.AgreementGetForMeasurePoint");
			
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("iMeasKey", OracleDbType.Int32, measurePoint.Key, ParameterDirection.Input);
			cmd.Parameters.Add("iSeqno", OracleDbType.Int32, measurePoint.Seqno, ParameterDirection.Input);
			cmd.Parameters.Add("iValidFrom", OracleDbType.Date, cal.ToDateTime(validAt.From), ParameterDirection.Input);
			cmd.Parameters.Add("iValidTo", OracleDbType.Date, cal.ToDateTime(validAt.Until), ParameterDirection.Input);
	
			Util.ExecuteCommand(cmd, connection);

			using (OracleRefCursor cursor = (OracleRefCursor)cmd.Parameters[0].Value)
			{
				using (OracleDataReader reader = cursor.GetDataReader())
				{
					while (reader.Read())
					{
						a = new Agreement(
							Util.GetInt32(reader, "agreement_key"),
							Util.GetDate(reader, "valid_from"),
							Util.GetDate(reader, "valid_to"),
							measurePoint.Id,
							null, /*TARIFF_CODE*/
							measurePoint.Supplier,
							//TODO (Agreement.DebitingType) Util.GetInt32(reader, "debitingtype"),
							Agreement.ExportFormatType.GS2, /*TODO export format*/
							Convert.ToBoolean(Util.GetInt32(reader, "imp_pulse")),
														Convert.ToBoolean(Util.GetInt32(reader, "imp_trafo")),
														Convert.ToBoolean(Util.GetInt32(reader, "exp_pulse")),
														Convert.ToBoolean(Util.GetInt32(reader, "exp_trafo")),
							//TODO (Agreement.TransferMethod) Util.GetInt32(reader, "interval_dif"),
							//TODO (Agreement.TransferMethod) Util.GetInt32(reader, "interval_acc"),
							Util.GetString(reader, "senderID"),
							Util.GetInt32(reader, "sender"),
							Util.GetString(reader, "receiverID"),
							Util.GetInt32(reader, "receiver"),
							null //commodities
							);
						agreements.Add(a);
					}
				}
			}
			return (Agreement[]) agreements.ToArray(typeof(Agreement));
		}

		public static int FetchNewTSKey(string tsName, UtcTime validFrom, IDbConnection connection)
		{

			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			OracleCommand cmd = new OracleCommand("ICC_METERING.TimeSeriesFetchNewKey");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("tsKey", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("Its_code", OracleDbType.Varchar2, tsName, ParameterDirection.Input);
		 
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

			return Util.GetInt32(cmd.Parameters["tsKey"]);
		}

		public static void DeleteByTSKey(int tsKey, IDbConnection connection)
		{
				OracleCommand cmd = new OracleCommand("ICC_METERING.TimeSeriesDeleteByKey");
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.Parameters.Add("ItsKey", OracleDbType.Int32, tsKey, ParameterDirection.Input);

				MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}
	}
}
