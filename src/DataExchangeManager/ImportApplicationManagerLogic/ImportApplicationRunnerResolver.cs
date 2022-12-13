using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using Powel.Icc.Common;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Runners.Enums;
using Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic.Settings;

namespace Powel.Icc.Messaging.DataExchangeManager.ImportApplicationManagerLogic
{
    public class ImportApplicationRunnerResolver
    {
        private readonly Func<string, IImportApplicationRunner> _importApplicationRunnerFactory;
        private readonly ImportSettings _settings;

        public ImportApplicationRunnerResolver(Func<string, IImportApplicationRunner> importApplicationRunnerFactory,
            Func<ImportSettings> settingsFactory)
        {
            _importApplicationRunnerFactory = importApplicationRunnerFactory;
            _settings = settingsFactory();
        }

        public IImportApplicationRunner GetImportApplicationRunner(DataExchangeImportMessage message)
        {
            var importApplication = GetApplicationType(message);
            var importApplicationRunner = _importApplicationRunnerFactory(importApplication.ToString());

            if (importApplicationRunner == null)
            {
                throw new DataExchangeConfigurationException(
                    string.Format("No import application runner is found for '{0}', message is not imported.",
                        importApplication));
            }

            return importApplicationRunner;
        }

        private ImportApplicationType GetApplicationType(DataExchangeImportMessage message)
        {
            var protocol = TrimmedUpperCase(message.Protocol);

            /* Used by ImportApplicationManagerService, AzureBusDataExchangeMangerService, EcpAmqpDataExchangeManagerService and MessageLogSearch
             * checks based on text from message (NP01, PD01) and PROTO_TP.CODE
             */
            switch (protocol)
            {
                case "EDIELAPERAK":
                case "AVRKVI":
                case "PROKVI":
                case "APLKVI":
                case "REQOTE":
                case "REQOTEFREQ":
                case "REQOTESEC":
                case "ANMOMR":
                case "SLSRPT":
                case "PRIMLD":
                case "EDK":         // At reimport: set from proto_tp.code. I.e N18 which is a UTILTS message get EDK as protocol.
                case DataExchangeMessageBase.ProtocolUtiltsS08:
                case DataExchangeMessageBase.ProtocolNpAuctions:
                case DataExchangeMessageBase.ProtocolNpAuction:
                case DataExchangeMessageBase.ProtocolNpContracts:
                case DataExchangeMessageBase.ProtocolNpContract:
                case DataExchangeMessageBase.ProtocolNpCurveOrder:
                case DataExchangeMessageBase.ProtocolNpBlockOrder:
                case DataExchangeMessageBase.ProtocolNpAuctionOrders:
                case DataExchangeMessageBase.ProtocolNpAuctionPortfolioVolumes:
                case DataExchangeMessageBase.ProtocolNpAuctionPrices:
                case DataExchangeMessageBase.ProtocolNpErrorMessage:
                    return EdkinApplicationType(message);
                case "DELFOR":
                case "PROGNO":
                case "MSCONS":
                case "AVREGN":
                case "EDI":
                    if (UseSpecialCaseForMsconsWhereEdkinIsUsed(message))
                    {
                        return ImportApplicationType.EdkIn;
                    }

                    return ImportApplicationType.EdiImp;
				case "UTILTSAPERAK":
                case "UTILTSERR":
                case "UTILTS":
                case "UTS":
				    return UtiltsApplicationType(message);
				case "GS2":
                case "PVE":
                case "LINKCOM":
                    return ImportApplicationType.Gs2imp;
                case "EBIX":
                case "ENTSOE":
                case "ERRP":
                case "PARTICIPANT_MASTER_DATA":
                case "XMLEL":
                    return XmlElApplicationType(message);
                case "PRODAT":
                    return ImportApplicationType.ProdatImp;
                case "NPSDV24X7":
                case "NPPO":
                    return ImportApplicationType.Nppoload;
                case "JSON.TS":
                    return ImportApplicationType.ServiceBus2Tss;
            }
            throw new DataExchangeInvalidMetadataException(string.Format("Invalid protocol '{0}' in metadata, message is not imported.",protocol));
        }

        private static string TrimmedUpperCase(string value)
        {
            if (!string.IsNullOrWhiteSpace(value))
            {
                return value.Trim().ToUpper();
            }

            return string.Empty;
        }

        private bool UseSpecialCaseForMsconsWhereEdkinIsUsed(DataExchangeImportMessage message)
        {
            // This is to determine if the message is a special case of MSCONS where
            // EDKIN is used instead of EDIIMP for import.

            var result = false;

            var tradeStatnettAddress = TrimmedUpperCase(_settings.TradeStatnettAddress);
            var tradeSvkAddress = TrimmedUpperCase(_settings.TradeSvkAddress);
            var senderName = TrimmedUpperCase(message.SenderName);
            var subAddress = TrimmedUpperCase(message.SubAddress);
            var productCode = TrimmedUpperCase(message.ProductCode);

            if (_settings.TradeHandlesInnmating &&
                subAddress == "INNMATING")
            {
                result = true;
            }
            else if (_settings.TradeAvregnToEdkin &&
                     tradeStatnettAddress.Length > 0 &&
                     tradeStatnettAddress == senderName &&
                     subAddress == "AVREGNING" &&
                     _settings.TradeEdiimpProductCodes.All(value => TrimmedUpperCase(value) != productCode))
            {
                result = true;
            }
            else if (_settings.TradeTimerToEdkin &&
                     tradeStatnettAddress.Length > 0 &&
                     tradeStatnettAddress == senderName &&
                     subAddress == "TIMER" &&
                     _settings.TradeEdiimpProductCodes.All(value => TrimmedUpperCase(value) != productCode))
            {
                result = true;
            }
            else if (_settings.TradeDelforToEdkin &&
                     tradeSvkAddress.Length > 0 &&
                     tradeSvkAddress == senderName &&
                     subAddress == "TRANS" &&
                     _settings.TradeEdkinProductCodes.Any(value => TrimmedUpperCase(value) == productCode))
            {
                result = true;
            }

            return result;
        }

        private bool IsAncitraMessageType(string messageData, IList<string> ancitraMessageTypes)
        {
            if (_settings.AncitraMessageTypeUseRe)
            {
                foreach (var ancitraMessageType in ancitraMessageTypes)
                {
                    var re = new Regex(ancitraMessageType);
                    if (re.IsMatch(messageData))
                        return true;
                }
            }
            else
            {
                using (var reader = new StringReader(messageData))
                {
                    string line;
                    while (!string.IsNullOrEmpty(line = reader.ReadLine()))
                    {
                        if (line.StartsWith("PD01 "))
                        {
                            var messageType = line.Substring(5, 3);
                            if (ancitraMessageTypes.Contains(messageType))
                                return true;
                            break;
                        }
                    }
                }
            }
            return false;
        }

        private ImportApplicationType UtiltsApplicationType(DataExchangeImportMessage message)
        {
            var ancitraMessageTypes = _settings.AncitraMessageTypes;
            if (ancitraMessageTypes?.Count > 0)
            {
                if (IsAncitraMessageType(message.GetMessageData(), ancitraMessageTypes))
                    return ImportApplicationType.AncitraQueue;
            }

            if (TrimmedUpperCase(message.Country) == "NOR")
                return ImportApplicationType.EdkIn;

            return ImportApplicationType.UtsImp;
        }

        private ImportApplicationType XmlElApplicationType(DataExchangeImportMessage message)
        {
            var ancitraMessageTypes = _settings.AncitraMessageTypes;
            if (ancitraMessageTypes?.Count > 0)
            {
                if (IsAncitraMessageType(message.GetMessageData(), ancitraMessageTypes))
                    return ImportApplicationType.AncitraQueue;
            }
            return ImportApplicationType.XmlElImp;
        }

        private ImportApplicationType EdkinApplicationType(DataExchangeImportMessage message)
        {
            var ancitraMessageTypes = _settings.AncitraMessageTypes;
            if (ancitraMessageTypes?.Count > 0)
            {
                if (IsAncitraMessageType(message.GetMessageData(), ancitraMessageTypes))
                    return ImportApplicationType.AncitraQueue;
            }
            return ImportApplicationType.EdkIn;
        }
    }
}
