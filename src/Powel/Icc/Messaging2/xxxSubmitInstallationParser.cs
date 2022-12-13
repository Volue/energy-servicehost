using System;
using System.Configuration;
using Powel.Icc.Interop;
using Powel.Icc.Messaging2.MeteringXML;
using Powel.Icc.Metering2;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Data;
using Powel.Icc.Diagnostics;

namespace Powel.Icc.Messaging2
{
	/// <summary>
	/// 
	/// </summary>
	public class SubmitInstallationParser
	{
		private EventLogModuleItem log;
	    private String connectionString;

		public SubmitInstallationParser(EventLogModuleItem log,String connectionString)
		{
			this.log = log;
		    this.connectionString = connectionString;
		}

		public submitInstallationResponse ImportInstallation(submitInstallationRequest si)
		{
			StatusType st;
			string statusMessage = null;
			string statusCode = null;
			
			try
			{	
				Installation inst =  xmlToEntity(si);

				//rename installation
				if(!string.IsNullOrEmpty(si.installation.oldInstallationID))
				{
				    InstallationLogic.RenameInstallation(si.installation.oldInstallationID,
				                                         si.installation.installationID, log, connectionString);
				}
				//rename measure points
				foreach (MeasurePointType mp in si.installation.measurePoints)
				{
					if (!string.IsNullOrEmpty(mp.oldMeasurePointID)) {
						log.LogMessage(10027, new string[] { mp.oldMeasurePointID, mp.measurePointID, 
						                                     si.installation.installationID } );
					    MeasurePointLogic.RenameMeasurePoint(mp.oldMeasurePointID, mp.measurePointID, connectionString);
					}
				}
				
				//save installation
				int retryCount = GetUniqueKeyRetryCount();
				int retryDelay = GetUniqueKeyRetryDelay();
			    bool bOk = InstallationLogic.SubmitInstallation(inst, si.validFrom, retryCount, retryDelay, log, connectionString);

				if (bOk) {
					st = StatusType.OK;
				} else {
					log.LogMessage(10025, new[] { si.installation.installationID ,
					                              si.installation.oldInstallationID,
					                              si.messageID });
					st = StatusType.FAILED;
				}
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
				log.LogMessage(10026, new[]{e.Message});//General error message
			}

			submitInstallationResponse response = new submitInstallationResponse(si.messageID, si.messageID, st, statusCode, statusMessage);
			return response;
		}


#region functions
		private Installation xmlToEntity(submitInstallationRequest si)
		{
			MeasurePoint[] measurePoints = new MeasurePoint[si.installation.measurePoints.Count];
			int i = 0;

			foreach (MeasurePointType mp in si.installation.measurePoints)
			{
				measurePoints[i] = new MeasurePoint();
				if(mp.measurePointID != null)          measurePoints[i].Id                  = mp.measurePointID;
				if(mp.measurePointDescription != null) measurePoints[i].Description         = mp.measurePointDescription;
				if(mp.secondaryMeasurePointID != null) measurePoints[i].SecondaryId         = mp.secondaryMeasurePointID;
				if(mp.fuseSize != null)                measurePoints[i].FuseSize            = mp.fuseSize;
				if(mp.gridOwner != null)               measurePoints[i].GridOwner           = mp.gridOwner;
				if(mp.loadProfile != null)             measurePoints[i].LoadProfile         = mp.loadProfile;
				if(mp.netAreaCode != null)             measurePoints[i].NetAreaCode         = mp.netAreaCode;
				if(mp.numberOfPhasesSpecified)         measurePoints[i].NumberOfPhases      = mp.numberOfPhases;
				if(mp.orgUnit != null)                 measurePoints[i].OrgUnit             = mp.orgUnit;
				if(mp.stationID != null)               measurePoints[i].StationId           = mp.stationID;
				if(mp.subscribedPowerSpecified)        measurePoints[i].SubscribedPower     = mp.subscribedPower;
				if(mp.supplyVoltage != null)           measurePoints[i].SupplyVoltage       = mp.supplyVoltage;
				if(mp.xCoordinateSpecified)            measurePoints[i].XCoordinate         = (decimal)mp.xCoordinate;
				if(mp.yCoordinateSpecified)            measurePoints[i].YCoordinate         = (decimal)mp.yCoordinate;
				if(mp.location != null)                measurePoints[i].Location            = mp.location;
				if(mp.yearlyVolumeSpecified)           measurePoints[i].YearlyVolume        = mp.yearlyVolume;
				if(mp.supplier != null)                measurePoints[i].Supplier            = mp.supplier;
				if(mp.centralNetPointCode != null)     measurePoints[i].CentralNetPointCode = mp.centralNetPointCode;
                if (mp.stateSpecified)
                {
                    switch (mp.state)
                    {
                        case MeasPntStateType.ACTIVE:
                            measurePoints[i].State = MeasurePoint.StateTypes.Active;
                            break;
                        case MeasPntStateType.CLOSED:
                            measurePoints[i].State = MeasurePoint.StateTypes.Closed;
                            break;
                        case MeasPntStateType.NO_CUSTOMER:
                            measurePoints[i].State = MeasurePoint.StateTypes.NoCustomer;
                            break;
                        default:
                            measurePoints[i].State = MeasurePoint.StateTypes.Unspecified;
                            break;
                    }
                }
                else
                {
                    measurePoints[i].State = MeasurePoint.StateTypes.Unspecified;
                }
			    i++;
			}

			Installation inst = new Installation();
			if(si.installation.installationID != null)          inst.Id              = si.installation.installationID;
			if(si.installation.secondaryInstallationID != null) inst.SecondaryId     = si.installation.secondaryInstallationID;
			if(si.installation.installationDescription != null) inst.Description     = si.installation.installationDescription;
			if(si.installation.installationCode != null)        inst.Code            = si.installation.installationCode;
			if(si.installation.customerName != null)            inst.CustomerName    = si.installation.customerName;
			if(si.installation.customerID != null)              inst.CustomerId      = si.installation.customerID;
			if(si.installation.appartmentNumber != null)        inst.ApartmentNo     = si.installation.appartmentNumber;
			if(si.installation.houseNumber != null)             inst.HouseNo         = si.installation.houseNumber;
			if(si.installation.addressMain != null)             inst.AddressMain     = si.installation.addressMain;
			if(si.installation.coInfoMain != null)              inst.CoInfoMain      = si.installation.coInfoMain;
			if(si.installation.zipCodeMain != null)             inst.PostCodeMain    = si.installation.zipCodeMain;
			if(si.installation.cityMain != null)                inst.CityMain        = si.installation.cityMain;
			if(si.installation.addressContact != null)          inst.AddressContact  = si.installation.addressContact;
			if(si.installation.coInfoContact != null)           inst.CoInfoContact   = si.installation.coInfoContact;
			if(si.installation.zipCodeContact != null)          inst.PostCodeContact = si.installation.zipCodeContact;
			if(si.installation.cityContact != null)             inst.CityContact     = si.installation.cityContact;
			if(si.installation.phoneHome != null)               inst.PhoneHome       = si.installation.phoneHome;
			if(si.installation.phoneWork != null)               inst.PhoneWork       = si.installation.phoneWork;
			if(si.installation.phoneMobile != null)             inst.PhoneMobile     = si.installation.phoneMobile;
			if(measurePoints.Length > 0)                        inst.MeasurePoints   = measurePoints;
			
			return inst;
		}

		private Int32 GetUniqueKeyRetryCount()
		{
			string retryCount = ConfigurationManager.AppSettings["UniqueKeyRetryCount"];
			if(string.IsNullOrEmpty(retryCount))
				return 100;
			return Convert.ToInt32(retryCount);
		}

		private Int32 GetUniqueKeyRetryDelay()
		{
			string retryCount = ConfigurationManager.AppSettings["UniqueKeyRetryDelay"];
			if (string.IsNullOrEmpty(retryCount))
				return 100;
			return Convert.ToInt32(retryCount);
		}

#endregion
	}
}
