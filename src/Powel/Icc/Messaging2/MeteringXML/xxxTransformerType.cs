namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class TransformerType
    {
        
        private string transformerIDField;
        
        private int trafoPrimaryField;
        
        private int trafoSecondaryField;
        
        private TrafoType trafoTypeField;
        
        private PhaseType phaseField;
        
        private bool phaseFieldSpecified;
        
        private string masterNameField;
        
        /// <remarks/>
        public string transformerID
        {
            get
            {
                return this.transformerIDField;
            }
            set
            {
                this.transformerIDField = value;
            }
        }
        
        /// <remarks/>
        public int trafoPrimary
        {
            get
            {
                return this.trafoPrimaryField;
            }
            set
            {
                this.trafoPrimaryField = value;
            }
        }
        
        /// <remarks/>
        public int trafoSecondary
        {
            get
            {
                return this.trafoSecondaryField;
            }
            set
            {
                this.trafoSecondaryField = value;
            }
        }
        
        /// <remarks/>
        public TrafoType trafoType
        {
            get
            {
                return this.trafoTypeField;
            }
            set
            {
                this.trafoTypeField = value;
            }
        }
        
        /// <remarks/>
        public PhaseType phase
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
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool phaseSpecified
        {
            get
            {
                return this.phaseFieldSpecified;
            }
            set
            {
                this.phaseFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public string masterName
        {
            get
            {
                return this.masterNameField;
            }
            set
            {
                this.masterNameField = value;
            }
        }
    }
}
