using System;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging2.MeteringXML;
using Powel.Icc.Metering2;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Data;
using Powel.Icc.Services.Time;

namespace  Powel.Icc.Messaging2
{
	/// <summary>
	/// Summary description for FinishInstallationParser.
	/// </summary>
	public class FinishInstallationParser
	{
        private EventLogModuleItem log;
	    private string connectionString;

        public FinishInstallationParser(EventLogModuleItem log,string connectionString)
		{
			this.log = log;
            this.connectionString = connectionString;
		}

		public finishInstallationResponse FinishInstallation(finishInstallationRequest fi)
		{
			StatusType st;
			string statusMessage = null;
			string statusCode = "";

			try
			{	
				var timeOfChange = new UtcTime(fi.validFrom.Ticks);
				var inst = new Installation {Id = fi.installationID};
			    InstallationLogic.FinishInstallation(inst,timeOfChange,connectionString);
				st = StatusType.OK;
			}
			catch(IccException ie)
			{
				st = StatusType.FAILED;
				statusMessage = ie.Message;
				statusCode = ie.Id.ToString();
                log.LogMessage(ie.Id, ie.Args);
			}
			catch(Exception e)
			{
				st = StatusType.FAILED;
				statusMessage = e.Message;
                log.LogMessage(2, new[] { e.Message });//General error message
            }
			return new finishInstallationResponse(fi.messageID, fi.messageID, st, statusCode, statusMessage);		
		}
	}
}
