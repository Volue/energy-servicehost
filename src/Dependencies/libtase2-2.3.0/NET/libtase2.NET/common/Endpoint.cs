using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using TASE2.Library.Server;

namespace TASE2.Library.Common
{

    public enum EndpointState
    {
        /// <summary>
        /// endpoint has not been used yet.
        /// </summary>
        IDLE = 0,

        /// <summary>
        /// passive endpoint is listening for incoming TCP client connections.
        /// </summary>
        LISTENING = 1,

        /// <summary>
        /// active endpoint is connected to remote TCP server.
        /// </summary>
        CONNECTED = 2,

        /// <summary>
        /// active endpoint is connecting to remote TCP server.
        /// </summary>
        CONNECTING = 3,

        /// <summary>
        /// connection attempt failed or endpoint cannot allocate local TCP port.
        /// </summary>
        ERROR = 4
    }

    /// <summary>
    /// Endpoint state changed handler (is called whenever the endpoint state changed).
    /// </summary>
    public delegate void EndpointStateChangedHandler(object parameter, Endpoint endpoint, EndpointState state);

    /// <summary>
    /// Endpoint connection handler (is called whenever a peer is connected or disconnected)
    /// </summary>
    public delegate void EndpointConnectionHandler(object parameter, Endpoint endpoint, EndpointConnection connection, [MarshalAs(UnmanagedType.I1)] bool connect);


    /// <summary>
    /// Endpoint connection accept handler (can be used to accept or reject new connections)
    /// </summary>
    public delegate bool EndpointConnectionAcceptHandler(object parameter, Endpoint endpoint, string peerAddress, BilateralTable blt, AuthenticationMethod authMethod, byte[] authValue);

    /// <summary>
    /// Represents an active or passive TASE.2/MMS endpoint
    /// </summary>
    public class Endpoint : IDisposable
    {
        private static Dictionary<IntPtr, WeakReference<Endpoint>> knownEndpoints = new Dictionary<IntPtr, WeakReference<Endpoint>>();

        private static Endpoint LookupKnownEndpoint(IntPtr key)
        {
            WeakReference<Endpoint> endpointRef = null;

            if (knownEndpoints.TryGetValue(key, out endpointRef))
            {
                if (endpointRef != null)
                {
                    Endpoint endpoint = null;

                    if (endpointRef.TryGetTarget(out endpoint))
                    {
                        return endpoint;
                    }
                    else
                    {
                        knownEndpoints.Remove(key);
                    }
                }
            }

            return null;
        }

        internal static Endpoint LookupKnownEndpointOrCreate(IntPtr key)
        {
            Endpoint endpoint = null;

            lock (knownEndpoints)
            {
                endpoint = LookupKnownEndpoint(key);

                if (endpoint == null)
                {
                    endpoint = new Endpoint(key);
                }
            }

            return endpoint;
        }


        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Endpoint_create(IntPtr tlsConfiguration, [MarshalAs(UnmanagedType.I1)] bool passive);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_destroy(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setTLSConfiguration(IntPtr self, IntPtr tlsConfiguration);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void
        Tase2_Endpoint_setAddressMatchingRules(IntPtr self,
            [MarshalAs(UnmanagedType.I1)] bool apTitle, [MarshalAs(UnmanagedType.I1)] bool aeQualifier,
            [MarshalAs(UnmanagedType.I1)] bool pSelector, [MarshalAs(UnmanagedType.I1)] bool sSelector,
            [MarshalAs(UnmanagedType.I1)] bool tSelector);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setIdentity(IntPtr self, string vendor, string model, string revision);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setLocalApTitle(IntPtr self, string apTitle, int aeQualifier);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setLocalApTitleEx(IntPtr self, string apTitle);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setLocalAeQualifier(IntPtr self, int aeQualifier);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setLocalAeInvocationId(IntPtr self, int aeInvocationId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setLocalApInvocationId(IntPtr self, int apInvocationId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setLocalIpAddress(IntPtr self, string hostname);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setLocalTcpPort(IntPtr self, int tcpPort);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setRemoteApTitle(IntPtr self, string apTitle, int aeQualifier);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setRemoteApTitleEx(IntPtr self, string apTitle);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setRemoteAeQualifier(IntPtr self, int aeQualifier);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setRemoteAeInvocationId(IntPtr self, int aeInvocationId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setRemoteApInvocationId(IntPtr self, int apInvocationId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setRemoteIpAddress(IntPtr self, string hostname);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setRemoteTcpPort(IntPtr self, int tcpPort);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setRequestTimeout(IntPtr self, int timeoutInMs);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setConnectTimeout(IntPtr self, int timeoutInMs);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_connect(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_disconnect(IntPtr self, int disconnectMethod);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_disconnectPeer(IntPtr self, string ipAddr);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_Endpoint_isConnected(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Endpoint_connectedPeers(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_Endpoint_waitForState(IntPtr self, int state, int timeoutInMs);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Endpoint_getState(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Endpoint_setMaxPduSize(IntPtr self, int maxPduSize);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Endpoint_setMaxServOutstandingCalling(IntPtr self, int value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Endpoint_setMaxServOutstandingCalled(IntPtr self, int value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_Endpoint_isPassive(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Endpoint_getId(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setId(IntPtr self, string id);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_addServer(IntPtr self, IntPtr server);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Endpoint_removeServer(IntPtr self, IntPtr server);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Endpoint_removeClient(IntPtr self, IntPtr server);

        [StructLayout(LayoutKind.Sequential)]
        private struct NativeTSelector
        {
            public byte size;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)] public byte[] value;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct NativeSSelector
        {
            public byte size;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)] public byte[] value;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct NativePSelector
        {
            public byte size;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)] public byte[] value;
        }

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setLocalAddresses(IntPtr self, NativePSelector pSelector, NativeSSelector sSelector, NativeTSelector tSelector);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setRemoteAddresses(IntPtr self, NativePSelector pSelector, NativeSSelector sSelector, NativeTSelector tSelector);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void InternalStateChangedHandler(IntPtr endpoint, IntPtr parameter, int state);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setStateChangedHandler(IntPtr self, InternalStateChangedHandler handler, IntPtr parameter);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void Tase2_Endpoint_ConnectionHandler(IntPtr endpoint, IntPtr parameter, IntPtr connection, [MarshalAs(UnmanagedType.I1)] bool connect);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setConnectionHandler(IntPtr self, Tase2_Endpoint_ConnectionHandler handler, IntPtr parameter);


        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private delegate bool Tase2_Endpoint_ConnectionAcceptHandler(IntPtr endpoint, IntPtr parameter, string peerAddress, IntPtr bltPtr, int authMethod, IntPtr authValue, int authValueLen);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Endpoint_setConnectionAcceptHandler(IntPtr self, Tase2_Endpoint_ConnectionAcceptHandler handler, IntPtr parameter);

        internal IntPtr self;

        bool manageNativeInstance = true;

        private EndpointStateChangedHandler stateChangedHandler = null;
        private object stateChangedHandlerParameter = null;
        private InternalStateChangedHandler stateChangedHanderRef = null;

        private void internalStateChangedHandler(IntPtr endpoint, IntPtr parameter, int state)
        {
            if (stateChangedHandler != null)
            {
                stateChangedHandler(stateChangedHandlerParameter, this, (EndpointState)state);
            }
        }

        private List<EndpointConnection> connections = new List<EndpointConnection>();

        private EndpointConnectionHandler connectionHandler = null;
        private object connectionHandlerParameter = null;
        private Tase2_Endpoint_ConnectionHandler connectionHandlerRef = null;

        private void internalConnectionHandler(IntPtr endpoint, IntPtr parameter, IntPtr conPtr, bool connect)
        {
            EndpointConnection connection = null;

            foreach (EndpointConnection con in connections)
            {
                if (con.self == conPtr)
                {
                    connection = con;
                    break;
                }
            }

            if (connection == null)
            {
                connection = new EndpointConnection(conPtr);
                connections.Add(connection);
            }

            if (connectionHandler != null)
            {
                connectionHandler(connectionHandlerParameter, this, connection, connect);
            }

            if (connect == false)
            {
                connections.Remove(connection);
            }
        }

        internal EndpointConnection LookupEndpointConnection(IntPtr conPtr)
        {
            EndpointConnection endpointConnection = connections.Find(y => y.self == conPtr);

            return endpointConnection;
        }

        private EndpointConnectionAcceptHandler connectionAcceptHandler = null;
        private object connectionAcceptHandlerParameter = null;
        private Tase2_Endpoint_ConnectionAcceptHandler connectionAcceptHandlerRef = null;

        private bool internalConnectionAcceptHandler(IntPtr endpoint, IntPtr parameter, string peerAddress, IntPtr bltPtr, int authMethod, IntPtr authValue, int authValueLen)
        {
            if (connectionAcceptHandler != null)
            {
                BilateralTable blt = null;

                if (bltPtr != IntPtr.Zero)
                {
                    blt = BilateralTable.GetInstance(bltPtr);
                }

                byte[] authVal = null;

                if ((authValue != IntPtr.Zero) && (authValueLen > 0))
                {
                    authVal = new byte[authValueLen];

                    Marshal.Copy(authValue, authVal, 0, authValueLen);
                }

                return connectionAcceptHandler(connectionAcceptHandlerParameter, this, peerAddress, blt, (AuthenticationMethod)authMethod, authVal);
            }
            else
            {
                return true;
            }
        }

        private void InitializeManagedEndpointInstance()
        {
            stateChangedHanderRef = new InternalStateChangedHandler(internalStateChangedHandler);

            Tase2_Endpoint_setStateChangedHandler(self, stateChangedHanderRef, IntPtr.Zero);

            connectionHandlerRef = new Tase2_Endpoint_ConnectionHandler(internalConnectionHandler);

            Tase2_Endpoint_setConnectionHandler(self, connectionHandlerRef, IntPtr.Zero);

            connectionAcceptHandlerRef = new Tase2_Endpoint_ConnectionAcceptHandler(internalConnectionAcceptHandler);

            Tase2_Endpoint_setConnectionAcceptHandler(self, connectionAcceptHandlerRef, IntPtr.Zero);

            knownEndpoints.Add(self, new WeakReference<Endpoint>(this));
        }

        private TLSConfiguration tlsConfiguration = null;
        private TLSConfiguration oldTlsConfiguration = null;

        /// <summary>
        /// Initializes a new instance of the <see cref="TASE2.Library.Common.Endpoint"/> class.
        /// </summary>
        /// <param name="passive">If set to <c>true</c> use passive mode (TCP server), otherwise use active mode (TCP client).</param>
        /// <param name="tlsConfiguration">TLS configuration instance, or <c>null</c> when not to use TLS.</param>
        public Endpoint(bool passive, TLSConfiguration tlsConfiguration = null)
        {
            IntPtr tlsConfigPtr = IntPtr.Zero;

            this.tlsConfiguration = tlsConfiguration;

            if (tlsConfiguration != null)
                tlsConfigPtr = tlsConfiguration.Self;

            self = Tase2_Endpoint_create(tlsConfigPtr, passive);

            lock (knownEndpoints)
            {
                InitializeManagedEndpointInstance();
            }
        }

        private Endpoint(IntPtr epPtr)
        {
            self = epPtr;
            manageNativeInstance = false;

            // at this point "knownEndpoints" is already locked by caller
            InitializeManagedEndpointInstance();
        }

        /// <summary>
        /// Set the TLS Configuration during runtime.
        /// </summary>
        /// <remarks>
        /// New configuration will only be active when the endpoint is restarted
        /// </remarks>
        /// <param name="tlsConfiguration">the TLS configuration object to use</param>
        public void SetTLSConfiguration(TLSConfiguration tlsConfiguration)
        {
            if (this.tlsConfiguration != null)
                this.oldTlsConfiguration = this.tlsConfiguration;

            this.tlsConfiguration = tlsConfiguration;

            Tase2_Endpoint_setTLSConfiguration(self, tlsConfiguration.Self);
        }

        /// <summary>
        /// Releases all resource used by the <see cref="TASE2.Library.Common.Endpoint"/> object.
        /// </summary>
        /// <remarks>Call <see cref="Dispose"/> when you are finished using the <see cref="TASE2.Library.Common.Endpoint"/>. The
        /// <see cref="Dispose"/> method leaves the <see cref="TASE2.Library.Common.Endpoint"/> in an unusable state.
        /// After calling <see cref="Dispose"/>, you must release all references to the
        /// <see cref="TASE2.Library.Common.Endpoint"/> so the garbage collector can reclaim the memory that the
        /// <see cref="TASE2.Library.Common.Endpoint"/> was occupying.</remarks>
        public void Dispose()
        {
            lock (this)
            {
                if (self != IntPtr.Zero)
                {
                    // remove from static endpoint list
                    lock (knownEndpoints)
                    {
                        knownEndpoints.Remove(self);

                        if (manageNativeInstance)
                            Tase2_Endpoint_destroy(self);

                        self = IntPtr.Zero;
                    }
                }
            }
        }

        ~Endpoint()
        {
            Dispose();
        }

        /// <summary>
        /// Gets or sets the ID string of the endpoint. The ID string can be used by the application to identify the endpoint.
        /// </summary>
        /// <value>the ID string</value>
        public string Id
        {
            get
            {
                IntPtr idStrPtr = Tase2_Endpoint_getId(self);
                return System.Runtime.InteropServices.Marshal.PtrToStringAnsi(idStrPtr);
            }
            set
            {
                Tase2_Endpoint_setId(self, value);
            }
        }

        /// <summary>
        /// Gets a value indicating whether this <see cref="T:TASE2.Library.Common.Endpoint"/> is passive.
        /// </summary>
        /// <value><c>true</c> if is passive; if active, <c>false</c>.</value>
        public bool IsPassive
        {
            get
            {
                return Tase2_Endpoint_isPassive(self);
            }
        }

        /// <summary>
        /// Set a handler that is called whenever the state of the endpoint changes.
        /// </summary>
        /// <param name="handler">handler delegate</param>
        /// <param name="parameter">parameter that is passed to the handler.</param>
        public void SetStateChangedHandler(EndpointStateChangedHandler handler, object parameter)
        {
            this.stateChangedHandler = handler;
            this.stateChangedHandlerParameter = parameter;
        }

        /// <summary>
        /// Set a hander that is called whenever a peer is connected or disconnected
        /// </summary>
        /// <param name="handler">handler delegate</param>
        /// <param name="parameter">parameter that is passed to the handler.</param>
        public void SetConnectionHandler(EndpointConnectionHandler handler, object parameter)
        {
            this.connectionHandler = handler;
            this.connectionHandlerParameter = parameter;
        }

        /// <summary>
        /// Set the connection accept handler (can be used to accept or reject new connections)
        /// </summary>
        /// <param name="handler">handler delegate</param>
        /// <param name="parameter">parameter that is passed to the handler.</param>
        public void SetConnectionAcceptHandler(EndpointConnectionAcceptHandler handler, object parameter)
        {
            this.connectionAcceptHandler = handler;
            this.connectionAcceptHandlerParameter = parameter;
        }

        /// <summary>
        /// Sets the address matching rules for connection endpoint identification. In the default configuration
        /// onle ap-title and ae-qualifier are used for matching - the other parameters are ignored.
        /// </summary>
        /// <param name="apTitle">If set to <c>true</c> ap-title has to match.</param>
        /// <param name="aeQualifier">If set to <c>true</c> ae-qualifier has to match.</param>
        /// <param name="pSelector">If set to <c>true</c> p-selector has to match.</param>
        /// <param name="sSelector">If set to <c>true</c> s-selector has to match.</param>
        /// <param name="tSelector">If set to <c>true</c> t-selector has to match.</param>
        public void SetAddressMatchingRules(bool apTitle, bool aeQualifier = false, bool pSelector = false,
            bool sSelector = false, bool tSelector = false)
        {
            Tase2_Endpoint_setAddressMatchingRules(self, apTitle, aeQualifier, pSelector, sSelector, tSelector);
        }

        /// <summary>
        /// Sets the endpoint identity
        /// </summary>
        /// <param name="vendor">Vendor name</param>
        /// <param name="model">Model name</param>
        /// <param name="revision">Revision/Version</param>
        public void SetIdentity(string vendor, string model, string revision)
        {
            Tase2_Endpoint_setIdentity(self, vendor, model, revision);
        }

        /// <summary>
        /// Sets the local IP address.
        /// </summary>
        /// <param name="ipAddress">local IP address to use.</param>
        public void SetLocalIpAddress(string ipAddress)
        {
            Tase2_Endpoint_setLocalIpAddress(self, ipAddress);
        }

        /// <summary>
        /// Sets the local TCP port (only for passive mode). If not set, the default port is used
        /// (102 for MMS, 3872 for MMS/TLS).
        /// </summary>
        /// <param name="tcpPort">local TCP port</param>
        public void SetLocalTcpPort(int tcpPort)
        {
            Tase2_Endpoint_setLocalTcpPort(self, tcpPort);
        }

        /// <summary>
        /// Sets the local AP-title
        /// </summary>
        /// <param name="apTitle">local AP-title</param>
        /// <param name="aeQualifier">local AE-qualifier</param>
        public void SetLocalApTitle(string apTitle, int aeQualifier)
        {
            Tase2_Endpoint_setLocalApTitle(self, apTitle, aeQualifier);
        }

        /// <summary>
        /// Sets the local AP-title
        /// </summary>
        /// <param name="apTitle">local AP-title</param>
        public void SetLocalApTitle(string apTitle)
        {
            Tase2_Endpoint_setLocalApTitleEx(self, apTitle);
        }

        /// <summary>
        /// Set local AE-qualifier
        /// </summary>
        /// <param name="aeQualifier">the new AE-qualifier value.</param>
        public void SetLocalAeQualifier(int aeQualifier)
        {
            Tase2_Endpoint_setLocalAeQualifier(self, aeQualifier);
        }

        /// <summary>
        /// Sets the local AE invocation identifier.
        /// </summary>
        /// <param name="aeInvocationId">AE invocation identifier.</param>
        public void SetLocalAeInvocationId(int aeInvocationId)
        {
            Tase2_Endpoint_setLocalAeInvocationId(self, aeInvocationId);
        }

        /// <summary>
        /// Sets the local AP invocation identifier.
        /// </summary>
        /// <param name="apInvocationId">AP invocation identifier.</param>
        public void SetLocalApInvocationId(int apInvocationId)
        {
            Tase2_Endpoint_setLocalApInvocationId(self, apInvocationId);
        }

        /// <summary>
        /// Sets the default remote AP-title for active mode
        /// </summary>
        /// <param name="apTitle">remote AP-title</param>
        /// <param name="aeQualifier">remote AE-qualifier</param>
        public void SetRemoteApTitle(string apTitle, int aeQualifier)
        {
            Tase2_Endpoint_setRemoteApTitle(self, apTitle, aeQualifier);
        }

        /// <summary>
        /// Sets the remote AP-title
        /// </summary>
        /// <param name="apTitle">local AP-title</param>
        public void SetRemoteApTitle(string apTitle)
        {
            Tase2_Endpoint_setRemoteApTitleEx(self, apTitle);
        }

        /// <summary>
        /// Set remote AE-qualifier
        /// </summary>
        /// <param name="aeQualifier">the new AE-qualifier value.</param>
        public void SetRemoteAeQualifier(int aeQualifier)
        {
            Tase2_Endpoint_setRemoteAeQualifier(self, aeQualifier);
        }

        /// <summary>
        /// Sets the remote AE invocation identifier.
        /// </summary>
        /// <param name="aeInvocationId">AE invocation identifier.</param>
        public void SetRemoteAeInvocationId(int aeInvocationId)
        {
            Tase2_Endpoint_setRemoteAeInvocationId(self, aeInvocationId);
        }

        /// <summary>
        /// Sets the remote AP invocation identifier.
        /// </summary>
        /// <param name="apInvocationId">AP invocation identifier.</param>
        public void SetRemoteApInvocationId(int apInvocationId)
        {
            Tase2_Endpoint_setRemoteApInvocationId(self, apInvocationId);
        }

        /// <summary>
        /// Sets the local addresses for ISO layers (transport, session, presentation) from a string representations of the addresses
        /// </summary>
        /// The string is formated as a sequence of bytes represented as hexadecimal numbers (e.g. "00 00 01 1a")
        /// <param name='pSelector'>
        /// presentation layer address
        /// </param>
        /// <param name='sSelector'>
        /// session layer address
        /// </param>
        /// <param name='tSelector'>
        /// ISO COTP transport layer address
        /// </param>
        /// <exception cref="System.FormatException">When an element is not formated correctly</exception>
        /// <exception cref="System.OverflowException">When an element is out of range</exception>
        /// <exception cref="System.ArgumentOutOfRangeException">When the number of address elements is out of range</exception>
        public void SetLocalAddresses(string pSelector, string sSelector, string tSelector)
        {
            string[] pSelParts = pSelector.Split(' ');

            byte[] psel = new byte[pSelParts.Length];

            for (int i = 0; i < pSelParts.Length; i++)
            {
                psel[i] = Convert.ToByte(pSelParts[i], 16);
            }

            string[] sSelParts = sSelector.Split(' ');

            byte[] ssel = new byte[sSelParts.Length];

            for (int i = 0; i < sSelParts.Length; i++)
            {
                ssel[i] = Convert.ToByte(sSelParts[i], 16);
            }

            string[] tSelParts = tSelector.Split(' ');

            byte[] tsel = new byte[tSelParts.Length];

            for (int i = 0; i < tSelParts.Length; i++)
            {
                tsel[i] = Convert.ToByte(tSelParts[i], 16);
            }

            SetLocalAddresses(psel, ssel, tsel);
        }

        /// <summary>
        /// Sets the local addresses for ISO layers (transport, session, presentation)
        /// </summary>
        /// <param name='pSelector'>
        /// presentation layer address
        /// </param>
        /// <param name='sSelector'>
        /// session layer address
        /// </param>
        /// <param name='tSelector'>
        /// ISO COTP transport layer address
        /// </param>
        /// <exception cref="System.ArgumentOutOfRangeException">When the number of address elements is out of range</exception>
        public void SetLocalAddresses(byte[] pSelector, byte[] sSelector, byte[] tSelector)
        {
            if (pSelector.Length > 16)
                throw new ArgumentOutOfRangeException("pSelector", "maximum size (16) exceeded");

            NativePSelector nativePSelector;
            nativePSelector.size = (byte)pSelector.Length;
            nativePSelector.value = new byte[16];

            for (int i = 0; i < pSelector.Length; i++)
                nativePSelector.value[i] = pSelector[i];

            if (tSelector.Length > 16)
                throw new ArgumentOutOfRangeException("tSelector", "maximum size (16) exceeded");

            NativeTSelector nativeTSelector;
            nativeTSelector.size = (byte)tSelector.Length;
            nativeTSelector.value = new byte[16];

            for (int i = 0; i < tSelector.Length; i++)
                nativeTSelector.value[i] = tSelector[i];

            if (sSelector.Length > 16)
                throw new ArgumentOutOfRangeException("sSelector", "maximum size (16) exceeded");

            NativeSSelector nativeSSelector;
            nativeSSelector.size = (byte)sSelector.Length;
            nativeSSelector.value = new byte[16];

            for (int i = 0; i < sSelector.Length; i++)
                nativeSSelector.value[i] = sSelector[i];

            Tase2_Endpoint_setLocalAddresses(self, nativePSelector, nativeSSelector, nativeTSelector);
        }

        /// <summary>
        /// Sets the local addresses for ISO layers (transport, session, presentation) from a string representations of the addresses
        /// </summary>
        /// The string is formated as a sequence of bytes represented as hexadecimal numbers (e.g. "00 00 01 1a")
        /// <param name='pSelector'>
        /// presentation layer address
        /// </param>
        /// <param name='sSelector'>
        /// session layer address
        /// </param>
        /// <param name='tSelector'>
        /// ISO COTP transport layer address
        /// </param>
        /// <exception cref="System.FormatException">When an element is not formated correctly</exception>
        /// <exception cref="System.OverflowException">When an element is out of range</exception>
        /// <exception cref="System.ArgumentOutOfRangeException">When the number of address elements is out of range</exception>
        public void SetRemoteAddresses(string pSelector, string sSelector, string tSelector)
        {
            string[] pSelParts = pSelector.Split(' ');

            byte[] psel = new byte[pSelParts.Length];

            for (int i = 0; i < pSelParts.Length; i++)
            {
                psel[i] = Convert.ToByte(pSelParts[i], 16);
            }

            string[] sSelParts = sSelector.Split(' ');

            byte[] ssel = new byte[sSelParts.Length];

            for (int i = 0; i < sSelParts.Length; i++)
            {
                ssel[i] = Convert.ToByte(sSelParts[i], 16);
            }

            string[] tSelParts = tSelector.Split(' ');

            byte[] tsel = new byte[tSelParts.Length];

            for (int i = 0; i < tSelParts.Length; i++)
            {
                tsel[i] = Convert.ToByte(tSelParts[i], 16);
            }

            SetRemoteAddresses(psel, ssel, tsel);
        }

        /// <summary>
        /// Sets the remote addresses for ISO layers (transport, session, presentation)
        /// </summary>
        /// <param name='pSelector'>
        /// presentation layer address
        /// </param>
        /// <param name='sSelector'>
        /// session layer address
        /// </param>
        /// <param name='tSelector'>
        /// ISO COTP transport layer address
        /// </param>
        /// <exception cref="System.ArgumentOutOfRangeException">When the number of address elements is out of range</exception>
        public void SetRemoteAddresses(byte[] pSelector, byte[] sSelector, byte[] tSelector)
        {
            if (pSelector.Length > 16)
                throw new ArgumentOutOfRangeException("pSelector", "maximum size (16) exceeded");

            NativePSelector nativePSelector;
            nativePSelector.size = (byte)pSelector.Length;
            nativePSelector.value = new byte[16];

            for (int i = 0; i < pSelector.Length; i++)
                nativePSelector.value[i] = pSelector[i];

            if (tSelector.Length > 16)
                throw new ArgumentOutOfRangeException("tSelector", "maximum size (16) exceeded");

            NativeTSelector nativeTSelector;
            nativeTSelector.size = (byte)tSelector.Length;
            nativeTSelector.value = new byte[16];

            for (int i = 0; i < tSelector.Length; i++)
                nativeTSelector.value[i] = tSelector[i];

            if (sSelector.Length > 16)
                throw new ArgumentOutOfRangeException("sSelector", "maximum size (16) exceeded");

            NativeSSelector nativeSSelector;
            nativeSSelector.size = (byte)sSelector.Length;
            nativeSSelector.value = new byte[16];

            for (int i = 0; i < sSelector.Length; i++)
                nativeSSelector.value[i] = sSelector[i];

            Tase2_Endpoint_setRemoteAddresses(self, nativePSelector, nativeSSelector, nativeTSelector);
        }

        /// <summary>
        /// Sets the remote IP address (only for active mode)
        /// </summary>
        /// <param name="ipAddress">remote IP address.</param>
        public void SetRemoteIpAddress(string ipAddress)
        {
            Tase2_Endpoint_setRemoteIpAddress(self, ipAddress);
        }

        /// <summary>
        /// Sets the remote TCP port (only for active mode)
        /// </summary>
        /// <param name="tcpPort">remote TCP port to connect to.</param>
        public void SetRemoteTcpPort(int tcpPort)
        {
            Tase2_Endpoint_setRemoteTcpPort(self, tcpPort);
        }

        /// <summary>
        /// Sets the maximum MMS PDU size used by the endpoint
        /// </summary>
        /// <remarks>
        /// NOTE: Actual values are negotiated with the peer and are equal or below this value.
        /// </remarks>
        /// <param name="maxPduSize">the maximum PDU size in bytes</param>
        public void SetMaxPduSize(int maxPduSize)
        {
            if (maxPduSize < 100)
                throw new ArgumentOutOfRangeException("Value too small");

            Tase2_Endpoint_setMaxPduSize(self, maxPduSize);
        }

        /// <summary>
        /// Set the maximum number of outstanding service calls (when calling)
        /// </summary>
        /// <param name="value">maximum allowed number of outstanding service calls</param>
        public void SetMaxServOutstandingCalling(int value)
        {
            if (value < 1)
                throw new ArgumentOutOfRangeException("Value larger than 0 required");

            Tase2_Endpoint_setMaxServOutstandingCalling(self, value);
        }

        /// <summary>
        /// Set the maximum number of outstanding service calls (when called)
        /// </summary>
        /// <param name="value">maximum allowed number of outstanding service calls</param>
        public void SetMaxServOutstandingCalled(int value)
        {
            if (value < 1)
                throw new ArgumentOutOfRangeException("Value larger than 0 required");

            Tase2_Endpoint_setMaxServOutstandingCalled(self, value);
        }

        /// <summary>
        ///  Sets the timeout for confirmed request messages
        /// </summary>
        /// <remarks>Default value is 5000 ms</remarks>
        /// <param name="timeoutInMs">Timeout in milliseconds</param>
        public void SetRequestTimeout(int timeoutInMs)
        {
            if (timeoutInMs < 0)
                throw new ArgumentOutOfRangeException("Only positive value allowed");

            Tase2_Endpoint_setRequestTimeout(self, timeoutInMs);
        }

        /// <summary>
        /// Sets the connect timeout of active endpoints
        /// </summary>
        /// <remarks>Default value is 10000 ms</remarks>
        /// <param name="timeoutInMs">Timeout in milliseconds</param>
        public void SetConnectTimeout(int timeoutInMs)
        {
            if (timeoutInMs < 0)
                throw new ArgumentOutOfRangeException("Only positive value allowed");

            Tase2_Endpoint_setConnectTimeout(self, timeoutInMs);
        }

        /// <summary>
        /// Connect this instance to a remote TCP server or listen to a local TCP port.
        /// </summary>
        public void Connect()
        {
            Tase2_Endpoint_connect(self);
        }

        /// <summary>
        /// Close all connections and stop listening to local TCP port.
        /// </summary>
        public void Disconnect()
        {
            Tase2_Endpoint_disconnect(self, 0);
        }

        /// <summary>
        /// Disconnect a specific connection identified by the IP address and optionally by the port number
        /// </summary>
        /// <param name="peerAddress">peer IP address (e.g. "10.0.0.10") with optional port number (e.g. "10.0.0.10:34237".</param>
        public void DisconnectPeer(string peerAddress)
        {
            Tase2_Endpoint_disconnectPeer(self, peerAddress);
        }

        /// <summary>
        /// Determines whether this instance is connected (to at least one TCP client or remote TCP server)
        /// </summary>
        /// <returns><c>true</c> if this instance is connected; otherwise, <c>false</c>.</returns>
        public bool IsConnected()
        {
            return Tase2_Endpoint_isConnected(self);
        }

        /// <summary>
        /// Gets the number of connected peers.
        /// </summary>
        /// <value>The number of connected peers.</value>
        public int ConnectedPeers
        {
            get
            {
                return Tase2_Endpoint_connectedPeers(self);
            }
        }

        /// <summary>
        /// Waits unitl new state is entered or a timeout or error occured.
        /// </summary>
        /// <returns><c>true</c>, if for state was entered, <c>false</c> otherwise.</returns>
        /// <param name="state">expected state</param>
        /// <param name="timeoutInMs">Timeout in ms.</param>
        public bool WaitForState(EndpointState state, int timeoutInMs)
        {
            return Tase2_Endpoint_waitForState(self, (int)state, timeoutInMs);
        }

        /// <summary>
        /// Gets the endpoint state
        /// </summary>
        /// <value>The endpoint state.</value>
        public EndpointState State
        {
            get
            {
                return (EndpointState)Tase2_Endpoint_getState(self);
            }
        }

        /// <summary>
        /// Add a server to an endpoint
        /// </summary>
        /// <param name="server">the server to be added</param>
        public void AddServer(Server.Server server)
        {
            Tase2_Endpoint_addServer(server.self, self);
        }

        /// <summary>
        /// Remove a server from the endpoint
        /// </summary>
        /// <returns>removed server, or null when the server was not associated with this endpoint</returns>
        /// <param name="server">the server to be removed</param>
        public Server.Server RemoveServer(Server.Server server)
        {
            IntPtr retVal = Tase2_Endpoint_removeServer(self, server.self);

            if (retVal == IntPtr.Zero)
                return null;
            else
                return server;
        }

        /// <summary>
        /// Remove a client from the endpoint
        /// </summary>
        /// <returns>removed client, or null when the client was not associated with this endpoint</returns>
        /// <param name="client">the client to be removed</param>
        public Client.Client RemoveClient(Client.Client client)
        {
            IntPtr retVal = Tase2_Endpoint_removeClient(self, client.self);

            if (retVal == IntPtr.Zero)
                return null;
            else
                return client;
        }
    }
}

