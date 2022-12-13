using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Threading;
using System.Linq;
using Powel.Icc.Services.Time;
using Powel.Icc.Data;
using Powel.Icc.Data.Metering;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.TimeSeries.CustomNaming;
using ComponentData = Powel.Icc.Data.Metering.ComponentData;
using log4net;
using log4net.Config;

namespace Powel.Icc.Metering
{
	/// <summary>
	/// AgreementLogic contains static business methods working on the Agreement entity.
	/// </summary>
	public abstract class AgreementLogic
	{
		private static ILog m_l4n = LogManager.GetLogger(typeof(AgreementLogic));

		/// <summary>
		/// Updates time series, import and export definitions for the measure point noted in the agreement information from of timeOfChange
		/// </summary>
		/// <param name="agreement"></param>
		/// <param name="timeOfChange"></param>
		/// <param name="retryCount"></param>
		/// <param name="retryDelay"></param>
		public static void SubmitAgreement(Agreement agreement, UtcTime timeOfChange, int retryCount, int retryDelay)
		{
			XmlConfigurator.Configure();
			m_l4n.Info("SubmitAgreement");
			m_l4n.Info("..agreement = " + agreement);
			m_l4n.Info("..timeOfChange = " + timeOfChange);
			
			using (IDbConnection connection = Util.OpenConnection())
			{
				m_l4n.Info("..connection.ConnectionString = " + connection.ConnectionString);

				List<MeasurePoint> mps = MeasurePointData.GetByID(agreement.MeasurePointId, agreement.ValidFromDate, connection);
				m_l4n.Info("..mps.Count = " + mps.Count);

				if (mps.Count == 0)
					throw new IccException(10004, agreement.MeasurePointId, timeOfChange.ToString()); //measure point not found at timeOfChange

				m_l4n.Info("..mps[0] = " + mps[0]);

				Hashtable htRegisters = GetRegisters(mps[0], timeOfChange, connection);
				m_l4n.Info("..htRegisters.Count = " + htRegisters.Count);
				foreach (Register register in htRegisters.Keys)
				{
					m_l4n.Info("..register = " + register.ToString());
				}

				List<int> liTSReservedKeys = GetPlaceHolderTSkeys(htRegisters.Count, timeOfChange, retryCount, retryDelay);
				m_l4n.Info("..liTSReservedKeys.Count = " + liTSReservedKeys.Count);
				for (int I = 0; I < liTSReservedKeys.Count; I++)
				{
					m_l4n.InfoFormat("..liTSReservedKeys[{0}] = {1}", I, liTSReservedKeys[I]);
				}

				try
				{
					using (IDbTransaction transaction = Util.OpenTransaction(connection))
					{
						// update measurepoint and installation
						MeasurePoint measurePoint = UpdateInstallation(agreement, timeOfChange, connection);
						m_l4n.Info("..measurePoint = " + measurePoint);

						//Finish active agreement on the measuepoint
						FinishAgreement(measurePoint.Id, timeOfChange, true, connection);

						AgreementData.SaveAgreement(agreement, measurePoint, timeOfChange, connection);

						m_l4n.Info("..agreement.Commodities.Length = " + agreement.Commodities.Length);

						if (agreement.Commodities.Length > 0)
						{
							Transformer[] transformers = TransformerLogic.GetTransformers(
								 measurePoint
								,timeOfChange
								,connection
								);

							List<ConsumptionCommodity> consumptionOrders = GetConsumptionCommodities(agreement);
							m_l4n.Info("..consumptionOrders.Count = " + consumptionOrders.Count);

							List<MeterReadingCommodity> meterReadingCommodities = GetMeterReadingCommodities(agreement);
							m_l4n.Info("..meterReadingCommodities.Count = " + meterReadingCommodities.Count);

							// We need to fetch new registers from DB because the old one might be deactivated by FinishAgreement:
							Hashtable htNewRegisters = GetRegisters(measurePoint, timeOfChange, connection);
							m_l4n.Info("..htNewRegisters.Count = " + htNewRegisters.Count);

							// Check compatibility between registers and commodities:
							if (!CommodityMatch(agreement, htNewRegisters.Keys))
								// No compatible registers found for measurePoint and respective commodities:
								throw new IccException(10007, measurePoint.Id);

							//activate
							//update registers, timeseries and impexp defs
							double importFactor;
							double exportFactor;
							Agreement.Frequency importFrequency;
							Agreement.Frequency exportMethod;
							bool doExport = true;

							foreach (Register register in htNewRegisters.Keys)
							{
								m_l4n.Info("..register = " + register);
								doExport = true;
								UtcTime exportStartTime = new UtcTime(0);

								//Commodity.ProductCodeType productCode = GetProductCode(register);

								if (register.RemoteMetering == RemoteMeteringType.NO)
									doExport = false;

								importFrequency = Agreement.Frequency.NEVER;
								exportMethod = Agreement.Frequency.NEVER;
								importFactor = CalculateFactor(agreement.PulseImport ? register : null,
																							 agreement.TrafoImport ? transformers : null, false);
								exportFactor = CalculateFactor(agreement.PulseExport ? register : null,
																							 agreement.TrafoExport ? transformers : null, true);


								//set exportMethod/importFrequency for register
								if (register.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
								{
									foreach (MeterReadingCommodity mrc in meterReadingCommodities)
									{
										m_l4n.Info("..mrc.ProductCode = " + mrc.ProductCode);
										if (mrc.ProductCode == GetProductCode(register) ||
												mrc.ProductCode == Commodity.ProductCodeType.MeterReadingAll)
										{
											//activate register if necessary
											if (register.State == RegisterState.INACTIVE)
											{
												RegisterLogic.ActivateRegister(
													register
													, (Meter) htNewRegisters[register]
													, timeOfChange
													, connection
													);
											}
											//ToDo:Se litt på logikken her
											if (exportStartTime == UtcTime.Null)
												exportStartTime = mrc.ValidFromDate;
											else if (exportStartTime > mrc.ValidFromDate)
												exportStartTime = mrc.ValidFromDate;

											if (mrc.ValueInterval == Agreement.Frequency.NEVER)
												doExport = false;
										}
									}

									//ToDo: Filter by productCode
									importFrequency = CalculateRequiredImportFrequency(meterReadingCommodities);
									exportMethod = Agreement.Frequency.DAILY;
									//common export method for all accumulative orders
								}
								else
								{
									//CONSUMPTION ORDER
									if (consumptionOrders != null && consumptionOrders.Count > 0)
									{
										foreach (ConsumptionCommodity cc in consumptionOrders)
										{
											m_l4n.Info("..cc.ProductCode = " + cc.ProductCode);
											if (cc.ProductCode == GetProductCode(register) ||
													cc.ProductCode == Commodity.ProductCodeType.ConsumptionAll)
											{
												//activate register if necessary
												if (register.State == RegisterState.INACTIVE)
													RegisterLogic.ActivateRegister(
														 register
														,(Meter)htNewRegisters[register]
														,timeOfChange
														,connection
														);

												exportMethod = cc.ExportInterval; //we only support 1 consumption commodity
												exportStartTime = cc.ValidFromDate;

												if (cc.ValueInterval == Agreement.Frequency.NEVER)
													doExport = false;
											}
											importFrequency = Agreement.Frequency.HOURLY;
											//consumption export orders currently supports only hour interval input
										}
									}
								}

								m_l4n.Info("..register.State = " + register.State);
								if (register.State == RegisterState.ACTIVE)
								{
									//update imp exp defs
									int timsKey = UpdateImpExpDef(
										 measurePoint
										,(Meter)htNewRegisters[register]
										,register
										,importFactor
										,exportFactor
										,doExport
										,importFrequency
										,exportMethod
										,agreement
										,timeOfChange
										,exportStartTime
										,connection
										,ref liTSReservedKeys
										);
									m_l4n.Info("..timsKey (after UpdateImpExpDef) = " + timsKey);

									m_l4n.Info("..register.WayOfRegistration) = " + register.WayOfRegistration);
									//save cyclic accumulative export orders if applicable
									if (register.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
									{
										MeteringOrderData.EndAccumulativeExportOrders(
											new[] { timsKey }
											,timeOfChange
											,connection
											);

										//ToDo: Match productCode
										foreach (MeterReadingCommodity mrc in meterReadingCommodities)
										{
											m_l4n.Info("..mrc.ValidFromDate = " + mrc.ValidFromDate);
											UtcTime dateTime = mrc.ValidFromDate;
											if (agreement.ExportScheduleDateFixed)
											{
												dateTime = GetFirstExportDate(dateTime);
											}

											MeteringOrderLogic.SaveCyclicAccumulativeMeteringOrder(
												timsKey,
												agreement.ReceiverId,
												dateTime, mrc.ValueInterval,
												mrc.ValidFromDate,
												mrc.ValidToDate,
												RegistrationReason.CYCLIC,
												connection
												);
										}
									}
								}
							} //foreach
						}
						transaction.Commit();
					}//transaction
				}
				finally
				{
					foreach (int tsKey in liTSReservedKeys)
					{
						m_l4n.InfoFormat("..Call DeleteByTSKey({0})", tsKey);
						AgreementData.DeleteByTSKey(tsKey, connection);
					}
					m_l4n.Info("SubmitAgreement. Exit");
				}
			}
		}

		/// <summary>
		/// Finishes the agreement, if any, for the given measurePoint from a given point in time
		/// </summary>
		/// <param name="measurePointId"></param>
		/// <param name="timeOfChange"></param>
				/// <param name="keepManualRegActive"></param>
		/// <param name="connection"></param>
		public static void FinishAgreement(string measurePointId, UtcTime timeOfChange,bool keepManualRegActive, IDbConnection connection)
		{
			XmlConfigurator.Configure();
			m_l4n.Info("FinishAgreement");
			m_l4n.Info("..measurePointId = " + measurePointId);
			m_l4n.Info("..timeOfChange = " + timeOfChange);
			m_l4n.Info("..keepManualRegActive = " + keepManualRegActive);

			if (connection == null)
				connection = Util.OpenConnection();
			
			List<MeasurePoint> measurePoints = MeasurePointData.GetByID(measurePointId, timeOfChange, connection);
			m_l4n.Info("..measurePoints.Count = " + measurePoints.Count);

			if (measurePoints.Count == 0)
				throw new IccException(10004, measurePointId, timeOfChange.ToString()); //mp not found at time

			if (measurePoints.Count > 1)
				throw new IccException(10005, measurePointId, timeOfChange.ToString()); //ambigous MP found at time
			MeasurePoint measurePoint = measurePoints[0];
			m_l4n.Info("..After MeasurePointData.GetByID. measurePoint = " + measurePoint);
			
			Hashtable htRegisters = GetRegisters(measurePoint, timeOfChange, connection);
			m_l4n.Info("..htRegisters.Count = " + htRegisters.Count);

			foreach(Register register in htRegisters.Keys)
			{
				m_l4n.Info("..register = " + register);
				if (((register.RemoteMetering == RemoteMeteringType.YES) || (!keepManualRegActive) && (register.RemoteMetering == RemoteMeteringType.NO)))
				{
					FinishImpExpDef(measurePoint, (Meter)htRegisters[register], register, true, timeOfChange, connection);
					if (register.State == RegisterState.ACTIVE)
						RegisterLogic.InactivateRegister(register, (Meter)htRegisters[register], timeOfChange, connection);
				}
				
			}
			
			m_l4n.Info("FinishAgreement. Exit");
		}

		/// <summary>
		/// Gets all registers for a given measure point at a given time, and with a given way of registration, as a Hashtable, where register is KEY, and the related meter is VALUE.
		/// </summary>
		/// <param name="measurePoint">Measure point for which registers are returned</param>
		/// <param name="registrationType">The required way of registration</param>
		/// <param name="validAtTime">Point of time where registers returned must be valid</param>
		/// <param name="connection"></param>
		/// <returns>Hashtable with registers as keys and the related meter as value</returns>
		internal static Hashtable GetRegisters(MeasurePoint measurePoint, WayOfRegistrationType registrationType, UtcTime validAtTime, IDbConnection connection)
		{
			ArrayList registersToRemove = new ArrayList();
			Hashtable registerList = GetRegisters(measurePoint, validAtTime, connection);

			foreach(Register register in registerList.Keys)
			{
				if(register.WayOfRegistration != registrationType || register.RemoteMetering != RemoteMeteringType.YES)
					registersToRemove.Add(register);
			}

			foreach( Register register in registersToRemove)
				registerList.Remove(register);

			return registerList;
		}

		
		/// <summary>
		/// Gets all registers for a given measure point at a given time as a Hashtable, where register is KEY, and the related meter is VALUE.
		/// </summary>
		/// <param name="measurePoint">Measure point for which registers are returned</param>
		/// <param name="validAtTime">Point of time where registers returned must be valid</param>
		/// <param name="connection"></param>
		/// <returns>Hashtable with registers as keys and the related meter as value</returns>
		internal static Hashtable GetRegisters(MeasurePoint measurePoint, UtcTime validAtTime, IDbConnection connection)
		{
			Hashtable registerList = new Hashtable();

			ArrayList components = ComponentData.GetForMeasurePoint(measurePoint, validAtTime, connection);
			foreach(Component component in components)
			{
				if(!(component is Meter))
					continue; //ignore other components than meter
				
				Meter meter = component as Meter;
					
				Register[] registersTemp = (Register[])RegisterData.GetForComponent(meter, validAtTime, connection).ToArray(typeof(Register));

				foreach(Register register in registersTemp)
				{
					registerList.Add(register, meter);
				}
			}
			return registerList;
		}

		/// <summary>
		/// Gets a tims_key for the given combination of measure point and register.
		/// </summary>
		/// <param name="measurePoint"></param>
		/// <param name="register"></param>
		/// <param name="connection"></param>
		/// <returns>int - the given tims_key if found, otherwise 0</returns>
		internal static int GetTimeSeries(MeasurePoint measurePoint, Register register, UtcTime timeOfChange, IDbConnection connection)
		{
			m_l4n.Info("GetTimeSeries");
			m_l4n.Info("..measurePoint = " + measurePoint);
			m_l4n.Info("..register = " + register);
			m_l4n.Info("..timeOfChange = " + timeOfChange);

			int timsKey = 0;
			string tscode = GetTSName(measurePoint, register);
		
			m_l4n.Info("..tscode = " + tscode);

			if( TimeSeriesData.Exists(tscode, connection))
			{
				TimeSeriesData.TimeSeriesInfo tInfo = new TimeSeriesData.TimeSeriesInfo();
				tInfo.tscode = tscode;
				tInfo = TimeSeriesData.Fetch(tInfo, connection);
				if( tInfo != null)
				{
					m_l4n.Info("..tInfo.tims_key = " + tInfo.tims_key);
					m_l4n.Info("..tInfo.Resolution = " + tInfo.Resolution);
					m_l4n.Info("..tInfo.ValueType = " + tInfo.ValueType);
					m_l4n.Info("..tInfo.ValueTypeValidFrom = " + tInfo.ValueTypeValidFrom);

					if 
						(
							tInfo.Resolution == Common.TimeResolution.Hour &&
							register.ValueType == PrimaryValueType.Accumulated &&
							(
								tInfo.ValueType != PrimaryValueType.Accumulated ||
								tInfo.ValueTypeValidFrom == UtcTime.Null ||
								tInfo.ValueTypeValidFrom > timeOfChange
							)
						)
					{
						m_l4n.Info("..Updating ValueType and ValueTypeValidFrom");
						tInfo.ValueType = PrimaryValueType.Accumulated;
						if (tInfo.ValueTypeValidFrom == UtcTime.Null || tInfo.ValueTypeValidFrom > timeOfChange) 
							tInfo.ValueTypeValidFrom = timeOfChange;
						TimeSeriesData.UpdateTsInfo(tInfo);
					}
					timsKey = tInfo.tims_key;
				}
			}

			m_l4n.Info("..timsKey = " + timsKey);

			m_l4n.Info("GetTimeSeries. Exit");

			return timsKey;
		}
		
		/// <summary>
		/// Checks if a time series exists for the combination of the given measure point and register. If not existing, a time series is created.
		/// </summary>
		/// <param name="measurePoint"></param>
		/// <param name="register"></param>
		/// <param name="connection"></param>
		/// <param name="liTSReservedKeys"</param>
		/// <returns>int - tims_key of the existing/created time series</returns>
		private static int UpdateTimeSeries(
			 MeasurePoint measurePoint
			,Register register
			,UtcTime timeOfUpdate
			,IDbConnection connection
			,ref List<int> liTSReservedKeys
			)
		{
			XmlConfigurator.Configure();
			m_l4n.Info("UpdateTimeSeries");
			m_l4n.Info("..measurePoint = " + measurePoint);
			m_l4n.Info("..register = " + register);
			m_l4n.Info("..timeOfUpdate = " + timeOfUpdate);

			int timsKey = GetTimeSeries(measurePoint, register, timeOfUpdate, connection);
			m_l4n.Info("..timsKey(after GetTimeSeries) = " + timsKey);

			if (timsKey == 0)//time series does not exist
			{
				TsType tsType = MapProductCodeTsType(GetProductCode(register));
				string tsName = GetTSName(measurePoint, register);
				timsKey = GetPlaceHolderTSkey(ref liTSReservedKeys);
				m_l4n.Info("..timsKey(after GetPlaceHolderTSkey) = " + timsKey);
				timsKey = AgreementData.CreateTimeSeries(timsKey, tsName, tsType, measurePoint, register, timeOfUpdate, connection);
				m_l4n.Info("..timsKey(after CreateTimeSeries) = " + timsKey);
			}

			m_l4n.Info("UpdateTimeSeries.Exit");
		
			return timsKey;
		}

		/// <summary>
		/// Calculates the import/export factor of an import/export definition, given the register and the transformers of the measure point.
		/// If transformers and/or register (pulse constant) is not supposed to be a part of the factor, these paramaters must be null.
		/// </summary>
		/// <param name="register"></param>
		/// <param name="transformers"></param>
		/// <param name="isExport"></param>
		/// <returns>double - a factor, default 1</returns>
		public static double CalculateFactor(Register register, IEnumerable<Transformer> transformers, bool isExport)
		{
			double factor = 1;
			if( register != null && register.PulseConstant != 0)
			{
								if (isExport)
										factor = 1 / register.PulseConstant;
								else
										factor = register.PulseConstant;			 
			}
				
			if( transformers != null)
			{
				foreach(Transformer t in transformers)
				{
					if(t.State == ComponentState.INSTR_STATE_ACTIVE && t.TrafoSecondary != 0)
						factor *=	((double)t.TrafoPrimary/t.TrafoSecondary);
				}
			}
			return factor;
		}

		public static void FinishImpExpDef(MeasurePoint measurePoint, Meter meter, Register register, bool finishExport, UtcTime timeOfFinish, IDbConnection connection)
		{
			int timsKey = GetTimeSeries(measurePoint, register, timeOfFinish, connection);
			//export reference name
			string expRef = "";
			if (register.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
				expRef = GetName("MDMS.MeteringService.ExportRef.Accumulative", measurePoint, register);
			else if (register.WayOfRegistration == WayOfRegistrationType.DIFFERENTIAL)
				expRef = GetName("MDMS.MeteringService.ExportRef.Differential", measurePoint, register);

			//import reference name
			string impRef = "";
			if (register.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
				impRef = GetName("MDMS.MeteringService.ImportRef.Accumulative", measurePoint, register);
			else if (register.WayOfRegistration == WayOfRegistrationType.DIFFERENTIAL)
				impRef = GetName("MDMS.MeteringService.ImportRef.Differential", measurePoint, register);

			if( timsKey > 0)
				AgreementData.FinishImpExpDef(timsKey, register, string.Empty, finishExport, impRef, expRef, timeOfFinish, connection);
			if(register.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
			{
				//TODO Delete cyclic accumulative metering orders
			}
		}
				
				public static int UpdateImpExpDef(MeasurePoint measurePoint, Meter meter, Register register, double importFactor, double exportFactor, bool createExport, Agreement.Frequency importFrequency, Agreement.Frequency exportMethod, Agreement agreement , UtcTime timeOfChange, UtcTime exportStartTime,	IDbConnection connection)
				{
						List<int> liTSReservedKeys = new List<int>();
						return UpdateImpExpDef(measurePoint, meter, register, importFactor, exportFactor, createExport,
																	 importFrequency, exportMethod, agreement, timeOfChange, exportStartTime, connection,
																	 ref liTSReservedKeys);
				}

		public static int UpdateImpExpDef(
			 MeasurePoint measurePoint
			,Meter meter
			,Register register
			,double importFactor
			,double exportFactor
			,bool createExport
			,Agreement.Frequency importFrequency
			,Agreement.Frequency exportMethod
			,Agreement agreement
			,UtcTime timeOfChange
			,UtcTime exportStartTime
			,IDbConnection connection
			,ref List<int> liTSReservedKeys
			)
		{
			int timsKey = UpdateTimeSeries(measurePoint, register, timeOfChange, connection, ref liTSReservedKeys);

			//export reference name
			string expRef = "";
			if (register.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
				expRef = GetName("MDMS.MeteringService.ExportRef.Accumulative", measurePoint, register);
			else if (register.WayOfRegistration == WayOfRegistrationType.DIFFERENTIAL)
				expRef = GetName("MDMS.MeteringService.ExportRef.Differential", measurePoint, register);

			//import reference name
			string impRef="";
			if (register.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
				impRef = GetName("MDMS.MeteringService.ImportRef.Accumulative", measurePoint, register);
			else if (register.WayOfRegistration == WayOfRegistrationType.DIFFERENTIAL)
				impRef = GetName("MDMS.MeteringService.ImportRef.Differential", measurePoint, register);

			AgreementData.UpdateImpExpDef(timsKey, register, string.Empty, importFactor, exportFactor, createExport, impRef, expRef, importFrequency, exportMethod, agreement, timeOfChange, exportStartTime, connection);
			
			return timsKey;
		}

		private static string GetName(string nameRuleName, MeasurePoint mp, Register reg)
		{
			var mdmsCN = new MDMSCustomName(mp, nameRuleName, reg);

			NameGenerator ng = new NameGenerator();
			return ng.getName(mdmsCN);
		}

		public static string GetTSName( MeasurePoint mp, Register reg)
		{
			string name = "";

			if
				(
							reg.MeteringFunctionCode == MeteringFunctionCodeType.HOUR
					||	reg.MeteringFunctionCode == MeteringFunctionCodeType.SINGLE
					||	reg.MeteringFunctionCode == MeteringFunctionCodeType.LOW
					||	reg.MeteringFunctionCode == MeteringFunctionCodeType.HIGH
				)
			{
					if (reg.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
							name = GetName("MDMS.MeteringServicePOWER.Accumulative", mp, reg);
					else if (reg.WayOfRegistration == WayOfRegistrationType.DIFFERENTIAL)
							name = GetName("MDMS.MeteringServicePOWER.Differential", mp, reg);
			}
			else 
			{
					if (reg.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
							name = GetName("MDMS.MeteringService.Accumulative", mp, reg);
					else if (reg.WayOfRegistration == WayOfRegistrationType.DIFFERENTIAL)
							name = GetName("MDMS.MeteringService.Differential", mp, reg);
					
			}

			return name;
		}

		private static List<ConsumptionCommodity> GetConsumptionCommodities(Agreement agreement)
		{
			List<ConsumptionCommodity> commodities = new List<ConsumptionCommodity>();
			foreach (Commodity commodity in agreement.Commodities)
			{
				if (commodity.Type == Commodity.CommodityType.CONSUMPTION)
				{
					commodities.Add((ConsumptionCommodity)commodity);
				}
			}
			return commodities;
		}

		private static List<MeterReadingCommodity> GetMeterReadingCommodities(Agreement agreement)
		{
			List<MeterReadingCommodity> commodities = new List<MeterReadingCommodity>();
			foreach (Commodity commodity in agreement.Commodities)
			{
				if (commodity.Type == Commodity.CommodityType.METER_READING)
				{
					commodities.Add((MeterReadingCommodity)commodity);
				}
			}
			return commodities;
		}

		private static Agreement.Frequency CalculateRequiredImportFrequency(IEnumerable<MeterReadingCommodity> commodities)
		{
			//Agreement.Frequency frequency = Agreement.Frequency.NEVER;
			Agreement.Frequency frequency = Agreement.Frequency.YEARLY;
			// find minimum. Is there no min with accessor adaptor, that would be more clear.
			return commodities.Aggregate(frequency, 
			                             (curr, next) => next.ImportInterval > 0 && next.ImportInterval < curr ? next.ImportInterval : curr);
			/*
			foreach (MeterReadingCommodity commodity in commodities)
			{
				Agreement.Frequency tempFrequency;

				if (commodity.ImportInterval != null)
					tempFrequency = commodity.ImportInterval;
				else
					tempFrequency = commodity.ValueInterval;

				if (tempFrequency > 0 && tempFrequency < frequency)
					frequency = tempFrequency;
			}
			return frequency;
			*/
		}

		//iterates through a list of registers and tries to match it with a productcode
		private static bool RegistersMatchProductCode(ICollection registers, Commodity.ProductCodeType productCode)
		{
			foreach (Register register in registers)
			{
				if (productCode == GetProductCode(register))
					return true;
			}
			return false;
		}

		//Used to determine if a register matches a given productCode.
		public static Commodity.ProductCodeType GetProductCode(Register reg)
		{
				//Interval/Differential/Volume
				switch (reg.WayOfRegistration)
				{
						case WayOfRegistrationType.DIFFERENTIAL:
								switch (reg.MeteringFunctionCode)
								{
										case MeteringFunctionCodeType.HOUR:
												switch (reg.DirectionOfFlow)
												{
														case FlowDirectionType.OUT:
																if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
																{ return Commodity.ProductCodeType.ConsumptionHourActiveOut; }//40100
																if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH)
																{ return Commodity.ProductCodeType.ConsumptionHourReactiveOut; }//40101 
																if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH_H)
																{ return Commodity.ProductCodeType.ConsumptionHourActiveEffectOut; }//40102 
																if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH_H)
																{ return Commodity.ProductCodeType.ConsumptionHourReactiveEffectOut; }//40103
																break;
														case FlowDirectionType.IN:
																if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
																{ return Commodity.ProductCodeType.ConsumptionHourActiveIn; }//40104 
																if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH)
																{ return Commodity.ProductCodeType.ConsumptionHourReactiveIn; }//40105 
																if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH_H)
																{ return Commodity.ProductCodeType.ConsumptionHourActiveEffectIn; }//40106 
																if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH_H)
																{ return Commodity.ProductCodeType.ConsumptionHourReactiveEffectIn; }//40107
																break;
												}
												break;
										case MeteringFunctionCodeType.WATERVOLUME:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.M3)
														{ return Commodity.ProductCodeType.ConsumptionWaterVolumeOUT; }//93150
												break;
										case MeteringFunctionCodeType.GASMINPRESS:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.BAR_A)
														{ return Commodity.ProductCodeType.ConsumptionGasMinimumPressureOut; }//93200
												}
												break;
										case MeteringFunctionCodeType.GASMAXPRESS:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.BAR_A)
														{ return Commodity.ProductCodeType.ConsumptionGasMaximumPressureOut; }//93210
												}
												break;
										case MeteringFunctionCodeType.GASMINTEMP:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.DEGREES_C)
														{ return Commodity.ProductCodeType.ConsumptionGasMinimumTemperatureOut; }//93220
												}
												break;
										case MeteringFunctionCodeType.GASMAXTEMP:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.DEGREES_C)
														{ return Commodity.ProductCodeType.ConsumptionGasMaximumTemperatureOut; }//93230
												}
												break;
										case MeteringFunctionCodeType.HOTWATERTEMPTURN:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.DEGREES_C)
														{ return Commodity.ProductCodeType.ConsumptionHotWaterTemperatureTurnOut; }//94000
												}
												break;
										case MeteringFunctionCodeType.HOTWATERTEMPRETURN:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.DEGREES_C)
														{ return Commodity.ProductCodeType.ConsumptionHotWaterTemperatureReturnOut; }//94010
												}
												break;
										case MeteringFunctionCodeType.HOTWATERMAXENERGY:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
														{ return Commodity.ProductCodeType.ConsumptionHotWaterMaximumEnergyOut; }//94020
												}
												break;
										case MeteringFunctionCodeType.HOTWATERMAXVOLUME:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.M3)
														{ return Commodity.ProductCodeType.ConsumptionHotWaterMaximumVolumeOut; }//94030
												}
												break;
										case MeteringFunctionCodeType.COOLWATERTEMPTURN:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.DEGREES_C)
														{ return Commodity.ProductCodeType.ConsumptionCoolWaterTemperatureTurnOut; }//94500
												}
												break;
										case MeteringFunctionCodeType.COOLWATERTEMPRETURN:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.DEGREES_C)
														{ return Commodity.ProductCodeType.ConsumptionCoolWaterTemperatureReturnOut; }//94510
												}
												break;
										case MeteringFunctionCodeType.COOLWATERMAXENERGY:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
														{ return Commodity.ProductCodeType.ConsumptionCoolWaterMaximumEnergyOut; }//94520
												}
												break;
										case MeteringFunctionCodeType.COOLWATERMAXVOLUME:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.M3)
														{ return Commodity.ProductCodeType.ConsumptionCoolWaterMaximumVolumeOut; }//94530
												}
												break;
								}
								break;
						case WayOfRegistrationType.ACCUMULATIVE:
								switch (reg.MeteringFunctionCode)
								{
										case MeteringFunctionCodeType.LOW:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
														{
																return Commodity.ProductCodeType.MeterReadingLowActiveOut;
														} //40110
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH)
														{
																return Commodity.ProductCodeType.MeterReadingLowReactiveOut;
														} //40111
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH_H)
														{
																return Commodity.ProductCodeType.MeterReadingLowActiveEffectOut;
														} //40112
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH_H)
														{
																return Commodity.ProductCodeType.MeterReadingLowReactiveEffectOut;
														} //40113
												}
												break;
										case MeteringFunctionCodeType.HIGH:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
														{
																return Commodity.ProductCodeType.MeterReadingHighActiveOut;
														} //40120
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH)
														{
																return Commodity.ProductCodeType.MeterReadingHighReactiveOut;
														} //40121
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH_H)
														{
																return Commodity.ProductCodeType.MeterReadingHighActiveEffectOut;
														} //40122
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH_H)
														{
																return Commodity.ProductCodeType.MeterReadingHighReactiveEffectOut;
														} //40123
												}
												break;
										case MeteringFunctionCodeType.SINGLE:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
														{
																return Commodity.ProductCodeType.MeterReadingSingleActiveOut;
														} //40130
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH)
														{
																return Commodity.ProductCodeType.MeterReadingSingleReactiveOut;
														} //40131
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH_H)
														{
																return Commodity.ProductCodeType.MeterReadingSingleActiveEffectOut;
														} //40132
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH_H)
														{
																return Commodity.ProductCodeType.MeterReadingSingleReactiveEffectOut;
														} //40133
												}
												if (reg.DirectionOfFlow == FlowDirectionType.IN)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
														{
																return Commodity.ProductCodeType.MeterReadingSingleActiveIn;
														} //40134
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH)
														{
																return Commodity.ProductCodeType.MeterReadingSingleReactiveIn;
														} //40135
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH_H)
														{
																return Commodity.ProductCodeType.MeterReadingSingleActiveEffectIn;
														} //40136
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KVARH_H)
														{
																return Commodity.ProductCodeType.MeterReadingSingleReactiveEffectIn;
														} //40137
												}
												break;
										case MeteringFunctionCodeType.WATERVOLUME:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.M3)
														{
																return Commodity.ProductCodeType.MeterReadingWaterVolumeOUT;
														} //93151
												break;
										case MeteringFunctionCodeType.GASMEASURED:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.M3)
														{
																return Commodity.ProductCodeType.MeterReadingGasMeasuredOut;
														} //93170
												}
												break;
										case MeteringFunctionCodeType.GASSTANDARD:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.NM3)
														{
																return Commodity.ProductCodeType.MeterReadingGasStandardOut;
														} //93180
												}
												break;
										case MeteringFunctionCodeType.GASENERGY:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
														{
																return Commodity.ProductCodeType.MeterReadingGasEnergyOut;
														} //93160
												}
												break;
										case MeteringFunctionCodeType.GASCONTROL:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.M3)
														{
																return Commodity.ProductCodeType.MeterReadingGasControlOut;
														} //93190
												}
												break;
										case MeteringFunctionCodeType.HOTWATERENERGY:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
														{
																return Commodity.ProductCodeType.MeterReadingHotWaterEnergyOut;
														} //94040
												}
												break;
										case MeteringFunctionCodeType.HOTWATERVOLUME:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.M3)
														{
																return Commodity.ProductCodeType.MeterReadingHotWaterVolumeOut;
														} //94050
												}
												break;
										case MeteringFunctionCodeType.COOLWATERENERGY:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.KWH)
														{
																return Commodity.ProductCodeType.MeterReadingCoolWaterEnergyOut;
														} //94540
												}
												break;
										case MeteringFunctionCodeType.COOLWATERVOLUME:
												if (reg.DirectionOfFlow == FlowDirectionType.OUT)
												{
														if (reg.MeasureUnitCode == MeasuringUnitCodeType.M3)
														{
																return Commodity.ProductCodeType.MeterReadingCoolWaterVolumeOut;
														} //94550
												}
												break;
								}
								break;
				}

						return Commodity.ProductCodeType.Undefined;
		}

			//mapping function for productcodes and TsType
		//TsType = TSER_KEY.Tp_Tseri_tp	
		//MeteringFunctionCodeType is a subset of this
		public static TsType MapProductCodeTsType(Commodity.ProductCodeType productCode)
		{
			switch (productCode)
			{
				case Commodity.ProductCodeType.ConsumptionAll:
				case Commodity.ProductCodeType.ConsumptionHourActiveOut:
				case Commodity.ProductCodeType.ConsumptionHourReactiveOut:
				case Commodity.ProductCodeType.ConsumptionHourActiveEffectOut:
				case Commodity.ProductCodeType.ConsumptionHourReactiveEffectOut:
								case Commodity.ProductCodeType.ConsumptionHourActiveIn:
								case Commodity.ProductCodeType.ConsumptionHourReactiveIn:
								case Commodity.ProductCodeType.ConsumptionHourActiveEffectIn:
								case Commodity.ProductCodeType.ConsumptionHourReactiveEffectIn:				 
										return TsType.ConsumpUns;//4010
								case Commodity.ProductCodeType.ConsumptionWaterVolumeOUT:
								case Commodity.ProductCodeType.MeterReadingWaterVolumeOUT:
										return TsType.WaterVol;//9315								
								case Commodity.ProductCodeType.ConsumptionGasMinimumPressureOut:
										return TsType.GasMinPress;//9320
								case Commodity.ProductCodeType.ConsumptionGasMaximumPressureOut:
										return TsType.GasMaxPress;//9321
								case Commodity.ProductCodeType.ConsumptionGasMinimumTemperatureOut:
										return TsType.GasMinTemp;//9323
								case Commodity.ProductCodeType.ConsumptionGasMaximumTemperatureOut:
										return TsType.GasMaxTemp;//9323
								case Commodity.ProductCodeType.ConsumptionHotWaterTemperatureTurnOut:
										return TsType.HotWaterTT;//9400
								case Commodity.ProductCodeType.ConsumptionHotWaterTemperatureReturnOut:
										return TsType.HotWaterTR;//9401
								case Commodity.ProductCodeType.ConsumptionHotWaterMaximumEnergyOut:
										return TsType.HotWaterME;//9402
								case Commodity.ProductCodeType.ConsumptionHotWaterMaximumVolumeOut:
										return TsType.HotWaterMV;//9403
								case Commodity.ProductCodeType.ConsumptionCoolWaterTemperatureTurnOut:
										return TsType.CoolWaterTT;//9450
								case Commodity.ProductCodeType.ConsumptionCoolWaterTemperatureReturnOut:
										return TsType.CoolWaterTR;//9451
								case Commodity.ProductCodeType.ConsumptionCoolWaterMaximumEnergyOut:
										return TsType.CoolWaterME;//9452
								case Commodity.ProductCodeType.ConsumptionCoolWaterMaximumVolumeOut:
										return TsType.CoolWaterMV;//9453
				case Commodity.ProductCodeType.MeterReadingLowActiveOut:
				case Commodity.ProductCodeType.MeterReadingLowReactiveOut:
				case Commodity.ProductCodeType.MeterReadingLowActiveEffectOut:
				case Commodity.ProductCodeType.MeterReadingLowReactiveEffectOut:
					return TsType.ConsumpLow;//4011
				case Commodity.ProductCodeType.MeterReadingHighActiveOut:
				case Commodity.ProductCodeType.MeterReadingHighReactiveOut:
				case Commodity.ProductCodeType.MeterReadingHighActiveEffectOut:
				case Commodity.ProductCodeType.MeterReadingHighReactiveEffectOut:
					return TsType.ConsumpHgh;//4012
				case Commodity.ProductCodeType.MeterReadingAll:
				case Commodity.ProductCodeType.MeterReadingSingleActiveOut:
				case Commodity.ProductCodeType.MeterReadingSingleReactiveOut:
				case Commodity.ProductCodeType.MeterReadingSingleActiveEffectOut:
				case Commodity.ProductCodeType.MeterReadingSingleReactiveEffectOut:
								case Commodity.ProductCodeType.MeterReadingSingleActiveIn:
								case Commodity.ProductCodeType.MeterReadingSingleReactiveIn:
								case Commodity.ProductCodeType.MeterReadingSingleActiveEffectIn:
								case Commodity.ProductCodeType.MeterReadingSingleReactiveEffectIn:
					return TsType.ConsumpMtr;//4013
								case Commodity.ProductCodeType.MeterReadingGasMeasuredOut:
										return TsType.GasVolume;//9317
								case Commodity.ProductCodeType.MeterReadingGasStandardOut:
										return TsType.GasStdVol;//9318
								case Commodity.ProductCodeType.MeterReadingGasEnergyOut:
										return TsType.GasEnergy;//9316
								case Commodity.ProductCodeType.MeterReadingGasControlOut:
										return TsType.GasCntrVol;//9319
								case Commodity.ProductCodeType.MeterReadingHotWaterEnergyOut:
										return TsType.HotWaterEn;//9403
								case Commodity.ProductCodeType.MeterReadingHotWaterVolumeOut:
										return TsType.HotWaterVo;//9404
								case Commodity.ProductCodeType.MeterReadingCoolWaterEnergyOut:
										return TsType.CoolWaterEn;//9453
								case Commodity.ProductCodeType.MeterReadingCoolWaterVolumeOut:
										return TsType.CoolWaterVo;//9454
				default:
					return TsType.Undefined;
			}
		}

		//checks to see if the register match the productcode of the commodities in the agreement
		public static bool CommodityMatch(Agreement agreement, ICollection registers)
		{
						List<bool> commodityMatch = new List<bool>();
						foreach (Commodity commodity in agreement.Commodities)
						{
								switch (commodity.ProductCode)
								{
										case Commodity.ProductCodeType.ConsumptionAll:
												foreach (Register reg in registers)
												{
														if (reg.WayOfRegistration == WayOfRegistrationType.DIFFERENTIAL)
														{
																commodityMatch.Add(true);
																break;
														}

												}
												break;
										case Commodity.ProductCodeType.MeterReadingAll:
												foreach (Register reg in registers)
												{
														if (reg.WayOfRegistration == WayOfRegistrationType.ACCUMULATIVE)
														{
																commodityMatch.Add(true);
																break;
														}
												}
												break;
										default:
												if (RegistersMatchProductCode(registers, commodity.ProductCode))
												{
														commodityMatch.Add(true);
														break;
												}
												break;
								}
						}
						if(commodityMatch.Count==agreement.Commodities.Length)
								return true;
					 
						return false;
		}


		//checks if measure point exists
		//updates supplier if it has changed
		//sets measurepoint type (note:this will not reset if the agreement is canceled)
		private static MeasurePoint UpdateInstallation(Agreement agreement, UtcTime timeOfChange, IDbConnection connection)
		{
			//checks measure point information
			List<MeasurePoint> measurePoints = MeasurePointData.GetByID(agreement.MeasurePointId, timeOfChange, connection);
			if (measurePoints.Count == 0)
				throw new IccException(10004, agreement.MeasurePointId, timeOfChange.ToString()); //measure point not found at timeOfChange
			if (measurePoints.Count > 1)
				throw new IccException(10005, agreement.MeasurePointId, timeOfChange.ToString()); //ambiguous measure points found
			MeasurePoint measurePoint = measurePoints[0];
						
			Installation installation = MeasurePointData.GetInstallation(measurePoint, agreement.ValidFromDate, connection);

				MeasurePoint.DebtorTypes oldDT = measurePoint.DebtorType;

						if (DbConfiguration.MeteringService.ClearSupplierWhenNoCommidities && agreement.Commodities.Length == 0)
								agreement.SupplierId = string.Empty;

			//set supplier, if changed
			if (agreement.SupplierIdEdited && measurePoint.Supplier != agreement.SupplierId)
			{
								//if(measurePoint.Supplier != string.Empty)//before change
								if (measurePoint.ValidFromDate < agreement.ValidFromDate)//post change
						installation = InstallationData.CreateNewVersion(installation, agreement.ValidFromDate, connection);

				measurePoint.Supplier = agreement.SupplierId;
				//note: This has faulty logic. Supplier is by Installation.. not by measurepoint.
				//Thus if you change one supplier for 1 MP.. then you change it for all   in that installation!
			}

			//set debtor type (if one or more cmmodities is consumption, measure point is hourly)
			bool isHourly = false;
			foreach (Commodity commodity in agreement.Commodities)
			{
				if (commodity.Type == Commodity.CommodityType.CONSUMPTION) isHourly = true;
			}
						
						if (isHourly)
				measurePoint.DebtorType = MeasurePoint.DebtorTypes.Hourly;
			else
				measurePoint.DebtorType = MeasurePoint.DebtorTypes.MeterReadings;

						if (agreement.Commodities.Length == 0)
								measurePoint.DebtorType = oldDT;

						if (measurePoint.DebtorType != oldDT)//post change
								MeasurePointData.CreateNewVersion(measurePoint, installation, agreement.ValidFromDate, connection);//post change

			//save updated fields
			MeasurePointData.Update(measurePoint, installation, measurePoint.CentralNetPointCode, connection);

			return measurePoint;
		}

		public static UtcTime GetFirstExportDate(UtcTime date)
		{
			System.TimeSpan offset =	System.TimeZone.CurrentTimeZone.GetUtcOffset(System.DateTime.Now);

			RegionalCalendar rc = new RegionalCalendar();
			date = rc.AddMonths(date, 1);
			date = rc.trimMonth(date);
			date = rc.AddHours(date, - offset.Hours );
			
			return date;
		}
		
		internal static int GetPlaceHolderTSkey(ref List<int> placeholderTSkeys)
		{
			int tsKey = 0;
			if(placeholderTSkeys.Count>0)
			{
				tsKey = placeholderTSkeys[0];
				placeholderTSkeys.RemoveAt(0);
			}

			return tsKey;
		}

        /// Added protected internal because tests are now external in Powel.Icc.Test
        protected internal static List<int> GetPlaceHolderTSkeys(int placeholderCount, UtcTime timeOfChange, int retryCount, int retryDelay)
		{
			List<int> placeholders = new List<int>();

			if(placeholderCount>0)
			{
				for (int i = 1; i <= placeholderCount;i++)
				{
					placeholders.Add(FetchNewKey(string.Format("PH_{0}", Guid.NewGuid()),timeOfChange,retryCount,retryDelay));
				}
			}
			return placeholders;
		}

		public static int FetchNewKey(string tsName, UtcTime validFrom, int retryCount, int retryDelay)
		{
			int newKey = -1;
			using (IDbConnection connection = Util.OpenConnection())
			{
				for (int i = 1; i <= retryCount; i++)
				{
					try
					{
						newKey = AgreementData.FetchNewTSKey(tsName, validFrom, connection);
						return newKey;
					}
					catch (IccException ie)
					{
						if (ie.Id == 1012) //generic ICC error message
						{
							if (ie.Message.Contains("ORA-00001")) //unique constraint
							{
							Random random = new Random();
							Thread.Sleep(random.Next(retryDelay));
							//wait then retry
							}
						}
						else
						{
							throw ie;
						}
					}
				}
			}
			return newKey;
		}
	}
}
