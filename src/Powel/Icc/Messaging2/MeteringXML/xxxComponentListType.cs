namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class ComponentListType
    {
        
        private string measurePointIDField;
        
        private TerminalType terminalField;
        
        private System.Collections.Generic.List<MeterType> metersField;
        
        private System.Collections.Generic.List<TransformerType> transformersField;
        
        /// <remarks/>
        public string measurePointID
        {
            get
            {
                return this.measurePointIDField;
            }
            set
            {
                this.measurePointIDField = value;
            }
        }
        
        /// <remarks/>
        public TerminalType terminal
        {
            get
            {
                return this.terminalField;
            }
            set
            {
                this.terminalField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlArrayItemAttribute("meter", IsNullable=false)]
        public System.Collections.Generic.List<MeterType> meters
        {
            get
            {
                return this.metersField;
            }
            set
            {
                this.metersField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlArrayItemAttribute("transformer", IsNullable=false)]
        public System.Collections.Generic.List<TransformerType> transformers
        {
            get
            {
                return this.transformersField;
            }
            set
            {
                this.transformersField = value;
            }
        }
        
        public virtual bool ShouldSerializemeters()
        {
            return ((this.meters != null) 
                        && (this.meters.Count > 0));
        }
        
        public virtual bool ShouldSerializetransformers()
        {
            return ((this.transformers != null) 
                        && (this.transformers.Count > 0));
        }
    }
}
