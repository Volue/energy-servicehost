package com.libtase2.client;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.libtase2.common.Endpoint;
import com.libtase2.common.PointValue;
import com.libtase2.common.TLSConfiguration;
import com.libtase2.common.Tag;
import com.libtase2.common.TagValue;
import com.sun.jna.Callback;
import com.sun.jna.CallbackThreadInitializer;
import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Memory;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;
import com.sun.jna.ptr.PointerByReference;

public class Client {

    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        public static class Tase2_TSelector extends Structure {

            public static class ByValue extends Tase2_TSelector implements Structure.ByValue {
            }

            public byte size;
            public byte[] value = new byte[16];

            @Override
            protected List<String> getFieldOrder() {
                List<String> fieldOrder = new LinkedList<String>();

                fieldOrder.add("size");
                fieldOrder.add("value");

                return fieldOrder;
            }

        }

        public static class Tase2_SSelector extends Structure {

            public static class ByValue extends Tase2_SSelector implements Structure.ByValue {
            }

            public byte size;
            public byte[] value = new byte[16];

            @Override
            protected List<String> getFieldOrder() {
                List<String> fieldOrder = new LinkedList<String>();

                fieldOrder.add("size");
                fieldOrder.add("value");

                return fieldOrder;
            }

        }

        public static class Tase2_PSelector extends Structure {

            public static class ByValue extends Tase2_PSelector implements Structure.ByValue {
            }

            public byte size;
            public byte[] value = new byte[16];

            @Override
            protected List<String> getFieldOrder() {
                List<String> fieldOrder = new LinkedList<String>();

                fieldOrder.add("size");
                fieldOrder.add("value");

                return fieldOrder;
            }

        }

        Pointer Tase2_Client_create(Pointer tlsConfiguration);

        Pointer Tase2_Client_createEx(Pointer endpoint);

        Pointer Tase2_Client_getEndpoint(Pointer self);

        int Tase2_Client_connect(Pointer self, String hostname, String apTitle, int aeQualifier);

        void Tase2_Client_disconnect(Pointer self);

        void Tase2_Client_destroy(Pointer self);

        int Tase2_Client_getState(Pointer self);

        int Tase2_Client_getServerVersionMajor(Pointer self);

        int Tase2_Client_getServerVersionMinor(Pointer self);

        int Tase2_Client_getSupportedFeatures(Pointer self);

        Pointer Tase2_Client_getVCCVariables(Pointer self, PointerByReference err);

        Pointer Tase2_Client_getVCCDataSets(Pointer self, PointerByReference err);

        Pointer Tase2_Client_getDomainNames(Pointer self, PointerByReference err);

        Pointer Tase2_Client_getDomainVariables(Pointer self, String domainName, PointerByReference err);

        Pointer Tase2_Client_getDomainDataSets(Pointer self, String domainName, PointerByReference err);

        int Tase2_Client_selectDevice(Pointer self, PointerByReference err, String domainName, String deviceName);

        byte Tase2_Client_setTag(Pointer self, PointerByReference err, String domainName, String deviceName,
                int tagValue, String reason);

        int Tase2_Client_getTag(Pointer self, PointerByReference err, String domainName, String deviceName,
                PointerByReference isArmed, Pointer reasonBuf, int reasonBufSize);

        byte Tase2_Client_sendCommand(Pointer self, PointerByReference err, String domainName, String deviceName,
                short commandValue);

        byte Tase2_Client_sendDiscreteSetPoint(Pointer self, PointerByReference err, String domainName,
                String deviceName, short setPointValue);

        byte Tase2_Client_sendRealSetPoint(Pointer self, PointerByReference err, String domainName, String deviceName,
                float setPointValue);

        Pointer Tase2_Client_readPointValue(Pointer self, PointerByReference err, String domainName, String pointName);

        Pointer Tase2_Client_readMultiplePointValues(Pointer self, PointerByReference err, String domainName,
                Pointer pointNames);

        Pointer Tase2_Client_getNextDSTransferSet(Pointer self, String domainName, PointerByReference err);

        interface Tase2_DSTransferSetValueHandler extends Callback {
            void callback(Pointer parameter, Pointer transferSet, String domainName, String pointName,
                    Pointer pointValue);
        }

        void Tase2_Client_installDSTransferSetValueHandler(Pointer self, Tase2_DSTransferSetValueHandler handler,
                Pointer parameter);

        interface Tase2_DSTransferSetReportHandler extends Callback {
            void invoke(Pointer parameter, byte finished, int seq, Pointer transferSet);
        }

        void Tase2_Client_installDSTransferSetReportHandler(Pointer self, Tase2_DSTransferSetReportHandler handler,
                Pointer parameter);

        interface Tase2_Client_StateChangedHandler extends Callback {
            void invoke(Pointer parameter, Pointer client, int newState);
        }

        void Tase2_Client_installStateChangedHandler(Pointer self, Tase2_Client_StateChangedHandler handler,
                Pointer parameter);

        interface Tase2_ConnectionClosedHandler extends Callback {
            void invoke(Pointer parameter, Pointer connection);
        }

        void Tase2_Client_installConnectionClosedHandler(Pointer self, Tase2_ConnectionClosedHandler handler,
                Pointer parameter);

        interface Tase2_InformationMessageHandler extends Callback {
            void invoke(Pointer parameter, int infoRef, int localRef, int msgId, int size, Pointer messageBuffer);
        }

        void Tase2_Client_installIMTransferSetHandler(Pointer self, Tase2_InformationMessageHandler handler,
                Pointer parameter);

        void Tase2_Client_IMTransferSetEnable(Pointer self, PointerByReference err);

        Pointer Tase2_Client_getDataSet(Pointer self, PointerByReference err, String domainName, String dataSetName);

        byte Tase2_Client_deleteDataSet(Pointer self, PointerByReference err, String domainName, String dataSetName);

        byte Tase2_Client_createDataSet(Pointer self, PointerByReference err, String domainName, String dataSetName,
                Pointer dataSetElements);

        Pointer LinkedList_create();

        void LinkedList_add(Pointer self, Pointer data);

        Pointer LinkedList_getNext(Pointer self);

        Pointer LinkedList_getData(Pointer self);

        void LinkedList_destroy(Pointer self);

        void LinkedList_destroyStatic(Pointer self);

        void Tase2_Client_setTcpPort(Pointer self, int tcpPort);

        void Tase2_Client_setRemoteApTitle(Pointer self, String apTitle, int aeQualifier);

        void Tase2_Client_setRemoteAddresses(Pointer self, Tase2_PSelector.ByValue pSelector,
                Tase2_SSelector.ByValue sSelector, Tase2_TSelector.ByValue tSelector);

        void Tase2_Client_setLocalAddresses(Pointer self, Tase2_PSelector.ByValue pSelector,
                Tase2_SSelector.ByValue sSelector, Tase2_TSelector.ByValue tSelector);

        void Tase2_Client_readPeerIdentity(Pointer self, PointerByReference err, PointerByReference vendor,
                PointerByReference model, PointerByReference revision);

        Pointer Tase2_Client_getBilateralTableId(Pointer self, PointerByReference err, String domainName);

        void Tase2_FreeMemory(Pointer ptr);
    }

    private static Map<String, ?> getOptions() {
        Map<String, Object> result = new HashMap<String, Object>();

        DefaultTypeMapper typeMapper = new DefaultTypeMapper();

        typeMapper.addTypeConverter(Boolean.class, new TypeConverter() {
            @Override
            public Object fromNative(Object nativeValue, FromNativeContext context) {
                return ((Byte) nativeValue) != 0;
            }

            @Override
            public Class<?> nativeType() {
                return byte.class;
            }

            @Override
            public Object toNative(Object value, ToNativeContext context) {
                if (value.getClass() == Boolean.class) {
                    value = (Boolean) value;
                }
                if ((Boolean) value) {
                    return (byte) 1;
                } else {
                    return 0;
                }
            }
        });

        result.put(Library.OPTION_TYPE_MAPPER, typeMapper);
        return result;
    }

    public Pointer self = Pointer.NULL;

    public Endpoint endpoint = null;

    TLSConfiguration tlsConfiguration = null;

    private List<ClientDSTransferSet> dsTransferSets = new LinkedList<ClientDSTransferSet>();

    private ConnectionClosedHandler connectionClosedHandler = null;
    private Object connectionClosedHandlerParameter = null;

    private StateChangedHandler stateChangedHandler = null;
    private Object stateChangedHandlerParameter = null;

    NativeLibTase2.Tase2_InformationMessageHandler infoMessHandlerNative = null;
    private InformationMessageHandler infoMessHandler = null;
    private Object infoMessHandlerParameter = null;

    private DSTransferSetHandler dstsHandler = null;
    private Object dstsHandlerParameter = null;
    NativeLibTase2.Tase2_DSTransferSetReportHandler dstsReportHandlerNative = null;
    NativeLibTase2.Tase2_DSTransferSetValueHandler dstsValueHandlerNative = null;
    NativeLibTase2.Tase2_Client_StateChangedHandler scHandler = null;

    private CallbackThreadInitializer cbThreadInitializer = new CallbackThreadInitializer();

    private ClientDSTransferSet lookupKnownDSTransferSet(Pointer ref) {
        ClientDSTransferSet clientDsts = null;

        synchronized (dsTransferSets) {
            for (ClientDSTransferSet dsts : dsTransferSets) {
                if (dsts.self.equals(ref))
                    clientDsts = dsts;
            }
        }

        return clientDsts;
    }

    private static long getUnsignedInt(int x) {
        return x & 0x00000000ffffffffL;
    }

    private void initializeClientInstance() {
        final Client _client = this;

        NativeLibTase2.Tase2_ConnectionClosedHandler fn = new NativeLibTase2.Tase2_ConnectionClosedHandler() {

            @Override
            public void invoke(Pointer parameter, Pointer connection) {
                if (connectionClosedHandler != null)
                    connectionClosedHandler.connectionClosed(connectionClosedHandlerParameter, _client);
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Client_installConnectionClosedHandler(self, fn, Pointer.NULL);

        dstsReportHandlerNative = new NativeLibTase2.Tase2_DSTransferSetReportHandler() {

            @Override
            public void invoke(Pointer parameter, byte finished, int seq, Pointer transferSet) {
                if (dstsHandler != null) {
                    /* lookup known transfer set instance */
                    ClientDSTransferSet clientDsts = lookupKnownDSTransferSet(transferSet);

                    long longSeq = getUnsignedInt(seq);

                    if (finished != 0)
                        dstsHandler.finishedReport(clientDsts, longSeq, dstsHandlerParameter);
                    else
                        dstsHandler.startReport(clientDsts, longSeq, dstsHandlerParameter);
                }

            }
        };

        NativeLibTase2.INSTANCE.Tase2_Client_installDSTransferSetReportHandler(self, dstsReportHandlerNative,
                Pointer.NULL);

        dstsValueHandlerNative = new NativeLibTase2.Tase2_DSTransferSetValueHandler() {

            @Override
            public void callback(Pointer parameter, Pointer transferSet, String domainName, String pointName,
                    Pointer pointValue) {
                if (dstsHandler != null) {
                    /* lookup known transfer set instance */
                    ClientDSTransferSet clientDsts = lookupKnownDSTransferSet(transferSet);

                    dstsHandler.valueReceived(clientDsts, domainName, pointName, new PointValue(pointValue, true));
                } else {
                    System.err.println("No DSTS value handler found!");
                }
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Client_installDSTransferSetValueHandler(self, dstsValueHandlerNative,
                Pointer.NULL);

        scHandler = new NativeLibTase2.Tase2_Client_StateChangedHandler() {

            @Override
            public void invoke(Pointer parameter, Pointer client, int newState) {
                ClientState state = ClientState.fromValue(newState);

                if (stateChangedHandler != null) {
                    stateChangedHandler.stateChanged(stateChangedHandlerParameter, _client, state);
                }

                if (state != ClientState.CONNECTED) {
                    synchronized (dsTransferSets) {
                        dsTransferSets.clear();
                    }
                }
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Client_installStateChangedHandler(self, scHandler, Pointer.NULL);

        Native.setCallbackThreadInitializer(dstsValueHandlerNative, cbThreadInitializer);
        Native.setCallbackThreadInitializer(dstsReportHandlerNative, cbThreadInitializer);
        Native.setCallbackThreadInitializer(scHandler, cbThreadInitializer);
    }

    /**
     * Create a new TASE.2 client instance
     *
     * NOTE: The function will also create a new {@link com.libtase2.common.Endpoint
     * Endpoint} instance
     */
    public Client() {
        self = NativeLibTase2.INSTANCE.Tase2_Client_create(Pointer.NULL);

        initializeClientInstance();
    }

    /**
     * Create a new TASE.2 client instance using the provided
     * {@link com.libtase2.common.TLSConfiguration TLSConfiguration}.
     *
     * NOTE: The function will also create a new {@link com.libtase2.common.Endpoint
     * Endpoint} instance
     * 
     * @param tlsConfiguration TLS configuration to be used.
     */
    public Client(TLSConfiguration tlsConfiguration) {
        self = NativeLibTase2.INSTANCE.Tase2_Client_create(tlsConfiguration.self);

        this.tlsConfiguration = tlsConfiguration;

        initializeClientInstance();
    }

    /**
     * Create a new Tase2_Client instance using an existing endpoint
     */
    public Client(Endpoint endpoint) {
        self = NativeLibTase2.INSTANCE.Tase2_Client_createEx(endpoint.self);

        this.endpoint = endpoint;

        initializeClientInstance();
    }

    private static class InternalEndpoint extends Endpoint {
        protected static Endpoint getManagedInstance(Pointer ptr) {
            return Endpoint.getManagedInstance(ptr);
        }

        protected InternalEndpoint(Pointer nativeInstance) {
            super(nativeInstance);
        }
    }

    /**
     * Get the endpoint instance this client is assigned to
     */
    public Endpoint getEndpoint() {
        if (this.endpoint != null)
            return this.endpoint;

        Pointer endpointPtr = NativeLibTase2.INSTANCE.Tase2_Client_getEndpoint(self);

        this.endpoint = InternalEndpoint.getManagedInstance(endpointPtr);

        return this.endpoint;
    }

    /**
     * Change default TCP port for this client
     * 
     * @param tcpPort the TCP port number to use
     */
    public void setTcpPort(int tcpPort) {
        NativeLibTase2.INSTANCE.Tase2_Client_setTcpPort(self, tcpPort);
    }

    /**
     * Set application endpoint identifier of remote endpoint
     *
     * NOTE: This is an optional parameter that might be required by some server
     *
     * @param apTitle     AP-title part of the remote application endpoint
     *                    identifier
     * @param aeQualifier AE-qualifier part of the remote application endpoint
     *                    identifier
     */
    public void setRemoteApTitle(String apTitle, int aeQualifier) {
        NativeLibTase2.INSTANCE.Tase2_Client_setRemoteApTitle(self, apTitle, aeQualifier);
    }

    /**
     * Sets the remote addresses for ISO layers (transport, session, presentation)
     *
     * @param pSelector presentation layer address
     * @param sSelector session layer address
     * @param tSelector ISO COTP layer address
     */
    public void setRemoteAddresses(byte[] pSelector, byte[] sSelector, byte[] tSelector) {
        if (pSelector.length > 16)
            throw new IllegalArgumentException("p-selector too long (max 16 bytes)!");

        if (sSelector.length > 16)
            throw new IllegalArgumentException("s-selector too long (max 16 bytes)!");

        if (tSelector.length > 4)
            throw new IllegalArgumentException("s-selector too long (max 4 bytes)!");

        NativeLibTase2.Tase2_PSelector.ByValue pSel = new NativeLibTase2.Tase2_PSelector.ByValue();
        pSel.size = (byte) pSelector.length;
        for (int i = 0; i < pSel.size; i++)
            pSel.value[i] = pSelector[i];

        NativeLibTase2.Tase2_SSelector.ByValue sSel = new NativeLibTase2.Tase2_SSelector.ByValue();
        sSel.size = (byte) sSelector.length;
        for (int i = 0; i < sSel.size; i++)
            sSel.value[i] = sSelector[i];

        NativeLibTase2.Tase2_TSelector.ByValue tSel = new NativeLibTase2.Tase2_TSelector.ByValue();
        tSel.size = (byte) tSelector.length;
        for (int i = 0; i < tSel.size; i++)
            tSel.value[i] = tSelector[i];

        NativeLibTase2.INSTANCE.Tase2_Client_setRemoteAddresses(self, pSel, sSel, tSel);
    }

    /**
     * Sets the local addresses for ISO layers (transport, session, presentation)
     *
     * @param pSelector presentation layer address
     * @param sSelector session layer address
     * @param tSelector ISO COTP layer address
     */
    public void setLocalAddresses(byte[] pSelector, byte[] sSelector, byte[] tSelector) {
        if (pSelector.length > 16)
            throw new IllegalArgumentException("p-selector too long (max 16 bytes)!");

        if (sSelector.length > 16)
            throw new IllegalArgumentException("s-selector too long (max 16 bytes)!");

        if (tSelector.length > 4)
            throw new IllegalArgumentException("s-selector too long (max 4 bytes)!");

        NativeLibTase2.Tase2_PSelector.ByValue pSel = new NativeLibTase2.Tase2_PSelector.ByValue();
        pSel.size = (byte) pSelector.length;
        for (int i = 0; i < pSel.size; i++)
            pSel.value[i] = pSelector[i];

        NativeLibTase2.Tase2_SSelector.ByValue sSel = new NativeLibTase2.Tase2_SSelector.ByValue();
        sSel.size = (byte) sSelector.length;
        for (int i = 0; i < sSel.size; i++)
            sSel.value[i] = sSelector[i];

        NativeLibTase2.Tase2_TSelector.ByValue tSel = new NativeLibTase2.Tase2_TSelector.ByValue();
        tSel.size = (byte) tSelector.length;
        for (int i = 0; i < tSel.size; i++)
            tSel.value[i] = tSelector[i];

        NativeLibTase2.INSTANCE.Tase2_Client_setLocalAddresses(self, pSel, sSel, tSel);
    }

    /**
     * Connect to server and collect basic server information
     *
     * @param hostname    hostname or IP address of the server
     * @param apTitle     local (own) AP-title part of the application endpoint
     *                    identifier
     * @param aeQualifier local (own) AE-qualifier part of the application endpoint
     *                    identifier
     *
     * @throws ClientException when service failed.
     */
    public void connect(String hostname, String apTitle, int aeQualifier) throws ClientException {
        int errorCode = NativeLibTase2.INSTANCE.Tase2_Client_connect(self, hostname, apTitle, aeQualifier);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Failed to connect to server", errorCode);
    }

    /**
     * Set a callback handler that is called when the underlying connection to the
     * server is closed
     * 
     * @param handler   callback interface implementation
     * @param parameter user parameter that will be passed to the callback
     * 
     * @deprecated Use {@link com.libtase2.client.Client#setStateChangedHandler
     *             setStateChangedHandler} instead.
     */
    public void setConnectionClosedHandler(ConnectionClosedHandler handler, Object parameter) {
        connectionClosedHandler = handler;
        connectionClosedHandlerParameter = parameter;
    }

    /**
     * Set a callback handler that is called whenever the client state changes
     * 
     * @param handler   callback interface implementation
     * @param parameter user parameter that will be passed to the callback
     */
    public void setStateChangedHandler(StateChangedHandler handler, Object parameter) {
        stateChangedHandler = handler;
        stateChangedHandlerParameter = parameter;
    }

    /**
     * Set a callback handler to handle information messages from the server
     * 
     * @param handler   callback interface implementation
     * @param parameter user parameter that will be passed to the callback
     */
    public void setIMTransferSetHandler(InformationMessageHandler handler, Object parameter) {
        infoMessHandler = handler;
        infoMessHandlerParameter = parameter;
    }

    /**
     * Set a callback handler to handle received data set transfer set (DSTS)
     * reports
     * 
     * @param handler   callback interface implementation
     * @param parameter user parameter that will be passed to the callback
     */
    public void setDSTransferSetHandler(DSTransferSetHandler handler, Object parameter) {
        dstsHandler = handler;
        dstsHandlerParameter = parameter;
    }

    /**
     * Disconnect from the server
     *
     * NOTE: This function will call
     * {@link com.libtase2.common.Endpoint#disconnect() Endpoint.disconnect} on the
     * underlying endpoint. This means that other clients and servers connected to
     * the endpoint will also be disconnected!
     */
    public void disconnect() {
        NativeLibTase2.INSTANCE.Tase2_Client_disconnect(self);
    }

    /**
     * Get the connection state of the connection
     *
     * @return one of IDLE, CONNECTED, CLOSED
     */
    public ClientState getState() {
        int result = NativeLibTase2.INSTANCE.Tase2_Client_getState(self);

        return ClientState.fromValue(result);
    }

    /**
     * Get the identity of the peer
     * 
     * The returned String array contains three elements: 1. element: vendor 2.
     * element: model 3. element: revision
     * 
     * @return identity information of the string
     * @throws ClientException when service failed.
     */
    public String[] readPeerIdentity() throws ClientException {
        PointerByReference err = new PointerByReference();
        PointerByReference vendor = new PointerByReference();
        PointerByReference model = new PointerByReference();
        PointerByReference revision = new PointerByReference();

        NativeLibTase2.INSTANCE.Tase2_Client_readPeerIdentity(self, err, vendor, model, revision);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Read peer identity failed", errorCode);

        String[] result = new String[3];

        result[0] = vendor.getValue().getString(0);
        result[1] = model.getValue().getString(0);
        result[2] = revision.getValue().getString(0);

        /* free native strings */
        NativeLibTase2.INSTANCE.Tase2_FreeMemory(vendor.getValue());
        NativeLibTase2.INSTANCE.Tase2_FreeMemory(model.getValue());
        NativeLibTase2.INSTANCE.Tase2_FreeMemory(revision.getValue());

        return result;
    }

    /**
     * Get a bit set value representing the support conformance blocks (CB) of the
     * server.
     *
     * @return get the supported features reported by the server
     */
    public SupportedFeatures getSupportedFeatures() {
        return new SupportedFeatures(NativeLibTase2.INSTANCE.Tase2_Client_getSupportedFeatures(self));
    }

    /**
     * Return the bilateral table ID associated with a specific TASE.2 domain
     *
     * This function has to be called after the Tase2_Client_connect function. When
     * the bilateral table ID does not match with the configured one, the client has
     * to close the connection.
     *
     * @return the table ID as string. Memory is allocated by the function and has
     *         to be released by caller.
     * @throws ClientException when service failed.
     */
    public String getBilateralTableId(String domainName) throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer bltIdRef = NativeLibTase2.INSTANCE.Tase2_Client_getBilateralTableId(self, err, domainName);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Read point value failed", errorCode);

        String bltId = null;

        if (bltIdRef != Pointer.NULL) {
            bltId = bltIdRef.getString(0);

            /* free on native pointer */
            NativeLibTase2.INSTANCE.Tase2_FreeMemory(bltIdRef);
        }

        return bltId;
    }

    private List<String> nativeListToManagedList(Pointer ptr) {
        List<String> list = new LinkedList<String>();

        Pointer element = NativeLibTase2.INSTANCE.LinkedList_getNext(ptr);

        while (element != Pointer.NULL) {

            Pointer strPtr = NativeLibTase2.INSTANCE.LinkedList_getData(element);

            String variableName = strPtr.getString(0);

            list.add(variableName);

            element = NativeLibTase2.INSTANCE.LinkedList_getNext(element);
        }

        NativeLibTase2.INSTANCE.LinkedList_destroy(ptr);

        return list;
    }

    /**
     * Get the list of VCC scope variables
     *
     * @return list of variable name strings
     * @throws ClientException when service failed.
     */
    public Collection<String> getVCCVariables() throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Client_getVCCVariables(self, err);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Read VCC variable list failed.", errorCode);

        return nativeListToManagedList(ptr);
    }

    /**
     * Read multiple data points from the server
     *
     * @param domainName the domain name of the data point or null if the data point
     *                   has VCC scope
     * @param pointNames list with the data point names
     *
     * @return a list of point values
     * @throws ClientException
     */
    public Collection<PointValue> readMultiplePointValues(String domainName, Collection<String> pointNames)
            throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer pointNameList = NativeLibTase2.INSTANCE.LinkedList_create();

        for (String pointName : pointNames) {
            Pointer pointNamePtr = new Memory(pointName.length() + 1);
            pointNamePtr.setString(0, pointName);

            NativeLibTase2.INSTANCE.LinkedList_add(pointNameList, pointNamePtr);
        }

        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Client_readMultiplePointValues(self, err, domainName,
                pointNameList);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Read multiple data points failed.", errorCode);

        NativeLibTase2.INSTANCE.LinkedList_destroyStatic(pointNameList);

        LinkedList<PointValue> pointValues = new LinkedList<PointValue>();

        Pointer element = NativeLibTase2.INSTANCE.LinkedList_getNext(ptr);

        while (element != Pointer.NULL) {

            Pointer pointValuePtr = NativeLibTase2.INSTANCE.LinkedList_getData(element);

            PointValue pointValue = new PointValue(pointValuePtr);

            pointValues.add(pointValue);

            element = NativeLibTase2.INSTANCE.LinkedList_getNext(element);
        }

        NativeLibTase2.INSTANCE.LinkedList_destroyStatic(ptr);

        return pointValues;
    }

    /**
     * Get the list of VCC scope data sets
     *
     * @return list of data set name strings
     * @throws ClientException when service failed.
     */
    public Collection<String> getVCCDataSets() throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Client_getVCCDataSets(self, err);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Read VCC data set list failed.", errorCode);

        return nativeListToManagedList(ptr);
    }

    /**
     * Get the list of available domains
     *
     * @return list of domain name strings
     * @throws ClientException when service failed.
     */
    public Collection<String> getDomainNames() throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Client_getDomainNames(self, err);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Read domain names failed.", errorCode);

        return nativeListToManagedList(ptr);
    }

    /**
     * Get the list of domain scope variables
     *
     * @param domainName domain name
     *
     * @return list of variable name strings
     * @throws ClientException when service failed.
     */
    public Collection<String> getDomainVariables(String domainName) throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Client_getDomainVariables(self, domainName, err);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Read domain variables failed.", errorCode);

        return nativeListToManagedList(ptr);
    }

    /**
     * Get the list of domain scope data sets
     *
     * @param domainName domain name
     *
     * @return list of data set name strings
     * @throws ClientException when service failed.
     */
    public Collection<String> getDomainDataSets(String domainName) throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Client_getDomainDataSets(self, domainName, err);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Read domain data sets failed.", errorCode);

        return nativeListToManagedList(ptr);
    }

    /**
     * Enable information message transfer set
     *
     * @throws ClientException when service failed.
     */
    public void enableIMTransferSet() throws ClientException {
        if (infoMessHandlerNative == null) {
            infoMessHandlerNative = new NativeLibTase2.Tase2_InformationMessageHandler() {
                @Override
                public void invoke(Pointer parameter, int infoRef, int localRef, int msgId, int size,
                        Pointer messageBuffer) {
                    if (infoMessHandler != null) {
                        byte[] messBuf = messageBuffer.getByteArray(0, size);

                        infoMessHandler.newMessage(infoMessHandlerParameter, infoRef, localRef, msgId, messBuf);
                    }
                }
            };

            Native.setCallbackThreadInitializer(infoMessHandlerNative, cbThreadInitializer);

            NativeLibTase2.INSTANCE.Tase2_Client_installIMTransferSetHandler(self, infoMessHandlerNative, Pointer.NULL);
        }

        PointerByReference err = new PointerByReference();

        NativeLibTase2.INSTANCE.Tase2_Client_IMTransferSetEnable(self, err);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Failed to enable IM transfer set", errorCode);
    }

    /**
     * Read a data point (indication point or protection equipment) from the server
     *
     * @param domainName the domain name of the data point or NULL if the data point
     *                   has VCC scope
     * @param pointName  the point name of the data point
     *
     * @return The point value, or NULL if the read has not been successful.
     * @throws ClientExceptionin case of a service error
     */
    public PointValue readPointValue(String domainName, String pointName) throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer pointValueRef = NativeLibTase2.INSTANCE.Tase2_Client_readPointValue(self, err, domainName, pointName);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Read point value failed", errorCode);

        return new PointValue(pointValueRef);
    }

    /**
     * Send a command to a device
     * 
     * @param domainName   the domain name of the device or NULL if the tag has VCC
     *                     scope
     * @param deviceName   the point name of the device
     * @param commandValue the command value
     * @return true if the command has been accepted, false otherwise
     * @throws ClientExceptionin case of a service error
     */
    public boolean sendCommand(String domainName, String deviceName, short commandValue) throws ClientException {
        PointerByReference err = new PointerByReference();

        boolean success = (NativeLibTase2.INSTANCE.Tase2_Client_sendCommand(self, err, domainName, deviceName,
                commandValue) != 0);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Send command failed", errorCode);

        return success;
    }

    /**
     * Send a discrete set point to a device
     * 
     * @param domainName    the domain name of the device or NULL if the tag has VCC
     *                      scope
     * @param deviceName    the point name of the device
     * @param setPointValue the set point value
     * @return true if the command has been accepted, false otherwise
     * @throws ClientExceptionin case of a service error
     */
    public boolean sendDiscreteSetPoint(String domainName, String deviceName, short setPointValue)
            throws ClientException {
        PointerByReference err = new PointerByReference();

        boolean success = (NativeLibTase2.INSTANCE.Tase2_Client_sendDiscreteSetPoint(self, err, domainName, deviceName,
                setPointValue) != 0);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Send discrete set point failed", errorCode);

        return success;
    }

    /**
     * Send a real (32 bit float) set point to a device
     * 
     * @param domainName    the domain name of the device or NULL if the tag has VCC
     *                      scope
     * @param deviceName    the point name of the device
     * @param setPointValue the set point value
     * @return true if the command has been accepted, false otherwise
     * @throws ClientException in case of a service error
     */
    public boolean sendRealSetPoint(String domainName, String deviceName, float setPointValue) throws ClientException {
        PointerByReference err = new PointerByReference();

        boolean success = (NativeLibTase2.INSTANCE.Tase2_Client_sendRealSetPoint(self, err, domainName, deviceName,
                setPointValue) != 0);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Send discrete real failed", errorCode);

        return success;
    }

    /**
     * Set tag operation. Set a tag value and a reason message for a device (control
     * point).
     *
     * @param tagValue the new tag value
     * @param reason   a text string to specify the reason of the tag
     *
     * @return true when set tag was successful, false otherwise
     * @throws ClientException in case of a service error
     */
    public boolean setTag(String domainName, String deviceName, TagValue tagValue, String reason)
            throws ClientException {
        PointerByReference err = new PointerByReference();

        boolean success = (NativeLibTase2.INSTANCE.Tase2_Client_setTag(self, err, domainName, deviceName,
                tagValue.getValue(), reason) != 0);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Set tag failed", errorCode);

        return success;
    }

    /**
     * Get tag of a device
     * 
     * @param domainName the domain name of the device or null if the tag has VCC
     *                   scope
     * @param deviceName the point name of the device
     * @return the tag information
     * @throws ClientExceptionin case of a service error
     */
    public Tag getTag(String domainName, String deviceName) throws ClientException {
        PointerByReference err = new PointerByReference();
        PointerByReference isArmedPtr = new PointerByReference();

        Memory mem = new Memory(1024);
        Pointer reasonBufPtr = mem.getPointer(0);

        int tagVal = NativeLibTase2.INSTANCE.Tase2_Client_getTag(self, err, domainName, deviceName, isArmedPtr,
                reasonBufPtr, 1024);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Get tag failed", errorCode);

        boolean isArmed = (isArmedPtr.getPointer().getInt(0) != 0);

        String reason = reasonBufPtr.getString(0);

        TagValue tagValue = TagValue.fromValue(tagVal);

        return new Tag(tagValue, reason, isArmed);
    }

    /**
     * Select a device (command or setpoint) for SBO control mode
     *
     * @param domainName domain name of the device
     * @param deviceName name of the device
     *
     * @return check back ID value
     * @throws ClientException in case of a service error
     */
    public int selectDevice(String domainName, String deviceName) throws ClientException {
        PointerByReference err = new PointerByReference();

        int checkback = NativeLibTase2.INSTANCE.Tase2_Client_selectDevice(self, err, domainName, deviceName);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Select device failed", errorCode);

        return checkback;
    }

    /**
     * Get the next available DS transfer set (GetNextDSTransferSet operation)
     *
     * @param domainName the domain name of associated TASE.2 domain of the
     *                   requesting client
     *
     * @throws ClientException
     *
     * @return transfer set object
     */
    public ClientDSTransferSet getNextDSTransferSet(String domainName) throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer tsRef = NativeLibTase2.INSTANCE.Tase2_Client_getNextDSTransferSet(self, domainName, err);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("get next DS transfer set failed", errorCode);

        ClientDSTransferSet dsts = new ClientDSTransferSet(tsRef);

        synchronized (dsTransferSets) {
            dsTransferSets.add(dsts);
        }

        return dsts;
    }

    /**
     * Delete a data set
     *
     * @param domainName  domain name of the data set or null for a VCC scope data
     *                    set
     * @param dataSetName name of the data set
     * 
     * @throws ClientException
     * 
     * @return true if data set has been deleted, false otherwise
     */
    public boolean deleteDataSet(String domainName, String dataSetName) throws ClientException {
        PointerByReference err = new PointerByReference();

        boolean success = (NativeLibTase2.INSTANCE.Tase2_Client_deleteDataSet(self, err, domainName, dataSetName) != 0);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Failed to delete data set", errorCode);

        return success;
    }

    /**
     * Read the data set directory from the server and return a new ClientDataSet
     * instance
     *
     * NOTE: A ClientDataSet instance will only be created when the data set was
     * read successfully.
     *
     * @param domainName  domain name of the data set, or null for VCC scope data
     *                    sets
     * @param dataSetName the name of the data set
     *
     * @return new instance or NULL when the data set directory cannot be read
     * 
     * @throws ClientException
     */
    public ClientDataSet getDataSet(String domainName, String dataSetName) throws ClientException {
        PointerByReference err = new PointerByReference();

        Pointer dataSetPtr = NativeLibTase2.INSTANCE.Tase2_Client_getDataSet(self, err, domainName, dataSetName);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Failed to get data set", errorCode);

        if (dataSetPtr.equals(Pointer.NULL))
            return null;
        else
            return new ClientDataSet(dataSetPtr);
    }

    /**
     * Request the server to create a new data set
     *
     * The data set element specifications are string of the format "<domain
     * name>/<data set name>"
     *
     * For VCC scope variable the domain name part and the '/' character has to be
     * omitted.
     *
     * @param domainName     domain name of the data set of NULL for a VCC scope
     *                       data set
     * @param dataSetName    name of the data set
     * @param dataSetElemens a list of strings with the data elements of the new
     *                       data set
     * 
     * @throws ClientException
     * 
     * @return true if data set has been created successfully, false otherwise
     */
    public boolean createDataSet(String domainName, String dataSetName, Collection<String> dataSetElements)
            throws ClientException {
        Pointer nativeList = NativeLibTase2.INSTANCE.LinkedList_create();

        for (String dataSetElement : dataSetElements) {

            Pointer dsElementPtr = new Memory(dataSetElement.length() + 1);
            dsElementPtr.setString(0, dataSetElement);

            NativeLibTase2.INSTANCE.LinkedList_add(nativeList, dsElementPtr);
        }

        PointerByReference err = new PointerByReference();

        boolean success = (NativeLibTase2.INSTANCE.Tase2_Client_createDataSet(self, err, domainName, dataSetName,
                nativeList) != 0);

        NativeLibTase2.INSTANCE.LinkedList_destroyStatic(nativeList);

        int errorCode = err.getPointer().getInt(0);

        ClientError error = ClientError.fromValue(errorCode);

        if (error != ClientError.OK)
            throw new ClientException("Failed to create data set", errorCode);

        return success;
    }

    /**
     * Release all resource allocated by the Client instance
     * 
     * NOTE: Do not use the object after calling dispose!
     */
    public void dispose() {
        if (self != null) {

            cbThreadInitializer.detach(dstsValueHandlerNative);
            cbThreadInitializer.detach(dstsReportHandlerNative);
            cbThreadInitializer.detach(scHandler);

            if (infoMessHandlerNative != null) {
                cbThreadInitializer.detach(infoMessHandlerNative);
            }

            if (self != Pointer.NULL)
                NativeLibTase2.INSTANCE.Tase2_Client_destroy(self);

            self = null;

            this.tlsConfiguration = null;
        }
    }

    @Override
    protected void finalize() {
        dispose();
    }
}
