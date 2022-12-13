package com.libtase2.server;

import java.util.Collection;
import java.util.HashMap;

/*
 *  Copyright 2018, 2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.libtase2.common.Endpoint;
import com.libtase2.common.EndpointConnection;
import com.libtase2.common.LibTase2;
import com.libtase2.common.LogLevel;
import com.libtase2.common.PointValue;
import com.libtase2.common.ReportReason;
import com.libtase2.common.TLSConfiguration;
import com.libtase2.common.Tag;
import com.libtase2.common.TagValue;
import com.sun.jna.Callback;
import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

/**
 * Represents a TASE.2 server instance
 */
public class Server {

    /* helper class to access library internal Endpoint functions */
    private static class InternalEndpoint extends Endpoint {
        protected static Endpoint getManagedInstance(Pointer ptr) {
            return Endpoint.getManagedInstance(ptr);
        }

        protected InternalEndpoint(Pointer nativeInstance) {
            super(nativeInstance);
        }

        protected static EndpointConnection lookupConnection(Endpoint endpoint, Pointer peer) {
            return Endpoint.lookupConnection(endpoint, peer);
        }
    }

    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        public static class Statistics extends Structure {

            public static class ByValue extends Statistics implements Structure.ByValue {
            }

            public long messagesSent;
            public long messagesRcvd;
            public long errors;

            @Override
            protected List<String> getFieldOrder() {
                List<String> fieldOrder = new LinkedList<String>();

                fieldOrder.add("messagesSent");
                fieldOrder.add("messagesRcvd");
                fieldOrder.add("errors");

                return fieldOrder;
            }

        }

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

        Pointer Tase2_Server_create(Pointer dataModel, Pointer tlsConfiguration);

        Pointer Tase2_Server_createEx(Pointer dataModel, Pointer endpoint);

        void Tase2_Server_start(Pointer self);

        void Tase2_Server_stop(Pointer self);

        byte Tase2_Server_isRunning(Pointer self);

        Pointer Tase2_Server_getEndpoint(Pointer self);

        void Tase2_Server_setLocalIpAddress(Pointer self, String localIpAddress);

        void Tase2_Server_setLocalApTitle(Pointer self, String apTitle, int aeQualifier);

        void Tase2_Server_setLocalAddresses(Pointer self, Tase2_PSelector pSelector, Tase2_SSelector sSelector,
                Tase2_TSelector tSelector);

        void Tase2_Server_destroy(Pointer self);

        void Tase2_Server_setIdentity(Pointer self, String vendor, String model, String revision);

        byte Tase2_Server_requestReport(Pointer self, Pointer transferSet, int reason, short eventCode);

        void Tase2_Server_setSelectTimeout(Pointer self, int timeoutInMs);

        void Tase2_Server_setTcpPort(Pointer self, int tcpPort);

        void Tase2_Server_setSupportedFeatures(Pointer self, int value);

        void Tase2_Server_setCriticalReportTimeout(Pointer self, int timeoutInMs);

        void Tase2_Server_setMaxDSTSEventQueueSize(Pointer self, int maxSize);

        void Tase2_Server_addBilateralTable(Pointer self, Pointer blt);

        void Tase2_Server_updateOnlineValue(Pointer self, Pointer dataPoint);

        void Tase2_Server_updateOnlineValueNoTrigger(Pointer self, Pointer dataPoint);

        void Tase2_Server_updateByOnlineValue(Pointer self, Pointer dataPoint);

        Statistics.ByValue Tase2_Server_getStatistics(Pointer self);

        interface InternalClientConnectionHandler extends Callback {
            void invoke(Pointer parameter, String peerAddress, Pointer bltPtr, byte connect);
        }

        void Tase2_Server_setClientConnectionHandler(Pointer self, InternalClientConnectionHandler HandlerResult,
                Pointer parameter);

        interface Tase2_SelectHandler extends Callback {
            int invoke(Pointer parameter, Pointer controlPoint);
        }

        void Tase2_Server_setSelectHandler(Pointer self, Tase2_SelectHandler handler, Pointer parameter);

        interface Tase2_OperateHandlerEx extends Callback {
            int invoke(Pointer parameter, Pointer controlPoint, short discreteValue, float realValue);
        }

        void Tase2_Server_setOperateHandlerEx(Pointer self, Tase2_OperateHandlerEx handler, Pointer parameter);

        interface InternalSetTagHandler extends Callback {
            int invoke(Pointer parameter, Pointer controlPoint, int tagValue, String reason);
        }

        void Tase2_Server_setSetTagHandler(Pointer self, InternalSetTagHandler handler, Pointer parameter);

        interface InternalWriteDataHandler extends Callback {
            int invoke(Pointer parameter, Pointer domain, Pointer dataPoint, Pointer pointValue);
        }

        void Tase2_Server_setWriteDataHandler(Pointer self, InternalWriteDataHandler handler, Pointer parameter);

        interface InternalTSCriticalNotConfirmedHandler extends Callback {
            void invoke(Pointer parameter, Pointer transferSet, long timeSent, byte nack);
        }

        interface InternalDSTransferSetUpdateHandler extends Callback {
            void invoke(Pointer parameter, Pointer peer, Pointer clientBlt, Pointer transferSet, boolean isEnabled);
        }

        interface InternalDataSetEventHandler extends Callback {
            void invoke(Pointer parameter, boolean create, Pointer peer, Pointer clientBlt, Pointer dataSetDomain,
                    String dataSetName, Pointer dataPoints);
        }

        interface InternalDSTransferSetReportSentHandler extends Callback {
            void invoke(Pointer parameter, Pointer peer, Pointer clientBlt, Pointer transferSet, Pointer sentDataPoints,
                    int reason);
        }

        interface InternalIMTransferSetEventHandler extends Callback {
            void invoke(Pointer parameter, Pointer peer, Pointer clientblt, boolean enabled);
        }

        void Tase2_Server_setTSCriticalNotConfirmedHandler(Pointer self, InternalTSCriticalNotConfirmedHandler handler,
                Pointer parameter);

        void Tase2_Server_setDSTransferSetUpdateHandler(Pointer self, InternalDSTransferSetUpdateHandler handler,
                Pointer parameter);

        void Tase2_Server_setDataSetEventHandler(Pointer self, InternalDataSetEventHandler handler, Pointer parameter);

        void Tase2_Server_setDSTransferSetReportSentHandler(Pointer self,
                InternalDSTransferSetReportSentHandler handler, Pointer parameter);

        void Tase2_Server_setIMTransferSetEventHandler(Pointer self, InternalIMTransferSetEventHandler handler,
                Pointer parameter);

        void Tase2_Server_sendInformationMessage(Pointer self, Pointer domain, int infoRef, int localRef, int msgId,
                int size, byte[] buffer);

        Pointer LinkedList_create();

        void LinkedList_add(Pointer self, Pointer data);

        Pointer LinkedList_getNext(Pointer self);

        Pointer LinkedList_getData(Pointer self);

        Pointer Tase2_SentPointValue_getDataPoint(Pointer self);

        Pointer Tase2_SentPointValue_getPointValue(Pointer self);
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

    /*
     * used to store the list of managed bilateral tables. Prevents the garbage
     * collector to remove the managed instance
     */
    private List<BilateralTable> bltList = new LinkedList<BilateralTable>();

    public Pointer self = Pointer.NULL;

    public Endpoint endpoint = null;

    private DataModel dataModel = null;
    private TLSConfiguration tlsConfiguration = null;

    private ClientConnectionHandler clientConnectionHandler = null;
    private Object clientConnectionHandlerParameter = null;

    private DeviceOperationHandler deviceOperationHandler = null;
    private Object deviceOperationHandlerParameter = null;

    private WriteDataHandler writeDataHandler = null;
    private Object writeDataHandlerParameter = null;

    private TSCriticalNotConfirmedHandler criticalReportNotConfirmedHandler = null;
    private Object criticalReportNotConfirmedHandlerParameter = null;

    private DSTransferSetUpdateHandler dsTransferSetUpdateHandler = null;
    private Object dsTransferSetUpdateHandlerParameter = null;

    private DataSetEventHandler dataSetEventHandler = null;
    private Object dataSetEventHandlerParameter = null;

    private DSTransferSetReportSentHandler dsTransferSetReportSentHandler = null;
    private Object dsTransferSetReportSentHandlerParameter = null;

    private IMTransferSetEventHandler imTransferSetEventHandler = null;
    private Object imTransferSetEventHandlerParameter = null;

    private static class InternalBilateralTable extends BilateralTable {
        protected static BilateralTable getManagedInstance(Pointer ptr) {
            return BilateralTable.getManagedInstance(ptr);
        }

        protected InternalBilateralTable() {
            super(null, null, null, 0);
        }
    }

    NativeLibTase2.InternalClientConnectionHandler nativeConnectionHandler;

    NativeLibTase2.InternalSetTagHandler nativeTagHandler;

    NativeLibTase2.Tase2_SelectHandler nativeSelectHandler;

    NativeLibTase2.Tase2_OperateHandlerEx nativeOperateHandler;

    NativeLibTase2.InternalWriteDataHandler nativeWriteHandler;

    NativeLibTase2.InternalTSCriticalNotConfirmedHandler nativeCriticalHandler;

    NativeLibTase2.InternalDSTransferSetUpdateHandler nativeDSTransferSetUpdateHandler;

    NativeLibTase2.InternalDataSetEventHandler nativeDataSetEventHandler;

    NativeLibTase2.InternalDSTransferSetReportSentHandler nativeDSTransferSetReportSentHandler;

    NativeLibTase2.InternalIMTransferSetEventHandler nativeIMTransferSetEventHandler;

    private void initializeServerInstance() {
        final Server _server = this;

        nativeConnectionHandler = new NativeLibTase2.InternalClientConnectionHandler() {

            @Override
            public void invoke(Pointer parameter, String peerAddress, Pointer bltPtr, byte connect) {
                BilateralTable blt = null;

                if (bltPtr != Pointer.NULL) {
                    blt = InternalBilateralTable.getManagedInstance(bltPtr);

                    if (blt == null) {
                        System.err.println("INTERNAL ERROR: blt == null!");
                    }
                }

                if (clientConnectionHandler != null)
                    clientConnectionHandler.connectionEvent(clientConnectionHandlerParameter, peerAddress, blt,
                            (connect != 0));

            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setClientConnectionHandler(self, nativeConnectionHandler, Pointer.NULL);

        nativeTagHandler = new NativeLibTase2.InternalSetTagHandler() {

            @Override
            public int invoke(Pointer parameter, Pointer controlPoint, int tagValue, String reason) {
                HandlerResult result = HandlerResult.SUCCESS;

                if (deviceOperationHandler != null) {

                    /* lookup managed ControlPoint instance */
                    ControlPoint point = (ControlPoint) dataModel.getDataPointByPointer(controlPoint);

                    if (point != null) {
                        Tag tag = new Tag(TagValue.fromValue(tagValue), reason, false);

                        result = deviceOperationHandler.setTag(deviceOperationHandlerParameter, point, tag);
                    } else {
                        LibTase2.log(LogLevel.ERROR,
                                "com.libtase2.server.Server: internal error - set tag handler called with invalid control point");
                    }

                }

                return result.getValue();
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setSetTagHandler(self, nativeTagHandler, Pointer.NULL);

        nativeSelectHandler = new NativeLibTase2.Tase2_SelectHandler() {

            @Override
            public int invoke(Pointer parameter, Pointer controlPoint) {
                HandlerResult result = HandlerResult.SUCCESS;

                if (deviceOperationHandler != null) {
                    /* lookup managed ControlPoint instance */
                    ControlPoint point = (ControlPoint) dataModel.getDataPointByPointer(controlPoint);

                    result = deviceOperationHandler.select(deviceOperationHandlerParameter, point);
                }

                return result.getValue();
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setSelectHandler(self, nativeSelectHandler, Pointer.NULL);

        nativeOperateHandler = new NativeLibTase2.Tase2_OperateHandlerEx() {

            @Override
            public int invoke(Pointer parameter, Pointer controlPoint, short discreteValue, float realValue) {
                HandlerResult result = HandlerResult.SUCCESS;

                if (deviceOperationHandler != null) {
                    /* lookup managed ControlPoint instance */
                    ControlPoint point = (ControlPoint) dataModel.getDataPointByPointer(controlPoint);

                    OperateValue opVal = new OperateValue(discreteValue, realValue);

                    result = deviceOperationHandler.operate(deviceOperationHandlerParameter, point, opVal);
                }

                return result.getValue();
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setOperateHandlerEx(self, nativeOperateHandler, Pointer.NULL);

        nativeWriteHandler = new NativeLibTase2.InternalWriteDataHandler() {

            @Override
            public int invoke(Pointer parameter, Pointer domainPtr, Pointer dataPointPtr, Pointer pointValuePtr) {
                HandlerResult result = HandlerResult.SUCCESS;

                if (writeDataHandler != null) {
                    Domain domain = dataModel.getDomainByPtr(domainPtr);
                    DataPoint dataPoint = dataModel.getDataPointByPointer(dataPointPtr);
                    PointValue pointValue = new PointValue(pointValuePtr, true);

                    result = writeDataHandler.writeDataEvent(writeDataHandlerParameter, domain, dataPoint, pointValue);
                }

                return result.getValue();
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setWriteDataHandler(self, nativeWriteHandler, Pointer.NULL);

        nativeCriticalHandler = new NativeLibTase2.InternalTSCriticalNotConfirmedHandler() {

            @Override
            public void invoke(Pointer parameter, Pointer transferSet, long timeSent, byte nack) {

                if (criticalReportNotConfirmedHandler != null) {

                    DSTransferSet ts = dataModel.getDSTransferSetByPtr(transferSet);

                    if (criticalReportNotConfirmedHandler != null) {
                        criticalReportNotConfirmedHandler.criticalReportNotConfirmed(
                                criticalReportNotConfirmedHandlerParameter, ts, timeSent, (nack != 0));
                    }
                }

            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setTSCriticalNotConfirmedHandler(self, nativeCriticalHandler,
                Pointer.NULL);

        nativeDSTransferSetUpdateHandler = new NativeLibTase2.InternalDSTransferSetUpdateHandler() {

            @Override
            public void invoke(Pointer parameter, Pointer peer, Pointer clientBlt, Pointer transferSet,
                    boolean isEnabled) {
                if (dsTransferSetUpdateHandler != null) {

                    DSTransferSet ts = dataModel.getDSTransferSetByPtr(transferSet);

                    BilateralTable peerBlt = BilateralTable.getManagedInstance(clientBlt);

                    EndpointConnection peerConnection = InternalEndpoint.lookupConnection(_server.endpoint, peer);

                    dsTransferSetUpdateHandler.transferSetUpdated(dsTransferSetUpdateHandlerParameter, peerConnection,
                            peerBlt, ts, isEnabled);
                }

            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setDSTransferSetUpdateHandler(self, nativeDSTransferSetUpdateHandler,
                Pointer.NULL);

        nativeDataSetEventHandler = new NativeLibTase2.InternalDataSetEventHandler() {

            @Override
            public void invoke(Pointer parameter, boolean create, Pointer peer, Pointer clientBlt,
                    Pointer dataSetDomain, String dataSetName, Pointer dataPoints) {

                if (dataSetEventHandler != null) {

                    BilateralTable peerBlt = BilateralTable.getManagedInstance(clientBlt);

                    EndpointConnection peerConnection = InternalEndpoint.lookupConnection(_server.endpoint, peer);

                    Domain domain = dataModel.getDomainByPtr(dataSetDomain);

                    Pointer element = NativeLibTase2.INSTANCE.LinkedList_getNext(dataPoints);

                    Collection<DataPoint> dsDataPoints = new LinkedList<DataPoint>();

                    while (element != Pointer.NULL) {
                        Pointer dataPointPtr = NativeLibTase2.INSTANCE.LinkedList_getData(element);

                        DataPoint dataPoint = dataModel.getDataPointByPointer(dataPointPtr);

                        if (dataPoint != null)
                            dsDataPoints.add(dataPoint);

                        element = NativeLibTase2.INSTANCE.LinkedList_getNext(element);
                    }

                    dataSetEventHandler.dataSetEvent(dataSetEventHandlerParameter, create, peerConnection, peerBlt,
                            domain, dataSetName, dsDataPoints);
                }

            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setDataSetEventHandler(self, nativeDataSetEventHandler, Pointer.NULL);

        nativeDSTransferSetReportSentHandler = new NativeLibTase2.InternalDSTransferSetReportSentHandler() {

            @Override
            public void invoke(Pointer parameter, Pointer peer, Pointer clientBlt, Pointer transferSet,
                    Pointer sentDataPoints, int reason) {
                if (dsTransferSetReportSentHandler != null) {

                    BilateralTable peerBlt = BilateralTable.getManagedInstance(clientBlt);

                    EndpointConnection peerConnection = InternalEndpoint.lookupConnection(_server.endpoint, peer);

                    DSTransferSet ts = dataModel.getDSTransferSetByPtr(transferSet);

                    Collection<SentPointValue> dataPoints = new LinkedList<SentPointValue>();

                    Pointer element = NativeLibTase2.INSTANCE.LinkedList_getNext(sentDataPoints);

                    while (element != Pointer.NULL) {
                        Pointer sentPointValuePtr = NativeLibTase2.INSTANCE.LinkedList_getData(element);

                        if (sentPointValuePtr != Pointer.NULL) {

                            Pointer dataPointPtr = NativeLibTase2.INSTANCE
                                    .Tase2_SentPointValue_getDataPoint(sentPointValuePtr);
                            Pointer pointValuePtr = NativeLibTase2.INSTANCE
                                    .Tase2_SentPointValue_getPointValue(sentPointValuePtr);

                            if ((dataPointPtr != Pointer.NULL) && (pointValuePtr != Pointer.NULL)) {
                                DataPoint dataPoint = dataModel.getDataPointByPointer(dataPointPtr);
                                PointValue pointValue = new PointValue(pointValuePtr, true);

                                SentPointValue sentPointValue = new SentPointValue(dataPoint, pointValue);

                                dataPoints.add(sentPointValue);
                            }
                        }

                        element = NativeLibTase2.INSTANCE.LinkedList_getNext(element);
                    }

                    dsTransferSetReportSentHandler.sentReport(dsTransferSetReportSentHandlerParameter, peerConnection,
                            peerBlt, ts, dataPoints, new ReportReason((short) reason));
                }
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setDSTransferSetReportSentHandler(self,
                nativeDSTransferSetReportSentHandler, Pointer.NULL);

        nativeIMTransferSetEventHandler = new NativeLibTase2.InternalIMTransferSetEventHandler() {

            @Override
            public void invoke(Pointer parameter, Pointer peer, Pointer clientBlt, boolean enabled) {

                if (imTransferSetEventHandler != null) {
                    BilateralTable peerBlt = BilateralTable.getManagedInstance(clientBlt);

                    EndpointConnection peerConnection = InternalEndpoint.lookupConnection(_server.endpoint, peer);

                    imTransferSetEventHandler.event(imTransferSetEventHandlerParameter, peerConnection, peerBlt,
                            enabled);
                }
            }
        };

        NativeLibTase2.INSTANCE.Tase2_Server_setIMTransferSetEventHandler(self, nativeIMTransferSetEventHandler,
                Pointer.NULL);
    }

    /**
     * Get the endpoint instance this server is assigned to
     */
    public Endpoint getEndpoint() {
        if (this.endpoint != null)
            return this.endpoint;

        Pointer endpointPtr = NativeLibTase2.INSTANCE.Tase2_Server_getEndpoint(self);

        this.endpoint = InternalEndpoint.getManagedInstance(endpointPtr);

        return this.endpoint;
    }

    /**
     * Create a new TASE.2 server instance
     *
     * NOTE: This function creates a new passive {@link com.libtase2.common.Endpoint
     * Endpoint} instance (TCP server).
     *
     * @param dataModel the data model to use
     */
    public Server(DataModel dataModel) {
        self = NativeLibTase2.INSTANCE.Tase2_Server_create(dataModel.self, Pointer.NULL);
        this.dataModel = dataModel;
        initializeServerInstance();
    }

    /**
     * Create a new TASE.2 server instance
     *
     * NOTE: This function creates a new passive {@link com.libtase2.common.Endpoint
     * Endpoint} instance (TCP server).
     *
     * @param dataModel        the data model to use
     * @param tlsConfiguration the TLS configuration to use
     */
    public Server(DataModel dataModel, TLSConfiguration tlsConfiguration) {
        self = NativeLibTase2.INSTANCE.Tase2_Server_create(dataModel.self, tlsConfiguration.self);
        this.dataModel = dataModel;
        this.tlsConfiguration = tlsConfiguration;
        initializeServerInstance();
    }

    /**
     * Create a new TASE.2 server instance using an existing
     * {@link com.libtase2.common.Endpoint Endpoint} instance
     *
     * @param dataModel the data model to use
     * @param endpoint  the endpoint instance to use
     */
    public Server(DataModel dataModel, Endpoint endpoint) {
        self = NativeLibTase2.INSTANCE.Tase2_Server_createEx(dataModel.self, endpoint.self);
        this.dataModel = dataModel;
        initializeServerInstance();
    }

    /**
     * Set the local IP address
     *
     * NOTE: Calls {@link com.libtase2.common.Endpoint#setLocalIpAddress(String)
     * Endpoint.setLocalIpAddress}
     *
     * @param localIpAddress the local IP address as C string (an internal copy will
     *                       be created)
     */
    public void setLocalIpAddress(String ipAddress) {
        NativeLibTase2.INSTANCE.Tase2_Server_setLocalIpAddress(self, ipAddress);
    }

    /**
     * Set the local TCP port.
     *
     * NOTE: Calls {@link com.libtase2.common.Endpoint#setLocalTcpPort(int)
     * Endpoint.setLocalTcpPort}
     *
     * @param tcpPort TCP port number or -1 to use the default TCP port numbers 102
     *                (MMS) or 3782 (MMS/TLS)
     */
    public void setTcpPort(int tcpPort) {
        NativeLibTase2.INSTANCE.Tase2_Server_setTcpPort(self, tcpPort);
    }

    /**
     * Set application endpoint identifier of the local endpoint
     *
     * NOTE: It is important to set this parameter correctly to match the remotely
     * configured parameter.
     *
     * @param apTitle     AP-title part of the local application endpoint identifier
     * @param aeQualifier AE-qualifier part of the local application endpoint
     *                    identifier
     */
    public void setLocalApTitle(String apTitle, int aeQualifier) {
        NativeLibTase2.INSTANCE.Tase2_Server_setLocalApTitle(self, apTitle, aeQualifier);
    }

    /**
     * Set local addresses for the lower layers
     *
     * This function can be used to set the addresses for the lower layer protocols
     * (presentation, session, and transport layer). Calling this function is
     * optional. It might be required when the peer expects specific values for the
     * parameters.
     *
     * @param pSelector the P-Selector (presentation layer address)
     * @param sSelector the S-Selector (session layer address)
     * @param tSelector the T-Selector (ISO transport layer address)
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

        NativeLibTase2.INSTANCE.Tase2_Server_setLocalAddresses(self, pSel, sSel, tSel);
    }

    /**
     * Set the timeout for select operations (device model)
     *
     * @param timeoutInMs select timeout in milliseconds
     */
    public void setSelectTimeout(int timeoutInMs) {
        NativeLibTase2.INSTANCE.Tase2_Server_setSelectTimeout(self, timeoutInMs);
    }

    /**
     * Set the timeout for critical report confirmations
     *
     * @param timeoutInMs timeout in milliseconds
     */
    public void setCriticalReportTimeout(int timeoutInMs) {
        NativeLibTase2.INSTANCE.Tase2_Server_setCriticalReportTimeout(self, timeoutInMs);
    }

    /**
     * Set the maximum event queue size for DS transfer sets
     * 
     * Use this function to limit the memory required to buffer events.
     * 
     * @param maxSize a positive number to limit the event queue size or -1
     *                (unlimited size)
     */
    public void SetMaxDSTSEventQueueSize(int maxSize) {
        NativeLibTase2.INSTANCE.Tase2_Server_setMaxDSTSEventQueueSize(self, maxSize);
    }

    /**
     * Load bilateral table (BLT) into the server (part of the server configuration
     * phase)
     *
     * @param blt a BLT instance
     */
    public void addBilateralTable(BilateralTable blt) {
        bltList.add(blt);
        NativeLibTase2.INSTANCE.Tase2_Server_addBilateralTable(self, blt.self);
    }

    /**
     * Set own identity for the identity service
     *
     * @param vendor   the vendor name of the TASE.2 application
     * @param model    the model name of the TASE.2 application
     * @param revision the revision string of the TASE.2 application
     */
    public void setIdentity(String vendor, String model, String revision) {
        NativeLibTase2.INSTANCE.Tase2_Server_setIdentity(self, vendor, model, revision);
    }

    /**
     * Set the client connection handler that is called when a client connects or
     * disconnects
     *
     * @param handler   the callback handler implementation.
     * @param parameter user provided parameter that is passed to the callback
     *                  handler
     */
    public void setClientConnectionHandler(ClientConnectionHandler handler, Object parameter) {
        clientConnectionHandler = handler;
        clientConnectionHandlerParameter = parameter;
    }

    /**
     * Set the callback handler for device operations (select, operate, set tag)
     *
     * @param handler   the callback handler implementation
     * @param parameter user provided parameter that is passed to the callback
     *                  handler
     */
    public void setDeviceOperationHandler(DeviceOperationHandler handler, Object parameter) {
        deviceOperationHandler = handler;
        deviceOperationHandlerParameter = parameter;
    }

    /**
     * Set the {@link com.libtase2.server.WriteDataHandler WriteDataHandler}
     * callback handler that is called when a data point is written
     *
     * A data point can be either a {@link com.libtase2.server.IndicationPoint
     * IndicationPoint} or a {@link com.libtase2.server.ProtectionEquipment
     * ProtectionEquipment} instance
     *
     * @param handler   the callback handler interface
     * @param parameter user provided parameter that is passed to the callback
     *                  handler
     */
    public void setWriteDataHandler(WriteDataHandler handler, Object parameter) {
        writeDataHandler = handler;
        writeDataHandlerParameter = parameter;
    }

    /**
     * Install a handler that is called when a transfer set report (marked as
     * critical) is not confirmed by the client or confirmed negative
     *
     * @param handler   the callback handler interface
     * @param parameter user provided parameter that is passed to the callback
     *                  handler
     */
    public void setTSCriticalNotConfirmedHandler(TSCriticalNotConfirmedHandler handler, Object parameter) {
        criticalReportNotConfirmedHandler = handler;
        criticalReportNotConfirmedHandlerParameter = parameter;
    }

    /**
     * Install a callback handler that is called whenever the DS transfer set is
     * updated (enabled/disabled)
     * 
     * @param handler   the callback handler interface
     * @param parameter user provided parameter that is passed to the callback
     *                  handler
     */
    public void setDSTransferSetUpdateHandler(DSTransferSetUpdateHandler handler, Object parameter) {
        dsTransferSetUpdateHandler = handler;
        dsTransferSetUpdateHandlerParameter = parameter;
    }

    public void setDataSetEventHandler(DataSetEventHandler handler, Object parameter) {
        dataSetEventHandler = handler;
        dataSetEventHandlerParameter = parameter;
    }

    public void setDSTransferSetReportSentHandler(DSTransferSetReportSentHandler handler, Object parameter) {
        dsTransferSetReportSentHandler = handler;
        dsTransferSetReportSentHandlerParameter = parameter;
    }

    public void setIMTransferSetEventHandler(IMTransferSetEventHandler handler, Object parameter) {
        imTransferSetEventHandler = handler;
        imTransferSetEventHandlerParameter = parameter;
    }

    /**
     * Manually trigger a DSTS report transmission of the given transfer set
     *
     * This function can be used to trigger data set transfer set reports with the
     * reasons "operator-requested" and "other-external-event". The reports will
     * only be sent when the transfer set it enabled.
     *
     * @param transferSet the transfer set to report/transmit
     * @param reason      the reason for the report (should be
     *                    ReportReason.OPERATOR_REQUESTED or
     *                    ReportReason.OTHER_EXTERNAL_EVENT)
     * @param eventCode   the event code in case the reason is
     *                    ReportReason.OTHER_EXTERNAL_EVENT
     *
     * @return true, when report has been sent, false otherwise (e.g. when client is
     *         not connected and/or TS not enabled)
     */
    public void requestDSTSReport(DSTransferSet transferSet, ReportReason reason, short eventCode) {
        NativeLibTase2.INSTANCE.Tase2_Server_requestReport(self, transferSet.self, reason.getValue(), eventCode);
    }

    /**
     * Send information message to all clients that have the IM transfer set enabled
     * and BLT matches the information object
     *
     * NOTE: when no BLT is defined the messages will be sent to all clients.
     *
     * @param domain use domain to send information object, null for VCC scope
     *               information message
     */
    public void sendInformationMessage(Domain domain, int infoRef, int localRef, int msgId, String message) {
        byte[] buffer = message.getBytes();

        NativeLibTase2.INSTANCE.Tase2_Server_sendInformationMessage(self, domain.self, infoRef, localRef, msgId,
                buffer.length, buffer);
    }

    /**
     * Starts the server (will start a background thread handling incoming
     * connections)
     *
     * NOTE: This function calls {@link com.libtase2.common.Endpoint#connect()
     * Endpoint.connect}. If multiple roles are connected to the endpoint the call
     * to the Endpoint.connect method is required only once.
     */
    public void start() {
        NativeLibTase2.INSTANCE.Tase2_Server_start(self);
    }

    /**
     * Stops the server
     *
     * NOTE: This function calls {@link com.libtase2.common.Endpoint#disconnect()
     * Endpoint.disconnect}. If multiple roles are connected to the endpoint they
     * will all be disconnected!
     */
    public void stop() {
        NativeLibTase2.INSTANCE.Tase2_Server_stop(self);
    }

    /**
     * Checks if the server is running
     *
     * Can be used to check if start has been successful
     *
     * @return true, if server is running, otherwise false
     */
    public boolean isRunning() {
        return (NativeLibTase2.INSTANCE.Tase2_Server_isRunning(self) != 0);
    }

    /**
     * Update the online available value with the local value of the data point
     *
     * NOTE: This function may trigger server actions (like sending DS transfer set
     * reports)
     *
     * @param dataPoint the data point instance to update
     */
    public void updateOnlineValue(DataPoint dataPoint) {
        NativeLibTase2.INSTANCE.Tase2_Server_updateOnlineValue(self, dataPoint.self);
    }

    /**
     * Update the online available value with the local value of the data point
     * (don't trigger transfer sets)
     *
     * This function can be used to implement application specific limit or deadband
     * handling for analogue indication points. Call this function when analogue
     * value change doesn't violate a limit or deadband condition. In the case the
     * new analogue value violates such a condition, then call
     * {@link #updateOnlineValue(DataPoint) updateOnlineValue} instead.
     *
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     *
     * @param self      Tase2_Server instance
     * @param dataPoint the data point instance to update
     */
    public void updateOnlineValueNoTrigger(DataPoint dataPoint) {
        NativeLibTase2.INSTANCE.Tase2_Server_updateOnlineValueNoTrigger(self, dataPoint.self);
    }

    /**
     * Update the local value by the online available value of the data point
     *
     * @param dataPoint the data point instance to update
     */
    public void updateByOnlineValue(DataPoint dataPoint) {
        NativeLibTase2.INSTANCE.Tase2_Server_updateByOnlineValue(self, dataPoint.self);
    }

    /**
     * Release all resource allocated by the Client instance
     * 
     * <p>
     * NOTE: Do not use the object after calling dispose!
     * </p>
     */
    public void dispose() {
        if (self != null) {

            if (self != Pointer.NULL)
                NativeLibTase2.INSTANCE.Tase2_Server_destroy(self);

            self = null;

            tlsConfiguration = null;
        }
    }

    @Override
    protected void finalize() {
        dispose();
    }
}
