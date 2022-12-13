using System;
using System.Configuration;
using Powel.Icc.Data;
using Powel.Icc.Data.Metering;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging2.MeteringXML;
using Powel.Icc.Metering2;

namespace Powel.Icc.Messaging2
{
	public class SubmitRegistersParser
	{
		private EventLogModuleItem log;
	    private string connectionString;

		public SubmitRegistersParser(EventLogModuleItem log,string connectionString)
		{
			this.log = log;
		    this.connectionString = connectionString;
		}

		public submitRegistersResponse ImportRegisters(submitRegistersRequest sr)
		{
			var st = new StatusType();
			string statusMessage = null;
			string statusCode = null;
	        
			try
			{
				int retryCount = GetUniqueKeyRetryCount();
				int retryDelay = GetUniqueKeyRetryDelay();
				Register[] registers = xmlToEntity(sr.registerInfo);
				TimeSeriesDefinition[] timeseries = xmlToTsEntity(sr.registerInfo);
			    RegisterLogic.SubmitRegisters(registers, timeseries, sr.registerInfo.measurePointID,
			                                  sr.registerInfo.meterID, sr.validFrom, retryCount, retryDelay, connectionString);
			}
			catch (IccException ie)
			{
				st = StatusType.FAILED;
				statusMessage = ie.Message;
				statusCode = ie.Id.ToString();
				log.LogMessage(ie.Id, ie.Args);
			}
			catch (Exception e)
			{
				st = StatusType.FAILED;
				statusMessage = e.Message;
				log.LogMessage(2, new[] { e.Message });//General error message
			}

			var response = new submitRegistersResponse(sr.messageID, sr.messageID, st, statusCode, statusMessage);
			return response;
		}

		#region functions

		private static Register[] xmlToEntity(RegisterInfoType ri)
		{
			var registers = new Register[ri.registers.Count];
			int i = 0;

			foreach (RegisterType register in ri.registers)
			{
				var r = new Register();
			      
				r.senderID = register.senderID;
			      
				//ReadingTypeLogic.DecodeReadingType(ref r, register.readingType);
				r.readingType = ReadingTypeLogic.GetReadingType(register.readingType);
				//r.MeteringFunctionCode = (Data.Entities.Metering.MeteringFunctionCodeType)Enum.Parse(typeof(Data.Entities.Metering.MeteringFunctionCodeType), register.MeteringFunctionCode.ToString(), true);
			      
				r.PulseConstant = register.pulseConstant;
				r.CapacityDecimals = register.registerCapacityDecimals;
				r.CapacityDigits = register.registerCapacityDigits;
				r.RegisterNumber = register.registerNumber;
				r.RemoteMetering = (Data.Entities.Metering.RemoteMeteringType)Enum.Parse(typeof(Data.Entities.Metering.RemoteMeteringType), register.remoteMetering.ToString(), true);

				r.importUseTrafo = register.importUseTrafo;
				r.importUsePulse = register.importUsePulse;
				r.importFormat = register.importFormat;
				r.importReference = register.importReference;
				if (register.importIntervalSpecified)
					r.importInterval = (int)Enum.Parse(typeof(RegisterData.Frequency), register.importInterval.ToString());
                if (register.annualConsumptionSpecified)
                    r.AnnualConsumption = register.annualConsumption;

				//r.WayOfRegistration = (Data.Entities.Metering.WayOfRegistrationType)Enum.Parse(typeof(Data.Entities.Metering.WayOfRegistrationType), register.WayOfRegistration.ToString(), true);
			      
				registers[i] = r;
				i++;
			}
			return registers; 
		}

		private static TimeSeriesDefinition[] xmlToTsEntity(RegisterInfoType ri)
		{
			var timeseries = new TimeSeriesDefinition[ri.registers.Count];
			int i = 0;
	        
			foreach (var timeserie in ri.registers)
			{
				var t = new TimeSeriesDefinition();
				//t.IECFlowDirection = (IECFlowDirectionType) timeserie.readingType.FlowDirection;
			      
				timeseries[i] = t;
			}
			return timeseries;
		}

		private static Int32 GetUniqueKeyRetryCount()
		{
			string retryCount = ConfigurationManager.AppSettings["UniqueKeyRetryCount"];
			if (string.IsNullOrEmpty(retryCount))
				return 100;
			return Convert.ToInt32(retryCount);
		}

		private static Int32 GetUniqueKeyRetryDelay()
		{
			string retryCount = ConfigurationManager.AppSettings["UniqueKeyRetryDelay"];
			if (string.IsNullOrEmpty(retryCount))
				return 100;
			return Convert.ToInt32(retryCount);
		}

		#endregion functions
	}
}
