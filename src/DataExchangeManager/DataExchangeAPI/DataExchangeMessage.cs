using System;
using System.Collections.Generic;
using System.Globalization;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi.ClaimCheck.Abstract;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi
{
    public abstract class DataExchangeMessageBase
    {
        // Constants used for check.
        public const string ProtocolUtiltsS08 = "UTILTS.S08";
        public const string ProtocolNpAuctions = "JSON.NPAUCTIONS";
        public const string ProtocolNpAuction = "JSON.NPAUCTION";
        public const string ProtocolNpContracts = "JSON.NPCONTRACTS";
        public const string ProtocolNpContract = "JSON.NPCONTRACT";
        public const string ProtocolNpCurveOrder = "JSON.NP_CURVE_ORDER";
        public const string ProtocolNpBlockOrder = "JSON.NP_BLOCK_ORDER";
        public const string ProtocolNpAuctionOrders = "JSON.NP.AUCTION_ORDERS";
        public const string ProtocolNpAuctionPortfolioVolumes = "JSON.NP.AUCTION_PORTFOLIO_VOLUMES";
        public const string ProtocolNpAuctionPrices = "JSON.NP.AUCTION_PRICES";
        public const string ProtocolNpErrorMessage = "JSON.NP_ERROR_MESSAGE";

        /// <summary>
        /// Initially set to LatestVersion.
        /// 0: Handles old type messages with payload stored inside message, in MessageData.
        /// 1: Payload is stored externally using the ClaimCheck pattern. See: https://www.ibm.com/developerworks/websphere/library/techarticles/1006_kharlamov/1006_kharlamov.html#download
        /// </summary>
        public static int LatestVersion { get { return 1; } }  // Change this when a new Version is implemented.
        public int Version { get; set; }
        /// <summary>
        /// This value is sent to the remote EDI-server and stored in MESSAGE_HEADER.EXTERNAL_REFERENCE. Preferably a GUID.
        /// Is also used as Id for the StorageHandler. Must be set before MessageData is initialized.
        /// </summary>
        public string ExternalReference { get; set; }

        #region Version0

        /// <summary>
        /// Version = 0: Old and deprecated interface where Payload is stored inside the message. 
        /// Kept to be able to handle list of old messages already in the queue when changing to a new version.
        /// </summary>
        public string MessageData { get; set; }
        #endregion

        #region Version1
        // Implementation of the ClaimCheck pattern.
        public ClaimTicket ClaimTicket { get; set; }    // Used as messageId for StorageHandler if ExternalReference is not set before MessageData is initialized. Also used when ExternalReference is set afterwards.
        public string GetMessageData(bool DeleteStorage = false)
        {
            if (Version == 0)
                return MessageData;

            IClaimHandler invoker = ClaimCheckFactory.Create(ClaimTicket.HandlerName);
            return invoker.checkOut(ClaimTicket, DeleteStorage);
        }

        public void SetMessageData(string Payload,string HandlerName = null)
        {
            if (Version == 0)
            {
                MessageData = Payload;
                return;
            }

            IClaimHandler invoker = ClaimCheckFactory.Create(HandlerName);
            ClaimTicket = invoker.checkIn(Payload);
        }

        public void DeleteMessageData()
        {
            if (Version == 0)
            {
                MessageData = string.Empty;
                return;
            }

            if (ClaimTicket == null)    // No message body.
                return;
            IClaimHandler invoker = ClaimCheckFactory.Create(ClaimTicket.HandlerName);
            invoker.clear(ClaimTicket);
        }

        #endregion

        // For metadata definition See: http://powelplanet.powelasa.powel.com/projects/newcomm/_layouts/WordViewer.aspx?id=/projects/newcomm/Shared%20Documents/commproj_IP.docx&Source=http%3A%2F%2Fpowelplanet%2Epowelasa%2Epowel%2Ecom%2Fprojects%2Fnewcomm%2FSitePages%2FHome%2Easpx%3F%26%26p%5FSortBehavior%3D0%26p%5FFileLeafRef%3DHandelsobjekt%2520og%2520tidsserier%252epptx%26%26PageFirstRow%3D1%26%26View%3D%7B7E1451E2%2D87D5%2D4CFA%2DB116%2DAFCBDD1D50BC%7D%26InitialTabId%3DRibbon%252EDocument%26VisibilityContext%3DWSSTabPersistence&DefaultItemOpen=1
        /// <summary>
        /// SenderId, ReceiverId, SenderName, ReceiverName and Country:
        /// For an export message it is recommended to use SenderKey and
        /// ReceiverKey as input parameter instead and let the DataExchangeAPI
        /// decide SenderId etc from SenderKey and ReceiverKey.
        /// </summary>
        public string SenderId { get; set; }

        public string ReceiverId { get; set; }

        public string SenderName { get; set; }

        public string ReceiverName { get; set;  }

        public string Format { get; set; }

        public string Country { get; set; }

        public int ProtocolId { get; set; }

        public string Priority { get; set; }

        public string SubAddress { get; set; }

        public string ProductCode { get; set; }
        
        /// <summary>
        /// This value determines which import application to use to import
        /// the message. See system documentation for valid protocol values.
        /// For an export message it is recommended to use only ProtocolId as
        /// input parameter and let the DataExchangeAPI decide Protocol from
        /// ProtocolId.
        /// </summary>
        public string Protocol { get; set; }

        /// <summary>
        /// database table RECADR, column ADDRESS
        /// </summary>
        public string RoutingAddress { get; set; }

        public bool IsEmpty
        {
            get { return (GetMessageData() ?? string.Empty).Trim().Length == 0; }
        }

        public virtual Dictionary<string,string> Metadata()
        {
            // Returns Metadata from the message. Actually most of it are data from the message body
            // I'm not happy with this string key. You have to support IgnoreCase and it makes it very little type-safe. The "Message type" is a bad example. Why isn't it called "Sender name"?
            // When implementing this method I was told to use names from the referenced document, and next from the XML entities, the rest is from the class properties with no capitalization.
            var md = new Dictionary<string, string>(12,StringComparer.InvariantCultureIgnoreCase);
            md.Add("Priority",Priority);
            md.Add("Sender", SenderId);
            md.Add("Receiver", ReceiverId);
            md.Add("format", Format);
            md.Add("country", Country);
            md.Add("Message type", Protocol);
            md.Add("Routing",RoutingAddress);
            md.Add("SenderName", SenderName);  
            md.Add("ReceiverName", ReceiverName);   // Obsolete: ToDo: Remove
            md.Add("ProtocolId", ProtocolId.ToString());   // Obsolete: ToDo: Remove
            md.Add("subaddress", SubAddress);
            md.Add("prodcode", ProductCode);  
            return md;
        }
    }

    public class DataExchangeExportMessage : DataExchangeMessageBase
    {
        //public DataExchangeExportMessage() { }  // For serializing
        /// <summary>
        /// database table MESSAGE_HEADER, column MSG_ID
        /// (This value is used for exports, when updating the message log)
        /// </summary>
        public long MessageLogId { get; set; }

        /// <summary>
        /// database table MESSAGE_HEADER, column MESSAGE_REFERENCE
        /// (used for what? possibly for look-up in MESSAGE_HEADER)
        /// </summary>
        public string MessageReference { get; set; }

        /// <summary>
        /// SenderKey and ReceiverKey:
        /// Is OPRTUNIT.OPUN_KEY for sender and receiver.
        /// Is recommended used instead of SenderId etc
        /// for deciding SenderId etc in DataExchangeAPI.
        /// </summary>
        public int SenderKey { get; set; }

        public int ReceiverKey { get; set; }

        public override Dictionary<string,string> Metadata()
        {
            var md = base.Metadata();
            md.Add("MessageLogId",MessageLogId.ToString());
            md.Add("MessageReference",MessageReference);
            md.Add("SenderKey",SenderKey.ToString());
            md.Add("ReceiverKey",ReceiverKey.ToString());
            md.Add("ExternalReference",ExternalReference);
            return md;
        }
    }

    public class DataExchangeImportMessage : DataExchangeMessageBase
    {
        public DataExchangeImportMessage() {}
        public DataExchangeImportMessage(DataExchangeExportMessage exportMessage)
        {
            ExternalReference = exportMessage.MessageLogId.ToString(CultureInfo.InvariantCulture);
            Version = exportMessage.Version;
            ClaimTicket = exportMessage.ClaimTicket;    // Version == 1
            SetMessageData(exportMessage.GetMessageData(),ClaimTicket != null ? ClaimTicket.HandlerName : null);    // Version == 0
            Country = exportMessage.Country;
            Format = exportMessage.Format;
            SenderId = exportMessage.SenderId;
            ReceiverId = exportMessage.ReceiverId;
            SenderName = exportMessage.SenderName;
            ReceiverName = exportMessage.ReceiverName;
            ProtocolId = exportMessage.ProtocolId;
            RoutingAddress = exportMessage.RoutingAddress;
            Priority = exportMessage.Priority;
            Protocol = exportMessage.Protocol;
        }

        /// <summary>
        /// This value can be generated by either by the EDI-server or the import module.
        /// It will be stored in MESSAGE_HEADER.EXTERNAL_REFERENCE by the import application, which
        /// receives it as a commmand line switch from the import application manager
        /// </summary>
        //public string ExternalReference { get; set; }

        /// <summary>
        /// Free text field for EDI-server to use. Will e.g. be visible in the Import error queue admin tool.
        /// </summary>
        public string ExternalText { get; set; }

        public DateTime EnqueuedTimeUtc { get; set; }
    }
}
