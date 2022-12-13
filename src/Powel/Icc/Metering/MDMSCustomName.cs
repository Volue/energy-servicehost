using System;
using System.Collections.Generic;
using System.Text;
using Powel.Icc.TimeSeries.CustomNaming;
using Powel.Icc.Data.Entities.Metering;

namespace Powel.Icc.Metering
{
	public class MDMSCustomName : CustomName
	{

		private MeasurePoint measurePoint;
		private Register register;
		private TimeSeriesDefinition TSDef;
		
		//constructor
		public MDMSCustomName(MeasurePoint mp, string rootElement, Register reg)
		{	
			register = reg;
			initMDMSCN(mp, rootElement);
		}

		public MDMSCustomName(MeasurePoint mp , string rootElement, TimeSeriesDefinition tsDef)
		{
			TSDef = tsDef;
			initMDMSCN(mp, rootElement);
		}

		public void initMDMSCN(MeasurePoint mp , string rootElement)
		{
			measurePoint = mp;
			rootName = rootElement;
			initCnElements();
		}

		public override string getValue(string nameElementName)
		{
			string elValue = "";

			if (cnElementValues.ContainsKey(nameElementName))
				cnElementValues.TryGetValue(nameElementName,out elValue);

			return elValue;
		}


		private void initCnElements()
		{
			if(register != null)
			{
				//measurementUnit
				switch (register.MeasureUnitCode)
				{
					case MeasuringUnitCodeType.KWH:
						cnElementValues.Add("MeasureUnit", "KWH");
						break;
					case MeasuringUnitCodeType.KWH_H:
						cnElementValues.Add("MeasureUnit", "KWH_H");
						break;
					case MeasuringUnitCodeType.KVARH:
						cnElementValues.Add("MeasureUnit", "KVARH");
						break;
					case MeasuringUnitCodeType.KVARH_H:
						cnElementValues.Add("MeasureUnit", "KVARH_H");
						break;
					case MeasuringUnitCodeType.L_H:
						cnElementValues.Add("MeasureUnit", "L_H");
						break;
					case MeasuringUnitCodeType.M3:
						cnElementValues.Add("MeasureUnit", "M3");
						break;
					case MeasuringUnitCodeType.M3_H:
						cnElementValues.Add("MeasureUnit", "M3_H");
						break;
                    case MeasuringUnitCodeType.BAR_A:
                        cnElementValues.Add("MeasureUnit", "BAR_A");
                        break;
                    case MeasuringUnitCodeType.NM3:
                        cnElementValues.Add("MeasureUnit", "NM3");
                        break;
                    case MeasuringUnitCodeType.DEGREES_C:
                        cnElementValues.Add("MeasureUnit", "M3_H");
                        break;
				}
				
				//flowdirection
				switch (register.DirectionOfFlow)
				{
					case FlowDirectionType.IN:
						cnElementValues.Add("FlowDirection", "IN");
						break;
					case FlowDirectionType.OUT:
						cnElementValues.Add("FlowDirection", "OUT");
						break;
					case FlowDirectionType.IN_IN:
						cnElementValues.Add("FlowDirection", "IN_IN");
						break;
					case FlowDirectionType.IN_OUT:
						cnElementValues.Add("FlowDirection", "IN_OUT");
						break;
					case FlowDirectionType.OUT_IN:
						cnElementValues.Add("FlowDirection", "OUT_IN");
						break;
					case FlowDirectionType.OUT_OUT:
						cnElementValues.Add("FlowDirection", "OUT_OUT");
						break;
					
				}

				//remotemetering
				switch(register.RemoteMetering)
				{
					case RemoteMeteringType.YES:
						cnElementValues.Add("RemoteMetering", "YES");
						break;
					case RemoteMeteringType.NO:
						cnElementValues.Add("RemoteMetering", "NO");
						break;
				}
				
				//meteringFunctionCode
                Array aryMeteringFunctionCodeAll = System.Enum.GetValues(typeof(MeteringFunctionCodeType));
               
                for (int i = 0; i < aryMeteringFunctionCodeAll.Length; i++)
                {
                    if (register.MeteringFunctionCode == (MeteringFunctionCodeType)aryMeteringFunctionCodeAll.GetValue(i))
                    {
                        cnElementValues.Add("MeteringFunction", aryMeteringFunctionCodeAll.GetValue(i).ToString());
                        break;
                    }
                        
                }

                //TODO:Remove switch statements

                /*
				switch(register.MeteringFunctionCode)
				{
					case MeteringFunctionCodeType.UNDEFINED:
						cnElementValues.Add("MeteringFunction", "UNDEFINED");
						break;
					case MeteringFunctionCodeType.LOW:
						cnElementValues.Add("MeteringFunction", "LOW");
						break;
					case MeteringFunctionCodeType.HIGH:
						cnElementValues.Add("MeteringFunction", "HIGH");
						break;
					case MeteringFunctionCodeType.SINGLE:
						cnElementValues.Add("MeteringFunction", "SINGLE");
						break;
					case MeteringFunctionCodeType.HOUR:
						cnElementValues.Add("MeteringFunction", "HOUR");
						break;

				}
                 * */

				//properties
				cnElementValues.Add("MeterID", register.MeterID);
				cnElementValues.Add("RegisterNumber", register.RegisterNumber.ToString());
			}

			//Elements for TimeSeries Definition
			else if (TSDef != null)
			{
				//measurementUnit
				switch (TSDef.MeasureUnitCode)
				{
					case MeasuringUnitCodeType.KWH:
						cnElementValues.Add("MeasureUnit", "KWH");
						break;
					case MeasuringUnitCodeType.KWH_H:
						cnElementValues.Add("MeasureUnit", "KWH_H");
						break;
					case MeasuringUnitCodeType.KVARH:
						cnElementValues.Add("MeasureUnit", "KVARH");
						break;
					case MeasuringUnitCodeType.KVARH_H:
						cnElementValues.Add("MeasureUnit", "KVARH_H");
						break;
					case MeasuringUnitCodeType.L_H:
						cnElementValues.Add("MeasureUnit", "L_H");
						break;
					case MeasuringUnitCodeType.M3:
						cnElementValues.Add("MeasureUnit", "M3");
						break;
					case MeasuringUnitCodeType.M3_H:
						cnElementValues.Add("MeasureUnit", "M3_H");
						break;
                    case MeasuringUnitCodeType.BAR_A:
                        cnElementValues.Add("MeasureUnit", "BAR_A");
                        break;
                    case MeasuringUnitCodeType.NM3:
                        cnElementValues.Add("MeasureUnit", "NM3");
                        break;
                    case MeasuringUnitCodeType.DEGREES_C:
                        cnElementValues.Add("MeasureUnit", "M3_H");
                        break;
				}

				//flowdirection
				switch (TSDef.DirectionOfFlow)
				{
					case FlowDirectionType.IN:
						cnElementValues.Add("FlowDirection", "IN");
						break;
					case FlowDirectionType.OUT:
						cnElementValues.Add("FlowDirection", "OUT");
						break;
					case FlowDirectionType.IN_IN:
						cnElementValues.Add("FlowDirection", "IN_IN");
						break;
					case FlowDirectionType.IN_OUT:
						cnElementValues.Add("FlowDirection", "IN_OUT");
						break;
					case FlowDirectionType.OUT_IN:
						cnElementValues.Add("FlowDirection", "OUT_IN");
						break;
					case FlowDirectionType.OUT_OUT:
						cnElementValues.Add("FlowDirection", "OUT_OUT");
						break;

				}

				//remotemetering
				switch (TSDef.RemoteMetering)
				{
					case RemoteMeteringType.YES:
						cnElementValues.Add("RemoteMetering", "YES");
						break;
					case RemoteMeteringType.NO:
						cnElementValues.Add("RemoteMetering", "NO");
						break;
				}

				//meteringFunctionCode
				switch (TSDef.MeteringFunctionCode)
				{
					case MeteringFunctionCodeType.UNDEFINED:
						cnElementValues.Add("MeteringFunction", "UNDEFINED");
						break;
					case MeteringFunctionCodeType.LOW:
						cnElementValues.Add("MeteringFunction", "LOW");
						break;
					case MeteringFunctionCodeType.HIGH:
						cnElementValues.Add("MeteringFunction", "HIGH");
						break;
					case MeteringFunctionCodeType.SINGLE:
						cnElementValues.Add("MeteringFunction", "SINGLE");
						break;
					case MeteringFunctionCodeType.HOUR:
						cnElementValues.Add("MeteringFunction", "HOUR");
						break;
					case MeteringFunctionCodeType.BILLING:
						cnElementValues.Add("MeteringFunction", "BILLING");
						break;

				}
			}
			

			//measurePointID
			cnElementValues.Add("MeasurePointID", measurePoint.Id.ToString());		

		}
	}
}
