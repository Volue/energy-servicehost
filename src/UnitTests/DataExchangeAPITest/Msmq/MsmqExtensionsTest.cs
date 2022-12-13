using System;
using System.Linq;
using System.Messaging;
using System.Text;
using System.Transactions;
using NUnit.Framework;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq.Exceptions;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApiTest.Msmq
{
    /// <summary>
    /// Tests for MessageQueue extensions.
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
    /// 
    /// For the MSDTC tests DTC may need to be configured:
    ///   1: Open the Control Panel, open Administrative Tools, and open Component Services.
    ///   2: In Component Services, in the left pane, click the Component Services node, then click the Computers node. In the right pane, right-click My Computer and select Properties from the context menu. This will open the My Computer Properties dialog.
    ///   3: In the My Computer Properties dialog, select the MSDTC tab, and click the Security Configuration... button in the Transaction Configuration group at the lower left. This will open the Security Configuration dialog.
    ///   4: In the Security Configuration dialog, check the Network DTC Access checkbox. This will enable the Client and Administration and Transaction Manager Communication groups.
    ///   5: In the Transaction Manager Communication group, check the Allow Inbound and Allow Outbound check boxes, and select the Mutual Authentication Required radio button.
    ///   6: Click OK to close the Security Configuration dialog. This will pop up a DTC Console Message dialog that asks if you want to stop and restart the MS DTC service. Click Yes.
    ///   7: Click OK to close the My Computer Properties dialog.
    /// </summary>
    [TestFixture]
    public class MsmqExtensionsTest
    {
        public class MessageQueueExtensionsTestDummy
        {
            public string Value { get; set;}
        }

        private MessageQueue _messageQueue;

        [SetUp]
        public void SetUp()
        {
            _messageQueue = MessageQueue.Create(@".\Private$\" + Guid.NewGuid(), true);
        }

        [TearDown]
        public void TearDown()
        {
            var messageQueuePath = _messageQueue.Path;
            _messageQueue.Close();
            MessageQueue.Delete(messageQueuePath);
        }

        [Test]
        public void SendT_MessageIsSent_IsPlacedInMessageQueue()
        {
            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send<MessageQueueExtensionsTestDummy>(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            var allMessages = _messageQueue.GetAllMessages();
            allMessages[0].Formatter = new XmlMessageFormatter(new[] {typeof (MessageQueueExtensionsTestDummy)});
            var result = (MessageQueueExtensionsTestDummy)allMessages[0].Body;

            Assert.AreEqual("Dummy message.", result.Value);
        }

        [Test]
        public void SendThrowsTooBigMessageExceptionForMessagesGreaterThan4Mb()
        {
            const int asciiCharsForMoreThan4Mb = (4 * 1024 * 1024) + 1;
            var veryBigString = new StringBuilder(asciiCharsForMoreThan4Mb);
            for (int i = 0; i < asciiCharsForMoreThan4Mb; i++)
            {
                veryBigString.Append("B");
            }
            var veryBigMessage = new MessageQueueExtensionsTestDummy { Value = veryBigString.ToString() };

            using (var transaction = new MessageQueueTransaction())
            {
                transaction.Begin();

                var ex = Assert.Throws<DataExchangeInvalidMessageException>(() => 
                    _messageQueue.Send<MessageQueueExtensionsTestDummy>(veryBigMessage, transaction));
                StringAssert.AreEqualIgnoringCase("Message not accepted due to its size, 4MB is maximum", ex.Message);
            }

            using (var transactionScope = new TransactionScope(TransactionScopeOption.Required))
            {
                var ex = Assert.Throws<DataExchangeInvalidMessageException>(() => 
                    _messageQueue.Send<MessageQueueExtensionsTestDummy>(veryBigMessage, MessageQueueTransactionType.Automatic));
                StringAssert.AreEqualIgnoringCase("Message not accepted due to its size, 4MB is maximum", ex.Message);
            }
        }

        [Test]
        public void SendTUsingMsdtc_MessageIsSent_IsPlacedInMessageQueue()
        {
            // Assign

            // Act

            using(var transactionScope = new TransactionScope(TransactionScopeOption.Required))
            {
                _messageQueue.Send<MessageQueueExtensionsTestDummy>(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, MessageQueueTransactionType.Automatic);
                transactionScope.Complete();
            }

            // Assert

            var allMessages = _messageQueue.GetAllMessages();
            allMessages[0].Formatter = new XmlMessageFormatter(new[] {typeof (MessageQueueExtensionsTestDummy)});
            var result = (MessageQueueExtensionsTestDummy)allMessages[0].Body;

            Assert.AreEqual("Dummy message.", result.Value);
        }

        [Test]
        public void Remove_OneMessageIsInTheMessageQueue_IsRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send("Dummy message.", transaction);
            transaction.Commit();

            var messageId = _messageQueue.GetAllMessages().Select(m => m.Id).First();

            // Act

            transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Remove(messageId, transaction);
            transaction.Commit();

            // Assert

            var allMessages = _messageQueue.GetAllMessages();

            Assert.AreEqual(0, allMessages.Length);
        }

        [Test]
        public void RemoveUsingMsdtc_OneMessageIsInTheMessageQueue_IsRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send("Dummy message.", transaction);
            transaction.Commit();

            var messageId = _messageQueue.GetAllMessages().Select(m => m.Id).First();

            // Act

            using(var transactionScope = new TransactionScope(TransactionScopeOption.Required))
            {
                _messageQueue.Remove(messageId, MessageQueueTransactionType.Automatic);
                transactionScope.Complete();
            }

            // Assert

            var allMessages = _messageQueue.GetAllMessages();

            Assert.AreEqual(0, allMessages.Length);
        }

        [Test]
        public void GetMessageIds_ThreeMessagesInTheMessageQueue_IdsForThreeMessagesAreReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send("Dummy message 1.", transaction);
            _messageQueue.Send("Dummy message 2.", transaction);
            _messageQueue.Send("Dummy message 3.", transaction);
            transaction.Commit();

            var messages = _messageQueue.GetAllMessages();

            // Act

            var messageIds = _messageQueue.GetMessageIds();

            // Assert

            Assert.IsTrue(messageIds.Contains(messages[0].Id), "Unable to find the message id " + messages[0].Id + ".");
            Assert.IsTrue(messageIds.Contains(messages[1].Id), "Unable to find the message id " + messages[1].Id + ".");
            Assert.IsTrue(messageIds.Contains(messages[2].Id), "Unable to find the message id " + messages[2].Id + ".");
        }

        [Test]
        public void ReceiveT_OneMessageIsInTheMessageQueue_IsRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            // Act

            transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Receive<MessageQueueExtensionsTestDummy>(transaction);
            transaction.Commit();

            // Assert

            var allMessages = _messageQueue.GetAllMessages();
            Assert.AreEqual(0, allMessages.Length);
        }

        [Test]
        public void ReceiveTUsingMsdtc_OneMessageIsInTheMessageQueue_IsRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            // Act

            using(var transactionScope = new TransactionScope(TransactionScopeOption.Required))
            {
                _messageQueue.Receive<MessageQueueExtensionsTestDummy>(MessageQueueTransactionType.Automatic);
                transactionScope.Complete();
            }

            // Assert

            var allMessages = _messageQueue.GetAllMessages();
            Assert.AreEqual(0, allMessages.Length);
        }

        [Test]
        public void ReceiveT_OneMessageIsInTheMessageQueue_IsReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            // Act

            transaction = new MessageQueueTransaction();
            transaction.Begin();
            var message = _messageQueue.Receive<MessageQueueExtensionsTestDummy>(transaction);
            transaction.Commit();

            // Assert

            Assert.AreEqual("Dummy message.", message.Value);
        }

        [Test]
        public void ReceiveTUsingMsdtc_OneMessageIsInTheMessageQueue_IsReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            // Act

            MessageQueueExtensionsTestDummy message;
            using(var transactionScope = new TransactionScope(TransactionScopeOption.Required))
            {
                message = _messageQueue.Receive<MessageQueueExtensionsTestDummy>(MessageQueueTransactionType.Automatic);
                transactionScope.Complete();
            }

            // Assert

            Assert.AreEqual("Dummy message.", message.Value);
        }

        [Test]
        public void PeekT_OneMessageIsInTheMessageQueue_IsReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            // Act

            string messageId;
            var message = _messageQueue.Peek<MessageQueueExtensionsTestDummy>(out messageId);

            // Assert

            Assert.AreEqual("Dummy message.", message.Value);
        }

        [Test]
        public void PeekT_OneMessageIsInTheMessageQueue_MessageIdIsCorrect()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            var expectedMessageId = _messageQueue.GetAllMessages().Select(x => x.Id).First();

            // Act

            string actualMessageId;
            _messageQueue.Peek<MessageQueueExtensionsTestDummy>(out actualMessageId);

            // Assert

            Assert.AreEqual(expectedMessageId, actualMessageId);
        }

        [Test]
        public void PeekT_OneMessageIsInTheMessageQueue_IsNotRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            // Act

            string messageId;
            _messageQueue.Peek<MessageQueueExtensionsTestDummy>(out messageId);

            // Assert

            var allMessages = _messageQueue.GetAllMessages();
            Assert.AreEqual(1, allMessages.Length);
        }

        [Test]
        public void PeekByIdT_OnMessageIsInTheMessageQueue_IsReturned()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            string messageId = _messageQueue.GetAllMessages().Select(x => x.Id).First();

            // Act

            var message = _messageQueue.PeekById<MessageQueueExtensionsTestDummy>(messageId);

            // Assert

            Assert.AreEqual("Dummy message.", message.Value);
        }

        [Test]
        public void PeekByIdT_OnMessageIsInTheMessageQueue_IsNotRemoved()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send(new MessageQueueExtensionsTestDummy { Value = "Dummy message." }, transaction);
            transaction.Commit();

            string messageId = _messageQueue.GetAllMessages().Select(x => x.Id).First();

            // Act

            _messageQueue.PeekById<MessageQueueExtensionsTestDummy>(messageId);

            // Assert

            var allMessages = _messageQueue.GetAllMessages();
            Assert.AreEqual(1, allMessages.Length);
        }

        [Test]
        public void ReceiveT_OneCorruptMessageInTheMessageQueue_ThrowsDataExchangeMessageException()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send("<string>Invalid message</string>", transaction);
            transaction.Commit();

            // Act

            transaction = new MessageQueueTransaction();
            transaction.Begin();
            Assert.Throws<InvalidMessageBodyException>(() => _messageQueue.Receive<MessageQueueExtensionsTestDummy>(transaction));
            transaction.Commit();
        }

        [Test]
        public void ReceiveTUsingMsdtc_OneCorruptMessageInTheMessageQueue_ThrowsDataExchangeMessageException()
        {
            // Assign

            var transaction = new MessageQueueTransaction();
            transaction.Begin();
            _messageQueue.Send("<string>Invalid message</string>", transaction);
            transaction.Commit();

            // Act

            using(var transactionScope = new TransactionScope(TransactionScopeOption.Required))
            {
                Assert.Throws<InvalidMessageBodyException>(() => _messageQueue.Receive<MessageQueueExtensionsTestDummy>(MessageQueueTransactionType.Automatic));
                transactionScope.Complete();
            }
        }
    }
}
