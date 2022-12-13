using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using log4net;
using Microsoft.Practices.Unity;
using NDesk.Options;
using Newtonsoft.Json;
using Oracle.ManagedDataAccess.Client;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.NordPoolRequestCommand;
using Powel.Icc.Services.DataContracts.EventMonitor;
using Powel.Icc.Services.Time;

namespace NpAuctionRequester
{
    class Program
    {
        private static readonly ILog Log = LogManager.GetLogger(typeof(Program));
        private static readonly IEventLogModuleItem EventLogModuleItem = new EventLogModuleItem(IccModule.M_NP_REQUESTER);
        private static int CommonGeneralInfo = 1010;
        private static string UsageText => @"Usage: NpAuctionRequester NewAuction|na|GetContract|gc|GetCurveOrder|gco|GetBlockOrder|gbo [parameters]";

        static int Main(string[] args)
        {
            Log.Info("Starting.");
            EventLogModuleItem.LogMessage(CommonGeneralInfo,$"Arguments are: {string.Join(" ",args)}");
            if (args.Length < 1)
            {
                Console.WriteLine(UsageText);
                return -1;
            }

            try
            {
                string command = args[0];
                var remainingOptions = new List<string>();
                for (int i = 1; i < args.Length; i++)
                    remainingOptions.Add(args[i]);
                var messages = new List<DataExchangeExportMessage>();
                switch (command.ToLower())
                {
                    case "newauction":
                    case "na":
                        messages = CreateNewAuctionCommands(remainingOptions);
                        break;
                    case "getcontract":
                    case "gc":
                        messages = CreateGetContractsCommands(remainingOptions);
                        break;
                    case "getcurveorder":
                    case "gco":
                        messages = CreateGetCurveOrderCommands(remainingOptions);
                        break;
                    case "getblockorder":
                    case "gbo":
                        messages = CreateGetBlockOrderCommands(remainingOptions);
                        break;
                    case "getauctionorders":
                    case "gao":
                        messages = CreateGetAuctionOrdersCommands(remainingOptions);
                        break;
                    case "getauctionportfoliovolumes":
                    case "gapv":
                        messages = CreateGetAuctionPortfolioVolumesCommands(remainingOptions);
                        break;
                    case "getauctionprices":
                    case "gap":
                        messages = CreateGetAuctionPricesCommands(remainingOptions);
                        break;
                    case "-h":
                    default:
                        if (command != "-h")
                            Console.WriteLine($"Unknown command: {command}");
                        Console.WriteLine(UsageText);
                        remainingOptions.Add("-h");
                        CreateNewAuctionCommands(remainingOptions);
                        CreateGetContractsCommands(remainingOptions);
                        CreateGetCurveOrderCommands(remainingOptions);
                        CreateGetBlockOrderCommands(remainingOptions);
                        CreateGetAuctionOrdersCommands(remainingOptions);
                        CreateGetAuctionPortfolioVolumesCommands(remainingOptions);
                        CreateGetAuctionPricesCommands(remainingOptions);
                        return -1;
                }

                if (messages.Count > 0)
                {
                    var unityContainer = RegisterDependencies();
                    var dataExchangeApi = unityContainer.Resolve<DataExchangeAPI>();
                    using (var transaction = dataExchangeApi.GetTransaction(DataExchangeQueueTransactionType.Enqueue))
                    {
                        transaction.Begin();
                        foreach (var message in messages)
                        {
                            dataExchangeApi.EnqueueExportMessage(message, transaction);
                        }

                        transaction.Commit();
                        EventLogModuleItem.LogMessage(8200);
                    }
                }

                Console.WriteLine($"Sent {messages.Count} messages.");
                EventLogModuleItem.Close();

            }
            catch (Exception Ex)
            {
                Console.WriteLine(Ex.Message);
                return -1;
            }
            return 0;
        }

        private static int GetReceiverKey(IDbConnection Conn)
        {
            var cmd = new OracleCommand("select ivalue from par_vl where pard_key = 1000 and part_key = 604");
            return Convert.ToInt32(Util.CommandToScalar(cmd));
        }

        private static string GetPriority(string command,string Default = null)
        {
            var priority = Default ?? "Normal";
            var commandPriorities = IccConfiguration.Trade.NpAuction.Requester.Priorities.Split(',');
            foreach (var commandPriority in commandPriorities)
            {
                if (commandPriority.StartsWith(command))
                {
                    var cpArr = commandPriority.Split(':');
                    if (cpArr.Length > 1)
                    {
                        priority = cpArr[1];
                        break;
                    }
                }
            }
            return priority;
        }

        private static Tuple<string,string> GetRoutingParameters(IDbConnection Conn, int OpunKey, string Command)
        {
            var cmd = new OracleCommand("select address,export_queue from recadr where opun_key = :opunKey and prot_key = 104 and valftime <= :time and valttime > :time");    // Prot_key = TsProtocol.NP_REQUEST_COMMAND
            cmd.Parameters.Add(":opunKey", OracleDbType.Int32).Value = OpunKey;
            cmd.Parameters.Add(":time",OracleDbType.Varchar2).Value = RegionalCalendar.GetCalendar(RegionalCalendar.dbName).ToDatabaseString(UtcTime.Now);
            var dt = Util.CommandToDataTable(cmd);
            if (dt.Rows.Count == 0)
                return new Tuple<string,string>("NPAUCTION:",GetPriority(Command,"LOW")); 
            return new Tuple<string, string>(dt.Rows[0]["address"].ToString(),GetPriority(Command,dt.Rows[0]["export_queue"].ToString()));
        }

        private static List<DataExchangeExportMessage> CreateNewAuctionCommands(List<string> args)
        {
            var commands = new List<DataExchangeExportMessage>();
            string[] _operators = null;
            int _days = 7;
            bool help = false, verbose = false, force = false;
            // Request only non-defined periods. Starting with delivery day tomorrow (GCT for today is already elapsed), ending n days later.
            var npCal = RegionalCalendar.GetCalendar("nordpool");
            RegionalCalendar dateCal = RegionalCalendar.Current;
            UtcTime start = npCal.trim(UtcTime.Now + TimeSpan.FromDays(1), TimeSpan.FromDays(1));
            var options = new OptionSet()
            {
                {"o|operators=", "Operator key(s). Default from TRD_NPAUCTION_REQUESTER_OPERATORS or TRD_DEFAULT_OPERATOR", o => _operators = o.Split(',') },
                {"s|start=", "First auction delivery date. Default: Tomorrow.",o => { start = RegionalCalendar.Txt2UtcTime(o, UtcTime.Null, out dateCal); } },
                {"d|days=", $"Period length. Max 7 days. Default={_days}", (int o) => _days = o },
                {"force", "Force request of already existing auction days", o => force = true },
                {"v|verbose", "Print verbose messages", o => verbose = true },
                {"h|help","Print help",o => help = true }
            };
            try
            {
                options.Parse(args);
            }
            catch (OptionException ex)
            {
                Console.WriteLine(ex.Message);
                options.WriteOptionDescriptions(Console.Out);
            }

            if (help)
            {
                Console.WriteLine("NewAuction|na: Requests new auction definitions for given operators and future days.");
                options.WriteOptionDescriptions(Console.Out);
                return commands;
            }
            if (_operators == null)
            {
                _operators = IccConfiguration.Trade.NpAuction.Requester.Operators?.Split(',');
                if (_operators == null || _operators.Length == 0)
                {
                    _operators = new string[] { IccConfiguration.Trade.DefaultOperator.ToString() };
                    if (string.IsNullOrEmpty(_operators[0]) && verbose)
                    {
                        Console.WriteLine("No operators defined.");
                        return commands;
                    }
                }
            }
            if (verbose)
                Console.WriteLine($"Operators handled: {string.Join(",", _operators)}");

            var periods = new List<TimePeriod>();   // List of GCT-dates. Not deliveryStartDate
            using (IDbConnection conn = Util.OpenConnection())
            {
                int receiver = GetReceiverKey(conn);
                var routingParam = GetRoutingParameters(conn, receiver,"NewAuction");
                var dbCal = RegionalCalendar.GetCalendar(RegionalCalendar.dbName);
                UtcTime end = npCal.AddDays(start,_days);
                var increment = TimeSpan.FromDays(1);
                var cmd = new OracleCommand("select 1 from trade_period tp, market m where delivery_start_date = :startdate and tp.mrkt_key = m.mrkt_key and m.code = :code and m.mrkt_type = 2"); // Type: Day-Ahead. See: PdMarket.MarketType.DayAHeadMarket
                cmd.Parameters.Add(":startdate", OracleDbType.Date);
                cmd.Parameters.Add(":code", OracleDbType.Varchar2).Value = IccConfiguration.Trade.NpAuction.Requester.MarketCode;
                for (UtcTime date = start; date < end; date += increment)
                {
                    cmd.Parameters[":startdate"].Value = dbCal.ToDateTime(date);
                    var exists = Util.CommandToScalar(cmd, conn);
                    if (exists != null && Convert.ToInt32(exists) == 1 && !force)
                        continue;

                    if (periods.Count == 0 || (!periods[periods.Count - 1].Until.IsNull && date - periods[periods.Count - 1].Until > increment))
                    {
                        periods.Add(new TimePeriod(date - increment, UtcTime.Null));    // GCT
                    }
                    else
                    {
                        periods[periods.Count - 1].SetUntil(date);  // GCT
                    }
                }

                if (periods.Count > 0 && periods[periods.Count - 1].Until == UtcTime.Null)
                    periods[periods.Count - 1].SetUntil(npCal.Add(periods[periods.Count - 1].From,increment));

                if (verbose)
                {
                    Console.WriteLine($"{periods.Count} undefined trade periods in delivery interval [{npCal.ToString(start)}..{npCal.ToString(end)}>");
                    foreach (var period in periods)
                    {
                        Console.WriteLine($"\tGCT: [{npCal.ToString(period.From)}..{npCal.ToString(period.Until)}>");
                    }
                }

                foreach (var oper in _operators)
                {
                    var opunKey = Convert.ToInt32(oper);
                    foreach (var per in periods)
                    {
                        var message = new DataExchangeExportMessage();
                        message.ProtocolId = (int)TsProtocol.NP_REQUEST_COMMAND;
                        message.Protocol = TsProtocol.NP_REQUEST_COMMAND.ToString();
                        message.SenderKey = opunKey;
                        message.ReceiverKey = receiver;
                        message.RoutingAddress = routingParam.Item1;
                        message.Format = "JSON";    // For eventlog message
                        // To overrule defaults:
                        message.Priority = routingParam.Item2;

                        var msgData = new NordPoolRequestCommand();
                        msgData.Command = NordPoolRequestCommand.Commands.NewAuction;
                        msgData.Parameters.Add(NordPoolRequestCommand.ParameterKeyFromDate, per.From.ToXmlDateTime());
                        msgData.Parameters.Add(NordPoolRequestCommand.ParameterKeyToDate, per.Until.ToXmlDateTime());
                        message.SetMessageData(JsonConvert.SerializeObject(msgData));

                        commands.Add(message);
                        EventLogModuleItem.LogMessage(CommonGeneralInfo,$"Sending message for operator {opunKey}, period: [{npCal.ToString(per.From)}..{npCal.ToString(per.Until)}>.");
                    }
                }
            }

            return commands;
        }

        private static List<DataExchangeExportMessage> CreateGetContractsCommands(List<string> args)
        {
            var commands = new List<DataExchangeExportMessage>();
            string[] _operators = null;
            RegionalCalendar dateCal = RegionalCalendar.Current;
            UtcTime date = dateCal.trim(UtcTime.Now + TimeSpan.FromDays(1),TimeSpan.FromDays(1));
            string[] portfolioIds = null;
            string[] areas = null;
            bool help = false, verbose = false, force = false, update = false;
            var options = new OptionSet()
            {
                {"o|operators=", "Operator key(s). Default from TRD_NPAUCTION_REQUESTER_OPERATORS or TRD_DEFAULT_OPERATOR.", o => _operators = o.Split(',') },
                {"d|DeliveryStartDate=", "Delivery start date. Default: Tomorrow.", o => { date = dateCal.Txt2UtcTime(o, UtcTime.Null); } },
                {"p|PortfolioIds=", "Portfolio id(s). Default: All.", o => portfolioIds = o.Split(',') },
                {"a|Areas=", "Areas. E.g. NO1[,NO2]. Default: All.", o => areas = o.Split(',') },
                {"u|update", "Update preceding dates with Final prices", o => update = true },
                {"force", "Force request of already existing auction days", o => force = true },
                {"v|verbose", "Print verbose messages", o => verbose = true },
                {"h|help", "Print help",o => help = true }
            };
            try
            {
                options.Parse(args);
            }
            catch (OptionException ex)
            {
                Console.WriteLine(ex.Message);
                options.WriteOptionDescriptions(Console.Out);
            }

            if (help)
            {
                Console.WriteLine("GetContract|gc: Get contracts for given operators and auction date. Filtered by portfolios and areas");
                options.WriteOptionDescriptions(Console.Out);
                return commands;
            }

            if (date.IsNull)
            {
                Console.WriteLine("Bad delivery start date. See logfile.");
                return commands;
            }

            if (_operators == null)
            {
                _operators = IccConfiguration.Trade.NpAuction.Requester.Operators?.Split(',');
                if (_operators == null || _operators.Length == 0)
                {
                    _operators = new string[] { IccConfiguration.Trade.DefaultOperator.ToString() };
                    if (string.IsNullOrEmpty(_operators[0]) && verbose)
                    {
                        Console.WriteLine("No operators defined.");
                        return commands;
                    }
                }
            }

            if (update)
            {
                // Use from Task Scheduler job to update with Final prices also after long holiday periods with no exchange rate updates (e.g easter).
                // Iterate backwards on dates until no commands is created (Final prices already received).
                var dateArgs = args;
                var updateIdx = dateArgs.IndexOf("-u");
                if (updateIdx == -1)
                {
                    updateIdx = dateArgs.IndexOf("-update");
                }
                dateArgs.RemoveAt(updateIdx);

                var dateIdx = dateArgs.IndexOf("-d");
                if (dateIdx == -1)
                {
                    dateIdx = dateArgs.IndexOf("-DeliveryStartDate");
                    if (dateIdx == -1)
                    {
                        dateIdx = dateArgs.Count;
                        dateArgs.Add("-DeliveryStartDate");
                        dateArgs.Add(dateCal.ToString(date,"yyyyMMdd"));
                    }
                }

                dateIdx++;
                var updateDate = date;

                var dateCommands = new List<DataExchangeExportMessage>();
                do
                {
                    dateCommands = CreateGetContractsCommands(dateArgs);
                    commands.AddRange(dateCommands);
                    updateDate = dateCal.AddDays(updateDate, -1);
                    dateArgs[dateIdx] = dateCal.ToString(updateDate, "yyyyMMdd");
                } while (dateCommands.Count > 0);
                return commands;
            }

            using (IDbConnection conn = Util.OpenConnection())
            {
                int receiver = GetReceiverKey(conn);
                var routingParam = GetRoutingParameters(conn, receiver,"GetContract");
                var dbCal = RegionalCalendar.GetCalendar(RegionalCalendar.dbName);

                var selTrelsbid = new OracleCommand("select distinct(portfolio_id) from trelsbid where opun_key = :opunKey and portfolio_id is not null");
                selTrelsbid.Parameters.Add(":opunKey", OracleDbType.Int32);

                var selAuc = new OracleCommand("select id from trade_period tp, market m where delivery_start_date = :startdate and tp.mrkt_key = m.mrkt_key and m.code = :code and m.mrkt_type = 2 and pcode_key = (select distinct(pcode_key) from trobject where opun_key = :opunKey and trot_key in ('B24T','BLCK'))"); // Type = 2: Day-Ahead. See: PdMarket.MarketType.DayAHeadMarket
                selAuc.Parameters.Add(":startdate", OracleDbType.Date).Value = dbCal.ToDateTime(date);
                selAuc.Parameters.Add(":code", OracleDbType.Varchar2).Value = IccConfiguration.Trade.NpAuction.Requester.MarketCode;
                selAuc.Parameters.Add(":opunKey", OracleDbType.Int32);

                var selAreasCmd = new OracleCommand("select code from trare_tp where mart_key = 'A' and vlcode in (select vlcode from meter_vl where tims_key = (select tims_key from trobject where opun_key = :opunKey and trot_key = 'EARO') and tsvalue = 1 and datetim = :startd and vlcode != 0)");
                selAreasCmd.Parameters.Add(":opunKey", OracleDbType.Int32);
                selAreasCmd.Parameters.Add(":startd", OracleDbType.Date).Value = dateCal.ToDateTime(date); // This is ts with dayly timespan and datetim = '20210923'

                /* Read the last TransactionLog status for Trades import for the given date and participant. Reads only status for the last import.
                 * Signal given from edkin:
		         * ImportSuccessful: Trades.Status = "Final". Stop polling for more Trades messages
		         * ImportPartly: Trades.Status = "PreliminaryResults. Continue polling for Final prices.
                 */
                var alreadyReceivedCmd = new OracleCommand("select message_status,date_created from message_header where direct = 0 and prot_key = 117 and start_date <= :delStart1 and end_date > :delStart2 and receiver = :opunKey order by date_created desc");
                alreadyReceivedCmd.Parameters.Add(":delStart1", OracleDbType.Date).Value = dbCal.ToDateTime(date);
                alreadyReceivedCmd.Parameters.Add(":delStart2", OracleDbType.Date).Value = dbCal.ToDateTime(date);
                alreadyReceivedCmd.Parameters.Add(":opunKey", OracleDbType.Int32);

                // Find price status for price object with product and value unit like the operators bid object.
                //var priceStatusCmd = new OracleCommand("select tsstatus from meter_vl mvl where datetim = :delStart " +
                //   "and tims_key = (select ts.tims_key from trobject tr,par_vl pv1,par_vl pv2,timeser ts where tr.opun_key = pv1.ivalue and pv1.pard_key = 1000 and pv1.part_key = 604 and trot_key = 'PRIC' and pcode_key = (select distinct(pcode_key) from trobject where opun_key = :opunKey and trot_key = 'B24T') and ts.tims_key = tr.tims_key and ts.unme_key = (" +
                //   "select case unme_key when 0 then (select pv2.ivalue from par_vl pv2 where pv2.pard_key = 1000 and pv2.part_key = 605) " +
                //   "else unme_key end from trelsbid trb,trobject tro where trb.opun_key = :opunKey and trb.opun_key = tro.opun_key and trb.trcode = tro.trcode and tro.trot_key = 'B24T'");
                //priceStatusCmd.Parameters.Add(":delStart",OracleDbType.Date).Value = dbCal.ToDateTime(date);
                //priceStatusCmd.Parameters.Add(":opunKey", OracleDbType.Date);
                foreach (var oper in _operators)
                {
                    var opunKey = Convert.ToInt32(oper);
                    if (!force)
                    {
                        alreadyReceivedCmd.Parameters[":opunKey"].Value = opunKey;
                        var alreadyReceivedDt = Util.CommandToDataTable(alreadyReceivedCmd);
                        if (alreadyReceivedDt.Rows.Count > 0) { 
                            // It seems like NP sends out a Trades message with preliminary prices on any day before they send out a message with final prices a bit later.
                            // So we must continue to poll on all days except saturday and sunday for Trades messages with final prices.
                            // During weekends (delivery on sunday and monday) there is no updated exchange rates, so final prices for these days are updated on monday at 13:00.
                            // As long as the status on price values are Suspect we will do this request on mondays until the status is Ok.
                            var row = alreadyReceivedDt.Rows[0];
                            var messageStatus = (TransLogMessageStatus)Convert.ToInt32(row["message_status"]);
                            if (messageStatus == TransLogMessageStatus.ImportSuccessful)
                            {
                                if (verbose)
                                    Console.WriteLine($"Trades message with Final status is imported at {Convert.ToDateTime(row["date_created"])} for operator {opunKey}");
                                continue;
                            }

                            if (messageStatus == TransLogMessageStatus.ImportPartly)
                            {
                                if (DateTime.Today.DayOfWeek == DayOfWeek.Saturday ||
                                    DateTime.Today.DayOfWeek == DayOfWeek.Sunday)
                                {
                                    switch (dbCal.DayOfWeek(date))
                                    {
                                        case DayOfWeek.Saturday:
                                            if ((DateTime.Today - dbCal.ToDateTime(date).Date) > TimeSpan.FromDays(1))
                                                break; // Last weekend
                                            if (verbose)
                                                Console.WriteLine("Saturday and Sunday will never get Final price updates.");
                                            continue;
                                        case DayOfWeek.Sunday:
                                            if ((DateTime.Today - dbCal.ToDateTime(date).Date) > TimeSpan.FromDays(2))
                                                break; // Last weekend
                                            if (verbose)
                                                Console.WriteLine("Saturday and Sunday will never get Final price updates.");
                                            continue;
                                        default: // If we ask for other days: Go on
                                            break;
                                    }
                                }
                            }
                        }
                    }

                    int r = 0;
                    if (portfolioIds == null)
                    {
                        Log.Info("Searching for portfolios:");
                        selTrelsbid.Parameters[":opunKey"].Value = opunKey;
                        var dt = Util.CommandToDataTable(selTrelsbid, conn);
                        if (dt.Rows.Count == 0)
                            continue;

                        portfolioIds = new string[dt.Rows.Count];
                        r = 0;
                        foreach (DataRow row in dt.Rows)
                        {
                            portfolioIds[r++] = row["portfolio_id"].ToString();
                        }
                        Log.Info($"Found: {string.Join(",", portfolioIds)}");
                    }

                    selAuc.Parameters[":opunKey"].Value = opunKey;
                    var aucDt = Util.CommandToDataTable(selAuc, conn);
                    if (aucDt.Rows.Count == 0)
                    {
                        Log.Info("No auctions defined.");
                        if (verbose)
                            Console.WriteLine($"No auctions defined for market:{selAuc.Parameters[":code"].Value.ToString()}, date:{selAuc.Parameters[":startdate"].Value.ToString()}, and operator:{opunKey}");
                        portfolioIds = null;
                        continue;
                    }
                    var ids = new string[aucDt.Rows.Count];
                    r = 0;
                    foreach (DataRow row in aucDt.Rows)
                    {
                        ids[r++] = row["id"].ToString();
                    }
                    Log.Info($"Found auctions: {string.Join(",",ids)}");

                    if (areas == null)
                    {
                        Log.Info("Searching for areas:");
                        selAreasCmd.Parameters[":opunKey"].Value = opunKey;
                        var dt = Util.CommandToDataTable(selAreasCmd, conn);
                        if (dt.Rows.Count == 0)
                        {
                            portfolioIds = null;
                            continue;
                        }

                        areas = new string[dt.Rows.Count];
                        r = 0;
                        foreach (DataRow row in dt.Rows)
                        {
                            areas[r++] = row["code"].ToString();
                        }
                        Log.Info($"Found: {string.Join(",",areas)}");
                    }

                    foreach (var id in ids)
                    {
                        var message = new DataExchangeExportMessage();
                        message.ProtocolId = (int)TsProtocol.NP_REQUEST_COMMAND;
                        message.Protocol = TsProtocol.NP_REQUEST_COMMAND.ToString();
                        message.SenderKey = opunKey;
                        message.ReceiverKey = receiver;
                        message.RoutingAddress = routingParam.Item1;
                        message.Format = "JSON";    // For eventlog message
                                                    // To overrule defaults:
                        message.Priority = routingParam.Item2;

                        var msgData = new NordPoolRequestCommand();
                        msgData.Command = NordPoolRequestCommand.Commands.GetContract;
                        msgData.Parameters.Add(NordPoolRequestCommand.ParameterAuctionId, id.ToString());
                        msgData.Parameters.Add(NordPoolRequestCommand.ParameterPortfolios, string.Join(",", portfolioIds));
                        msgData.Parameters.Add(NordPoolRequestCommand.ParameterAreaCodes, string.Join(",", areas));
                        message.SetMessageData(JsonConvert.SerializeObject(msgData));

                        commands.Add(message);
                        EventLogModuleItem.LogMessage(CommonGeneralInfo,$"Sending message for operator {opunKey}, auction: {id}, portfolios: {string.Join(",", portfolioIds)}, areas: {string.Join(",", areas)}.");
                    }

                    portfolioIds = null;
                    areas = null;
                }
            }

            return commands;
        }

        private static List<DataExchangeExportMessage> CreateGetCurveOrderCommands(List<string> args)
        {
            var commands = new List<DataExchangeExportMessage>();
            int opunKey = 0;
            var orderId = string.Empty;
            RegionalCalendar dateCal = RegionalCalendar.Current;
            UtcTime date = dateCal.trim(UtcTime.Now + TimeSpan.FromDays(1),TimeSpan.FromDays(1));
            string area = null;
            bool help = false, verbose = false;
            var options = new OptionSet()
            {
                {"oid|orderId=", "Order id.", (string o) => orderId = o },
                {"o|operator=", "Operator key. Default from TRD_DEFAULT_OPERATOR",(int o) => opunKey = o },
                {"d|DeliveryStartDate=", "Delivery start date. Default: Tomorrow.", o => { date = dateCal.Txt2UtcTime(o, UtcTime.Null); } },
                {"a|Area=", "Area. E.g. NO1. Default: All.", o => area = o },
                {"v|verbose", "Print verbose messages", o => verbose = true },
                {"h|help","Print help",o => help = true }
            };
            try
            {
                options.Parse(args);
            }
            catch (OptionException ex)
            {
                Console.WriteLine(ex.Message);
                options.WriteOptionDescriptions(Console.Out);
            }

            if (help)
            {
                Console.WriteLine("GetCurveOrder|gco: Requests curve order message for given order id or given operator, delivery start date and area.");
                options.WriteOptionDescriptions(Console.Out);
                return commands;
            }

            if (opunKey == 0)
            {
                opunKey = IccConfiguration.Trade.DefaultOperator;
                if (verbose)
                    Console.WriteLine($"Operator set to {opunKey}");
            }

            if (string.IsNullOrEmpty(orderId) && (opunKey == 0 || string.IsNullOrEmpty(area)))
            {
                Console.WriteLine("No order id and missing operator or area");
                return commands;
            }

            using (IDbConnection conn = Util.OpenConnection())
            {
                int receiver = GetReceiverKey(conn);
                var routingParam = GetRoutingParameters(conn, receiver,"GetCurveOrder");
                var messageReference = string.Empty;
                if (!string.IsNullOrEmpty(orderId))
                {
                    // OrderId given. Find Operator and MessageReference for OrderId.
                    var cmd = new OracleCommand("select opun_key,mess_ref from bid24frm where order_id = :orderId and mess_ref is not null order by formno desc");
                    cmd.Parameters.Add(":orderId", OracleDbType.Varchar2).Value = orderId;
                    var dt = Util.CommandToDataTable(cmd);
                    if (dt.Rows.Count == 0)
                    {
                        Console.WriteLine($"Unknown order id:{orderId}");
                        return commands;
                    }

                    opunKey = Convert.ToInt32(dt.Rows[0]["opun_key"]);
                    messageReference = dt.Rows[0]["mess_ref"].ToString();
                    if (verbose)
                        Console.WriteLine($"Found bid form with message reference {messageReference}.");
                }
                else
                {
                    // OrderId not given. Find OrderId and MessageReference from Operator, DeliveryDate and Area.
                    var dbCal = RegionalCalendar.GetCalendar(RegionalCalendar.dbName);
                    var cmd = new OracleCommand(@"select order_id,mess_ref from bid24frm b,trobject t 
                    where b.opun_key = :opunKey 
                    and b.opun_key = t.opun_key
                    and t.trot_key = 'B24T'
                    and b.trcode = t.trcode
                    and timstamp = :tstamp 
                    and trat_key = (select trat_key from trare_tp where mart_key = 'A' and code = :area) 
                    and mess_ref is not null 
                    order by formno desc");
                    cmd.Parameters.Add(":opunKey", OracleDbType.Int32).Value = opunKey;
                    cmd.Parameters.Add(":tstamp", OracleDbType.Varchar2).Value = dbCal.ToDatabaseString(date);
                    cmd.Parameters.Add(":area", OracleDbType.Varchar2).Value = area;
                    var dt = Util.CommandToDataTable(cmd);
                    if (dt.Rows.Count == 0)
                    {
                        Console.WriteLine("No bid form found for given operator, date and area.");
                        return commands;
                    }

                    for (int i = 0; i < dt.Rows.Count; i++)
                    {
                        orderId = dt.Rows[0]["order_id"].ToString();
                        messageReference = dt.Rows[0]["mess_ref"].ToString();
                        if (!string.IsNullOrEmpty(orderId))
                            break;
                    }
                    if (string.IsNullOrEmpty(orderId))
                    {
                        Console.WriteLine($"Found {dt.Rows.Count} forms, but no order id.");
                        return commands;
                    }
                    if (verbose) 
                        Console.WriteLine($"Found bid form with order id {orderId} and message reference {messageReference}");
                }

                var message = new DataExchangeExportMessage();
                message.ProtocolId = (int)TsProtocol.NP_REQUEST_COMMAND;
                message.Protocol = TsProtocol.NP_REQUEST_COMMAND.ToString();
                message.SenderKey = opunKey;
                message.ReceiverKey = receiver;
                message.RoutingAddress = routingParam.Item1;
                message.Format = "JSON";    // For eventlog message
                message.Priority = routingParam.Item2;
                message.MessageReference = messageReference;

                var msgData = new NordPoolRequestCommand();
                msgData.Command = NordPoolRequestCommand.Commands.GetCurveOrder;
                msgData.Parameters.Add(NordPoolRequestCommand.ParameterOrderId, orderId);
                message.SetMessageData(JsonConvert.SerializeObject(msgData));

                commands.Add(message);
                EventLogModuleItem.LogMessage(CommonGeneralInfo,$"Sending message for operator {opunKey}, order id: {orderId} message reference: {messageReference}.");
            }

            return commands;
        }

        private static List<DataExchangeExportMessage> CreateGetBlockOrderCommands(List<string> args)
        {
            var commands = new List<DataExchangeExportMessage>();
            int opunKey = 0;
            var orderId = string.Empty;
            RegionalCalendar dateCal = RegionalCalendar.Current;
            UtcTime date = dateCal.trim(UtcTime.Now + TimeSpan.FromDays(1),TimeSpan.FromDays(1));
            string area = null;
            bool help = false, verbose = false;
            var options = new OptionSet()
            {
                {"oid|orderId=", "Order id.", (string o) => orderId = o },
                {"o|operator=", "Operator key. Default from TRD_DEFAULT_OPERATOR",(int o) => opunKey = o },
                {"d|DeliveryStartDate=", "Delivery start date. Default: Tomorrow.", o => { date = dateCal.Txt2UtcTime(o, UtcTime.Null); } },
                {"a|Area=", "Area. E.g. NO1.", o => area = o },
                {"v|verbose", "Print verbose messages", o => verbose = true },
                {"h|help","Print help",o => help = true }
            };
            try
            {
                options.Parse(args);
            }
            catch (OptionException ex)
            {
                Console.WriteLine(ex.Message);
                options.WriteOptionDescriptions(Console.Out);
            }

            if (help)
            {
                Console.WriteLine("GetBlockOrder|gbo: Requests block order message for given order id or given operator, delivery start date and area.");
                options.WriteOptionDescriptions(Console.Out);
                return commands;
            }

            if (opunKey == 0)
            {
                opunKey = IccConfiguration.Trade.DefaultOperator;
                if (verbose)
                    Console.WriteLine($"Operator set to {opunKey}");
            }

            if (string.IsNullOrEmpty(orderId) && (opunKey == 0 || string.IsNullOrEmpty(area)))
            {
                Console.WriteLine("No order id and missing operator or area");
                return commands;
            }

            using (IDbConnection conn = Util.OpenConnection())
            {
                int receiver = GetReceiverKey(conn);
                var routingParam = GetRoutingParameters(conn, receiver,"GetBlockOrder");
                var messageReference = string.Empty;
                if (!string.IsNullOrEmpty(orderId))
                {
                    // OrderId given. Find Operator and MessageReference for OrderId.
                    var cmd = new OracleCommand("select opun_key,mess_ref from bidblock where order_id = :orderId and mess_ref is not null order by sentdate desc");
                    cmd.Parameters.Add(":orderId", OracleDbType.Varchar2).Value = orderId;
                    var dt = Util.CommandToDataTable(cmd);
                    if (dt.Rows.Count == 0)
                    {
                        Console.WriteLine($"Unknown order id:{orderId}");
                        return commands;
                    }

                    opunKey = Convert.ToInt32(dt.Rows[0]["opun_key"]);
                    messageReference = dt.Rows[0]["mess_ref"].ToString();
                    if (verbose)
                        Console.WriteLine($"Found bid block with message reference {messageReference}.");
                }
                else
                {
                    // OrderId not given. Find OrderId and MessageReference from Operator, DeliveryDate and Area.
                    var dbCal = RegionalCalendar.GetCalendar(RegionalCalendar.dbName);
                    var cmd = new OracleCommand("select order_id,mess_ref from bidblock where opun_key = :opunKey and timstamp = :tstamp and trat_key = (select trat_key from trare_tp where mart_key = 'A' and code = :area) and mess_ref is not null order by sentdate desc");
                    cmd.Parameters.Add(":opunKey", OracleDbType.Int32).Value = opunKey;
                    cmd.Parameters.Add(":tstamp", OracleDbType.Varchar2).Value = dbCal.ToDatabaseString(date);
                    cmd.Parameters.Add(":area", OracleDbType.Varchar2).Value = area;
                    var dt = Util.CommandToDataTable(cmd);
                    if (dt.Rows.Count == 0)
                    {
                        Console.WriteLine("No bid blocks found for given operator, date and area.");
                        return commands;
                    }

                    for (int i = 0; i < dt.Rows.Count; i++)
                    {
                        orderId = dt.Rows[0]["order_id"].ToString();
                        messageReference = dt.Rows[0]["mess_ref"].ToString();
                        if (!string.IsNullOrEmpty(orderId))
                            break;
                    }
                    if (string.IsNullOrEmpty(orderId))
                    {
                        Console.WriteLine($"Found {dt.Rows.Count} blocks, but no order id");
                        return commands;
                    }
                    if (verbose) 
                        Console.WriteLine($"Found bid block with order id {orderId} and message reference {messageReference}");
                }

                var message = new DataExchangeExportMessage();
                message.ProtocolId = (int)TsProtocol.NP_REQUEST_COMMAND;
                message.Protocol = TsProtocol.NP_REQUEST_COMMAND.ToString();
                message.SenderKey = opunKey;
                message.ReceiverKey = receiver;
                message.RoutingAddress = routingParam.Item1;
                message.Format = "JSON";    // For eventlog message
                message.Priority = routingParam.Item2;
                message.MessageReference = messageReference;

                var msgData = new NordPoolRequestCommand();
                msgData.Command = NordPoolRequestCommand.Commands.GetBlockOrder;
                msgData.Parameters.Add(NordPoolRequestCommand.ParameterOrderId, orderId);
                message.SetMessageData(JsonConvert.SerializeObject(msgData));

                commands.Add(message);
                EventLogModuleItem.LogMessage(CommonGeneralInfo,$"Sending message for operator {opunKey}, order id: {orderId} message reference: {messageReference}.");
            }

            return commands;
        }

        struct AuctionOrderIds
        {
            public int opunKey;
            public string portfolio;
            public int pcodeKey;
        };

        private static List<DataExchangeExportMessage> CreateGetAuctionOrdersCommands(List<string> args)
        {
            var commands = new List<DataExchangeExportMessage>();
            var auctionId = string.Empty;
            RegionalCalendar dateCal = RegionalCalendar.Current;
            UtcTime date = dateCal.trim(UtcTime.Now + TimeSpan.FromDays(1),TimeSpan.FromDays(1));
            string[] _operators = null, portfolioIds = Array.Empty<string>(), areas = Array.Empty<string>();
            bool help = false, verbose = false;
            var options = new OptionSet()
            {
                {"aid|auctionId=", "Auction id.", (string o) => auctionId = o },
                {"o|operators=", "Operator key(s). Default from TRD_NPAUCTION_REQUESTER_OPERATORS or TRD_DEFAULT_OPERATOR.", o => _operators = o.Split(',') },
                {"d|DeliveryStartDate=", "Delivery start date. Default: Tomorrow.", o => { date = dateCal.Txt2UtcTime(o, UtcTime.Null); } },
                {"p|PortfolioIds=", "Portfolio id(s). Default: All.", o => portfolioIds = o.Split(',') },
                {"a|Areas=", "Areas. E.g. NO1[,NO2]. Default: All.", o => areas = o.Split(',') },
                {"v|verbose", "Print verbose messages", o => verbose = true },
                {"h|help","Print help",o => help = true }
            };
            try
            {
                options.Parse(args);
            }
            catch (OptionException ex)
            {
                Console.WriteLine(ex.Message);
                options.WriteOptionDescriptions(Console.Out);
            }

            if (help)
            {
                Console.WriteLine("GetAuctionOrders|gao: Requests auction orders message for given auction id or given operator, delivery start date, portfolios and areas.");
                options.WriteOptionDescriptions(Console.Out);
                return commands;
            }

            if (_operators == null)
            {
                _operators = IccConfiguration.Trade.NpAuction.Requester.Operators?.Split(',');
                if (_operators == null || _operators.Length == 0)
                {
                    _operators = new string[] { IccConfiguration.Trade.DefaultOperator.ToString() };
                    if (string.IsNullOrEmpty(_operators[0]) && verbose)
                    {
                        Console.WriteLine("No operators defined.");
                        return commands;
                    }
                }
            }

            if (portfolioIds.Length == 0)
            {
                if (_operators.Length == 0 || (_operators.Length == 1 && string.IsNullOrEmpty(_operators[0])))
                {
                    if (verbose)
                        Console.WriteLine("No portfolio and no operator given.");
                    return commands;
                }
            }

            using (IDbConnection conn = Util.OpenConnection())
            {
                // Find products given auctionid or operator and portfolio.
                var pcodeKeys = new List<int>();
                var dbCal = RegionalCalendar.GetCalendar(RegionalCalendar.dbName);
                var aucOrdIds = new List<AuctionOrderIds>();
                if (!string.IsNullOrEmpty(auctionId))
                {
                    var productCmd = new OracleCommand("select pcode_key from trade_period where id = :auctionId");
                    productCmd.Parameters.Add(":auctionId", OracleDbType.Varchar2).Value = auctionId;
                    var productDt = Util.CommandToDataTable(productCmd, conn);
                    foreach (DataRow row in productDt.Rows)
                    {
                        pcodeKeys.Add(Convert.ToInt32(row["pcode_key"]));
                    }
                }
                if (_operators.Length > 0)
                {
                    var opunKeys = _operators.Select(k => Convert.ToInt32(k));
                    var oprtCmd = new OracleCommand("select tob.opun_key,pcode_key,portfolio_id from trobject tob, trelsbid tb where tob.opun_key in (:opunKeys) and tob.opun_key = tb.opun_key and tob.trcode = tb.trcode and trot_key = 'B24T'");
                    oprtCmd = oprtCmd.AddParameterCollection("opunKeys", OracleDbType.Int32, opunKeys);
                    if (portfolioIds.Length > 0)
                    {
                        oprtCmd.CommandText += " and portfolio_id in (:portfolios)";
                        oprtCmd = oprtCmd.AddParameterCollection("portfolios",OracleDbType.Varchar2,portfolioIds);
                    }

                    if (pcodeKeys.Count > 0)
                    {
                        oprtCmd.CommandText += " and pcode_key in (:pcodeKeys)";
                        oprtCmd = oprtCmd.AddParameterCollection("pcodeKeys", OracleDbType.Int32, pcodeKeys);
                    }

                    var oprtDt = Util.CommandToDataTable(oprtCmd,conn);
                    foreach (DataRow row in oprtDt.Rows)
                    {
                        var aucOrdId = new AuctionOrderIds();
                        aucOrdId.opunKey = Convert.ToInt32(row["opun_key"]);
                        aucOrdId.portfolio = row["portfolio_id"].ToString();
                        aucOrdId.pcodeKey = Convert.ToInt32(row["pcode_key"]);
                        aucOrdIds.Add(aucOrdId);
                    }
                    if (oprtDt.Rows.Count == 0 && verbose)
                        Console.WriteLine($"No portfolio found for Participant: {string.Join(",", _operators)} and portfolio id: {string.Join(",",portfolioIds)} and product keys: {string.Join(",", pcodeKeys)}");
                }

                var auctionCmd = new OracleCommand("select id from trade_period tp, market m where delivery_start_date = :startdate and tp.mrkt_key = m.mrkt_key and m.code = :code and m.mrkt_type = 2 and pcode_key = :pcodeKey"); // Type: Day-Ahead. See: PdMarket.MarketType.DayAHeadMarket
                auctionCmd.Parameters.Add(":startdate", OracleDbType.Date).Value = dbCal.ToDateTime(date);
                auctionCmd.Parameters.Add(":code", OracleDbType.Varchar2).Value = IccConfiguration.Trade.NpAuction.Requester.MarketCode;
                auctionCmd.Parameters.Add(":pcodeKey", OracleDbType.Int32);
                int receiver = GetReceiverKey(conn);
                var routingParam = GetRoutingParameters(conn, receiver,"GetAuctionOrders");
                var uiCal = RegionalCalendar.Current;
                foreach (var aucOrdId in aucOrdIds)
                {
                    string auctionParam;
                    if (string.IsNullOrEmpty(auctionId))
                    {
                        auctionCmd.Parameters[":pcodeKey"].Value = aucOrdId.pcodeKey;
                        var auctionDt = Util.CommandToDataTable(auctionCmd, conn);
                        if (auctionDt.Rows.Count == 0)
                        {
                            if (verbose)
                                Console.WriteLine(
                                    $"No auction found for product key {aucOrdId.pcodeKey} and delivery {uiCal.ToString(date,"d")}");
                            continue;
                        }

                        auctionParam = auctionDt.Rows[0]["id"].ToString();
                    }
                    else auctionParam = auctionId;

                    var message = new DataExchangeExportMessage();
                    message.ProtocolId = (int)TsProtocol.NP_REQUEST_COMMAND;
                    message.Protocol = TsProtocol.NP_REQUEST_COMMAND.ToString();
                    message.SenderKey = aucOrdId.opunKey;
                    message.ReceiverKey = receiver;
                    message.RoutingAddress = routingParam.Item1;
                    message.Format = "JSON";    // For eventlog message
                    message.Priority = routingParam.Item2;
                    message.MessageReference = Guid.NewGuid().ToString();

                    var msgData = new NordPoolRequestCommand();
                    msgData.Command = NordPoolRequestCommand.Commands.GetAuctionOrders;
                    msgData.Parameters.Add(NordPoolRequestCommand.ParameterAuctionId, auctionParam);
                    msgData.Parameters.Add(NordPoolRequestCommand.ParameterPortfolios, aucOrdId.portfolio);
                    msgData.Parameters.Add(NordPoolRequestCommand.ParameterAreaCodes, string.Join(",", areas));
                    message.SetMessageData(JsonConvert.SerializeObject(msgData));

                    commands.Add(message);
                    EventLogModuleItem.LogMessage(CommonGeneralInfo,$"Sending message for operator {aucOrdId.opunKey}, auction: {auctionParam}, portfolio: {aucOrdId.portfolio}.");
                }
            }

            return commands;
        }

        private static List<DataExchangeExportMessage> CreateGetAuctionPortfolioVolumesCommands(List<string> args)
        {
            var commands = new List<DataExchangeExportMessage>();
            var auctionId = string.Empty;
            RegionalCalendar dateCal = RegionalCalendar.Current;
            UtcTime date = dateCal.trim(UtcTime.Now + TimeSpan.FromDays(1),TimeSpan.FromDays(1));
            string[] _operators = null, portfolioIds = Array.Empty<string>(), areas = Array.Empty<string>();
            bool help = false, verbose = false;
            var options = new OptionSet()
            {
                {"aid|auctionId=", "Auction id.", (string o) => auctionId = o },
                {"o|operators=", "Operator key(s). Default from TRD_NPAUCTION_REQUESTER_OPERATORS or TRD_DEFAULT_OPERATOR.", o => _operators = o.Split(',') },
                {"d|DeliveryStartDate=", "Delivery start date. Default: Tomorrow.", o => { date = dateCal.Txt2UtcTime(o, UtcTime.Null); } },
                {"p|PortfolioIds=", "Portfolio id(s). Default: All.", o => portfolioIds = o.Split(',') },
                {"a|Areas=", "Areas. E.g. NO1[,NO2]. Default: All.", o => areas = o.Split(',') },
                {"v|verbose", "Print verbose messages", o => verbose = true },
                {"h|help","Print help",o => help = true }
            };
            try
            {
                options.Parse(args);
            }
            catch (OptionException ex)
            {
                Console.WriteLine(ex.Message);
                options.WriteOptionDescriptions(Console.Out);
            }

            if (help)
            {
                Console.WriteLine("GetAuctionPortfolioVolumes|gapv: Requests auction portfolio volumes message for given auction id or given operator, delivery start date, portfolios and areas.");
                options.WriteOptionDescriptions(Console.Out);
                return commands;
            }

            if (_operators == null)
            {
                _operators = IccConfiguration.Trade.NpAuction.Requester.Operators?.Split(',');
                if (_operators == null || _operators.Length == 0)
                {
                    _operators = new string[] { IccConfiguration.Trade.DefaultOperator.ToString() };
                    if (string.IsNullOrEmpty(_operators[0]) && verbose)
                    {
                        Console.WriteLine("No operators defined.");
                        return commands;
                    }
                }
            }

            if (portfolioIds.Length == 0)
            {
                if (_operators.Length == 0 || (_operators.Length == 1 && string.IsNullOrEmpty(_operators[0])))
                {
                    if (verbose)
                        Console.WriteLine("No portfolio and no operator given.");
                    return commands;
                }
            }

            using (IDbConnection conn = Util.OpenConnection())
            {
                // Find product given operator or portfolio.
                int pcodeKey = 0;
                var dbCal = RegionalCalendar.GetCalendar(RegionalCalendar.dbName);
                var aucOrdIds = new List<AuctionOrderIds>();
                if (!string.IsNullOrEmpty(auctionId))
                {
                    var productCmd = new OracleCommand("select nvl(pcode_key,0) from trade_period where id = :auctionId");
                    productCmd.Parameters.Add(":auctionId", OracleDbType.Varchar2).Value = auctionId;
                    pcodeKey = Convert.ToInt32(Util.CommandToScalar(productCmd, conn));
                }
                if (_operators.Length > 0)
                {
                    var opunKeys = _operators.Select(k => Convert.ToInt32(k));
                    var oprtCmd = new OracleCommand("select tob.opun_key,pcode_key,portfolio_id from trobject tob, trelsbid tb where tob.opun_key in (:opunKeys) and tob.opun_key = tb.opun_key and tob.trcode = tb.trcode and trot_key = 'B24T'");
                    oprtCmd = oprtCmd.AddParameterCollection("opunKeys", OracleDbType.Int32, opunKeys);
                    if (portfolioIds.Length > 0)
                    {
                        oprtCmd.CommandText += " and portfolio_id in (:portfolios)";
                        oprtCmd = oprtCmd.AddParameterCollection("portfolios",OracleDbType.Varchar2,portfolioIds);
                    }

                    if (pcodeKey > 0)
                    {
                        oprtCmd.CommandText += " and pcode_key = :pcodeKey";
                        oprtCmd.Parameters.Add(":pcodeKey", OracleDbType.Int32).Value = pcodeKey;
                    }

                    var oprtDt = Util.CommandToDataTable(oprtCmd,conn);
                    foreach (DataRow row in oprtDt.Rows)
                    {
                        var aucOrdId = new AuctionOrderIds();
                        aucOrdId.opunKey = Convert.ToInt32(row["opun_key"]);
                        aucOrdId.portfolio = row["portfolio_id"].ToString();
                        aucOrdId.pcodeKey = Convert.ToInt32(row["pcode_key"]);
                        aucOrdIds.Add(aucOrdId);
                    }
                }

                var auctionCmd = new OracleCommand("select id from trade_period tp, market m where delivery_start_date = :startdate and tp.mrkt_key = m.mrkt_key and m.code = :code and m.mrkt_type = 2 and pcode_key = :pcodeKey"); // Type: Day-Ahead. See: PdMarket.MarketType.DayAHeadMarket
                auctionCmd.Parameters.Add(":startdate", OracleDbType.Date).Value = dbCal.ToDateTime(date);
                auctionCmd.Parameters.Add(":code", OracleDbType.Varchar2).Value = IccConfiguration.Trade.NpAuction.Requester.MarketCode;
                auctionCmd.Parameters.Add(":pcodeKey", OracleDbType.Int32);
                int receiver = GetReceiverKey(conn);
                var routingParam = GetRoutingParameters(conn, receiver,"GetAuctionPortfolioVolumes");
                var uiCal = RegionalCalendar.Current;
                foreach (var aucOrdId in aucOrdIds)
                {
                    string auctionParam;
                    if (string.IsNullOrEmpty(auctionId))
                    {
                        auctionCmd.Parameters[":pcodeKey"].Value = aucOrdId.pcodeKey;
                        var auctionDt = Util.CommandToDataTable(auctionCmd, conn);
                        if (auctionDt.Rows.Count == 0)
                        {
                            if (verbose)
                                Console.WriteLine(
                                    $"No auction found for product key {aucOrdId.pcodeKey} and delivery {uiCal.ToString(date,"d")}");
                            continue;
                        }

                        auctionParam = auctionDt.Rows[0]["id"].ToString();
                    }
                    else auctionParam = auctionId;

                    var message = new DataExchangeExportMessage();
                    message.ProtocolId = (int)TsProtocol.NP_REQUEST_COMMAND;
                    message.Protocol = TsProtocol.NP_REQUEST_COMMAND.ToString();
                    message.SenderKey = aucOrdId.opunKey;
                    message.ReceiverKey = receiver;
                    message.RoutingAddress = routingParam.Item1;
                    message.Format = "JSON";    // For eventlog message
                    message.Priority = routingParam.Item2;
                    message.MessageReference = Guid.NewGuid().ToString();

                    var msgData = new NordPoolRequestCommand();
                    msgData.Command = NordPoolRequestCommand.Commands.GetAuctionPortfolioVolumes;
                    msgData.Parameters.Add(NordPoolRequestCommand.ParameterAuctionId, auctionParam);
                    msgData.Parameters.Add(NordPoolRequestCommand.ParameterPortfolios, aucOrdId.portfolio);
                    msgData.Parameters.Add(NordPoolRequestCommand.ParameterAreaCodes, string.Join(",", areas));
                    message.SetMessageData(JsonConvert.SerializeObject(msgData));

                    commands.Add(message);
                    EventLogModuleItem.LogMessage(CommonGeneralInfo,$"Sending message for operator {aucOrdId.opunKey}, auction: {auctionParam}, portfolio: {aucOrdId.portfolio}.");
                }
            }

            return commands;
        }

        private static List<DataExchangeExportMessage> CreateGetAuctionPricesCommands(List<string> args)
        {
            var commands = new List<DataExchangeExportMessage>();
            var auctionId = string.Empty;
            RegionalCalendar dateCal = RegionalCalendar.Current;
            UtcTime date = dateCal.trim(UtcTime.Now + TimeSpan.FromDays(1),TimeSpan.FromDays(1));
            bool help = false, verbose = false;
            var options = new OptionSet()
            {
                {"aid|auctionId=", "Auction id.", (string o) => auctionId = o },
                {"d|DeliveryStartDate=", "Delivery start date. Default: Tomorrow.", o => { date = dateCal.Txt2UtcTime(o, UtcTime.Null); } },
                {"v|verbose", "Print verbose messages", o => verbose = true },
                {"h|help","Print help",o => help = true }
            };
            try
            {
                options.Parse(args);
            }
            catch (OptionException ex)
            {
                Console.WriteLine(ex.Message);
                options.WriteOptionDescriptions(Console.Out);
            }

            if (help)
            {
                Console.WriteLine("GetAuctionPrices|gap: Requests auction prices message for given auction id or delivery start date.");
                options.WriteOptionDescriptions(Console.Out);
                return commands;
            }

            using (IDbConnection conn = Util.OpenConnection())
            {
                var dbCal = RegionalCalendar.GetCalendar(RegionalCalendar.dbName);
                int receiver = GetReceiverKey(conn);
                var routingParam = GetRoutingParameters(conn, receiver,"GetAuctionPrices");
                if (string.IsNullOrEmpty(auctionId))
                {
                    // Uses distinct since the same product can be used for both norwegian and swedish operators.
                    var auctionCmd = new OracleCommand("select distinct(id) from trade_period tp, market m where delivery_start_date = :startdate and tp.mrkt_key = m.mrkt_key and m.code = :code and m.mrkt_type = 2"); // Type: Day-Ahead. See: PdMarket.MarketType.DayAHeadMarket
                    auctionCmd.Parameters.Add(":startdate", OracleDbType.Date).Value = dbCal.ToDateTime(date);
                    auctionCmd.Parameters.Add(":code", OracleDbType.Varchar2).Value = IccConfiguration.Trade.NpAuction.Requester.MarketCode;
                    var auctionDt = Util.CommandToDataTable(auctionCmd, conn);
                    if (verbose && auctionDt.Rows.Count == 0)
                        Console.WriteLine($"No auctions found for {dateCal.ToString(date,"d")}");
                    foreach (DataRow row in auctionDt.Rows)
                    {
                        commands.Add(CreateGetAuctionPricesCommand(row["id"].ToString(),receiver,routingParam));
                    }
                }
                else
                {
                    var auctionCmd = new OracleCommand("select 1 from trade_period where id = :auctionId");
                    auctionCmd.Parameters.Add(":auctionId", OracleDbType.Varchar2).Value = auctionId;
                    if (Util.CommandToScalar(auctionCmd, conn) == null)
                    {
                        if (verbose)
                            Console.WriteLine($"No auction {auctionId} defined");
                        return commands;
                    }

                    commands.Add(CreateGetAuctionPricesCommand(auctionId, receiver, routingParam));
                }
            }

            return commands;
        }

        private static DataExchangeExportMessage CreateGetAuctionPricesCommand(string auctionId, int receiver, Tuple<string,string> routingParam)
        {
            var message = new DataExchangeExportMessage();
            message.ProtocolId = (int)TsProtocol.NP_REQUEST_COMMAND;
            message.Protocol = TsProtocol.NP_REQUEST_COMMAND.ToString();
            message.SenderKey = receiver;
            message.ReceiverKey = receiver;
            message.RoutingAddress = routingParam.Item1;
            message.Format = "JSON";    // For eventlog message
            message.Priority = routingParam.Item2;
            message.MessageReference = Guid.NewGuid().ToString();

            var msgData = new NordPoolRequestCommand();
            msgData.Command = NordPoolRequestCommand.Commands.GetAuctionPrices;
            msgData.Parameters.Add(NordPoolRequestCommand.ParameterAuctionId, auctionId);
            message.SetMessageData(JsonConvert.SerializeObject(msgData));

            EventLogModuleItem.LogMessage(CommonGeneralInfo,$"Sending message for operator {receiver}, auction: {auctionId}.");
            return message;
        }

        private static UnityContainer RegisterDependencies()
        {
            var unityContainer = new UnityContainer();
            unityContainer.RegisterInstance<IEventLogModuleItem>(EventLogModuleItem);
            //unityContainer.RegisterInstance<EventLog>(new EventLog());
            unityContainer.RegisterType<ICriticalLogger, CriticalLogger>();
            unityContainer.RegisterType<IServiceEventLogger, ServiceEventLogger>();
            unityContainer.RegisterModule<DataExchangeApiModule>();
            return unityContainer;
        }
    }
}
