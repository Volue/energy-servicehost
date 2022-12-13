namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class ParameterType
    {
        
        private string parameterNameField;
        
        private string parameterValueField;
        
        /// <remarks/>
        public string parameterName
        {
            get
            {
                return this.parameterNameField;
            }
            set
            {
                this.parameterNameField = value;
            }
        }
        
        /// <remarks/>
        public string parameterValue
        {
            get
            {
                return this.parameterValueField;
            }
            set
            {
                this.parameterValueField = value;
            }
        }
    }
}
