using System;
using System.Collections;
using System.Data;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Interop;
using Powel.Icc.Services.DataContracts;
using Powel.Icc.Services.Time;

namespace Powel.Icc.TimeSeries
{
	/// <summary>
	/// Summary description for OracleTimeSeriesAdapter.
	/// </summary>
	public class OracleTimeSeriesAdapter : IDisposable
	{
		IDbConnection connection = null;
		IDbTransaction transaction;
		IccProgram iccProgram;
		RegionalCalendar calendar = IccConfiguration.Time.DatabaseCalendar;
		bool autoCommit;

		public IDbConnection GetConnection()
		{
			return connection;	
		}

		public OracleTimeSeriesAdapter(IccProgram iccProgram)
		{
			this.iccProgram = iccProgram;
			OpenTransaction();
			this.autoCommit = true;
		}

		public OracleTimeSeriesAdapter(IccProgram iccProgram, IDbTransaction transaction)
		{
			this.iccProgram = iccProgram;
			this.connection = transaction.Connection;
			this.transaction = transaction;
			// user have supplied it's own transaction object, we should not do any autoCommit.
			this.autoCommit = false;
		}

		void OpenTransaction()
		{
			this.connection = Util.OpenConnection();
			this.transaction = Util.OpenTransaction(connection);
		}

		void CommitTransaction()
		{
			transaction.Commit();
		}

		public void Dispose()
		{
			if (autoCommit)
				DisposeTransaction();
		}

		void DisposeTransaction()
		{
			if (transaction != null)
			{
				transaction.Dispose();
				transaction = null;
			}

			if (connection != null)
			{
				connection.Dispose();
				connection = null;
			}
		}

		public bool Exists(int id)
		{
			return TimeSeriesData.Exists(id, connection);
		}

		public bool Exists(string name)
		{
			return TimeSeriesData.Exists(name, connection);
		}

		public void Fill(TimeSeries ts, string name)
		{
			Fill(ts, name, 0);
		}

		public void Fill(TimeSeries ts, string name, UtcTime beginning, UtcTime end)
		{
			Fill(ts, name, 0, beginning, end);
		}

		public void Fill(TimeSeries ts, int id)
		{
			Fill(ts, id, 0);
		}

		public void Fill(TimeSeries ts, int id, UtcTime beginning, UtcTime end)
		{
			Fill(ts, id, 0, beginning, end);
		}

		public void Fill(TimeSeries ts, int id, int virtualCode)
		{
			TimeSeriesData.TimeSeriesInfo info = new TimeSeriesData.TimeSeriesInfo();
			info.tims_key = id;

			info = TimeSeriesData.Fetch(info, connection);

			if( info == null)
				throw new ArgumentException("Time series id " + id + " not found", "id");

			Fill(ts, info, virtualCode, UtcTime.MinValue, UtcTime.MaxValue);
		}

		public void Fill(TimeSeries ts, string name, int virtualCode)
		{
			Fill(ts, name, virtualCode, UtcTime.MinValue, UtcTime.MaxValue);
		}

		public void Fill(TimeSeries ts, string name, int virtualCode, 
			UtcTime beginning, UtcTime end)
		{
			TimeSeriesData.TimeSeriesInfo info = new TimeSeriesData.TimeSeriesInfo();
			info.tscode = name;

			info = TimeSeriesData.Fetch(info, connection);

			if( info == null)
				throw new ArgumentException("Time series " + name + " not found", "name");

			Fill(ts, info, virtualCode, beginning, end);
		}

		public void Fill(TimeSeries ts, int id, int virtualCode, 
			UtcTime beginning, UtcTime end)
		{
			TimeSeriesData.TimeSeriesInfo info = new TimeSeriesData.TimeSeriesInfo();
			info.tims_key = id;

			info = TimeSeriesData.Fetch(info, connection);

			if( info == null)
				throw new ArgumentException("Time series id " + id + " not found", "name");

			Fill(ts, info, virtualCode, beginning, end);
		}

		void Fill(TimeSeries ts, TimeSeriesData.TimeSeriesInfo info,
			int virtualCode, UtcTime beginning, UtcTime end)
		{
			ts.Name = info.tscode;
			ts.Description = info.descript;
			ts.ID = info.tims_key;
			ts.Resolution = info.Resolution;
			ts.CurveType = info.CurveType;
			ts.Unit = UnitData.GetUnit(info.Unit,connection);
			ts.Paraflag = info.paraflag;
			ts.LastValidatedDate = calendar.ToUtcTime(info.lvdate);
			ts.UseRegistrationHandling = Convert.ToBoolean(info.use_registration_handling);

			FillValues(ts, info.tims_key, virtualCode, info.Resolution,
				beginning, end);
		}

		void FillValues(TimeSeries ts, int timsKey, int vlCode,
		                TimeResolution resolution, UtcTime beginning, UtcTime end)
		{
			if (this.connection == null && this.transaction == null)
				OpenTransaction();

			DataTable dt;
			string timeColumn = "timstamp";
			string valueColumn = "value";
			string statusColumn = "status";

			if (resolution == TimeResolution.Unconstrained || resolution == TimeResolution.Hour || resolution == TimeResolution.Minute15)
			{
				dt = TimeSeriesData.FetchUnconstrainedValues(
				                                             timsKey, vlCode, beginning, end, transaction);
				timeColumn = "datetim";
				valueColumn = "tsvalue";
				statusColumn = "tsstatus";
			}
			//else if (resolution == TimeResolution.Minute15)
			//    dt = TimeSeriesData.Fetch15MinuteValues(
			//        timsKey, vlCode, beginning, end, transaction);
			//else if (resolution == TimeResolution.Hour)
			//    dt = TimeSeriesData.FetchHourValues(
			//        timsKey, vlCode, beginning, end, transaction);
			else
				throw new ArgumentException(String.Format("This time series resolution ({0}) is not yet supported.", resolution), "resolution");
			
			foreach (DataRow dr in dt.Rows)
			{
				//UtcTime time = (resolution == TimeResolution.Unconstrained)
				//    ? calendar.ToUtcTime((DateTime)dr[timeColumn])
				//    : calendar.ToUtcTime((string)dr[timeColumn]);
				UtcTime time = calendar.ToUtcTime((DateTime)dr[timeColumn]);
				object value = dr[valueColumn];

				if (value == DBNull.Value)
					ts.SetValue(new Tvq(time, 0, Quality.Missing), false);
				else
				{
					Tvq tvq = new Tvq(time,
					                  Convert.ToDouble(value),
					                  new Quality(Convert.ToInt32(dr[statusColumn])));
					ts.SetValue(tvq, false);
				}
			}
		}

		/// <summary>
		/// Creates a new time series
		/// </summary>
		/// <param name="ts">TimeSeries - time series to create</param>
		/// <returns>bool - true if a new time series was created - false if the time series already existed</returns>
		public bool Create(TimeSeries ts)
		{
			return Create(ts, TsProducer.Undefined);
		}

		/// <summary>
		/// Creates a new time series
		/// </summary>
		/// <param name="ts">Time sereis to create</param>
		/// <param name="producer">TsProduer - describes what/which program that created the time series</param>
		/// <returns>bool - true if a new time series was created - false if the time series already existed</returns>
		public bool Create(TimeSeries ts, TsProducer producer)
		{
			if (autoCommit)
				OpenTransaction();
			
			try
			{
				// Get some metadata from database
				TimeSeriesData.TimeSeriesInfo timeser =
					new TimeSeriesData.TimeSeriesInfo();

				timeser.tscode = NameComponent(ts.Name);
				timeser.filename = PathComponent(ts.Name);

				if (TimeSeriesData.Fetch(timeser, connection) == null)
				{
					timeser.tspr_key = (int) producer;
					timeser.descript = ts.Description;
					timeser.Resolution = ts.Resolution;
					timeser.CurveType = ts.CurveType;
					timeser.paraflag = ts.Paraflag;
					timeser.use_registration_handling = Convert.ToInt32(ts.UseRegistrationHandling);
					CreateTimeSeries(ts, timeser, transaction);

					if (autoCommit)
						CommitTransaction();

					return true;
				}
				return false;
			}
			catch (Exception ex)
			{
				throw ex;
			}
			finally
			{
				if (autoCommit)
					DisposeTransaction();
			}
		}


		public TvqStorageStatus[] Update(TimeSeries ts)
		{
			return Update(ts, 0);
		}

		/// <summary>
		/// Updates a time series information, including time series values.
		/// </summary>
		/// <param name="ts">TimeSeries - Time Series</param>
		/// <param name="virtualCode">int - </param>
		public TvqStorageStatus[] Update(TimeSeries ts, int virtualCode)
		{
			ArrayList alTvqResponse = new ArrayList();

			if (autoCommit)
				OpenTransaction();

			try
			{
				// Required before using icc_tsstore.
				//TOSA TimeSeriesData.CheckVersion(IccApplication.CodeVersion, transaction);

				// Turn on debugging
				// TimeSeriesData.SetDebug(true, transaction);

				// Get some metadata from database
				TimeSeriesData.TimeSeriesInfo info = new TimeSeriesData.TimeSeriesInfo();
				info.FullName = ts.Name;
			
				//Check ts information against stored time series
				decimal factor = decimal.One;
				int unitKey = 0;
				info = TimeSeriesData.Fetch(info, connection);
				if (info != null)
				{
					if (ts.Resolution != info.Resolution &&
					    !(info.Resolution == TimeResolution.Hour &&
					      ts.Resolution == TimeResolution.Unconstrained &&
					      info.ValueType == PrimaryValueType.Accumulated))
						throw new StorageException(
							"The time series resolution does not match the corresponding database time series.", 
							StorageStatus.InvalidResolution);
					if(ts.Unit != null && ts.Unit.Length > 0 && UnitData.IsUnit(ts.Unit, ref unitKey))
					{
						if (unitKey != info.unme_key)
							if(!UnitData.IsScalable(unitKey, info.unme_key, ref factor))
								throw new StorageException(
									"The time series measuring unit is not compatible with that of the corresponding database time series.", 
									StorageStatus.IncompatibleMeasuringUnit);
					}
					else{} //normal save if unit is invalid or not present
					RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
					ts.ID = info.tims_key;
					ts.Paraflag = info.paraflag;
					ts.LastCorrectedDate = cal.ToUtcTime(info.lcdate);
					ts.LastValidatedDate = cal.ToUtcTime(info.lvdate);
				}
				else
					throw new StorageException("Time series " + ts.Name + " not found", StorageStatus.InternalError );

				StorageStatus status = StorageStatus.Stored;
				string message = "Stored.";
				try
				{
					Write(ts, factor);
				}
				catch(IccException e)
				{
					status = StorageStatus.InternalError;
					message = e.Message;
				}

				foreach (Tvq tvq in ts)
				{
					alTvqResponse.Add(new TvqStorageStatus(tvq, status, message));
				}

				if (autoCommit)
					CommitTransaction();

				return (TvqStorageStatus[])alTvqResponse.ToArray(typeof(TvqStorageStatus));
			}
			finally
			{
				if (autoCommit)
					DisposeTransaction();
			}
		}

	
		void CreateTimeSeries(TimeSeries ts, TimeSeriesData.TimeSeriesInfo info, IDbTransaction transaction)
		{
			info.Resolution = ts.Resolution;
			ts.ID = TimeSeriesData.CreateTimeSeries(info, transaction);
		}

		void Write(TimeSeries ts)
		{
			Write(ts, 1);
		}

		void Write(TimeSeries ts, decimal factor)
		{
			if (ts.Count == 0) return;
			int[] stepTypes = new int[ts.Count];
			int[] timsKeys = new int[ts.Count];
			int[] vlcodes = new int[ts.Count];
			long[] pdtimeDatetimes = new long[ts.Count];
			double[] values = new double[ts.Count];
			int[] qualities = new int[ts.Count];
			int?[] readKeys = new int?[ts.Count];
			int?[] reptKeys = new int?[ts.Count];
			int?[] reasKeys = new int?[ts.Count];
			int[] paraFlags = new int[ts.Count];
			UtcTime[] lcDates = new UtcTime[ts.Count];
			int[] deleteTimsKeys = new int[1];
			int[] deleteVlCodes = new int[1];
			string[] deletePeriodStarts = new string[1];
			string[] deletePeriodEnds = new string[1];

			deleteTimsKeys[0] = ts.ID;
			deleteVlCodes[0] = 0;// no support for other vlcodes
			deletePeriodStarts[0] = calendar.ToDatabaseString(ts[0].Time);
			deletePeriodEnds[0] = calendar.ToDatabaseString(ts[ts.Count - 1].Time);
			
			int i = 0;
			foreach (Tvq tvq in ts)
			{
				stepTypes[i] = ts.Resolution.Steptype;
				timsKeys[i] = ts.ID;
				vlcodes[i] = 0; //default 0. No support for vlcodes yet.
				pdtimeDatetimes[i] = tvq.Time.ToPdtime();
				values[i] = tvq.Value;
				qualities[i] = (int)tvq.Quality;
				readKeys[i] = 0;
				reptKeys[i] = 0;
				reasKeys[i] = 0;
				paraFlags[i] = ts.Paraflag;
				lcDates[i] = ts.LastCorrectedDate;
				//No support for deletePeriods yet
				i++;
			}

			// Store values to database. Append original values if the time series has this flag.
			// Do not overwrite corrected values if 'intelligent storage mode' is on.
			RegionalCalendar TzDb = RegionalCalendar.CurrentNT;
			bool appendOrigSeries = ts.Paraflag == 1 ? true : false;
			bool protectCorrectedValues = TimeSeriesData.GetPDDBEnv("ICC_TSSTORE_OVERWRITE_CC_PERIODS", "true", connection).ToLower() != "true" ? true : false;
			ITsWriter tsWriter = new TsWriter();
			tsWriter.writeValues(new TsWriteItemsFromArrays(stepTypes, timsKeys, vlcodes, pdtimeDatetimes, values,
			                                                qualities, readKeys, reptKeys, reasKeys, null, paraFlags, lcDates, deleteTimsKeys,
			                                                deleteVlCodes, deletePeriodStarts, deletePeriodEnds, TzDb, appendOrigSeries,
			                                                protectCorrectedValues), connection, TzDb, iccProgram, 0);
		}

		public void Delete(string name)
		{
			if (autoCommit)
				OpenTransaction();

			TimeSeriesData.DeleteTimeSeries(name, connection);

			if (autoCommit)
				CommitTransaction();
		}

		public void Delete(string name, TimePeriod period)
		{
			if (autoCommit)
				OpenTransaction();

			TimeSeriesData.DeleteTimeSeries(name, period.From, period.Until, connection);

			if (autoCommit)
				CommitTransaction();
		}

		int GetNumberOfValuesPrRow(TimeResolution resolution)
		{
			if( resolution == TimeResolution.Year ||
				resolution == TimeResolution.Month   ||
				resolution == TimeResolution.Unconstrained)
				return 1;
			else if (resolution == TimeResolution.Week)
				return 53;
			else if (resolution == TimeResolution.Day)
				return 31;
			else if (resolution == TimeResolution.Hour)
				return 24;
			else if (resolution == TimeResolution.Minute15)
				return 96;
			else
				throw new ArgumentException("Invalid TimeResolution");
		}

		int GetIndexPrRow(TimeResolution resolution, DateTime time)
		{	//1-based index
			int year = 0;
			if( resolution == TimeResolution.Year ||
				resolution == TimeResolution.Month ||
				resolution == TimeResolution.Unconstrained)
				return 1;
			else if (resolution == TimeResolution.Week)
				return calendar.GetWeekNumber(calendar.ToUtcTime(time), out year);
			else if (resolution == TimeResolution.Day)
				return time.Day;
			else if (resolution == TimeResolution.Hour)
				return time.Hour + 1;
			else if (resolution == TimeResolution.Minute15)
				return (time.Hour * 4) + (time.Minute /15) + 1;
			else
				throw new ArgumentException("Invalid time resolution");
		}

        /// Added protected internal because tests are now external in Powel.Icc.Test
        protected internal string NameComponent(string fullName)
		{
			int index = fullName.LastIndexOf("/");

			if (index == -1)
				return fullName;
			else
				return fullName.Substring(index + 1, fullName.Length - index - 1);
		}

        /// Added protected internal because tests are now external in Powel.Icc.Test
        protected internal string PathComponent(string fullName)
		{
			int index = fullName.LastIndexOf("/");

			if (index == -1)
				return "/";
			else
				return fullName.Substring(0, index + 1);
		}
	}

	public struct TvqStorageStatus
	{
		Tvq tvq;
		StorageStatus status;	
		string message;

		public TvqStorageStatus(Tvq tvq, StorageStatus status, string message)
		{
			this.tvq = tvq;
			this.status = status;
			this.message = message;
		}

		public string Message
		{
			get{ return message;}
			set{ message = value;}
		}

		public StorageStatus Status
		{
			get{ return status;}
			set{ status = value;}
		}

		public Tvq Tvq
		{
			get{ return tvq;}
			set{ tvq = value;}
		}
	}

	public enum StorageStatus
	{
		Stored,
		InternalError,
		MissingImportDefinition,
		ImportDefinitionIgnore,
		InvalidResolution,
		IncompatibleMeasuringUnit,
		MoreRecentDataExists
	}

	[Serializable]
	public class StorageException : ApplicationException
	{
		StorageStatus status;

		public StorageException(string message, StorageStatus status) : base(message)
		{
			this.status = status;
		}

		public StorageException(string message, StorageStatus status, Exception innerException) : base(message, innerException)
		{
			this.status = status;
		}

		public StorageStatus Status
		{
			get { return status; }
		}
	}
}
