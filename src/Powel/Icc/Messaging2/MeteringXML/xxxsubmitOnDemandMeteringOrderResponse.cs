namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(WrapperName="submitOnDemandMeteringOrderResponse", WrapperNamespace="http://www.powel.no/icc/ws/metering/messages/1.5/", IsWrapped=true)]
    public partial class submitOnDemandMeteringOrderResponse
    {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/", Order=0)]
        public string messageID;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/", Order=1)]
        public string referringMessageID;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/", Order=2)]
        [System.Xml.Serialization.XmlArrayItemAttribute("status", IsNullable=false)]
        public System.Collections.Generic.List<StatusItemType> statuses;
        
        public submitOnDemandMeteringOrderResponse()
        {
        }
        
        public submitOnDemandMeteringOrderResponse(string messageID, string referringMessageID, System.Collections.Generic.List<StatusItemType> statuses)
        {
            this.messageID = messageID;
            this.referringMessageID = referringMessageID;
            this.statuses = statuses;
        }
        
        public virtual bool ShouldSerializestatuses()
        {
            return ((this.statuses != null) 
                        && (this.statuses.Count > 0));
        }
    }
}
