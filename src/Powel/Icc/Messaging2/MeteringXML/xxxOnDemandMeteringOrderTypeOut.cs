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
    public partial class OnDemandMeteringOrderTypeOut
    {
        
        private PriorityType priorityField;
        
        private UtcTime dateTransferField;
        
        private UtcTime dateFromField;
        
        private UtcTime dateToField;
        
        private System.Collections.Generic.List<MeasurePointDetailsType> measurePointsField;
        
        /// <remarks/>
        public PriorityType priority
        {
            get
            {
                return this.priorityField;
            }
            set
            {
                this.priorityField = value;
            }
        }

        [XmlIgnore]
        public UtcTime dateTransfer
        {
            get
            {
                return this.dateTransferField;
            }
            set
            {
                if ((this.dateTransferField != value))
                {
                    this.dateTransferField = value;
                }
            }
        }
        /// <remarks/>
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/", Name = "dateTransfer")]
        [XmlElement(ElementName = "dateTransfer", Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/")]
        public DateTime dateTransferXml
        {
            get
            {
                return IccConfiguration.Time.DatabaseCalendar.ToDateTime(this.dateTransferField);
            }
            set
            {
                this.dateTransferField = IccConfiguration.Time.DatabaseCalendar.ToUtcTime(value);
            }
        }

        [XmlIgnore]
        public UtcTime dateFrom
        {
            get
            {
                return this.dateFromField;
            }
            set
            {
                if ((this.dateFromField != value))
                {
                    this.dateFromField = value;
                }
            }
        }
        /// <remarks/>
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/", Name = "dateFrom")]
        [XmlElement(ElementName = "dateFrom", Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/")]
        public DateTime dateFromXml
        {
            get
            {
                return IccConfiguration.Time.DatabaseCalendar.ToDateTime(this.dateFromField);
            }
            set
            {
                this.dateFromField = IccConfiguration.Time.DatabaseCalendar.ToUtcTime(value);
            }
        }

        [XmlIgnore]
        public UtcTime dateTo
        {
            get
            {
                return this.dateToField;
            }
            set
            {
                if ((this.dateToField != value))
                {
                    this.dateToField = value;
                }
            }
        }
        /// <remarks/>
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/", Name = "dateTo")]
        [XmlElement(ElementName = "dateTo", Namespace = "http://www.powel.no/icc/ws/metering/data/1.5/")]
        public DateTime dateToXml
        {
            get
            {
                return IccConfiguration.Time.DatabaseCalendar.ToDateTime(this.dateToField);
            }
            set
            {
                this.dateToField = IccConfiguration.Time.DatabaseCalendar.ToUtcTime(value);
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlArrayItemAttribute("measurePoint", IsNullable=false)]
        public System.Collections.Generic.List<MeasurePointDetailsType> measurePoints
        {
            get
            {
                return this.measurePointsField;
            }
            set
            {
                this.measurePointsField = value;
            }
        }
        
        public virtual bool ShouldSerializemeasurePoints()
        {
            return ((this.measurePoints != null) 
                        && (this.measurePoints.Count > 0));
        }
    }
}
