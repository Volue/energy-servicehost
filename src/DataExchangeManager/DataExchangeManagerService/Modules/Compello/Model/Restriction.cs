using System;
using System.Collections.Generic;
using System.Linq;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model
{
    public class Restriction
    {
        private readonly HashSet<TransLogMessageStatus> _currentStatuses;
        private readonly HashSet<TransLogMessageStatus> _allowedNextStatuses;

        public Restriction(HashSet<TransLogMessageStatus> currentStatuses,
            HashSet<TransLogMessageStatus> allowedNextStatuses)
        {
            if(currentStatuses == null)
            {
                throw new ArgumentNullException("currentStatuses");
            }

            _allowedNextStatuses = allowedNextStatuses;
            _currentStatuses = currentStatuses;
        }

        public bool IsFulfilled(TransLogMessageStatus currentStatus, TransLogMessageStatus newStatus)
        {
            if (_currentStatuses.Contains(currentStatus))
            {
                if (IsEmpty(_allowedNextStatuses)
                    || !_allowedNextStatuses.Contains(newStatus))
                {
                    return false;
                }
            }

            return true;
        }

        private static bool IsEmpty(IEnumerable<TransLogMessageStatus> messageStatuses)
        {
            return messageStatuses == null || !messageStatuses.Any();
        }
    }
}