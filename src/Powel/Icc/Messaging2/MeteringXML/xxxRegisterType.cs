namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class RegisterType
    {
        
        private string senderIDField;
        
        private int registerNumberField;
        
        private IECReadingTypeType readingTypeField;
        
        private int registerCapacityDigitsField;
        
        private bool registerCapacityDigitsFieldSpecified;
        
        private int registerCapacityDecimalsField;
        
        private bool registerCapacityDecimalsFieldSpecified;
        
        private float pulseConstantField;
        
        private bool pulseConstantFieldSpecified;
        
        private bool importUseTrafoField;
        
        private bool importUseTrafoFieldSpecified;
        
        private bool importUsePulseField;
        
        private bool importUsePulseFieldSpecified;
        
        private RemoteMeteringType remoteMeteringField;
        
        private string importFormatField;
        
        private string importReferenceField;
        
        private IntervalType importIntervalField;
        
        private bool importIntervalFieldSpecified;

        private float annualConsumptionField;

        private bool annualConsumptionFieldSpecified;
        
        /// <remarks/>
        public string senderID
        {
            get
            {
                return this.senderIDField;
            }
            set
            {
                this.senderIDField = value;
            }
        }
        
        /// <remarks/>
        public int registerNumber
        {
            get
            {
                return this.registerNumberField;
            }
            set
            {
                this.registerNumberField = value;
            }
        }
        
        /// <remarks/>
        public IECReadingTypeType readingType
        {
            get
            {
                return this.readingTypeField;
            }
            set
            {
                this.readingTypeField = value;
            }
        }
        
        /// <remarks/>
        public int registerCapacityDigits
        {
            get
            {
                return this.registerCapacityDigitsField;
            }
            set
            {
                this.registerCapacityDigitsField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool registerCapacityDigitsSpecified
        {
            get
            {
                return this.registerCapacityDigitsFieldSpecified;
            }
            set
            {
                this.registerCapacityDigitsFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public int registerCapacityDecimals
        {
            get
            {
                return this.registerCapacityDecimalsField;
            }
            set
            {
                this.registerCapacityDecimalsField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool registerCapacityDecimalsSpecified
        {
            get
            {
                return this.registerCapacityDecimalsFieldSpecified;
            }
            set
            {
                this.registerCapacityDecimalsFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public float pulseConstant
        {
            get
            {
                return this.pulseConstantField;
            }
            set
            {
                this.pulseConstantField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool pulseConstantSpecified
        {
            get
            {
                return this.pulseConstantFieldSpecified;
            }
            set
            {
                this.pulseConstantFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public bool importUseTrafo
        {
            get
            {
                return this.importUseTrafoField;
            }
            set
            {
                this.importUseTrafoField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool importUseTrafoSpecified
        {
            get
            {
                return this.importUseTrafoFieldSpecified;
            }
            set
            {
                this.importUseTrafoFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public bool importUsePulse
        {
            get
            {
                return this.importUsePulseField;
            }
            set
            {
                this.importUsePulseField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool importUsePulseSpecified
        {
            get
            {
                return this.importUsePulseFieldSpecified;
            }
            set
            {
                this.importUsePulseFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public RemoteMeteringType remoteMetering
        {
            get
            {
                return this.remoteMeteringField;
            }
            set
            {
                this.remoteMeteringField = value;
            }
        }
        
        /// <remarks/>
        public string importFormat
        {
            get
            {
                return this.importFormatField;
            }
            set
            {
                this.importFormatField = value;
            }
        }
        
        /// <remarks/>
        public string importReference
        {
            get
            {
                return this.importReferenceField;
            }
            set
            {
                this.importReferenceField = value;
            }
        }
        
        /// <remarks/>
        public IntervalType importInterval
        {
            get
            {
                return this.importIntervalField;
            }
            set
            {
                this.importIntervalField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool importIntervalSpecified
        {
            get
            {
                return this.importIntervalFieldSpecified;
            }
            set
            {
                this.importIntervalFieldSpecified = value;
            }
        }

        /// <remarks/>
        public float annualConsumption
        {
            get
            {
                return this.annualConsumptionField;
            }
            set
            {
                this.annualConsumptionField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool annualConsumptionSpecified
        {
            get
            {
                return this.annualConsumptionFieldSpecified;
            }
            set
            {
                this.annualConsumptionFieldSpecified = value;
            }
        }
    }
}
