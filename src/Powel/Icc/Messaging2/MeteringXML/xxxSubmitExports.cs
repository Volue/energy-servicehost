using Powel.Icc.Services.Time;

namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/")]
    public partial class SubmitExports
    {
        
        private string messageIDField;
        
        private UtcTime validFromField;
        
        private ExportInfoType exportInfoField;
        
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
        public UtcTime validFrom
        {
            get
            {
                return this.validFromField;
            }
            set
            {
                this.validFromField = value;
            }
        }
        
        /// <remarks/>
        public ExportInfoType exportInfo
        {
            get
            {
                return this.exportInfoField;
            }
            set
            {
                this.exportInfoField = value;
            }
        }
    }
}
