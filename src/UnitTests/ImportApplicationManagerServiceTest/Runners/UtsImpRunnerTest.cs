using System;
using Moq;
using NUnit.Framework;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.EventLogging.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.IO.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;
using System.IO;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerServiceTest.Runners
{
    [TestFixture]
    public class UtsImpRunnerTest
    {
        private const int EXIT_CODE_SUCCESS = 0;

        private Mock<IProcessRunner> _processRunnerMock;
        private ImportSettings _setting;
        private Mock<IFileUtility> _fileUtilityMock;
        private Mock<IImportEventLogger> _importEventLoggerMock;

        private UtsImpRunner _utsImpRunner;

        [SetUp]
        public void SetUp()
        {
            _fileUtilityMock = new Mock<IFileUtility>();
            _importEventLoggerMock = new Mock<IImportEventLogger>();
            _processRunnerMock = new Mock<IProcessRunner>();            
            _setting = new ImportSettings();
            _setting.EdiImportDirectory = @"C:\temporary\import\directory";
            _utsImpRunner = new UtsImpRunner(_fileUtilityMock.Object, _importEventLoggerMock.Object,
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

            const string importFileName = @"C:/Irrelevant/path/to/file.xml";
            var expectedArguments = String.Format("-f {0} -R {1} -Q {2}", importFileName, message.RoutingAddress,
                message.ExternalReference);
            
			_fileUtilityMock.Setup(x => x.SaveImportToFile(It.IsAny<DataExchangeImportMessage>(), It.IsAny<String>(), _utsImpRunner.FileEncoding))                .Returns(importFileName);

            var exePath = Path.Combine(IccConfiguration.IccHome, @"bin\utsimp.exe");
            _processRunnerMock.Setup(
                x => x.Run(exePath, @"C:\temporary\import\directory\UTS\new", expectedArguments))
                .Returns(EXIT_CODE_SUCCESS);

            _utsImpRunner.Run(message);

            _processRunnerMock.VerifyAll();
        }
    }
}