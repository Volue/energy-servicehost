using System;
using System.Collections;
using System.Configuration;
using System.Data;
using System.IO;
//using System.Web.Services;
//using System.Web.Services.Protocols;
using System.Xml;
using System.Xml.Serialization;
using Powel.Icc.TimeSeries.TimeSeriesXML;
using Powel.Icc.TimeSeries;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;

namespace Powel.Icc.Messaging
{
	/// <summary>
	/// Summary description for SubmitTimeSeriesParser.
	/// </summary>
	public class SubmitTimeSeriesParser
	{
		bool noDatabase = false;
		const string sMessage = "SubmitTimeSeries";
		string sMessageId;
		string request;
		SubmitTimeSeriesResponse response;
		int nTotalItems = 0, nTotalTimeSeries = 0, nOKTimeSeries = 0, nPartlyOkTimeSeries = 0;

		public SubmitTimeSeriesParser(string request)
		{
			//initialize all message specific variables etc.
			this.request = request;
			this.response = new SubmitTimeSeriesResponse();	
			IccConfiguration.Data.OracleConnectionString = ConfigurationManager.AppSettings.Get("IccConnectionString");
		}

		public SubmitTimeSeriesResponse ParseMessagePart()
		{
			XmlReader reader = new XmlTextReader(request);
			bool bMessagePartFound = false;

			while (reader.Read())
			{
				switch (reader.LocalName)
				{
					case "MessageID":
						sMessageId = reader.ReadString();
						response.MessageID = sMessageId + "Response";
						response.ReferringMessageID = sMessageId;
						break;
				}
				if( bMessagePartFound)
					break;
			}
			return response;
		}

		public SubmitTimeSeriesResponse Import(string logpath, EventLogModuleItem log)
		{
			TimeSeriesResponseTypeCollection tsRespCol = new TimeSeriesResponseTypeCollection();
			response.SetOfTimeSeriesResponse = tsRespCol;

			try
			{
				XmlDocument document = new XmlDocument();
				document.LoadXml(request);

				XmlReader reader = new XmlNodeReader(document);

				while (reader.Read())
				{
					switch (reader.LocalName)
					{
						case "MessageID":
							sMessageId = reader.ReadString();
							response.MessageID = sMessageId + "Response";
							response.ReferringMessageID = sMessageId;
							log.LogMessage(IccModule.M_XML_WEB_SERVICES, 5402, new string[]{sMessage + ": " + sMessageId}); //New message received
							break;

						case "TimeSeries":
							while(reader.IsStartElement("TimeSeries", "http://www.powel.no/icc/ws/tsws/data/1.0/"))
								ImportTimeSeries(reader.ReadOuterXml(), tsRespCol, log, logpath);
							break;
					}
				}

				this.UpdateMessageStatus(response);
				//log.LogMessage(IccModule.M_XML_WEB_SERVICES, 5403, new string[]{sMessage}); //Message response sent
			}
			catch (DatabaseConnectionException)
			{
				throw;
			}
			catch(Exception e)
			{
				AddMessageInfo(response, "Internal error during import of message '" + sMessage + "' : " + e, StatusType.FAILED);				
				log.LogMessage(IccModule.M_XML_WEB_SERVICES, 5407, new string[]{sMessage}); //Error parsing XML
			}				
			return response;
		}

		void ImportTimeSeries(string fragment, TimeSeriesResponseTypeCollection tsRespCol, EventLogModuleItem log, string logpath)
		{
			DateTime start = DateTime.Now;
//			ImportExport.Log(sMessageId, "Parsing time series.", logpath);

			TsXmlImportData tsImportData = null;
			TimeSeriesResponseType tsResponse = null;

			TextReader reader = new StringReader(fragment);
			XmlSerializer serializer = new XmlSerializer(typeof(TimeSeriesType), "http://www.powel.no/icc/ws/tsws/data/1.0/");

			TextWriter writer = new StringWriter();
			serializer.Serialize(writer, new TimeSeriesType());

			TimeSeriesType tsXML = (TimeSeriesType)serializer.Deserialize(reader);
			
			try
			{
				nTotalTimeSeries++;

				//Parsing
				TimeSeriesXMLAdapter xmlAdapter = new TimeSeriesXMLAdapter();			
				tsImportData = xmlAdapter.ParseTimeSeries(tsXML);
											
				if( tsImportData.TsResponse != null)
				{
					tsResponse = tsImportData.TsResponse;
					nTotalItems+= tsImportData.TotalItems;
					if( tsResponse.Status != StatusType.FAILED)
					{
						//Storage
						TvqStorageStatus[] storageResponse;
						if( noDatabase) //TODO Debug block. To be removed
						{
							ArrayList alStatuses = new ArrayList();
							foreach(Tvq tvq in tsImportData.TimeSeries)
								alStatuses.Add(new TvqStorageStatus(tvq, StorageStatus.Stored, "Stored"));
							storageResponse = (TvqStorageStatus[])alStatuses.ToArray(typeof(TvqStorageStatus));
						}
						else
							storageResponse = ImportExport.ImportTimeSeries(
								tsImportData.TimeSeries, 
								tsImportData.OriginatorSendingTime, 
								tsImportData.ReplacePeriod,
								TsProtocol.PDXML,
								logpath,
								log.Connection);

						//Response update
//						ImportExport.Log(sMessageId, "Updating time series response.", logpath);
						StatusType tsStatus = xmlAdapter.UpdateTimeSeriesResponse(tsImportData, storageResponse);
					}
					//TimeSpan duration = DateTime.Now - start;
					//Console.WriteLine("ImportTimeSeries: after ImportExport.ImportTimeSeries {0} ms.", duration.TotalMilliseconds);

					//Logging
					int logMessageNr = 5409;
					switch(tsResponse.Status)
					{
						case StatusType.OK: logMessageNr = 5400;
							break;
						case StatusType.PARTLYOK: logMessageNr = 5408;
							break;
						case StatusType.FAILED: 
						default:
							logMessageNr = 5409;
							break;
					}
					//LOG:
					//Import status <STATUS> for time series <extRef> (x items in time period <from> - <to>)
					//Time series <extRef> import FAIL: "Missing import definition (2 items)"
					log.LogMessage(IccModule.M_XML_WEB_SERVICES, logMessageNr, new string[]{tsResponse.Status.ToString(), tsImportData.TimeSeries.Name, tsImportData.TotalItems.ToString(), tsImportData.TimeSeries.Span.From.ToString(), tsImportData.TimeSeries.Span.Until.ToString() }); 
					foreach(string error in tsImportData.Errors)
						log.LogMessage(IccModule.M_XML_WEB_SERVICES, 5401, new string[]{tsImportData.TimeSeries.Name, error});
															

					//sum time series statuses, and update ts response if OK and TSFAIL
					if (tsResponse.Status == StatusType.OK)
						nOKTimeSeries++;
					else if (tsResponse.Status == StatusType.PARTLYOK)
						nPartlyOkTimeSeries++;
				}
			}
			catch (DatabaseConnectionException)
			{
				throw;
			}
			catch(Exception e)
			{
				AddMessageInfo(response, "Internal error during import of time series '" + tsXML.Reference.ID + "': " + e, StatusType.FAILED);
				log.LogMessage(IccModule.M_XML_WEB_SERVICES, 5401, new string[]{tsXML.Reference.ID, e.Message}); 
				//unhandled exception, rollback and everything should be set to FAILED
				if( tsImportData != null && tsImportData.TsResponse != null)
				{
					tsImportData.TsResponse.Status = StatusType.FAILED;
					foreach( TsItemsResponseType itemResponse in tsImportData.TsResponse.ItemsResponse)
						itemResponse.Status = StatusType.FAILED;
				}
			}
			finally
			{
				if( tsResponse != null &&
					tsResponse.ResponseLevel != TsResponseLevelType.NONE &&
					(!(tsResponse.ResponseLevel == TsResponseLevelType.TSFAIL && 
					tsResponse.Status == StatusType.OK)))
								
					tsRespCol.Add(tsResponse);
			}
			//duration = DateTime.Now - start;
			//Console.WriteLine("ImportTimeSeries: Total {0} ms.", duration.TotalMilliseconds);
		}

		private void UpdateMessageStatus(SubmitTimeSeriesResponse response)
		{
			AddMessageInfo(response, nTotalTimeSeries + " time series with a total of " + nTotalItems + " items received!", StatusType.OK);
			int nFailedTimeSeries = nTotalTimeSeries - nPartlyOkTimeSeries - nOKTimeSeries;
			if( nFailedTimeSeries >= nTotalTimeSeries)
				AddMessageInfo(response, "All time series failed", StatusType.FAILED);
			else
			{
				if ( nOKTimeSeries > 0)
					AddMessageInfo(response, nOKTimeSeries + " time series imported OK", StatusType.OK);
				if( nPartlyOkTimeSeries > 0)
					AddMessageInfo(response, nPartlyOkTimeSeries + " time series imported PARTLY OK", StatusType.PARTLYOK);
				if( nFailedTimeSeries > 0 )
					AddMessageInfo(response, nFailedTimeSeries + " time series FAILED to import", StatusType.PARTLYOK);
			}
		}

		private void AddMessageInfo(SubmitTimeSeriesResponse response, string info, StatusType statusType)
		{
			if( (int) statusType > (int)response.Status)
				response.Status = statusType;

			if( response.StatusMessage == null)
				response.StatusMessage = new StringCollection();
			response.StatusMessage.Add(info);
		}
	}
}