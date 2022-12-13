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
    public class NbsImpRunnerTest
    {
        private const int EXIT_CODE_SUCCESS = 0;
        
        private Mock<IProcessRunner> _processRunnerMock;
        private ImportSettings _setting;
        private Mock<IFileUtility> _fileUtilityMock;
        private Mock<IImportEventLogger> _importEventLoggerMock;

        private XmlElImpRunner _xmlElImpRunner;

        [SetUp]
        public void SetUp()
        {
            _fileUtilityMock = new Mock<IFileUtility>();
            _importEventLoggerMock = new Mock<IImportEventLogger>();
            _processRunnerMock = new Mock<IProcessRunner>();
            _setting = new ImportSettings();

            _xmlElImpRunner = new XmlElImpRunner(_fileUtilityMock.Object, _importEventLoggerMock.Object,
                _processRunnerMock.Object, () => _setting);
        }

        [Test]
        public void Run_ProcessRunnerIsCalled_WithSpecifiedArgumentsAndImpExpApplicationPath()
        {
            const string importFileName = @"C:/Irrelevant/path/to/file.xml";            
            var importExportApplication = Path.Combine(IccConfiguration.IccHome, @"bin\XmlElImpExp\XmlElImp.exe");
            var expectedArguments = String.Format("-f {0}", importFileName);
            _setting.EdiImportDirectory = @"C:\temporary\import\directory";

			_fileUtilityMock.Setup(x => x.SaveImportToFile(It.IsAny<DataExchangeImportMessage>(), It.IsAny<String>(),_xmlElImpRunner.FileEncoding)).Returns(importFileName);
            _processRunnerMock.Setup(x => x.Run(It.IsAny<String>(),It.IsAny<String>(),It.IsAny<String>()))
                .Returns(EXIT_CODE_SUCCESS);

            _xmlElImpRunner.Run(new DataExchangeImportMessage());

            _processRunnerMock.VerifyAll();
            _fileUtilityMock.VerifyAll();
        }
    }
}