//------------------------------------------------------------------------------
// <autogenerated code>
//     This code was generated by a tool.
//     Changes to this file may cause incorrect behavior and will be lost if 
//     the code is regenerated.
// </autogenerated code>
//------------------------------------------------------------------------------
// File time 18-08-05 11:16 
//
// This source code was auto-generated by WsContractFirst, Version=0.5.1.5216
// --------------------------------
// Edited 01.sep 2005 in order to save datetime information during import, because of data loss during automatic conversion to localtime.


namespace Powel.Icc.TimeSeries.TimeSeriesXML
{
    using System.Diagnostics;
    using System.Xml.Serialization;
    using System;
    using System.Web.Services.Protocols;
    using System.ComponentModel;
    using System.Web.Services;
	using Powel.Icc.Services.Time;
	using Powel.Xml;
	using System.Globalization;
    
    
    /// <summary>
    /// Time, value and quality of a time series value
    /// </summary>
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/tsws/data/1.0/", TypeName="TsPVQItemType")]
    [System.ComponentModel.TypeConverterAttribute(typeof(System.ComponentModel.ExpandableObjectConverter))]
    [Serializable()]
    public class TsPVQItemType
    {
        
        /// <remarks/>
        private UtcTime from;
        
        /// <remarks/>
        private UtcTime to;
        
        /// <remarks/>
        private bool toSpecified;
        
        /// <remarks/>
        private string duration;
        
        /// <remarks/>
        private System.Double value;
        
        /// <remarks/>
        private bool n_OK = false;
        
        /// <remarks/>
        private bool suspect = false;
        
        /// <remarks/>
        private bool manual = false;
        
        /// <remarks/>
        private bool accepted = false;
        
        /// <remarks/>
        private bool locked = false;
        
        /// <remarks/>
        private bool validated = false;
        
        /// <remarks/>
        private bool corrected = false;
        
        /// <remarks/>
        private TsValMethodType valMethod;
        
        /// <remarks/>
        private bool valMethodSpecified;
        
        /// <remarks/>
        private TsCorMethodType corMethod;
        
        /// <remarks/>
        private bool corMethodSpecified;
        
        /// <remarks/>
        private bool missing = false;

		/// <remarks/>
		private string custom;
        
        public TsPVQItemType()
        {
        }
        
        public TsPVQItemType(
                    UtcTime from, 
                    UtcTime to, 
                    bool toSpecified, 
                    string duration, 
                    System.Double value, 
                    bool n_OK, 
                    bool suspect, 
                    bool manual, 
                    bool accepted, 
                    bool locked, 
                    bool validated, 
                    bool corrected, 
                    TsValMethodType valMethod, 
                    bool valMethodSpecified, 
                    TsCorMethodType corMethod, 
                    bool corMethodSpecified, 
                    bool missing,
					string custom)
        {
            this.from = from;
            this.to = to;
            this.toSpecified = toSpecified;
            this.duration = duration;
            this.value = value;
            this.n_OK = n_OK;
            this.suspect = suspect;
            this.manual = manual;
            this.accepted = accepted;
            this.locked = locked;
            this.validated = validated;
            this.corrected = corrected;
            this.valMethod = valMethod;
            this.valMethodSpecified = valMethodSpecified;
            this.corMethod = corMethod;
            this.corMethodSpecified = corMethodSpecified;
            this.missing = missing;
			this.custom = custom;
        }
        
		//[System.Xml.Serialization.XmlAttributeAttribute(AttributeName="From")]
		[XmlIgnore()]
		public UtcTime FromDateTime
		{
			get
			{
				return this.from;
			}
			set
			{
				this.from = value;
			}
		}

		[System.Xml.Serialization.XmlAttributeAttribute(AttributeName="From")]
		public string From
		{
			get
			{
				return from.ToXmlDateTime();
			}
			set
			{
				this.from = XmlUtil.ParseDate(value);
			}
		}
        

		//[System.Xml.Serialization.XmlAttributeAttribute(AttributeName="To")]
		[XmlIgnore()]
		public UtcTime ToDateTime
		{
			get
			{
				return this.to;
			}
			set
			{
				this.to = value;
			}
		}

		[System.Xml.Serialization.XmlAttributeAttribute(AttributeName="To")]
		public string To
		{
			get
			{
				return to.ToXmlDateTime();
			}
			set
			{
				this.to = XmlUtil.ParseDate(value);
			}
		}
        
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool ToSpecified
        {
            get
            {
                return this.toSpecified;
            }
            set
            {
                this.toSpecified = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(DataType="duration", AttributeName="Duration")]
        public string Duration
        {
            get
            {
                return this.duration;
            }
            set
            {
                this.duration = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="Value")]
        public System.Double Value
        {
            get
            {
                return this.value;
            }
            set
            {
                this.value = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="N_OK")]
        [System.ComponentModel.DefaultValueAttribute(false)]
        public bool N_OK
        {
            get
            {
                return this.n_OK;
            }
            set
            {
                this.n_OK = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="Suspect")]
        [System.ComponentModel.DefaultValueAttribute(false)]
        public bool Suspect
        {
            get
            {
                return this.suspect;
            }
            set
            {
                this.suspect = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="Manual")]
        [System.ComponentModel.DefaultValueAttribute(false)]
        public bool Manual
        {
            get
            {
                return this.manual;
            }
            set
            {
                this.manual = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="Accepted")]
        [System.ComponentModel.DefaultValueAttribute(false)]
        public bool Accepted
        {
            get
            {
                return this.accepted;
            }
            set
            {
                this.accepted = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="Locked")]
        [System.ComponentModel.DefaultValueAttribute(false)]
        public bool Locked
        {
            get
            {
                return this.locked;
            }
            set
            {
                this.locked = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="Validated")]
        [System.ComponentModel.DefaultValueAttribute(false)]
        public bool Validated
        {
            get
            {
                return this.validated;
            }
            set
            {
                this.validated = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="Corrected")]
        [System.ComponentModel.DefaultValueAttribute(false)]
        public bool Corrected
        {
            get
            {
                return this.corrected;
            }
            set
            {
                this.corrected = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="ValMethod")]
        public TsValMethodType ValMethod
        {
            get
            {
                return this.valMethod;
            }
            set
            {
                this.valMethod = value;
            }
        }
        
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool ValMethodSpecified
        {
            get
            {
                return this.valMethodSpecified;
            }
            set
            {
                this.valMethodSpecified = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="CorMethod")]
        public TsCorMethodType CorMethod
        {
            get
            {
                return this.corMethod;
            }
            set
            {
                this.corMethod = value;
            }
        }
        
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool CorMethodSpecified
        {
            get
            {
                return this.corMethodSpecified;
            }
            set
            {
                this.corMethodSpecified = value;
            }
        }
        
        [System.Xml.Serialization.XmlAttributeAttribute(AttributeName="Missing")]
        [System.ComponentModel.DefaultValueAttribute(false)]
        public bool Missing
        {
            get
            {
                return this.missing;
            }
            set
            {
                this.missing = value;
            }
        }

		[System.Xml.Serialization.XmlAttributeAttribute(DataType="integer", AttributeName="Custom")]
		public string Custom
		{
			get
			{
				return this.custom;
			}
			set
			{
				this.custom = value;
			}
		}
    }
}
