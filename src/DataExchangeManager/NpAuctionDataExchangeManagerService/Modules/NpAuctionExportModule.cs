using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using log4net.Util;
using Newtonsoft.Json;
using NP.Auction.Client.API;
using NP.Auction.Client.Configuration;
using NP.Auction.Client.Contracts;
using NP.Auction.Client.Exceptions;
using NP.Auction.Client.Utils;
using Oracle.ManagedDataAccess.Client;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeCommon;
using Powel.Icc.Messaging.DataExchangeCommon.Abstract;
using Powel.Icc.Messaging.DataExchangeCommon.Settings;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.NordPoolRequestCommand;
using Powel.Icc.Messaging.NpAuctionDataExchangeManager.Modules;
using Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService.Settings;
using Powel.Icc.Messaging.NpAuctionDataExchangeManager.Settings;
using Refit;

namespace Powel.Icc.Messaging.NpAuctionDataExchangeManager.NpAuctionDataExchangeManagerService.Modules
{
    public class NpAuctionExportModule : PushExportModule
    {
        internal const string Modulename = "NpAuction";
        public override string ModuleName => Modulename;
        private NpAuctionParameters _parameters;
        private readonly FileSystemWatcher _fileWatcher = new FileSystemWatcher();
        private NpAuctionModuleSettings Settings => _settings as NpAuctionModuleSettings;
        private IAuctionApiClient _apiClient;
        private readonly NpAuctionLogic _logic;
        private Dictionary<int,IAuthConfig> _authCache = new Dictionary<int,IAuthConfig>();
        private int _currentOpunKey = 0;
        private IDbConnection _dbConn = null;

        private readonly object _executionLock = new object();

        public NpAuctionExportModule(NpAuctionModuleSettings settings, MessageExporter messageExporter, IServiceEventLogger serviceEventLogger, NpAuctionLogic logic)
            : base(settings, messageExporter, serviceEventLogger)
        {
            _logic = logic;
            var paramFile = settings.ExportParametersFile;
            var paramPath = Environment.ExpandEnvironmentVariables(settings.ExportParametersFilePath);
            var paramFilter = settings.ExportParametersFileFilter;
            InitParametersFromJsonFile(Path.Combine(paramPath, paramFile));
            SetUpFileWatcherForParametersFile(paramPath, paramFilter);
        }

        private void Connect(int opunKey)
        {
            IAuthConfig auth = _currentOpunKey != 0 ? _authCache[_currentOpunKey] : null;
            bool doReconnect = false;
            if (opunKey != _currentOpunKey)
            {
                Log.Info($"Switching logon from {_currentOpunKey} to {opunKey}");
//#if DEBUG
//                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.GeneralInfoMsg,$"Switching logon from {_currentOpunKey} to {opunKey}");
//#endif
                doReconnect = true;
                if (_authCache.ContainsKey(opunKey))
                {
                    auth = _authCache[opunKey];
                    _currentOpunKey = opunKey;
                }
                else
                {
                    if (_dbConn == null)
                    {
                        _dbConn = Util.OpenConnection();
                    }
                    var dbAuth = GetAuthenticationParameters(opunKey, _dbConn);
                    if (dbAuth == null)
                    {
                        var msg = $"Operator with key {opunKey} does not exist or lacks auction authentication parameters.";
                        Log.Error(msg);
                        ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.AnErrorOccurred, msg);
                        throw new DataExchangeInvalidMetadataException(msg);
                    }
                    _authCache[opunKey] = dbAuth;
                    _currentOpunKey = opunKey;
                }

                if (auth != null && auth.Equals(_authCache[_currentOpunKey]))
                    doReconnect = false;
            }

            if (doReconnect)
            {
                Disconnect();
                auth = _authCache[_currentOpunKey];
//#if DEBUG
//                Log.DebugExt($"Connecting as {auth.Username}/{auth.Password} client: {auth.ClientId}/{auth.ClientSecret}");
//                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.GeneralInfoMsg,$"Connecting as {auth.Username}/{auth.Password} client: {auth.ClientId}/{auth.ClientSecret}");
//#else
//                Log.DebugExt($"Connecting as {auth.Username} client: {auth.ClientId}");
//#endif
                var ssoClient = InitializeSsoClient(auth);
                _apiClient = InitializeAuctionClient(ssoClient, Settings.AuctionUrl); // Handles Token expiration.
                Log.Debug("Connected.");
            }
        }

        private void Disconnect()
        {
            if (_dbConn != null)
                _dbConn.Close();
            Log.Debug("Disconnecting.");
//#if DEBUG
//            ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.GeneralInfoMsg,"Disconnecting");
//#endif
            _apiClient = null;
        }

        public override void Stop(TimeSpan timeout)
        {
            Log.Info("Stopping.");
            Disconnect();
            base.Stop(timeout);
        }

        protected sealed override string SendExportMessage(DataExchangeExportMessage document)
        {
            string ret;
            lock (_executionLock)
            {
                Log.Debug($"Starting export {document.ExternalReference}");
                ret = document.ExternalReference;
                try
                {
                    switch (document.ProtocolId)
                    {
                        case (int) TsProtocol.NP_REQUEST_COMMAND:
                            Log.Debug("RequestCommand.");
                            HandleRequestCommand(document);
                            break;
                        case (int)TsProtocol.NP_CURVE:
                            Log.Debug("SendCurve.");
                            HandleSendCurveAsync(document).GetAwaiter().GetResult();
                            break;
                        case (int)TsProtocol.NP_BLOCKS:
                            Log.Debug("SendBlocks.");
                            HandleSendBlocksAsync(document).GetAwaiter().GetResult();
                            break;
                        default:
                            throw new NotImplementedException($"Not implemented protocol: {document.ProtocolId}");
                    }
                }
                catch (Exception Ex)
                {
                    Log.Error($"{ModuleName}: MessageLogId: {document.MessageLogId.ToString()}", Ex);
                    LogError(Ex.Message);
                    throw;
                }
                Log.Debug("Export done.");
            }

            return ret;
        }

        private void HandleRequestCommand(DataExchangeExportMessage message)
        {
            var payload = message.GetMessageData();
            var command = JsonConvert.DeserializeObject<NordPoolRequestCommand>(payload);
            switch (command.Command)
            {
                case NordPoolRequestCommand.Commands.NewAuction:
                    Log.Debug("NewAuction.");
                    HandleNewAuctionCommandAsync(message, command).GetAwaiter().GetResult();    // To run it synchronously
                    break;
                case NordPoolRequestCommand.Commands.GetContract:
                    Log.Debug("GetContract.");
                    HandleGetContractCommandAsync(message, command).GetAwaiter().GetResult();
                    break;
                case NordPoolRequestCommand.Commands.PatchCurves:
                    Log.Debug("PatchCurves.");
                    HandlePatchCurvesCommandAsync(message, command).GetAwaiter().GetResult();
                    break;
                case NordPoolRequestCommand.Commands.PatchBlocks:
                    Log.Debug("PatchBlocks.");
                    HandlePatchBlocksCommandAsync(message, command).GetAwaiter().GetResult();
                    break;
                case NordPoolRequestCommand.Commands.GetCurveOrder:
                    Log.Debug("GetCurveOrder.");
                    HandleGetCurveOrderCommandAsync(message, command).GetAwaiter().GetResult();
                    break;
                case NordPoolRequestCommand.Commands.GetBlockOrder:
                    Log.Debug("GetBlockOrder.");
                    HandleGetBlockOrderCommandAsync(message, command).GetAwaiter().GetResult();
                    break;
                case NordPoolRequestCommand.Commands.GetAuctionOrders:
                    Log.Debug("GetAuctionOrders.");
                    HandleGetAuctionOrdersCommandAsync(message, command).GetAwaiter().GetResult();
                    break;
                case NordPoolRequestCommand.Commands.GetAuctionPortfolioVolumes:
                    Log.Debug("GetAuctionPortfolioVolumes.");
                    HandleGetAuctionPortfolioVolumesCommandAsync(message, command).GetAwaiter().GetResult();
                    break;
                case NordPoolRequestCommand.Commands.GetAuctionPrices:
                    Log.Debug("GetAuctionPrices.");
                    HandleGetAuctionPricesCommandAsync(message, command).GetAwaiter().GetResult();
                    break;
                default:
                    throw new NotImplementedException($"Not implemented command: {command.Command}");
            }
        }

        private async Task HandleNewAuctionCommandAsync(DataExchangeExportMessage message, NordPoolRequestCommand command)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }
            DateTime from, to;
            if (!DateTime.TryParse(command.Parameters[NordPoolRequestCommand.ParameterKeyFromDate], out from))
                return;
            if (!DateTime.TryParse(command.Parameters[NordPoolRequestCommand.ParameterKeyToDate], out to))
                return;
            Log.Debug($"Executing GetAuctions(From:{from},To:{to})");
            try
            {
                var availableAuctions = await _apiClient.GetAuctionsAsync(from, to);
                Log.Debug($"{availableAuctions.Count()} auctions received.");
                if (_parameters.SplitAuctions)
                {
                    foreach (var auction in availableAuctions)
                    {
                        _logic.SubmitImport(message, auction);
                    }
                }
                else _logic.SubmitImport(message, availableAuctions);

            }
            catch (AuctionApiException Ex)
            {
                Log.Error(Ex);
            }
            catch (ValidationApiException Ex)
            {
                Log.Error(Ex.Content.Detail, Ex);
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.AnErrorOccurred, Ex.Content.Detail);
            }
            catch (ApiException Ex)
            {
                Log.Error(Ex);
                if (Ex.StatusCode == HttpStatusCode.BadRequest)
                {
                    if (Ex.HasContent)
                    {
                        var content = await Ex.GetContentAsAsync<Dictionary<string, string>>();
                        string value;
                        if (content.TryGetValue("error", out value))
                        {
                            if (value.Equals("invalid_client"))
                                throw new DataExchangeInvalidMetadataException(message.MessageLogId,
                                    $"Authentication failure.", $"Participant internal key = {message.SenderKey}", Ex);
                        }
                    }
                }

                throw;
            }
            catch (HttpRequestException Ex)
            {
                Log.Error(ModuleName, Ex);
                var errMsg = Ex.Message;
                var inner = Ex.InnerException;
                while (inner != null)
                {
                    errMsg += " " + inner.Message;
                    inner = inner.InnerException;
                }
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.AnErrorOccurred, errMsg);
                if (Ex.InnerException is WebException)
                {
                    Log.Debug($"WebException status: {((WebException)Ex.InnerException).Status}");
                    if ((Ex.InnerException as WebException).Status == WebExceptionStatus.NameResolutionFailure)
                        RequestStop();
                }
                throw new DataExchangeConfigurationException(errMsg, Ex);
            }
            catch (AggregateException Ex)
            {
                var errMsg = Ex.Message;
                var inner = Ex.InnerException;
                while (inner != null)
                {
                    errMsg += " " + inner.Message;
                    inner = inner.InnerException;
                }

                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.AnErrorOccurred, errMsg);
            }
            catch (Exception Ex)
            {
                Log.Error(Ex);
                throw;
            }
        }

        private async Task HandleGetContractCommandAsync(DataExchangeExportMessage message, NordPoolRequestCommand command)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            string auctionId = command.Parameters[NordPoolRequestCommand.ParameterAuctionId];
            var portfolioIds = command.Parameters[NordPoolRequestCommand.ParameterPortfolios].Split(',');
            var areas = command.Parameters[NordPoolRequestCommand.ParameterAreaCodes].Split(',');
            Log.Debug($"Executing GetTrades(AuctionId:{auctionId},Portfolios:{command.Parameters[NordPoolRequestCommand.ParameterPortfolios]},areaCodes:{command.Parameters[NordPoolRequestCommand.ParameterAreaCodes]})");
            try
            {
                var auctionContracts = await _apiClient.GetTradesAsync(auctionId, portfolioIds, areas);
                Log.Debug($"{auctionContracts.Count()} auctions received.");
                _logic.SubmitImport(message, auctionContracts);
            }
            catch (AuctionApiException Ex)
            {
                Log.Error(Ex);
            }
            catch (ValidationApiException Ex)
            {
                // Auction results were not yet published.
                Log.Error(Ex.Content.Detail,Ex);
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.GeneralInfoMsg, Ex.Content.Detail);
            }
            catch (ApiException Ex)
            {
                // Handled like Exception.
                Log.Error(Ex);
                throw;
            }
            catch (AggregateException Ex)
            {
                var errMsg = Ex.Message;
                var inner = Ex.InnerException;
                while (inner != null)
                {
                    errMsg += " " + inner.Message;
                    inner = inner.InnerException;
                }
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.AnErrorOccurred, errMsg);
            }
            catch (Exception Ex)
            {
                Log.Error(Ex);
                throw;
            }
        }

        private async Task HandleSendCurveAsync(DataExchangeExportMessage message)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            var curveOrderRequest = JsonConvert.DeserializeObject<CurveOrderRequest>(message.GetMessageData());
            Log.Debug($"Executing PlaceCurveOrder(CurveOrder:{message.GetMessageData()})");
            try
            {
                var sendCurveResponse = await _apiClient.PlaceCurveOrder(curveOrderRequest);
                Log.Debug($"{sendCurveResponse.Curves.Count} curves accepted.");
                _logic.SubmitImport(message, sendCurveResponse);
            }
            catch (AuctionApiException ex)
            {
                Log.Error(ex);
                _logic.SubmitImport(message, CreateErrorMessage(ex, ex.HttpStatusCode == HttpStatusCode.BadRequest ? ", missing OrderId on bid form?" : null));
            }
            catch (HttpRequestException Ex)
            {
                Log.Error(ModuleName, Ex);
                var errMsg = Ex.Message;
                var inner = Ex.InnerException;
                while (inner != null)
                {
                    errMsg += " " + inner.Message;
                    inner = inner.InnerException;
                }
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.AnErrorOccurred, errMsg);
                if (Ex.InnerException is WebException)
                {
                    Log.Debug($"WebException status: {((WebException)Ex.InnerException).Status}");
                    if ((Ex.InnerException as WebException).Status == WebExceptionStatus.NameResolutionFailure)
                        RequestStop();
                }
                throw new DataExchangeConfigurationException(errMsg, Ex);
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                throw;
            }
        }

        private async Task HandleSendBlocksAsync(DataExchangeExportMessage message)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            var blockOrderRequest = JsonConvert.DeserializeObject<BlockOrderRequest>(message.GetMessageData());
            Log.Debug($"Executing PlaceBlockOrder(BlockOrder:{message.GetMessageData()})");
            try
            {
                var sendBlockResponse = await _apiClient.PlaceBlockOrder(blockOrderRequest);
                Log.Debug($"{sendBlockResponse.Blocks.Count} blocks accepted.");
                _logic.SubmitImport(message, sendBlockResponse);
            }
            catch (AuctionApiException ex)
            {
                Log.Error(ex);
                _logic.SubmitImport(message, CreateErrorMessage(ex, ex.HttpStatusCode == HttpStatusCode.BadRequest ? ", missing OrderId on blocks?" : null));
            }
            catch (HttpRequestException Ex)
            {
                Log.Error(ModuleName, Ex);
                var errMsg = Ex.Message;
                var inner = Ex.InnerException;
                while (inner != null)
                {
                    errMsg += " " + inner.Message;
                    inner = inner.InnerException;
                }
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.AnErrorOccurred, errMsg);
                if (Ex.InnerException is WebException)
                {
                    Log.Debug($"WebException status: {((WebException)Ex.InnerException).Status}");
                    if ((Ex.InnerException as WebException).Status == WebExceptionStatus.NameResolutionFailure)
                        RequestStop();
                }
                throw new DataExchangeConfigurationException(errMsg, Ex);
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                throw;
            }
        }

        private async Task HandlePatchCurvesCommandAsync(DataExchangeExportMessage message, NordPoolRequestCommand command)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            var curves = JsonConvert.DeserializeObject<List<Curve>>(command.Parameters[NordPoolRequestCommand.ParameterOrder]);
            Log.Debug($"Executing ModifyCurveOrder(MessageData:{message.GetMessageData()})");
            try
            {
                var modifyCurveOrderResponse = await _apiClient.ModifyCurveOrder(Guid.Parse(command.Parameters[NordPoolRequestCommand.ParameterOrderId]), curves).ConfigureAwait(false);
                Log.Debug("ModifyCurveOrder done.");
                Log.Debug($"{modifyCurveOrderResponse.Curves.Count} curves defined.");
                _logic.SubmitImport(message, modifyCurveOrderResponse);
            }
            catch (AuctionApiException ex)
            {
                Log.Error(ex);
                //try
                //{
                    _logic.SubmitImport(message, CreateErrorMessage(ex));
                //}
                //catch (Exception exc)
                //{
                //    Log.Error(exc);
                //}
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                throw;
            }
        }

        private async Task HandlePatchBlocksCommandAsync(DataExchangeExportMessage message, NordPoolRequestCommand command)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            Log.Debug($"Order:{command.Parameters[NordPoolRequestCommand.ParameterOrder]}");
            var blocks = JsonConvert.DeserializeObject<List<Block>>(command.Parameters[NordPoolRequestCommand.ParameterOrder]);
            Log.Debug($"Executing ModifyBlockOrder(orderId:{command.Parameters[NordPoolRequestCommand.ParameterOrderId]}, #blocks:{blocks.Count})");
            try
            {
                var modifyBlockOrderResponse = await _apiClient.ModifyBlockOrder(Guid.Parse(command.Parameters[NordPoolRequestCommand.ParameterOrderId]), blocks);
                Log.Debug($"ModifyBlockOrder done.");
                Log.Debug($"{modifyBlockOrderResponse.Blocks.Count} blocks defined.");
                _logic.SubmitImport(message, modifyBlockOrderResponse);
            }
            catch (AuctionApiException ex)
            {
                Log.Error(ex);
                _logic.SubmitImport(message, CreateErrorMessage(ex));
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                throw;
            }
        }

        private async Task HandleGetCurveOrderCommandAsync(DataExchangeExportMessage message, NordPoolRequestCommand command)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            Log.Debug($"Executing GetCurveOrder(OrderId:{command.Parameters[NordPoolRequestCommand.ParameterOrderId]})");
            try
            {
                var curveOrderResponse = await _apiClient.GetCurveOrderAsync(Guid.Parse(command.Parameters[NordPoolRequestCommand.ParameterOrderId]));
                Log.Debug($"{curveOrderResponse.Curves.Count} curves defined.");
                _logic.SubmitImport(message, curveOrderResponse);
            }
            catch (AuctionApiException ex)
            {
                Log.Error(ex);
                //try
                //{
                _logic.SubmitImport(message, CreateErrorMessage(ex));
                //}
                //catch (Exception exc)
                //{
                //    Log.Error(exc);
                //}
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                throw;
            }
        }

        private async Task HandleGetBlockOrderCommandAsync(DataExchangeExportMessage message, NordPoolRequestCommand command)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            Log.Debug($"Executing GetBlockOrder(MessageData:{message.GetMessageData()})");
            try
            {
                var blockOrderResponse = await _apiClient.GetBlockOrderAsync(Guid.Parse(command.Parameters[NordPoolRequestCommand.ParameterOrderId]));
                Log.Debug($"{blockOrderResponse.Blocks.Count} blocks defined.");
                _logic.SubmitImport(message, blockOrderResponse);
            }
            catch (AuctionApiException ex)
            {
                Log.Error(ex);
                _logic.SubmitImport(message, CreateErrorMessage(ex));
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                throw;
            }
        }

        private async Task HandleGetAuctionOrdersCommandAsync(DataExchangeExportMessage message, NordPoolRequestCommand command)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            Log.DebugExt($"Executing GetOrders(MessageData:{message.GetMessageData()})");
            var portfolioIds = command.Parameters[NordPoolRequestCommand.ParameterPortfolios].Split(',');
            var areas = command.Parameters[NordPoolRequestCommand.ParameterAreaCodes].Split(',');
            try
            {
                var ordersResponse = await _apiClient.GetOrdersAsync(command.Parameters[NordPoolRequestCommand.ParameterAuctionId],portfolioIds,areas);
                Log.Debug($"{ordersResponse.CurveOrders.Count} curve orders, {ordersResponse.BlockLists.Count} blocks defined.");
                _logic.SubmitImport(message, ordersResponse);
            }
            catch (AuctionApiException ex)
            {
                Log.Error(ex);
                _logic.SubmitImport(message, CreateErrorMessage(ex));
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                throw;
            }
        }

        private async Task HandleGetAuctionPortfolioVolumesCommandAsync(DataExchangeExportMessage message, NordPoolRequestCommand command)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            Log.DebugExt($"Executing GetPortfolioVolumes(MessageData:{message.GetMessageData()})");
            var portfolioIds = command.Parameters[NordPoolRequestCommand.ParameterPortfolios].Split(',');
            var areas = command.Parameters[NordPoolRequestCommand.ParameterAreaCodes].Split(',');
            try
            {
                var portfolioVolumesResponse = await _apiClient.GetPortfolioVolumesAsync(command.Parameters[NordPoolRequestCommand.ParameterAuctionId],portfolioIds,areas);
                Log.Debug($"{portfolioVolumesResponse.PortfolioNetVolumes.Count} net volumes.");
                _logic.SubmitImport(message, portfolioVolumesResponse);
            }
            catch (AuctionApiException ex)
            {
                Log.Error(ex);
                _logic.SubmitImport(message, CreateErrorMessage(ex));
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                throw;
            }
        }

        private async Task HandleGetAuctionPricesCommandAsync(DataExchangeExportMessage message, NordPoolRequestCommand command)
        {
            Connect(message.SenderKey);
            if (_apiClient == null)
            {
                Log.Error("Connect failed.");
                throw new DataExchangeConfigurationException("Connect failed.");
            }

            Log.DebugExt($"Executing GetPrices(MessageData:{message.GetMessageData()})");
            try
            {
                var pricesResponse = await _apiClient.GetPrices(command.Parameters[NordPoolRequestCommand.ParameterAuctionId]);
                Log.Debug($"{pricesResponse.Contracts.Count} contracts.");
                _logic.SubmitImport(message, pricesResponse);
            }
            catch (AuctionApiException ex)
            {
                Log.Error(ex);
                _logic.SubmitImport(message, CreateErrorMessage(ex));
            }
            catch (Exception ex)
            {
                Log.Error(ex);
                throw;
            }
        }

        private static NordPoolErrorMessage CreateErrorMessage(AuctionApiException Exc, string AdditionalInfo = null)
        {
            return new NordPoolErrorMessage
            {
                StatusCode = (int)Exc.HttpStatusCode,
                Message = Exc.Message + AdditionalInfo ?? String.Empty
            };
        }

        private static IAuthConfig GetAuthenticationParameters(long OpunKey, IDbConnection DbConnect)
        {
            IAuthConfig ret = null;
            var cmd = new OracleCommand("select np_user,icc_trade.decrypt(np_password) as np_password,np_client_name,icc_trade.decrypt(np_client_secret) as np_client_secret from oprtunit where opun_key = :opunKey");
            cmd.Parameters.Add(":opunKey", OracleDbType.Int64).Value = OpunKey;
            var oprtDt = Util.CommandToDataTable(cmd, DbConnect);
            if (oprtDt.Rows.Count > 0)
            {
                var row = oprtDt.Rows[0];
                if (Util.IsNull(row["np_user"]) || Util.IsNull(row["np_password"]) || Util.IsNull(row["np_client_name"]) || Util.IsNull(row["np_client_secret"]))
                    return ret;
                ret = new AuthenticationParameters();
                ret.Username = row["np_user"].ToString();
                ret.Password = row["np_password"].ToString();
                ret.ClientId = row["np_client_name"].ToString();
                ret.ClientSecret = row["np_client_secret"].ToString();
            }
            return ret;
        }

        #region ParameterFile

        private void InitParametersFromJsonFile(string fullPathFilename)
        {
            lock (_executionLock)
            {
                ServiceEventLogger.LogMessage(Constants.MessageIdentifiers.GeneralInfoMsg, $"Loading {fullPathFilename}");
                using (var strReader = new StreamReader(fullPathFilename))
                {
                    _parameters = JsonConvert.DeserializeObject<NpAuctionParameters>(strReader.ReadToEnd());
                }

                Log.DebugExt(() => $"{ModuleName}: New parameters loaded. SplitAuctions: {_parameters.SplitAuctions}");
            }
        }

        private void SetUpFileWatcherForParametersFile(string paramPath, string paramFilter)
        {
            _fileWatcher.NotifyFilter = NotifyFilters.CreationTime | NotifyFilters.LastWrite;
            _fileWatcher.Path = paramPath;
            _fileWatcher.Filter = paramFilter;
            _fileWatcher.Changed += OnParametersFileChanged;
            _fileWatcher.EnableRaisingEvents = true;
        }

        private void OnParametersFileChanged(object source, FileSystemEventArgs eArgs)
        {
            InitParametersFromJsonFile(eArgs.FullPath);
        }

        #endregion

        #region API

        private ICachedSsoApiClient InitializeSsoClient(IAuthConfig authentication)
        {
            Log.Debug($"Fetching token on {Settings.SsoUrl}");
            var ssoUrl = Settings.SsoUrl;
            var ssoClient = RestService.For<ISsoApiClient>(ssoUrl);
            return new CachedSsoApiClient(authentication, ssoClient);
        }

        private static IAuctionApiClient InitializeAuctionClient(ICachedSsoApiClient ssoClient,string auctionUrl)
        {
            try
            {
                Log.Debug($"Logging on to {auctionUrl}");
                return RestService.For<IAuctionApiClient>(
                    new HttpClient(new AuthenticatedHttpClientHandler(ssoClient))
                    {
                        BaseAddress = new Uri(auctionUrl)
                    });
            }
            catch (Exception ex)
            {
                Log.Error(ex);
            }

            return null;
        }

        #endregion
    }
}
