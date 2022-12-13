namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class OrderType
    {
        
        private string measurePointIDField;
        
        private System.Collections.Generic.List<ImportRequestType> importRequestsField;
        
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
        [System.Xml.Serialization.XmlArrayItemAttribute("importRequest", IsNullable=false)]
        public System.Collections.Generic.List<ImportRequestType> importRequests
        {
            get
            {
                return this.importRequestsField;
            }
            set
            {
                this.importRequestsField = value;
            }
        }
        
        public virtual bool ShouldSerializeimportRequests()
        {
            return ((this.importRequests != null) 
                        && (this.importRequests.Count > 0));
        }
    }
}
