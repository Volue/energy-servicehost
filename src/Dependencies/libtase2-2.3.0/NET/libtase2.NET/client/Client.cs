using System;
using TASE2.Library.Common;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Text;

namespace TASE2.Library.Client
{
    /// <summary>
    /// Client error.
    /// </summary>
    public enum ClientError
    {
        /// <summary>
        /// No error occured (request has been successful)
        /// </summary>
        OK = 0,

        /// <summary>
        /// Request failed
        /// </summary>
        FAILED = 1,

        /// <summary>
        /// Request timeout
        /// </summary>
        TIMEOUT = 2,

        /// <summary>
        /// Access denied by server
        /// </summary>
        ACCESS_DENIED = 3,

        /// <summary>
        /// Server resource is temporarily unavailable
        /// </summary>
        TEMPORARILY_UNAVAILABLE = 4,

        /// <summary>
        /// invalid argument
        /// </summary>
        INVALID_ARGUMENT = 5,

        /// <summary>
        /// server return value invalid
        /// </summary>
        INVALID_RETURN_VALUE = 6,

        /// <summary>
        /// other error
        /// </summary>
        OTHER = 7,

        /// <summary>
        /// object does not exist
        /// </summary>
        OBJECT_NOT_EXISTING = 8,

        /// <summary>
        /// object already exists
        /// </summary>
        OBJECT_EXISTS = 9
    }

    /// <summary>
    /// Client connection state.
    /// </summary>
    public enum ClientState
    {
        /// <summary>
        /// Client is idle (not yet connected)
        /// </summary>
        STATE_IDLE = 0,

        /// <summary>
        /// Client is connected to server
        /// </summary>
        STATE_CONNECTED,

        /// <summary>
        /// Client is closed (has been connected before)
        /// </summary>
        STATE_CLOSED
    }

    /// <summary>
    /// Handler that is called for each value received with a DS transfer set report
    /// </summary>
    public delegate void DSTransferSetValueHandler(object parameter, ClientDSTransferSet transferSet, string domainName, string pointName, PointValue pointValue);

    /// <summary>
    /// Handler that is called for each received DS transfer set report
    /// </summary>
	public delegate void DSTransferSetReportHandler(object parameter, bool finished, UInt32 seqNo, ClientDSTransferSet transferSet);

    /// <summary>
    /// Handler that is called when the server closes the connection
    /// </summary>
	public delegate void ConnectionClosedHandler(object parameter, Client connection);

    /// <summary>
    /// Handler that is called when an information message (from IM transfer set) is received.
    /// </summary>
	public delegate void InformationMessageHandler(object parameter, int infoRef, int localRef, int msgId, byte[] message);

    /// <summary>
    /// Handler that is called when the client state changed (e.g. from CONNECTED to CLOSED)
    /// </summary>
    public delegate void ClientStateChangedHandler(object parameter, Client client, ClientState newState);

    /// <summary>
    /// Data set entry specification
    /// </summary>
    public class DataSetEntrySpec
    {
        private string domainName;
        private string variableName;

        internal DataSetEntrySpec(string domainName, string variableName)
        {
            this.domainName = domainName;
            this.variableName = variableName;
        }

        /// <summary>
        /// Gets the domain name of the data set entry
        /// </summary>
        /// <value>The domain name or null for VCC scope variables</value>
        public string DomainId
        {
            get
            {
                return domainName;
            }
        }

        /// <summary>
        /// Gets the variable name of the data set entry
        /// </summary>
        /// <value>The variable name</value>
        public string ItemId
        {
            get
            {
                return variableName;
            }
        }
    }

    /// <summary>
    /// Client API entrance point. Represents a connection to a server.
    /// </summary>
    public class Client : IDisposable
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_create(IntPtr tlsConfig);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_createEx(IntPtr endpoint);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_destroy(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getEndpoint(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_setTcpPort(IntPtr self, int tcpPort);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Client_connect(IntPtr self, string hostname, string apTitle, int aeQualifier);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_disconnect(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_setIdentity(IntPtr self, string vendor, string model, string revision);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_readPeerIdentity(IntPtr self, out int error, out IntPtr vendor, out IntPtr model, out IntPtr revision);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Client_getState(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getBilateralTableId(IntPtr self, out int err, string domainName);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Client_getServerVersionMajor(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Client_getServerVersionMinor(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Client_getSupportedFeatures(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_readPointValue(IntPtr self, out int err, string domainName, string pointName);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_readMultiplePointValues(IntPtr self, out int err, string domainName, IntPtr pointNames);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_Client_setTag(IntPtr self, out int err, string domainName, string pointName, int tagValue, string reason);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Client_getTag(IntPtr self, out int err, string domainName, string pointName, [MarshalAs(UnmanagedType.I1)] out bool isArmed, StringBuilder reasonBuf, int reasonBufSize);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getVCCVariables(IntPtr self, out int err);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getDomainNames(IntPtr self, out int err);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getDomainVariables(IntPtr self, string domainName, out int err);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getVCCDataSets(IntPtr self, out int err);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getDomainDataSets(IntPtr self, string domainName, out int err);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getDataSetDirectory(IntPtr self, string domainName, string dataSetName, out int err);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern int Tase2_Client_selectDevice(IntPtr self, out int err, string domainName, string deviceName);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_Client_sendCommand(IntPtr self, out int err, string domainName, string deviceName, Int16 commandValue);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_Client_sendDiscreteSetPoint(IntPtr self, out int err, string domainName, string deviceName, Int16 setPointValue);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_Client_sendRealSetPoint(IntPtr self, out int err, string domainName, string deviceName, float setPointValue);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_writePointValue(IntPtr self, out int err, string domainName, string pointName, IntPtr value);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getDataSet(IntPtr self, out int err, string domainName, string dataSetName);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_Client_createDataSet(IntPtr self, out int err, string domainName, string dataSetName, IntPtr dataSetElements);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool Tase2_Client_deleteDataSet(IntPtr self, out int err, string domainName, string dataSetName);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_Client_getNextDSTransferSet(IntPtr self, string domainName, out int err);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void InternalDSTransferSetValueHandler(IntPtr parameter, IntPtr transferSet, string domainName, string pointName, IntPtr pointValue);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_installDSTransferSetValueHandler(IntPtr self, InternalDSTransferSetValueHandler handler, IntPtr parameter);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void InternalDSTransferSetReportHandler(IntPtr parameter, [MarshalAs(UnmanagedType.I1)] bool finished, UInt32 seqNo, IntPtr transferSet);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_installDSTransferSetReportHandler(IntPtr self, InternalDSTransferSetReportHandler handler, IntPtr parameter);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern Statistics Tase2_Client_getStatistics(IntPtr self);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void InternalConnectionClosedHandler(IntPtr parameter, IntPtr connection);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_installConnectionClosedHandler(IntPtr self, InternalConnectionClosedHandler handler, IntPtr parameter);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void Tase2_Client_StateChangedHandler(IntPtr parameter, IntPtr client, int newState);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_installStateChangedHandler(IntPtr self, Tase2_Client_StateChangedHandler handler, IntPtr parameter);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void InternalInformationMessageHandler(IntPtr parameter, Int32 infoRef, Int32 localRef, Int32 msgId, Int32 size, IntPtr messageBuffer);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_installIMTransferSetHandler(IntPtr self, InternalInformationMessageHandler handler, IntPtr parameter);

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
        private static extern void Tase2_Client_setRemoteApTitle(IntPtr self, string apTitle, int aeQualifier);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_setLocalApTitle(IntPtr self, string apTitle, int aeQualifier);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_setLocalApInvocationId(IntPtr self, int apInvocationId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_setLocalAeInvocationId(IntPtr self, int aeInvocationId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_setRemoteAddresses(IntPtr self, NativePSelector pSelector, NativeSSelector sSelector, NativeTSelector tSelector);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_setLocalAddresses(IntPtr self, NativePSelector pSelector, NativeSSelector sSelector, NativeTSelector tSelector);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_setRemoteApInvocationId(IntPtr self, int apInvocationId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_setRemoteAeInvocationId(IntPtr self, int aeInvocationId);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_IMTransferSetEnable(IntPtr self, out int err);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_Client_IMTransferSetDisable(IntPtr self, out int err);

        /****************
    	 * LinkedList
     	 ***************/
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr LinkedList_getNext(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr LinkedList_getData(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        static extern void LinkedList_destroy(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        static extern void LinkedList_destroyStatic(IntPtr self);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void LinkedListValueDeleteFunction(IntPtr pointer);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        static extern void LinkedList_destroyDeep(IntPtr list, LinkedListValueDeleteFunction valueDeleteFunction);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr LinkedList_create();

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        static extern void LinkedList_add(IntPtr self, IntPtr data);

        internal IntPtr self;
        internal Endpoint endpoint = null;

        /* table to lookup managed ClientDSTransferSet object from native instance */
        private Dictionary<IntPtr, ClientDSTransferSet> dsTransferSetInstances =
            new Dictionary<IntPtr, ClientDSTransferSet>();

        private TLSConfiguration tlsConfiguration = null;

        public Client(TLSConfiguration tlsConfig = null)
        {
            IntPtr tlsConfigPtr = IntPtr.Zero;

            this.tlsConfiguration = tlsConfig;

            if (tlsConfig != null)
                tlsConfigPtr = tlsConfig.Self;

            self = Tase2_Client_create(tlsConfigPtr);
        }

        public Client(Endpoint endpoint)
        {
            self = Tase2_Client_createEx(endpoint.self);
        }

        /// <summary>
        /// Releases all resource used by the <see cref="TASE2.Library.Client.Client"/> object.
        /// </summary>
        /// <remarks>Call <see cref="Dispose"/> when you are finished using the <see cref="TASE2.Library.Client.Client"/>. The
        /// <see cref="Dispose"/> method leaves the <see cref="TASE2.Library.Client.Client"/> in an unusable state. After
        /// calling <see cref="Dispose"/>, you must release all references to the <see cref="TASE2.Library.Client.Client"/> so
        /// the garbage collector can reclaim the memory that the <see cref="TASE2.Library.Client.Client"/> was occupying.</remarks>
        public void Dispose()
        {
            lock (this)
            {
                if (self != IntPtr.Zero)
                {
                    Tase2_Client_destroy(self);
                    self = IntPtr.Zero;
                }
            }
        }

        ~Client()
        {
            Dispose();
        }

        /// <summary>
        /// Gets the underlying endpoint instance.
        /// </summary>
        /// <value>Endpoint instance used by this server</value>
        public Endpoint Endpoint
        {
            get
            {
                if (this.endpoint == null)
                {
                    IntPtr endpointPtr = Tase2_Client_getEndpoint(self);

                    if (endpointPtr != IntPtr.Zero)
                    {
                        this.endpoint = Endpoint.LookupKnownEndpointOrCreate(endpointPtr);
                    }
                }

                return this.endpoint;
            }
        }

        /// <summary>
        /// Sets the identity of the client TASE.2 application
        /// </summary>
        /// <param name="vendor">application vendor name</param>
        /// <param name="model">application model name</param>
        /// <param name="revision">application revision string</param>
        public void SetIdentity(string vendor, string model, string revision)
        {
            Tase2_Client_setIdentity(self, vendor, model, revision);
        }

        /// <summary>
        /// Gets the identity of the peer TASE.2 application
        /// </summary>
        /// <param name="vendor">application vendor name</param>
        /// <param name="model">application model name</param>
        /// <param name="revision">application revision string</param>
        public void GetPeerIdentity(out string vendor, out string model, out string revision)
        {
            IntPtr vendorPtr = IntPtr.Zero;
            IntPtr modelPtr = IntPtr.Zero;
            IntPtr revisionPtr = IntPtr.Zero;

            vendor = null;
            model = null;
            revision = null;

            int errorInt;

            Tase2_Client_readPeerIdentity(self, out errorInt, out vendorPtr, out modelPtr, out revisionPtr);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
            {
                if (vendorPtr != IntPtr.Zero)
                {
                    vendor = Marshal.PtrToStringAnsi(vendorPtr);
                    Marshal.FreeHGlobal(vendorPtr);
                }

                if (modelPtr != IntPtr.Zero)
                {
                    model = Marshal.PtrToStringAnsi(modelPtr);
                    Marshal.FreeHGlobal(modelPtr);
                }

                if (revisionPtr != IntPtr.Zero)
                {
                    revision = Marshal.PtrToStringAnsi(revisionPtr);
                    Marshal.FreeHGlobal(revisionPtr);
                }

            }
            else
                throw new ClientException("get peer identity failed", clientError);
        }

        /// <summary>
        /// Sets the tcp port to connect to (default is 102 for MMS)
        /// </summary>
        /// <param name="tcpPort">The TCP port number</param>
        public void SetTcpPort(int tcpPort)
        {
            Tase2_Client_setTcpPort(self, tcpPort);
        }

        /// <summary>
        /// Sets the remote application endpoint related parameters
        /// </summary>
        /// <param name='apTitle'>
        /// remote AP title.
        /// </param>
        /// <param name='aeQualifier'>
        /// remote AE qualifier.
        /// </param>
        public void SetRemoteApTitle(string apTitle, int aeQualifier)
        {
            Tase2_Client_setRemoteApTitle(self, apTitle, aeQualifier);
        }

        /// <summary>
        /// Sets the remote AE invocation identifier.
        /// </summary>
        /// <param name="aeInvocationId">AE invocation identifier.</param>
        public void SetRemoteAeInvocationId(int aeInvocationId)
        {
            Tase2_Client_setRemoteAeInvocationId(self, aeInvocationId);
        }

        /// <summary>
        /// Sets the remote AP invocation identifier.
        /// </summary>
        /// <param name="apInvocationId">AP invocation identifier.</param>
        public void SetRemoteApInvocationId(int apInvocationId)
        {
            Tase2_Client_setRemoteApInvocationId(self, apInvocationId);
        }

        /// <summary>
        /// Sets the local application endpoint related parameters
        /// </summary>
        /// <param name='apTitle'>
        /// local AP title.
        /// </param>
        /// <param name='aeQualifier'>
        /// local AE qualifier.
        /// </param>
        public void SetLocalApTitle(string apTitle, int aeQualifier)
        {
            Tase2_Client_setLocalApTitle(self, apTitle, aeQualifier);
        }

        /// <summary>
        /// Sets the local AE invocation identifier.
        /// </summary>
        /// <param name="aeInvocationId">AE invocation identifier.</param>
        public void SetLocalAeInvocationId(int aeInvocationId)
        {
            Tase2_Client_setLocalAeInvocationId(self, aeInvocationId);
        }

        /// <summary>
        /// Sets the local AP invocation identifier.
        /// </summary>
        /// <param name="apInvocationId">AP invocation identifier.</param>
        public void SetLocalApInvocationId(int apInvocationId)
        {
            Tase2_Client_setLocalApInvocationId(self, apInvocationId);
        }

        /// <summary>
        /// Sets the remote addresses for ISO layers (transport, session, presentation) from a string representations of the addresses
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

            Tase2_Client_setRemoteAddresses(self, nativePSelector, nativeSSelector, nativeTSelector);
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

            Tase2_Client_setLocalAddresses(self, nativePSelector, nativeSSelector, nativeTSelector);
        }

        /// <summary>
        /// Connect to the server with the specified hostname/IP address, with given apTitle and aeQualifier.
        /// </summary>
        /// <param name="hostname">Hostname or IP address of the server</param>
        /// <param name="apTitle">apTitle to identify the client</param>
        /// <param name="aeQualifier">aeQualifier to identify the client</param>
        public void Connect(string hostname, string apTitle, int aeQualifier)
        {
            int errorInt = Tase2_Client_connect(self, hostname, apTitle, aeQualifier);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("Failed to connect", clientError);
        }

        public void Connect(string hostname)
        {
            Connect(hostname, null, 0);
        }

        /// <summary>
        /// Gets the connection state.
        /// </summary>
        /// <returns>The connection state.</returns>
        public ClientState GetState()
        {
            int stateVal = Tase2_Client_getState(self);

            return (ClientState)stateVal;
        }

        private InternalDSTransferSetValueHandler tsHandlerRef = null;
        private DSTransferSetValueHandler tsHandler = null;
        private object tsHandlerParameter = null;

        private InternalDSTransferSetReportHandler tsReportHandlerRef = null;
        private DSTransferSetReportHandler tsReportHandler = null;
        private object tsReportHandlerParameter = null;

        private void internalTsHandler(IntPtr parameter, IntPtr transferSetRef, string domainName, string pointName, IntPtr pointValue)
        {
            ClientDSTransferSet transferSet;

            if (transferSetRef == IntPtr.Zero)
                transferSet = null;
            else
                transferSet = GetDSTransferSetInstance(transferSetRef);

            if (tsHandler != null)
                tsHandler(tsHandlerParameter, transferSet, domainName, pointName, new PointValue(pointValue, true));
        }

        /// <summary>
        /// Sets the callback handler for received DSTransferSet report data point values
        /// </summary>
        /// <param name="handler">callback handler function (delegate)</param>
        /// <param name="parameter">user provided context parameter</param>
        public void SetDSTransferSetValueHandler(DSTransferSetValueHandler handler, object parameter)
        {
            if (tsHandlerRef == null)
                tsHandlerRef = new InternalDSTransferSetValueHandler(internalTsHandler);

            Tase2_Client_installDSTransferSetValueHandler(self, tsHandlerRef, IntPtr.Zero);

            this.tsHandler = handler;
            this.tsHandlerParameter = parameter;
        }

        private void internalTsReportHandler(IntPtr parameter, bool finished, UInt32 seqNo, IntPtr transferSetRef)
        {
            ClientDSTransferSet transferSet;

            if (transferSetRef == IntPtr.Zero)
                transferSet = null;
            else
                transferSet = GetDSTransferSetInstance(transferSetRef);

            if (tsReportHandler != null)
                tsReportHandler(tsReportHandlerParameter, finished, seqNo, transferSet);
        }

        /// <summary>
        /// Sets the callback handler for received DSTransferSet reports
        /// </summary>
        /// <param name="handler">callback handler function (delegate)</param>
        /// <param name="parameter">user provided context parameter</param>
        public void SetDSTransferSetReportHandler(DSTransferSetReportHandler handler, object parameter)
        {
            if (tsReportHandlerRef == null)
                tsReportHandlerRef = new InternalDSTransferSetReportHandler(internalTsReportHandler);

            Tase2_Client_installDSTransferSetReportHandler(self, tsReportHandlerRef, IntPtr.Zero);

            this.tsReportHandler = handler;
            this.tsReportHandlerParameter = parameter;
        }

        private InternalConnectionClosedHandler connectionClosedHandlerRef = null;
        private ConnectionClosedHandler connectionClosedHandler = null;
        private object connectionClosedHandlerParameter = null;

        private void internalConnectionClosedHandler(IntPtr parameter, IntPtr connection)
        {
            if (connectionClosedHandler != null)
                connectionClosedHandler(connectionClosedHandlerParameter, this);
        }

        public void SetConnectionClosedHandler(ConnectionClosedHandler handler, object parameter)
        {
            if (connectionClosedHandlerRef == null)
                connectionClosedHandlerRef = new InternalConnectionClosedHandler(internalConnectionClosedHandler);

            Tase2_Client_installConnectionClosedHandler(self, connectionClosedHandlerRef, IntPtr.Zero);

            this.connectionClosedHandler = handler;
            this.connectionClosedHandlerParameter = parameter;
        }

        private ClientStateChangedHandler stateChangedHandler = null;
        private object stateChangedHandlerParameter = null;
        private Tase2_Client_StateChangedHandler stateChangedHandlerRef = null;

        private void internalStateChangedHandler(IntPtr parameter, IntPtr client, int newState)
        {
            if (stateChangedHandler != null)
                stateChangedHandler(stateChangedHandlerParameter, this, (ClientState)newState);
        }

        /// <summary>
        /// Sets the state changed handler (called when the client state changes between IDLE/CONNECTED/CLOSED)
        /// </summary>
        /// <param name="handler">callback handler function (delegate)</param>
        /// <param name="parameter">user provided context parameter.</param>
        public void SetStateChangedHandler(ClientStateChangedHandler handler, object parameter)
        {
            if (stateChangedHandlerRef == null)
                stateChangedHandlerRef = new Tase2_Client_StateChangedHandler(internalStateChangedHandler);

            Tase2_Client_installStateChangedHandler(self, stateChangedHandlerRef, IntPtr.Zero);

            stateChangedHandler = handler;
            stateChangedHandlerParameter = parameter;
        }


        private InternalInformationMessageHandler infoMessHandlerRef = null;
        private InformationMessageHandler informationMessageHandler;
        private object infoMessageHandlerParameter;

        private void internalInformationMessageHandler(IntPtr parameter, Int32 infoRef, Int32 localRef, Int32 msgId, Int32 size, IntPtr messageBuffer)
        {
            if (informationMessageHandler != null)
            {

                byte[] message = new byte[size];

                Marshal.Copy(messageBuffer, message, 0, size);

                informationMessageHandler(infoMessageHandlerParameter, infoRef, localRef, msgId, message);
            }
        }

        public void SetInformationMessageHandler(InformationMessageHandler handler, object parameter)
        {
            if (infoMessHandlerRef == null)
                infoMessHandlerRef = new InternalInformationMessageHandler(internalInformationMessageHandler);

            Tase2_Client_installIMTransferSetHandler(self, infoMessHandlerRef, IntPtr.Zero);

            this.informationMessageHandler = handler;
            this.infoMessageHandlerParameter = parameter;
        }


        /// <summary>
        /// Disconnect from the server
        /// </summary>
        public void Disconnect()
        {
            Tase2_Client_disconnect(self);
        }

        /// <summary>
        /// Gets the bilateral table ID used by the server for this client/domain
        /// </summary>
        /// <returns>The bilateral table ID</returns>
        /// <param name="domainName">Domain name of the client specific domain</param>
        public string GetBilateralTableId(string domainName)
        {
            int errorInt;

            IntPtr bltId = Tase2_Client_getBilateralTableId(self, out errorInt, domainName);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
            {
                if (bltId == IntPtr.Zero)
                    return null;
                else
                    return Marshal.PtrToStringAnsi(bltId);
            }
            else
                throw new ClientException("GetBilateralTableId failed", clientError);
        }

        /// <summary>
        /// Gets the TASE.2 specification major version number used by the server.
        /// </summary>
        /// <returns>The TASE.2 version major number</returns>
        public int GetServerVersionMajor()
        {
            return Tase2_Client_getServerVersionMajor(self);
        }

        /// <summary>
        /// Gets the TASE.2 specification minor version number used by the server.
        /// </summary>
        /// <returns>The TASE.2 version minor number</returns>
        public int GetServerVersionMinor()
        {
            return Tase2_Client_getServerVersionMinor(self);
        }

        /// <summary>
        /// Gets the supported features of the connected server
        /// </summary>
        /// <returns>The supported features.</returns>
        public SupportedFeatures GetSupportedFeatures()
        {
            int features = Tase2_Client_getSupportedFeatures(self);

            return (SupportedFeatures)features;
        }

        /// <summary>
        /// Gets the VCC scope variables (VCC scope variables are not part of a specific domain)
        /// </summary>
        /// <returns>The VCC scope variables at the server</returns>
        public List<string> GetVCCVariables()
        {
            int errorInt;

            IntPtr linkedList = Tase2_Client_getVCCVariables(self, out errorInt);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
            {

                List<string> newList = new List<string>();

                IntPtr element = LinkedList_getNext(linkedList);

                while (element != IntPtr.Zero)
                {
                    string ld = Marshal.PtrToStringAnsi(LinkedList_getData(element));

                    newList.Add(ld);

                    element = LinkedList_getNext(element);
                }

                LinkedList_destroy(linkedList);

                return newList;
            }
            else
                throw new ClientException("get VCC variables failed", clientError);
        }

        /// <summary>
        /// Read multiple data points from the server
        /// </summary>
        /// <returns>a list of point values</returns>
        /// <param name="domainName">the domain name of the data point or null if the data point has VCC scope</param>
        /// <param name="pointNames">a list of point names.</param>
        public List<PointValue> ReadMultiplePointValues(string domainName, List<string> pointNames)
        {
            List<PointValue> pointValues = null;

            IntPtr listPtr = LinkedList_create();

            foreach (string pointName in pointNames)
            {
                IntPtr handle = System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(pointName);

                LinkedList_add(listPtr, handle);
            }

            int errorInt;

            IntPtr valuesListPtr = Tase2_Client_readMultiplePointValues(self, out errorInt, domainName, listPtr);

            LinkedList_destroyDeep(listPtr, new LinkedListValueDeleteFunction(FreeHGlobaleDeleteFunction));

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
            {
                pointValues = new List<PointValue>();

                IntPtr element = LinkedList_getNext(valuesListPtr);

                while (element != IntPtr.Zero)
                {
                    IntPtr pointValPtr = LinkedList_getData(element);

                    PointValue pointValue = new PointValue(pointValPtr);

                    pointValues.Add(pointValue);

                    element = LinkedList_getNext(element);
                }

                LinkedList_destroyStatic(valuesListPtr);
            }
            else
                throw new ClientException("read multiple point values failed", clientError);

            return pointValues;
        }


        /// <summary>
        /// Gets the list of VCC scope data set names.
        /// </summary>
        /// <returns>The list of VCC scope data set names.</returns>
        public List<string> GetVCCDataSets()
        {
            int errorInt;

            IntPtr linkedList = Tase2_Client_getVCCDataSets(self, out errorInt);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
            {

                List<string> newList = new List<string>();

                IntPtr element = LinkedList_getNext(linkedList);

                while (element != IntPtr.Zero)
                {
                    string ld = Marshal.PtrToStringAnsi(LinkedList_getData(element));

                    newList.Add(ld);

                    element = LinkedList_getNext(element);
                }

                LinkedList_destroy(linkedList);

                return newList;
            }
            else
                throw new ClientException("get VCC data sets failed", clientError);
        }

        /// <summary>
        /// Gets the list of domain names available at the server.
        /// </summary>
        /// <returns>The list of domain names.</returns>
        public List<string> GetDomainNames()
        {
            int errorInt;

            IntPtr linkedList = Tase2_Client_getDomainNames(self, out errorInt);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
            {

                List<string> newList = new List<string>();

                IntPtr element = LinkedList_getNext(linkedList);

                while (element != IntPtr.Zero)
                {
                    string ld = Marshal.PtrToStringAnsi(LinkedList_getData(element));

                    newList.Add(ld);

                    element = LinkedList_getNext(element);
                }

                LinkedList_destroy(linkedList);

                return newList;
            }
            else
                throw new ClientException("get domain names failed", clientError);
        }

        /// <summary>
        /// Gets the list of domain specific variables in the given domain
        /// </summary>
        /// <returns>The domain variable names</returns>
        /// <param name="domainName">Domain name.</param>
        public List<string> GetDomainVariables(string domainName)
        {
            int errorInt;

            IntPtr linkedList = Tase2_Client_getDomainVariables(self, domainName, out errorInt);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
            {

                List<string> newList = new List<string>();

                IntPtr element = LinkedList_getNext(linkedList);

                while (element != IntPtr.Zero)
                {
                    string ld = Marshal.PtrToStringAnsi(LinkedList_getData(element));

                    newList.Add(ld);

                    element = LinkedList_getNext(element);
                }

                LinkedList_destroy(linkedList);

                return newList;
            }
            else
                throw new ClientException("get domain variables failed", clientError);
        }

        /// <summary>
        /// Gets the list of domain specific data sets.
        /// </summary>
        /// <returns>the domain data set names</returns>
        /// <param name="domainname">Domain name.</param>
        public List<string> GetDomainDataSets(string domainname)
        {
            int errorInt;

            IntPtr linkedList = Tase2_Client_getDomainDataSets(self, domainname, out errorInt);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
            {

                List<string> newList = new List<string>();

                IntPtr element = LinkedList_getNext(linkedList);

                while (element != IntPtr.Zero)
                {
                    string ld = Marshal.PtrToStringAnsi(LinkedList_getData(element));

                    newList.Add(ld);

                    element = LinkedList_getNext(element);
                }

                LinkedList_destroy(linkedList);

                return newList;
            }
            else
                throw new ClientException("get domain datasets failed", clientError);
        }

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_DataSetEntrySpec_getDomainId(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_DataSetEntrySpec_getItemId(IntPtr self);

        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Tase2_DataSetEntrySpec_destroy(IntPtr self);

        /// <summary>
        /// Gets the data set directory.
        /// </summary>
        /// <param name="domainname">Domainname.</param>
        /// <param name="datasetName">Dataset name.</param>
        /// <returns>List of data set entries</returns>
        public List<DataSetEntrySpec> GetDataSetDirectory(string domainname, string datasetName)
        {
            int errorInt;

            IntPtr linkedList = Tase2_Client_getDataSetDirectory(self, domainname, datasetName, out errorInt);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
            {

                List<DataSetEntrySpec> entries = new List<DataSetEntrySpec>();

                IntPtr element = LinkedList_getNext(linkedList);

                while (element != IntPtr.Zero)
                {

                    IntPtr entryData = LinkedList_getData(element);

                    IntPtr domainNamePtr = Tase2_DataSetEntrySpec_getDomainId(entryData);

                    IntPtr itemIdPtr = Tase2_DataSetEntrySpec_getItemId(entryData);

                    string domainName;

                    if (domainNamePtr == IntPtr.Zero)
                        domainName = null;
                    else
                    {
                        domainName = Marshal.PtrToStringAnsi(domainNamePtr);
                        if (domainName.Equals(""))
                            domainName = null;
                    }

                    string itemId;

                    if (itemIdPtr == IntPtr.Zero)
                        itemId = null;
                    else
                    {
                        itemId = Marshal.PtrToStringAnsi(itemIdPtr);
                        if (itemId.Equals(""))
                            itemId = null;
                    }

                    DataSetEntrySpec entry = new DataSetEntrySpec(domainName, itemId);

                    entries.Add(entry);

                    element = LinkedList_getNext(element);
                }

                LinkedList_destroyDeep(linkedList, Tase2_DataSetEntrySpec_destroy);


                return entries;
            }
            else
                throw new ClientException("get data set directory failed", clientError);
        }


        public void IMTransferSetEnable()
        {
            int errorInt;

            Tase2_Client_IMTransferSetEnable(self, out errorInt);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("Failed to enable IM transfer set", clientError);
        }

        public void IMTransferSetDisable()
        {
            int errorInt;

            Tase2_Client_IMTransferSetDisable(self, out errorInt);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("Failed to enable IM transfer set", clientError);

        }

        /// <summary>
        /// Reads the value of a data point
        /// </summary>
        /// <returns>The point value.</returns>
        /// <param name="domainName">Domain name.</param>
        /// <param name="pointName">Point name.</param>
        public PointValue ReadPointValue(string domainName, string pointName)
        {
            int errorInt;

            IntPtr pointValuePtr = Tase2_Client_readPointValue(self, out errorInt, domainName, pointName);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
                return new PointValue(pointValuePtr);
            else
                throw new ClientException("read point value failed", clientError);
        }

        /// <summary>
        /// Sets a tag for a device
        /// </summary>
        /// <param name="domainName">Domain name of the device</param>
        /// <param name="deviceName">Device name</param>
        /// <param name="tagValue">Tag value</param>
        /// <param name="reason">Reason string</param>
        public void SetTag(string domainName, string deviceName, TagValue tagValue, string reason)
        {
            int errorInt;

            bool result = Tase2_Client_setTag(self, out errorInt, domainName, deviceName, (int)tagValue, reason);

            if (result != true)
            {
                ClientError clientError = (ClientError)errorInt;

                throw new ClientException("set tag failed", clientError);
            }
        }

        /// <summary>
        /// Gets the tag value of a device
        /// </summary>
        /// <returns>The tag value</returns>
        /// <param name="domainName">Domain name of the device</param>
        /// <param name="deviceName">Device name</param>
        public TagValue GetTag(string domainName, string deviceName)
        {
            bool isArmed;

            return GetTag(domainName, deviceName, out isArmed);
        }

        /// <summary>
        /// Gets the tag value of a device
        /// </summary>
        /// <returns>The tag value</returns>
        /// <param name="domainName">Domain name of the device</param>
        /// <param name="deviceName">Device name</param>
        /// <param name="reason">Reason string for the tag returned by the server</param>
        public TagValue GetTag(string domainName, string deviceName, out string reason)
        {
            bool isArmed;

            return GetTag(domainName, deviceName, out isArmed, out reason);
        }

        /// <summary>
        /// Gets the tag value of a device
        /// </summary>
        /// <returns>The tag value</returns>
        /// <param name="domainName">Domain name of the device</param>
        /// <param name="deviceName">Device name</param>
        /// <param name="isArmed">Flag that informs that the device is armed</param>
        public TagValue GetTag(string domainName, string deviceName, out bool isArmed)
        {
            int errorInt;

            int result = Tase2_Client_getTag(self, out errorInt, domainName, deviceName, out isArmed, null, 0);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
                return (TagValue)result;
            else
                throw new ClientException("get tag failed", clientError);
        }

        /// <summary>
        /// Gets the tag value of a device
        /// </summary>
        /// <returns>The tag value</returns>
        /// <param name="domainName">Domain name of the device</param>
        /// <param name="deviceName">Device name</param>
        /// <param name="isArmed">Flag that informs that the device is armed</param>
        /// <param name="reason">Reason string for the tag returned by the server</param>
        public TagValue GetTag(string domainName, string deviceName, out bool isArmed, out string reason)
        {
            int errorInt;

            StringBuilder reasonBuffer = new StringBuilder(255);

            int result = Tase2_Client_getTag(self, out errorInt, domainName, deviceName, out isArmed, reasonBuffer, reasonBuffer.Capacity);

            reason = reasonBuffer.ToString();

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
                return (TagValue)result;
            else
                throw new ClientException("get tag failed", clientError);
        }

        /// <summary>
        /// Selects the device.
        /// </summary>
        /// <returns>The check back ID of the device</returns>
        /// <param name="domainName">Domain name of the device.</param>
        /// <param name="deviceName">Device name.</param>
        public int SelectDevice(string domainName, string deviceName)
        {
            int errorInt;

            int result = Tase2_Client_selectDevice(self, out errorInt, domainName, deviceName);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
                return result;
            else
                throw new ClientException("select device failed", clientError);
        }

        /// <summary>
        /// Sends a command to the device
        /// </summary>
        /// <param name="domainName">Domain name of the device.</param>
        /// <param name="deviceName">Device name.</param>
        /// <param name="commandValue">Command value.</param>
        public void SendCommand(string domainName, string deviceName, Int16 commandValue)
        {
            int errorInt;

            Tase2_Client_sendCommand(self, out errorInt, domainName, deviceName, commandValue);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("send command failed", clientError);
        }

        /// <summary>
        /// Sends the discrete set point.
        /// </summary>
        /// <returns><c>true</c>, if discrete set point was sent, <c>false</c> otherwise.</returns>
        /// <param name="domainName">Domain name of the device.</param>
        /// <param name="deviceName">Device name.</param>
        /// <param name="setPointValue">Set point value.</param>
        public bool SendDiscreteSetPoint(string domainName, string deviceName, Int16 setPointValue)
        {
            int errorInt;

            bool result = Tase2_Client_sendDiscreteSetPoint(self, out errorInt, domainName, deviceName, setPointValue);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
                return result;
            else
                throw new ClientException("send discrete set point failed", clientError);
        }

        public bool SendRealSetPoint(string domainName, string deviceName, float setPointValue)
        {
            int errorInt;

            bool result = Tase2_Client_sendRealSetPoint(self, out errorInt, domainName, deviceName, setPointValue);

            ClientError clientError = (ClientError)errorInt;

            if (clientError == ClientError.OK)
                return result;
            else
                throw new ClientException("send real set point failed", clientError);
        }

        public void WritePointValue(string domainName, string pointName, PointValue value)
        {
            int errorInt;

            Tase2_Client_writePointValue(self, out errorInt, domainName, pointName, value.Self);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("write point value failed", clientError);
        }

        /// <summary>
        /// Gets the data set. Create \ref ClientDataSet instance representing the data set.
        /// </summary>
        /// <returns>The data set.</returns>
        /// <param name="domainName">Domain name of the data set.</param>
        /// <param name="dataSetName">Data set name.</param>
        public ClientDataSet GetDataSet(string domainName, string dataSetName)
        {
            int errorInt;

            IntPtr dataSetRef = Tase2_Client_getDataSet(self, out errorInt, domainName, dataSetName);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("get data set failed", clientError);

            return new ClientDataSet(dataSetRef, this);
        }

        private void FreeHGlobaleDeleteFunction(IntPtr pointer)
        {
            Marshal.FreeHGlobal(pointer);
        }

        /// <summary>
        /// Create a new dynamic data set at the connected server.
        /// </summary>
        /// Data set elements can be VCC scope or domain scope variables.
        /// To define domain scope variable name has to be prefixed with the domain name.
        /// The domain name is separated ffrom the data set name with the "/" sign.
        /// E.g. "icc1/variable1"
        /// <returns><c>true</c>, if data set was created, <c>false</c> otherwise.</returns>
        /// <param name="domainName">Domain name of the data set or null for VCC scope data set</param>
        /// <param name="dataSetName">Data set name.</param>
        /// <param name="dataSetElements">list of data set elements.</param>
        public bool CreateDataSet(string domainName, string dataSetName, List<string> dataSetElements)
        {
            IntPtr linkedList = LinkedList_create();

            foreach (string dataSetElement in dataSetElements)
            {
                IntPtr handle = System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(dataSetElement);

                LinkedList_add(linkedList, handle);
            }

            int errorInt;

            bool result = Tase2_Client_createDataSet(self, out errorInt, domainName, dataSetName, linkedList);

            LinkedList_destroyDeep(linkedList, new LinkedListValueDeleteFunction(FreeHGlobaleDeleteFunction));

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("create data set failed", clientError);

            return result;
        }

        /// <summary>
        /// Delete a dynamic data set of the connected server
        /// </summary>
        /// <returns><c>true</c>, if data set was deleted, <c>false</c> otherwise.</returns>
        /// <param name="domainName">Domain name of the data set or null for VCC scope data set</param>
        /// <param name="dataSetName">Data set name.</param>
        public bool DeleteDataSet(string domainName, string dataSetName)
        {
            int errorInt;

            bool result = Tase2_Client_deleteDataSet(self, out errorInt, domainName, dataSetName);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("delete data set failed", clientError);

            return result;
        }


        internal ClientDSTransferSet GetDSTransferSetInstance(IntPtr ptr)
        {
            try
            {
                ClientDSTransferSet transferSet = dsTransferSetInstances[ptr];

                /* remove from list when disposed! */
                if (transferSet.IsDisposed())
                {
                    dsTransferSetInstances.Remove(ptr);
                    transferSet = null;
                }

                return transferSet;
            }
            catch (KeyNotFoundException)
            {
                return null;
            }
        }

        internal void RemoveDSTransferSetInstance(IntPtr ptr)
        {
            dsTransferSetInstances.Remove(ptr);
        }

        /// <summary>
        /// Gets the next available DS transfer set. This will reserve the DS transfer set for this client
        /// </summary>
        /// <returns>The next DS transfer set.</returns>
        /// <param name="domainName">Domain name</param>
        public ClientDSTransferSet GetNextDSTransferSet(string domainName)
        {
            int errorInt;

            IntPtr transferSetRef = Tase2_Client_getNextDSTransferSet(self, domainName, out errorInt);

            ClientError clientError = (ClientError)errorInt;

            if (clientError != ClientError.OK)
                throw new ClientException("get next transfer set failed", clientError);

            if (transferSetRef == IntPtr.Zero)
                return null;
            else
            {
                ClientDSTransferSet newTransferSet = new ClientDSTransferSet(transferSetRef, this);


                /* remove old reference if existing */
                try
                {
                    ClientDSTransferSet tsInstance = dsTransferSetInstances[transferSetRef];

                    if (tsInstance != null)
                        dsTransferSetInstances.Remove(transferSetRef);
                }
                catch (KeyNotFoundException)
                {
                }


                dsTransferSetInstances.Add(transferSetRef, newTransferSet);

                return newTransferSet;
            }
        }

        /// <summary>
        /// Gets some statistic information.
        /// </summary>
        /// <returns>statistics</returns>
        public Statistics GetStatistics()
        {
            return Tase2_Client_getStatistics(self);
        }
    }
}

