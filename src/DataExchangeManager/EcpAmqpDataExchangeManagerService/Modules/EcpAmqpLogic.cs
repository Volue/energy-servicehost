using System;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Text.RegularExpressions;
using Amqp;
using Microsoft.Practices.ObjectBuilder2;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Settings;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Modules
{
    public class EcpAmqpLogic : WsLogicBase
    {
        public bool UseImportApplicationManagerService { get; set; } = false;
        public bool UseEdx { get; set; } = false;
        private struct QueryPriority
        {
            public Regex query; // E.g. <DocumentType v="A40"
            public DataExchangeQueuePriority priority; // E.g. "Low"
        };
        private List<QueryPriority> _prioritizedQueries;

        private struct QueryProtocol
        {
            public Regex query;
            public string protocol;
        };
        private List<QueryProtocol> _protocolQueries;

        private Dictionary<string, int> _states;

        private readonly IServiceEventLogger _serviceEventLogger;
        private readonly IImportApplication _importApplication;

        public EcpAmqpLogic(Func<IDataExchangeApi> dataExchangeApiFactory,IServiceEventLogger serviceEventLogger, IImportApplication importApplication)
            : base(dataExchangeApiFactory,serviceEventLogger)
        {
            _serviceEventLogger = serviceEventLogger;
            _importApplication = importApplication;

            InitializePriorityList(null);
            InitializeProtocolRules(null);
            InitializeStateRules(null);
        }

        public DataExchangeImportMessage BuildDataExchangeImportMessage(Message message, string payload)
        {
            var dataExchangeMessage = new DataExchangeImportMessage
            {
                Version = DataExchangeMessageBase.LatestVersion,
                ExternalReference = message.Properties.CorrelationId,
                Priority = GetPriority(message, payload).ToString(),
                Protocol = GetProtocol(message, payload),
                RoutingAddress = GetRoutingAddress(message)
            };
            dataExchangeMessage.SetMessageData(payload, null);
            // For acknowledgment and response routing out.
            //MapToMessage(m => m.SenderName, azMessage, dataExchangeMessage, SendercodeProperty);
            MapToMessage(m => m.ReceiverName, message, dataExchangeMessage, MessageReaderExtensions.ApplicationPropertyKeys.ReceiverCode);
            //MapToMessage(m => m.ProductCode, azMessage, dataExchangeMessage, ProductCodeProperty);
            //MapToMessage(m => m.Country, azMessage, dataExchangeMessage, SenderCountryProperty);
            MapToMessage(m => m.SubAddress, message, dataExchangeMessage, UseEdx ? MessageReaderExtensions.ApplicationPropertyKeys.BusinessType : MessageReaderExtensions.ApplicationPropertyKeys.MessageType);   // Used by XmlElImp
            return dataExchangeMessage;
        }

        private void MapToMessage(Expression<Func<DataExchangeImportMessage, string>> func, Message azMessage, DataExchangeImportMessage message, string propertyName)
        {
            object obj;
            if (azMessage.ApplicationProperties.Map.TryGetValue(propertyName, out obj))
            {
                func.GetPropertyInfoFromExpression().SetValue(message, obj);
            }
        }

        private string GetRoutingAddress(Message ecpMessage)
        {
            // On Import: For acknowledgement and response routing out.
            // ToDo: This does not match the wanted structure in the Export module. There a senderCode is wanted. BMO(20200807) says in a mail it is not used for the time being.
            return "ECPAMQP:" + ecpMessage.GetBusinessType(); // senderCode;   // Format: <NameOfExportQueue>:<EICaddress>   Given in Participant.Communication info on export.
        }

        public void InitializePriorityList(string priorityDescription)
        {
            // Initialize document priority specification from config variable
            _prioritizedQueries = new List<QueryPriority>();
            if (string.IsNullOrEmpty(priorityDescription))
                return;

            var queryPrioL = priorityDescription.Split(';');
            foreach (var queryPrio in queryPrioL)
            {
                var qpS = queryPrio.Split(',');
                try
                {
                    var reQ = new Regex(qpS[0]);
                    var qp = new QueryPriority() { query = reQ, priority = DataExchangeQueuePriorityConverter.FromString(qpS[1]) };
                    if (qp.priority != DataExchangeQueuePriority.Undefined)
                        _prioritizedQueries.Add(qp);
                }
                catch (ArgumentException E)
                {
                    LogError(E + qpS[0]);
                }
            }
        }

        private DataExchangeQueuePriority GetPriority(Message metaData, string payload)
        {
            // Decide priority from externally specified query
            foreach (var prioritizedQuery in _prioritizedQueries)
            {
                if (prioritizedQuery.query.IsMatch(payload))
                    return prioritizedQuery.priority;
                foreach (var kvp in metaData.ApplicationProperties.Map)
                {
                    if (prioritizedQuery.query.IsMatch(kvp.Value.ToString()))
                        return prioritizedQuery.priority;
                }
            }
            return DataExchangeQueuePriority.Normal;
        }

        public void InitializeProtocolRules(string protocolRulesDefinition)
        {
            _protocolQueries = new List<QueryProtocol>();
            if (string.IsNullOrEmpty(protocolRulesDefinition))
                return;

            var queryProtL = protocolRulesDefinition.Split(new char[] {';'});
            foreach (var queryProt in queryProtL)
            {
                var qpS = queryProt.Split(new char[] {','});
                try
                {
                    var reQ = new Regex(qpS[0]);
                    var qp = new QueryProtocol() {query = reQ,protocol = qpS[1]};
                    _protocolQueries.Add(qp);
                }
                catch (ArgumentException E)
                {
                    // Invalid user defined RE
                    LogError(E + qpS[0]);
                    _serviceEventLogger.LogMessage(Constants.MessageIdentifiers.AnErrorOccurred,E.Message); // An error occurred. The error was: %01
                }
            }
        }

        private string GetProtocol(Message metaData, string payload)
        {
            foreach (var protocolQuery in _protocolQueries)
            {
                if (protocolQuery.query.IsMatch(payload))
                    return protocolQuery.protocol;
            }
            return "ENTSOE";
        }

        public void InitializeStateRules(string stateRulesDefinition)
        {
            _states = new Dictionary<string, int>();
            if (string.IsNullOrEmpty(stateRulesDefinition))
                return;

            stateRulesDefinition.Split(new char[] { ';' }).ForEach(state =>
            {
               var conv = state.Split(new char[] {','});
                _states.Add(conv[0], Convert.ToInt32(conv[1]));
            });
        }

        public override void SubmitImport(DataExchangeImportMessage message, Action<bool> onCompletion)
        {
            if (UseImportApplicationManagerService)
                base.SubmitImport(message, onCompletion);
            else
            {
                //_importApplication.StartImportJobImmediately(message, onCompletion, 
                var ret = _importApplication.HandleMessageWithExceptionHandling(() => 
                    {
                        var lRet = _importApplication.HandleMessage(message);
                        message.DeleteMessageData();
                        return lRet;
                    },
                    onImportRunnerFailed: (exc) =>
                    {
                        message.DeleteMessageData();    // Not put on Error queue. further handling from MessageLogSearch
                    },
                    onTimeout: (exc) =>
                    {
                        message.DeleteMessageData();    // Not put on Error queue. further handling from MessageLogSearch
                    },
                    onErrorCaught:() =>
                    {
                        var api = _dataExchangeApiFactory();
                        using (IDataExchangeQueueTransaction transaction = api.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
                        {
                            transaction.Begin();
                            api.EnqueueErroneousImportMessage(message, transaction); // Move to the error queue.
                            transaction.Commit();
                        }
                    });
                onCompletion?.Invoke(ret);
            }
        }

        public bool changeMessageLogState(string externalReference, string status)
        {
            int state = 0;
            if (!_states.TryGetValue(status, out state))
            {
                return false; // Status with no MessageLog conversion value.
            }
            return base.changeMessageLogState(externalReference,(TransLogMessageStatus)state);
        }
    }
}
