using Powel.Icc.Services.Time;

namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class LevelDetailsType
    {
        
        private UtcTime valuesFromField;
        
        private UtcTime valuesToField;
        
        private bool valuesToFieldSpecified;
        
        private IntervalType valueIntervalField;
        
        private MeterReadingImportIntervalType importIntervalField;
        
        private bool importIntervalFieldSpecified;
        
        private string productCodeField;
        
        /// <remarks/>
        //[System.Xml.Serialization.XmlAttributeAttribute()]
        public UtcTime valuesFrom
        {
            get
            {
                return this.valuesFromField;
            }
            set
            {
                this.valuesFromField = value;
            }
        }
        
        /// <remarks/>
        //[System.Xml.Serialization.XmlAttributeAttribute()]
        public UtcTime valuesTo
        {
            get
            {
                return this.valuesToField;
            }
            set
            {
                this.valuesToField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool valuesToSpecified
        {
            get
            {
                return this.valuesToFieldSpecified;
            }
            set
            {
                this.valuesToFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IntervalType valueInterval
        {
            get
            {
                return this.valueIntervalField;
            }
            set
            {
                this.valueIntervalField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public MeterReadingImportIntervalType importInterval
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
        [System.Xml.Serialization.XmlAttributeAttribute(DataType="integer")]
        public string productCode
        {
            get
            {
                return this.productCodeField;
            }
            set
            {
                this.productCodeField = value;
            }
        }
    }
}
