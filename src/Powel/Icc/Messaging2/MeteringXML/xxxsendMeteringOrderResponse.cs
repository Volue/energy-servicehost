namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(WrapperName="sendMeteringOrderResponse", WrapperNamespace="http://www.powel.no/icc/ws/metering/messages/1.5/", IsWrapped=true)]
    public partial class sendMeteringOrderResponse
    {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/", Order=0)]
        public string messageID;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/", Order=1)]
        public string referringMessageID;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/", Order=2)]
        public StatusType status;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/", Order=3)]
        [System.Xml.Serialization.XmlElementAttribute(DataType="integer")]
        public string statusCode;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/data/1.5/", Order=4)]
        public string statusMessage;
        
        public sendMeteringOrderResponse()
        {
        }
        
        public sendMeteringOrderResponse(string messageID, string referringMessageID, StatusType status, string statusCode, string statusMessage)
        {
            this.messageID = messageID;
            this.referringMessageID = referringMessageID;
            this.status = status;
            this.statusCode = statusCode;
            this.statusMessage = statusMessage;
        }
    }
}
