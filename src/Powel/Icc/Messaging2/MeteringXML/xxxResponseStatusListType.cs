namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class ResponseStatusListType
    {
        
        private string messageIDField;
        
        private string referringMessageIDField;
        
        private System.Collections.Generic.List<StatusItemType> statusesField;
        
        /// <remarks/>
        public string messageID
        {
            get
            {
                return this.messageIDField;
            }
            set
            {
                this.messageIDField = value;
            }
        }
        
        /// <remarks/>
        public string referringMessageID
        {
            get
            {
                return this.referringMessageIDField;
            }
            set
            {
                this.referringMessageIDField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlArrayItemAttribute("status", IsNullable=false)]
        public System.Collections.Generic.List<StatusItemType> statuses
        {
            get
            {
                return this.statusesField;
            }
            set
            {
                this.statusesField = value;
            }
        }
        
        public virtual bool ShouldSerializestatuses()
        {
            return ((this.statuses != null) 
                        && (this.statuses.Count > 0));
        }
    }
}
