using System.Collections.Generic;
using Powel.Icc.Data.Entities.MessageLog;
using Powel.Icc.Services.Time;

namespace Powel.Icc.Data.Interfaces
{
    public interface IMessageLogData
    {
        List<MessageHeader> GetMessageHeaders(int roleKey, MessageLogQueryParameters parameters);

        List<MessageHeaderStatus> GetMessageHeaderStatuses(int roleKey, List<string> messageReferences);

        List<MessageLogExportStatus> GetExportStatuses(int roleKey, List<string> exchangeMethodNames,
            TimePeriod timePeriod, int maximumNumberOfMessageLogEntries);
    }
}