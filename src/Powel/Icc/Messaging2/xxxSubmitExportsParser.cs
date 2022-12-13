using System;
using Powel.Icc.Data;
using Powel.Icc.Data.Metering;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging2.MeteringXML;
using Powel.Icc.Metering2;

namespace Powel.Icc.Messaging2
{
    public class SubmitExportsParser
    {
        private EventLogModuleItem log;
        private string connectionString;

        public SubmitExportsParser(EventLogModuleItem log, string connectionString)
		{
			this.log = log;
            this.connectionString = connectionString;
		}

        public submitExportsResponse ImportExports(submitExportsRequest se)
        {
            var st = new StatusType();
			string statusMessage = null;
			string statusCode = null;

			try
			{
			    var exportRegisters = xmlToEntity(se.exportInfo);
			    ExportLogic.SubmitExports(exportRegisters, se.exportInfo.measurePointID, se.exportInfo.meterID, se.validFrom, log,
			                              connectionString);
			}
            catch (IccException ie)
            {
                st = StatusType.FAILED;
                statusMessage = ie.Message;
                if (ie.Args != null)
                {
                    foreach (string value in ie.Args)
                    {
                        statusMessage = statusMessage + " " + value;
                    }
                }
                statusCode = ie.Id.ToString();
                log.LogMessage(ie.Id, ie.Args);
            }
            catch (Exception e)
            {
                st = StatusType.FAILED;
                statusMessage = e.Message;
                log.LogMessage(2, new[] { e.Message });//General error message
            }

            var response = new submitExportsResponse(se.messageID, se.messageID, st, statusCode, statusMessage);
            return response;
        }

        #region functions
        private static Register[] xmlToEntity(ExportInfoType ri)
        {
            var registers = new Register[ri.exports.Count];
            int i = 0;

            foreach (ExportDefinitionType register in ri.exports)
            {
                var r = new Register();
                r.receiverID = register.receiverID;
                r.RegisterNumber = register.registerNumber;
                r.CapacityDigits = register.exportCapacityDigits;
                r.CapacityDecimals = register.exportCapacityDecimals;
                r.PulseConstant = register.exportConstant;
                r.exportFormat = register.exportFormat;
                r.exportMethod = register.exportMethod;
                r.exportReference = register.exportReference;
                r.exportUseTrafo = register.exportUseTrafo;
                r.exportUsePulse = register.exportUsePulse;
                r.readingType = ReadingTypeLogic.GetReadingType(register.readingType);
                if (register.mrExportIntervalSpecified)
                    r.mrExportInterval = (int)Enum.Parse(typeof(RegisterData.Frequency), register.mrExportInterval.ToString());
                r.mrExportValuesFrom = register.mrExportValuesFrom;
                r.mrExportStartTime = register.mrExportStartTime;

                registers[i++] = r;
            }
            return registers;
        }
        #endregion functions
    }
}
