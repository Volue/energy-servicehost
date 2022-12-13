namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class ComponentType
    {
        
        private string componentIDField;
        
        private System.Collections.Generic.List<ParameterType> componentParametersField;
        
        private string masterNameField;
        
        /// <remarks/>
        public string componentID
        {
            get
            {
                return this.componentIDField;
            }
            set
            {
                this.componentIDField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlArrayItemAttribute("parameter", IsNullable=false)]
        public System.Collections.Generic.List<ParameterType> componentParameters
        {
            get
            {
                return this.componentParametersField;
            }
            set
            {
                this.componentParametersField = value;
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
        
        public virtual bool ShouldSerializecomponentParameters()
        {
            return ((this.componentParameters != null) 
                        && (this.componentParameters.Count > 0));
        }
    }
}
