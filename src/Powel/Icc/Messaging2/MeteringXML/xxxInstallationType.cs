namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class InstallationType
    {
        
        private string installationIDField;
        
        private string oldInstallationIDField;
        
        private string installationDescriptionField;
        
        private string secondaryInstallationIDField;
        
        private string installationCodeField;
        
        private string customerNameField;
        
        private string customerIDField;
        
        private string appartmentNumberField;
        
        private string houseNumberField;
        
        private string addressMainField;
        
        private string coInfoMainField;
        
        private string zipCodeMainField;
        
        private string cityMainField;
        
        private string addressContactField;
        
        private string coInfoContactField;
        
        private string zipCodeContactField;
        
        private string cityContactField;
        
        private string phoneHomeField;
        
        private string phoneWorkField;
        
        private string phoneMobileField;
        
        private System.Collections.Generic.List<MeasurePointType> measurePointsField;
        
        /// <remarks/>
        public string installationID
        {
            get
            {
                return this.installationIDField;
            }
            set
            {
                this.installationIDField = value;
            }
        }
        
        /// <remarks/>
        public string oldInstallationID
        {
            get
            {
                return this.oldInstallationIDField;
            }
            set
            {
                this.oldInstallationIDField = value;
            }
        }
        
        /// <remarks/>
        public string installationDescription
        {
            get
            {
                return this.installationDescriptionField;
            }
            set
            {
                this.installationDescriptionField = value;
            }
        }
        
        /// <remarks/>
        public string secondaryInstallationID
        {
            get
            {
                return this.secondaryInstallationIDField;
            }
            set
            {
                this.secondaryInstallationIDField = value;
            }
        }
        
        /// <remarks/>
        public string installationCode
        {
            get
            {
                return this.installationCodeField;
            }
            set
            {
                this.installationCodeField = value;
            }
        }
        
        /// <remarks/>
        public string customerName
        {
            get
            {
                return this.customerNameField;
            }
            set
            {
                this.customerNameField = value;
            }
        }
        
        /// <remarks/>
        public string customerID
        {
            get
            {
                return this.customerIDField;
            }
            set
            {
                this.customerIDField = value;
            }
        }
        
        /// <remarks/>
        public string appartmentNumber
        {
            get
            {
                return this.appartmentNumberField;
            }
            set
            {
                this.appartmentNumberField = value;
            }
        }
        
        /// <remarks/>
        public string houseNumber
        {
            get
            {
                return this.houseNumberField;
            }
            set
            {
                this.houseNumberField = value;
            }
        }
        
        /// <remarks/>
        public string addressMain
        {
            get
            {
                return this.addressMainField;
            }
            set
            {
                this.addressMainField = value;
            }
        }
        
        /// <remarks/>
        public string coInfoMain
        {
            get
            {
                return this.coInfoMainField;
            }
            set
            {
                this.coInfoMainField = value;
            }
        }
        
        /// <remarks/>
        public string zipCodeMain
        {
            get
            {
                return this.zipCodeMainField;
            }
            set
            {
                this.zipCodeMainField = value;
            }
        }
        
        /// <remarks/>
        public string cityMain
        {
            get
            {
                return this.cityMainField;
            }
            set
            {
                this.cityMainField = value;
            }
        }
        
        /// <remarks/>
        public string addressContact
        {
            get
            {
                return this.addressContactField;
            }
            set
            {
                this.addressContactField = value;
            }
        }
        
        /// <remarks/>
        public string coInfoContact
        {
            get
            {
                return this.coInfoContactField;
            }
            set
            {
                this.coInfoContactField = value;
            }
        }
        
        /// <remarks/>
        public string zipCodeContact
        {
            get
            {
                return this.zipCodeContactField;
            }
            set
            {
                this.zipCodeContactField = value;
            }
        }
        
        /// <remarks/>
        public string cityContact
        {
            get
            {
                return this.cityContactField;
            }
            set
            {
                this.cityContactField = value;
            }
        }
        
        /// <remarks/>
        public string phoneHome
        {
            get
            {
                return this.phoneHomeField;
            }
            set
            {
                this.phoneHomeField = value;
            }
        }
        
        /// <remarks/>
        public string phoneWork
        {
            get
            {
                return this.phoneWorkField;
            }
            set
            {
                this.phoneWorkField = value;
            }
        }
        
        /// <remarks/>
        public string phoneMobile
        {
            get
            {
                return this.phoneMobileField;
            }
            set
            {
                this.phoneMobileField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlArrayItemAttribute("measurePoint", IsNullable=false)]
        public System.Collections.Generic.List<MeasurePointType> measurePoints
        {
            get
            {
                return this.measurePointsField;
            }
            set
            {
                this.measurePointsField = value;
            }
        }
        
        public virtual bool ShouldSerializemeasurePoints()
        {
            return ((this.measurePoints != null) 
                        && (this.measurePoints.Count > 0));
        }
    }
}
