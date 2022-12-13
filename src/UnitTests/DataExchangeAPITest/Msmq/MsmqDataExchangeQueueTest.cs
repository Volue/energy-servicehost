using System;
using System.Collections.Generic;
using System.Linq;
using System.Messaging;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApiTest.Msmq
{
    /// <summary>
    /// Tests for the MSMQ implementation of IDataExchangeQueue.
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
    public class MsmqDataExchangeQueueTest
    {
        public class MsmqDataExchangeQueueTestDummy : DataExchangeMessageBase
        {
            public MsmqDataExchangeQueueTestDummy() { }
            public string Value { get; set; }
        }

        private IDataExchangeQueue<MsmqDataExchangeQueueTestDummy> _dataExchangeQueue;
        private Dictionary<DataExchangeQueuePriority, MsmqPath> _msmqPaths;
        private Dictionary<DataExchangeQueuePriority, MessageQueue> _messageQueues;

        [SetUp]
        public void SetUp()
        {
            _msmqPaths = new Dictionary<DataExchangeQueuePriority, MsmqPath>
                {
                    {DataExchangeQueuePriority.High, new MsmqPath {FullPath = @".\Private$\" + Guid.NewGuid()}},
                    {DataExchangeQueuePriority.Normal, new MsmqPath {FullPath = @".\Private$\" + Guid.NewGuid()}},
                    {DataExchangeQueuePriority.Low, new MsmqPath {FullPath = @".\Private$\" + Guid.NewGuid()}},
                    {DataExchangeQueuePriority.Undefined, new MsmqPath {FullPath = @".\Private$\" + Guid.NewGuid()}}
                };
            
            var orderedPriorities = new List<DataExchangeQueuePriority>
                {
                    DataExchangeQueuePriority.High,
                    DataExchangeQueuePriority.Normal,
                    DataExchangeQueuePriority.Low,
                    DataExchangeQueuePriority.Undefined
                };

            _messageQueues = new Dictionary<DataExchangeQueuePriority, MessageQueue>();
            foreach (var priority in orderedPriorities)
            {
                _messageQueues[priority] = MessageQueue.Create(_msmqPaths[priority].FullPath, true);
            }

            var serviceEventLoggerMock = new Mock<IServiceEventLogger>();

            _dataExchangeQueue = new MsmqDataExchangeQueue<MsmqDataExchangeQueueTestDummy>(orderedPriorities, _msmqPaths, serviceEventLoggerMock.Object);
        }

        [TearDown]
        public void TearDown()
        {
            _dataExchangeQueue.Dispose();

            foreach (var key in _messageQueues.Keys)
            {
                var messageQueuePath = _messageQueues[key].Path;
                _messageQueues[key].Close();
                MessageQueue.Delete(messageQueuePath);
            }
        }

        [Test]
        public void Enqueue_MessageIsEnqueuedWithNormalPriority_MessageIsFoundInNormalPriorityQueue()
        {
            // Assign

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            _dataExchangeQueue.Enqueue(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value." }, DataExchangeQueuePriority.Normal, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            var allMessages = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages();
            Assert.AreEqual(1, allMessages.Length);
        }

        [Test]
        public void Enqueue_MessageIsEnqueuedWithNormalPriority_MessageIsCorrect()
        {
            // Assign

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            _dataExchangeQueue.Enqueue(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value." }, DataExchangeQueuePriority.Normal, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            var allMessages = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages();
            allMessages[0].Formatter = new XmlMessageFormatter(new[] { typeof(MsmqDataExchangeQueueTestDummy) });
            var message = (MsmqDataExchangeQueueTestDummy) allMessages[0].Body;
            Assert.AreEqual("Dummy value.", message.Value);
        }

        
        [Test]
        public void Enqueue_NoPriorityIsSpecified_PriorityIsUndefined()
        {
            // Assign

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            _dataExchangeQueue.Enqueue(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value." }, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            var allMessages = _messageQueues[DataExchangeQueuePriority.Undefined].GetAllMessages();
            Assert.AreEqual(1, allMessages.Length);
        }

        
        [Test]
        public void Dequeue_MessageIsAvailableInNormalPriorityQueue_MessageIsReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value."}, transaction);
            transaction.Commit();

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            var message = _dataExchangeQueue.Dequeue(new TimeSpan(0), dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            Assert.AreEqual("Dummy value.", message.Value);
        }

        [Test]
        public void Dequeue_MessageIsAvailableInNormalPriorityQueue_MessageIsRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value."}, transaction);
            transaction.Commit();

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            _dataExchangeQueue.Dequeue(new TimeSpan(0), dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            var allMessages = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages();
            Assert.AreEqual(0, allMessages.Length);
        }

        [Test]
        public void Dequeue_MessageIsAvailableInNormalPriorityQueue_PriorityIsNormal()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value.", Priority = "Normal" }, transaction);
            transaction.Commit();

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            var message = _dataExchangeQueue.Dequeue(new TimeSpan(0), dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            Assert.AreEqual("Normal", message.Priority);
        }

        [Test]
        public void Dequeue_ThreeMessagesAreAvailableInAnyQueueAndThreeAreDequeued_ThreeMessagesAreReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3."}, transaction);
            transaction.Commit();

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            var messages = _dataExchangeQueue.Dequeue(3, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Act

            Assert.AreEqual(3, messages.Count);
        }

        [Test]
        public void Dequeue_ThreeMessagesAreAvailableInAnyQueueAndThreeAreDequeued_ThreeMessagesAreReturnedInTheCorrectOrder()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3."}, transaction);
            transaction.Commit();

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            var messages = _dataExchangeQueue.Dequeue(3, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            Assert.AreEqual("Dummy value 1.", messages[0].Value);
            Assert.AreEqual("Dummy value 2.", messages[1].Value);
            Assert.AreEqual("Dummy value 3.", messages[2].Value);
        }

        [Test]
        public void Dequeue_ThreeMessagesAreAvailableInAnyQueueAndThreeAreDequeued_ThreeMessagesAreReturnedWithTheCorrectPriorities()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2.", Priority = "Normal"}, transaction);
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1.", Priority = "Low"}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3.", Priority = "High"}, transaction);
            transaction.Commit();

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            var messages = _dataExchangeQueue.Dequeue(3, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            Assert.AreEqual("High", messages[0].Priority);
            Assert.AreEqual("Normal", messages[1].Priority);
            Assert.AreEqual("Low", messages[2].Priority);
        }

        [Test]
        public void Dequeue_ThreeMessagesAreAvailableInAnyQueueAndThreeAreDequeued_AllMessagesAreRemovedFromAllQueues()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3."}, transaction);
            transaction.Commit();

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            _dataExchangeQueue.Dequeue(3, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            var allMessages = new Dictionary<DataExchangeQueuePriority, Message[]>();
            allMessages[DataExchangeQueuePriority.High] = _messageQueues[DataExchangeQueuePriority.High].GetAllMessages();
            allMessages[DataExchangeQueuePriority.Normal] = _messageQueues[DataExchangeQueuePriority.High].GetAllMessages();
            allMessages[DataExchangeQueuePriority.Low] = _messageQueues[DataExchangeQueuePriority.Low].GetAllMessages();

            Assert.AreEqual(0, allMessages[DataExchangeQueuePriority.High].Length);
            Assert.AreEqual(0, allMessages[DataExchangeQueuePriority.Normal].Length);
            Assert.AreEqual(0, allMessages[DataExchangeQueuePriority.Low].Length);
        }

        [Test]
        public void Dequeue_ThreeMessagesAreAvailableInAnyQueueAndTwoAreDequeued_AllMessagesButOneAreRemovedFromAllQueues()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3."}, transaction);
            transaction.Commit();

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            _dataExchangeQueue.Dequeue(2, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            var allMessages = new Dictionary<DataExchangeQueuePriority, Message[]>();
            allMessages[DataExchangeQueuePriority.High] = _messageQueues[DataExchangeQueuePriority.High].GetAllMessages();
            allMessages[DataExchangeQueuePriority.Normal] = _messageQueues[DataExchangeQueuePriority.High].GetAllMessages();
            allMessages[DataExchangeQueuePriority.Low] = _messageQueues[DataExchangeQueuePriority.Low].GetAllMessages();

            Assert.AreEqual(0, allMessages[DataExchangeQueuePriority.High].Length);
            Assert.AreEqual(0, allMessages[DataExchangeQueuePriority.Normal].Length);
            Assert.AreEqual(1, allMessages[DataExchangeQueuePriority.Low].Length);
        }

        [Test]
        public void Peek_MessageIsAvailableInNormalPriorityQueue_MessageIsReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value."}, transaction);
            transaction.Commit();

            // Act

            string messageId;
            var message = _dataExchangeQueue.Peek(new TimeSpan(0), out messageId);

            // Assert

            Assert.AreEqual("Dummy value.", message.Value);
        }

        [Test]
        public void Peek_MessageIsAvailableInNormalPriorityQueue_MessageIsNotRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value."}, transaction);
            transaction.Commit();

            // Act

            string messageId;
            _dataExchangeQueue.Peek(new TimeSpan(0), out messageId);

            // Assert

            var allMessages = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages();
            Assert.AreEqual(1, allMessages.Length);
        }

        [Test]
        public void Peek_MessageIsAvailableInNormalPriorityQueue_PriorityIsNormal()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value.", Priority = "Normal"}, transaction);
            transaction.Commit();

            // Act

            string messageId;
            var message = _dataExchangeQueue.Peek(new TimeSpan(0), out messageId);

            // Assert

            Assert.AreEqual("Normal", message.Priority);
        }

        [Test]
        public void Peek_MessageIsAvailableInNormalPriorityQueue_MessageIdIsCorrect()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value."}, transaction);
            transaction.Commit();

            var expectedMessageId = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages()[0].Id;

            // Act

            string actualMessageId;
            _dataExchangeQueue.Peek(new TimeSpan(0), out actualMessageId);

            // Assert

            Assert.AreEqual(expectedMessageId, actualMessageId);
        }

        [Test]
        public void Peek_ThreeMessagesAreAvailableInAnyQueueAndThreeArePeeked_ThreeMessagesAreReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3."}, transaction);
            transaction.Commit();

            // Act

            var messages = _dataExchangeQueue.Peek(3);

            // Assert

            Assert.AreEqual(3, messages.Count);
        }

        [Test]
        public void Peek_ThreeMessagesAreAvailableInAnyQueueAndThreeArePeeked_ThreeMessagesAreReturnedWithTheCorrectMessageIds()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3."}, transaction);
            transaction.Commit();

            var expectedMessageIds = new Dictionary<DataExchangeQueuePriority, string>();
            expectedMessageIds[DataExchangeQueuePriority.High] = _messageQueues[DataExchangeQueuePriority.High].GetAllMessages()[0].Id;
            expectedMessageIds[DataExchangeQueuePriority.Normal] = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages()[0].Id;
            expectedMessageIds[DataExchangeQueuePriority.Low] = _messageQueues[DataExchangeQueuePriority.Low].GetAllMessages()[0].Id;

            // Act

            var messages = _dataExchangeQueue.Peek(3);

            // Assert

            Assert.AreEqual("Dummy value 1.", messages[expectedMessageIds[DataExchangeQueuePriority.High]].Value);
            Assert.AreEqual("Dummy value 2.", messages[expectedMessageIds[DataExchangeQueuePriority.Normal]].Value);
            Assert.AreEqual("Dummy value 3.", messages[expectedMessageIds[DataExchangeQueuePriority.Low]].Value);
        }

        [Test]
        public void Peek_ThreeMessagesAreAvailableInAnyQueueAndThreeArePeeked_ThreeMessagesAreReturnedWithTheCorrectPriorities()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1.", Priority = "Low"}, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2.", Priority = "Normal"}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3.", Priority = "High"}, transaction);
            transaction.Commit();

            var expectedMessageIds = new Dictionary<DataExchangeQueuePriority, string>();
            expectedMessageIds[DataExchangeQueuePriority.High] = _messageQueues[DataExchangeQueuePriority.High].GetAllMessages()[0].Id;
            expectedMessageIds[DataExchangeQueuePriority.Normal] = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages()[0].Id;
            expectedMessageIds[DataExchangeQueuePriority.Low] = _messageQueues[DataExchangeQueuePriority.Low].GetAllMessages()[0].Id;

            // Act

            var messages = _dataExchangeQueue.Peek(3);

            // Assert

            Assert.AreEqual("High", messages[expectedMessageIds[DataExchangeQueuePriority.High]].Priority);
            Assert.AreEqual("Normal", messages[expectedMessageIds[DataExchangeQueuePriority.Normal]].Priority);
            Assert.AreEqual("Low", messages[expectedMessageIds[DataExchangeQueuePriority.Low]].Priority);
        }

        [Test]
        public void Peek_ThreeMessagesAreAvailableInAnyQueueAndTwoArePeeked_TwoMessagesAreReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3."}, transaction);
            transaction.Commit();

            // Act

            var messages = _dataExchangeQueue.Peek(2);

            // Assert

            Assert.AreEqual(2, messages.Count);
        }

        [Test]
        public void Peek_ThreeMessagesAreAvailableInAnyQueueAndThreeArePeeked_NoMessagesAreRemovedFromAnyQueue()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3."}, transaction);
            transaction.Commit();

            // Act

            _dataExchangeQueue.Peek(3);

            // Assert

            var allMessages = new Dictionary<DataExchangeQueuePriority, Message[]>();
            allMessages[DataExchangeQueuePriority.High] = _messageQueues[DataExchangeQueuePriority.High].GetAllMessages();
            allMessages[DataExchangeQueuePriority.Normal] = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages();
            allMessages[DataExchangeQueuePriority.Low] = _messageQueues[DataExchangeQueuePriority.Low].GetAllMessages();

            Assert.AreEqual(1, allMessages[DataExchangeQueuePriority.High].Length);
            Assert.AreEqual(1, allMessages[DataExchangeQueuePriority.Normal].Length);
            Assert.AreEqual(1, allMessages[DataExchangeQueuePriority.Low].Length);
        }

        [Test]
        public void Remove_MessageIsAvailableInNormalPriorityQueue_MessageIsRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value."}, transaction);
            transaction.Commit();

            var messageId = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages()[0].Id;

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            _dataExchangeQueue.Remove(messageId, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            var allMessages = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages();
            Assert.AreEqual(0, allMessages.Length);
        }

        [Test]
        public void Remove_ThreeMessagesAreAvailableInAnyQueueAndTheyAreRemoved_AllAreRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy value 3."}, transaction);
            transaction.Commit();

            var messageIds = new Dictionary<DataExchangeQueuePriority, string[]>();
            messageIds[DataExchangeQueuePriority.High] = _messageQueues[DataExchangeQueuePriority.High].GetAllMessages().Select(x => x.Id).ToArray();
            messageIds[DataExchangeQueuePriority.Normal] = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages().Select(x => x.Id).ToArray();
            messageIds[DataExchangeQueuePriority.Low] = _messageQueues[DataExchangeQueuePriority.Low].GetAllMessages().Select(x => x.Id).ToArray();

            var allMessageIds = new List<string>();
            allMessageIds.AddRange(messageIds[DataExchangeQueuePriority.High]);
            allMessageIds.AddRange(messageIds[DataExchangeQueuePriority.Normal]);
            allMessageIds.AddRange(messageIds[DataExchangeQueuePriority.Low]);

            // Act

            IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction();
            dataExchangeQueueTransaction.Begin();
            _dataExchangeQueue.Remove(allMessageIds, dataExchangeQueueTransaction);
            dataExchangeQueueTransaction.Commit();

            // Assert

            var allMessages = new Dictionary<DataExchangeQueuePriority, Message[]>();
            allMessages[DataExchangeQueuePriority.High] = _messageQueues[DataExchangeQueuePriority.High].GetAllMessages();
            allMessages[DataExchangeQueuePriority.Normal] = _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages();
            allMessages[DataExchangeQueuePriority.Low] = _messageQueues[DataExchangeQueuePriority.Low].GetAllMessages();

            Assert.AreEqual(0, allMessages[DataExchangeQueuePriority.High].Length);
            Assert.AreEqual(0, allMessages[DataExchangeQueuePriority.Normal].Length);
            Assert.AreEqual(0, allMessages[DataExchangeQueuePriority.Low].Length);
        }

        [Test]
        public void GetQueueSize_ThreeMessagesAreAvailableInAnyQueue_AllAreCounted()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy message 1."}, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy message 2."}, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy {Value = "Dummy message 3."}, transaction);
            transaction.Commit();

            // Assert

            var queueSize = _dataExchangeQueue.GetQueueSize();

            // Act

            Assert.AreEqual(3, queueSize);
        }

        [Test]
        public void GetAll_ThreeMessagesAreAvailableInAnyQueue_ThreeMessagesAreReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Low].Send(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value 1." }, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value 2." }, transaction);
            _messageQueues[DataExchangeQueuePriority.High].Send(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value 3." }, transaction);
            transaction.Commit();

            // Act

            Dictionary<string, MsmqDataExchangeQueueTestDummy> messages;
            using(IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction())
            {
                dataExchangeQueueTransaction.Begin();
                messages = _dataExchangeQueue.GetAll(dataExchangeQueueTransaction);
                dataExchangeQueueTransaction.Commit();
            }

            // Act

            Assert.AreEqual(3, messages.Count);
        }

        [Test]
        public void GetAll_ContainsPoisonousMessage_OnlyValidMessagesAreReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value 1." }, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send("Invalid message", transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value 2." }, transaction);
            transaction.Commit();

            // Act

            Dictionary<string, MsmqDataExchangeQueueTestDummy> messages;
            using(IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction())
            {
                dataExchangeQueueTransaction.Begin();
                messages = _dataExchangeQueue.GetAll(dataExchangeQueueTransaction);
                dataExchangeQueueTransaction.Commit();
            }

            // Act

            Assert.AreEqual(2, messages.Count);
        }

        [Test]
        public void GetAll_ContainsPoisonousMessage_ThePoisonousMessageIsDiscarded()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value 1." }, transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send("Invalid message", transaction);
            _messageQueues[DataExchangeQueuePriority.Normal].Send(new MsmqDataExchangeQueueTestDummy { Value = "Dummy value 2." }, transaction);
            transaction.Commit();

            // Act

            using(IDataExchangeQueueTransaction dataExchangeQueueTransaction = new MsmqDataExchangeQueueTransaction())
            {
                dataExchangeQueueTransaction.Begin();
                _dataExchangeQueue.GetAll(dataExchangeQueueTransaction);
                dataExchangeQueueTransaction.Commit();
            }

            // Act

            Assert.AreEqual(2, _messageQueues[DataExchangeQueuePriority.Normal].GetAllMessages().Length, "The poisonous message hasn't been removed.");
        }
    }
}
