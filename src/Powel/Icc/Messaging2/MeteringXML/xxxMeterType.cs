namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class MeterType
    {
        
        private string meterIDField;
        
        private System.Collections.Generic.List<ParameterType> parametersField;
        
        private string masterNameField;
        
        private string terminalIDField;
        
        /// <remarks/>
        public string meterID
        {
            get
            {
                return this.meterIDField;
            }
            set
            {
                this.meterIDField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlArrayItemAttribute("parameter", IsNullable=false)]
        public System.Collections.Generic.List<ParameterType> parameters
        {
            get
            {
                return this.parametersField;
            }
            set
            {
                this.parametersField = value;
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
        
        /// <remarks/>
        public string terminalID
        {
            get
            {
                return this.terminalIDField;
            }
            set
            {
                this.terminalIDField = value;
            }
        }
        
        public virtual bool ShouldSerializeparameters()
        {
            return ((this.parameters != null) 
                        && (this.parameters.Count > 0));
        }
    }
}
