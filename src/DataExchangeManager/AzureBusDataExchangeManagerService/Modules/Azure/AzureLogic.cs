using System;
using System.Linq.Expressions;
using System.Text.RegularExpressions;
using Microsoft.ServiceBus.Messaging;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic;

namespace Powel.Icc.Messaging.AzureBusDataExchangeManager.AzureBusDataExchangeManagerService.Modules.Azure
{
    public class AzureLogic : IWsLogicBase
    {
        private const string RoutingAddress = "AZURE:";
        private const string OutputFormatProperty = "Output_Format";
        private const string SendercodeProperty = "SenderCode";
        private const string ReceiverCodeProperty = "ReceiverCode";
        private const string BusinessTypeProperty = "BusinessType";
        private const string ProductCodeProperty = "ProductCode";
        private const string SenderCountryProperty = "SenderCountry";

        private const string NPFormatPrefix = "NP";
        private const string AperakProtocol = "APERAK";
        private const string UtiltsProtocol = "UTILTS";
        private const string EdiElProtocol = "EDIEL" + AperakProtocol;
        private const string UtilTsProtocol = "UTILTS" + AperakProtocol;
        private readonly IServiceEventLogger _serviceEventLogger;
        private readonly IImportApplication _importApplication;

        public AzureLogic(IServiceEventLogger serviceEventLogger, IImportApplication importApplication)
        {
            _serviceEventLogger = serviceEventLogger;
            _importApplication = importApplication;
        }

        public void SubmitImport(BrokeredMessage metaData, string payload)
        {
            var message = BuildDataExchangeImportMessage(metaData, payload);
            SubmitImport(message, null);
        }
        public void SubmitImport(BrokeredMessage metaData, string payload, Action<bool> onCompletion)
        {
            var message = BuildDataExchangeImportMessage(metaData, payload);
            SubmitImport(message, onCompletion);
            message.DeleteMessageData();
        }
        public void SubmitImport(DataExchangeImportMessage message, Action<bool> onCompletion)
        {
            _importApplication.StartImportJobImmediately(message, onCompletion);            
        }
        public void SubmitImport(DataExchangeImportMessage message)
        {            
            _importApplication.StartImportJobImmediately(message, null);            
        }

        public DataExchangeImportMessage BuildDataExchangeImportMessage(BrokeredMessage azMessage, string payload)
        {
            var dataExchangeMessage = new DataExchangeImportMessage
            {
                Version = DataExchangeImportMessage.LatestVersion,
                ExternalReference = azMessage.CorrelationId,
                Priority = GetPriority(azMessage, payload).ToString(),
                Protocol = GetProtocol(azMessage, payload),
                RoutingAddress = RoutingAddress
            };
            dataExchangeMessage.SetMessageData(payload, null);
            // For acknowledgment and response routing out.
            MapToMessage(m => m.SenderName, azMessage, dataExchangeMessage, SendercodeProperty);
            MapToMessage(m => m.ReceiverName, azMessage, dataExchangeMessage, ReceiverCodeProperty);
            MapToMessage(m => m.SubAddress, azMessage, dataExchangeMessage, BusinessTypeProperty);
            MapToMessage(m => m.ProductCode, azMessage, dataExchangeMessage, ProductCodeProperty);
            MapToMessage(m => m.Country, azMessage, dataExchangeMessage, SenderCountryProperty);
            return dataExchangeMessage;
        }

        private void MapToMessage(Expression<Func<DataExchangeImportMessage, string>> func, BrokeredMessage azMessage, DataExchangeImportMessage message, string propertyName)
        {
            object obj;
            if (azMessage.Properties.TryGetValue(propertyName, out obj))
            {
                func.GetPropertyInfoFromExpression().SetValue(message, obj);
            }
        }

        private DataExchangeQueuePriority GetPriority(BrokeredMessage metaData, string payload)
        {
            if (payload == null) throw new ArgumentNullException(nameof(payload));
            object obj;
            return metaData.Properties.TryGetValue("Priority", out obj) ? DataExchangeQueuePriorityConverter.FromString(obj.ToString()) : DataExchangeQueuePriority.Normal;
        }

        private string GetProtocol(BrokeredMessage metaData,string payload)
        {
            object obj;
            if (!metaData.Properties.TryGetValue(OutputFormatProperty, out obj))
                throw new MissingFieldException(ExpressionHelper.GetPropertyName(metaData, m=>m.Properties), OutputFormatProperty);
            var protocol = obj.ToString();
            if (protocol == UtiltsProtocol)
            {
                var re = new Regex(@"PD00 UTILTS.*\nPD01 S08");
                var match = re.Match(payload);
                if (match.Success)
                    return DataExchangeMessageBase.ProtocolUtiltsS08;
            }

            return protocol != AperakProtocol
                ? protocol
                : IsNPFormat(payload)
                    ? EdiElProtocol
                    : UtilTsProtocol;
        }

        private static bool IsNPFormat(string payload)
        {
            return payload.Substring(0, 2) == NPFormatPrefix;
        }

        public void LogError(string error)
        {
            _serviceEventLogger.LogMessage(8100, error);
        }

        public void LogError(int msgKey, params string[] errorArgs)
        {
            _serviceEventLogger.LogMessage(msgKey, errorArgs);
        }
    }
}
