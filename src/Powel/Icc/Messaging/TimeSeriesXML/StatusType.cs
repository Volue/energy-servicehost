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
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.powel.no/icc/ws/tsws/enums/1.0/", TypeName="StatusType")]
    [Serializable()]
    public enum StatusType
    {
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="OK")]
        OK,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute("PARTLY OK")]
        PARTLYOK,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute(Name="FAILED")]
        FAILED,
    }
}
