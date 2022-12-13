namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class ImportRequestType
    {
        
        private string importReferenceField;
        
        private MeterReadingImportIntervalType importIntervalField;
        
        private RegisterType registerField;
        
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
        public MeterReadingImportIntervalType importInterval
        {
            get
            {
                return this.importIntervalField;
            }
            set
            {
                this.importIntervalField = value;
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
