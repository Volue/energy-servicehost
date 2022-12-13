using System;
using System.Xml.Serialization;
using Powel.Icc.Common;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(WrapperName="sendTimeseriesGS2", WrapperNamespace="http://www.powel.no/icc/ws/metering/messages/1.5/", IsWrapped=true)]
    public partial class sendTimeseriesGS2Request
    {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/", Order=0)]
        public string messageID;

        private UtcTime ValidFrom;
        [XmlIgnore]
        public UtcTime validFrom
        {
            get
            {
                return this.ValidFrom;
            }
            set
            {
                if ((this.ValidFrom != value))
                {
                    this.ValidFrom = value;
                }
            }
        }

        [System.ServiceModel.MessageBodyMemberAttribute(Namespace = "http://www.powel.no/icc/ws/metering/messages/1.5/", Order = 1, Name = "validFrom")]
        [XmlElement(ElementName = "validFrom", Namespace = "http://www.powel.no/icc/ws/metering/messages/1.5/")]
        public DateTime ValidFromXml
        {
            get
            {
                return IccConfiguration.Time.DatabaseCalendar.ToDateTime(this.ValidFrom);
            }
            set
            {
                this.ValidFrom = IccConfiguration.Time.DatabaseCalendar.ToUtcTime(value);
            }
        } 
       
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/", Order=2)]
        public string timeseriesGS2;
        
        public sendTimeseriesGS2Request()
        {
        }
        
        public sendTimeseriesGS2Request(string messageID, UtcTime validFrom, string timeseriesGS2)
        {
            this.messageID = messageID;
            this.validFrom = validFrom;
            this.timeseriesGS2 = timeseriesGS2;
        }
    }
}
