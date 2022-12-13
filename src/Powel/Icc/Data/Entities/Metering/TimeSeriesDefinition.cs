using System;
using System.Collections.Specialized;
using Powel.Icc.Services.Time;


namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for TimeseriesDefinition.
	/// </summary>
	public class TimeSeriesDefinition
	{
		#region Fields
		
		protected BitVector32 fieldEditStatus = new BitVector32(0);

		private static readonly int wayOfRegistrationBit = BitVector32.CreateMask();
		private static readonly int measureUnitCodeBit = BitVector32.CreateMask(wayOfRegistrationBit);
		private static readonly int directionOfFlowBit = BitVector32.CreateMask(measureUnitCodeBit);
		private static readonly int remoteMeteringBit = BitVector32.CreateMask(directionOfFlowBit);
		private static readonly int meteringFunctionCodeBit = BitVector32.CreateMask(remoteMeteringBit);
        private static readonly int valueTypeBit = BitVector32.CreateMask(meteringFunctionCodeBit);

		private int timeSeriesKey;
		private WayOfRegistrationType wayOfRegistration;
		private MeasuringUnitCodeType measureUnitCode;
		private FlowDirectionType directionOfFlow;
		private RemoteMeteringType remoteMetering;
		private MeteringFunctionCodeType meteringFunctionCode;
        private PrimaryValueType valueType;

		#endregion

		#region Properties

		public int TimeSeriesKey
		{
			get { return this.timeSeriesKey; }
			set
			{
				this.timeSeriesKey = value;
			}
		}

		public MeasuringUnitCodeType MeasureUnitCode
		{
			get	{ return this.measureUnitCode; }
			set
			{
				this.measureUnitCode = value;
				fieldEditStatus[measureUnitCodeBit] = true;
			}
		}

		public bool MeasureUnitCodeEdited
		{
			get{ return fieldEditStatus[measureUnitCodeBit]; }
		}
        
		public FlowDirectionType DirectionOfFlow
		{
			get { return this.directionOfFlow; }
			set
			{
				this.directionOfFlow = value;
				fieldEditStatus[directionOfFlowBit] = true;
			}
		}

		public bool DirectionOfFlowEdited
		{
			get{ return fieldEditStatus[directionOfFlowBit]; }
		}

		public RemoteMeteringType RemoteMetering
		{
			get	{ return this.remoteMetering; }
			set
			{
				this.remoteMetering = value;
				fieldEditStatus[remoteMeteringBit] = true;
			}
		}
        
		public bool RemoteMeteringEdited
		{
			get{ return fieldEditStatus[remoteMeteringBit]; }
		}

		public WayOfRegistrationType WayOfRegistration
		{
			get { return this.wayOfRegistration; }
			set
			{
				this.wayOfRegistration = value;
				fieldEditStatus[wayOfRegistrationBit] = true;
			}
		}
        
		public bool WayOfRegistrationEdited
		{
			get{ return fieldEditStatus[wayOfRegistrationBit]; }
		}

		public MeteringFunctionCodeType MeteringFunctionCode
		{
			get	{ return this.meteringFunctionCode;	}
			set
			{
				this.meteringFunctionCode = value;
				fieldEditStatus[meteringFunctionCodeBit] = true;
			}
		}

		public bool MeteringFunctionCodeEdited
		{
			get{ return fieldEditStatus[meteringFunctionCodeBit]; }
		}

        public PrimaryValueType ValueType
        {
            get { return this.valueType; }
            set
            {
                this.valueType = value;
                fieldEditStatus[valueTypeBit] = true;
            }
        }

		#endregion
		
		#region Methods

		private void ClearEdited()
		{
			fieldEditStatus = new BitVector32(0);
		}

		#endregion

		#region Constructors
		
		public TimeSeriesDefinition()
		{
		}

		public TimeSeriesDefinition(TimeSeriesDefinition timeseriesDefinition)
		{
			this.MeasureUnitCode = timeseriesDefinition.MeasureUnitCode;
			this.DirectionOfFlow = timeseriesDefinition.DirectionOfFlow;
			this.RemoteMetering = timeseriesDefinition.RemoteMetering;
			this.WayOfRegistration = timeseriesDefinition.WayOfRegistration;
			this.MeteringFunctionCode = timeseriesDefinition.MeteringFunctionCode;
            this.ValueType = timeseriesDefinition.ValueType;

			ClearEdited();
		}

		internal TimeSeriesDefinition(
			int timeSeriesKey,
			MeasuringUnitCodeType measureUnitCode,
			FlowDirectionType directionOfFlow,
			RemoteMeteringType remoteMetering,
			WayOfRegistrationType wayOfRegistration,
			MeteringFunctionCodeType meteringFunctionCode,
            PrimaryValueType valueType) : this (measureUnitCode, directionOfFlow, remoteMetering, wayOfRegistration, meteringFunctionCode, valueType)
		{
			this.timeSeriesKey = timeSeriesKey;
		}

		public TimeSeriesDefinition(
			MeasuringUnitCodeType measureUnitCode,
			FlowDirectionType directionOfFlow,
			RemoteMeteringType remoteMetering,
			WayOfRegistrationType wayOfRegistration,
			MeteringFunctionCodeType meteringFunctionCode,
            PrimaryValueType valueType)
		{
			this.MeasureUnitCode = measureUnitCode;
			this.DirectionOfFlow = directionOfFlow;
			this.RemoteMetering = remoteMetering;
			this.WayOfRegistration = wayOfRegistration;
			this.MeteringFunctionCode = meteringFunctionCode;
            this.ValueType = valueType;

			ClearEdited();
		}

		#endregion
	}
}
