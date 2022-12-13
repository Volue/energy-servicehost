package com.libtase2.common;

import java.util.HashMap;

/*
 *  Copyright 2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.libtase2.client.Client;
import com.libtase2.server.BilateralTable;
import com.libtase2.server.Server;
import com.sun.jna.Callback;
import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

public class Endpoint {

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

        Pointer Tase2_Endpoint_create(Pointer tlsConfig, boolean passive);

        interface Tase2_Endpoint_StateChangedHandler extends Callback {
            void invoke(Pointer endpoint, Pointer parameter, int newState);
        }

        void Tase2_Endpoint_setStateChangedHandler(Pointer self, Tase2_Endpoint_StateChangedHandler handler,
                Pointer parameter);

        interface Tase2_Endpoint_ConnectionHandler extends Callback {
            void invoke(Pointer endpoint, Pointer parameter, Pointer connection, boolean connect);
        }

        void Tase2_Endpoint_setConnectionHandler(Pointer self, Tase2_Endpoint_ConnectionHandler handler,
                Pointer parameter);

        interface Tase2_Endpoint_ConnectionAcceptHandler extends Callback {
            boolean invoke(Pointer endpoint, Pointer parameter, Pointer peerAddress, Pointer bltPlt, int authType,
                    Pointer authValue, int authValLen);
        }

        void Tase2_Endpoint_setConnectionAcceptHandler(Pointer self, Tase2_Endpoint_ConnectionAcceptHandler handler,
                Pointer parameter);

        void Tase2_Endpoint_setAddressMatchingRules(Pointer self, boolean apTitle, boolean aeQualifier,
                boolean pSelector, boolean sSelector, boolean tSelector);

        void Tase2_Endpoint_setIdentity(Pointer self, String vendor, String model, String revision);

        void Tase2_Endpoint_setLocalApTitle(Pointer self, String apTitle, int aeQualifier);

        void Tase2_Endpoint_setLocalApTitleEx(Pointer self, String apTitle);

        void Tase2_Endpoint_setLocalAeQualifier(Pointer self, int aeQualifier);

        void Tase2_Endpoint_setLocalAeInvocationId(Pointer self, int aeInvocationId);

        void Tase2_Endpoint_setLocalApInvocationId(Pointer self, int apInvocationId);

        void Tase2_Endpoint_setLocalAddresses(Pointer self, Tase2_PSelector.ByValue pSelector,
                Tase2_SSelector.ByValue sSelector, Tase2_TSelector.ByValue tSelector);

        void Tase2_Endpoint_setLocalIpAddress(Pointer self, String ipAddress);

        void Tase2_Endpoint_setLocalTcpPort(Pointer self, int tcpPort);

        void Tase2_Endpoint_setRemoteApTitle(Pointer self, String apTitle, int aeQualifier);

        void Tase2_Endpoint_setRemoteApTitleEx(Pointer self, String apTitle);

        void Tase2_Endpoint_setRemoteAeQualifier(Pointer self, int aeQualifier);

        void Tase2_Endpoint_setRemoteAeInvocationId(Pointer self, int aeInvocationId);

        void Tase2_Endpoint_setRemoteApInvocationId(Pointer self, int apInvocationId);

        void Tase2_Endpoint_setRemoteAddresses(Pointer self, Tase2_PSelector.ByValue pSelector,
                Tase2_SSelector.ByValue sSelector, Tase2_TSelector.ByValue tSelector);

        void Tase2_Endpoint_setRemoteIpAddress(Pointer self, String hostname);

        void Tase2_Endpoint_setRemoteTcpPort(Pointer self, int tcpPort);

        void Tase2_Endpoint_setMaxMmsPduSize(Pointer self, int maxPduSize);

        void Tase2_Endpoint_setMaxServOutstandingCalling(Pointer self, int value);

        void Tase2_Endpoint_setMaxServOutstandingCalled(Pointer self, int value);

        void Tase2_Endpoint_setRequestTimeout(Pointer self, int timeoutInMs);

        void Tase2_Endpoint_setConnectTimeout(Pointer self, int timeoutInMs);

        void Tase2_Endpoint_connect(Pointer self);

        void Tase2_Endpoint_disconnect(Pointer self, int disconnectMethod);

        void Tase2_Endpoint_disconnectPeer(Pointer self, String ipAddr);

        boolean Tase2_Endpoint_isConnected(Pointer self);

        int Tase2_Endpoint_connectedPeers(Pointer self);

        boolean Tase2_Endpoint_waitForState(Pointer self, int state, int timeoutInMs);

        int Tase2_Endpoint_getState(Pointer self);

        void Tase2_Endpoint_destroy(Pointer self);

        String Tase2_Endpoint_getId(Pointer self);

        void Tase2_Endpoint_setId(Pointer self, String id);

        void Tase2_Endpoint_setTLSConfiguration(Pointer self, Pointer tlsConfiguration);

        void Tase2_Endpoint_addServer(Pointer self, Pointer server);

        Pointer Tase2_Endpoint_removeServer(Pointer self, Pointer server);

        Pointer Tase2_Endpoint_removeClient(Pointer self, Pointer client);
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

    /*
     * store managed TLSConfiguration instances to avoid problems with garbage
     * collector
     */
    private TLSConfiguration tlsConfiguration = null;
    private TLSConfiguration oldTlsConfiguration = null;

    /*
     * controls if Tase2_Endpoint_destroy is called by the destructor of the managed
     * instance
     */
    private boolean manageNativeInstance = true;

    private List<EndpointConnection> endpointConnections = new LinkedList<EndpointConnection>();

    private static List<Endpoint> managedInstances = new LinkedList<Endpoint>();

    private EndpointConnection lookupConnection(Pointer conPtr) {
        EndpointConnection retVal = null;

        synchronized (endpointConnections) {
            for (EndpointConnection connection : endpointConnections) {
                if (connection.self.equals(conPtr)) {
                    retVal = connection;
                    break;
                }
            }
        }

        return retVal;
    }

    protected static EndpointConnection lookupConnection(Endpoint endpoint, Pointer conPtr) {
        return endpoint.lookupConnection(conPtr);
    }

    protected static Endpoint getManagedInstance(Pointer ptr) {
        Endpoint instance = null;

        synchronized (managedInstances) {
            for (Endpoint endpoint : managedInstances) {
                if (endpoint.self.equals(ptr)) {
                    instance = endpoint;
                    break;
                }
            }
        }

        if (instance == null) {
            instance = new Endpoint(ptr);
        }

        return instance;
    }

    private static class InternalBilateralTable extends BilateralTable {
        protected static BilateralTable getManagedInstance(Pointer ptr) {
            return BilateralTable.getManagedInstance(ptr);
        }

        protected InternalBilateralTable() {
            super(null, null, null, 0);
        }
    }

    NativeLibTase2.Tase2_Endpoint_StateChangedHandler nativeStateChangedHandler = null;
    NativeLibTase2.Tase2_Endpoint_ConnectionHandler nativeConnectionHandler = null;
    NativeLibTase2.Tase2_Endpoint_ConnectionAcceptHandler nativeConnectionAcceptHandler = null;

    private void initializeEndpoint() {
        synchronized (managedInstances) {
            managedInstances.add(this);
        }

        final Endpoint _endpoint = this;

        nativeStateChangedHandler = new NativeLibTase2.Tase2_Endpoint_StateChangedHandler() {

            @Override
            public void invoke(Pointer endpoint, Pointer parameter, int newState) {
                if (stateChangedHandler != null)
                    stateChangedHandler.stateChanged(_endpoint, stateChangedHandlerParameter,
                            EndpointState.fromValue(newState));

            }

        };

        NativeLibTase2.INSTANCE.Tase2_Endpoint_setStateChangedHandler(self, nativeStateChangedHandler, Pointer.NULL);

        nativeConnectionHandler = new NativeLibTase2.Tase2_Endpoint_ConnectionHandler() {

            @Override
            public void invoke(Pointer endpoint, Pointer parameter, Pointer connection, boolean connect) {

                EndpointConnection endpointConnection = null;

                if (connect) {
                    endpointConnection = new EndpointConnection(connection);

                    synchronized (endpointConnections) {
                        endpointConnections.add(endpointConnection);
                    }
                } else {
                    endpointConnection = lookupConnection(connection);

                    synchronized (endpointConnections) {
                        endpointConnections.remove(endpointConnection);
                    }
                }

                if (endpointConnection != null) {
                    if (connectionHandler != null) {
                        if (connect)
                            connectionHandler.connected(_endpoint, connectionHandlerParameter, endpointConnection);
                        else
                            connectionHandler.disconnected(_endpoint, connectionHandlerParameter, endpointConnection);

                    }
                } else {
                    System.err.println("endpoint connection not found!");
                }
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Endpoint_setConnectionHandler(self, nativeConnectionHandler, Pointer.NULL);

        nativeConnectionAcceptHandler = new NativeLibTase2.Tase2_Endpoint_ConnectionAcceptHandler() {

            @Override
            public boolean invoke(Pointer endpoint, Pointer parameter, Pointer peerAddressPtr, Pointer bltPtr,
                    int authType, Pointer authValue, int authValLen) {
                if (connectionAcceptHandler != null) {

                    BilateralTable blt = null;

                    if (bltPtr.equals(Pointer.NULL) == false) {
                        blt = InternalBilateralTable.getManagedInstance(bltPtr);
                    }

                    byte[] authValueJava = null;

                    if (authValue != Pointer.NULL) {
                        authValueJava = authValue.getByteArray(0, authValLen);
                    }

                    String peerAddress = null;

                    if (peerAddressPtr != Pointer.NULL) {
                        peerAddress = peerAddressPtr.getString(0);
                    }

                    return connectionAcceptHandler.connectionRequest(_endpoint, connectionAccpetHandlerParameter,
                            peerAddress, blt, AuthenticationMethod.fromValue(authType), authValueJava);
                }

                return true;
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Endpoint_setConnectionAcceptHandler(self, nativeConnectionAcceptHandler,
                Pointer.NULL);
    }

    /**
     * Create a new endpoint instance.
     * 
     * @param passive true for passive mode (TCP server), false for active mode (TCP
     *                client).
     */
    public Endpoint(boolean passive) {
        self = NativeLibTase2.INSTANCE.Tase2_Endpoint_create(Pointer.NULL, passive);

        initializeEndpoint();
    }

    /**
     * Create a new endpoint instance with TLS support.
     * 
     * @param passive          true for passive mode (TCP server), false for active
     *                         mode (TCP client).
     * @param tlsConfiguration the TLS configuration to use.
     */
    public Endpoint(boolean passive, TLSConfiguration tlsConfiguration) {
        if (tlsConfiguration != null)
            self = NativeLibTase2.INSTANCE.Tase2_Endpoint_create(tlsConfiguration.self, passive);
        else
            self = NativeLibTase2.INSTANCE.Tase2_Endpoint_create(Pointer.NULL, passive);

        this.tlsConfiguration = tlsConfiguration;

        initializeEndpoint();
    }

    /* only for internal use */
    protected Endpoint(Pointer nativeInstance) {
        self = nativeInstance;
        manageNativeInstance = false;

        initializeEndpoint();
    }

    private EndpointStateChangedHandler stateChangedHandler = null;
    private Object stateChangedHandlerParameter = null;

    /**
     * Set a handler that is called whenever the state of the endpoint changes.
     *
     * @param handler   the handler interface implementation
     * @param parameter parameter that is passed when the interface is called
     */
    public void setStateChangedHandler(EndpointStateChangedHandler handler, Object parameter) {
        stateChangedHandler = handler;
        stateChangedHandlerParameter = parameter;
    }

    private EndpointConnectionHandler connectionHandler = null;
    private Object connectionHandlerParameter = null;

    /**
     * Set a handler that is called whenever a connection to a remote endpoint is
     * opened or closed.
     *
     * @param handler   the handler interface implementation
     * @param parameter parameter that is passed when the interface is called
     */
    public void setConnectionHandler(EndpointConnectionHandler handler, Object parameter) {
        connectionHandler = handler;
        connectionHandlerParameter = parameter;
    }

    private ConnectionAcceptHandler connectionAcceptHandler = null;
    private Object connectionAccpetHandlerParameter = null;

    public void setConnectionAcceptHandler(ConnectionAcceptHandler handler, Object parameter) {
        connectionAcceptHandler = handler;
        connectionAccpetHandlerParameter = parameter;
    }

    /**
     * Set the address matching rules for identifying peer endpoints
     *
     * @param apTitle     true, use AP-title for matching
     * @param aeQualifier true, use AE-qualifier for matching
     * @param pSelector   true, use presentation selector for matching
     * @param sSelector   true, use session selector for matching
     * @param tSelector   true, use transport selector for matching
     */
    public void setAddressMatchingRules(boolean apTitle, boolean aeQualifier, boolean pSelector, boolean sSelector,
            boolean tSelector) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setAddressMatchingRules(self, apTitle, aeQualifier, pSelector, sSelector,
                tSelector);
    }

    /**
     * Set own identity for the identity service
     *
     * @param vendor   the vendor name of the TASE.2 application
     * @param model    the model name of the TASE.2 application
     * @param revision the revision string of the TASE.2 application
     */
    public void setIdentity(String vendor, String model, String revision) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setIdentity(self, vendor, model, revision);
    }

    /**
     * Set the local AP-title value
     *
     * @param apTitle     the new AP-title value
     * @param aeQualifier the new AE-qualifier value
     */
    public void setLocalApTitle(String apTitle, int aeQualifier) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setLocalApTitle(self, apTitle, aeQualifier);
    }

    /**
     * Set the local AP-title value
     *
     * @param apTitle the new AP-title value
     */
    public void setLocalApTitle(String apTitle) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setLocalApTitleEx(self, apTitle);
    }

    /**
     * Set local AE-qualifier
     *
     * @param aeQualifier the new AE-qualifier value
     */
    public void setLocalAeQualifier(int aeQualifier) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setLocalAeQualifier(self, aeQualifier);
    }

    public void setLocalAeInvocationId(int aeInvocationId) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setLocalAeInvocationId(self, aeInvocationId);
    }

    public void setLocalApInvocationId(int apInvocationId) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setLocalApInvocationId(self, apInvocationId);
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

        if (tSelector.length > 16)
            throw new IllegalArgumentException("s-selector too long (max 16 bytes)!");

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

        NativeLibTase2.INSTANCE.Tase2_Endpoint_setLocalAddresses(self, pSel, sSel, tSel);
    }

    /**
     * Sets the local IP address
     *
     * @param ipAddress local IP address to use.
     */
    public void setLocalIpAddress(String ipAddress) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setLocalIpAddress(self, ipAddress);
    }

    /**
     * Sets the local TCP port (only for passive mode)
     *
     * If not set, the default port is used (102 for MMS, 3872 for MMS/TLS).
     *
     * @param tcpPort local TCP port to use.
     */
    public void setLocalTcpPort(int tcpPort) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setLocalTcpPort(self, tcpPort);
    }

    /**
     * Set remote AP-title and AE-qualifier (for active endpoint)
     *
     * @param apTitle     the new AP-title value
     * @param aeQualifier the new AE-qualifier value
     */
    public void setRemoteApTitle(String apTitle, int aeQualifier) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setRemoteApTitle(self, apTitle, aeQualifier);
    }

    /**
     * Set remote AP-title (for active endpoint)
     *
     * @param apTitle the new AP-title value
     */
    public void setRemoteApTitle(String apTitle) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setRemoteApTitleEx(self, apTitle);
    }

    /**
     * Set remote AE-qualifier (for active endpoint)
     *
     * @param aeQualifier the new AE-qualifier value
     */
    public void setRemoteAeQualifier(int aeQualifier) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setRemoteAeQualifier(self, aeQualifier);
    }

    public void setRemoteAeInvocationId(int aeInvocationId) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setRemoteAeInvocationId(self, aeInvocationId);
    }

    public void setRemoteApInvocationId(int apInvocationId) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setRemoteApInvocationId(self, apInvocationId);
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

        NativeLibTase2.INSTANCE.Tase2_Endpoint_setRemoteAddresses(self, pSel, sSel, tSel);
    }

    /**
     * Sets the remote IP address (only for active mode)
     *
     * @param ipAddress remote IP address to use.
     */
    public void setRemoteIpAddress(String ipAddress) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setRemoteIpAddress(self, ipAddress);
    }

    /**
     * Sets the remote TCP port (only for active mode)
     *
     * If not set, the default port is used (102 for MMS, 3872 for MMS/TLS).
     *
     * @param tcpPort remote TCP port to use.
     */
    public void setRemoteTcpPort(int tcpPort) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setRemoteTcpPort(self, tcpPort);
    }

    /**
     * Sets the maximum MMS PDU size used by the endpoint
     *
     * NOTE: Actual values are negotiated with the peer and are equal or below this
     * value.
     *
     * @param self       endpoint instance to use
     * @param maxPduSize the maximum PDU size in bytes
     */
    public void setMaxPduSize(int maxPduSize) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setMaxMmsPduSize(self, maxPduSize);
    }

    /**
     * Set the maximum number of outstanding service calls (when calling)
     * 
     * NOTE: Actual values are negotiated with the peer and are equal or below this
     * value.
     * 
     * @param value maximum allowed number of outstanding service calls
     */
    public void setMaxServOutstandingCalling(int value) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setMaxServOutstandingCalling(self, value);
    }

    /**
     * Set the maximum number of outstanding service calls (when called)
     * 
     * NOTE: Actual values are negotiated with the peer and are equal or below this
     * value.
     * 
     * @param value maximum allowed number of outstanding service calls
     */
    public void setMaxServOutstandingCalled(int value) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setMaxServOutstandingCalled(self, value);
    }

    /**
     * Sets the connect timeout of active endpoints
     *
     * Default value is 10000 ms.
     *
     * @param self        endpoint instance to use
     * @param timeoutInMs timeout in milliseconds
     */
    public void setConnectTimeout(int timeoutInMs) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setConnectTimeout(self, timeoutInMs);
    }

    /**
     * Sets the timeout for confirmed request messages
     *
     * Default value is 5000 ms.
     *
     * @param self        endpoint instance to use
     * @param timeoutInMs timeout in milliseconds
     */
    public void setRequestTimeout(int timeoutInMs) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setRequestTimeout(self, timeoutInMs);
    }

    /**
     * Connect the endpoint (start a passive or active TCP endpoint)
     */
    public void connect() {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_connect(self);
    }

    /**
     * Disconnect (close all connections and stop listening for new connections)
     */
    public void disconnect() {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_disconnect(self, 0);
    }

    /**
     * Disconnect a specific connection identified by the IP address and optionally
     * by the port number
     * 
     * @param peerAddress peer IP address (e.g. "10.0.0.10") with optional port
     *                    number (e.g. "10.0.0.10:34237")
     */
    public void disconnectPeer(String peerAddress) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_disconnectPeer(self, peerAddress);
    }

    /**
     * Check if Check if the endpoint is connected with at least one TCP clients
     * (passive endpoint) or connected to a TCP server (active endpoint)
     *
     * @return true if the endpoint is connected or listening, false otherwise
     */
    public boolean isConnected() {
        return NativeLibTase2.INSTANCE.Tase2_Endpoint_isConnected(self);
    }

    /**
     * Get number of connected peers
     *
     * @return number of connected peers
     */
    public int connectedPeers() {
        return NativeLibTase2.INSTANCE.Tase2_Endpoint_connectedPeers(self);
    }

    /**
     * Wait until the endpoint enters a specific state of state changes to ERROR
     *
     * @param state   the expected state (state to wait for)
     * @param timeout maximum time to wait for the expected state (in ms)
     *
     * @return true when state is reached, false when timeout or error occurs.
     */
    public boolean waitForState(EndpointState state, int timeoutInMs) {
        return NativeLibTase2.INSTANCE.Tase2_Endpoint_waitForState(self, state.getValue(), timeoutInMs);
    }

    /**
     * Get the current state of the endpoint
     *
     * NOTE: Possible values depend on the endpoint type (active/passive).
     *
     * @return current state of the endpoint
     */
    public EndpointState getState() {
        int stateVal = NativeLibTase2.INSTANCE.Tase2_Endpoint_getState(self);
        return EndpointState.fromValue(stateVal);
    }

    /**
     * Set the ID string of the endpoint
     *
     * The ID string can be used by the application to identify the endpoint.
     *
     * @param id the ID string of the endpoint
     */
    public void setId(String id) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setId(self, id);
    }

    /**
     * Get the ID string of the endpoint
     *
     * The ID string can be used by the application to identify the endpoint.
     *
     * @return the ID string of the endpoint
     */
    public String getId() {
        return NativeLibTase2.INSTANCE.Tase2_Endpoint_getId(self);
    }

    /**
     * Set the TLS Configuration during runtime.
     * 
     * NOTE: New configuration will only be active when endpoint is restarted.
     * 
     * @param tlsConfiguration the TLS configuration object to use
     */
    public void setTLSConfiguration(TLSConfiguration tlsConfiguration) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_setTLSConfiguration(self, tlsConfiguration.self);

        this.oldTlsConfiguration = this.tlsConfiguration;
        this.tlsConfiguration = tlsConfiguration;
    }

    /**
     * Add a server to the endpoint
     * 
     * NOTE: This function can be used to add a server to an endpoint that has been
     * removed before using the removeServer method
     * 
     * @param server the server to be added
     */
    public void addServer(Server server) {
        NativeLibTase2.INSTANCE.Tase2_Endpoint_addServer(self, server.self);
    }

    /**
     * Remove a server from the endpoint
     * 
     * @param server the server to be removed
     * @return removed server, or null when the server was not associated with this
     *         endpoint
     */
    public Server removeServer(Server server) {
        Pointer retVal = NativeLibTase2.INSTANCE.Tase2_Endpoint_removeServer(self, server.self);

        if (retVal == Pointer.NULL)
            return null;
        else
            return server;
    }

    /**
     * Remove a client from the endpoint
     * 
     * @param client removed client, or null when the client was not associated with
     *               this endpoint
     * @return the client to be removed
     */
    public Client removeClient(Client client) {
        Pointer retVal = NativeLibTase2.INSTANCE.Tase2_Endpoint_removeClient(self, client.self);

        if (retVal == Pointer.NULL)
            return null;
        else
            return client;
    }

    /**
     * Release all resource allocated by the Endpoint instance
     * 
     * NOTE: Do not use the object after calling dispose!
     */
    public void dispose() {
        if (self != null) {

            synchronized (managedInstances) {
                managedInstances.remove(this);
            }

            if (self != Pointer.NULL)
                NativeLibTase2.INSTANCE.Tase2_Endpoint_destroy(self);

            self = null;

            this.tlsConfiguration = null;
        }
    }

    @Override
    protected void finalize() {
        dispose();
    }
}
