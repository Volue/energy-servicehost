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

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerServiceTest.Runners.Abstract
{
    [TestFixture]
    public class BaseImportRunnerTest
    {
        private const int EXIT_CODE_SUCCESS = 0;
        private const int EXIT_CODE_FAIL = -1;

        private Mock<IProcessRunner> _processRunnerMock;
        private ImportSettings _setting;
        private Mock<IFileUtility> _fileUtilityMock;
        private Mock<IImportEventLogger> _importEventLoggerMock;

        private XmlElImpRunner _xmlElImpRunner;
        private EdkInRunner _edkInRunner;
        private EdiImpRunner _ediImpRunner;
        private UtsImpRunner _utsImpRunner;

        [SetUp]
        public void SetUp()
        {
            _fileUtilityMock = new Mock<IFileUtility>();
            _importEventLoggerMock = new Mock<IImportEventLogger>();
            _processRunnerMock = new Mock<IProcessRunner>();
            _setting = new ImportSettings
                {
                    EdkFilesDirectory = @"C:\edk\file\directory",
                    EdiImportDirectory = @"C:\edk\file\directory"
                };

            _xmlElImpRunner = new XmlElImpRunner(_fileUtilityMock.Object, _importEventLoggerMock.Object,
                _processRunnerMock.Object, () => _setting);

            _edkInRunner = new EdkInRunner(_fileUtilityMock.Object, _importEventLoggerMock.Object,
                _processRunnerMock.Object, () => _setting);

            _ediImpRunner = new EdiImpRunner(_fileUtilityMock.Object, _importEventLoggerMock.Object,
                _processRunnerMock.Object, () => _setting);

            _utsImpRunner = new UtsImpRunner(_fileUtilityMock.Object, _importEventLoggerMock.Object,
                _processRunnerMock.Object, () => _setting);
        }

        [Test]
        public void Run_ProcessRunnerReturnsEXIT_CODE_SUCCESS_ThenLogSuccessfulImportIsCalled()
        {
            RunSuccessPath(_xmlElImpRunner);
            RunSuccessPath(_ediImpRunner);
            RunSuccessPath(_edkInRunner);
            RunSuccessPath(_utsImpRunner);
        }

        private void RunSuccessPath(BaseImportRunner runner)
        {
            _processRunnerMock.Setup(
                x => x.Run(It.IsAny<String>(), It.IsAny<String>(), It.IsAny<String>()))
                .Returns(EXIT_CODE_SUCCESS);

            runner.Run(new DataExchangeImportMessage());

            _importEventLoggerMock.Verify(x => x.LogSuccessfulImport(It.IsAny<DataExchangeImportMessage>()));
        }

        [Test]
        public void Run_ProcessRunnerReturnsEXIT_CODE_FAIL_ThenLogFailedImportIsCalledAndExceptionThrow()
        {
            RunFailPath(_xmlElImpRunner);
            RunFailPath(_ediImpRunner);
            RunFailPath(_edkInRunner);
            RunFailPath(_utsImpRunner);
        }

        private void RunFailPath(BaseImportRunner runner)
        {
            const string importFileName = @"C:/Irrelevant/path/to/file.xml";
            _setting.EdiImportDirectory = @"C:\Irrelevant\path";

			_fileUtilityMock.Setup(x => x.SaveImportToFile(It.IsAny<DataExchangeImportMessage>(), It.IsAny<String>(),runner.FileEncoding))                .Returns(importFileName);

            _processRunnerMock.Setup(
                x => x.Run(It.IsAny<String>(), It.IsAny<String>(), It.IsAny<String>()))
                .Returns(EXIT_CODE_FAIL);

            Assert.Throws<DataExchangeImportRunnerFailedException>(
                () => runner.Run(new DataExchangeImportMessage()));

            _importEventLoggerMock.Verify(x => x.LogFailedImport(It.IsAny<DataExchangeImportMessage>()));
        }
    }
}