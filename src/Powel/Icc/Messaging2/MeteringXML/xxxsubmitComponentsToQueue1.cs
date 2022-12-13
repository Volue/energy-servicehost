using Powel.Icc.Services.Time;

namespace Powel.Icc.Messaging2.MeteringXML
{
    
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(WrapperName="submitComponentsToQueue", WrapperNamespace="http://www.powel.no/icc/ws/metering/messages/1.5/", IsWrapped=true)]
    public partial class submitComponentsToQueue1
    {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/", Order=0)]
        public string messageID;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/", Order=1)]
        public UtcTime validFrom;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/", Order=2)]
        public ActionType action;
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.powel.no/icc/ws/metering/messages/1.5/", Order=3)]
        public ComponentListType components;
        
        public submitComponentsToQueue1()
        {
        }
        
        public submitComponentsToQueue1(string messageID, UtcTime validFrom, ActionType action, ComponentListType components)
        {
            this.messageID = messageID;
            this.validFrom = validFrom;
            this.action = action;
            this.components = components;
        }
    }
}
