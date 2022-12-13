using System;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerServiceTest
{
    [TestFixture]
    public class SettingsRepositoryTest
    {
        private SettingsRepository _settingsRepository;

        [SetUp]
        public void SetUp()
        {
            // Clear all environment variables used by the SettingsRepository
            Environment.SetEnvironmentVariable("TRADE_HANDLES_INNMATING", "");
            Environment.SetEnvironmentVariable("TRADE_AVREGN_TO_EDKIN", "");
            Environment.SetEnvironmentVariable("TRADE_TIMER_TO_EDKIN", "");
            Environment.SetEnvironmentVariable("TRADE_STATNETT_ADDRESS", "");
            Environment.SetEnvironmentVariable("TRADE_EDIIMP_PCODES", "");
            Environment.SetEnvironmentVariable("EDK_FILES_DIR", "");
            Environment.SetEnvironmentVariable("ICC_IMPORT_DIR", "");
            
            _settingsRepository = new SettingsRepository();
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeHandlesInmatningIsTrue_SettingsTradeHandlesInmatningIsTrue()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_HANDLES_INNMATING", "TRUE");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsTrue(settings.TradeHandlesInnmating);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeHandlesInnmatingIsFalse_SettingsTradeHandlesInnmatingIsFalse()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_HANDLES_INNMATING", "FALSE");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsFalse(settings.TradeHandlesInnmating);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeHandlesInnmatingIsEmpty_SettingsTradeHandlesInnmatingIsFalse()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_HANDLES_INNMATING", "");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsFalse(settings.TradeHandlesInnmating);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeAvregnToEdkinIsTrue_SettingsTradeAvregnToEdkinIsTrue()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_AVREGN_TO_EDKIN", "TRUE");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsTrue(settings.TradeAvregnToEdkin);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeAvregnToEdkinIsFalse_SettingsTradeAvregnToEdkinIsFalse()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_AVREGN_TO_EDKIN", "FALSE");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsFalse(settings.TradeAvregnToEdkin);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeAvregnToEdkinIsEmpty_SettingsTradeAvregnToEdkinIsFalse()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_AVREGN_TO_EDKIN", "");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsFalse(settings.TradeAvregnToEdkin);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeTimerToEdkinIsTrue_SettingsTradeTimerToEdkinIsTrue()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_TIMER_TO_EDKIN", "TRUE");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsTrue(settings.TradeTimerToEdkin);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeTimerToEdkinIsFalse_SettingsTradeTimerToEdkinIsFalse()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_TIMER_TO_EDKIN", "FALSE");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsFalse(settings.TradeTimerToEdkin);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeTimerToEdkinIsEmpty_SettingsTradeTimerToEdkinFalse()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_TIMER_TO_EDKIN", "");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsFalse(settings.TradeTimerToEdkin);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeStatnettAddressIsSet_SettingsTradeStatnettAddressHasValue()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_STATNETT_ADDRESS", "DUMMY_ADDRESS");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.AreEqual("DUMMY_ADDRESS", settings.TradeStatnettAddress);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeStatnettAddressIsEmpty_SettingsTradeStatnettAddressIsEmpty()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_STATNETT_ADDRESS", "");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsEmpty(settings.TradeStatnettAddress);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeEdiimpPcodesContainsOneValue_SettingsTradeEdiimpProductCodesContainsOneValue()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_EDIIMP_PCODES", "DUMMY_PRODUCT_CODE");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.AreEqual("DUMMY_PRODUCT_CODE", settings.TradeEdiimpProductCodes[0]);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeEdiimpPcodesContainsThreeValues_SettingsTradeEdiimpProductCodesContainsThreeValues()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_EDIIMP_PCODES", "DUMMY_PRODUCT_CODE_1, DUMMY_PRODUCT_CODE_2, DUMMY_PRODUCT_CODE_3");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.AreEqual(3, settings.TradeEdiimpProductCodes.Count);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeEdiimpPcodesContainsThreeValuesAndEndsWithComma_SettingsTradeEdiimpProductCodesContainsThreeValues()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_EDIIMP_PCODES", "DUMMY_PRODUCT_CODE_1, DUMMY_PRODUCT_CODE_2, DUMMY_PRODUCT_CODE_3,");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.AreEqual(3, settings.TradeEdiimpProductCodes.Count);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeEdiimpPcodesContainsThreeValuesContainingEmptyProductCode_SettingsTradeEdiimpProductCodesContainsTwoValues()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_EDIIMP_PCODES", "DUMMY_PRODUCT_CODE_1, , DUMMY_PRODUCT_CODE_3,");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.AreEqual(2, settings.TradeEdiimpProductCodes.Count);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentVariableTradeEdiimpPcodesIsEmpty_SettingsTradeEdiimpProductCodesContainsNoValues()
        {
            // Assign

            Environment.SetEnvironmentVariable("TRADE_EDIIMP_PCODES", "");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.AreEqual(0, settings.TradeEdiimpProductCodes.Count);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentEdkFilesDirIsSet_SettingsEdkFilesDirHasValue()
        {
            // Assign

            Environment.SetEnvironmentVariable("EDK_FILES_DIR", "DUMMY_DIR");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.AreEqual("DUMMY_DIR", settings.EdkFilesDirectory);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentEdkFilesDirIsSet_SettingsEdkFilesDirIsEmpty()
        {
            // Assign

            Environment.SetEnvironmentVariable("EDK_FILES_DIR", "");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsEmpty(settings.EdkFilesDirectory);
        }


        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentEdiFilesDirIsSet_SettingsEdiFilesDirHasValue()
        {
            // Assign

            Environment.SetEnvironmentVariable("ICC_IMPORT_DIR", "DUMMY_DIR");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.AreEqual("DUMMY_DIR", settings.EdiImportDirectory);
        }

        [Test]
        public void GetSettingsFromEnvironmentVariables_EnvironmentEdiFilesDirIsSet_SettingsEdiFilesDirIsEmpty()
        {
            // Assign

            Environment.SetEnvironmentVariable("ICC_IMPORT_DIR", "");

            // Act

            var settings = _settingsRepository.GetSettingsFromEnvironmentVariables();

            // Assert

            Assert.IsEmpty(settings.EdiImportDirectory);
        }       
    }
}
