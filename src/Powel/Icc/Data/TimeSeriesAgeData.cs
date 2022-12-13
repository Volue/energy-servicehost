using System;
using System.Data;
using Oracle.ManagedDataAccess.Client;
using Powel.Icc.Common;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data
{
    /// <summary>
    /// Summary description for TimeSeriesAgeData.
    /// </summary>
    public sealed class TimeSeriesAgeData
    {
        public static DataTable Fetch(TimeSeriesData.TimeSeriesInfo info)
        {
            return Fetch(info, null);
        }

        public static DataTable Fetch(TimeSeriesData.TimeSeriesInfo info, IDbConnection connection)
        {
            PrepareTimeSeriesInfoForLookup(info, connection);
            OracleCommand cmd = new OracleCommand("select ts.tims_key, ts.filename, ts.tscode, tsa.segment_from, tsa.segment_until, tsa.age from timeseries_age tsa join timeser ts on tsa.tims_key = ts.tims_key where ts.tims_key = :1");
            cmd.Parameters.Add(null, info.tims_key);
            return Util.CommandToDataTable(cmd, connection);
        }

        public static DataTable Fetch(string timeSeriesName, IDbConnection connection)
        {
            var info = new TimeSeriesData.TimeSeriesInfo(timeSeriesName);
            return Fetch(info, connection);
        }

        private static void PrepareTimeSeriesInfoForLookup(TimeSeriesData.TimeSeriesInfo info, IDbConnection connection)
        {
            if (info.tims_key > 0)
                return;

            if (String.IsNullOrEmpty(info.tscode))
                throw new ArgumentException("The TimeSeriesInfo needs to contain either a time series ID or a name.", "info");

            var cmd = new OracleCommand("select tims_key from timeser where filename||tscode = :1");
            cmd.Parameters.Add(null, info.FullName);

            try
            {
                info.tims_key = Convert.ToInt32(Util.CommandToScalar(cmd, connection));
            }
            catch
            {
                throw new ArgumentException(String.Format(
                    "No time series called {0} exists in the database.", info.FullName), "info");
            }
        }

        public static void Delete(TimeSeriesData.TimeSeriesInfo info, IDbTransaction transaction)
        {
            PrepareTimeSeriesInfoForLookup(info, transaction.Connection);
            OracleCommand cmd = new OracleCommand("delete from timeseries_age where tims_key = :1");
            cmd.Parameters.Add(null, info.tims_key);
            Util.ExecuteCommand(cmd, transaction);
        }

        public static void Delete(string timeSeriesName, IDbTransaction transaction)
        {
            var info = new TimeSeriesData.TimeSeriesInfo(timeSeriesName);
            Delete(info, transaction);
        }

        public static void Store(TimeSeriesData.TimeSeriesInfo info, UtcTime from, UtcTime until, UtcTime age, IDbTransaction transaction)
        {
            RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;

            PrepareTimeSeriesInfoForLookup(info, transaction.Connection);
            OracleCommand cmd = new OracleCommand("insert into timeseries_age (tims_key, segment_from, segment_until, age) values (:1, :2, :3, :4)");
            cmd.Parameters.Add(null, info.tims_key);
            cmd.Parameters.Add(null, calendar.ToDatabaseString(from));
            cmd.Parameters.Add(null, calendar.ToDatabaseString(until));
            cmd.Parameters.Add(null, calendar.ToDatabaseString(age));
            Util.ExecuteCommand(cmd, transaction);
        }

        public static void Store(string timeSeriesName, UtcTime from, UtcTime until, UtcTime age, IDbTransaction transaction)
        {
            var info = new TimeSeriesData.TimeSeriesInfo(timeSeriesName);
            Store(info, from, until, age, transaction);
    }
    }
}
