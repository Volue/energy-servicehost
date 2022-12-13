using System;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.Msmq;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApiTest.Msmq
{
    [TestFixture]
    public class MsmqPathFactoryTest
    {
        private const string PRIVATE = @"Private$\";
        private const string DEFAULT_EXPORT_QUEUE_PREFIX = "ICC_EXPORT_";
        private const string DEFAULT_IMPORT_QUEUE_PREFIX = "ICC_IMPORT";
        private const string MODULE_INSTANCE_NAME = "DummyName";

        private IDataExchangeSettingsFactory _dataExchangeSettingsFactory;
        private MsmqPathFactory _queuePathFactory;
        private IDataExchangeSettingsFactory _nullDataExchangeSettingsFactory;
        private MsmqPathFactory _nullQueuePathFactory;

        private string GetExpectedFullPathForExport(string firstPart)
        {
            return firstPart + PRIVATE + DEFAULT_EXPORT_QUEUE_PREFIX + MODULE_INSTANCE_NAME;
        }

        private string GetExpectedFullPathForImport(string firstPart)
        {
            return firstPart + PRIVATE + DEFAULT_IMPORT_QUEUE_PREFIX;
        }

        private string GetExpectedFullPathForCustomImport(string firstPart)
        {
            return firstPart + PRIVATE + MODULE_INSTANCE_NAME;
        }

        [SetUp]
        public void SetUp()
        {
            DataExchangeSettingsFactory.Testing = true;
            _nullDataExchangeSettingsFactory = new DataExchangeSettingsFactory();
            _nullQueuePathFactory = new MsmqPathFactory(_nullDataExchangeSettingsFactory);

            DataExchangeSettingsFactory.ExportMachineNames = ".;;   ;dummymachine; dummymachine ;192.168.0.1; 192.168.0.1 ";
            DataExchangeSettingsFactory.ImportMachineNames = ".;;   ;dummymachine; dummymachine ;192.168.0.1; 192.168.0.1 ";
            _dataExchangeSettingsFactory = new DataExchangeSettingsFactory();
            _queuePathFactory = new MsmqPathFactory(_dataExchangeSettingsFactory);
        }

        [Test]
        public void CreateInternalExportQueuePath_MachineNameIsDot_FullPathBeginsWithDot()
        {
            // Assign
            _dataExchangeSettingsFactory.ExportIndex = 0;

            // Act
            var fullPath = _queuePathFactory.CreateInternalExportQueuePath("DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForExport(".\\"), fullPath);
        }
        
        [Test]
        public void CreateInternalImportQueuePath_MachineNameIsDot_FullPathBeginsWithDot()
        {
            // Assign
            _dataExchangeSettingsFactory.ImportIndex = 0;

            // Act
            var fullPath = _queuePathFactory.CreateInternalImportQueuePath().FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForImport(".\\"), fullPath);
        }

        [Test]
        public void CreateCustomImportQueuePath_MachineNameIsDot_FullPathBeginsWithDot()
        {
            // Assign
            _dataExchangeSettingsFactory.ImportIndex = 0;

            // Act
            var fullPath = _queuePathFactory.CreateCustomImportQueuePath(".", "DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForCustomImport(".\\"), fullPath);
        }

        [Test]
        public void CreateInternalExportQueuePath_MachineNameIsEmpty_FullPathBeginsWithDot()
        {
            // Assign
            _dataExchangeSettingsFactory.ExportIndex = 1;

            // Act
            var fullPath = _queuePathFactory.CreateInternalExportQueuePath("DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForExport(".\\"), fullPath);
        }

        [Test]
        public void CreateInternalImportQueuePath_MachineNameIsEmpty_FullPathBeginsWithDot()
        {
            // Assign
            _dataExchangeSettingsFactory.ImportIndex = 1;

            // Act
            var fullPath = _queuePathFactory.CreateInternalImportQueuePath().FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForImport(".\\"), fullPath);
        }

        [Test]
        public void CreateCustomImportQueuePath_MachineNameIsEmpty_FullPathBeginsWithDot()
        {
            // Assign

            // Act

            var fullPath = _queuePathFactory.CreateCustomImportQueuePath(string.Empty, "DummyName").FullPath;

            // Assert

            Assert.AreEqual(GetExpectedFullPathForCustomImport(".\\"), fullPath);
        }
        [Test]
        public void CreateInternalExportQueuePath_MachineNameIsNull_FullPathBeginsWithDot()
        {
            // Assign

            // Act
            var fullPath = _nullQueuePathFactory.CreateInternalExportQueuePath("DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForExport(".\\"), fullPath);
        }

        [Test]
        public void CreateInternalImportQueuePath_MachineNameIsNull_FullPathBeginsWithDot()
        {
            // Assign

            // Act
            var fullPath = _nullQueuePathFactory.CreateInternalImportQueuePath().FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForImport(".\\"), fullPath);
        }

        [Test]
        public void CreateCustomImportQueuePath_MachineNameIsNull_FullPathBeginsWithDot()
        {
            // Assign

            // Act
            var fullPath = _nullQueuePathFactory.CreateCustomImportQueuePath(null, "DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForCustomImport(".\\"), fullPath);
        }

        [Test]
        public void CreateInternalExportQueuePath_MachineNameIsOnlyWhiteSpace_FullPathBeginsWithDot()
        {
            // Assign
            _dataExchangeSettingsFactory.ExportIndex = 2;

            // Act
            var fullPath = _queuePathFactory.CreateInternalExportQueuePath("DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForExport(".\\"), fullPath);
        }

        [Test]
        public void CreateInternalImportQueuePath_MachineNameIsOnlyWhiteSpace_FullPathBeginsWithDot()
        {
            // Assign
            _dataExchangeSettingsFactory.ImportIndex = 2;

            // Act
            var fullPath = _queuePathFactory.CreateInternalImportQueuePath().FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForImport(".\\"), fullPath);
        }

        [Test]
        public void CreateCustomImportQueuePath_MachineNameIsOnlyWhiteSpace_FullPathBeginsWithDot()
        {
            // Assign

            // Act
            var fullPath = _queuePathFactory.CreateCustomImportQueuePath("   ", "DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForCustomImport(".\\"), fullPath);
        }

        [Test]
        public void CreateInternalExportQueuePath_MachineNameIsOsName_FullPathBeginsWithFormatNameDirectOs()
        {
            // Assign
            _dataExchangeSettingsFactory.ExportIndex = 3;

            // Act

            var fullPath = _queuePathFactory.CreateInternalExportQueuePath("DummyName").FullPath;

            // Assert

            Assert.AreEqual(GetExpectedFullPathForExport("FormatName:Direct=OS:dummymachine\\"), fullPath);
        }

        [Test]
        public void CreateInternalImportQueuePath_MachineNameIsOsName_FullPathBeginsWithFormatNameDirectOs()
        {
            // Assign
            _dataExchangeSettingsFactory.ImportIndex = 3;

            // Act
            var fullPath = _queuePathFactory.CreateInternalImportQueuePath().FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForImport("FormatName:Direct=OS:dummymachine\\"), fullPath);
        }

        [Test]
        public void CreateCustomImportQueuePath_MachineNameIsOsName_FullPathBeginsWithFormatNameDirectOs()
        {
            // Assign

            // Act
            var fullPath = _queuePathFactory.CreateCustomImportQueuePath("dummymachine", "DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForCustomImport("FormatName:Direct=OS:dummymachine\\"), fullPath);
        }

        [Test]
        public void CreateInternalExportQueuePath_MachineNameIsOsNameWithWhiteSpace_FullPathBeginsWithFormatNameDirectOs()
        {
            // Assign
            _dataExchangeSettingsFactory.ExportIndex = 4;

            // Act
            var fullPath = _queuePathFactory.CreateInternalExportQueuePath("DummyName").FullPath;

            // Assert

            Assert.AreEqual(GetExpectedFullPathForExport("FormatName:Direct=OS:dummymachine\\"), fullPath);
        }

        [Test]
        public void CreateInternalImportQueuePath_MachineNameIsOsNameWithWhiteSpace_FullPathBeginsWithFormatNameDirectOs()
        {
            // Assign
            _dataExchangeSettingsFactory.ImportIndex = 4;

            // Act
            var fullPath = _queuePathFactory.CreateInternalImportQueuePath().FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForImport("FormatName:Direct=OS:dummymachine\\"), fullPath);
        }

        [Test]
        public void CreateCustomImportQueuePath_MachineNameIsOsNameWithWhiteSpace_FullPathBeginsWithFormatNameDirectOs()
        {
            // Assign

            // Act
            var fullPath = _queuePathFactory.CreateCustomImportQueuePath(" dummymachine ", "DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForCustomImport("FormatName:Direct=OS:dummymachine\\"), fullPath);
        }

        [Test]
        public void CreateInternalExportQueuePath_MachineNameIsIpAddress_FullPathBeginsWithFormatNameDirectTcp()
        {
            // Assign
            _dataExchangeSettingsFactory.ExportIndex = 5;

            // Act

            var fullPath = _queuePathFactory.CreateInternalExportQueuePath("DummyName").FullPath;

            // Assert

            Assert.AreEqual(GetExpectedFullPathForExport("FormatName:Direct=TCP:192.168.0.1\\"), fullPath);
        }

        [Test]
        public void CreateInternalImportQueuePath_MachineNameIsIpAddress_FullPathBeginsWithFormatNameDirectTcp()
        {
            // Assign
            _dataExchangeSettingsFactory.ImportIndex = 5;

            // Act
            var fullPath = _queuePathFactory.CreateInternalImportQueuePath().FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForImport("FormatName:Direct=TCP:192.168.0.1\\"), fullPath);
        }

        [Test]
        public void CreateCustomImportQueuePath_MachineNameIsIpAddress_FullPathBeginsWithFormatNameDirectTcp()
        {
            // Assign

            // Act
            var fullPath = _queuePathFactory.CreateCustomImportQueuePath("192.168.0.1", "DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForCustomImport("FormatName:Direct=TCP:192.168.0.1\\"), fullPath);
        }

        [Test]
        public void CreateInternalExportQueuePath_MachineNameIsIpAddressWithWhiteSpace_FullPathBeginsWithFormatNameDirectTcp()
        {
            // Assign
            _dataExchangeSettingsFactory.ExportIndex = 6;

            // Act
            var fullPath = _queuePathFactory.CreateInternalExportQueuePath("DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForExport("FormatName:Direct=TCP:192.168.0.1\\"), fullPath);
        }

        [Test]
        public void CreateInternalImportQueuePath_MachineNameIsIpAddressWithWhiteSpace_FullPathBeginsWithFormatNameDirectTcp()
        {
            // Assign
            _dataExchangeSettingsFactory.ImportIndex = 6;

            // Act
            var fullPath = _queuePathFactory.CreateInternalImportQueuePath().FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForImport("FormatName:Direct=TCP:192.168.0.1\\"), fullPath);
        }

        [Test]
        public void CreateCustomImportQueuePath_MachineNameIsIpAddressWithWhiteSpace_FullPathBeginsWithFormatNameDirectTcp()
        {
            // Assign

            // Act
            var fullPath = _queuePathFactory.CreateCustomImportQueuePath(" 192.168.0.1 ", "DummyName").FullPath;

            // Assert
            Assert.AreEqual(GetExpectedFullPathForCustomImport("FormatName:Direct=TCP:192.168.0.1\\"), fullPath);
        }
    }
}
