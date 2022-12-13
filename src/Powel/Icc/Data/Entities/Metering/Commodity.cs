using System;
using System.Collections.Specialized;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for Commodity.
	/// </summary>
	public class Commodity
	{
		#region Fields

		private BitVector32 fieldEditStatus = new BitVector32(0);

		private static readonly int validFromDateBit = BitVector32.CreateMask();
		private static readonly int validToDateBit = BitVector32.CreateMask(validFromDateBit);
		private static readonly int valueIntervalBit = BitVector32.CreateMask(validToDateBit);
		private static readonly int productCodeBit = BitVector32.CreateMask(valueIntervalBit);

		private UtcTime validFromDate;
		private UtcTime validToDate;
		private Agreement.Frequency valueInterval;
		private CommodityType type;
		private ProductCodeType productCode;

		#endregion

		#region Properties

		public UtcTime ValidFromDate
		{
			get{ return validFromDate; }
			set
			{
				validFromDate = value;
				fieldEditStatus[validFromDateBit] = true;
			}
		}

		public bool ValidFromDateEdited
		{
			get{ return fieldEditStatus[validFromDateBit]; }
		}

		public UtcTime ValidToDate
		{
			get{ return validToDate; }
			set
			{
				validToDate = value;
				fieldEditStatus[validToDateBit] = true;
			}
		}

		public bool ValidToDateEdited
		{
			get{ return fieldEditStatus[validToDateBit]; }
		}

		public Agreement.Frequency ValueInterval
		{
			get{ return valueInterval; }
			set
			{
				valueInterval = value;
				fieldEditStatus[valueIntervalBit] = true;
			}
		}

		public bool ValueIntervalEdited
		{

			get{ return fieldEditStatus[valueIntervalBit];}
		}

		public CommodityType Type
		{
			get { return type; }
			set { type = value; }
		}

		public ProductCodeType ProductCode
		{
			get { return productCode; }
			set
			{
				this.productCode = value;
				fieldEditStatus[productCodeBit] = true;
			}
		}
		public bool ProductCodeEdited
		{
			get { return fieldEditStatus[productCodeBit]; }
		}

		#endregion

		#region Constructors

		public Commodity()
		{
		}

		public Commodity(Commodity commodity)
		{
			this.ValidFromDate = commodity.ValidFromDate;
			this.ValidToDate = commodity.ValidToDate;
			this.ValueInterval = commodity.ValueInterval;
			this.productCode = commodity.productCode;
		}


		public Commodity(
			UtcTime validFromDate,
			UtcTime validToDate,
			Agreement.Frequency valueInterval,
			ProductCodeType productCode
			)
		{
			this.ValidFromDate = validFromDate;
			this.ValidToDate = validToDate;
			this.ValueInterval = valueInterval;
			this.ProductCode = productCode;
		}

		#endregion

		#region Methods

		public bool Merge(Commodity agreement)
		{
			throw new NotImplementedException("Commodity.Merge has not been implemented yet");
		}

		#endregion
				

		public enum CommodityType
		{
			METER_READING,
			CONSUMPTION
		}
		
		public enum ProductCodeType
		{
			Undefined = -1,
			ConsumptionAll = 1,
			MeterReadingAll = 2,
			//Power
            ConsumptionHourActiveOut = 40100,
			ConsumptionHourReactiveOut = 40101,
			ConsumptionHourActiveEffectOut = 40102,
			ConsumptionHourReactiveEffectOut = 40103,
            ConsumptionHourActiveIn = 40104,
            ConsumptionHourReactiveIn = 40105,
            ConsumptionHourActiveEffectIn = 40106,
            ConsumptionHourReactiveEffectIn = 40107,
			MeterReadingLowActiveOut = 40110,
			MeterReadingLowReactiveOut = 40111,
			MeterReadingLowActiveEffectOut = 40112,
			MeterReadingLowReactiveEffectOut = 40113,
			MeterReadingHighActiveOut = 40120,
			MeterReadingHighReactiveOut = 40121,
			MeterReadingHighActiveEffectOut = 40122,
			MeterReadingHighReactiveEffectOut = 40123,
			MeterReadingSingleActiveOut = 40130,
			MeterReadingSingleReactiveOut = 40131,
			MeterReadingSingleActiveEffectOut = 40132,
			MeterReadingSingleReactiveEffectOut = 40133,
            MeterReadingSingleActiveIn = 40134,
            MeterReadingSingleReactiveIn = 40135,
            MeterReadingSingleActiveEffectIn = 40136,
            MeterReadingSingleReactiveEffectIn = 40137,
            //Water
            MeterReadingWaterVolumeOUT = 93150,
            ConsumptionWaterVolumeOUT = 93151,
            //Gas
            ConsumptionGasMinimumPressureOut = 93200,
            ConsumptionGasMaximumPressureOut = 93210,
            ConsumptionGasMinimumTemperatureOut = 93220,
            ConsumptionGasMaximumTemperatureOut = 93230,
            MeterReadingGasMeasuredOut = 93170,
            MeterReadingGasStandardOut = 93180,
            MeterReadingGasEnergyOut = 93160,
            MeterReadingGasControlOut = 93190,
            //District Heating
            ConsumptionHotWaterTemperatureTurnOut = 94000,
            ConsumptionHotWaterTemperatureReturnOut = 94010,
            ConsumptionHotWaterMaximumEnergyOut = 94020,
            ConsumptionHotWaterMaximumVolumeOut = 94030,
            MeterReadingHotWaterEnergyOut = 94040,
            MeterReadingHotWaterVolumeOut = 94050,
            //District Cooling
            ConsumptionCoolWaterTemperatureTurnOut = 94500,
            ConsumptionCoolWaterTemperatureReturnOut = 94510,
            ConsumptionCoolWaterMaximumEnergyOut = 94520,
            ConsumptionCoolWaterMaximumVolumeOut = 94530,
            MeterReadingCoolWaterEnergyOut = 94540,
            MeterReadingCoolWaterVolumeOut = 94550	
	        
		}
	}

	public class MeterReadingCommodity : Commodity
	{
	    public Agreement.Frequency ImportInterval { get; set; }

	    public MeterReadingCommodity()
		{
			this.Type = CommodityType.METER_READING;
		}

		public MeterReadingCommodity(UtcTime validFromDate, UtcTime validToDate, Agreement.Frequency valueInterval, ProductCodeType productCode)
			:base(validFromDate, validToDate, valueInterval, productCode)
		{
			this.Type = CommodityType.METER_READING;
		}
	}

	public class ConsumptionCommodity : Commodity
	{
		private Agreement.Frequency exportInterval;

		public Agreement.Frequency ExportInterval
		{
			get{ return this.exportInterval;}
			set{ this.exportInterval = value;}
		}

		public ConsumptionCommodity()
		{
			this.Type = CommodityType.CONSUMPTION;
		}
		
		public ConsumptionCommodity(UtcTime validFromDate, UtcTime validToDate, Agreement.Frequency valueInterval, Agreement.Frequency exportInterval, ProductCodeType productCode)
			:base(validFromDate, validToDate, valueInterval, productCode)
		{
			this.exportInterval = exportInterval;
			this.Type = CommodityType.CONSUMPTION;
		}
	}
}
