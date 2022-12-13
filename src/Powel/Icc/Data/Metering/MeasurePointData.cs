using System;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using Powel.Icc.Common;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data.Metering
{
	/// <summary>
	/// Class for accessing MeasurePoint-related data tables
	/// </summary>
	public class MeasurePointData
	{
		/// <summary>
		/// Stores a measure point whose insl_key has been set. Otherwise use overloaded method which takes an Installation as parameter.
		/// </summary>
		/// <param name="measurePoint"></param>
		/// <param name="connection"></param>
		public static void Update(MeasurePoint measurePoint, IDbConnection connection)
		{
            Update(measurePoint, null, measurePoint.CentralNetPointCode, connection);
		}

        public static void Update(MeasurePoint measurePoint, Installation relatedInstallation, string centralNetPointCode, IDbConnection connection)
		{
            Update(measurePoint, relatedInstallation, centralNetPointCode, UtcTime.Null, connection);
		}

		/// <summary>
		/// Stores a measure point. relatedInstallation is necessary if measurePoint.InstallationKey is not set.
		/// </summary>
        public static void Update(MeasurePoint measurePoint, Installation relatedInstallation, string centralNetPointCode, UtcTime timeOfChange, IDbConnection connection)
		{
			int instSeqno;

			if (measurePoint.ValidFromDate == UtcTime.Null)
			{
				throw new ArgumentException("Measurement.Update() needs measurePoint.ValidFromDate to be set.");
			}
			if (measurePoint.InstallationKey == 0)
			{
				if (relatedInstallation == null)
				{
					throw new ArgumentException("Measurepoint.Update() needs a related installation, either by passing one as an argument, or by providing an InstallationKey on the measure point.");
				}
				if (relatedInstallation.Key == 0)
				{
					throw new ArgumentException(
					                            "MeasurePoint.Update() needs relatedInstallation to have a valid Key (not 0)");
				}

				measurePoint.InstallationKey = relatedInstallation.Key;
			}

			if (relatedInstallation != null)
			{
				instSeqno = relatedInstallation.Seqno;
			}
			else
			{
				instSeqno = 0; //TODO: needs to be tested
			}

			//check if NetArea is CentralNetPoint and change properties accordingly
			CheckNetArea(measurePoint, connection);

			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			var cmd = new OracleCommand("ICC_METERING.MeasurePointUpdate");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("IMeasKey", OracleDbType.Int32, measurePoint.Key, ParameterDirection.InputOutput);
			cmd.Parameters.Add("ISeqno", OracleDbType.Int32, measurePoint.Seqno, ParameterDirection.InputOutput);
			cmd.Parameters.Add("Iinsl_key", OracleDbType.Int32, measurePoint.InstallationKey, ParameterDirection.Input);
			cmd.Parameters.Add("Iinst_seqno", OracleDbType.Int32, instSeqno, ParameterDirection.InputOutput);
			cmd.Parameters.Add("ImeasName", OracleDbType.Varchar2, measurePoint.Id, ParameterDirection.Input);
			cmd.Parameters.Add("ImeasName2", OracleDbType.Varchar2, measurePoint.SecondaryId, ParameterDirection.Input);
			cmd.Parameters.Add("INetArea_Code", OracleDbType.Varchar2, measurePoint.NetAreaCode,
			                   ParameterDirection.Input);
			cmd.Parameters.Add("IDescript", OracleDbType.Varchar2, measurePoint.Description, ParameterDirection.Input);
			cmd.Parameters.Add("Ivalftime", OracleDbType.Varchar2, cal.ToDatabaseString(measurePoint.ValidFromDate),
			                   ParameterDirection.Input);
			cmd.Parameters.Add("Ivalttime", OracleDbType.Varchar2, cal.ToDatabaseString(measurePoint.ValidToDate),
			                   ParameterDirection.Input);
			cmd.Parameters.Add("IeanNumber", OracleDbType.Varchar2, "", ParameterDirection.Input);
			cmd.Parameters.Add("IXCoordinate", OracleDbType.Single, measurePoint.XCoordinate, ParameterDirection.Input);
			cmd.Parameters.Add("IYCoordinate", OracleDbType.Single, measurePoint.YCoordinate, ParameterDirection.Input);
            cmd.Parameters.Add("IStationID", OracleDbType.Varchar2, measurePoint.StationId, ParameterDirection.Input);
            cmd.Parameters.Add("INPhases", OracleDbType.Int32, measurePoint.NumberOfPhases, ParameterDirection.Input);
			cmd.Parameters.Add("ISupplyVoltage", OracleDbType.Varchar2, measurePoint.SupplyVoltage,
			                   ParameterDirection.Input);
            cmd.Parameters.Add("IFuseSize", OracleDbType.Varchar2, measurePoint.FuseSize, ParameterDirection.Input);
			cmd.Parameters.Add("IYearlyVolume", OracleDbType.Single, Convert.ToSingle(measurePoint.YearlyVolume),
			                   ParameterDirection.Input);
			cmd.Parameters.Add("ISubscribedPower", OracleDbType.Int32, measurePoint.SubscribedPower,
			                   ParameterDirection.Input);
            cmd.Parameters.Add("IOrgUnit", OracleDbType.Varchar2, measurePoint.OrgUnit, ParameterDirection.Input);
            cmd.Parameters.Add("IGridOwner", OracleDbType.Varchar2, measurePoint.GridOwner, ParameterDirection.Input);
            cmd.Parameters.Add("ILoadProfile", OracleDbType.Varchar2, measurePoint.LoadProfile, ParameterDirection.Input);
            cmd.Parameters.Add("ILocation", OracleDbType.Varchar2, measurePoint.Location, ParameterDirection.Input);
            cmd.Parameters.Add("ISupplier", OracleDbType.Varchar2, measurePoint.Supplier, ParameterDirection.Input);
			
			//add CentralNetPointCode
            if (String.IsNullOrEmpty(centralNetPointCode))
			{
				cmd.Parameters.Add("ICentralNetPointCode", OracleDbType.Varchar2, DBNull.Value, ParameterDirection.Input);
			}
			else
			{
                cmd.Parameters.Add("ICentralNetPointCode", OracleDbType.Varchar2, centralNetPointCode,
				                   ParameterDirection.Input);
			}
			
			//add DebtorType
            switch (measurePoint.DebtorType)
			{
				case MeasurePoint.DebtorTypes.Unspecified:
					cmd.Parameters.Add("IDebtorType", OracleDbType.Int32, null, ParameterDirection.Input);
					break;
				case MeasurePoint.DebtorTypes.Hourly:
					cmd.Parameters.Add("IDebtorType", OracleDbType.Int32, 0, ParameterDirection.Input);
					break;
				case MeasurePoint.DebtorTypes.MeterReadings:
					cmd.Parameters.Add("IDebtorType", OracleDbType.Int32, 1, ParameterDirection.Input);
					break;
			}
            //add State
            switch (measurePoint.State)
            {
                case MeasurePoint.StateTypes.Unspecified:
                    cmd.Parameters.Add("IState", OracleDbType.Int32, null, ParameterDirection.Input);
                    break;
                case MeasurePoint.StateTypes.Closed:
                    cmd.Parameters.Add("IState", OracleDbType.Int32, 0, ParameterDirection.Input);
                    break;
                case MeasurePoint.StateTypes.Active:
                    cmd.Parameters.Add("IState", OracleDbType.Int32, 1, ParameterDirection.Input);
                    break;
                case MeasurePoint.StateTypes.NoCustomer:
                    cmd.Parameters.Add("IState", OracleDbType.Int32, 2, ParameterDirection.Input);
                    break;
            }

			if (timeOfChange == UtcTime.Null)
				cmd.Parameters.Add("ITimeOfChange", OracleDbType.Varchar2, null, ParameterDirection.Input);
			else
				cmd.Parameters.Add("ITimeOfChange", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfChange), ParameterDirection.Input);

			cmd = ParameterNullifier(cmd);

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);

            if (measurePoint.Key == 0)
			{
                measurePoint.Key = Util.GetInt32(cmd.Parameters["IMeasKey"]); //update if new
			}
            if( measurePoint.Seqno == 0)
			{
				measurePoint.Seqno = Util.GetInt32(cmd.Parameters["ISeqno"]); //update if new
			}
		}

		/// <summary>
		/// Gets all measure point versions with a given key
		/// </summary>
		public static List<MeasurePoint> GetByKey(int measKey)
		{
			return GetByKey(measKey, UtcTime.Null, null);
		}

		public static List<MeasurePoint> GetByKey(int measKey, IDbConnection connection)
		{
			return GetByKey(measKey, UtcTime.Null, connection);
		}

		/// <summary>
		/// Gets all measure point versions with a given key (if validAtTime == UtcTime.Null), or the single measure point version at a given time.
		/// </summary>
		public static List<MeasurePoint> GetByKey(int measKey, UtcTime validAtTime)
		{
			return GetByKey(measKey, validAtTime, null);
		}

		/// <summary>
		/// Gets all measure point versions with a given key (if validAtTime == UtcTime.Null), or the single measure point version at a given time.
		/// </summary>
		public static List<MeasurePoint> GetByKey(int measKey, UtcTime validAtTime, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			var cmd = new OracleCommand("ICC_METERING.MeasurePointGetByKey");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("Imeas_key", OracleDbType.Int32, measKey, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime),
			                   ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			return GetMeasurePointsFromCursor((OracleRefCursor) cmd.Parameters[0].Value);
		}

		/// <summary>
		/// Gets measure points, limited amount returned by <paramref name="maxNumberOfMeasurePoints"/>.
		/// </summary>
		/// <param name="connection">The DB connection.</param>
		/// <param name="spatialReferenceSystem">The spatial reference system to transform result to, can be null for no transform.</param>
		/// <param name="maxNumberOfMeasurePoints">The max number of measure points to return.</param>
		/// <returns><see cref="DataTable"/> with MeasurePoints</returns>
		/// <exception cref="ArgumentOutOfRangeException">If <c>spatialReferenceSystem</c> is a number at or below 0 (but it is ALLOWED to be null)</exception>
		public static DataTable GetLimitedAmountOfMeasurePoints(IDbConnection connection, int? spatialReferenceSystem,
		                                                        int maxNumberOfMeasurePoints)
		{
			if (spatialReferenceSystem == 0)
			{
				throw new ArgumentOutOfRangeException("spatialReferenceSystem", spatialReferenceSystem,
				                                      "Invalid spatial reference system.");
			}
			string sql;
			if (spatialReferenceSystem == null)
			{
				sql = @"select * from
(select meas_key,
m.position.sdo_point.x as x_coordinate, 
m.position.sdo_point.y as y_coordinate 
from meas_pnt_tab m
where m.seqno = 1
and position is not null
order by meas_key)
where rownum <= :1";
			} else
			  {
				  sql = @"select * from
(select meas_key,
SDO_CS.transform(position, :2).sdo_point.x as x_coordinate, 
SDO_CS.transform(position, :3).sdo_point.y as y_coordinate 
from meas_pnt_tab m
where m.seqno = 1
and position is not null
order by meas_key)
where rownum <= :1";
			  }
			var cmd = new OracleCommand(sql);
			cmd.Parameters.Add(null, maxNumberOfMeasurePoints);
			if (spatialReferenceSystem != null)
			{
				cmd.Parameters.Add(null, spatialReferenceSystem);
				cmd.Parameters.Add(null, spatialReferenceSystem);
			}

			return Util.CommandToDataTable(cmd, connection);
		}

		/// <summary>
		/// Searches for measure points.
		/// </summary>
		/// <param name="query">The query text.</param>
		/// <param name="maxNumberOfMeasurePoints">The max number of measure points to return.</param>
		/// <param name="searchOffset">Where to begin the search</param>
		/// <param name="sortName">Column to sort</param>
		/// <param name="sortOrder">Which order to sort in</param>
		/// <param name="spatialReferenceSystem">The spatial reference system to transform result to, can be null for no transform.</param>
		/// <param name="connection">The DB connection.</param>                
		/// <returns> DataTable with MeasurePoints </returns>
		/// <exception cref="ArgumentOutOfRangeException">If <c>spatialReferenceSystem</c> is a number at or below 0 (but it is ALLOWED to be null)</exception>
		public static DataTable SearchForMeasurePoints(string query, int maxNumberOfMeasurePoints, int searchOffset, string sortName, 
		                                               string sortOrder, int? spatialReferenceSystem, IDbConnection connection)
		{
			if (spatialReferenceSystem <= 0)
			{
				throw new ArgumentOutOfRangeException("spatialReferenceSystem", spatialReferenceSystem,
				                                      "Invalid spatial reference system.");
			}

			var lowerLimit = searchOffset + 1;
			var upperLimit = searchOffset + maxNumberOfMeasurePoints;

			var orderBy = string.IsNullOrEmpty(sortName) || string.IsNullOrEmpty(sortOrder)
				? string.Empty
				: string.Format("order by {0} {1}", sortName, sortOrder);

			string sql;
			string xyString;

			if (spatialReferenceSystem == null)
			{
				xyString = @"
       m.position.sdo_point.x as x_coordinate, 
       m.position.sdo_point.y as y_coordinate,
                ";
			}
			else
			{
				xyString = @"
       SDO_CS.transform(position, :sref1).sdo_point.x as x_coordinate, 
       SDO_CS.transform(position, :sref2).sdo_point.y as y_coordinate 
                ";                
			}
			sql = string.Format(@"
select * from
(select a.*, rownum r from
(select distinct m.meas_key,
       m.name,
       {1}
       il.insl_key,
       il.descript,
       il.address || ' ' || il.houseno as address,
       il.postcode,
       il.posttown,
       it.inst_key,
       it.instr_no
from meas_pnt_tab m
     inner join installation il on m.insl_key = il.insl_key
     inner join instr_st on m.meas_key = instr_st.meas_key
     inner join instrument it on it.inst_key = instr_st.inst_key
where m.position is not null
  and it.inst_typ = 91
  and instr_st.state = 1
  and il.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
  and il.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
  and m.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
  and m.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
  and instr_st.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
  and instr_st.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
  and (upper(il.descript) like upper(:query1)
   or upper(il.address || ' ' || il.houseno) like upper(:query2)
   or upper(il.address) like upper(:query3)
   or upper(il.houseno) like upper(:query4)
   or upper(il.postcode) like upper(:query5)
   or upper(il.posttown) like upper(:query6)
   or upper(m.name) like upper(:query7)
   or upper(it.instr_no) like upper(:query8))
  {0}) a) 
where r between :lowerlimit and :upperlimit", orderBy, xyString);

			var cmd = new OracleCommand(sql);

			if (spatialReferenceSystem != null)
			{
				cmd.Parameters.Add("sref1", OracleDbType.Int32, spatialReferenceSystem, ParameterDirection.Input);
				cmd.Parameters.Add("sref2", OracleDbType.Int32, spatialReferenceSystem, ParameterDirection.Input);
			}

			query = query.Trim();
			cmd.Parameters.Add("query1", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query2", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query3", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query4", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query5", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query6", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query7", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query8", OracleDbType.Varchar2, query, ParameterDirection.Input);

			cmd.Parameters.Add("lowerlimit", OracleDbType.Int32, lowerLimit, ParameterDirection.Input);
			cmd.Parameters.Add("upperlimit", OracleDbType.Int32, upperLimit, ParameterDirection.Input);

			return Util.CommandToDataTable(cmd, connection);
		}

		/// <summary>
		/// Get total number of search results when searching for measure points.
		/// </summary>
		/// <param name="query">The query text.</param>        
		/// <param name="connection">The DB connection.</param>                
		/// <returns> DataTable with MeasurePoints </returns>
		/// <exception cref="ArgumentOutOfRangeException">If <c>spatialReferenceSystem</c> is a number at or below 0 (but it is ALLOWED to be null)</exception>
		public static object GetTotalNumberOfHitsWhenSearchingForMeasurePoints(string query, IDbConnection connection)
		{
			const string sql = @"
select count(1) from
(select a.*, rownum r from
(select m.meas_key
from meas_pnt_tab m
     inner join installation il on m.insl_key = il.insl_key
     inner join instr_st on m.meas_key = instr_st.meas_key
     inner join instrument it on it.inst_key = instr_st.inst_key
where m.position is not null
  and it.inst_typ = 91
  and instr_st.state = 1
  and il.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
  and il.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
  and m.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
  and m.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
  and instr_st.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
  and instr_st.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
  and (upper(il.descript) like upper(:query1)
   or upper(il.address || ' ' || il.houseno) like upper(:query2)
   or upper(il.address) like upper(:query3)
   or upper(il.houseno) like upper(:query4)
   or upper(il.postcode) like upper(:query5)
   or upper(il.posttown) like upper(:query6)
   or upper(m.name) like upper(:query7)
   or upper(it.instr_no) like upper(:query8))
) a)";

			var cmd = new OracleCommand(sql);

			query = query.Trim();

			cmd.Parameters.Add("query1", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query2", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query3", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query4", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query5", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query6", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query7", OracleDbType.Varchar2, query, ParameterDirection.Input);
			cmd.Parameters.Add("query8", OracleDbType.Varchar2, query, ParameterDirection.Input);

			return Util.CommandToScalar(cmd, connection);
		}

		/// <summary>
		/// Searches for measure points by coordinates.
		/// Retries search without map layer (databaseViewName) specified if nothing was found in the specified map layer (databaseViewName).
		/// </summary>
		/// <param name="xMapCoordinate">The x map coordinate.</param>
		/// <param name="yMapCoordinate">The y map coordinate.</param>
		/// <param name="databaseViewName">Name of the database view (or table) to search for Measure points in. Ignored if null or empty.</param>
		/// <param name="whereClause">The where clause that shall be combined with the database view/table databaseViewName. Ignored if null or empty.</param>
		/// <param name="radiusInMapUnits">The search radius in map units (to know the search radius).</param>
		/// <param name="maxNumberOfMeasurePoints">The max number of measure points to return.</param>
		/// <param name="searchOffset">Where to begin the search</param>
		/// <param name="sortName">Column to sort</param>
		/// <param name="sortOrder">Which order to sort in</param>
		/// <param name="spatialReferenceSystem">The spatial reference system to transform result to, can be null for no transform.</param>        
		/// <param name="connection">The DB connection.</param>
		/// <returns> DataTable with MeasurePoints found within radius</returns>
		/// <exception cref="ArgumentOutOfRangeException">If <c>spatialReferenceSystem</c> is a number at or below 0 (but it is ALLOWED to be null)</exception>
		public static DataTable SearchForMeasurePointsByCoordinates(decimal xMapCoordinate, decimal yMapCoordinate,
		                                                            string databaseViewName, string whereClause,
		                                                            decimal radiusInMapUnits,
		                                                            int maxNumberOfMeasurePoints, int searchOffset, string sortName, string sortOrder, 
		                                                            int? spatialReferenceSystem, IDbConnection connection)
		{
			if (spatialReferenceSystem <= 0)
			{
				throw new ArgumentOutOfRangeException("spatialReferenceSystem", spatialReferenceSystem,
				                                      "Invalid spatial reference system.");
			}

			var lowerLimit = searchOffset + 1;
			var upperLimit = searchOffset + maxNumberOfMeasurePoints;

			var orderBy = string.IsNullOrEmpty(sortName) || string.IsNullOrEmpty(sortOrder)
				? string.Empty
				: string.Format("order by {0} {1}", sortName, sortOrder);

			string mapLayerViewSql;
			string mapLayerWhereClause;
			GetMapLayerViewSqlAndWhereClause(databaseViewName, whereClause, out mapLayerWhereClause, out mapLayerViewSql);

			string sridStringInSql = GetSridStringInSql(spatialReferenceSystem);

			string sql = string.Format(@"
select 
  meas_key, name, descript, address, postcode, posttown, x_coordinate, y_coordinate, instr_no, inst_key 
from 
  (
    select 
      a.*, rownum r 
    from 
      (
        select distinct
          m.meas_key as meas_key, m.name as name, m.descript as descript, il.address || ' ' || il.houseno as address, il.postcode as postcode, 
          il.posttown as posttown, m.position.sdo_point.x as x_coordinate, m.position.sdo_point.y as y_coordinate, it.instr_no as instr_no, it.inst_key as inst_key 
        from
          {3}
          meas_pnt_tab m
            inner join installation il on m.insl_key = il.insl_key
            inner join instr_st on m.meas_key = instr_st.meas_key
            inner join instrument it on it.inst_key = instr_st.inst_key
        where
          m.position is not null
          and it.inst_typ = 91
          and instr_st.state = 1
          and il.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
          and il.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
          and m.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
          and m.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
          and instr_st.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
          and instr_st.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
          and SDO_WITHIN_DISTANCE(
                m.position, 
                (SDO_geometry(2001, {2}, SDO_point_type(:1, :2, NULL), NULL, NULL)), 
                'distance={0}') = 'TRUE'
          {4}
        {1}
        ) a
  ) 
where
  r between :3 and :4
"
			                           , radiusInMapUnits.ToString().Replace(',', '.'), orderBy, sridStringInSql, mapLayerViewSql, mapLayerWhereClause);

			var cmd = new OracleCommand(sql);
			cmd.Parameters.Add(null, xMapCoordinate);
			cmd.Parameters.Add(null, yMapCoordinate);
			cmd.Parameters.Add(null, lowerLimit);
			cmd.Parameters.Add(null, upperLimit);            

			if (spatialReferenceSystem != null)
			{
				cmd.Parameters.Add(null, spatialReferenceSystem);
			}

			DataTable dt = Util.CommandToDataTable(cmd, connection);

			if (dt.Rows.Count == 0)
			{
				if (string.IsNullOrEmpty(databaseViewName) && string.IsNullOrEmpty(whereClause))
				{
					// Return null if the search was without map layer specified:
					return null;
				}
				// Retry search without map layer specified if nothing was found in the specified map layer:
				return SearchForMeasurePointsByCoordinates(xMapCoordinate, yMapCoordinate,
				                                           null, null,
				                                           radiusInMapUnits,
				                                           maxNumberOfMeasurePoints, searchOffset, sortName,
				                                           sortOrder,
				                                           spatialReferenceSystem, connection);
			}

			return dt;
		}

		private static string GetSridStringInSql(int? spatialReferenceSystem)
		{
			string sridStringInSql;
			if (spatialReferenceSystem == null)
			{
				sridStringInSql =
					@" (SELECT t.position.SDO_SRID 
  FROM MEAS_PNT_TAB t 
  WHERE t.position.SDO_POINT.x is not null 
  and t.position.SDO_POINT.y is not null 
  and rownum = 1
  ) ";
			} else
			  {
				  sridStringInSql = ":5";
			  }
			return sridStringInSql;
		}

		private static void GetMapLayerViewSqlAndWhereClause(string databaseViewName, string whereClause, out string mapLayerWhereClause, out string mapLayerViewSql)
		{
			if (string.IsNullOrEmpty(databaseViewName))
			{
				mapLayerViewSql = " ";
				mapLayerWhereClause = " ";
			}
			else
			{
				mapLayerViewSql = " " + databaseViewName + " mapview, \n";
				mapLayerWhereClause = " and mapview.meas_key = m.meas_key and mapview.SEQNO = m.seqno ";
				if (!string.IsNullOrEmpty(whereClause))
				{
					mapLayerWhereClause += " and " + whereClause + " ";
				}
			}
		}

		/// <summary>
		/// Get total number of search results when searching by coordinates.
		/// Retries search without map layer (databaseViewName) specified if nothing was found in the specified map layer (databaseViewName).
		/// </summary>
		/// <param name="xMapCoordinate">The x map coordinate.</param>
		/// <param name="yMapCoordinate">The y map coordinate.</param>
		/// <param name="databaseViewName">Name of the database view (or table) to search for Measure points in. Ignored if null or empty.</param>
		/// <param name="whereClause">The where clause that shall be combined with the database view/table databaseViewName. Ignored if null or empty.</param>
		/// <param name="radiusInMapUnits">The search radius in map units (to know the search radius).</param>
		/// <param name="spatialReferenceSystem">The spatial reference system to transform result to, can be null for no transform.</param>        
		/// <param name="connection">The DB connection.</param>
		/// <returns> </returns>
		/// <exception cref="ArgumentOutOfRangeException">If <c>spatialReferenceSystem</c> is a number at or below 0 (but it is ALLOWED to be null)</exception>
		public static object GetTotalNumberOfHitsWhenSearchingByCoordinates(decimal xMapCoordinate, decimal yMapCoordinate,
		                                                                    string databaseViewName, string whereClause,
		                                                                    decimal radiusInMapUnits,                                                                    
		                                                                    int? spatialReferenceSystem, IDbConnection connection)
		{
			if (spatialReferenceSystem <= 0)
			{
				throw new ArgumentOutOfRangeException("spatialReferenceSystem", spatialReferenceSystem,
				                                      "Invalid spatial reference system.");
			}

			string mapLayerViewSql;
			string mapLayerWhereClause;
			GetMapLayerViewSqlAndWhereClause(databaseViewName, whereClause, out mapLayerWhereClause, out mapLayerViewSql);

			string sridStringInSql = GetSridStringInSql(spatialReferenceSystem);

			string sql = string.Format(@"
select
  count(1)
from 
  (
    select 
      a.*, rownum r 
    from 
      (
        select distinct
          m.meas_key as meas_key, m.name as name, m.descript as descript, il.address || ' ' || il.houseno as address, il.postcode as postcode, 
          il.posttown as posttown, m.position.sdo_point.x as x_coordinate, m.position.sdo_point.y as y_coordinate, it.instr_no as instr_no, it.inst_key as inst_key 
        from
          {2}
          meas_pnt_tab m
            inner join installation il on m.insl_key = il.insl_key
            inner join instr_st on m.meas_key = instr_st.meas_key
            inner join instrument it on it.inst_key = instr_st.inst_key
        where
          m.position is not null
          and it.inst_typ = 91
          and instr_st.state = 1
          and il.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
          and il.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
          and m.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
          and m.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
          and instr_st.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
          and instr_st.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
          and SDO_WITHIN_DISTANCE(
                m.position, 
                (SDO_geometry(2001, {1}, SDO_point_type(:1, :2, NULL), NULL, NULL)), 
                'distance={0}') = 'TRUE'
          {3}
        ) a
  ) 
",
			                           radiusInMapUnits.ToString().Replace(',', '.'), sridStringInSql, mapLayerViewSql, mapLayerWhereClause);

			var cmd = new OracleCommand(sql);
			cmd.Parameters.Add(null, xMapCoordinate);
			cmd.Parameters.Add(null, yMapCoordinate);

			if (spatialReferenceSystem != null)
			{
				cmd.Parameters.Add(null, spatialReferenceSystem);
			}

			object numberOfMPsFound = Util.CommandToScalar(cmd, connection);
			if (Convert.ToInt32(numberOfMPsFound) == 0)
			{
				if (string.IsNullOrEmpty(databaseViewName) && string.IsNullOrEmpty(whereClause))
				{
					// Return zero if the search was without map layer specified:
					return 0;
				}
				// Retry search without map layer specified if nothing was found in the specified map layer:
				return GetTotalNumberOfHitsWhenSearchingByCoordinates(xMapCoordinate, yMapCoordinate,
				                                                      null, null,
				                                                      radiusInMapUnits,
				                                                      spatialReferenceSystem, connection);
			}
			return numberOfMPsFound;
		}

		/// <summary>
		/// Gets the measure point, by key.
		/// </summary>
		/// <param name="key">The measure point key.</param>
		/// <param name="spatialReferenceSystem">The spatial reference system to transform result to, can be null for no transform.</param>
		/// <param name="connection">The DB connection.</param>
		/// <returns>
		/// The MeasurePoint with the given key
		/// </returns>
		/// <exception cref="ArgumentOutOfRangeException">If <c>spatialReferenceSystem</c> is a number at or below 0 (but it is ALLOWED to be null)</exception>
		/// <exception cref="ArgumentOutOfRangeException">If a Measure Point with the given key can not be found</exception>
		public static DataRow GetDetails(int key, int? spatialReferenceSystem, IDbConnection connection)
		{
			if (spatialReferenceSystem <= 0)
			{
				throw new ArgumentOutOfRangeException("spatialReferenceSystem", spatialReferenceSystem,
				                                      "Invalid spatial reference system.");
			}

			string sql;
			if (spatialReferenceSystem == null)
			{
				sql =
					@"select meas_key, name, name2, descript, location, insl_key, n_phases, volts, fusesz, 
m.position.sdo_point.x as x_coordinate, 
m.position.sdo_point.y as y_coordinate 
from meas_pnt_tab m
where meas_key = :1 
and seqno = (select max(seqno) from meas_pnt im where im.meas_key = m.meas_key)";
			}
			else
			{
				sql =
					@"select meas_key, name, name2, descript, location, insl_key, n_phases, volts, fusesz, 
SDO_CS.transform(position, :2).sdo_point.x as x_coordinate, 
SDO_CS.transform(position, :3).sdo_point.y as y_coordinate 
from meas_pnt_tab m
where meas_key = :1 
and seqno = (select max(seqno) from meas_pnt im where im.meas_key = m.meas_key)";
			}

			var cmd = new OracleCommand(sql);
			cmd.Parameters.Add(null, key);
			if (spatialReferenceSystem != null)
			{
				cmd.Parameters.Add(null, spatialReferenceSystem);
				cmd.Parameters.Add(null, spatialReferenceSystem);
			}

			DataTable dt = Util.CommandToDataTable(cmd, connection);

			if (dt.Rows.Count == 0)
			{
				throw new ArgumentOutOfRangeException("key", key, "Measure point with key " + key + " does not exist.");
			}

			return dt.Rows[0];
		}

		/// <summary>
		/// Gets details from measure points given coordinates nearby
		/// Retries search without map layer (databaseViewName) specified if nothing was found in the specified map layer (databaseViewName).
		/// </summary>
		/// <param name="xMapCoordinate">The x map coordinate.</param>
		/// <param name="yMapCoordinate">The y map coordinate.</param>
		/// <param name="databaseViewName">Name of the database view (or table) to search for Measure points in. Ignored if null or empty.</param>
		/// <param name="whereClause">The where clause that shall be combined with the database view/table databaseViewName. Ignored if null or empty.</param>
		/// <param name="radiusInMapUnits">The search radius in map units (to know the search radius).</param>
		/// <param name="spatialReferenceSystem">The spatial reference system to transform result to, can be null for no transform.</param>
		/// <param name="connection">The DB connection.</param>
		/// <returns>
		/// Returns null if no measure point can be found within search radius
		/// </returns>
		/// <exception cref="ArgumentOutOfRangeException">If <c>spatialReferenceSystem</c> is a number at or below 0 (but it is ALLOWED to be null)</exception>
		public static DataRow GetDetails(decimal xMapCoordinate, decimal yMapCoordinate,
		                                 string databaseViewName, string whereClause,
		                                 decimal radiusInMapUnits,
		                                 int? spatialReferenceSystem, IDbConnection connection)
		{
			if (spatialReferenceSystem <= 0)
			{
				throw new ArgumentOutOfRangeException("spatialReferenceSystem", spatialReferenceSystem,
				                                      "Invalid spatial reference system.");
			}

			string mapLayerViewSql;
			string mapLayerWhereClause;
			GetMapLayerViewSqlAndWhereClause(databaseViewName, whereClause, out mapLayerWhereClause, out mapLayerViewSql);

			string sridStringInSql = GetSridStringInSql(spatialReferenceSystem);
			string xCoordSql;
			string yCoordSql;
			if (spatialReferenceSystem == null)
			{
				xCoordSql = "m.position.sdo_point.x as x_coordinate";
				yCoordSql = "m.position.sdo_point.y as y_coordinate";
			}
			else
			{
				xCoordSql = "SDO_CS.transform(m.position, :4).sdo_point.x as x_coordinate, ";
				yCoordSql = "SDO_CS.transform(m.position, :5).sdo_point.y as y_coordinate, ";
			}

			string sql = string.Format(@"
SELECT /*+ INDEX(i meas_pnt_spatial_idx) */ 
  m.meas_key, m.name, m.name2, m.descript, m.location, m.insl_key, m.n_phases, m.volts, m.fusesz, 
  {0}, 
  {1} 
FROM
  {3}
  meas_pnt_tab m 
WHERE 
  SDO_NN(m.position, SDO_geometry(2001, {2}, SDO_point_type(:1, :2, NULL), NULL, NULL), 'sdo_num_res=1000', 1) = 'TRUE'
  AND SDO_NN_DISTANCE(1) < :3
  {4}
ORDER BY
  SDO_NN_DISTANCE(1)
"
			                           , xCoordSql, yCoordSql, sridStringInSql, mapLayerViewSql, mapLayerWhereClause);

			var cmd = new OracleCommand(sql);
			cmd.Parameters.Add(null, xMapCoordinate);
			cmd.Parameters.Add(null, yMapCoordinate);
			cmd.Parameters.Add(null, radiusInMapUnits);
			if (spatialReferenceSystem != null)
			{
				cmd.Parameters.Add(null, spatialReferenceSystem);
				cmd.Parameters.Add(null, spatialReferenceSystem);
			}

			DataTable dt = Util.CommandToDataTable(cmd, connection);

			if (dt.Rows.Count == 0)
			{
				if (string.IsNullOrEmpty(databaseViewName) && string.IsNullOrEmpty(whereClause))
				{
					// Return null if the search was without map layer specified:
					return null;
				}
				// Retry search without map layer specified if nothing was found in the specified map layer:
				return GetDetails(xMapCoordinate, yMapCoordinate,
				                  null, null,
				                  radiusInMapUnits,
				                  spatialReferenceSystem, connection);
			}

			return dt.Rows[0];
		}

		///<summary>
		/// Fetches the meter id for a measure point.
		///</summary>
		///<param name="measurePointKey">The measure point key.</param>
		///<param name="connection">The DB connection.</param>
		///<returns>The meter ID.</returns>
		public static DataTable GetMeterId(int measurePointKey, IDbConnection connection)
		{
			var sql = @"
select
  it.instr_no as meter_id 
from 
  meas_pnt_tab m
  inner join instr_st on m.meas_key = instr_st.meas_key
  inner join instrument it on it.inst_key = instr_st.inst_key
where
  m.meas_key = :1 
  and inst_typ = 91
  and instr_st.state = 1
  and instr_st.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
  and instr_st.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
";

			var cmd = new OracleCommand(sql);
			cmd.Parameters.Add(null, measurePointKey);

			return Util.CommandToDataTable(cmd, connection);
		}

		/// <summary>
		/// Gets the consumption series, by finding the time series that have an active export definition.
		/// If more than one time series is found, then a hour-series will be prioritized before a "stand"?/"knekkpunkt"?-series.
		/// No constraints on actor (short-)names.
		/// </summary>
		/// <param name="measurePointKey">The measure point key.</param>
		/// <param name="connection">The DB connection.</param>
		/// <returns>tims_key (consumption series key)</returns>
		public static int GetConsumptionSeries(int measurePointKey, IDbConnection connection)
		{
			return GetConsumptionSeries(measurePointKey, new string[0], connection);
		}

		/// <summary>
		/// Gets the consumption series, by finding the time series that have an active export definition.
		/// If more than one time series is found, then a hour-series will be prioritized before a "stand"?/"knekkpunkt"?-series.
		/// </summary>
		/// <param name="measurePointKey">The measure point key.</param>
		/// <param name="actorShortNames">The valid export definition must belong to at least one of these actors (short name is used).
		///   Can be empty array or null to allow all actors.
		///   If this filter produces no timesKey, then this method is called again with this filter set to null (=ignored) to mimic old functionality.</param>
		/// <param name="connection">The DB connection.</param>
		/// <returns>tims_key (consumption series key)</returns>
		public static int GetConsumptionSeries(int measurePointKey, string[] actorShortNames, IDbConnection connection)
		{
			string actorShortNameRestraint = string.Empty;
			if (actorShortNames != null && actorShortNames.Length > 0)
			{
				actorShortNameRestraint = " and o.shorname in ('" + actorShortNames[0] + "'";
				for (int i = 1; i < actorShortNames.Length; i++)
				{
					string actorShortName = actorShortNames[i];
					actorShortNameRestraint += ", '" + actorShortName + "'"; 
				}
				actorShortNameRestraint += ") ";
			}
            
			string sql = string.Format(@"
select
  * 
from (
    select
      t.tims_key 
    from
      timeser t 
      join impexpts iet on t.tims_key = iet.tims_key
      join meas_ts mts on iet.tims_key = mts.tims_key 
      join meas_pnt m on mts.meas_key = m.meas_key 
      join oprtunit o on o.opun_key = iet.opun_key
    where
      m.meas_key = :1
      and t.tsin_key in (-1,101)
      and iet.direct = 1
      and iet.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
      and iet.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
      {0}
    order by
      t.tsin_key desc
    )
where rownum = 1
", actorShortNameRestraint);

			var cmd = new OracleCommand(sql);
			cmd.Parameters.Add(null, measurePointKey);

			int timsKey = Convert.ToInt32(Util.CommandToScalar(cmd, connection));
			// If nothing was found and search was made with constraints on actor, then retry without constraints on actor:
			if (timsKey == 0 && !string.IsNullOrEmpty(actorShortNameRestraint))
			{
				timsKey = GetConsumptionSeries(measurePointKey, null, connection);
			}
			return timsKey;
		}

		/// <summary>
		/// Gets the consumption series, by finding the time series that have an active export definition.
		/// If more than one time series is found, then a hour-series will be prioritized before a "stand"?/"knekkpunkt"?-series.
		/// </summary>
		/// <param name="measurePointKey">The measure point key.</param>
		/// <param name="actorShortNames">The valid export definition must belong to at least one of these actors (short name is used).
		///   Can be empty array or null to allow all actors.
		///   If this filter produces no timesKey, then this method is called again with this filter set to null (=ignored) to mimic old functionality.</param>
		/// <param name="connection">The DB connection.</param>
		/// <returns>DataRow</returns>
		public static DataRow GetConsumptionSeriesReturnDataRow(int measurePointKey, string[] actorShortNames, IDbConnection connection)
		{
			string actorShortNameRestraint = string.Empty;
			if (actorShortNames != null && actorShortNames.Length > 0)
			{
				actorShortNameRestraint = " and o.shorname in ('" + actorShortNames[0] + "'";
				for (int i = 1; i < actorShortNames.Length; i++)
				{
					string actorShortName = actorShortNames[i];
					actorShortNameRestraint += ", '" + actorShortName + "'";
				}
				actorShortNameRestraint += ") ";
			}

			string sql = string.Format(@"
select
  * 
from (
    select
      t.tims_key, t.tsin_key, t.vltp_key, t.vltp_valftime
    from
      timeser t 
      join impexpts iet on t.tims_key = iet.tims_key
      join meas_ts mts on iet.tims_key = mts.tims_key 
      join meas_pnt m on mts.meas_key = m.meas_key 
      join oprtunit o on o.opun_key = iet.opun_key
    where
      m.meas_key = :1
      and t.tsin_key in (-1,101)
      and iet.direct = 1
      and iet.valftime < to_char(SYSDATE, 'yyyyMMddHHmiss')
      and iet.valttime > to_char(SYSDATE, 'yyyyMMddHHmiss')
      {0}
    order by
      t.tsin_key desc
    )
where rownum = 1
", actorShortNameRestraint);

			var cmd = new OracleCommand(sql);
			cmd.Parameters.Add(null, measurePointKey);
            
            
			DataTable dt = Util.CommandToDataTable(cmd, connection);

			if (dt.Rows.Count == 0 && !string.IsNullOrEmpty(actorShortNameRestraint))
			{
				return GetConsumptionSeriesReturnDataRow(measurePointKey, null, connection);
			}
			else
			{
				return dt.Rows[0];
			}

            


            
		}

		/// <summary>
		/// Gets a measure point given a key and a seqno
		/// </summary>
		/// <param name="measKey"></param>
		/// <param name="seqno"></param>
		/// <param name="connection"></param>
		/// <returns></returns>
		public static MeasurePoint GetByKey(int measKey, int seqno, IDbConnection connection)
		{
			MeasurePoint measurePoint = null;
			var cmd = new OracleCommand("ICC_METERING.MeasurePointGetByKey");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("Imeas_key", OracleDbType.Int32, measKey, ParameterDirection.Input);
			cmd.Parameters.Add("Iseqno", OracleDbType.Int32, seqno, ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			List<MeasurePoint> measurePoints = GetMeasurePointsFromCursor((OracleRefCursor) cmd.Parameters[0].Value);
			if (measurePoints.Count > 0)
			{
				measurePoint = measurePoints[0];
			}
			return measurePoint;
		}

		/// <summary>
		/// Gets meter type given a key
		/// </summary>
		/// <param name="measurePointKey"></param>
		/// <param name="connection"></param>
		/// <returns></returns>
		public static DataTable GetMeterTypeByKey(int measurePointKey, IDbConnection connection)
		{
			string sql = @"select automatic from meas_type_view where meas_key = :1 order by automatic desc";

			var cmd = new OracleCommand(sql);
			cmd.Parameters.Add(null, measurePointKey);

			return Util.CommandToDataTable(cmd, connection);
		}

		/// <summary>
		/// Gets a measure point at a given time given an id. If validAtTime is omitted, all versions of mesaure point is returned.
		/// </summary>
		/// <param name="measurePointID">Id</param>
		/// <param name="validAtTime"></param>
		/// <param name="connection"></param>
		/// <returns></returns>
		public static List<MeasurePoint> GetByID(string measurePointID, UtcTime validAtTime, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			var cmd = new OracleCommand("ICC_METERING.MeasurePointGetByName");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("ImeasName", OracleDbType.Varchar2, measurePointID, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime),
			                   ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			return GetMeasurePointsFromCursor((OracleRefCursor) cmd.Parameters[0].Value);
		}

		/// <summary>
		/// Gets the latest version of a measure point with a given Id
		/// </summary>
		public static MeasurePoint GetLatestByID(string measurePointId, IDbConnection connection)
		{
			List<MeasurePoint> measurePoints = GetByID(measurePointId, UtcTime.Null, connection);
			UtcTime latest = UtcTime.MinValue;
			MeasurePoint latestMP = null;
			foreach (MeasurePoint measurePoint in measurePoints)
			{
				if (measurePoint.ValidFromDate > latest)
				{
					latest = measurePoint.ValidFromDate;
					latestMP = measurePoint;
				}
			}
			return latestMP;
		}

		/// <summary>
		/// Gets all valid measure points of a given installation at a given time
		/// </summary>
		public static List<MeasurePoint> GetForInstallation(Installation installation)
		{
			return GetForInstallation(installation, UtcTime.Null, null);
		}

		/// <summary>
		/// Gets all valid measure points of a given installation at a given time
		/// </summary>
		public static List<MeasurePoint> GetForInstallation(Installation installation, UtcTime validAtTime)
		{
			return GetForInstallation(installation, validAtTime, null);
		}

		/// <summary>
		/// Gets all valid measure points of a given installation at a given time
		/// </summary>
		public static List<MeasurePoint> GetForInstallation(Installation installation, UtcTime validAtTime,
		                                                    IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			var cmd = new OracleCommand("ICC_METERING.MeasurePointGetForInstallation");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("Iinsl_key", OracleDbType.Int32, installation.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime),
			                   ParameterDirection.Input);

			if (connection == null)
			{
				connection = Util.OpenConnection();
			}

			Util.ExecuteCommand(cmd, connection);

			return GetMeasurePointsFromCursor((OracleRefCursor) cmd.Parameters[0].Value);
		}

		/// <summary>
		/// Gets the installation to which a measure point belongs
		/// </summary>
		public static Installation GetInstallation(MeasurePoint measurePoint, UtcTime validAtTime)
		{
			return GetInstallation(measurePoint, validAtTime, null);
		}

		/// <summary>
		/// Gets the installation to which a measure point belongs
		/// </summary>
		public static Installation GetInstallation(MeasurePoint measurePoint, UtcTime validAtTime,
		                                           IDbConnection connection)
		{
			if (validAtTime == UtcTime.Null)
			{
				throw new ArgumentException("Time must be specified in call to MeasurePointData.GetInstallation");
			}
			Installation installation = InstallationData.GetByKey(measurePoint.InstallationKey, validAtTime, connection);
			return installation;
		}

		/// <summary>
		/// Gets a measure point, which register is connected to a given time series at a given time.
		/// </summary>
		/// <param name="timsKey"></param>
		/// <param name="validAtTime"></param>
		/// <param name="connection"></param>
		/// <returns></returns>
		public static MeasurePoint GetForTimeSeries(int timsKey, UtcTime validAtTime, IDbConnection connection)
		{
			IDbConnection con;
			MeasurePoint measurePoint = null;
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			var cmd = new OracleCommand("ICC_METERING.MeasurePointGetForTimeSeries");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("ItimsKey", OracleDbType.Int32, timsKey, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime),
			                   ParameterDirection.Input);

			if (connection == null)
			{
				con = Util.OpenConnection();
			}
			else
			{
				con = connection;
			}

			Util.ExecuteCommand(cmd, con);

			List<MeasurePoint> measurePoints = GetMeasurePointsFromCursor((OracleRefCursor) cmd.Parameters[0].Value);
			if (measurePoints.Count > 0)
			{
				measurePoint = measurePoints[0];
			}
			return measurePoint;
		}

		/// <summary>
		/// Finishes a measure point, i.e. sets its valid time period to an end. Finishes also its related components if parameter finishComponents are true.
		/// </summary>
		/// <param name="measurePoint">measure point</param>
		/// <param name="timeOfFinish">time of finish</param>
		/// <param name="finishComponents">indicates that all related components of measure point must be finished as well</param>
		/// <param name="connection">connection must be provided if part of a transaction</param>
		public static void Finish(MeasurePoint measurePoint, UtcTime timeOfFinish, bool finishComponents,
		                          IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			var cmd = new OracleCommand("ICC_METERING.MeasurePointFinish");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("IMeas_Key", OracleDbType.Int32, measurePoint.Key, ParameterDirection.Input);
			cmd.Parameters.Add("ISeqno", OracleDbType.Int32, measurePoint.Seqno, ParameterDirection.Input);
			cmd.Parameters.Add("IfinishTime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfFinish),
			                   ParameterDirection.Input);
			if (finishComponents)
			{
				cmd.Parameters.Add("IfinishComponents", OracleDbType.Int32, 1, ParameterDirection.Input);
			}

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);

			//update object due to database changes
			measurePoint.ValidToDate = timeOfFinish;
		}

		/// <summary>
		/// Creates a new version of a measure point. Includes finishing current version, creating new version and copying of time series connections from old to new version.
		/// </summary>
		public static void CreateNewVersion(MeasurePoint measurePoint, Installation installation, UtcTime timeOfChange,
		                                    IDbConnection connection)
		{
			var oldMeasurePoint = new MeasurePoint(measurePoint);
			Finish(measurePoint, timeOfChange, false, connection);
			measurePoint.Seqno = 0;
			measurePoint.ValidFromDate = timeOfChange;
			measurePoint.ValidToDate = UtcTime.MaxValue;
            Update(measurePoint, installation, measurePoint.CentralNetPointCode, connection);
			CopyTimeSeriesConnection(oldMeasurePoint, measurePoint, connection);
		}

		private static List<MeasurePoint> GetMeasurePointsFromCursor(OracleRefCursor refCursor)
		{
			var measurePoints = new List<MeasurePoint>();
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			using (refCursor)
			{
				using (OracleDataReader reader = refCursor.GetDataReader())
				{
					while (reader.Read())
					{
						measurePoints.Add(new MeasurePoint(Util.GetInt32(reader, "meas_key"),
						                                   Util.GetInt32(reader, "seqno"),
						                                   Util.GetInt32(reader, "insl_key"),
						                                   Util.GetString(reader, "name"),
						                                   Util.GetString(reader, "name2"),
						                                   Util.GetString(reader, "svk_id"),
						                                   Util.GetString(reader, "descript"),
						                                   cal.ToUtcTime(Util.GetString(reader, "valftime")),
						                                   cal.ToUtcTime(Util.GetString(reader, "valttime")),
						                                   Util.GetDecimal(reader, "x_coordinate"),
						                                   Util.GetDecimal(reader, "y_coordinate"),
						                                   Util.GetString(reader, "station_id"),
						                                   Util.GetInt32(reader, "n_phases"),
						                                   Util.GetString(reader, "volts"),
						                                   Util.GetString(reader, "fusesz"),
						                                   reader.IsDBNull(reader.GetOrdinal("used_power"))
						                                   ? Int32.MinValue
						                                   : Convert.ToInt32(Util.GetFloat(reader, "used_power")), 0
						                                   /*TODOUtil.Util.GetString(Util.Util.GetOrdinal("subscribed"))*/,
						                                   Util.GetString(reader, "org_unit"),
						                                   Util.GetString(reader, "grid_owner"),
						                                   Util.GetString(reader, "profile"),
						                                   Util.GetString(reader, "location"),
						                                   Util.GetString(reader, "supplier"),
						                                   Util.GetString(reader, "central_net_point_code"),
                                                           Util.GetInt32(reader, "debt_typ"),
                                                           reader.IsDBNull(reader.GetOrdinal("state"))
                                                               ? (int)MeasurePoint.StateTypes.Unspecified
                                                               : Util.GetInt32(reader, "state")));
					}
				}
			}
			return measurePoints;
		} // GetMeasurePointsFromCursor

		/// <summary>
		/// Method that calls for a copy of a measure point's connection to its time series to another measure point (-version).
		/// </summary>
		internal static void CopyTimeSeriesConnection(MeasurePoint oldMeasurePoint, MeasurePoint newMeasurePoint,
		                                              IDbConnection connection)
		{
			var cmd = new OracleCommand("ICC_METERING.TimeSeriesConnectionCopy");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("IMeas_Key", OracleDbType.Int32, oldMeasurePoint.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IOldSeqno", OracleDbType.Int32, oldMeasurePoint.Seqno, ParameterDirection.Input);
			cmd.Parameters.Add("INewSeqNo", OracleDbType.Int32, newMeasurePoint.Seqno, ParameterDirection.Input);

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}

		public static void CheckNetArea(MeasurePoint measurePoint, IDbConnection connection)
		{
			if (measurePoint.NetAreaCode != null && measurePoint.NetAreaCode != "")
			{
				var cmd = new OracleCommand("ICC_METERING.NetAreaGetType");
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.Parameters.Add("netAreaType", OracleDbType.Int32, null, ParameterDirection.ReturnValue);
				cmd.Parameters.Add("InetAreaCode", OracleDbType.Varchar2, measurePoint.NetAreaCode,
				                   ParameterDirection.Input);

				//get type of net area
				Util.ExecuteCommand(cmd, connection);
				int netAreaType = Util.GetInt32(cmd.Parameters["netAreaType"]);

				switch (netAreaType) //what type?
				{
					case 1: //main net area
					case 2: //regional net area
					case 3: //local net area
						cmd = new OracleCommand("ICC_METERING.CentralNetPointGetForNetArea");
						cmd.CommandType = CommandType.StoredProcedure;
						cmd.Parameters.Add("centralNetPointCode", OracleDbType.Varchar2, 10, null,
						                   ParameterDirection.ReturnValue);
						cmd.Parameters.Add("InetAreaCode", OracleDbType.Varchar2, measurePoint.NetAreaCode,
						                   ParameterDirection.Input);

						//get central net point code for this net area
						Util.ExecuteCommand(cmd, connection);
						string centralNetPointCode;
						if (Util.IsNull(cmd.Parameters["centralNetPointCode"]))
						{
							centralNetPointCode = null;
						}
						else
						{
							centralNetPointCode = cmd.Parameters["centralNetPointCode"].Value.ToString();
						}

						//set central net point code
						measurePoint.CentralNetPointCode = centralNetPointCode;
						break;
					case 8: //central net point
						cmd = new OracleCommand("ICC_METERING.NetAreaGetForCentralNetPoint");
						cmd.CommandType = CommandType.StoredProcedure;
						cmd.Parameters.Add("netAreaCode", OracleDbType.Varchar2, 10, null,
						                   ParameterDirection.ReturnValue);
						if (measurePoint.CentralNetPointCodeEdited && !String.IsNullOrEmpty(measurePoint.CentralNetPointCode))
							cmd.Parameters.Add("IcentralNetPointCode", OracleDbType.Varchar2,
							                   measurePoint.CentralNetPointCode, ParameterDirection.Input);
						else
							cmd.Parameters.Add("IcentralNetPointCode", OracleDbType.Varchar2, measurePoint.NetAreaCode,
							                   ParameterDirection.Input);

						//get net area code for this central net point
						Util.ExecuteCommand(cmd, connection);
						string netAreaCode;
						if (Util.IsNull(cmd.Parameters["netAreaCode"]))
						{
							netAreaCode = null;
						}
						else
						{
							netAreaCode = cmd.Parameters["netAreaCode"].Value.ToString();
						}

						//move central net point code to correct property and then set net area code
						if (!measurePoint.CentralNetPointCodeEdited)
							measurePoint.CentralNetPointCode = measurePoint.NetAreaCode;
						measurePoint.NetAreaCode = netAreaCode;
						break;
					default: //do nothing if other type of net area
						break;
				}
			}
		}

		private static OracleCommand ParameterNullifier(OracleCommand cmd)
		{
			foreach (OracleParameter param in cmd.Parameters)
			{
				if (param.Value != null)
				{ 
					if (param.OracleDbType == OracleDbType.Single)
						if(param.Value.ToString() == decimal.MinValue.ToString())
							param.Value = null;
					if (param.OracleDbType == OracleDbType.Int32)
						if (param.Value.ToString() == Int32.MinValue.ToString())
							param.Value = null;
				}

			}
			return cmd;
		}

		public static void SaveMeasurePoints(MeasurePoint[] measurePoints, IDbConnection connection)
		{
			int count = measurePoints.Length;

			var measKeys = new int[count];
			var names = new string[count];
			var xCoordinates = new double[count];
			var yCoordinates = new double[count];

			for (int i = 0; i < count; i++)
			{
				Debug.Assert(measurePoints[i] != null);

				measKeys[i] = measurePoints[i].Key;
				names[i] = measurePoints[i].Id;
				xCoordinates[i] = (double) measurePoints[i].XCoordinate;
				yCoordinates[i] = (double) measurePoints[i].YCoordinate;
			}

			const string sql =
				"begin insert into meas_pnt (meas_key, seqno, name, x_coordinate, y_coordinate) values (:1, 0, :2, :3, :4); end;";
			var cmd = new OracleCommand(sql);
			cmd.ArrayBindCount = count;

			OracleParameter par;

			par = cmd.Parameters.Add(":1", OracleDbType.Int32);
			par.Value = measKeys;

			par = cmd.Parameters.Add(":2", OracleDbType.Varchar2);
			par.Value = names;

			par = cmd.Parameters.Add(":3", OracleDbType.Double);
			par.Value = xCoordinates;

			par = cmd.Parameters.Add(":4", OracleDbType.Double);
			par.Value = yCoordinates;

			Util.ExecuteCommand(cmd, connection);
		}

		public static int FetchNewKey(string mpID,int instKey, string fuseSize, UtcTime validFrom, IDbConnection connection)
		{

			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;

			OracleCommand cmd = new OracleCommand("ICC_METERING.MeasurePointFetchNewKey");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("measKey", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("Iname", OracleDbType.Varchar2, mpID, ParameterDirection.Input);
			cmd.Parameters.Add("Iinsl_key", OracleDbType.Int32, instKey, ParameterDirection.Input);
			cmd.Parameters.Add("Ivalftime", OracleDbType.Varchar2, cal.ToDatabaseString(validFrom), ParameterDirection.Input);
			cmd.Parameters.Add("IfuseSize", OracleDbType.Varchar2, fuseSize, ParameterDirection.Input);

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

			return Util.GetInt32(cmd.Parameters["measKey"]);
		}

		public static void DeleteByKey(int mpKey, IDbConnection connection)
		{
			OracleCommand cmd = new OracleCommand("ICC_METERING.MeasurePointDeleteByKey");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("Imeas_key", OracleDbType.Int32, mpKey, ParameterDirection.Input);

			cmd = ParameterNullifier(cmd);

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);
		}
	}
}
