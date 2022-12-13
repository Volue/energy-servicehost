using System.Diagnostics;
using System.Reflection;
using Powel.Icc.Diagnostics;
using TASE2.Library.Client;
using TASE2.Library.Common;
using TASE2.Library.Server;
using IccpConstants = Powel.Icc.Messaging.IccpDataExchangeManager.Settings.Constants;

namespace Powel.Icc.Messaging.IccpDataExchangeManager.Modules
{
    public class IccpDualRoleImportModule : IccpImportModule
    {
        private Endpoint _endpoint = null;
        private Server _server = null;
        public IccpDualRoleImportModule(IccpLogic logic, IServiceEventLogger serviceEventLogger) : base(logic, serviceEventLogger)
        {
        }

        protected override bool Connect()
        {
            Log.Debug("Dual role initiating.");
            // SCADA is the connection initiator
            // Create a passive endpoint
            // Don't instantiate new objects on reconnect.
            var doConnect = false;
            if (_endpoint == null)
            {
                TLSConfiguration tlsConfig = null;
                if (_iccpParameters.LocalParameters.Tls.UseTls)
                {
                    tlsConfig = CreateTlsConfiguration(_iccpParameters.LocalParameters.Tls.TlsChainValidation, _iccpParameters.LocalParameters.Tls.TlsAllowOnlyKnownCertificates,
                        _iccpParameters.LocalParameters.Tls.TlsOwnKeyFile, _iccpParameters.LocalParameters.Tls.TlsOwnCertificateFile,
                        _iccpParameters.LocalParameters.Tls.TlsCACertificateFiles, _iccpParameters.TlsCertificateDirectory, _iccpParameters.LocalParameters.Tls.TlsOwnKeyPassword);
                }
                _endpoint = new Endpoint(true, tlsConfig);
                _endpoint.SetIdentity(FileVersionInfo.GetVersionInfo(Assembly.GetEntryAssembly().Location).CompanyName, ModuleName, Assembly.GetExecutingAssembly().GetName().Version.ToString());
                Log.Debug($"Local address: {_iccpParameters.LocalParameters.ApTitle}/{_iccpParameters.LocalParameters.AeQualifier}");
                _endpoint.SetLocalApTitle(_iccpParameters.LocalParameters.ApTitle, _iccpParameters.LocalParameters.AeQualifier);
                _endpoint.SetLocalAddresses(_iccpParameters.LocalParameters.PSelectorArray, _iccpParameters.LocalParameters.SSelectorArray, _iccpParameters.LocalParameters.TSelectorArray);
                //_endpoint.SetRemoteApTitle(_iccpParameters.Clients[_clientIndex].ApTitle, _iccpParameters.Clients[_clientIndex].AeQualifier);
                //_endpoint.SetRemoteAddresses(_iccpParameters.Clients[_clientIndex].PSelectorArray, _iccpParameters.Clients[_clientIndex].SSelectorArray, _iccpParameters.Clients[_clientIndex].TSelectorArray);
                _endpoint.SetConnectionHandler(endpointConnectionHandler, this);
                _endpoint.SetConnectTimeout(30000);

                doConnect = true;
            }

            // Add a TASE.2 server to the endpoint
            if (_server == null)
            {
                var dataModel = new DataModel();
                var vccDomain = dataModel.VCC;
                vccDomain.AddInformationMessageObject(64);
                vccDomain.AddInformationMessageObject(1024);
                _server = new Server(dataModel, _endpoint);
            }

            string apTitle = _iccpParameters.Clients[_clientIndex].ApTitle;
            if (_client == null)
            {
                _client = new Client(_endpoint);
                /* remote address is required for the client to associate with the correct incoming TCP client connection */
                int aeQualifier = _iccpParameters.Clients[_clientIndex].AeQualifier;
                Log.Debug($"Remote association: {apTitle}/{aeQualifier}");
                _client.SetRemoteApTitle(apTitle, aeQualifier);
                _client.SetRemoteAddresses(_iccpParameters.Clients[_clientIndex].PSelectorArray, _iccpParameters.Clients[_clientIndex].SSelectorArray, _iccpParameters.Clients[_clientIndex].TSelectorArray);
                _client.SetStateChangedHandler(clientStateChangedHandler, this);
                if (_iccpParameters.ReceiveInformationMessages)
                    _client.SetInformationMessageHandler(informationMessageHandler, this);
            }

            if (doConnect)
                _endpoint.Connect();
            var ret = _endpoint.WaitForState(EndpointState.LISTENING, (int)_iccpParameters.LocalParameters.ListeningTimeout.TotalMilliseconds) && _client.GetState() == ClientState.STATE_CONNECTED;
            Log.Debug($"Endpoint state: {_endpoint.State}. Client state: {_client.GetState()}");
            if (!ret)
                _reconnect = true;
            else
            {
                ServiceEventLogger.LogMessage(IccpConstants.MessageIdentifiers.ConnectedMessage, apTitle);
                _reconnect = false;
                if (_iccpParameters.ReceiveInformationMessages)
                    _client.IMTransferSetEnable();
            }
            return ret;
        }

        private static void endpointConnectionHandler(object parameter, Endpoint endpoint, EndpointConnection connection, bool connect)
        {
            var module = parameter as IccpDualRoleImportModule;
            if (connect)
            {
                Log.Debug(string.Format("Peer {0} {1} connected from {2} (max PDU size: {3})", connection.PeerApTitle, connection.PeerAeQualifier, connection.PeerAddress, connection.MaxPduSize));
            }
            else
            {
                Log.Debug(string.Format("Peer {0} {1} disconnected from {2}", connection.PeerApTitle, connection.PeerAeQualifier, connection.PeerAddress));
                module?.OnConnectionClosed();
            }
        }

        /* callback handler that is called when the client state changes (e.g. CONNECTED to CLOSED) */
        private static void clientStateChangedHandler(object parameter, Client client, ClientState newState)
        {
            Log.Debug($"Client state changed to " + newState.ToString());
        }
    }
}
