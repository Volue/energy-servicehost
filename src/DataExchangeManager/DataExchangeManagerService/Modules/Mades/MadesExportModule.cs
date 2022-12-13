using System.ServiceModel;
using System.Text;
using System.Xml;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Exporters;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.MadesService;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Common.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Mades
{
    class MadesExportModule : PushExportModule
    {
        public const string MODULE_NAME = "MADES";  // ToDo: Mix up of definitions. ModuleName is both used as name of the plugin (Unity injection) but also used as part of message queue name. See RunThread()

        public override string ModuleName => MODULE_NAME;
        private readonly string _endpointConfigName;

        public MadesExportModule(
            IDataExchangeManagerServiceSettingsFactory dataExchangeManagerServiceSettingsFactory,
            MessageExporter messageExporter,
            IServiceEventLogger serviceEventLogger)
            : base(dataExchangeManagerServiceSettingsFactory, messageExporter, serviceEventLogger)
        {
            _endpointConfigName = IccConfiguration.DataExchangeManager.Mades.EndpointConfigurationName("MadesEndpointSOAP12");
        }

        protected sealed override string SendExportMessage(DataExchangeExportMessage export)
        {
            var wMsg = new SentMessage();
            wMsg.businessType = GetBusinessType(export);
            if (string.IsNullOrEmpty(wMsg.businessType))
                throw new DataExchangeInvalidMessageException(export.MessageLogId.ToString(),"Missing BusinessType/Application-ID/Routing");
            wMsg.receiverCode = GetReceiverCode(export);
            if (string.IsNullOrEmpty(wMsg.receiverCode))
                throw new DataExchangeInvalidMessageException(export.MessageLogId.ToString(), "Missing ReceiverCode/EICaddress");
            wMsg.baMessageID = GetMessageId(export);
            wMsg.content = UnicodeEncoding.Default.GetBytes(export.GetMessageData());
            wMsg.senderApplication = "Powel";

            try
            {
                using (var ecp = new MadesEndpointClient(_endpointConfigName))
                {
                    // send message, Guid shall in reality be substituted by business ID of the message
                    return ecp.SendMessage(wMsg, export.ExternalReference);
                }
            }
            catch (FaultException<SendMessageError> E)
            {
                LogError(E.Detail.errorMessage);
                throw new DataExchangeInvalidMessageException(export.MessageLogId.ToString(), "ECP failure", E); // Throw an exception that removes the message from the MSMQ queue.
            }
            catch (FaultException E)
            {
                LogError(E.Message);
                throw new DataExchangeInvalidMessageException(export.MessageLogId.ToString(), "ECP failure", E); // Trying to throw an exception that removes the message from the MSMQ queue.
            }
        }

        private static string GetBusinessType(DataExchangeMessageBase message)
        {
            return message.SubAddress;
        }

        private static string GetReceiverCode(DataExchangeMessageBase message)
        {
            return message.RoutingAddress.Split(new char[] {':'})[1];   // Format: MADES:<EICaddress>
        }

        private static string GetMessageId(DataExchangeMessageBase message)
        {
            var doc = new XmlDocument();
            doc.LoadXml(message.GetMessageData());
            var ns = new XmlNamespaceManager(doc.NameTable);
            ns.AddNamespace("act", "urn:entsoe.eu:wgedi:errp:activatondocument:5:0");
            ns.AddNamespace("ack", "urn:entsoe.eu:wgedi:acknowledgement:acknowledgementdocument:6:0");
            var docId = doc.DocumentElement.SelectSingleNode("//act:DocumentIdentification/@v", ns);
            XmlNode docV = null;
            if (docId == null)
                docId = doc.DocumentElement.SelectSingleNode("//ack:DocumentIdentification/@v", ns);
            else
                docV = doc.DocumentElement.SelectSingleNode("//act:DocumentVersion/@v", ns);
            string ret = "";
            if (docId == null)
                return ret;
            ret = docId.Value;
            if (docV == null)
                return ret;
            return ret + "-" + docV.Value;  // Legal characters [A-Za-z0-9-]*
        }
    }
}
