namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.5420")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/")]
    public partial class MeasurePointType
    {
        
        private string measurePointIDField;
        
        private string oldMeasurePointIDField;
        
        private string measurePointDescriptionField;
        
        private UtilityType utilityField;
        
        private string netAreaCodeField;
        
        private string secondaryMeasurePointIDField;
        
        private float xCoordinateField;
        
        private bool xCoordinateFieldSpecified;
        
        private float yCoordinateField;
        
        private bool yCoordinateFieldSpecified;
        
        private string stationIDField;
        
        private int numberOfPhasesField;
        
        private bool numberOfPhasesFieldSpecified;
        
        private string supplyVoltageField;
        
        private string fuseSizeField;
        
        private int yearlyVolumeField;
        
        private bool yearlyVolumeFieldSpecified;
        
        private int subscribedPowerField;
        
        private bool subscribedPowerFieldSpecified;
        
        private string orgUnitField;
        
        private string gridOwnerField;
        
        private string loadProfileField;
        
        private string locationField;

        private string supplierField;

        private string centralNetPointCodeField;

        private MeasPntStateType stateField;

        private bool stateFieldSpecified;
        
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
        public string oldMeasurePointID
        {
            get
            {
                return this.oldMeasurePointIDField;
            }
            set
            {
                this.oldMeasurePointIDField = value;
            }
        }
        
        /// <remarks/>
        public string measurePointDescription
        {
            get
            {
                return this.measurePointDescriptionField;
            }
            set
            {
                this.measurePointDescriptionField = value;
            }
        }
        
        /// <remarks/>
        public UtilityType utility
        {
            get
            {
                return this.utilityField;
            }
            set
            {
                this.utilityField = value;
            }
        }
        
        /// <remarks/>
        public string netAreaCode
        {
            get
            {
                return this.netAreaCodeField;
            }
            set
            {
                this.netAreaCodeField = value;
            }
        }
        
        /// <remarks/>
        public string secondaryMeasurePointID
        {
            get
            {
                return this.secondaryMeasurePointIDField;
            }
            set
            {
                this.secondaryMeasurePointIDField = value;
            }
        }
        
        /// <remarks/>
        public float xCoordinate
        {
            get
            {
                return this.xCoordinateField;
            }
            set
            {
                this.xCoordinateField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool xCoordinateSpecified
        {
            get
            {
                return this.xCoordinateFieldSpecified;
            }
            set
            {
                this.xCoordinateFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public float yCoordinate
        {
            get
            {
                return this.yCoordinateField;
            }
            set
            {
                this.yCoordinateField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool yCoordinateSpecified
        {
            get
            {
                return this.yCoordinateFieldSpecified;
            }
            set
            {
                this.yCoordinateFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public string stationID
        {
            get
            {
                return this.stationIDField;
            }
            set
            {
                this.stationIDField = value;
            }
        }
        
        /// <remarks/>
        public int numberOfPhases
        {
            get
            {
                return this.numberOfPhasesField;
            }
            set
            {
                this.numberOfPhasesField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool numberOfPhasesSpecified
        {
            get
            {
                return this.numberOfPhasesFieldSpecified;
            }
            set
            {
                this.numberOfPhasesFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public string supplyVoltage
        {
            get
            {
                return this.supplyVoltageField;
            }
            set
            {
                this.supplyVoltageField = value;
            }
        }
        
        /// <remarks/>
        public string fuseSize
        {
            get
            {
                return this.fuseSizeField;
            }
            set
            {
                this.fuseSizeField = value;
            }
        }
        
        /// <remarks/>
        public int yearlyVolume
        {
            get
            {
                return this.yearlyVolumeField;
            }
            set
            {
                this.yearlyVolumeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool yearlyVolumeSpecified
        {
            get
            {
                return this.yearlyVolumeFieldSpecified;
            }
            set
            {
                this.yearlyVolumeFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public int subscribedPower
        {
            get
            {
                return this.subscribedPowerField;
            }
            set
            {
                this.subscribedPowerField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool subscribedPowerSpecified
        {
            get
            {
                return this.subscribedPowerFieldSpecified;
            }
            set
            {
                this.subscribedPowerFieldSpecified = value;
            }
        }
        
        /// <remarks/>
        public string orgUnit
        {
            get
            {
                return this.orgUnitField;
            }
            set
            {
                this.orgUnitField = value;
            }
        }
        
        /// <remarks/>
        public string gridOwner
        {
            get
            {
                return this.gridOwnerField;
            }
            set
            {
                this.gridOwnerField = value;
            }
        }
        
        /// <remarks/>
        public string loadProfile
        {
            get
            {
                return this.loadProfileField;
            }
            set
            {
                this.loadProfileField = value;
            }
        }
        
        /// <remarks/>
        public string location
        {
            get
            {
                return this.locationField;
            }
            set
            {
                this.locationField = value;
            }
        }

        /// <remarks/>
        public string supplier
        {
            get
            {
                return this.supplierField;
            }
            set
            {
                this.supplierField = value;
            }
        }

        /// <remarks/>
        public string centralNetPointCode
        {
            get
            {
                return this.centralNetPointCodeField;
            }
            set
            {
                this.centralNetPointCodeField = value;
            }
        }

        /// <remarks/>
        public MeasPntStateType state
        {
            get { return this.stateField; }
            set { this.stateField = value; }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool stateSpecified
        {
            get
            {
                return this.stateFieldSpecified;
            }
            set
            {
                this.stateFieldSpecified = value;
            }
        }
    }
}
