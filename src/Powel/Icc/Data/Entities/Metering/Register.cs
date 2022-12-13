using System;
using System.Collections.Specialized;
using Powel.Icc.Services.Time;


namespace Powel.Icc.Data.Entities.Metering
{
	/// <summary>
	/// Summary description for Register.
	/// </summary>
	public class Register
	{

		#region Fields
		
		protected BitVector32 fieldEditStatus = new BitVector32(0);

		private static readonly int registerCapacityDigitsBit = BitVector32.CreateMask();
		private static readonly int registerCapacityDecimalsBit = BitVector32.CreateMask(registerCapacityDigitsBit);
		private static readonly int stateBit = BitVector32.CreateMask(registerCapacityDecimalsBit);
		private static readonly int validFromDateBit = BitVector32.CreateMask(stateBit);
		private static readonly int validToDateBit = BitVector32.CreateMask(validFromDateBit);
		private static readonly int meteringConstantBit = BitVector32.CreateMask(validToDateBit);
		private static readonly int pulseConstantBit = BitVector32.CreateMask(meteringConstantBit);
		private static readonly int wayOfRegistrationBit = BitVector32.CreateMask(pulseConstantBit);
		private static readonly int measureUnitCodeBit = BitVector32.CreateMask(wayOfRegistrationBit);
		private static readonly int directionOfFlowBit = BitVector32.CreateMask(measureUnitCodeBit);
		private static readonly int remoteMeteringBit = BitVector32.CreateMask(directionOfFlowBit);
		private static readonly int meteringFunctionCodeBit = BitVector32.CreateMask(remoteMeteringBit);
        private static readonly int meterIDBit = BitVector32.CreateMask(meteringFunctionCodeBit);
        private static readonly int valueTypeBit = BitVector32.CreateMask(meterIDBit);
	    private static readonly int annualConsumptionBit = BitVector32.CreateMask(valueTypeBit);

        /*private static readonly int iecTimeAttributeBit = BitVector32.CreateMask(valueTypeBit);
        private static readonly int iecDataQualifierBit = BitVector32.CreateMask(iecTimeAttributeBit);
        private static readonly int iecAccumulationBehaviourBit = BitVector32.CreateMask(iecDataQualifierBit);
        private static readonly int iecDirectionOfFlowBit = BitVector32.CreateMask(iecAccumulationBehaviourBit);
        private static readonly int iecUOMCategoryBit = BitVector32.CreateMask(iecDirectionOfFlowBit);
        private static readonly int iecMeasurementCategoryBit = BitVector32.CreateMask(iecUOMCategoryBit);
        private static readonly int iecPhaseBit = BitVector32.CreateMask(iecMeasurementCategoryBit);
        private static readonly int iecMultiplierBit = BitVector32.CreateMask(iecPhaseBit);
        private static readonly int iecMeasureUnitCodeBit = BitVector32.CreateMask(iecMultiplierBit);*/

		internal int key;
		private int registerNumber;
		private int registerCapacityDigits;
		private int registerCapacityDecimals;
		private RegisterState state;
		private UtcTime validFromDate;
		private UtcTime validToDate;
		private string meteringConstant;
		private System.Single pulseConstant;
		private WayOfRegistrationType wayOfRegistration;
		private MeasuringUnitCodeType measureUnitCode;
		private FlowDirectionType directionOfFlow;
		private RemoteMeteringType remoteMetering;
		private MeteringFunctionCodeType meteringFunctionCode;
		private string meterID;
        private PrimaryValueType valueType;
	    private float annualConsumption;

        //private int iecTimeAttribute;
        //private int iecDataQualifier;
        //private int iecAccumulationBehaviour;
        private int iecDirectionOfFlow;
        //private int iecUOMCategory;
        //private int iecMeasurementCategory;
        //private int iecPhase;
        //private int iecMultiplier;
        //private int iecMeasureUnitCode;

        // Variables used to merge registers
	    public int unme_key;
	    public int tsin_key;
	    public int tser_key;

        // Variables to be used to set up import/export definitions.
	    public int timsKey;
	    public string senderID;
        public bool importUseTrafo;
	    public bool importUsePulse;
	    public string importFormat;
	    public string importReference;
	    public int importInterval;
	    public string exportFormat;
	    public string exportMethod;
	    public string exportReference;
	    public bool exportUseTrafo;
	    public bool exportUsePulse;
	    public int mrExportInterval;
	    public UtcTime mrExportValuesFrom;
	    public UtcTime mrExportStartTime;
        public string receiverID;
	    public ReadingType readingType;

		#endregion

		#region Properties

		public int Key
		{
			get { return this.key; }
		}

		public int RegisterNumber
		{
			get { return this.registerNumber; }
			set { this.registerNumber = value;}
		}
        
		public int CapacityDigits
		{
			get { return this.registerCapacityDigits; }
			set
			{
				this.registerCapacityDigits = value;
				fieldEditStatus[registerCapacityDigitsBit] = true;
			}
		}

		public bool CapacityDigitsEdited
		{
			get{ return fieldEditStatus[registerCapacityDigitsBit]; }
		}
        
		public int CapacityDecimals
		{
			get { return this.registerCapacityDecimals; }
			set
			{
				this.registerCapacityDecimals = value;
				fieldEditStatus[registerCapacityDecimalsBit] = true;
			}
		}

		public bool CapacityDecimalsEdited
		{
			get{ return fieldEditStatus[registerCapacityDecimalsBit]; }
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

		public string MeteringConstant
		{
			get { return this.meteringConstant;	}
			set
			{
				this.meteringConstant = value;
				fieldEditStatus[meteringConstantBit] = true;
			}
		}
        
		public bool MeteringConstantEdited
		{
			get{ return fieldEditStatus[meteringConstantBit]; }
		}

		public System.Single PulseConstant
		{
			get { return this.pulseConstant; }
			set
			{
				this.pulseConstant = value;
				fieldEditStatus[pulseConstantBit] = true;
			}
		}
        
		public bool PulseConstantEdited
		{
			get{ return fieldEditStatus[pulseConstantBit]; }
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

		public RegisterState State
		{
			get{ return this.state;}
			set
			{
				this.state = value;
				fieldEditStatus[stateBit] = true;
			}
		}

		public bool StateEdited
		{
			get{ return fieldEditStatus[stateBit]; }
		}

		public UtcTime ValidFromDate
		{
			get{ return this.validFromDate;}
			set
			{
				this.validFromDate = value;
				fieldEditStatus[validFromDateBit] = true;
			}
		}

		public bool ValidFromDateEdited
		{
			get{ return fieldEditStatus[validFromDateBit]; }
		}

		public UtcTime ValidToDate
		{
			get{ return this.validToDate;}
			set
			{
				this.validToDate = value;
				fieldEditStatus[validToDateBit] = true;
			}
		}

		public bool ValidToDateEdited
		{
			get{ return fieldEditStatus[validToDateBit]; }
		}

		public string MeterID
		{
			get { return this.meterID; }
			set
			{
				this.meterID = value;
				fieldEditStatus[meterIDBit] = true;
			}
		}
		public bool MeterIDEdited
		{
			get { return fieldEditStatus[meterIDBit]; }
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

	    public float AnnualConsumption
	    {
            get { return this.annualConsumption; }
            set 
            { 
                this.annualConsumption = value;
                fieldEditStatus[annualConsumptionBit] = true;
            }
	    }

	    public bool AnnualConsumptionEdited
	    {
            get { return fieldEditStatus[annualConsumptionBit]; }
	    }

        //public int IECTimeAttribute
        //{
        //    get { return iecTimeAttribute; }
        //    set
        //    {
        //        iecTimeAttribute = value;
        //        //fieldEditStatus[iecTimeAttributeBit] = true;
        //    }
        //}

        // /*public bool IECTimeAttributeEdited
        //{
        //    get { return fieldEditStatus[iecTimeAttributeBit]; }
        //}*/

        //public int IECDataQualifier
        //{
        //    get { return iecDataQualifier; }
        //    set
        //    {
        //        iecDataQualifier = value;
        //        //fieldEditStatus[iecDataQualifierBit] = true;
        //    }
        //}

        // /*public bool IECDataQualifierEdited
        //{
        //    get { return fieldEditStatus[iecDataQualifierBit]; }
        //}*/

        //public int IECAccumulationBehaviour
        //{
        //    get { return iecAccumulationBehaviour; }
        //    set
        //    {
        //        iecAccumulationBehaviour = value;
        //        //fieldEditStatus[iecAccumulationBehaviourBit] = true;
        //    }
        //}

        // /*public bool IECAccumulationBehaviourEdited
        //{
        //    get { return fieldEditStatus[iecAccumulationBehaviourBit]; }
        //}*/


        public int IECDirectionOfFlow
        {
            get { return iecDirectionOfFlow; }
            set
            {
                iecDirectionOfFlow = value;
                //fieldEditStatus[iecDirectionOfFlowBit] = true;
            }
        }

        // /*public bool IECDirectionOfFlowEdited
        //{
        //    get { return fieldEditStatus[iecDirectionOfFlowBit]; }
        //}*/

        //public int IECUOMCategory
        //{
        //    get { return iecUOMCategory; }
        //    set
        //    {
        //        iecUOMCategory = value;
        //        //fieldEditStatus[iecUOMCategoryBit] = true;
        //    }
        //}

        // /*public bool IECUOMCategoryEdited
        //{
        //    get { return fieldEditStatus[iecUOMCategoryBit]; }
        //}*/

        //public int IECMeasurementCategory {
        //    get { return iecMeasurementCategory; }
        //    set
        //    {
        //        iecMeasurementCategory = value;
        //        //fieldEditStatus[iecMeasurementCategoryBit] = true;
        //    }
        //}

        // /*public bool IECMeasurementCategoryEdited
        //{
        //    get { return fieldEditStatus[iecMeasurementCategoryBit]; }
        //}*/

        //public int IECPhase
        //{
        //    get { return iecPhase; }
        //    set
        //    {
        //        iecPhase = value;
        //        //fieldEditStatus[iecPhaseBit] = true;
        //    }
        //}

        // /*public bool IECPhaseEdited
        //{
        //    get { return fieldEditStatus[iecPhaseBit]; }
        //}*/
        
        //public int IECMultiplier
        //{
        //    get { return iecMultiplier; }
        //    set
        //    {
        //        iecMultiplier = value;
        //        //fieldEditStatus[iecMultiplierBit] = true;
        //    }
        //}

        // /*public bool IECMultiplierEdited
        //{
        //    get { return fieldEditStatus[iecMultiplierBit]; }
        //}*/

        //public int IECMeasureUnitCode
        //{
        //    get { return iecMeasureUnitCode; }
        //    set
        //    {
        //        iecMeasureUnitCode = value;
        //        //fieldEditStatus[iecMeasureUnitCodeBit] = true;
        //    }
        //}

        // /*public bool IECMeasureUnitCodeEdited
        //{
        //    get { return fieldEditStatus[iecMeasureUnitCodeBit]; }
        //}*/

		#endregion

		#region Constructors
		
		public Register()
		{
		    this.AnnualConsumption = float.NaN;
		    this.ClearEdited();
		}

		public Register(Register register)
		{
			this.key = register.Key; // read-only property
			this.RegisterNumber = register.RegisterNumber;
			this.CapacityDigits = register.CapacityDigits;
			this.CapacityDecimals = register.CapacityDecimals;
			this.MeasureUnitCode = register.MeasureUnitCode;
			this.DirectionOfFlow = register.DirectionOfFlow;
			this.MeteringConstant = register.MeteringConstant;
			this.PulseConstant = register.PulseConstant;
			this.RemoteMetering = register.RemoteMetering;
			this.WayOfRegistration = register.WayOfRegistration;
			this.MeteringFunctionCode = register.MeteringFunctionCode;
			this.State = register.State;
			this.ValidFromDate = register.ValidFromDate;
			this.ValidToDate = register.ValidToDate;
			this.meterID = register.MeterID;
		    this.AnnualConsumption = register.AnnualConsumption;
			ClearEdited();
		}

		internal Register(
			int key,
			int registerNumber,
			int capacityDigits,
			int capacityDecimals,
			MeasuringUnitCodeType measureUnitCode,
			FlowDirectionType directionOfFlow,
			string meteringConstant,
			System.Single pulseConstant,
			RemoteMeteringType remoteMetering,
			WayOfRegistrationType wayOfRegistration,
			MeteringFunctionCodeType meteringFunctionCode,
			RegisterState state,
			UtcTime validFromDate,
			UtcTime validToDate,
			string meterID,
            PrimaryValueType valueType,
            float annualConsumption)
			: this(
			registerNumber,
			capacityDigits,
			capacityDecimals,
			measureUnitCode,
			directionOfFlow,
			meteringConstant,
			pulseConstant,
			remoteMetering,
			wayOfRegistration,
			meteringFunctionCode,
			state,
			validFromDate,
			validToDate,
			meterID,
            valueType,
            annualConsumption)
		{
			this.key = key;
			ClearEdited();
		}

        internal Register(
            int key,
            int registerNumber,
            int capacityDigits,
            int capacityDecimals,
            //int iecMeasureUnitCode,
            int iecDirectionOfFlow,
            string meteringConstant,
            System.Single pulseConstant,
            RemoteMeteringType remoteMetering,
            RegisterState state,
            UtcTime validFromDate,
            UtcTime validToDate,
            string meterID,
            PrimaryValueType valueType,
            float annualConsumption)
            : this(
            registerNumber,
            capacityDigits,
            capacityDecimals,
            //iecMeasureUnitCode,
            iecDirectionOfFlow,
            meteringConstant,
            pulseConstant,
            remoteMetering,
            state,
            validFromDate,
            validToDate,
            meterID,
            valueType,
            annualConsumption)
        {
            this.key = key;
            ClearEdited();
        }

		public Register(
			int registerNumber,
			int capacityDigits,
			int capacityDecimals,
			MeasuringUnitCodeType measureUnitCode,
			FlowDirectionType directionOfFlow,
			string meteringConstant,
			System.Single pulseConstant,
			RemoteMeteringType remoteMetering,
			WayOfRegistrationType wayOfRegistration,
			MeteringFunctionCodeType meteringFunctionCode,
			RegisterState state,
			UtcTime validFromDate,
			UtcTime validToDate,
			string meterID,
            PrimaryValueType valueType,
            float annualConsumption)
		{
			this.RegisterNumber = registerNumber;
			this.CapacityDigits = capacityDigits;
			this.CapacityDecimals = capacityDecimals;
			this.MeasureUnitCode = measureUnitCode;
			this.DirectionOfFlow = directionOfFlow;
			this.MeteringConstant = meteringConstant;
			this.PulseConstant = pulseConstant;
			this.RemoteMetering = remoteMetering;
			this.WayOfRegistration = wayOfRegistration;
			this.MeteringFunctionCode = meteringFunctionCode;
			this.State = state;
			this.ValidFromDate = validFromDate;
			this.ValidToDate = validToDate;
			this.MeterID = meterID;
            this.ValueType = valueType;
		    this.AnnualConsumption = annualConsumption;
			ClearEdited();
		}

        public Register(
            int registerNumber,
            int capacityDigits,
            int capacityDecimals,
            //MeasuringUnitCodeType measureUnitCode,
            //int iecMeasureUnitCode,
            //FlowDirectionType directionOfFlow,
            int iecDirectionOfFlow,
            string meteringConstant,
            System.Single pulseConstant,
            RemoteMeteringType remoteMetering,
            //WayOfRegistrationType wayOfRegistration,
            //MeteringFunctionCodeType meteringFunctionCode,
            RegisterState state,
            UtcTime validFromDate,
            UtcTime validToDate,
            string meterID,
            PrimaryValueType valueType,
            float annualConsumption)
        {
            this.RegisterNumber = registerNumber;
            this.CapacityDigits = capacityDigits;
            this.CapacityDecimals = capacityDecimals;
            //this.MeasureUnitCode = measureUnitCode;
            //this.IECMeasureUnitCode = iecMeasureUnitCode;
            //this.DirectionOfFlow = directionOfFlow;
            this.IECDirectionOfFlow = iecDirectionOfFlow;
            this.MeteringConstant = meteringConstant;
            this.PulseConstant = pulseConstant;
            this.RemoteMetering = remoteMetering;
            //this.WayOfRegistration = wayOfRegistration;
            //this.MeteringFunctionCode = meteringFunctionCode;
            this.State = state;
            this.ValidFromDate = validFromDate;
            this.ValidToDate = validToDate;
            this.MeterID = meterID;
            this.ValueType = valueType;
            this.AnnualConsumption = annualConsumption;
            ClearEdited();
        }

		#endregion

		#region Methods

		public bool Merge(Register register)
		{
			bool edited = false;

			if(register.DirectionOfFlowEdited && this.DirectionOfFlow != register.DirectionOfFlow)
			{
				this.DirectionOfFlow = register.DirectionOfFlow;
				edited = true;
			}
			if(register.MeasureUnitCodeEdited && this.MeasureUnitCode != register.MeasureUnitCode)
			{
				this.MeasureUnitCode = register.MeasureUnitCode;
				edited = true;
			}
			if(register.MeteringConstantEdited && this.MeteringConstant != register.MeteringConstant)
			{
				this.MeteringConstant = register.MeteringConstant;
				edited = true;
			}
			if(register.PulseConstantEdited && this.PulseConstant != register.PulseConstant)
			{
				this.PulseConstant = register.PulseConstant;
				edited = true;
			}
			if(register.CapacityDigitsEdited && this.CapacityDigits != register.CapacityDigits)
			{
				this.CapacityDigits = register.CapacityDigits;
				edited = true;
			}
			if(register.CapacityDecimalsEdited && this.CapacityDecimals != register.CapacityDecimals)
			{
				this.CapacityDecimals = register.CapacityDecimals;
				edited = true;
			}
			if(register.RemoteMeteringEdited && this.RemoteMetering != register.RemoteMetering)
			{
				this.RemoteMetering = register.RemoteMetering;
				edited = true;
			}
			if(register.StateEdited && this.State != register.State)
			{
				this.State = register.State;
				edited = true;
			}
			if(register.ValidFromDateEdited && this.ValidFromDate != register.ValidFromDate)
			{
				this.ValidFromDate = register.ValidFromDate;
				edited = true;
			}
			if(register.ValidToDateEdited && this.ValidToDate != register.ValidToDate)
			{
				this.ValidToDate = register.ValidToDate;
				edited = true;
			}
			if(register.WayOfRegistrationEdited && this.WayOfRegistration != register.WayOfRegistration)
			{
				this.WayOfRegistration = register.WayOfRegistration;
				edited = true;
			}
			if (register.MeterIDEdited && this.MeterID != register.MeterID)
			{
				this.MeterID = register.MeterID;
				edited = true;
			}

            /*if (register.IECTimeAttributeEdited && this.IECTimeAttribute != register.IECTimeAttribute)
            {
                this.IECTimeAttribute = register.IECTimeAttribute;
                edited = true;
            }

            if (register.IECDataQualifierEdited && this.IECDataQualifier != register.IECDataQualifier)
            {
                this.IECDataQualifier = register.IECDataQualifier;
                edited = true;
            }

            if (register.IECAccumulationBehaviourEdited && this.IECAccumulationBehaviour != register.IECAccumulationBehaviour)
            {
                this.IECAccumulationBehaviour = register.IECAccumulationBehaviour;
                edited = true;
            }

            if (register.IECDirectionOfFlowEdited && this.IECDirectionOfFlow != register.IECDirectionOfFlow)
            {
                this.IECDirectionOfFlow = register.IECDirectionOfFlow;
                edited = true;
            }
            
            if (register.IECUOMCategoryEdited && this.IECUOMCategory != register.IECUOMCategory)
            {
                this.IECUOMCategory = register.IECUOMCategory;
                edited = true;
            }

            if (register.IECMeasurementCategoryEdited && this.IECMeasurementCategory != register.IECMeasurementCategory)
            {
                this.IECMeasurementCategory = register.IECMeasurementCategory;
                edited = true;
            }

            if (register.IECPhaseEdited && this.IECPhase != register.IECPhase)
            {
                this.IECPhase = register.IECPhase;
                edited = true;
            }

            if (register.IECMultiplierEdited && this.IECMultiplier != register.IECMultiplier)
            {
                this.IECMultiplier = register.IECMultiplier;
                edited = true;
            }

            if (register.IECMeasureUnitCodeEdited && this.IECMeasureUnitCode != register.IECMeasureUnitCode)
            {
                this.IECMeasureUnitCode = register.IECMeasureUnitCode;
                edited = true;
            }*/

            if (register.AnnualConsumptionEdited && this.AnnualConsumption != register.AnnualConsumption)
            {
                this.AnnualConsumption = register.AnnualConsumption;
                edited = true;
            }

			return edited;
		}

        public bool Merge(Register register, ReadingType readingType)
        {
            bool edited = Merge(register);

            //if (this.IECMeasureUnitCode != register.IECMeasureUnitCode)
            if (this.unme_key != readingType.UnmeKey)
            {
                this.unme_key = readingType.UnmeKey;
                edited = true;
            }
                if (this.IECDirectionOfFlow != register.IECDirectionOfFlow)
            {
                this.iecDirectionOfFlow = register.IECDirectionOfFlow;
                edited = true;
            }
            /*if (this.tsin_key != register.tsin_key)
            {
                this.tsin_key = register.tsin_key;
                edited = true;
            }
            if (this.tser_key != register.tser_key)
            {
                this.tser_key = register.tser_key;
                edited = true;
            }*/
            if (this.tsin_key != readingType.TsinKey)
            {
                this.tsin_key = readingType.TsinKey;
                edited = true;
            }
            if (this.tser_key != readingType.TserKey) 
            {
                this.tser_key = readingType.TserKey;
                edited = true;
            }
            if (this.ValueType != (PrimaryValueType)readingType.VltpKey)
            {
                this.ValueType = (PrimaryValueType) readingType.VltpKey;
                edited = true;
            }

            return edited;
        }

		#endregion

        #region System.Object

        /// <summary>
        /// Prints all public properties of a Measure Point, excluded any related agreements, components or terminals.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
          string separator = ";";
          return "Key: " + this.Key + separator +
								 "MeterID: " + this.MeterID + separator +
								 "RegisterNumber: " + this.RegisterNumber + separator +
								 "ValueType: " + this.ValueType + separator +
								 "ValidFromDate: " + this.ValidFromDate + separator +
								 "ValidToDate: " + this.ValidToDate + separator +
								 "CapacityDigits: " + this.CapacityDigits + separator +
                 "CapacityDecimals: " + this.CapacityDecimals + separator +
                 "MeasureUnitCode: " + this.MeasureUnitCode + separator +
                 "DirectionOfFlow: " +this.DirectionOfFlow + separator +
                 "MeteringConstant: " + this.MeteringConstant + separator +
                 "PulseConstant:" + this.PulseConstant + separator +
                 "RemoteMetering: " + this.RemoteMetering + separator +
                 "WayOfRegistration: " + this.WayOfRegistration + separator +
								 "MeteringFunctionCode: " + this.MeteringFunctionCode + separator +
                 "State:" + this.State + separator +
			       "";
        }

        /// <summary>
        /// Calculates a unique hash code for a regiser object.
        /// </summary>
        /// <returns></returns>
        public override int GetHashCode()
        {
            return (this.ToString() + key.ToString() + registerNumber.ToString() + meterID.ToString()).GetHashCode();
        }

        /// <summary>
        /// Two registers are equal if all their properties are equal.
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj)
        {
            Register r = obj as Register;
            if (r == null)
                return false;
            if (this.Key == r.Key &&
                this.RegisterNumber == r.RegisterNumber &&
                this.CapacityDigits == r.CapacityDigits &&
                this.CapacityDecimals == r.CapacityDecimals &&
                this.MeasureUnitCode == r.MeasureUnitCode &&
                this.DirectionOfFlow == r.DirectionOfFlow &&
                this.MeteringConstant == r.MeteringConstant &&
                this.PulseConstant == r.PulseConstant &&
                this.RemoteMetering == r.RemoteMetering &&
                this.WayOfRegistration == r.WayOfRegistration &&
                this.MeteringFunctionCode == r.MeteringFunctionCode &&
                this.State == r.State &&
                this.ValidFromDate == r.ValidFromDate &&
                this.ValidToDate == r.ValidToDate &&
                this.MeterID == r.MeterID &&
                this.ValueType == r.ValueType)
                return true;
            return false;
        }

        #endregion

		private void ClearEdited()
		{
			fieldEditStatus = new BitVector32(0);
		}
	}

	public enum RegisterState
	{
		INACTIVE = 0,
		ACTIVE = 1
	}

	//dummy enums - correct values need to be added
	public enum MeasuringUnitCodeType {
		KWH       = 800,
		KWH_H     = 140,
		KVARH     = 827,
		KVARH_H   = 828,
		M3        = 100,
		M3_H      = 830,
		L_H       = 847,
		BAR_A     = 210,
		NM3       = 204,
		DEGREES_C = 114 
	}

	public enum FlowDirectionType {
		IN      = 1,
		OUT     = 2,
		IN_IN   = 3,
		IN_OUT  = 4,
		OUT_IN  = 5,
		OUT_OUT = 6
	}

	public enum RemoteMeteringType {
		NO  = 0,
		YES = 1
	}

	public enum WayOfRegistrationType{
		ACCUMULATIVE = -1,
		DIFFERENTIAL = 101
	}

   	public enum MeteringFunctionCodeType { 
        UNDEFINED = -1, 
        LOW = 4011, 
        HIGH = 4012, 
        SINGLE = 4013, 
        HOUR = 4010,
        WATERVOLUME = 9315,
        GASENERGY   = 9316,
        GASMEASURED = 9317,
        GASSTANDARD = 9318,
        GASCONTROL  = 9319,
        GASMINPRESS = 9320,
        GASMAXPRESS = 9321,
        GASMINTEMP  = 9322,
        GASMAXTEMP  = 9323,
        HOTWATERTEMPTURN    = 9400,
        HOTWATERTEMPRETURN  = 9401,
        HOTWATERMAXENERGY   = 9402,
        HOTWATERMAXVOLUME   = 9403,
        HOTWATERENERGY      = 9404,
        HOTWATERVOLUME      = 9405,
        COOLWATERTEMPTURN   = 9450,
        COOLWATERTEMPRETURN = 9451,
        COOLWATERMAXENERGY  = 9452,
        COOLWATERMAXVOLUME  = 9453,
        COOLWATERENERGY     = 9454,
        COOLWATERVOLUME     = 9455,
		BILLING             = 20200 
	}

}
