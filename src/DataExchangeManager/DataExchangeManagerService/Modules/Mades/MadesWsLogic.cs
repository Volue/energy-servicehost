using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.MadesService;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Mades
{
    public class MadesWsLogic : WsLogicBase
    {
        private struct QueryPriority
        {
            public Regex query; // E.g. <DocumentType v="A40"
            public DataExchangeQueuePriority priority; // E.g. "Low"
        };

        private List<QueryPriority> _prioritizedQueries = new List<QueryPriority>();

        public MadesWsLogic(Func<IDataExchangeApi> dataExchangeApiFactory, IServiceEventLogger serviceEventLogger)
            : base(dataExchangeApiFactory,serviceEventLogger)
        {
            // Initialize from config variable
            string defpriQ = IccConfiguration.ConfigurationReader.ReadOptionalString("ICC_MADES_IMPORT_DOCQUERY_PRIORITY_LIST", "<DocumentType v=\"A40\",High;<DocumentType v=\"Z15\",High;<ReceivingDocumentType v=\"A41\",High");
            if (!string.IsNullOrEmpty(defpriQ))
            {
                var queryPrioL = defpriQ.Split(new char[] { ';' });
                foreach (var queryPrio in queryPrioL)
                {
                    var qpS = queryPrio.Split(new char[] { ',' });
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
        }

        private string GetRoutingAddress(ReceivedMessage ecpMessage)
        {
            // On Import: For acknowledgement and response routing out.
            return "MADES:" + ecpMessage.senderCode;   // Format: <NameOfExportQueue>:<EICaddress>   Given in Participant.Comunication info on export.
        }

        public bool SubmitImport(ReceivedMessage ecpMessage)
        {
            var message = new DataExchangeImportMessage
            {
                Version = DataExchangeImportMessage.LatestVersion,
                ExternalReference = ecpMessage.messageID,
                SenderId = ecpMessage.senderCode,
                ReceiverId = ecpMessage.receiverCode,
                SubAddress = ecpMessage.businessType
            };
            var msg = UnicodeEncoding.Default.GetString(ecpMessage.content);
            message.SetMessageData(msg,null);
            message.Priority = GetPriority(msg).ToString();
            message.Protocol = GetProtocol(message);
            message.RoutingAddress = GetRoutingAddress(ecpMessage);   // For acknowledgement and response routing out.
            return SubmitImport(message);
        }

        private DataExchangeQueuePriority GetPriority(string message)
        {
            // Decide priority from externally specified query

            foreach (var prioritizedQuery in _prioritizedQueries)
            {
                if (prioritizedQuery.query.IsMatch(message))
                    return prioritizedQuery.priority;
            }
            return DataExchangeQueuePriority.Normal;
        }

        private static string GetProtocol(DataExchangeMessageBase message)
        {
            const string defRe = "<ActivationDocument xmlns=\"urn:entsoe.eu:wgedi:errp";   // "<DocumentType v=\"(A40|Z15|A39)\"";
            Regex errpDocs = null;
            try
            {
                errpDocs = new Regex(IccConfiguration.ConfigurationReader.ReadOptionalString("ICC_MADES_IMPORT_ERRPDOCS", defRe));
            }
            catch (ArgumentException E)
            {   // Invalid user defined RE
                Console.WriteLine(E.Message);
                errpDocs = new Regex(defRe);
            }
            return errpDocs.IsMatch(message.GetMessageData()) ? "ERRP" : "ENTSOE";
        }
    }
}
