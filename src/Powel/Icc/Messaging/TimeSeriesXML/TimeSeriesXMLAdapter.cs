using System;
using System.Collections;
using System.Xml;
using Powel.Icc.Common;
using Powel.Icc.Services.DataContracts;
using Powel.Icc.Services.Time;

namespace Powel.Icc.TimeSeries.TimeSeriesXML
{

	// <summary>
	/// Summary description for TimeSeriesWSBusiness.
	/// </summary>
	public class TimeSeriesXMLAdapter
	{
		public TsXmlImportData ParseTimeSeries(TimeSeriesType tsXML)
		{
			//locals
			TsXmlImportData importData = new TsXmlImportData();
			TimeSeriesResponseType resp = new TimeSeriesResponseType();
			importData.TsResponse = resp;
			TimeSeries ts = new TimeSeries();
			resp.ItemsResponse = new TsItemsResponseTypeCollection();
			importData.ResponseLevel = tsXML.ResponseLevel;
			importData.TimeSeries = ts;
			
			int nItems = 0;
			int nItemsInvalid = 0;

			//set response level etc
			if( tsXML.ResponseLevel != TsResponseLevelType.NONE)
			{
				resp.ResponseLevel = tsXML.ResponseLevel;
				resp.StatusSpecified = true;
				resp.Status = StatusType.OK;
			}


			//business handling
			//** Time Series
			//****attributes seqNo and responseLevel
			
			//****Reference TODO Should also be unnecessary, due to xsd validation check
			if( tsXML.Reference.ID.Length == 0)
				AddTsStatus(resp, importData, "Invalid external reference: " + tsXML.Reference.ID, StatusType.FAILED);
			else
			{
				ts.Name = tsXML.Reference.ID;
				ts.Description = tsXML.Reference.ID + " - " + tsXML.Reference.Originator +
					" - " + tsXML.Reference.OriginatorSendingTime;
			}

			//****ValueMetaInfo
			if( tsXML.ValueMetaInfo != null)
			{
				if ( tsXML.ValueMetaInfo.IntervalSpecified)
				{
					switch( tsXML.ValueMetaInfo.Interval )
					{
						case TsIntervalType.HOUR:
							ts.Resolution = TimeResolution.Hour;
							break;
						case TsIntervalType.DAY:
							ts.Resolution = TimeResolution.Day;
							break;
						case TsIntervalType.MIN15:
							ts.Resolution = TimeResolution.Minute15;
							break;
						case TsIntervalType.WEEK:
							ts.Resolution = TimeResolution.Week;
							break;
						case TsIntervalType.MONTH:
							ts.Resolution = TimeResolution.Month;
							break;
						case TsIntervalType.YEAR:
							ts.Resolution = TimeResolution.Year;
							break;
						case TsIntervalType.MOMENTUM:
						default:
							ts.Resolution = TimeResolution.Unconstrained;
							break;
					}
				}
				else
					ts.Resolution = TimeResolution.Unconstrained;

				//measurement unit
				if( tsXML.ValueMetaInfo.MeasurementUnit != null)
					if( tsXML.ValueMetaInfo.MeasurementUnit.Length > 0)
						ts.Unit = tsXML.ValueMetaInfo.MeasurementUnit;
				
				//replace period
				if(tsXML.ValueMetaInfo.ReplacePeriod != null)
				{
					try{importData.ReplacePeriod = ParseTimePeriod(tsXML.ValueMetaInfo.ReplacePeriod);}
					catch(Exception e){ AddTsStatus(resp, importData, e.Message, StatusType.FAILED);}
				}
			}

			//****Items
			TsItemsResponseType itemResp = null;
			Tvq tvq = null;
			foreach( TsPVQItemType item in tsXML.Items)
			{
				nItems++;
				if( resp.Status != StatusType.FAILED) //skip parsing items if ts already failed
				{
					try
					{
						tvq = ParseTsItem(item, tsXML.ResponseLevel, ref itemResp);
					}
					catch(Exception e)
					{
						nItemsInvalid++;
						AddTsItemStatus(importData, itemResp, e.Message, StatusType.FAILED);
						if( itemResp != null) importData.TsResponse.ItemsResponse.Add(itemResp); //error during parsing. Add item response directly
						continue;
					}
					AddImportItem(importData, tvq, itemResp);
				}
				else if(tsXML.ResponseLevel == TsResponseLevelType.TSVFAIL ||
						tsXML.ResponseLevel == TsResponseLevelType.TSV)
				{	//add response items as they arrived, with status FAILED ("skipped")
					importData.TsResponse.ItemsResponse.Add(new TsItemsResponseType(item, StatusType.FAILED, true, "Item skipped because of time series parsing error"));
					nItemsInvalid++;
				}
			}
			
			AddTsStatus(resp, importData, nItems + " items received", StatusType.OK); //not an error actually, but it will work
			if( nItems == 0)
				AddTsStatus(resp, importData, "No items received", StatusType.FAILED);
			if( nItemsInvalid > 0)
				if( nItemsInvalid == nItems) AddTsStatus(resp, importData, "All items failed during parsing of XML", StatusType.FAILED);
						
			//response generation
			switch( tsXML.ResponseLevel)
			{
				case TsResponseLevelType.TS:
				case TsResponseLevelType.TSFAIL:
				case TsResponseLevelType.TSV:
				case TsResponseLevelType.TSVFAIL:
					resp.Reference = tsXML.Reference; 
					resp.TsID = tsXML.TsID;
					break;
				case TsResponseLevelType.NONE:
				default:
					break;
			}
			
			if( resp.Status != StatusType.FAILED)
			{
				importData.OriginatorSendingTime = new UtcTime(tsXML.Reference.OriginatorSendingTimeDateTime.Ticks);
				importData.TotalItems = nItems;
				importData.FailedItems = nItemsInvalid;
			}
			
			return importData;
		}

		//Parses a PVQItem. Checks internal consistency between To and From, and thereafter consistency to Time Series Interval.
		private Tvq ParseTsItem(TsPVQItemType tsItem, TsResponseLevelType respLevel, ref TsItemsResponseType itemResp)
		{
			//locals
			Tvq tvq = null;
			TimePeriod period;

			if( respLevel == TsResponseLevelType.TSV ||
				respLevel == TsResponseLevelType.TSVFAIL)
			{
				itemResp = new TsItemsResponseType();
				itemResp.TsPVQItem = tsItem;
				itemResp.StatusSpecified = true;
				itemResp.Status = StatusType.OK;
			}
			
			period = ParseTimePeriod(new TsTimePeriodType(tsItem.FromDateTime, tsItem.ToDateTime, tsItem.ToSpecified, tsItem.Duration));
			tvq = new Tvq(period.From, tsItem.Value, ParseQuality(tsItem));
			
			return tvq;
		}

		private void AddImportItem(TsXmlImportData importData, Tvq tvq, TsItemsResponseType itemResponse)
		{
			if( importData != null && tvq != null)
			{
				if (! importData.ItemsResponses.Contains(tvq.Time.Ticks))
				{
					importData.ItemsResponses.Add(tvq.Time.Ticks, itemResponse);
					importData.TimeSeries.SetValue(tvq);
				}
				else
				{
					AddTsItemStatus(importData, itemResponse, "Item with time '" + tvq.Time.ToXmlDateTime() + "' already added", StatusType.FAILED);
					if(itemResponse != null) importData.TsResponse.ItemsResponse.Add(itemResponse);
				}
			}
		}

		private void AddTsItemStatus(TsXmlImportData importData, TsItemsResponseType itemResponse, string statusMessage, StatusType statusType)
		{
			if(importData != null)
			{
				importData.AddItemStatus(statusMessage, statusType == StatusType.FAILED);

				if(itemResponse != null &&
					(importData.ResponseLevel == TsResponseLevelType.TSV ||
					  (importData.ResponseLevel == TsResponseLevelType.TSVFAIL &&
						statusType == StatusType.FAILED
					  )
				    )
				  )
				{
					//Set status only to worse found.
					if( (int) statusType > (int)itemResponse.Status)
					{
						itemResponse.Status = statusType;
						itemResponse.StatusMessage = statusMessage;
					}
					else if( (int) statusType == (int)itemResponse.Status)
					{	//if similar statusType, preserve first error found
						if(itemResponse.StatusMessage == null || itemResponse.StatusMessage.Length == 0)
							itemResponse.StatusMessage = statusMessage;
					}
				}
			}
		}

		private void AddTsStatus(TimeSeriesResponseType response, TsXmlImportData importData, string statusMessage, StatusType statusType)
		{
			if( response != null &&
				response.ResponseLevel != TsResponseLevelType.NONE)
			{
				//Set status only to worse
				if( (int) statusType > (int)response.Status)
					response.Status = statusType;

				if( response.StatusMessage == null)
					response.StatusMessage = new StringCollection();
				response.StatusMessage.Add(statusMessage);

				if( statusType != StatusType.OK)
					importData.Errors.Add(statusMessage);
			}
		}


		Quality ParseQuality( TsPVQItemType tsItem)
		{
			Quality q = new Quality(0);
			if( tsItem.N_OK)		q |= Quality.NotOK;
			if( tsItem.Validated)	q |= Quality.Validated;
			if( tsItem.Corrected)	q |= Quality.Corrected;
			if( tsItem.Manual)		q |= Quality.Manual;
			if( tsItem.Suspect)		q |= Quality.Suspect;
			if( tsItem.Missing)		q |= Quality.Missing;
			if( tsItem.Locked)		q |= Quality.Locked;
			if( tsItem.Accepted)	q |= Quality.Accepted;
			
			if( tsItem.Custom != null)
				q.SetCustom(Convert.ToInt32(tsItem.Custom));
			
			if( tsItem.ValMethodSpecified)
				q.SetValidation(TsXMLEnumMapper.GetValMethod(tsItem.ValMethod));
			else
				q.SetValidation(ValidationMethods.None);
			
			if( tsItem.CorMethodSpecified)
				q.SetCorrection(TsXMLEnumMapper.GetCorMethod(tsItem.CorMethod));
			else
				q.SetCorrection(CorrectionMethods.None);

			return q;
		}

		TimePeriod ParseTimePeriod( TsTimePeriodType tsTime)
		{
			//from
			UtcTime from = tsTime.FromDateTime;
			TimePeriod period = new TimePeriod(from);

			//To
			TimeSpan dur = TimeSpan.MinValue;
			if( tsTime.ToSpecified)
			{
				period = new TimePeriod(from, tsTime.ToDateTime);
				dur = period.From - period.Until;
			}

			//Duration
			TimeSpan dur2;
			if( tsTime.Duration != null && tsTime.Duration.Length > 0)
			{
				dur2 = XmlConvert.ToTimeSpan(tsTime.Duration);
				if( dur != TimeSpan.MinValue)
				{
					if( !dur2.Equals(dur))
						throw new ArgumentException("Ambiguous time period", tsTime.Duration);
				}
				else
					period = new TimePeriod(from, new UtcTime(from.Ticks + dur2.Ticks));
			}
		
			return period;
		}

		/// <summary>
		/// Updates submitTimeSeriesResponse with storage results. If TSFAIL or TSVFAIL, OK parts of response is removed.
		/// </summary>
		public StatusType UpdateTimeSeriesResponse(TsXmlImportData metadata, TvqStorageStatus[] alStorageResponse)
		{
			TimeSeriesResponseType tsResponse = metadata.TsResponse;
			TsResponseLevelType responseLevel = tsResponse.ResponseLevel;
			
			if(tsResponse != null) //tsResponse should not be null, not even for responseLevel NONE
			{
				Hashtable htItemResponses = metadata.ItemsResponses;

				//step through every tvq-response from storage process
				Hashtable htErrors = new Hashtable();
				StatusType statusType;
				string statusMessage;
				TsItemsResponseType responseType;
				foreach(TvqStorageStatus tvqStatus in alStorageResponse)
				{
					responseType = null;
					statusMessage = tvqStatus.Message;

					//OK status
					if( tvqStatus.Status == StorageStatus.Stored ||
						tvqStatus.Status == StorageStatus.ImportDefinitionIgnore ||
						tvqStatus.Status == StorageStatus.MoreRecentDataExists)
					{
						statusType = StatusType.OK;
					}
					else	//FAILED status
					{
						metadata.FailedItems++;
						statusType = StatusType.FAILED;

						if (tvqStatus.Status == StorageStatus.InternalError)
							statusMessage = "Internal error: " + tvqStatus.Message;
					}

					if( htItemResponses.Contains(tvqStatus.Tvq.Time.Ticks))
						responseType = (TsItemsResponseType)htItemResponses[tvqStatus.Tvq.Time.Ticks];

					AddTsItemStatus(metadata, responseType, statusMessage, statusType);
					if(responseType != null) tsResponse.ItemsResponse.Add(responseType);	
				}

				foreach(string sStatus in metadata.ItemStatuses.Keys)
				{
					AddTsStatus(tsResponse, metadata, "STATUS: " + sStatus + " (" + ((Counter)metadata.ItemStatuses[sStatus]).Count + " items)", StatusType.OK);
				}

				foreach(string sError in metadata.ItemErrors.Keys)
				{
					AddTsStatus(tsResponse, metadata, "ERROR: " + sError + " (" + ((Counter)metadata.ItemErrors[sError]).Count + " items)", StatusType.PARTLYOK);
				}
				
				if( metadata.FailedItems >= metadata.TotalItems)
					tsResponse.Status = StatusType.FAILED;
			
				return tsResponse.Status;
			}
			else
				return StatusType.FAILED;
		}
	}

	class Counter
	{
		int i;
		
		public Counter(): this(0){}
		
		public Counter(int value)
		{
			i = value;
		}

		public void Add()
		{
			i++;
		}

		public int Count
		{
			get{return i;}
		}
	}

	//**********************************************************************************************
	//TsXmlImportData
	//**********************************************************************************************
	public class TsXmlImportData
	{
		TimeSeries ts;						//Time series to fill with parsed items
		TimeSeriesResponseType tsResponse;  //Actual response to be returned
		Hashtable htItemsResponses;			//Responses for items that is sent to storage. Parsing error responses are directly attached to tsResponse.
		TimePeriod replacePeriod;
		UtcTime originatorSendingTime;
		int nTotalItems;
		int nFailedItems;
		ArrayList errors;
		TsResponseLevelType responseLevel;
		Hashtable htItemErrors;
		Hashtable htItemStatuses;

		public TsXmlImportData()
		{
			errors = new ArrayList();
			htItemErrors = new Hashtable();
			htItemStatuses = new Hashtable();
			htItemsResponses = new Hashtable();
		}

		public TsXmlImportData(TimeSeries ts, TimeSeriesResponseType tsResponse, Hashtable htItemsResponses, TimePeriod replacePeriod, UtcTime originatorSendingTime, int nTotalItems, int nFailedItems, ArrayList errors, TsResponseLevelType responseLevel, Hashtable itemErrors, Hashtable itemStatuses)
		{
			this.ts = ts;
			this.tsResponse = tsResponse;
			this.htItemsResponses = htItemsResponses;
			this.replacePeriod = replacePeriod;
			this.originatorSendingTime = originatorSendingTime;
			this.nTotalItems = nTotalItems;
			this.nFailedItems = nFailedItems;
			this.errors = errors;
			this.responseLevel = responseLevel;
			this.htItemErrors = itemErrors;
			this.htItemStatuses = itemStatuses;
		}

		public int TotalItems
		{
			get{ return nTotalItems;}
			set{ nTotalItems = value;}
		}

		public int FailedItems
		{
			get{ return nFailedItems;}
			set{ nFailedItems = value;}
		}

		public UtcTime OriginatorSendingTime
		{
			get{ return originatorSendingTime;}
			set{ originatorSendingTime = value;}
		}

		public TimeSeriesResponseType TsResponse
		{
			get{ return tsResponse;}
			set{ tsResponse = value;}
		}

		public Hashtable ItemsResponses
		{
			get{ return htItemsResponses;}
			set{ htItemsResponses = value;}
		}

		public TimePeriod ReplacePeriod
		{
			get{ return replacePeriod;}
			set{ replacePeriod = value;}			
		}

		public TimeSeries TimeSeries
		{
			get{ return ts; }
			set{ ts=value; }
		}

		public ArrayList Errors
		{
			get{ return errors; }
			set{ errors = value; }
		}

		public TsResponseLevelType ResponseLevel
		{
			get{ return responseLevel; }
			set{ responseLevel = value; }
		}

		public Hashtable ItemErrors
		{
			get{ return htItemErrors;}
			set{ htItemErrors = value;}
		}

		public Hashtable ItemStatuses
		{
			get{ return htItemStatuses;}
			set{ htItemStatuses = value;}
		}

		public void AddItemStatus(string status, bool bError)
		{
			Hashtable ht;
			if( bError)
				ht = htItemErrors;
			else
				ht = htItemStatuses;

			if( ht == null)
				ht = new Hashtable();

			if(!ht.Contains(status))
				ht.Add(status, new Counter(1));
			else
				((Counter)ht[status]).Add();
		}
	}
}
