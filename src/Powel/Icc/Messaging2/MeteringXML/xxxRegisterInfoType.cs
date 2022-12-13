namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class RegisterInfoType
    {
        
        private string measurePointIDField;
        
        private string meterIDField;
        
        private System.Collections.Generic.List<RegisterType> registersField;
        
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
        [System.Xml.Serialization.XmlArrayItemAttribute("register", IsNullable=false)]
        public System.Collections.Generic.List<RegisterType> registers
        {
            get
            {
                return this.registersField;
            }
            set
            {
                this.registersField = value;
            }
        }
        
        public virtual bool ShouldSerializeregisters()
        {
            return ((this.registers != null) 
                        && (this.registers.Count > 0));
        }
    }
}
