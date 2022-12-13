using System;
using Moq;
using NUnit.Framework;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Enums;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;
using ImportApplicationService = Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerService;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerServiceTest
{
    [TestFixture]
    public class ImportApplicationManagerServiceTest
    {
        private ImportApplicationService.ImportApplicationManagerService _importApplicationManagerService;
        private Mock<IDataExchangeQueueTransaction> _dataExchangeApiTransactionMock;
        private Mock<IDataExchangeApi> _dataExchangeApiMock;
        private Mock<IImportApplicationRunner> _importApplicationRunnerMock;
        private Mock<IServiceEventLogger> _serviceEventLogger;
        private ImportSettings _settings;

        private const string PRIORITY = "NORMAL";
        private const string PROTOCOL = "MSCONS";
        private const string PROTOCOL_WITHOUT_APPLICATION_RUNNER = "REQOTE";
        private const ImportApplicationType IMPORT_APPLICATION = ImportApplicationType.EdiImp;
        private const string ROUTING_ADDRESS = "STANDARDMSMQ:dummyaddress:1234";
        private const string COUNTRY = "SWE";
        private const string MESSAGE_DATA = "Dummy message data";

        [SetUp]
        public void SetUp()
        {
            _dataExchangeApiTransactionMock = new Mock<IDataExchangeQueueTransaction>();
            _dataExchangeApiTransactionMock.SetupAllProperties();

            _dataExchangeApiMock = new Mock<IDataExchangeApi>();
            _dataExchangeApiMock.Setup(x => x.GetTransaction(It.IsAny<DataExchangeQueueTransactionType>()))
                .Returns(_dataExchangeApiTransactionMock.Object);

            _importApplicationRunnerMock = new Mock<IImportApplicationRunner>();
            _importApplicationRunnerMock.SetupAllProperties();

            _serviceEventLogger = new Mock<IServiceEventLogger>();
            _serviceEventLogger.SetupAllProperties();

            _settings = new ImportSettings();

            _importApplicationManagerService = new ImportApplicationService.ImportApplicationManagerService(_serviceEventLogger.Object, _dataExchangeApiMock.Object, new ImportApplication(_serviceEventLogger.Object, new ImportApplicationRunnerResolver(name =>
                {
                    if(name == IMPORT_APPLICATION.ToString())
                    {
                        return _importApplicationRunnerMock.Object;
                    }
                    return null;
                }, () => _settings)));
            _importApplicationManagerService.Initialize();
            _importApplicationManagerService.RequestStop(); // This makes sure the RunIteration method only executes once
        }

        [Test]
        public void RunIteration_NextImportMessageIsProcessed_BeginIsCalledOnTransaction()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA,null);

            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);
            
            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);
            
            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Begin());
        }

        [Test]
        public void RunIteration_NextImportMessageIsProcessed_CommitIsCalledOnTransaction()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA,null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
        }

        [Test]
        public void RunIteration_NextImportMessageIsNotAvailable_CommitIsNotCalledOnTransaction()
        {
            // Assign

            var message = default(DataExchangeImportMessage);

            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Commit(), Times.Never());
        }

        [Test]
        public void RunIteration_NextImportMessageIsProcessed_ImportMessageIsDequeued()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA,null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiMock.Verify(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void RunIteration_NextImportMessageIsProcessed_IsImportedUsingImportApplicationRunner()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA,null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _importApplicationRunnerMock.Verify(x => x.Run(message));
        }
        
        [Test]
        public void RunIteration_NextImportMessageFailsToBeImported_CommitIsCalledOnTransaction()
        {
            // Assign
            _importApplicationRunnerMock.Setup(x => x.Run(It.IsAny<DataExchangeImportMessage>())).Throws(new DataExchangeImportRunnerFailedException(""));
            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA,null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act
            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert
            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
            _dataExchangeApiMock.Verify(api => api.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.Never(), "Runner failed. Message shall not be placed on Error import queue. Fixing and further handling through MessageLogSearch.");
        }

        [Test]
        public void RunIteration_NextImportMessageIsProcessed_ReadQueueTimeOutIsGreaterThanZero()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiMock.Verify(x => x.DequeueImportMessage(It.IsInRange(TimeSpan.FromSeconds(1), TimeSpan.FromSeconds(int.MaxValue), Range.Inclusive), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void RunIteration_NextImportMessageIsProcessed_TheTransactionIsOfTypeDequeue()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiMock.Verify(x => x.GetTransaction(DataExchangeQueueTransactionType.Dequeue));
        }

        [Test]
        public void RunIteration_NextProcessedHasNoProtocol_MessageIsDequeuedButNotProcessed()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS
                };
            message.SetMessageData(MESSAGE_DATA, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _importApplicationRunnerMock.Verify(x => x.Run(It.IsAny<DataExchangeImportMessage>()), Times.Never());
        }
        
        [Test]
        public void RunIteration_NextProcessedHasNoProtocol_MessageIsPlacedInTheErrorQueue()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS
                };
            message.SetMessageData(MESSAGE_DATA, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()));
        }

        [Test]
        public void RunIteration_NextProcessedHasNoProtocol_CommitIsCalledOnTransaction()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS
                };
            message.SetMessageData(MESSAGE_DATA, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
        }

        [Test]
        public void RunIteration_NextProcessedHasInvalidProtocol_MessageIsDequeuedButNotProcessed()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = "NONEXISTINGPROTOCOL",
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _importApplicationRunnerMock.Verify(x => x.Run(It.IsAny<DataExchangeImportMessage>()), Times.Never());
        }
        
        [Test]
        public void RunIteration_NextProcessedHasInvalidProtocol_MessageIsPlacedInTheErrorQueue()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = "NONEXISTINGPROTOCOL",
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()));
        }
        
        [Test]
        public void RunIteration_NextProcessedHasInvalidProtocol_CommitIsCalledOnTransaction()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = "NONEXISTINGPROTOCOL",
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
        }

        [Test]
        public void RunIteration_NextMessageHasEmptyMessageData_MessageIsDiscarded()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(string.Empty, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _importApplicationRunnerMock.Verify(x => x.Run(It.IsAny<DataExchangeImportMessage>()), Times.Never());
        }

        [Test]
        public void RunIteration_NextMessageHasEmptyMessageData_MessageIsNotPlacedInTheErrorQueue()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(string.Empty, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.Never());
        }
        
        [Test]
        public void RunIteration_NextMessageHasEmptyMessageData_CommitIsCalledOnTransaction()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(string.Empty, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
        }

        [Test]
        public void RunIteration_NextMessageHasNullMessageData_MessageIsDiscarded()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY,
                };
            message.SetMessageData(null, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _importApplicationRunnerMock.Verify(x => x.Run(It.IsAny<DataExchangeImportMessage>()), Times.Never());
        }
        
        [Test]
        public void RunIteration_NextMessageHasNullMessageData_MessageIsNotPlacedInTheErrorQueue()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(null, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.Never());
        }
        
        [Test]
        public void RunIteration_NextMessageHasNullMessageData_CommitIsCalledOnTransaction()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData(null, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
        }

        [Test]
        public void RunIteration_NextMessageHasBlankMessageData_MessageIsDiscarded()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData("   ", null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _importApplicationRunnerMock.Verify(x => x.Run(It.IsAny<DataExchangeImportMessage>()), Times.Never());
        }

        [Test]
        public void RunIteration_NextMessageHasBlankMessageData_MessageIsNotPlacedInTheErrorQueue()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData("   ", null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiMock.Verify(x => x.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.Never());
        }

        [Test]
        public void RunIteration_NextMessageHasBlankMessageData_CommitIsCalledOnTransaction()
        {
            // Assign

            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL,
                    Country = COUNTRY
                };
            message.SetMessageData("   ", null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert

            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
        }

        [Test]
        public void RunIteration_NextMessageIsInvalid_MessageIsDiscarded()
        {
            // Assign

            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Throws(new DataExchangeInvalidMessageException("123"));

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert
            
            _importApplicationRunnerMock.Verify(x => x.Run(It.IsAny<DataExchangeImportMessage>()), Times.Never());
        }

        [Test]
        public void RunIteration_NextMessageIsInvalid_MessageIsPlacedInErrorQueue()
        {
            /* An invalid message should not be left on the import queue, so the import process never progresses.
             * It should be placed on the error queue and handled manually.
             */
            // Assign

            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Throws(new DataExchangeInvalidMessageException("123"));

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert
            
            _dataExchangeApiMock.Verify(x => x.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.AtLeastOnce());
        }

        [Test]
        public void RunIteration_NextMessageIsInvalid_CommitIsCalledOnTransaction()
        {
            // Assign

            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Throws(new DataExchangeInvalidMessageException("123"));

            // Act

            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert
            
            _dataExchangeApiTransactionMock.Verify(x => x.Commit());
        }

        [Test]
        public void RunIteration_NoImportApplicationRunnerFound_ThrowsDataExchangeImportRunnerNotFoundException()
        {
            // Assign
            var message = new DataExchangeImportMessage
                {
                    Priority = PRIORITY,
                    RoutingAddress = ROUTING_ADDRESS,
                    Protocol = PROTOCOL_WITHOUT_APPLICATION_RUNNER,
                    Country = COUNTRY
                };
            message.SetMessageData(MESSAGE_DATA,null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>()))
                .Returns(message);

            // Act and Assert
            bool actualWorkDone;
            Assert.Throws<DataExchangeConfigurationException>(() => _importApplicationManagerService.RunIteration(out actualWorkDone));
            _dataExchangeApiMock.Verify(x => x.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.Never(),"");
        }

        [Test]
        public void RunIteration_ImportApplicationRunnerTimeout_MessageIsNotPlacedInErrorQueue()
        {
            // Timeout on runners shall not place messages in Error queue. Such messages must be handled from MessageLogSearch.
            // Assign
            var message = new DataExchangeImportMessage
            {
                Priority = PRIORITY,
                RoutingAddress = ROUTING_ADDRESS,
                Protocol = PROTOCOL,
                Country = COUNTRY
            };
            message.SetMessageData(MESSAGE_DATA, null);
            _dataExchangeApiMock.Setup(x => x.DequeueImportMessage(It.IsAny<TimeSpan>(), It.IsAny<IDataExchangeQueueTransaction>())).Returns(message);
            _importApplicationRunnerMock.Setup(x => x.Run(message)).Throws<TimeoutException>();

            // Act
            bool actualWorkDone;
            _importApplicationManagerService.RunIteration(out actualWorkDone);

            // Assert
            //_dataExchangeApiMock.Verify(x => x.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.AtLeastOnce());
            _dataExchangeApiMock.Verify(x => x.EnqueueErroneousImportMessage(It.IsAny<DataExchangeImportMessage>(), It.IsAny<IDataExchangeQueueTransaction>()), Times.Never(), "Timeout on messages shall not place message in Error queue.");
        }
    }
}
