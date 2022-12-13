using System;
using Amqp;
using Moq;
using NUnit.Framework;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.EcpAmqpDataExchangeManager.EcpAmqpDataExchangeManagerService.Modules;

namespace EcpAmqpDataExchangeManagerServiceTest
{
    [TestFixture, System.ComponentModel.Category("Unit tests")]
    class EcpAmqpLogicTests
    {
        private EcpAmqpLogic _instance;
        private Mock<IServiceEventLogger> _mockServiceEventLogger;

        private const string RoutingAddress = "ECPAMQP:";

        [SetUp]
        public void SetUp()
        {
            var apiMock = new Mock<IDataExchangeApi>();
            _mockServiceEventLogger = new Mock<IServiceEventLogger>();
            var mockImportApplication = new Mock<IImportApplication>();
            _instance = new EcpAmqpLogic(() => apiMock.Object, _mockServiceEventLogger.Object, mockImportApplication.Object);
        }

        [TestCase("TRE_active_ack","High","Swissgrid")]
        [TestCase("RD-active-ack","Normal",null)]
        public void BuildDataExchangeImportMessage_ParameterTest(string businessType, string priority, string receiver)
        {
            // Assign
            var priorityDesc = "Priority," + priority;
            _instance.InitializePriorityList(priorityDesc);

            var payload = "Test payload";
            if (!string.IsNullOrEmpty(priority))
                payload += priorityDesc;
            var amqpMsg = CreateAmqpMessage(businessType, payload, receiver);

            // Act
            var importMsg = _instance.BuildDataExchangeImportMessage(amqpMsg, payload);

            // Assert
            Assert.AreEqual(amqpMsg.Properties.CorrelationId,importMsg.ExternalReference);
            Assert.AreEqual(priority,importMsg.Priority);
            Assert.AreEqual(RoutingAddress + amqpMsg.GetBusinessType(),importMsg.RoutingAddress);   // ToDo: To be aligned with the export module this should be ECPAMQP:<EICaddress> to sender for correct export of an acknowledgement. Currently no ack is sent in Switzerland/Swissgrid
            Assert.AreEqual("ENTSOE", importMsg.Protocol);
            Assert.AreEqual(payload,importMsg.GetMessageData(true));
            Assert.AreEqual(amqpMsg.GetReceiverCode(),importMsg.ReceiverName);
            Assert.AreEqual(amqpMsg.GetMessageType(),importMsg.SubAddress); // Swissgrid uses messageType.
        }

        #region HelperFunctions

        private Message CreateAmqpMessage(string businessType, string payload, string receiver)
        {
            var msg = MessageBuilder.CreateMessage(businessType,payload).WithCorrelationId(Guid.NewGuid());
            if (!string.IsNullOrEmpty(receiver))
                msg.WithReceiverAddress(receiver);
            return msg;
        }

        #endregion
    }
}
