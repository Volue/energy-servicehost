using Powel.Icc.Services.Time;

namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class ScheduleType
    {
        
        private UtcTime startTimeField;
        
        private UtcTime endTimeField;
        
        private bool endTimeFieldSpecified;
        
        private IntervalType valueIntervalField;
        
        private object exportIntervalField;
        
        /// <remarks/>
        public UtcTime startTime
        {
            get
            {
                return this.startTimeField;
            }
            set
            {
                this.startTimeField = value;
            }
        }
        
        /// <remarks/>
        public UtcTime endTime
        {
            get
            {
                return this.endTimeField;
            }
            set
            {
                this.endTimeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool endTimeSpecified
        {
            get
            {
                return this.endTimeFieldSpecified;
            }
            set
            {
                this.endTimeFieldSpecified = value;
            }
        }
        
        /// <remarks/>
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
        public object exportInterval
        {
            get
            {
                return this.exportIntervalField;
            }
            set
            {
                this.exportIntervalField = value;
            }
        }
    }
}
