using System.Collections.Generic;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;
using Powel.Icc.Services.DataContracts.EventMonitor;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class RestrictionTest
    {

        [TestCase(TransLogMessageStatus.ExportSentWaitingForAcknowledgement, TransLogMessageStatus.ExportSentReceivedOk)]
        [TestCase(TransLogMessageStatus.ExportSentWaitingForAcknowledgement, TransLogMessageStatus.ExportSentReceivedError)]
        [TestCase(TransLogMessageStatus.ExportSentToRemoteSystem, TransLogMessageStatus.ExportSentReceivedError)]
        [TestCase(TransLogMessageStatus.ExportSentToRemoteSystem, TransLogMessageStatus.ExportSentReceivedError)]
        public void IsFulfilled_RestrictionIsFulfilled_ReturnsTrue(TransLogMessageStatus currentStatus, TransLogMessageStatus newStatus)
        {
            var restriction = new Restriction(new HashSet<TransLogMessageStatus>
                {
                    TransLogMessageStatus.ExportSentWaitingForAcknowledgement,
                    TransLogMessageStatus.ExportSentToRemoteSystem
                },
                new HashSet<TransLogMessageStatus>
                    {
                        TransLogMessageStatus.ExportSentReceivedOk,
                        TransLogMessageStatus.ExportSentReceivedError
                    });

            Assert.AreEqual(restriction.IsFulfilled(currentStatus, newStatus), true);
        }

        [TestCase(TransLogMessageStatus.ExportSentWaitingForAcknowledgement, TransLogMessageStatus.ExportSentSuccessful)]
        [TestCase(TransLogMessageStatus.ExportSentToRemoteSystem, TransLogMessageStatus.ImportFailed)]
        public void IsFulfilled_RestrictionNotFulfilled_ReturnsFalse(TransLogMessageStatus currentStatus, TransLogMessageStatus newStatus)
        {
            var restriction = new Restriction(new HashSet<TransLogMessageStatus>
                {
                    TransLogMessageStatus.ExportSentWaitingForAcknowledgement,
                    TransLogMessageStatus.ExportSentToRemoteSystem
                },
                new HashSet<TransLogMessageStatus>
                    {
                        TransLogMessageStatus.ExportSentReceivedOk,
                        TransLogMessageStatus.ExportSentReceivedError
                    });

            Assert.AreEqual(restriction.IsFulfilled(currentStatus, newStatus), false);
        }

        [TestCase(TransLogMessageStatus.ImportError, TransLogMessageStatus.ExportSentSuccessful)]
        [TestCase(TransLogMessageStatus.ExportTransferredError, TransLogMessageStatus.ExportSentReceivedOk)]
        public void IsFulfilled_RestrictionDoesntExist_ReturnsTrue(TransLogMessageStatus currentStatus, TransLogMessageStatus newStatus)
        {
            var restriction = new Restriction(new HashSet<TransLogMessageStatus>
                {
                    TransLogMessageStatus.ExportSentWaitingForAcknowledgement,
                    TransLogMessageStatus.ExportSentToRemoteSystem
                },
                new HashSet<TransLogMessageStatus>
                    {
                        TransLogMessageStatus.ExportSentReceivedOk,
                        TransLogMessageStatus.ExportSentReceivedError
                    });

            Assert.AreEqual(restriction.IsFulfilled(currentStatus, newStatus), true);
        } 
    }
}