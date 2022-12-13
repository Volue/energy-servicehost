using System;
using System.Xml.Serialization;
using Powel.Icc.Common;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class ExportDefinitionType
    {
        
        private string receiverIDField;
        
        private int registerNumberField;
        
        private IECReadingTypeType readingTypeField;
        
        private int exportCapacityDigitsField;
        
        private bool exportCapacityDigitsFieldSpecified;
        
        private int exportCapacityDecimalsField;
        
        private bool exportCapacityDecimalsFieldSpecified;
        
        private float exportConstantField;
        
        private bool exportConstantFieldSpecified;
        
        private string exportFormatField;
        
        private string exportMethodField;
        
        private string exportReferenceField;
        
        private bool exportUseTrafoField;
        
        private bool exportUseTrafoFieldSpecified;
        
        private bool exportUsePulseField;
        
        private bool exportUsePulseFieldSpecified;
        
        private IntervalType mrExportIntervalField;
        
        private bool mrExportIntervalFieldSpecified;
        
        private UtcTime mrExportValuesFromField;
        
        private bool mrExportValuesFromFieldSpecified;

        private UtcTime mrExportStartTimeField;
        
        private bool mrExportStartTimeFieldSpecified;
        
        /// <remarks/>
        public string receiverID
        {
            get
            {
                return this.receiverIDField;
            }
            set
            {
                this.receiverIDField = value;
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
        public int exportCapacityDigits
        {
            get
            {
                return this.exportCapacityDigitsField;
            }
            set
            {
                this.exportCapacityDigitsField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool exportCapacityDigitsSpecified
        {
            get
            {
                return this.exportCapacityDigitsFieldSpecified;
            }
            set
            {
                this.exportCapacityDigitsFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public int exportCapacityDecimals
        {
            get
            {
                return this.exportCapacityDecimalsField;
            }
            set
            {
                this.exportCapacityDecimalsField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool exportCapacityDecimalsSpecified
        {
            get
            {
                return this.exportCapacityDecimalsFieldSpecified;
            }
            set
            {
                this.exportCapacityDecimalsFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public float exportConstant
        {
            get
            {
                return this.exportConstantField;
            }
            set
            {
                this.exportConstantField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool exportConstantSpecified
        {
            get
            {
                return this.exportConstantFieldSpecified;
            }
            set
            {
                this.exportConstantFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public string exportFormat
        {
            get
            {
                return this.exportFormatField;
            }
            set
            {
                this.exportFormatField = value;
            }
        }
        
        /// <remarks/>
        public string exportMethod
        {
            get
            {
                return this.exportMethodField;
            }
            set
            {
                this.exportMethodField = value;
            }
        }
        
        /// <remarks/>
        public string exportReference
        {
            get
            {
                return this.exportReferenceField;
            }
            set
            {
                this.exportReferenceField = value;
            }
        }
        
        /// <remarks/>
        public bool exportUseTrafo
        {
            get
            {
                return this.exportUseTrafoField;
            }
            set
            {
                this.exportUseTrafoField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool exportUseTrafoSpecified
        {
            get
            {
                return this.exportUseTrafoFieldSpecified;
            }
            set
            {
                this.exportUseTrafoFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public bool exportUsePulse
        {
            get
            {
                return this.exportUsePulseField;
            }
            set
            {
                this.exportUsePulseField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool exportUsePulseSpecified
        {
            get
            {
                return this.exportUsePulseFieldSpecified;
            }
            set
            {
                this.exportUsePulseFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public IntervalType mrExportInterval
        {
            get
            {
                return this.mrExportIntervalField;
            }
            set
            {
                this.mrExportIntervalField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool mrExportIntervalSpecified
        {
            get
            {
                return this.mrExportIntervalFieldSpecified;
            }
            set
            {
                this.mrExportIntervalFieldSpecified = value;
            }
        }

        [XmlIgnore]
        public UtcTime mrExportValuesFrom
        {
            get
            {
                return this.mrExportValuesFromField;
            }
            set
            {
                if ((this.mrExportValuesFromField != value))
                {
                    this.mrExportValuesFromField = value;
                }
            }
        }
        /// <remarks/>
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/", Name = "mrExportValuesFrom")]
        [XmlElement(ElementName = "mrExportValuesFrom", Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/")]
        public DateTime mrExportValuesFromXml
        {
            get
            {
                return IccConfiguration.Time.DatabaseCalendar.ToDateTime(this.mrExportValuesFromField);
            }
            set
            {
                this.mrExportValuesFromField = IccConfiguration.Time.DatabaseCalendar.ToUtcTime(value);
                this.mrExportValuesFromFieldSpecified = true;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool mrExportValuesFromSpecified
        {
            get
            {
                return this.mrExportValuesFromFieldSpecified;
            }
            set
            {
                this.mrExportValuesFromFieldSpecified = value;
            }
        }

        [XmlIgnore]
        public UtcTime mrExportStartTime
        {
            get
            {
                return this.mrExportStartTimeField;
            }
            set
            {
                if ((this.mrExportStartTimeField != value))
                {
                    this.mrExportStartTimeField = value;
                }
            }
        }

        /// <remarks/>
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/", Name = "mrExportStartTime")]
        [XmlElement(ElementName = "mrExportStartTime", Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/")]
        public DateTime mrExportStartTimeXml
        {
            get
            {
                return IccConfiguration.Time.DatabaseCalendar.ToDateTime(this.mrExportStartTimeField);
            }
            set
            {
                this.mrExportStartTimeField = IccConfiguration.Time.DatabaseCalendar.ToUtcTime(value);
                this.mrExportStartTimeFieldSpecified = true;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool mrExportStartTimeSpecified
        {
            get
            {
                return this.mrExportStartTimeFieldSpecified;
            }
            set
            {
                this.mrExportStartTimeFieldSpecified = value;
            }
        }
    }
}
