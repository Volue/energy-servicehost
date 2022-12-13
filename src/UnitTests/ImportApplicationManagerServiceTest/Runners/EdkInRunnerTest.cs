using System;
using System.IO;
using Moq;
using NUnit.Framework;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerServiceTest.Runners
{
    [TestFixture]
    public class EdkInRunnerTest
    {
        private const int EXIT_CODE_SUCCESS = 0;

        private Mock<IProcessRunner> _processRunnerMock;
        private ImportSettings _setting;
        private Mock<IFileUtility> _fileUtilityMock;
        private Mock<IImportEventLogger> _importEventLoggerMock;

        private EdkInRunner _edkInRunner;

        [SetUp]
        public void SetUp()
        {
            _fileUtilityMock = new Mock<IFileUtility>();
            _importEventLoggerMock = new Mock<IImportEventLogger>();
            _processRunnerMock = new Mock<IProcessRunner>();
            _setting = new ImportSettings();

            _edkInRunner = new EdkInRunner(_fileUtilityMock.Object, _importEventLoggerMock.Object,
                _processRunnerMock.Object, () => _setting);
        }

        [Test]
        public void Run_ProcessRunnerIsCalled_WithSpecifiedArgumentsAndImpExpApplicationPath()
        {
            var message = new DataExchangeImportMessage
                {
                    RoutingAddress = "RoutingAddress",
                    ExternalReference = "ExternalReference"
                };

            _setting.EdkFilesDirectory = @"C:\edk\file\directory";
            const string importFileName = @"C:/Irrelevant/path/to/file.xml";

            var expectedArguments = String.Format("-f {0} -R {1} -Q {2}", importFileName, message.RoutingAddress, message.ExternalReference);

            _fileUtilityMock.Setup(x => x.SaveImportToFile(It.IsAny<DataExchangeImportMessage>(), It.IsAny<String>(), _edkInRunner.FileEncoding))
                .Returns(importFileName);
            var exePath = Path.Combine(IccConfiguration.IccHome, @"bin\edkin.exe");
            _processRunnerMock.Setup(
                x => x.Run(exePath, _setting.EdkFilesDirectory, expectedArguments))
                .Returns(EXIT_CODE_SUCCESS);

            _edkInRunner.Run(message);

            _processRunnerMock.VerifyAll();
        }
    }
}