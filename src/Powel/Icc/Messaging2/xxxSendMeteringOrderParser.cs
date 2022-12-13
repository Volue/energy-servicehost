using System;
using System.Collections.Generic;
using System.Configuration;
using System.Xml;
using Microsoft.Web.Services3;
using Powel.Icc.Data.Entities;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging2.MeteringXML;
using Powel.Icc.Services.Time;


namespace  Powel.Icc.Messaging2
{
	/// <summary>
	/// Summary description for SendMeteringOrderParser.
	/// </summary>
	public class SendMeteringOrderParser
	{
		private EventLogModuleItem log;

		public SendMeteringOrderParser(EventLogModuleItem log)
		{
			this.log = log;
		}

        public sendMeteringOrderRequest InitSendMeteringOrderXML(string messageID, MeteringOrder meteringOrder)
        {
            sendMeteringOrderRequest smo = null;

            if (meteringOrder.SendStatus == MeteringOrder.SendingStatus.OK)
            {
                smo = new sendMeteringOrderRequest();
                smo.order = new OrderType();
                smo.order.measurePointID = meteringOrder.MeasurePoint.Id;
                smo.order.importRequests = new List<ImportRequestType>();

                foreach (Register register in meteringOrder.RegisterImportDefinitions.Keys)
                {
                    var ir = new ImportRequestType();
                    var id = (ImportDefinition)meteringOrder.RegisterImportDefinitions[register];
                    ir.importReference = id.ExtRef;

                    ir.importInterval = (MeterReadingImportIntervalType)Enum.Parse(typeof(MeterReadingImportIntervalType), id.ImportFrequency.ToString());
                    /*
                    if (register.WayOfRegistration == Powel.Icc.Data.Entities.Metering.WayOfRegistrationType.ACCUMULATIVE)
                        ir.Item = (TransferIntervalAccumulativeType)Enum.Parse(typeof(TransferIntervalAccumulativeType), id.ImportFrequency.ToString());
                    else
                        ir.Item = (TransferIntervalDifferentialType)Enum.Parse(typeof(TransferIntervalDifferentialType), id.ImportFrequency.ToString());
                    */
                    //ir.register = registerEntityToXml(register);

                    smo.order.importRequests.Add(ir);
                }

                smo.messageID = messageID;
                smo.validFrom = new UtcTime(DateTime.Now.Ticks); // ToDo: meteringOrder.ImportDefinition.ValidPeriod.From;
            }
            else if (meteringOrder.SendStatus == MeteringOrder.SendingStatus.MISSING_MEASUREPOINT)
            {
                if (log != null)
                    log.LogMessage(6710, new[] { "0", DateTime.Now.ToString(), meteringOrder.SendStatus.ToString() });
            }
            else
            {
                if (log != null)
                    log.LogMessage(6710, new[] { Convert.ToString(meteringOrder.MeasurePoint.Id), DateTime.Now.ToString(), meteringOrder.SendStatus.ToString() });
                //log.LogMessage(6710, new string[] { Convert.ToString(meteringOrder.ImportDefinition.TimsKey), meteringOrder.ImportDefinitions.ValidPeriod.From.ToString(), meteringOrder.SendStatus.ToString() });
            }

            return smo;
        }

        public sendMeteringOrderResponse WriteToFile(sendMeteringOrderRequest smo, string filePrefix)
		{
            var response = new sendMeteringOrderResponse();
			var se = new SoapEnvelope();
			
			string fileName = ConfigurationManager.AppSettings["xmlDumpPath"] + filePrefix + Guid.NewGuid() + ".xml"; 

			se.SetBodyObject(smo);
			var writer = new XmlTextWriter(fileName,System.Text.Encoding.UTF8) {Formatting = Formatting.Indented};
            se.Encoding = System.Text.Encoding.UTF8;
			se.Save(writer);
			writer.Close();

			response.messageID = smo.messageID;
			response.referringMessageID = smo.messageID;
			response.status = StatusType.OK;			
            response.statusMessage = "Message serialized to file: " + fileName;
			return response;
		}

        //public RegisterType registerEntityToXml(Register register)
        //{
        //    RegisterType regXML = new RegisterType();
			
        //    //<RegisterNumber>
        //    regXML.registerNumber = register.RegisterNumber;
        //    //<CapacityDigits>
        //    regXML.registerCapacityDigits = register.CapacityDigits;
        //    //<CapacityDecimals>
        //    regXML.registerCapacityDecimals = register.CapacityDecimals;
        //    //<MeasureUnitCode>
        //    regXML.measureUnitCode = (Messaging.MeteringXML.MeasuringUnitCodeType)Enum.Parse(typeof(Messaging.MeteringXML.MeasuringUnitCodeType), register.MeasureUnitCode.ToString(), true);
        //    //<DirectionOfFlow>
        //    regXML.directionOfFlow = (Messaging.MeteringXML.FlowDirectionType)Enum.Parse(typeof(Messaging.MeteringXML.FlowDirectionType), register.DirectionOfFlow.ToString(), true);
        //    //<PulseConstant>
        //    regXML.pulseConstant = (float) register.PulseConstant;			
        //    //<RemoteMetering>
        //    regXML.remoteMetering = (Messaging.MeteringXML.RemoteMeteringType)Enum.Parse(typeof(Messaging.MeteringXML.RemoteMeteringType), register.RemoteMetering.ToString(), true);
        //    //<WayOfRegistration>
        //    regXML.wayOfRegistration = (Messaging.MeteringXML.WayOfRegistrationType)Enum.Parse(typeof(Messaging.MeteringXML.WayOfRegistrationType), register.WayOfRegistration.ToString(), true);
        //    //<MeteringFunctionCode>
        //    regXML.meteringFunctionCode = (Messaging.MeteringXML.MeteringFunctionCodeType)Enum.Parse(typeof(Messaging.MeteringXML.MeteringFunctionCodeType), register.MeteringFunctionCode.ToString(), true);
        //    return regXML;
        //}
	}
}
