using System;
using System.Data;
using Powel.Icc.Data;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.MetaData
{
    public class DataExchangeMetaData : IDataExchangeMetaData
    {
        //TODO: It may be vulnerable to depend on every protocol to be specified in the switch, find a solution to that?
        //TODO: A suggested solution is to look up PROTO_TP.CODE, for example, as default code UTS -> protocol "UTILTS",
        //TODO: but for some protocolId's, it should be overridden, ex TsProtocol.UTILTS_APERAK -> "UTILTSAPERAK"

        public string GetProtocolName(int protocolId)
        {
            TsProtocol id = (TsProtocol)protocolId;

            string protocol = "";

            switch (id)
            {
                case TsProtocol.EDI_DELFOR:
                case TsProtocol.EDI_DELFOR_READINGS:
                    protocol = "DELFOR";
                    //protocol = "PROGNO";
                    break;
                case TsProtocol.EDI_MSCONS:
                case TsProtocol.EDI_MSCONS_READINGS:
                case TsProtocol.EDI_MSCONS_SETTLEMENT:
                case TsProtocol.EDI_MSCONS_Z02:
                case TsProtocol.EDI_MSCONS_CONSUMPTION:
                    protocol = "MSCONS";
                    break;
                case TsProtocol.EDI_QUOTES:
                    protocol = "QUOTES";
                    break;
                case TsProtocol.EDI_APERAK:
                    protocol = "EDIELAPERAK";
                    break;
                case TsProtocol.GS2:
                case TsProtocol.GS2_V12_INFOSYNERGI:
                case TsProtocol.GS2_V10:
                case TsProtocol.GS2_ALBATROSS:
                case TsProtocol.GS2_HourReadings:
                    protocol = "GS2";
                    break;
                case TsProtocol.EXCEL:
                    protocol = "EXCEL";
                    break;
                case TsProtocol.PVE:
                    protocol = "PVE";
                    break;
                case TsProtocol.NETBAS_DNMI:
                    protocol = "NETBASDNMI";
                    break;
                case TsProtocol.POPDAT:
                    protocol = "POPDAT";
                    break;
                case TsProtocol.FRIICE:
                    protocol = "FRIICE";
                    break;
                case TsProtocol.UTILTS_N01:
                case TsProtocol.UTILTS_N02:
                case TsProtocol.UTILTS_N03:
                case TsProtocol.UTILTS_N06:
                case TsProtocol.UTILTS_E31:
                case TsProtocol.UTILTS_E26:
                case TsProtocol.UTILTS_E30:
                case TsProtocol.UTILTS_E66:
                case TsProtocol.UTILTS_S01:
                case TsProtocol.UTILTS_S02:
                case TsProtocol.UTILTS_S03:
                case TsProtocol.UTILTS_S04:
                case TsProtocol.UTILTS_S05:
                case TsProtocol.UTILTS_S06:
                case TsProtocol.UTILTS_S07:
                case TsProtocol.UTILTS_N13:
                case TsProtocol.UTILTS_N14:
                case TsProtocol.UTILTS_N10:
                case TsProtocol.UTILTS_N11:
                case TsProtocol.UTILTS_S12:
                case TsProtocol.UTILTS_E66_MONTH:
                case TsProtocol.UTILTS_E31_MONTH:
                case TsProtocol.UTILTS_S13:
                case TsProtocol.UTILTS_S37:
                    protocol = "UTILTS";
                    break;
                case TsProtocol.UTILTS_APERAK:
                    protocol = "UTILTSAPERAK";
                    break;
                case TsProtocol.UTILTS_ERR:
                    protocol = "UTILTSERR";
                    break;
                case TsProtocol.EDI_REQOTE:
                    protocol = "REQOTE";
                    //protocol = "ANMOMR";
                    break;
                case TsProtocol.EDI_REQOTE_N13:
                    protocol = "REQOTEFREQ";
                    break;
                case TsProtocol.EDI_REQOTE_S36:
                    protocol = "REQOTESEC";
                    break;
                case TsProtocol.EDK_SLSRPT:
                    protocol = "SLSRPT";
                    //protocol = "PRIMLD";
                    break;
                case TsProtocol.LINKCOM:
                    protocol = "LINKCOM";
                    break;
                case TsProtocol.EBIX:
                    protocol = "EBIX";
                    break;
                case TsProtocol.ECAN:
                case TsProtocol.EOD:
                case TsProtocol.EPD:
                    protocol = "ENTSOE";
                    break;
                case TsProtocol.ERRP:
                    protocol = "ERRP";
                    break;
                case TsProtocol.PARTICIPANT_MASTER_DATA:
                    protocol = "PARTICIPANT_MASTER_DATA";
                    break;
                case TsProtocol.PRODAT:
                    protocol = "PRODAT";
                    break;
                case TsProtocol.NPPO:
                    protocol = "NPSDV24X7";
                    //protocol = "NPPO":
                    break;
                case TsProtocol.ESS_A01:
                    protocol = "ESS";
                    break;
            }

            return protocol;
        }

        //TODO: Other structure than DataTable

        public DataTable GetSenderInfo(int opunKey)
        {
            return MiscData.GetOprtunitImpExpInfo(opunKey);
        }

        public DataTable GetReceiverInfo(int opunKey)
        {
            return MiscData.GetOprtunitImpExpInfo(opunKey);
        }
    }
}
