namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class MeasurePointDetailsType
    {
        
        private string measurePointIDField;
        
        private string importReferenceField;
        
        private RegisterType registerField;
        
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
        public string importReference
        {
            get
            {
                return this.importReferenceField;
            }
            set
            {
                this.importReferenceField = value;
            }
        }
        
        /// <remarks/>
        public RegisterType register
        {
            get
            {
                return this.registerField;
            }
            set
            {
                this.registerField = value;
            }
        }
    }
}
