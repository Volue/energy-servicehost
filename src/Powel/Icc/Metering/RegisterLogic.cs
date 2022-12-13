using System;
using System.Data;
using System.Collections;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Services.Time;
using Powel.Icc.Data;
using Powel.Icc.Data.Metering;
using ComponentData = Powel.Icc.Data.Metering.ComponentData;
using log4net;

namespace Powel.Icc.Metering
{
	/// <summary>
	/// Summary description for RegisterLogic.
	/// </summary>
	public class RegisterLogic
	{
		private static ILog m_l4n = LogManager.GetLogger(typeof(RegisterLogic));
	
		internal static void SubmitRegisters(Register[] registers, Component relatedComponent, UtcTime timeOfUpdate, IDbConnection connection)
		{
			ArrayList alRegistersOrig = RegisterData.GetForComponent(relatedComponent, timeOfUpdate, connection);
			MergeRegisters(registers, (Register[]) alRegistersOrig.ToArray(typeof(Register)), relatedComponent, timeOfUpdate, connection);
		}

		public static void MergeRegisters(Register[] registersExt, Register[] registersOrig, Component relatedComponent, UtcTime timeOfUpdate, IDbConnection connection)
		{
			//TODO check master system?
			bool bExtFound = false;
			ArrayList alOrigFound = new ArrayList();
			if( registersExt != null)
			{
				foreach( Register regExt in registersExt)	
				{
					bExtFound = false;
					foreach( Register regOrig in registersOrig)
					{
						if( regExt.RegisterNumber == regOrig.RegisterNumber)
						{	//update existing register
							if( regOrig.Merge(regExt))
							{
								RegisterData.Update(regOrig, relatedComponent, timeOfUpdate, connection);
							}
							bExtFound = true;
							alOrigFound.Add(regOrig);
							break;
						}
					}
					if(!bExtFound)
					{
						//add register
						regExt.ValidFromDate = timeOfUpdate;
						regExt.ValidToDate = UtcTime.MaxValue;
						RegisterData.Update(regExt, relatedComponent, timeOfUpdate, connection);
					}
				}
			}

			//disconnect existing that were not part of message
			if( registersOrig != null)
			{
				foreach( Register regOrig in registersOrig)
				{
					if( !alOrigFound.Contains(regOrig))
					{
						FinishRegister(regOrig, timeOfUpdate, connection);
					}
				}
			}
		}

		public static void FinishRegister(Register register, UtcTime timeOfFinish, IDbConnection connection)
		{
			RegisterData.Finish(register, timeOfFinish, connection);
		}

		public static void FinishRegisters(Component component, UtcTime timeOfFinish, IDbConnection connection)
		{
			ArrayList alRegisters = RegisterData.GetForComponent(component, timeOfFinish, connection);
			foreach( Register register in alRegisters)
			{		
				FinishRegister(register, timeOfFinish, connection);
			}
		}

		public static void ActivateRegister(Register register, Meter relatedMeter, UtcTime timeOfActivation, IDbConnection connection)
		{
			m_l4n.Info("ActivateRegister");
			m_l4n.Info("..register = " + register);
			m_l4n.Info("..relatedMeter.Id = " + relatedMeter.Id);
			m_l4n.Info("..timeOfActivation = " + timeOfActivation);

			register.State = RegisterState.ACTIVE;
			RegisterData.Update(register, relatedMeter, timeOfActivation, connection);

			m_l4n.Info("ActivateRegister. Exit");
		}

		public static void InactivateRegister(Register register, Meter relatedMeter, UtcTime timeOfInactivation, IDbConnection connection)
		{
			register.State = RegisterState.INACTIVE;
			RegisterData.Update(register, relatedMeter, timeOfInactivation, connection);
		}

		/// <summary>
		/// Gets the related register of a time series, if found. NULL otherwise.
		/// </summary>
		/// <param name="tsName">Name of the given time series</param>
		/// <param name="measurePoint">The related measure point of the time series</param>
        /// <param name="validAt</param>
		/// <param name="connection">database connection</param>
		/// <returns>Register if found. NULL otherwise.</returns>
		public static Register GetForTimeSeries(string tsName, MeasurePoint measurePoint, UtcTime validAt, IDbConnection connection)
		{
			//TODO Future implementation?
			//May use naming to decide register properties - AgreementLogic.GetRegisterProperties(tsName);
			//May use register properties to find register from RegisterData - RegisterData.GetByProperties(WayOfRegistration..., )
		
			//Now just finds all registers for given measurePoint and regenerates time series names to check for equality
			ArrayList allRegisters = new ArrayList();
			ArrayList components = ComponentData.GetForMeasurePoint(measurePoint, validAt, connection);
			foreach( Component component in components)
			{
				if(component is Meter)
				{
					foreach( Register register in ((Meter)component).Registers)
						allRegisters.Add(register);
				}
			}

			string generatedTsName = "";
			foreach(Register register in allRegisters)
			{
				generatedTsName = AgreementLogic.GetTSName(measurePoint, register);
				if( generatedTsName == tsName)
					return register;
			}
			return null;
		}

		/// <summary>
		/// Method that checks for uniqueness in parameters of registers (from which a ts-name is generated) between registers in a collection.
		/// </summary>
		/// <param name="measurePoint"></param>
		/// <param name="registers"></param>
		public static void CheckUniqueness(MeasurePoint measurePoint, Register[] registers)
		{
            Hashtable productcodes = new Hashtable();

            foreach(Register r in registers)
			{
                Commodity.ProductCodeType productcode = AgreementLogic.GetProductCode(r);
                if(productcode == Commodity.ProductCodeType.Undefined)
                    throw new IccException(string.Format("The attribute combinations for register #{0} are not supported by Metering Services", r.RegisterNumber), 123);
                string sPC = productcode + r.RemoteMetering.ToString();
                if (!productcodes.Contains(sPC))
                    productcodes.Add(sPC, r.RegisterNumber);
                else
                   throw new IccException(10015, measurePoint.Id, r.RegisterNumber.ToString(), ((int)productcodes[productcode + r.RemoteMetering.ToString()]).ToString());
			}
		}
			
	}
}
