using System;
using System.Text.RegularExpressions;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;
using Microsoft.ServiceBus.Messaging;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure
{
    public class AzureLogic : WsLogicBase
    {
        public AzureLogic(Func<IDataExchangeApi> dataExchangeApiFactory, IServiceEventLogger serviceEventLogger)
            : base(dataExchangeApiFactory, serviceEventLogger)
        {
        }

        public bool SubmitImport(BrokeredMessage MetaData, string Payload)
        {
            var message = new DataExchangeImportMessage()
            {
                Version = DataExchangeImportMessage.LatestVersion,
                ExternalReference = MetaData.CorrelationId,
            };
            message.SetMessageData(Payload, null);
            message.Priority = GetPriority(MetaData, Payload).ToString();
            message.Protocol = GetProtocol(MetaData, Payload);
            message.RoutingAddress = GetRoutingAddress(MetaData,Payload);   // For acknowledgment and response routing out.
            object obj;
            if (MetaData.Properties.TryGetValue("SenderCode",out obj))
                message.SenderName = obj.ToString();
            if (MetaData.Properties.TryGetValue("ReceiverCode",out obj))
                message.ReceiverName = obj.ToString();
            if (MetaData.Properties.TryGetValue("BusinessType",out obj))
                message.SubAddress = obj.ToString();
            if (MetaData.Properties.TryGetValue("ProductCode", out obj))
                message.ProductCode = obj.ToString();
            if (MetaData.Properties.TryGetValue("SenderCountry", out obj))
                message.Country = obj.ToString();
            return SubmitImport(message);
        }

        private DataExchangeQueuePriority GetPriority(BrokeredMessage MetaData,string Payload)
        {
            Object obj;
            return MetaData.Properties.TryGetValue("Priority", out obj) ? DataExchangeQueuePriorityConverter.FromString(obj.ToString()) : DataExchangeQueuePriority.Normal;
        }

        private string GetProtocol(BrokeredMessage MetaData,string Payload)
        {
            Object obj;
            const string prop = "Output_Format";
            if (MetaData.Properties.TryGetValue(prop, out obj))
            {
                if (obj.ToString() == "APERAK")
                {
                    if (Payload.Substring(0, 2) == "NP")
                        return "EDIELAPERAK";
                    return "UTILTSAPERAK";
                }
                if (obj.ToString() == "UTILTS")
                {
                    var re = new Regex(@"PD00 UTILTS.*\nPD01 S08");
                    var match = re.Match(Payload);
                    if (match.Success)
                        return DataExchangeMessageBase.ProtocolUtiltsS08;
                }
                return obj.ToString();
            }
            throw new MissingFieldException("BrokeredMessage.Properties", prop);
        }

        private string GetRoutingAddress(BrokeredMessage MetaData,string Payload)
        {
            return "AZURE:";
        }
    }
}
