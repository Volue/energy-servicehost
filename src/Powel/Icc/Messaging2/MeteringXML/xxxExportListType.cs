namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class ExportListType
    {
        
        private System.Collections.Generic.List<ScheduleType> exportField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlArrayItemAttribute("schedule", IsNullable=false)]
        public System.Collections.Generic.List<ScheduleType> export
        {
            get
            {
                return this.exportField;
            }
            set
            {
                this.exportField = value;
            }
        }
        
        public virtual bool ShouldSerializeexport()
        {
            return ((this.export != null) 
                        && (this.export.Count > 0));
        }
    }
}
