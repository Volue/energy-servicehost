namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class IECReadingTypeType
    {
        
        private IECTimeAttributeType timeAttributeField;
        
        private IECDataQualifierType dataQualifierField;
        
        private IECAccumulationBehaviourType accumulationBehaviourField;
        
        private IECDirectionOfFlowType flowDirectionField;
        
        private IECUnitOfMeasurementEnumerationType uomCategoryField;
        
        private IECMeasurementCategoryType measurementCategoryField;
        
        private IECPhaseEnumerationType phaseField;
        
        private IECMetricMultiplierType multiplierField;
        
        private IECUnitOfMeasurementType unitOfMeasurementField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IECTimeAttributeType TimeAttribute
        {
            get
            {
                return this.timeAttributeField;
            }
            set
            {
                this.timeAttributeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IECDataQualifierType DataQualifier
        {
            get
            {
                return this.dataQualifierField;
            }
            set
            {
                this.dataQualifierField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IECAccumulationBehaviourType AccumulationBehaviour
        {
            get
            {
                return this.accumulationBehaviourField;
            }
            set
            {
                this.accumulationBehaviourField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IECDirectionOfFlowType FlowDirection
        {
            get
            {
                return this.flowDirectionField;
            }
            set
            {
                this.flowDirectionField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IECUnitOfMeasurementEnumerationType UomCategory
        {
            get
            {
                return this.uomCategoryField;
            }
            set
            {
                this.uomCategoryField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IECMeasurementCategoryType MeasurementCategory
        {
            get
            {
                return this.measurementCategoryField;
            }
            set
            {
                this.measurementCategoryField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IECPhaseEnumerationType Phase
        {
            get
            {
                return this.phaseField;
            }
            set
            {
                this.phaseField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IECMetricMultiplierType Multiplier
        {
            get
            {
                return this.multiplierField;
            }
            set
            {
                this.multiplierField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public IECUnitOfMeasurementType UnitOfMeasurement
        {
            get
            {
                return this.unitOfMeasurementField;
            }
            set
            {
                this.unitOfMeasurementField = value;
            }
        }
    }
}
