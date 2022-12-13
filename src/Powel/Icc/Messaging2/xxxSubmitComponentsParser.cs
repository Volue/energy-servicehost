using System;
using Powel.Icc.Common;
using Powel.Icc.Messaging2.MeteringXML;
using Powel.Icc.Metering2;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Data;
using Powel.Icc.Diagnostics;
using System.IO;
using System.Xml;
using System.Xml.Serialization;


namespace Powel.Icc.Messaging2
{
	/// <summary>
	/// Summary description for SubmitComponentsParser.
	/// </summary>
	public class SubmitComponentsParser
	{
		private EventLogModuleItem log;
	    private string connectionString;

		public SubmitComponentsParser(EventLogModuleItem log,string connectionString)
		{
			this.log = log;
		    this.connectionString = connectionString;
		}

		//public ResponseType ImportComponents(SubmitComponents sc)
		public submitComponentsResponse ImportComponents(submitComponentsRequest sc)
		{
			StatusType st = new StatusType();
			string statusMessage = null;
			string statusCode = null;

			try
			{
				//sc = applyRegisterTemplate(sc);

				/*
				if(DbConfiguration.MeteringService.UseBilling)
					sc = applyBillingRegisterTemplate(sc,"BillingRegisterTemplate.xml");
				*/

				/*
				XmlSerializer s = new XmlSerializer(typeof(SubmitComponents));
				using (TextWriter writer = new StreamWriter(@"c:\temp\test.xml"))
				{
					s.Serialize(writer, sc);
				}
				*/

				Component[] components = xmlToEntity(sc.components);

				ComponentLogic.SubmitComponents(components,sc.components.measurePointID, sc.validFrom,log,connectionString);

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

			submitComponentsResponse response = new submitComponentsResponse(sc.messageID, sc.messageID, st, statusCode, statusMessage);
			return response;

		}

#region functions

		private static Component[] xmlToEntity(ComponentListType c)
		{
			int iLength = c.meters.Count + c.transformers.Count, i = 0;
			if (c.terminal != null) iLength++;

			Component[] components = new Component[iLength];

			// Terminal
			if (c.terminal != null)
			{
				Terminal t = new Terminal();
				t.Id = c.terminal.terminalID;
				t.State = ComponentState.INSTR_STATE_ACTIVE;
				t.MasterName = c.terminal.masterName;
				// add correct Master Key
				ComponentLogic.ResolveMaster(t);

				foreach (ParameterType parameter in c.terminal.parameters) {
					t.AttributesString.Add(parameter.parameterName, parameter.parameterValue);
				}

				components[i] = t;
				i++;
			}

			// Meters
			foreach (MeterType meter in c.meters)
			{
				Meter m = new Meter();
				m.Id = meter.meterID;

				if (!string.IsNullOrEmpty(meter.terminalID)) {
					m.TerminalComponentId = meter.terminalID;
				}

				m.State = ComponentState.INSTR_STATE_ACTIVE;
				m.MasterName = meter.masterName;
				ComponentLogic.ResolveMaster(m);		//add correct Master Key

				foreach (ParameterType parameter in meter.parameters) {
					int num;
					bool isNum;

					if (parameter.parameterName.ToUpper() == "INSTR_PRODUCTION_YEAR") {
						isNum = int.TryParse(parameter.parameterValue, out num);
						if (!isNum) {
							throw new IccException(2177, parameter.parameterName);
						}
						m.AttributesInt.Add(parameter.parameterName, num);
					} else if (parameter.parameterName.ToUpper() == "LOTKEY") {
						isNum = int.TryParse(parameter.parameterValue, out num);
						if (!isNum)	{
							throw new IccException(2177, parameter.parameterName);
						}
						m.AttributesString.Add(parameter.parameterName, num);
					} else if (parameter.parameterName.ToUpper() == "INSTR_NE TOWNER_METERID") {
						isNum = int.TryParse(parameter.parameterValue, out num);
						if (!isNum)	{
							throw new IccException(2177, parameter.parameterName);
						}
						m.AttributesInt.Add(parameter.parameterName, num);
					}	else if (parameter.parameterName.ToUpper() == "FABRNO")	{
						isNum = int.TryParse(parameter.parameterValue, out num);
						if (!isNum)	{
							throw new IccException(2177, parameter.parameterName);
						}
						m.AttributesString.Add(parameter.parameterName, parameter.parameterValue);
					}
				}

				//registers
				/*Register[] registers = new Register[meter.Registers.Count];
					int j = 0;
					foreach (RegisterType register in meter.Registers)
					{
					Register r = new Register();
					r.DirectionOfFlow = (Data.Entities.Metering.FlowDirectionType)Enum.Parse(typeof(Data.Entities.Metering.FlowDirectionType), register.DirectionOfFlow.ToString(), true);
					r.MeasureUnitCode = (Data.Entities.Metering.MeasuringUnitCodeType)Enum.Parse(typeof(Data.Entities.Metering.MeasuringUnitCodeType), register.MeasureUnitCode.ToString(), true);
					r.MeteringFunctionCode = (Data.Entities.Metering.MeteringFunctionCodeType)Enum.Parse(typeof(Data.Entities.Metering.MeteringFunctionCodeType), register.MeteringFunctionCode.ToString(), true);
					r.PulseConstant = register.PulseConstant;
					r.CapacityDecimals = register.RegisterCapacityDecimals;
					r.CapacityDigits = register.RegisterCapacityDigits;
					r.RegisterNumber = register.RegisterNumber;
					r.RemoteMetering = (Data.Entities.Metering.RemoteMeteringType)Enum.Parse(typeof(Data.Entities.Metering.RemoteMeteringType), register.RemoteMetering.ToString(), true);
					r.WayOfRegistration = (Data.Entities.Metering.WayOfRegistrationType)Enum.Parse(typeof(Data.Entities.Metering.WayOfRegistrationType), register.WayOfRegistration.ToString(), true);
					registers[j] = r;
					j++;
					}
					m.Registers = registers;
				*/
				components[i] = m;
				i++;
			}

			//transformers
			foreach (MeteringXML.TransformerType transformer in c.transformers)
			{
				Transformer t = new Transformer();
				t.Id = transformer.transformerID;
				t.TrafoPrimary = transformer.trafoPrimary;
				t.TrafoSecondary = transformer.trafoSecondary;
				t.TrafoType = (Data.Entities.Metering.TransformerType)Enum.Parse(typeof(Data.Entities.Metering.TransformerType), transformer.trafoType.ToString(), true);
				t.State = ComponentState.INSTR_STATE_ACTIVE;
				t.MasterName = transformer.masterName;
				ComponentLogic.ResolveMaster(t);		//add correct Master Key
				
				// Phase
				string tempPhase = transformer.phase.ToString().Trim();
				if (tempPhase.Equals("PHASEA") || tempPhase.Equals("PHASE1")) {
					t.Phase = 1;
				} else if (tempPhase.Equals("PHASEB") || tempPhase.Equals("PHASE2")) {
					t.Phase = 2;
				} else if (tempPhase.Equals("PHASEC") || tempPhase.Equals("PHASE3")) {
					t.Phase = 3;
				}
				
					/*foreach (ParameterType para in transformer.Component.ComponentParameters)
										t.AttributesString.Add(para.ParameterName, para.ParameterValue);
								*/

								components[i] = t;
				i++;
			}

			return components;
		}


	/*	private static submitComponentsRequest applyRegisterTemplate(submitComponentsRequest sc)
		{
			//HttpContext ctx = HttpContext.Current;
			//string templatePath = ctx.Server.MapPath("RegisterTemplates");
				string templatePath = IccConfiguration.IccHome + "\\templates\\";
			string templateNameDefault = getTemplateName(templatePath);

			if (templateNameDefault == string.Empty)
				return sc;

			foreach (MeterType meter in sc.Components.Meters)
			{
				RegisterTypeCollection registers = null;

				if (meter.RegisterTemplateName != null)
				{
					if (meter.RegisterTemplateName == string.Empty)
					{
						return sc;
					}
					string templateName = getTemplateName(templatePath, meter.RegisterTemplateName);

					//if regsiterTemplateName is set and not empty, use submitted template
					if (templateName != string.Empty)
						registers = dsRegisterTypeCollection(templatePath + "\\" + templateName);
					//if registerTemplateName is empty do nothing
				}
				else
				{
					//if registerTemplateName is not set, use default register template
					registers = dsRegisterTypeCollection(templatePath + "\\" + templateNameDefault);
				}

				if (registers != null)
				{
					int i = meter.Registers.Count;
					//add registers from template
					foreach (RegisterType register in registers)
					{
						meter.Registers.Add(
							new RegisterType(
								i + (registers.IndexOf(register)) + 1,
								register.RegisterCapacityDigits,
								register.RegisterCapacityDecimals,
								register.MeasureUnitCode,
								register.DirectionOfFlow,
								register.PulseConstant,
								register.PulseConstantSpecified,
								register.RemoteMetering,
								register.WayOfRegistration,
								register.MeteringFunctionCode
								)
							);
					}
				}
			}

			return sc;
		}

		private static string getTemplateName(string templatePath)
		{
				string templateName = IccConfiguration.Messaging.RegisterTemplate;

				//config settings not set properly
			if (templateName == null)
			{
				throw new IccException(10012); //'RegisterTemplate' configuration key was not set properly in web.config.
			}

			return getTemplateName(templatePath, templateName);
		}

		private static string getTemplateName(string templatePath, string templateName)
		{
			//RegisterTemplate="" --> return.
			if (templateName == String.Empty)
			{
				return templateName;
			}

			FileInfo fi = new FileInfo(templatePath + "\\" + templateName);

			if (!fi.Exists)
			{
				throw new IccException(10013, templateName); //Register template file '%01' was not found.
			}

			return templateName;
		}

		private static RegisterTypeCollection dsRegisterTypeCollection(string strFileName)
		{

			XmlSerializer serializer = new XmlSerializer(typeof(RegisterTemplateType));
			using (XmlTextReader xmlR = new XmlTextReader(strFileName))
			{
				RegisterTemplateType regTemp = (RegisterTemplateType)serializer.Deserialize(xmlR);
				return regTemp.Registers;
			}
		}*/

#endregion
		
	}
}
