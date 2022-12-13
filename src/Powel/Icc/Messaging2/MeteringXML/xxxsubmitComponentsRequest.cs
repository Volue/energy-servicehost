using System;
using System.Xml.Serialization;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(WrapperName="submitComponents", WrapperNamespace="http://www.powel.no/icc/ws/metering/messages/1.5/", IsWrapped=true)]
    public partial class submitComponentsRequest
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

        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/", Order=1, Name="validFrom")]
        [XmlElement(ElementName = "validFrom", Namespace = "http://www.powel.no/icc/ws/metering/messages/1.5/")]
        public DateTime ValidFromXml
        {
            get
            {
                return RegionalCalendar.UtcCalendar.ToDateTime(this.ValidFrom);
            }
            set
            {
                DateTime date = value.ToUniversalTime();
                this.ValidFrom = RegionalCalendar.UtcCalendar.ToUtcTime(date);
            }
        }
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/", Order=2)]
        public ActionType action;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/", Order=3)]
        public ComponentListType components;

        [System.ServiceModel.MessageBodyMemberAttribute(Namespace = "http://www.powel.no/icc/ws/metering/messages/1.5/", Order = 4)]
        public SecurityType security;
        
        public submitComponentsRequest()
        {
        }
        
        public submitComponentsRequest(string messageID, UtcTime validFrom, ActionType action, ComponentListType components, SecurityType serurity)
        {
            this.messageID = messageID;
            this.validFrom = validFrom;
            this.action = action;
            this.components = components;
            this.security = security;
        }
    }
}
