//------------------------------------------------------------------------------
// <autogenerated code>
//     This code was generated by a tool.
//     Changes to this file may cause incorrect behavior and will be lost if 
//     the code is regenerated.
// </autogenerated code>
//------------------------------------------------------------------------------
// File time 07-09-05 02:20 
//
// This source code was auto-generated by WsContractFirst, Version=0.5.1.5216


namespace Powel.Icc.TimeSeries.TimeSeriesXML
{
    using System.Diagnostics;
    using System.Xml.Serialization;
    using System;
    using System.Web.Services.Protocols;
    using System.ComponentModel;
    using System.Web.Services;
    
    
    /// <remarks/>
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/tsws/enums/1.0/", TypeName="TsValMethodType")]
    [Serializable()]
    public enum TsValMethodType
    {
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="ABSOLUTE_LIMIT")]
        ABS_LIMIT,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="DELTA_LIMIT")]
        DELTA_LIMIT,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="DELTA_CHANGE")]
        DELTA_CHANGE,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="REPEAT_VALUE")]
        REP_VALUE,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="CONTROL_METER")]
        CTRL_METER,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="METER_SUM")]
        VAL_METER_SUM,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="CONTROL_HIGH_TARIFF_PERIOD")]
        CTRL_HT,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="CONTROL_TARIFF")]
        CTRL_T,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="CONTROL_ACCESSIBLE_ENERGY")]
        CTRL_U,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="CONTROL_IMPORT")]
        CTRL_IMPORT,
    }
}