using System;
using System.Collections;
using System.Configuration;
using System.Data;
using System.IO;
using System.Threading;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Services.Time;
using Powel.Icc.Interop;
using Powel.Icc.TimeSeries.Operations;

namespace Powel.Icc.TimeSeries
{
	public class ImportExport
	{
		TimeSeries[] relevantData;
		ReplacePeriod[] relevantReplacePeriods;
		TimeSeriesAge[] ages;
		TvqStorageStatus[] status;

		public TimeSeries[] RelevantData
		{
			get { return relevantData; }
		}

		internal TimeSeriesAge[] Ages
		{
			get { return ages; }
		}

		public static TvqStorageStatus[] ImportTimeSeries(TimeSeries sourceTs, 
		                                                  UtcTime sourceTime, 
		                                                  TimePeriod replacePeriod,
		                                                  TsProtocol protocol,
		                                                  string logpath,
		                                                  IDbConnection connection)
		{
			ArrayList tvqStorageStatus = new ArrayList();
			ExternalReference extRef = ImportExport.FetchImportDefinitions(sourceTs.Name, protocol, connection);

			ImportExport importExport = new ImportExport();
			importExport.ReceiveTimeSeries(sourceTs, sourceTime, replacePeriod, extRef, null, logpath, connection);

			Log("Writing to database.", logpath);

			// We'll run one transaction context per internal series, i.e. per TimeSeriesAge
			foreach (TimeSeriesAge age in importExport.ages)
			{
				bool transactionSerializedSuccessFully = false;

				while (!transactionSerializedSuccessFully)
				{
					try
					{
 						// There is a new transaction scope here because we want to process each 
 						// imported time series separately even if there are several in a given message.
 						// We don't want to roll back the entire message if one of the time series
 						// causes an error.
 
 						using (IDbTransaction transaction = Util.OpenTransaction(connection))
						{
							foreach (ReplacePeriod relevantReplacePeriod in importExport.relevantReplacePeriods)
							{
								if (relevantReplacePeriod.TimeSeriesName == age.TimeSeriesName)
									importExport.DeleteValuesInReplacePeriod(relevantReplacePeriod, transaction);
							}

							foreach (TimeSeries relevantFragment in importExport.relevantData)
							{
								if (relevantFragment.Name == age.TimeSeriesName)
									importExport.StoreTimeSeries(relevantFragment, transaction, logpath);
							}

							importExport.StoreTimeSeriesAge(age, transaction);
							transaction.Commit();
							transactionSerializedSuccessFully = true;
						}
					}
					catch (TransactionSerializationException)
					{
						// If we get here, it means that the serialized
						// transaction wasn't successful, because some other transaction has
						// changed the same data at the same time. In that case, we simply
						// retry.

						Log("Database contention. Will retry in 5 seconds.", logpath);
						Thread.Sleep(5000);
					}
				}
			}

			Log("Done writing to database.", logpath);

			tvqStorageStatus.AddRange(importExport.status);
			return (TvqStorageStatus[])tvqStorageStatus.ToArray(typeof(TvqStorageStatus));
		}

		public void ReceiveTimeSeries(TimeSeries sourceTs, 
		                                UtcTime sourceTime, 
		                                TimePeriod replacePeriod,
		                                ExternalReference externalReference, 
		                                TimeSeriesAge[] currentAges,
		                                string logpath,
		                                IDbConnection connection)
		{
			ArrayList relevantData = new ArrayList();
			ArrayList relevantReplacePeriods = new ArrayList();
			ArrayList updatedAges = new ArrayList();
			ArrayList tvqStorageStatus = new ArrayList();

			VerifyReplacePeriod(sourceTs, replacePeriod);

			// Checks import definitions and splits time series if necessary
			SplitterOperation splitter = new SplitterOperation();
			ISplitterOperator op = new ExternalReferenceSplitterOperator(externalReference);
			TimeSeries[] internalSeriesArray = splitter.Run(sourceTs, op);

			foreach( TimeSeries ts in internalSeriesArray)
			{
				TimeSeries internalSeries = ts;
				if( internalSeries.Name == null) //tvqs missing import definition
					AddStatus(tvqStorageStatus, internalSeries, StorageStatus.MissingImportDefinition, "Missing import definition");
				else if ( internalSeries.Name == ExternalReference.ImportDefinitionIgnore) //import definition that should be ignored
					AddStatus(tvqStorageStatus, internalSeries, StorageStatus.ImportDefinitionIgnore, "Import definition set to be ignored");
				else
				{
					TimePeriod internalReplacePeriod = null;
					externalReference.UseMapping(internalSeries.Name);

					if( externalReference.Digits >= 0 || externalReference.Decimals >= 0)
						internalSeries = TruncateValues(internalSeries, externalReference);

					if( externalReference.Scale != 1.0)
						internalSeries = ScaleValues(internalSeries, externalReference);

					if( externalReference.Offset != 0.0)
						internalSeries = TranslateValues(internalSeries, externalReference);
										
					TimeSeriesAge age = GetTimeSeriesAge(internalSeries.Name, currentAges, connection);
					updatedAges.Add(age);

					// Age is updated on basis of either replacePeriod or individual points
					if (replacePeriod != null)
					{
						internalReplacePeriod = InternalReplacePeriod(replacePeriod, externalReference);
						relevantReplacePeriods.AddRange(TrimOldReplacePeriod(internalSeries, age, internalReplacePeriod, sourceTime));
					}

					Log("Trimming old data.", logpath);
					foreach (TimeSeries trimmedSeries in TrimOldData(internalSeries, age, sourceTime))
					{
						if (trimmedSeries.Name == null)
							AddStatus(tvqStorageStatus, trimmedSeries, 
								StorageStatus.MoreRecentDataExists, "More recent data already exists.");
						else
						{
							relevantData.Add(trimmedSeries);

							// Age is updated on basis of either replacePeriod or individual points
							if (replacePeriod == null)
							{
								Log("Updating age.", logpath);
								UpdateAge(age, trimmedSeries, sourceTime);
							}
						}
					}

					if (internalReplacePeriod != null)
					{
						Log("Updating age.", logpath);
						UpdateAge(age, internalSeries, sourceTime, internalReplacePeriod, externalReference);
					}
				}
			}

			this.relevantData = (TimeSeries[])relevantData.ToArray(typeof(TimeSeries));
			this.relevantReplacePeriods = (ReplacePeriod[])relevantReplacePeriods.ToArray(typeof(ReplacePeriod));
			this.ages = (TimeSeriesAge[])updatedAges.ToArray(typeof(TimeSeriesAge));
			this.status = (TvqStorageStatus[])tvqStorageStatus.ToArray(typeof(TvqStorageStatus));
		}

		TimeSeries TruncateValues(TimeSeries ts, ExternalReference externalReference)
		{
			BinaryOperation binop = new BinaryOperation();
			IBinaryOperator truncate = new TruncateOperator(externalReference.Digits, externalReference.Decimals);
			return binop.Run(ts, truncate);
		}

		TimeSeries ScaleValues(TimeSeries ts, ExternalReference externalReference)
		{
			UnaryOperation unop = new UnaryOperation();
			IUnaryOperator unoperator = new ScaleOperator(externalReference.Scale);
			return unop.Run(ts, unoperator);
		}

		TimeSeries TranslateValues(TimeSeries ts, ExternalReference externalReference)
		{
			UnaryOperation unop = new UnaryOperation();
			IUnaryOperator unoperator = new TranslateOperator(externalReference.Offset);
			return unop.Run(ts, unoperator);
		}

		void VerifyReplacePeriod(TimeSeries ts, TimePeriod replacePeriod)
		{
			if (replacePeriod == null)
				return;

			foreach (Tvq tvq in ts)
				if (!replacePeriod.Intersects(tvq.Time))
					throw new ApplicationException("The replace period does not span the submitted time series.");
		}

		void AddStatus(ArrayList list, TimeSeries ts, StorageStatus status, string message)
		{
			foreach (Tvq tvq in ts)
				list.Add(new TvqStorageStatus(tvq, status, message));
		}

		void UpdateAge(TimeSeriesAge age, TimeSeries newData, UtcTime sourceTime)
		{
			// Cannot update age of a time series with unconstrained resolution unless supplying a replace period.
			if (newData.Resolution == TimeResolution.Unconstrained)
				return; 

			foreach (Tvq tvq in newData)
			{
				UtcTime from = tvq.Time;
				UtcTime until = newData.Resolution.Step(from, 1, RegionalCalendar.UtcCalendar);
				age.UpdateSegment(new TimePeriod(from, until), sourceTime);
			}
		}

		TimePeriod InternalReplacePeriod(TimePeriod replacePeriod, ExternalReference externalReference)
		{
			return replacePeriod.Intersection(externalReference.Span);
		}

		void UpdateAge(TimeSeriesAge age, TimeSeries internalSeries, UtcTime sourceTime, TimePeriod replacePeriod, ExternalReference externalReference)
		{
			if (replacePeriod != null)
				age.UpdateSegment(replacePeriod, sourceTime);
		}

		TimeSeriesAge GetTimeSeriesAge(string timeSeriesName, TimeSeriesAge[] ages, IDbConnection connection)
		{
			if (ages != null)
				foreach (TimeSeriesAge age in ages)
					if (age.TimeSeriesName == timeSeriesName)
						return age;

			DataTable dt = TimeSeriesAgeData.Fetch(timeSeriesName, connection);

			if (dt.Rows.Count > 0)
				return new TimeSeriesAge(dt);

			return new TimeSeriesAge(timeSeriesName);
		}

		void StoreTimeSeries(TimeSeries ts, IDbTransaction transaction, string logpath)
		{
			ArrayList tvqStorageStatus = new ArrayList();
			tvqStorageStatus.AddRange(status);

			try
			{
				OracleTimeSeriesAdapter dbAdapter = new OracleTimeSeriesAdapter(IccProgram.P_IMPCOMMON, transaction);
				tvqStorageStatus.AddRange(dbAdapter.Update(ts));
			}
			catch (StorageException e)
			{
				string message;

				if (e.InnerException != null)
					message = e.Message + ": " + e.InnerException.Message;
				else
					message = e.Message;

				Log(message, logpath);
				AddStatus(tvqStorageStatus, ts, e.Status, message);
			}

			this.status = (TvqStorageStatus[])tvqStorageStatus.ToArray(typeof(TvqStorageStatus));
		}

		void StoreTimeSeriesAge(TimeSeriesAge age, IDbTransaction transaction)
		{
			TimeSeriesAgeData.Delete(age.TimeSeriesName, transaction);

			foreach (TimeSeriesAge.AgeSegment segment in age.Segments)
				TimeSeriesAgeData.Store(
					age.TimeSeriesName, 
					segment.Period.From, 
					segment.Period.Until, 
					segment.Age, 
					transaction);
		}

		void DeleteValuesInReplacePeriod(ReplacePeriod relevantReplacePeriod, IDbTransaction transaction)
		{
			OracleTimeSeriesAdapter db = new OracleTimeSeriesAdapter(IccProgram.P_IMPCOMMON, transaction);
			db.Delete(relevantReplacePeriod.TimeSeriesName, relevantReplacePeriod.Period);
		}
	
		static ExternalReference FetchImportDefinitions(string externalReference, TsProtocol protocol, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			ExternalReference extRef = new ExternalReference();
			DataTable dt = ImportExportData.FetchImportDefinitions(externalReference, protocol, connection);
			string internalName;
			int digits = -1;
			int decimals = -1;
			foreach(DataRow r in dt.Rows)
			{
                if ((int)r["tims_key"] == -1) //ignore ts
                    internalName = ExternalReference.ImportDefinitionIgnore;
                else
                {
                    var info = new TimeSeriesData.TimeSeriesInfo();
                    info.filename = (string) r["filename"];
                    info.tscode = (string) r["tscode"];
                    internalName = info.FullName;
                }

			    if (r["rval1"] != DBNull.Value)
					digits = (int)(decimal) r["rval1"];
				if (r["rval2"] != DBNull.Value)
					decimals = (int)(decimal) r["rval2"];

				extRef.AddMapping(new TimePeriod(cal.ToUtcTime((string)r["valftime"]), cal.ToUtcTime((string)r["valttime"])), internalName, (double)(decimal)r["factor"], (double)(decimal)r["offset"], digits, decimals);
			}

			return extRef;
		}

		static TimeSeries[] TrimOldData(TimeSeries ts, TimeSeriesAge age, UtcTime sourceTime)
		{
			// Checks time series age and returns an array with up to two
			// series, one with older data trimmed away, and one with
			// name=null containing the thrown away data.

			SplitterOperation splitter = new SplitterOperation();
			ISplitterOperator op = new NewerDataSplitterOperator(ts, age, sourceTime);
			return splitter.Run(ts, op);
		}

		static ReplacePeriod[] TrimOldReplacePeriod(TimeSeries ts, TimeSeriesAge age, TimePeriod replacePeriod, UtcTime sourceTime)
		{
			TimePeriod[] replacePeriodFragments = age.TrimOverlappingAndOlder(replacePeriod, sourceTime);
			ReplacePeriod[] result = new ReplacePeriod[replacePeriodFragments.Length];

			for (int i=0; i<result.Length; i++)
				result[i] = new ReplacePeriod(ts.Name, replacePeriodFragments[i]);

			return result;
		}

		public static void Log(string message, string filename)
		{
			Log("", message, filename);

		}

		public static void Log(string messageId, string message, string filename)
		{
			if (filename == null)
				return;

            EnsureDirectoryExists(filename);

            lock (typeof(ImportExport))
            {
                using (TextWriter writer = new StreamWriter(filename, true))
                {
                    writer.WriteLine("{0} - {1} - {2}", DateTime.Now.ToString(), messageId, message);
                }
            }
		}

        static void EnsureDirectoryExists(string filename)
        {
            string dir = Path.GetDirectoryName(filename);
            Directory.CreateDirectory(dir);
        }

        class ReplacePeriod
		{
			public string TimeSeriesName;
			public TimePeriod Period;

			public ReplacePeriod(string timeSeriesName, TimePeriod period)
			{
				this.TimeSeriesName = timeSeriesName;
				this.Period = period;
			}
		}
	}

	/// <summary>
	/// Summary description for ExternalReference.
	/// </summary>
	public class ExternalReference
	{
		public const string ImportDefinitionIgnore = "##PDCONST##ImportDefinitionIgnore##"; //must be special, to avoid accidental identicalness to a time series name
		ArrayList mappings = new ArrayList();
		Mapping mapping;

		public ExternalReference()
		{
		}

		public void AddMapping(TimePeriod period, string internalName, double scale, double offset, int digits, int decimals)
		{
			mappings.Add(new Mapping(period, internalName, scale, offset, digits, decimals));
		}

		public void AddMapping(TimePeriod period, string internaName)
		{
			this.AddMapping(period, internaName, (double)1.0, (double)0.0, -1, -1);
		}

		public bool UseMapping(UtcTime time)
		{
			foreach (Mapping mapping in mappings)
			{
				if (mapping.Period.Intersects(time))
				{
					this.mapping = mapping;
					return true;
				}
			}

			return false;
		}

		public void UseMapping(string internalName)
		{
			foreach (Mapping mapping in mappings)
				if (mapping.InternalName == internalName)
				{
					this.mapping = mapping;
					return;
				}

			throw new ApplicationException("No mapping exists for given internalName.");
		}

		public string InternalName
		{
			get { return mapping.InternalName; }
		}

		public TimePeriod Span
		{
			get { return mapping.Period;}
		}

		public double Scale
		{
			get { return mapping.Scale;}
		}

		public double Offset
		{
			get { return mapping.Offset;}
		}

		public int Digits
		{
			get { return mapping.Digits;}
		}

		public int Decimals
		{
			get { return mapping.Decimals;}
		}


		class Mapping
		{
			TimePeriod period;
			string internalName;
			double scale;
			double offset;
			int digits;
			int decimals;

			public Mapping(TimePeriod period, string internalName, double scale, double offset, int digits, int decimals)
			{
				this.period = period;
				this.internalName = internalName;
				this.scale = scale;
				this.offset = offset;
				this.digits = digits;
				this.decimals = decimals;
			}

			public TimePeriod Period
			{
				get { return period; }
			}

			public string InternalName
			{
				get { return internalName; }
			}

			public double Scale
			{
				get { return scale;}
			}

			public double Offset
			{
				get { return offset;}
			}

			public int Digits
			{
				get { return digits;}
			}

			public int Decimals
			{
				get { return decimals;}
			}
		}
	}
}
