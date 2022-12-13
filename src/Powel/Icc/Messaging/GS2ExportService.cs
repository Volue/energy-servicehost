using Powel.Icc.Common;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Data;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.EmbeddedTextMessageXml;
using Powel.Icc.Metering;
using Powel.Icc.Process;
using Powel.Icc.Services.Time;
using System.Collections;
using System.Data;
using System.Diagnostics;
using System.IO;
using System.Text;
using System;
using TS = Powel.Icc.TimeSeries;

namespace Powel.Icc.Messaging
{
	/// <summary>
	/// Summary description for GS2ExportService.
	/// </summary>
	public class GS2ExportService : ServiceIterationBase
	{
		RegionalCalendar dbCalendar = IccConfiguration.Time.DatabaseCalendar;

        public GS2ExportService(
			EventLog eventLog, EventLogModuleItem iccLog) 
			: base(eventLog, iccLog)
		{
		}

		public override string ServiceIterationName
		{
			get { return "GS2 Export Service"; }
		}

		public override IccModule ServiceIterationModule
		{
			get { return IccModule.M_POWEL_ICC_GS2_EXPORT_SERVICE; }
		}

		public override void Initialize()
		{
		}

		public override void RunIteration(out bool possiblyMoreWork)
		{
			bool etlMoreWork;
			bool gs2MoreWork;
			bool gs2ExpMoreWork;

			ProcessEtlData(out etlMoreWork);
			ProcessGS2Data(out gs2MoreWork);
			ProcessDifferentialGS2Export(out gs2ExpMoreWork);

			possiblyMoreWork = etlMoreWork | gs2MoreWork | gs2ExpMoreWork;
		}

		void ProcessEtlData(out bool possiblyMoreWork)
		{
			possiblyMoreWork = false;

			DataTable dt = MessageData.FetchNewReadings();

			if (dt == null || dt.Rows.Count == 0)
				return;

			var gs2 = new GS2Message("Meter-reading", 
				IccConfiguration.Messaging.GS2To,
				IccConfiguration.Messaging.GS2From,
				"0");

			var rowids = new ArrayList();

			foreach (DataRow dr in dt.Rows)
			{
				rowids.Add(dr["rowid"]);

				var tvq = new TS.Tvq(
					dbCalendar.ToUtcTime((string)dr["time"]), 
					Convert.ToDouble(dr["value"]));

				var ts = new TS.TimeSeries();
				ts.Name = (string)dr["globeref"];
				ts.Unit = (string)dr["unit"];
				ts.SetValue(tvq, false);

				gs2.AddTimeSeries(ts);
			}

			SendMessage(gs2);
			MessageData.UpdateRowStatus((string[])rowids.ToArray(typeof(string)), "SENT");
			possiblyMoreWork = true;
		}

		void ProcessGS2Data(out bool possiblyMoreWork)
		{
			possiblyMoreWork = false;

			IComparer comparer = new DateComparer();
			var list = new SortedList(comparer);

			var directory = new DirectoryInfo(IccConfiguration.Messaging.GS2UnprocessedFilePath);

			foreach (FileInfo file in directory.GetFiles())
				list.Add(file, null);

			if (list.Count == 0)
				return;

			var gs2 = new GS2Message();

			// Iterates through a sorted list of files.
			foreach (DictionaryEntry entry in list)
			{
				var file = (FileInfo)entry.Key;
				
				using (TextReader reader = new StreamReader(file.OpenRead(), Encoding.Default))
				{
					string fileText = reader.ReadToEnd();
					gs2.AppendGS2Message(new GS2Message(fileText));
				}
			}

			if (!gs2.IsEmpty)
				SendMessage(gs2);

			foreach (DictionaryEntry entry in list)
			{
				var file = (FileInfo)entry.Key;
				string processedPath = IccConfiguration.Messaging.GS2ProcessedFilePath;

				if (processedPath == null)
					file.Delete();
				else
					file.MoveTo(Path.Combine(processedPath, file.Name));
			}

			possiblyMoreWork = true;
		}

		void SendMessage(GS2Message gs2)
		{
			var message = new SendEmbeddedTextMessage(
				Guid.NewGuid().ToString(), gs2.MessageType, gs2.ToString());

			var service = new EmbeddedTextMessage();
			service.Url = IccConfiguration.Messaging.GS2ReceiverUrl;
			service.SendEmbeddedTextMessage(message);
		}

		class DateComparer : IComparer
		{
			public int Compare(object x, object y)
			{
				var file1 = x as FileInfo; 
				var file2 = y as FileInfo; 

				if (file1 == null || file2 == null)
					return 0;

				DateTime date1 = file1.LastWriteTimeUtc; 
				DateTime date2 = file2.LastWriteTimeUtc; 

				if (date1 == date2) 
					return 0; 
				else if (date1 < date2) 
					return -1; 
				else 
					return +1; 
			}
		}

		void ProcessDifferentialGS2Export(out bool possiblyMoreWork)
		{
			possiblyMoreWork = false;

			using(IDbConnection connection = Util.OpenConnection())
			{
				using (IDbTransaction transaction = Util.OpenTransaction(connection))
				{
					MeteringOrderOnDemandCollection[] moodCols = MeteringOrderLogic.GetDifferentialMeteringOrders(UtcTime.Now, connection);
					
					foreach(MeteringOrderOnDemandCollection moodCol in moodCols)
					{	
						foreach(MeteringOrderOnDemand mood in moodCol.MeteringOrdersOnDemand)
						{
							var export = new GS2Export(TsProtocol.GS2, mood.OpunKey, moodCol.DataFromTime, moodCol.DataToTime, GS2Export.ValueTypeType.ALL_VALUES);
							export.AddTimsKey(mood.TimsKey);
							try
							{
								export.RunExport();
								mood.ExportStatus = MeteringOrderOnDemand.ExpStatus.EXPORTED_OK;
								iccLog.LogMessage(6712, new string[]{mood.TimsKey.ToString(), mood.OpunKey.ToString()});
							}
							catch(Exception e)
							{
								iccLog.LogMessage(6711, new string[]{mood.TimsKey.ToString(), mood.OpunKey.ToString(), e.Message});
								mood.ExportStatus = MeteringOrderOnDemand.ExpStatus.EXPORT_FAILED;
							}
						}
					}

					MeteringOrderLogic.MarkExportedOnDemandMeteringOrders(moodCols, connection);
					transaction.Commit();
				}
			}
		}
	}
}