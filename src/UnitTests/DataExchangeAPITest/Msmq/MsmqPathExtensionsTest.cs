using System;
using System.Collections.Generic;
using System.Messaging;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApiTest.Msmq
{
    /// <summary>
    /// Tests for MessageQueue-array extensions.
    /// 
    /// These tests requires MSMQ to be installed on the machine running the tests,
    /// and the user running the test must be allowed to create and delete message queues.
    /// 
    /// To install Message Queuing 4.0 on Windows Server 2008 or Windows Server 2008 R2
    ///    In Server Manager, click Features.
    ///    In the right-hand pane under Features Summary, click Add Features.
    ///    In the resulting window, expand Message Queuing.
    ///    Expand Message Queuing Services.
    ///    Click Directory Services Integration (for computers joined to a Domain), then click HTTP Support.
    ///    Click Next,then click Install.
    ///
    /// To install Message Queuing 4.0 on Windows 7 or Windows Vista
    ///   Open Control Panel.
    ///   Click Programs and then, under Programs and Features, click Turn Windows Features on and off.
    ///   Expand Microsoft Message Queue (MSMQ) Server, expand Microsoft Message Queue (MSMQ) Server Core, and then select the check boxes for the following Message Queuing features to install:
    ///    * MSMQ Active Directory Domain Services Integration (for computers joined to a Domain).
    ///    * MSMQ HTTP Support.
    ///   Click OK.
    ///   If you are prompted to restart the computer, click OK to complete the installation.
    /// </summary>
    [TestFixture]
    public class MsmqPathExtensionsTest
    {
        private Dictionary<DataExchangeQueuePriority, MsmqPath> _msmqPaths;
        private MessageQueue[] _messageQueues;

        [SetUp]
        public void SetUp()
        {
            _msmqPaths = new Dictionary<DataExchangeQueuePriority, MsmqPath>
                {
                    {DataExchangeQueuePriority.High, new MsmqPath {FullPath = @".\Private$\" + Guid.NewGuid()}},
                    {DataExchangeQueuePriority.Normal, new MsmqPath {FullPath = @".\Private$\" + Guid.NewGuid()}},
                    {DataExchangeQueuePriority.Low, new MsmqPath {FullPath = @".\Private$\" + Guid.NewGuid()}}
                };

            var orderedPriorities = new List<DataExchangeQueuePriority>
                {
                    DataExchangeQueuePriority.High,
                    DataExchangeQueuePriority.Normal,
                    DataExchangeQueuePriority.Low
                };

            _messageQueues = new MessageQueue[orderedPriorities.Count];
            for (int i = 0; i < orderedPriorities.Count; i++)
            {
                _messageQueues[i] = MessageQueue.Create(_msmqPaths[orderedPriorities[i]].FullPath, true);
            }
        }

        [TearDown]
        public void TearDown()
        {
            foreach (var messageQueue in _messageQueues)
            {
                var messageQueuePath = messageQueue.Path;
                messageQueue.Close();
                MessageQueue.Delete(messageQueuePath);
            }
        }

        [Test]
        public void WaitAny_NoMessagesInAnyOfTheMessageQueues_ReturnsMinusOne()
        {
            // Assign

            // Act

            int index = _msmqPaths.WaitAny(new TimeSpan(0));
            
            // Assert

            Assert.AreEqual(-1, index);
        }

        [Test]
        public void WaitAny_OneMessageInTheFirstMessageQueue_ReturnsZero()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[0].Send("Dummy object.", transaction);
            transaction.Commit();

            // Act

            int index = _msmqPaths.WaitAny(new TimeSpan(0));
            
            // Assert

            Assert.AreEqual(0, index);
        }

        [Test]
        public void WaitAny_FiveMessagesInAnyOfTheMessageQueues_TheMessagesAreReceivedCorrectly()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[0].Send("Dummy object 1.", transaction);
            _messageQueues[0].Send("Dummy object 2.", transaction);
            _messageQueues[1].Send("Dummy object 3.", transaction);
            _messageQueues[1].Send("Dummy object 4.", transaction);
            _messageQueues[2].Send("Dummy object 5.", transaction);
            transaction.Commit();

            var result = new []
                {
                    new List<string>(),
                    new List<string>(),
                    new List<string>()
                };

            // Act

            while(true)
            {
                int index = _msmqPaths.WaitAny(new TimeSpan(0));

                if(index < 0)
                {
                    break;
                }

                transaction = new MessageQueueTransaction();
                transaction.Begin();
                var message = _messageQueues[index].Receive(new TimeSpan(0), transaction);
                transaction.Commit();

                if(message != null)
                {
                    result[index].Add(message.Body.ToString());
                }
            }

            // Assert

            Assert.AreEqual("Dummy object 1.", result[0][0]);
            Assert.AreEqual("Dummy object 2.", result[0][1]);
            Assert.AreEqual("Dummy object 3.", result[1][0]);
            Assert.AreEqual("Dummy object 4.", result[1][1]);
            Assert.AreEqual("Dummy object 5.", result[2][0]);
        }

        [Test]
        public void CountMessages_ThreeMessagesInTheMessageQueue_ThreeIsReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[0].Send("Dummy message 1.", transaction);
            _messageQueues[1].Send("Dummy message 2.", transaction);
            _messageQueues[2].Send("Dummy message 3.", transaction);
            transaction.Commit();

            // Act

            var result = _msmqPaths.CountMessages();

            // Assert

            Assert.AreEqual(3, result);
        }
    }
}
