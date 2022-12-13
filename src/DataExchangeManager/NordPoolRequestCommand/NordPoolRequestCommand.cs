using System.Collections.Generic;

namespace Powel.Icc.Messaging.DataExchangeManager.NordPoolRequestCommand
{
    public class NordPoolRequestCommand
    {
        public enum Commands
        {
            NewAuction,
            GetContract,
            PatchCurves,
            PatchBlocks,
            GetCurveOrder,
            GetBlockOrder,
            GetAuctionOrders,
            GetAuctionPortfolioVolumes,
            GetAuctionPrices
        }
        public Commands Command { get; set; }
        public Dictionary<string, string> Parameters { get; set; } = new Dictionary<string, string>(); // Command-specific
        // NewAuction
        public const string ParameterKeyFromDate = "FromDate";
        public const string ParameterKeyToDate = "ToDate";
        // GetContract
        public const string ParameterAuctionId = "AuctionId";
        public const string ParameterPortfolios = "Portfolios";
        public const string ParameterAreaCodes = "AreaCodes";
        // PatchCurves, PatchBlocks, GetCurveOrder, GetBlockOrder
        public const string ParameterOrderId = "OrderId";
        // PatchCurves, PatchBlocks
        public const string ParameterOrder = "Order";
    }
}
