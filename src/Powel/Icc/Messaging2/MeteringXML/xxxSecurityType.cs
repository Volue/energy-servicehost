namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class SecurityType
    {
        
        private string _logonUserField;
        
        private string _logonPwdField;

        private string _userNameField;
        
        private string _transactionIdField;
        
        /// <remarks/>
        public string logonUser
        {
            get
            {
                return this._logonUserField;
            }
            set
            {
                this._logonUserField = value;
            }
        }
        
        /// <remarks/>
        public string logonPwd
        {
            get
            {
                return this._logonPwdField;
            }
            set
            {
                this._logonPwdField = value;
            }
        }

        /// <remarks/>
        public string userName
        {
            get
            {
                return this._userNameField;
            }
            set
            {
                this._userNameField = value;
            }
        }
        
        /// <remarks/>
        public string transactionID
        {
            get
            {
                return this._transactionIdField;
            }
            set
            {
                this._transactionIdField = value;
            }
        }
    }
}
