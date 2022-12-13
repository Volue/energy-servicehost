using System;
using System.Collections.Generic;
using Powel.Icc.Messaging2.MeteringXML;
using Powel.Icc.Metering;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Data;
using Powel.Icc.Services.Time;
using Powel.Icc.Diagnostics;

namespace Powel.Icc.Messaging2
{
	/// <summary>
	/// Summary description for SubmitOnDemandMeteringOrderParser.
	/// </summary>
	public class SubmitOnDemandMeteringOrderParser
	{
		private EventLogModuleItem log;
	    private string connectionString;

        public SubmitOnDemandMeteringOrderParser(EventLogModuleItem log, string connectionString)
		{
			this.log = log;
            this.connectionString = connectionString;
		}

        public submitOnDemandMeteringOrderResponse ImportOnDemandMeteringOrder(submitOnDemandMeteringOrderRequest sodmo)
		{
			string[] measurePointIDs			= xmlToMPList(sodmo.onDemandMeteringOrderIn.measurePoints);
			Agreement.DebitingType debitingType	= (Agreement.DebitingType)Enum.Parse(typeof(Agreement.DebitingType), sodmo.onDemandMeteringOrderIn.onDemandDebitingType.ToString(),true);
			UtcTime dateOfTransfer				= sodmo.onDemandMeteringOrderIn.dateTransfer;
			UtcTime dataFromDate				= sodmo.onDemandMeteringOrderIn.dateFrom;
			UtcTime dataToDate					= sodmo.onDemandMeteringOrderIn.dateTo;
			RegistrationReason reason			= (RegistrationReason) Enum.Parse(typeof(RegistrationReason),sodmo.onDemandMeteringOrderIn.readingReason.ToString(),true);

            var response = new submitOnDemandMeteringOrderResponse();
			//var sitc = new StatusItemTypeCollection();
            var sitc = new List<StatusItemType>();

			for (int i=0;i < measurePointIDs.Length;i++)
			{
				var sit = new StatusItemType {id = measurePointIDs[i]};

			    try
			    {
			        MeteringOrderLogic.SaveOnDemandMeteringOrder(measurePointIDs[i], debitingType, dateOfTransfer, dataFromDate,
			                                                     dataToDate, reason, connectionString);
					sit.status			= StatusType.OK;
					sit.statusMessage	= null;
					sit.statusCode		= null;
				}
				catch(IccException ie)
				{
					sit.status			= StatusType.FAILED;
					sit.statusMessage	= ie.Message;
					sit.statusCode		= ie.Id.ToString();
					log.LogMessage(ie.Id, ie.Args);

				}
				catch(Exception e)
				{
					sit.status			= StatusType.FAILED;
					sit.statusMessage	= e.Message;
					log.LogMessage(2, new[]{e.Message});//General error message
				}

				sitc.Add(sit);
				//todo:Log status in ICC EventLog
			}		

			response.messageID = sodmo.messageID;
			response.referringMessageID = sodmo.messageID;

			response.statuses = sitc;

			return response;			
		}

		#region functions
		
		string[] xmlToMPList(List<string> measurePointIDs)
		{	
			var mpIDs = new string[measurePointIDs.Count];
			
			for (int i = 0; i < measurePointIDs.Count; i++)
			{
				mpIDs[i] = measurePointIDs[i]; 
			}
			
			return mpIDs;
		}
		
		#endregion
	}
}
