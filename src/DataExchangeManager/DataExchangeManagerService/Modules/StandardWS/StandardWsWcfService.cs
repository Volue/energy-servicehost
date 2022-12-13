using System.Collections.Generic;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeModuleStandardWsInterface;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.StandardWS
{
    public class StandardWsWcfService : IStandardWs
    {
        private readonly StandardWsLogic _logic;

        public StandardWsWcfService(StandardWsLogic logic)
        {
            _logic = logic;
        }

        public GetExportResponse GetExport(int maxWaitTimeInSeconds)
        {
            return _logic.GetExport(maxWaitTimeInSeconds);
        }

        public AcknowledgeExportResponse SendExportAck(string messageReference)
        {
            return _logic.SendExportAck(messageReference);
        }

        public SetMessageStatusResponse SetMessageStatus(string reference, MessageStatusTypeEnum status, string statusText)
        {
            return _logic.SetMessageStatus(reference, status, statusText);
        }

        public SubmitImportResponse SubmitImport(Dictionary<string, string> metadata, string importData, string reference)
        {
            return _logic.SubmitImport(metadata, importData, reference);
        }

        public SubmitEventResponse SubmitEvent(EventLevelTypeEnum level, string eventText)
        {
            return _logic.SubmitEvent(level, eventText);
        }
    }
}
