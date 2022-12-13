using System.Collections.Generic;
using NUnit.Framework;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Enums;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerServiceTest
{
    [TestFixture]
    public class ImportApplicationRunnerResolverTest
    {
        private ImportApplicationRunnerResolver _importApplicationRunnerResolver;
        private ImportSettings _settings;

        private class ImportApplicationRunnerDummy : IImportApplicationRunner
        {
            private readonly string _name;

            public ImportApplicationRunnerDummy(string name)
            {
                _name = name;
            }

            public string Name
            {
                get { return _name; }
            }

            public void Run(DataExchangeImportMessage message)
            {
                ;
            }
        }

        private IImportApplicationRunner ImportApplicationRunnerFactory(string name)
        {
            return new ImportApplicationRunnerDummy(name);
        }

        [SetUp]
        public void SetUp()
        {
            _settings = new ImportSettings();
            _settings.TradeEdiimpProductCodes = new List<string>();

            _importApplicationRunnerResolver = new ImportApplicationRunnerResolver(ImportApplicationRunnerFactory, () => _settings);
        }

        [Test]
        [TestCase("DELFOR", ImportApplicationType.EdiImp)]
        [TestCase("PROGNO", ImportApplicationType.EdiImp)]
        [TestCase("EDIELAPERAK", ImportApplicationType.EdkIn)]
        [TestCase("AVRKVI", ImportApplicationType.EdkIn)]
        [TestCase("PROKVI", ImportApplicationType.EdkIn)]
        [TestCase("APLKVI", ImportApplicationType.EdkIn)]
        [TestCase("MSCONS", ImportApplicationType.EdiImp)]
        [TestCase("AVREGN", ImportApplicationType.EdiImp)]
        [TestCase("REQOTE", ImportApplicationType.EdkIn)]
        [TestCase("ANMOMR", ImportApplicationType.EdkIn)]
        [TestCase("SLSRPT", ImportApplicationType.EdkIn)]
        [TestCase("PRIMLD", ImportApplicationType.EdkIn)]
        [TestCase("UTILTS", ImportApplicationType.UtsImp)]
        [TestCase("ENTSOE", ImportApplicationType.XmlElImp)]
        [TestCase("EBIX", ImportApplicationType.XmlElImp)]
        [TestCase("ERRP", ImportApplicationType.XmlElImp)]
        public void GetImportApplicationRunner_TrimmedProtocol_ReturnsProperRunner(string protocolName, int importApplicationType)
        {
            var message = new DataExchangeImportMessage
            {
                Protocol = protocolName,
                Country = string.Empty
            };

            var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);

            Assert.AreEqual(((ImportApplicationType)importApplicationType).ToString(), ((ImportApplicationRunnerDummy)importApplicationRunner).Name);
        }

        [Test]
        [TestCase("  DELFOR  ", ImportApplicationType.EdiImp)]
        [TestCase(" PROGNO  ", ImportApplicationType.EdiImp)]
        public void GetImportApplicationRunner_ProtocolWithWhiteSpaces_ReturnsProperRunner(string protocolName, int importApplicationType)
        {
            var message = new DataExchangeImportMessage
            {
                Protocol = protocolName,
                Country = string.Empty
            };

            var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);

            Assert.AreEqual(((ImportApplicationType)importApplicationType).ToString(), ((ImportApplicationRunnerDummy)importApplicationRunner).Name);
        }

        [Test]
        public void GetImportApplication_ProtocolIsUnknown_ExceptionIsThrown()
        {
            var message = new DataExchangeImportMessage
            {
                Protocol = "DUMMYPROTOCOL",
                Country = ""
            };

            Assert.Catch<DataExchangeInvalidMetadataException>(() => _importApplicationRunnerResolver.GetImportApplicationRunner(message));
        }

        [Test]
        [TestCase("UTILTS", "NOR", ImportApplicationType.EdkIn)]
        [TestCase("UTILTS", "nor", ImportApplicationType.EdkIn)]
        [TestCase("UTILTS", "  NOR  ", ImportApplicationType.EdkIn)]
        [TestCase("UTILTS", "SWE", ImportApplicationType.UtsImp)]
        public void GetImportApplication_ProtocolIsUtiltsAndCountryIsGiven_ReturnsProperRunner(string protocolName, string countryCode, int importApplicationType)
        {
            var message = new DataExchangeImportMessage
            {
                Protocol = protocolName,
                Country = countryCode
            };

            var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);

            Assert.AreEqual(((ImportApplicationType)importApplicationType).ToString(), ((ImportApplicationRunnerDummy)importApplicationRunner).Name);
        }

        [Test,Description("Testing of AncitraMessageTypes settings.")]
        [TestCase("UTILTS", "N11,N18", null, null, "PD01 N11", ImportApplicationType.AncitraQueue)]
        [TestCase("UTILTS", "N11,N18", null, null, "PD01 N18", ImportApplicationType.AncitraQueue)]
        [TestCase("UTILTS", "N11,N18", null, "NOR", 
            @"PD01 N14
PD28 7       N11", ImportApplicationType.EdkIn)]    // Without this notation with @" and newline in string NUnit does not run these tests with \n in it. This first testcase solves all later testcases.
        [TestCase("UTILTS", "N11,N18", null, "SWE", "PD01 S01", ImportApplicationType.UtsImp)]
        [TestCase("UTILTS", "N11,N18", "TRUE", null, "PD01 N11", ImportApplicationType.AncitraQueue)]
        [TestCase("UTILTS", "N11,N18", "TRUE", null, "PD01 N18", ImportApplicationType.AncitraQueue)]
        [TestCase("UTILTS", "N11,N18", "TRUE", "NOR", "PD01 N14\nPD28 7       N11", ImportApplicationType.AncitraQueue)]    // This is as designed but probably not as expected. With RE search you cannot use short messageType strings. See next case.
        [TestCase("UTILTS", "PD01 N11,PD01 N18", "TRUE", "NOR", "PD01 N14\nPD28 7       N11", ImportApplicationType.EdkIn)]
        [TestCase("XMLEL", "ReserveAllocation_MarketDocument,Balancing_MarketDocument", "TRUE", null, "<ReserveAllocation_MarketDocument>", ImportApplicationType.AncitraQueue)]
        [TestCase("XMLEL", "ReserveAllocation_MarketDocument,Balancing_MarketDocument", "TRUE", null, "<Balancing_MarketDocument>", ImportApplicationType.AncitraQueue)]
        [TestCase("XMLEL", "ReserveAllocation_MarketDocument,Balancing_MarketDocument", "TRUE", null, "<Another_Document>", ImportApplicationType.XmlElImp)]
        [TestCase("UTILTS", "PD01 N11,PD01 N18,ReserveAllocation_MarketDocument,Balancing_MarketDocument", "TRUE", null, "PD01 N11", ImportApplicationType.AncitraQueue)]
        [TestCase("UTILTS", "PD01 N11,PD01 N18,ReserveAllocation_MarketDocument,Balancing_MarketDocument", "TRUE", null, "PD01 N18", ImportApplicationType.AncitraQueue)]
        public void GetImportApplication_UtiltsOrXmlel_AncitraMessageTypeSet_ReturnsProperRunner(string protocol, string messageTypes, string useRE, string country, string messageData, ImportApplicationType importApplicationType)
        {
            var message = new DataExchangeImportMessage
            {
                Protocol = protocol,
                MessageData = messageData,
                Country = country
            };
            _settings.AncitraMessageTypes = SettingsRepository.GetStringListFromCommaSeparatedValue(messageTypes);
            _settings.AncitraMessageTypeUseRe = SettingsRepository.GetBoolFromValue(useRE);

            var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);

            Assert.AreEqual(importApplicationType.ToString(), ((ImportApplicationRunnerDummy)importApplicationRunner).Name);
        }

        [Test]
        [TestCase("MSCONS", "INNMATING", ImportApplicationType.EdkIn)]
        [TestCase("MSCONS", "NOT_INMATNING", ImportApplicationType.EdiImp)]
        public void GetImportApplication_SubAddressIsSet_ReturnsProperRunner(string protocolName, string subAddress, int importApplicationType)
        {
            var message = new DataExchangeImportMessage
            {
                Protocol = protocolName,
                SubAddress = subAddress
            };

            _settings.TradeHandlesInnmating = true;

            var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);

            Assert.AreEqual(((ImportApplicationType)importApplicationType).ToString(), ((ImportApplicationRunnerDummy)importApplicationRunner).Name);
        }

        [Test]
        [TestCase("MSCONS", "AVREGNING", "DUMMY_SENDER", "DUMMY_SENDER", ImportApplicationType.EdkIn)]
        [TestCase("MSCONS", "AVREGNING", "DUMMY_SENDER", "NOT_DUMMY_SENDER", ImportApplicationType.EdiImp)]
        [TestCase("MSCONS", "NOT_AVREGNING", "DUMMY_SENDER", "DUMMY_SENDER", ImportApplicationType.EdiImp)]
        public void GetImportApplication_SenderNameAndTradeStatnettAddressAreSet_TradeAvregnToEdkinIsTrue_ReturnsProperRunner(string protocolName, string subAddress, string senderName, 
            string tradeStatnettAddress, int importApplicationType)
        {
            var message = new DataExchangeImportMessage
            {
                Protocol = protocolName,
                SubAddress = subAddress,
                SenderName = senderName
            };

            _settings.TradeAvregnToEdkin = true;
            _settings.TradeStatnettAddress = tradeStatnettAddress;

            var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);

            Assert.AreEqual(((ImportApplicationType)importApplicationType).ToString(), ((ImportApplicationRunnerDummy)importApplicationRunner).Name);
        }

        [TestCase("MSCONS", "TIMER", "DUMMY_SENDER", "DUMMY_SENDER", ImportApplicationType.EdkIn)]
        [TestCase("MSCONS", "TIMER", "DUMMY_SENDER", "NOT_DUMMY_SENDER", ImportApplicationType.EdiImp)]
        [TestCase("MSCONS", "NOT_TIMER", "DUMMY_SENDER", "DUMMY_SENDER", ImportApplicationType.EdiImp)]
        public void GetImportApplication_SenderNameAndTradeStatnettAddressAreSet_TradeTimerToEdkinIsTrue_ReturnsProperRunner(string protocolName, string subAddress, string senderName,
               string tradeStatnettAddress, int importApplicationType)
        {
            var message = new DataExchangeImportMessage
            {
                Protocol = protocolName,
                SubAddress = subAddress,
                SenderName = senderName
            };

            _settings.TradeTimerToEdkin = true;
            _settings.TradeStatnettAddress = tradeStatnettAddress;

            var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);

            Assert.AreEqual(((ImportApplicationType)importApplicationType).ToString(), ((ImportApplicationRunnerDummy)importApplicationRunner).Name);
        }

        [Test]
        public void GetImportApplication_ProtocolIsMscons_TradeAvregnToEdkinIsTrue_TradeStatnettAddressIsSenderName_SubAddressIsAvregning_ProductCodeIsInTradeEdiimpProductCodes_EdiimpIsReturned()
        {
            var message = new DataExchangeImportMessage
                {
                    Protocol = "MSCONS",
                    SubAddress = "AVREGNING",
                    ProductCode = "DUMMY_PRODUCT_CODE",
                    SenderName = "DUMMY_SENDER"
                };

            _settings.TradeAvregnToEdkin = true;
            _settings.TradeStatnettAddress = "DUMMY_SENDER";
            _settings.TradeEdiimpProductCodes = new List<string> { "DUMMY_PRODUCT_CODE" };

            var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);

            Assert.AreEqual(ImportApplicationType.EdiImp.ToString(), ((ImportApplicationRunnerDummy)importApplicationRunner).Name);
        }

        [Test]
        public void GetImportApplication_ProtocolIsMscons_TradeTimerToEdkinIsTrue_TradeStatnettAddressIsSenderName_SubAddressIsTimer_ProductCodeIsInTradeEdiimpProductCodes_EdiimpIsReturned()
        {
            var message = new DataExchangeImportMessage
                {
                    Protocol = "MSCONS",
                    SubAddress = "TIMER",
                    ProductCode = "DUMMY_PRODUCT_CODE",
                    SenderName = "DUMMY_SENDER"
                };

            _settings.TradeTimerToEdkin = true;
            _settings.TradeStatnettAddress = "DUMMY_SENDER";
            _settings.TradeEdiimpProductCodes = new List<string> { "DUMMY_PRODUCT_CODE" };

            var importApplicationRunner = _importApplicationRunnerResolver.GetImportApplicationRunner(message);

            Assert.AreEqual(ImportApplicationType.EdiImp.ToString(), ((ImportApplicationRunnerDummy)importApplicationRunner).Name);
        }
    }
}
