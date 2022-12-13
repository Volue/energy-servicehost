using System;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Configuration;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using Powel.Icc.Data;
using Message = System.ServiceModel.Channels.Message;

namespace Powel.Icc.Messaging2
{
    public class QueueMessageInvoker : IDispatchMessageInspector 
    {
        //The implementation of DispatchMessageInspector; Omitted
        public object AfterReceiveRequest(ref Message request, IClientChannel channel, InstanceContext instanceContext)
        {
            var action = request.Headers.Action;
            var actions = action.Split('/');
            var operation = actions[actions.GetUpperBound(0)];
            var version = actions[actions.GetUpperBound(0) - 2];

            QueueMessageType messageType;
            switch (operation)
            {
                case "submitInstallationToQueue":
                    messageType = QueueMessageType.METERING_SERVICE_INSTALLATION;
                    break;
                case "submitComponentsToQueue":
                    messageType = QueueMessageType.METERING_SERVICE_COMPONENTS;
                    break;
                case "submitRegistersToQueue":
                    messageType = QueueMessageType.METERING_SERVICE_REGISTERS;
                    break;
                case "submitExportsToQueue":
                    messageType = QueueMessageType.METERING_SERVICE_EXPORTS;
                    break;
                default:
                    // Will not do any queing for other message types
                    return null;
            }

            try
            {
                var msg = request.ToString();
//                msg = msg.Replace("</validFrom>", "+01:00</validFrom>");
                MessageData.EnqueueInputMessage(msg, messageType, version);
            }
            catch (Exception e)
            {   
                Console.WriteLine(@"Something failed during message queing.");
                Console.WriteLine(@"Error; {0}", e.Message);
                throw new FaultException<Exception>(e, e.Message);
            }

            return null;
        }

        public void BeforeSendReply(ref Message reply, object correlationState)
        {
        }
    }
    public class QueueMessageBehavior : IEndpointBehavior
    {
        public void Validate(ServiceEndpoint endpoint)
        {
            return;
        }

        public void AddBindingParameters(ServiceEndpoint endpoint, BindingParameterCollection bindingParameters)
        {
            return;
        }

        public void ApplyDispatchBehavior(ServiceEndpoint endpoint, EndpointDispatcher endpointDispatcher)
        {
            var extension = new QueueMessageInvoker();
            endpointDispatcher.DispatchRuntime.MessageInspectors.Add(extension);
        }

        public void ApplyClientBehavior(ServiceEndpoint endpoint, ClientRuntime clientRuntime)
        {
            throw new Exception("Behavior not supported on the consumer side.");
        }
    }
    public class QueueMessageExtension : BehaviorExtensionElement
    {
        protected override object CreateBehavior()
        {
            return new QueueMessageBehavior();
        }

        public override Type BehaviorType
        {
            get { return typeof(QueueMessageBehavior); }
        }
    }
}
