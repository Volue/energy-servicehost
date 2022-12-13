using System;
using System.Runtime.InteropServices;

using TASE2.Library.Common;
using System.Text;
using System.Collections.Generic;
using libtase2.NET.server;

namespace TASE2
{
    namespace Library
    {

        namespace Server
        {
            /// <summary>
            /// Operate value for devices (\ref ControlPoint)
            /// </summary>
            [StructLayout(LayoutKind.Explicit)]
            public struct OperateValue
            {
                /// <summary>
                /// Value for commands
                /// </summary>
                [FieldOffset(0)]
                public short commandValue;

                /// <summary>
                /// Value for discrete set points
                /// </summary>
                [FieldOffset(0)]
                public short discreteValue;

                /// <summary>
                /// Value for analog (float) set points
                /// </summary>
                [FieldOffset(0)]
                public float realValue;
            }

            /// <summary>
            /// Callback hander result values
            /// </summary>
            public enum HandlerResult
            {
                /** Operation has been successful */
                SUCCESS = -1,

                /** Operation cannot be executed because of an hardware fault */
                HARDWARE_FAULT = 1,

                /** The requested resource is temporarily unavailable */
                TEMPORARILY_UNAVAILABLE = 2,

                /** The access to the resource is denied because of insufficient user privileges */
                OBJECT_ACCESS_DENIED = 3,

                /** Operation is rejected because of an invalid parameter value */
                OBJECT_VALUE_INVALID = 11
            }

            /// <summary>
            /// Client connection handler for connect/disconnect events
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="clientAddress">client address (IP address and port number)</param>
            /// <param name="blt">BLT assiciated with the client, or null when BLTs are not used</param>
            /// <param name="connect">If true, client is connected, false when client disconnected</param>
            public delegate void ClientConnectionHandler(object parameter, string clientAddress, BilateralTable blt, bool connect);

            /// <summary>
            /// Callback handler for device (control point) select event
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="controlPoint">the target device of the event</param>
            public delegate HandlerResult SelectHandler(object parameter, ControlPoint controlPoint);

            /// <summary>
            /// Callback handler for device (control point) operate event
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="controlPoint">the target device of the event</param>
            /// <param name="value">the value of the command or set point</param>
            public delegate HandlerResult OperateHandler(object parameter, ControlPoint controlPoint, OperateValue value);

            /// <summary>
            /// Callback handler for device (control point) set tag event
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="controlPoint">the target device of the event</param>
            /// <param name="value">the tag value</param>
            /// <param name="reason">the reason message for the tag</param>
            public delegate HandlerResult SetTagHandler(object parameter, ControlPoint controlPoint, TagValue value, string reason);

            /// <summary>
            /// Callback handler for indication point write events
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="dataPoint">the target data point of the event</param>
            /// <param name="value">the data point value</param>
            public delegate HandlerResult WriteDataHandler(object parameter, DataPoint dataPoint, PointValue value);

            /// <summary>
            /// Callback handler for reports that are marked as "critical"
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="transferSet">the transfer set that caused the report</param>
            /// <param name="timeSent">the timestamp when the report was sent (milliseconds since epoch)</param>
            /// <param name="nack">If true, the report was negatively acknowledged</param>
            public delegate HandlerResult TSCriticalNotConfirmedHandler(object parameter, TransferSet transferSet, UInt64 timeSent, bool nack);

            /// <summary>
            /// Callback handler for DS transfer set state changes (called when transfer set is enabled/disabled)
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="transferSet">the transfer set that caused the report</param>
            /// <param name="enabled">true, is enabled, false otherwise</param>
            public delegate void DSTransferSetStateChangedHandler(object parameter, DSTransferSet transferSet, bool enabled);


            /// <summary>
            /// Callback handler for DSTS updates (enabled/disabled)
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="peer">the client that caused the update</param>
            /// <param name="clientBlt">the BLT associated with the client</param>
            /// <param name="transferSet">the transfer set sending the report</param>
            /// <param name="isEnabled">isEnabled true when the state is enabled, false otherwise</param>
            public delegate void DSTransferSetUpdateHandler(object parameter, EndpointConnection peer, BilateralTable clientBlt, DSTransferSet transferSet, bool isEnabled);

            /// <summary>
            /// Callback handler that is called when a data set is created or deleted
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="create">when true, the data set is created, when false it is deleted</param>
            /// <param name="peer">the client that caused the update</param>
            /// <param name="clientBlt">the BLT associated with the client</param>
            /// <param name="domain">the domain of the data set</param>
            /// <param name="dataSetName">the name of the created or deleted data set</param>
            /// <param name="dataPoints">the data points of the created or deleted data set</param>
            public delegate void DataSetEventHandler(object parameter, bool create, EndpointConnection peer, BilateralTable clientBlt, Domain domain, string dataSetName, List<DataPoint> dataPoints);

            /// <summary>
            /// Callback handler for sent DSTS reports
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="peer">the client that caused the update</param>
            /// <param name="clientBlt">the BLT associated with the client</param>
            /// <param name="transferSet">the transfer set sending the report</param>
            /// <param name="sendData">list of sent data points</param>
            /// <param name="reason">Reason code of the sent report</param>
            public delegate void DSTransferSetReportSentHandler(object parameter, EndpointConnection peer, BilateralTable clientBlt, DSTransferSet transferSet, List<SentPointValue> sendData, ReportReason reason);

            /// <summary>
            /// Callback handler for IM transfer set events (enabled/disabled)
            /// </summary>
            /// <param name="parameter">User provided context parameter</param>
            /// <param name="peer">the client that caused the update</param>
            /// <param name="clientBlt">the BLT associated with the client</param>
            /// <param name="enabled">true when the state is enabled, false otherwise</param>
            public delegate void IMTransferSetEventHandler(object parameter, EndpointConnection peer, BilateralTable clientBlt, bool enabled);

            /// <summary>
            /// Represents a TASE.2/ICCP server instance
            /// </summary>
            public class Server : IDisposable
            {
                //
                // LinkedList
                //
                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                static extern IntPtr LinkedList_getNext(IntPtr self);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                static extern IntPtr LinkedList_getData(IntPtr self);

                //
                // Tase2_SentPointValue
                //
                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                static extern IntPtr Tase2_SentPointValue_getDataPoint(IntPtr self);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                static extern IntPtr Tase2_SentPointValue_getPointValue(IntPtr self);


                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern IntPtr Tase2_Server_create(IntPtr dataModel, IntPtr tlsConfiguration);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern IntPtr Tase2_Server_createEx(IntPtr dataModel, IntPtr endpoint);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_start(IntPtr self);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_stop(IntPtr self);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                [return: MarshalAs(UnmanagedType.Bool)]
                private static extern bool Tase2_Server_isRunning(IntPtr self);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setLocalIpAddress(IntPtr self, string localIpAddress);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_destroy(IntPtr self);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setIdentity(IntPtr self, string vendor, string model, string revision);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                [return: MarshalAs(UnmanagedType.Bool)]
                private static extern bool Tase2_Server_requestReport(IntPtr self, IntPtr transferSet, int reason, Int16 eventCode);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setSelectTimeout(IntPtr self, int timeoutInMs);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setTcpPort(IntPtr self, int tcpPort);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setSupportedFeatures(IntPtr self, int value);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setCriticalReportTimeout(IntPtr self, int timeoutInMs);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_addBilateralTable(IntPtr self, IntPtr blt);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_updateOnlineValue(IntPtr self, IntPtr dataPoint);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_updateOnlineValueNoTrigger(IntPtr self, IntPtr dataPoint);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_updateByOnlineValue(IntPtr self, IntPtr dataPoint);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern Statistics Tase2_Server_getStatistics(IntPtr self);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate void InternalClientConnectionHandler(IntPtr parameter, string peerAddress, IntPtr bltPtr, [MarshalAs(UnmanagedType.I1)] bool connect);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setClientConnectionHandler(IntPtr self, InternalClientConnectionHandler HandlerResult, IntPtr parameter);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate int InternalSelectHandler(IntPtr parameter, IntPtr controlPoint);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setSelectHandler(IntPtr self, InternalSelectHandler handler, IntPtr parameter);


                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate int InternalOperateHandler(IntPtr parameter, IntPtr controlPoint, OperateValue value);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setOperateHandler(IntPtr self, InternalOperateHandler handler, IntPtr parameter);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate int InternalSetTagHandler(IntPtr parameter, IntPtr controlPoint, int tagValue, string reason);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setSetTagHandler(IntPtr self, InternalSetTagHandler handler, IntPtr parameter);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate int InternalWriteDataHandler(IntPtr parameter, IntPtr domain, IntPtr dataPoint, IntPtr value);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setWriteDataHandler(IntPtr self, InternalWriteDataHandler handler, IntPtr parameter);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate void InternalTSCriticalNotConfirmedHandler(IntPtr self, IntPtr transferSet, UInt64 timeSent, [MarshalAs(UnmanagedType.I1)] bool nack);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setTSCriticalNotConfirmedHandler(IntPtr self, InternalTSCriticalNotConfirmedHandler handler, IntPtr parameter);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate void Tase2_DSTranferSetStateChangedHandler(IntPtr parameter, IntPtr transferSet, [MarshalAs(UnmanagedType.I1)] bool enabled);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setDSTransferSetStateChangedHandler(IntPtr self, Tase2_DSTranferSetStateChangedHandler handler, IntPtr parameter);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate void Tase2_DSTranferSetUpdatedHandler(IntPtr parameter, IntPtr peer, IntPtr clientBlt, IntPtr transferSet, [MarshalAs(UnmanagedType.I1)] bool enabled);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setDSTransferSetUpdateHandler(IntPtr self, Tase2_DSTranferSetUpdatedHandler handler, IntPtr parameter);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate void Tase2_DataSetEventHandler(IntPtr parameter, [MarshalAs(UnmanagedType.I1)] bool create, IntPtr peer, IntPtr clientBlt, IntPtr dataSetDomain, string dataSetName, IntPtr dataPoints);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setDataSetEventHandler(IntPtr self, Tase2_DataSetEventHandler handler, IntPtr parameter);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate void Tase2_DSTransferSetReportSentHandler(IntPtr parameter, IntPtr peer, IntPtr clientBlt, IntPtr transferSet, IntPtr dataPoints, int reason);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setDSTransferSetReportSentHandler(IntPtr self, Tase2_DSTransferSetReportSentHandler handler, IntPtr parameter);

                [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
                private delegate void Tase2_IMTransferSetEventHandler(IntPtr parameter, IntPtr peer, IntPtr clientBlt, [MarshalAs(UnmanagedType.I1)] bool enable);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setIMTransferSetEventHandler(IntPtr self, Tase2_IMTransferSetEventHandler handler, IntPtr parameter);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void
                Tase2_Server_sendInformationMessage(IntPtr self, IntPtr domain, Int32 infoRef, Int32 localRef, Int32 msgId, Int32 size, [Out] byte[] buffer);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setLocalApTitle(IntPtr self, string apTitle, int aeQualifier);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern void Tase2_Server_setMaxDSTSEventQueueSize(IntPtr self, int maxSize);

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
                private static extern void Tase2_Server_setLocalAddresses(IntPtr self, NativePSelector pSelector, NativeSSelector sSelector, NativeTSelector tSelector);

                [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
                private static extern IntPtr Tase2_Server_getEndpoint(IntPtr self);

                private DataModel dataModel;
                internal Endpoint endpoint = null;

                internal IntPtr self;

                private TLSConfiguration tlsConfiguration = null;

                /// <summary>
                /// Initializes a new instance of the <see cref="TASE2.Library.Server.Server"/> class
                /// and creates a new passive endpoint instance.
                /// </summary>
                /// <param name="dataModel">the server data model</param>
                /// <param name="tlsConfiguration">TLS configuration to use</param>
                public Server(DataModel dataModel, TLSConfiguration tlsConfiguration = null)
                {
                    IntPtr tlsConfigPtr = IntPtr.Zero;

                    this.tlsConfiguration = tlsConfiguration;

                    if (tlsConfiguration != null)
                        tlsConfigPtr = tlsConfiguration.Self;

                    self = Tase2_Server_create(dataModel.Self, tlsConfigPtr);
                    this.dataModel = dataModel;
                }

                /// <summary>
                /// Initializes a new instance of the <see cref="TASE2.Library.Server.Server"/> class using 
                /// an existing endpoint instance.
                /// </summary>
                /// <param name="dataModel">Data model.</param>
                /// <param name="endpoint">Endpoint.</param>
                public Server(DataModel dataModel, Endpoint endpoint)
                {
                    this.endpoint = endpoint;

                    self = Tase2_Server_createEx(dataModel.Self, endpoint.self);
                    this.dataModel = dataModel;
                }

                /// <summary>
                /// Releases all resource used by the <see cref="TASE2.Library.Server.Server"/> object.
                /// </summary>
                /// <remarks>Call <see cref="Dispose"/> when you are finished using the <see cref="TASE2.Library.Server.Server"/>. The
                /// <see cref="Dispose"/> method leaves the <see cref="TASE2.Library.Server.Server"/> in an unusable state. After
                /// calling <see cref="Dispose"/>, you must release all references to the <see cref="TASE2.Library.Server.Server"/>
                /// so the garbage collector can reclaim the memory that the <see cref="TASE2.Library.Server.Server"/> was occupying.</remarks>
                public void Dispose()
                {
                    lock (this)
                    {
                        if (self != IntPtr.Zero)
                        {
                            Tase2_Server_destroy(self);
                            self = IntPtr.Zero;
                        }
                    }
                }

                ~Server()
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
                            IntPtr endpointPtr = Tase2_Server_getEndpoint(self);

                            if (endpointPtr != IntPtr.Zero)
                            {
                                this.endpoint = Endpoint.LookupKnownEndpointOrCreate(endpointPtr);
                            }
                        }

                        return this.endpoint;
                    }
                }

                /// <summary>
                /// Set the select timeout for all SBO devices
                /// </summary>
                /// <value>The SBO select timeout in milliseconds</value>
                public int SelectTimeout
                {
                    set
                    {
                        Tase2_Server_setSelectTimeout(self, value);
                    }
                }

                /// <summary>
                /// Sets the TCP port to be used by the server (default value is 102 for non-TLS connection)
                /// </summary>
                /// <value>The tcp port.</value>
                public int TcpPort
                {
                    set
                    {
                        Tase2_Server_setTcpPort(self, value);
                    }
                }

                /// <summary>
                /// Sets the local ip address (restrict the server to listen to a specific local IP address)
                /// </summary>
                /// <value>The local ip address.</value>
                public String LocalIpAddress
                {
                    set
                    {
                        Tase2_Server_setLocalIpAddress(self, value);
                    }
                }

                /// <summary>
                /// Set the timeout for critical report confirmations
                /// </summary>
                /// <value>The critical report timeout in milliseconds</value>
                public int CriticalReportTimeout
                {
                    set
                    {
                        Tase2_Server_setCriticalReportTimeout(self, value);
                    }
                }

                /// <summary>
                /// Sets the server identity
                /// </summary>
                /// <param name="vendor">Vendor name</param>
                /// <param name="model">Model name</param>
                /// <param name="revision">Revision/Version</param>
                public void SetIdentity(string vendor, string model, string revision)
                {
                    Tase2_Server_setIdentity(self, vendor, model, revision);
                }

                /// <summary>
                /// Change the default value of the "Supported_Features" variable.
                /// Defaults are conformance blocks 1, 2, and 5
                /// </summary>
                /// <param name="features">supported features by this server</param>
                public void SetSupportedFeatures(SupportedFeatures features)
                {
                    Tase2_Server_setSupportedFeatures(self, (int)features);
                }

                /// <summary>
                /// Adds a bilateral table (BLT) to the server
                /// </summary>
                /// <param name="blt">the bilateral table to add</param>
                public void AddBilateralTable(BilateralTable blt)
                {
                    Tase2_Server_addBilateralTable(self, blt.Self);
                }


                private InternalClientConnectionHandler clientConnectionHandlerRef = null;
                private ClientConnectionHandler clientConnectionHandler = null;
                private object clientConnectionHandlerParameter = null;

                private void internalClientConnectionHandler(IntPtr parameter, string peerAddress, IntPtr bltPtr, bool connect)
                {
                    if (clientConnectionHandler != null)
                    {

                        BilateralTable blt = null;

                        if (bltPtr != IntPtr.Zero)
                            blt = BilateralTable.GetInstance(bltPtr);

                        clientConnectionHandler(clientConnectionHandlerParameter, peerAddress, blt, connect);
                    }
                }

                public void SetClientConnectionHandler(ClientConnectionHandler handler, object parameter)
                {
                    if (clientConnectionHandlerRef == null)
                        clientConnectionHandlerRef = new InternalClientConnectionHandler(internalClientConnectionHandler);

                    Tase2_Server_setClientConnectionHandler(self, clientConnectionHandlerRef, IntPtr.Zero);

                    this.clientConnectionHandler = handler;
                    this.clientConnectionHandlerParameter = parameter;
                }

                private InternalSelectHandler selectHandlerRef = null;
                private SelectHandler selectHandler = null;
                private object selectHandlerParameter = null;

                private int internalSelectHandler(IntPtr parameter, IntPtr controlPointRef)
                {
                    ControlPoint controlPoint = dataModel.LookupControlPoint(controlPointRef);

                    if (selectHandler != null)
                        return (int)selectHandler(selectHandlerParameter, controlPoint);

                    return (int)HandlerResult.OBJECT_ACCESS_DENIED;
                }

                /// <summary>
                /// Sets the select handler for devices. Used to handle select commands from clients
                /// </summary>
                /// <param name="handler">callback handler function</param>
                /// <param name="parameter">parameter that will be passed to the callback function, or null</param>
                public void SetSelectHandler(SelectHandler handler, object parameter)
                {
                    if (selectHandlerRef == null)
                        selectHandlerRef = new InternalSelectHandler(internalSelectHandler);

                    Tase2_Server_setSelectHandler(self, selectHandlerRef, IntPtr.Zero);

                    this.selectHandler = handler;
                    this.selectHandlerParameter = parameter;
                }


                private InternalOperateHandler operateHandlerRef = null;
                private OperateHandler operateHandler = null;
                private object operateHandlerParameter = null;

                private int internalOperateHandler(IntPtr parameter, IntPtr controlPointRef, OperateValue value)
                {
                    ControlPoint controlPoint = dataModel.LookupControlPoint(controlPointRef);

                    if (operateHandler != null)
                        return (int)operateHandler(operateHandlerParameter, controlPoint, value);

                    return (int)HandlerResult.OBJECT_ACCESS_DENIED;
                }

                /// <summary>
                /// Sets the operate handler for devices. Used to handle operate commands from clients
                /// </summary>
                /// <param name="handler">callback handler function</param>
                /// <param name="parameter">parameter that will be passed to the callback function, or null</param>
                public void SetOperateHandler(OperateHandler handler, object parameter)
                {
                    if (operateHandlerRef == null)
                        operateHandlerRef = new InternalOperateHandler(internalOperateHandler);

                    Tase2_Server_setOperateHandler(self, operateHandlerRef, IntPtr.Zero);

                    this.operateHandler = handler;
                    this.operateHandlerParameter = parameter;
                }


                private InternalSetTagHandler setTagHandlerRef = null;
                private SetTagHandler setTagHandler = null;
                private object setTagHandlerParameter = null;

                private int internalSetTagHandler(IntPtr parameter, IntPtr controlPointRef, int tagValue, string reason)
                {
                    ControlPoint controlPoint = dataModel.LookupControlPoint(controlPointRef);

                    if (setTagHandler != null)
                        return (int)setTagHandler(setTagHandlerParameter, controlPoint, (TagValue)tagValue, reason);

                    return (int)HandlerResult.OBJECT_ACCESS_DENIED;
                }

                /// <summary>
                /// Sets the set tag handler for devices. Used to handle set tag commands from clients
                /// </summary>
                /// <param name="handler">callback handler function</param>
                /// <param name="parameter">parameter that will be passed to the callback function, or null</param>
                public void SetSetTagHandler(SetTagHandler handler, object parameter)
                {
                    if (setTagHandlerRef == null)
                        setTagHandlerRef = new InternalSetTagHandler(internalSetTagHandler);

                    Tase2_Server_setSetTagHandler(self, setTagHandlerRef, IntPtr.Zero);

                    this.setTagHandler = handler;
                    this.setTagHandlerParameter = parameter;
                }

                private InternalWriteDataHandler writeDataHandlerRef = null;
                private WriteDataHandler writeDataHandler = null;
                private object writeDataHandlerParameter = null;

                private int internalWriteDataHandler(IntPtr parameter, IntPtr domain, IntPtr dataPointRef, IntPtr pointValue)
                {
                    DataPoint dataPoint = dataModel.LookupDataPoint(dataPointRef);

                    if (dataPoint != null)
                    {
                        PointValue value = new PointValue(pointValue, true);

                        if (writeDataHandler != null)
                            return (int)writeDataHandler(writeDataHandlerParameter, dataPoint, value);
                    }

                    return (int)HandlerResult.OBJECT_ACCESS_DENIED;
                }

                /// <summary>
                /// Sets the write handler for indication points. Used to handle write requests from clients
                /// </summary>
                /// <param name="handler">callback handler function</param>
                /// <param name="parameter">parameter that will be passed to the callback function, or null</param>
                public void SetWriteDataHandler(WriteDataHandler handler, object parameter)
                {
                    if (writeDataHandlerRef == null)
                        writeDataHandlerRef = new InternalWriteDataHandler(internalWriteDataHandler);

                    Tase2_Server_setWriteDataHandler(self, writeDataHandlerRef, IntPtr.Zero);

                    this.writeDataHandler = handler;
                    this.writeDataHandlerParameter = parameter;
                }


                private InternalTSCriticalNotConfirmedHandler notConfirmedHandlerRef = null;
                private TSCriticalNotConfirmedHandler criticalConfirmedHandler = null;
                private object criticalConfirmedHandlerParameter = null;

                private void internalCriticalConfirmedHandler(IntPtr self, IntPtr transferSetPtr, UInt64 timeSent, bool nack)
                {
                    TransferSet transferSet = dataModel.LookupTransferSet(transferSetPtr);

                    if (criticalConfirmedHandler != null)
                        criticalConfirmedHandler(criticalConfirmedHandlerParameter, transferSet, timeSent, nack);
                }

                /// <summary>
                /// Sets a callback handler that is called when reports marked as critical are not confirmed.
                /// </summary>
                /// <param name="handler">callback handler function</param>
                /// <param name="parameter">parameter that will be passed to the callback function, or null</param>
                public void SetTSCriticalNotConfirmedHandler(TSCriticalNotConfirmedHandler handler, object parameter)
                {
                    if (notConfirmedHandlerRef == null)
                        notConfirmedHandlerRef = new InternalTSCriticalNotConfirmedHandler(internalCriticalConfirmedHandler);

                    Tase2_Server_setTSCriticalNotConfirmedHandler(self, notConfirmedHandlerRef, IntPtr.Zero);

                    this.criticalConfirmedHandler = handler;
                    this.criticalConfirmedHandlerParameter = parameter;
                }

                private Tase2_DSTranferSetStateChangedHandler dsTransferSetStateChangedHandlerRef = null;
                private DSTransferSetStateChangedHandler dsTransferSetStateChangedHandler = null;
                private object dsTransferSetStateChangedHandlerParameter = null;

                private void internalDSTransferSetStateChangedHandler(IntPtr parameter, IntPtr transerSetPtr, bool enable)
                {
                    TransferSet transferSet = dataModel.LookupTransferSet(transerSetPtr);

                    if (transferSet != null)
                    {
                        if (dsTransferSetStateChangedHandler != null)
                            dsTransferSetStateChangedHandler(dsTransferSetStateChangedHandlerParameter, transferSet as DSTransferSet, enable);
                    }
                }

                /// <summary>
                /// Sets a callback handler that is called when DS transfer set are enabled or disabled
                /// </summary>
                /// <param name="handler">callback handler function</param>
                /// <param name="parameter">parameter that will be passed to the callback function, or null</param>
                public void SetDSTransferSetStateChangedHandler(DSTransferSetStateChangedHandler handler, object parameter)
                {
                    if (dsTransferSetStateChangedHandlerRef == null)
                        dsTransferSetStateChangedHandlerRef = new Tase2_DSTranferSetStateChangedHandler(internalDSTransferSetStateChangedHandler);

                    Tase2_Server_setDSTransferSetStateChangedHandler(self, dsTransferSetStateChangedHandlerRef, IntPtr.Zero);

                    this.dsTransferSetStateChangedHandler = handler;
                    this.dsTransferSetStateChangedHandlerParameter = parameter;
                }

                private Tase2_DSTranferSetUpdatedHandler dsTransferSetUpdateHandlerRef = null;
                private DSTransferSetUpdateHandler dsTransferSetUpdateHandler = null;
                private object dsTransferSetUpdateHandlerParameter = null;

                private void InternalDSTransferSetUpdateHandler(IntPtr parameter, IntPtr peerPtr, IntPtr clientBltPtr, IntPtr transferSetPtr, bool enable)
                {
                    if (dsTransferSetUpdateHandler != null)
                    {
                        TransferSet transferSet = dataModel.LookupTransferSet(transferSetPtr);

                        if (transferSet != null && transferSet is DSTransferSet)
                        {
                            BilateralTable clientBlt = null;

                            if (clientBltPtr != IntPtr.Zero)
                                clientBlt = BilateralTable.GetInstance(clientBltPtr);

                            EndpointConnection peer = Endpoint.LookupEndpointConnection(peerPtr);

                            dsTransferSetUpdateHandler(dsTransferSetUpdateHandlerParameter, peer, clientBlt, transferSet as DSTransferSet, enable);

                        }
                    }
                }

                /// <summary>
                /// Sets a callback handler that is called whenever the DS transfer set is updated (enabled/disabled)
                /// </summary>
                /// <param name="handler">the callback handler function</param>
                /// <param name="parameter">user provided parameter that is passed to the callback handler</param>
                public void SetDSTransferSetUpdateHandler(DSTransferSetUpdateHandler handler, object parameter)
                {
                    if (dsTransferSetUpdateHandlerRef == null)
                    {
                        dsTransferSetUpdateHandlerRef = new Tase2_DSTranferSetUpdatedHandler(InternalDSTransferSetUpdateHandler);

                        Tase2_Server_setDSTransferSetUpdateHandler(self, dsTransferSetUpdateHandlerRef, IntPtr.Zero);
                    }

                    this.dsTransferSetUpdateHandler = handler;
                    this.dsTransferSetUpdateHandlerParameter = parameter;
                }

                private Tase2_DataSetEventHandler dataSetEventHandlerRef = null;
                private DataSetEventHandler dataSetEventHandler = null;
                private object dataSetEventHandlerParameter = null;

                private void InternalDataSetEventHandler(IntPtr parameter, bool create, IntPtr peerPtr, IntPtr clientBltPtr, IntPtr dataSetDomain, string dataSetName, IntPtr dataPoints)
                {
                    if (dataSetEventHandler != null)
                    {
                        BilateralTable clientBlt = null;

                        if (clientBltPtr != IntPtr.Zero)
                            clientBlt = BilateralTable.GetInstance(clientBltPtr);

                        EndpointConnection peer = endpoint.LookupEndpointConnection(peerPtr);

                        Domain dsDomain = dataModel.LookupDomain(dataSetDomain);

                        List<DataPoint> dsDataPoints = new List<DataPoint>();

                        IntPtr element = LinkedList_getNext(dataPoints);

                        while (element != IntPtr.Zero)
                        {
                            IntPtr dataPointPtr = LinkedList_getData(element);

                            DataPoint dataPoint = dataModel.LookupDataPoint(dataPointPtr);

                            if (dataPoint != null)
                            {
                                dsDataPoints.Add(dataPoint);
                            }

                            element = LinkedList_getNext(element);
                        }

                        dataSetEventHandler(dataSetEventHandlerParameter, create, peer, clientBlt, dsDomain, dataSetName, dsDataPoints);
                    }
                }

                /// <summary>
                /// Install a callback handler that is called whenever a dynamic data set is created or deleted by a client
                /// </summary>
                /// <param name="handler">the callback handler function</param>
                /// <param name="parameter">user provided parameter that is passed to the callback handler</param>
                public void SetDataSetEventHandler(DataSetEventHandler handler, object parameter)
                {
                    if (dataSetEventHandlerRef == null)
                    {
                        dataSetEventHandlerRef = new Tase2_DataSetEventHandler(InternalDataSetEventHandler);

                        Tase2_Server_setDataSetEventHandler(self, dataSetEventHandlerRef, IntPtr.Zero);
                    }

                    this.dataSetEventHandler = handler;
                    this.dataSetEventHandlerParameter = parameter;
                }

                private Tase2_DSTransferSetReportSentHandler dsTransferSetReportSentHandlerRef = null;
                private DSTransferSetReportSentHandler dSTransferSetReportSentHandler = null;
                private object dSTransferSetReportSentHandlerParameter = null;

                private void InternalDSTransferSetReportSentHandler(IntPtr parameter, IntPtr peerPtr, IntPtr clientBltPtr, IntPtr transferSetPtr, IntPtr dataPoints, int reason)
                {
                    if (dSTransferSetReportSentHandler != null)
                    {
                        TransferSet transferSet = dataModel.LookupTransferSet(transferSetPtr);

                        if (transferSet != null && transferSet is DSTransferSet)
                        {

                            BilateralTable clientBlt = null;

                            if (clientBltPtr != IntPtr.Zero)
                                clientBlt = BilateralTable.GetInstance(clientBltPtr);

                            EndpointConnection peer = endpoint.LookupEndpointConnection(peerPtr);

                            List<SentPointValue> sentDataPoints = new List<SentPointValue>();

                            IntPtr element = LinkedList_getNext(dataPoints);

                            while (element != IntPtr.Zero)
                            {
                                IntPtr sentPointValuePtr = LinkedList_getData(element);

                                IntPtr dataPointPtr = Tase2_SentPointValue_getDataPoint(sentPointValuePtr);
                                IntPtr pointValuePtr = Tase2_SentPointValue_getPointValue(sentPointValuePtr);

                                if ((dataPointPtr != IntPtr.Zero) && (pointValuePtr != IntPtr.Zero))
                                {
                                    DataPoint dataPoint = dataModel.LookupDataPoint(dataPointPtr);
                                    PointValue pointValue = new PointValue(pointValuePtr, true);

                                    SentPointValue sentPointValue = new SentPointValue(dataPoint, pointValue);

                                    sentDataPoints.Add(sentPointValue);
                                }

                                element = LinkedList_getNext(element);
                            }

                            dSTransferSetReportSentHandler(dSTransferSetReportSentHandlerParameter, peer, clientBlt, transferSet as DSTransferSet, sentDataPoints, (ReportReason)reason);
                        }

                    }
                }

                /// <summary>
                /// Install a callback handler that is called whenever a transfer set report is sent
                /// </summary>
                /// <param name="handler">the callback handler function</param>
                /// <param name="parameter">user provided parameter that is passed to the callback handler</param>
                public void SetDSTransferSetReportSentHandler(DSTransferSetReportSentHandler handler, object parameter)
                {
                    this.dSTransferSetReportSentHandler = handler;
                    this.dSTransferSetReportSentHandlerParameter = parameter;

                    if (dsTransferSetReportSentHandlerRef == null)
                    {
                        dsTransferSetReportSentHandlerRef = new Tase2_DSTransferSetReportSentHandler(InternalDSTransferSetReportSentHandler);

                        Tase2_Server_setDSTransferSetReportSentHandler(self, dsTransferSetReportSentHandlerRef, IntPtr.Zero);
                    }
                }

                private Tase2_IMTransferSetEventHandler imTransferSetEventHandlerRef = null;
                private IMTransferSetEventHandler imTransferSetEventHandler = null;
                private object imTransferSetEventHandlerParameter = null;

                private void InternalIMTransferSetEventHandler(IntPtr parameter, IntPtr peerPtr, IntPtr clientBltPtr, bool enable)
                {
                    if (imTransferSetEventHandler != null)
                    {
                        BilateralTable clientBlt = null;

                        if (clientBltPtr != IntPtr.Zero)
                            clientBlt = BilateralTable.GetInstance(clientBltPtr);

                        EndpointConnection peer = endpoint.LookupEndpointConnection(peerPtr);

                        imTransferSetEventHandler(imTransferSetEventHandlerParameter, peer, clientBlt, enable);
                    }
                }

                /// <summary>
                /// Install a callback handler that is called whenever the IM Transfer set is enabled or disabled
                /// </summary>
                /// <param name="handler">the callback handler function</param>
                /// <param name="parameter">user provided parameter that is passed to the callback handler</param>
                public void SetIMTransferSetEventHandler(IMTransferSetEventHandler handler, object parameter)
                {
                    this.imTransferSetEventHandler = handler;
                    this.imTransferSetEventHandlerParameter = parameter;

                    if (imTransferSetEventHandlerRef == null)
                    {
                        imTransferSetEventHandlerRef = new Tase2_IMTransferSetEventHandler(InternalIMTransferSetEventHandler);

                        Tase2_Server_setIMTransferSetEventHandler(self, imTransferSetEventHandlerRef, IntPtr.Zero);
                    }
                }

                /// <summary>
                /// Starts the server. Listen for, and handle client connections.
                /// </summary>
                public void Start()
                {
                    Tase2_Server_start(self);
                }

                /// <summary>
                /// Stops the server. Stops listening for new client connections and closes all open client connections.
                /// </summary>
                public void Stop()
                {
                    Tase2_Server_stop(self);
                }

                /// <summary>
                /// Determines whether the server is running (listening for TCP connections).
                /// </summary>
                /// If this method returns <c>false</c> after the \ref Start call this means that
                /// the Start method failed. The reason can be that the program requires special permissions
                /// to bind to ports (root permission on Linux to bind to some ports) or another server
                /// is already listening on the requested TCP port.
                /// <returns><c>true</c> if this instance is running; otherwise, <c>false</c>.</returns>
                public bool IsRunning()
                {
                    return Tase2_Server_isRunning(self);
                }

                /// <summary>
                /// Updates the online visible value of the data point by its local value
                /// </summary>
                /// <param name="dataPoint">the data point instance to update</param>
                public void UpdateOnlineValue(DataPoint dataPoint)
                {
                    Tase2_Server_updateOnlineValue(self, dataPoint.Self);
                }

                /// <summary>
                /// Update the online available value with the local value of the data point (don't trigger transfer sets)
                /// </summary>
                ///  This function can be used to implement application specific limit or deadband handling for analogue indication points.
                /// Call this function when analogue value change doesn't violate a limit or deadband condition. In the case
                /// the new analogue value violates such a condition, then call <see cref="UpdateOnlineValue"/> instead.
                /// <param name="dataPoint">the data point instance to update</param>
                public void UpdateOnlineValueNoTrigger(DataPoint dataPoint)
                {
                    Tase2_Server_updateOnlineValueNoTrigger(self, dataPoint.Self);
                }

                /// <summary>
                /// Updates the local value of the DataPoint instance by its online visible value
                /// </summary>
                /// <param name="dataPoint">Data point.</param>
                public void UpdateByOnlineValue(DataPoint dataPoint)
                {
                    Tase2_Server_updateByOnlineValue(self, dataPoint.Self);
                }

                /// <summary>
                /// Trigger to send a report (the reason should be OPERATOR_REQUESTED)
                /// </summary>
                /// <returns><c>true</c>, if report was sent, <c>false</c> otherwise.</returns>
                /// <param name="transferSet">TransferSet instance used to send the report</param>
                /// <param name="reason">reason of the report (should be OPERATOR_REQUESTED)</param>
                /// <param name="eventCode">Event code</param>
                public bool RequestReport(DSTransferSet transferSet, ReportReason reason, Int16 eventCode)
                {
                    return Tase2_Server_requestReport(self, transferSet.Self, (int)reason, eventCode);
                }

                /// <summary>
                /// Send information message to all clients that have the IM transfer set enabled and BLT matches the information object
                /// </summary>
                /// <param name="domain">domain instance or null for VCC scope</param>
                /// <param name="infoRef">information reference</param>
                /// <param name="localRef">local reference</param>
                /// <param name="msgId">message identifier.</param>
                /// <param name="size">message size (valid bytes in buffer)</param>
                /// <param name="buffer">buffer containing the message</param>
                public void SendInformationMessage(Domain domain, int infoRef, int localRef, int msgId, int size, byte[] buffer)
                {
                    IntPtr domainPtr;

                    if (domain != null)
                        domainPtr = domain.Self;
                    else
                        domainPtr = IntPtr.Zero;

                    if (size > buffer.Length)
                        return;

                    Tase2_Server_sendInformationMessage(self, domainPtr, infoRef, localRef, msgId, size, buffer);
                }

                /// <summary>
                /// Send information message to all clients that have the IM transfer set enabled and BLT matches the information object
                /// </summary>
                /// <param name="domain">domain instance or null for VCC scope</param>
                /// <param name="infoRef">information reference</param>
                /// <param name="localRef">local reference</param>
                /// <param name="msgId">message identifier.</param>
                /// <param name="buffer">the message content buffer</param>
                public void SendInformationMessage(Domain domain, int infoRef, int localRef, int msgId, byte[] buffer)
                {
                    SendInformationMessage(domain, infoRef, localRef, msgId, buffer.Length, buffer);
                }

                /// <summary>
                /// Send information message to all clients that have the IM transfer set enabled and BLT matches the information object
                /// </summary>
                /// <param name="domain">domain instance or null for VCC scope</param>
                /// <param name="infoRef">information reference</param>
                /// <param name="localRef">local reference</param>
                /// <param name="msgId">message identifier.</param>
                /// <param name="message">message content</param>
                public void SendInformationMessage(Domain domain, int infoRef, int localRef, int msgId, string message)
                {
                    byte[] buffer = Encoding.UTF8.GetBytes(message);

                    SendInformationMessage(domain, infoRef, localRef, msgId, buffer.Length, buffer);
                }

                /// <summary>
                /// Gets some statistic information.
                /// </summary>
                /// <returns>statistics</returns>
                public Statistics GetStatistics()
                {
                    return Tase2_Server_getStatistics(self);
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
                    Tase2_Server_setLocalApTitle(self, apTitle, aeQualifier);
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
                        throw new ArgumentOutOfRangeException(nameof(pSelector), "maximum size (16) exceeded");

                    NativePSelector nativePSelector;
                    nativePSelector.size = (byte)pSelector.Length;
                    nativePSelector.value = new byte[16];

                    for (int i = 0; i < pSelector.Length; i++)
                        nativePSelector.value[i] = pSelector[i];

                    if (tSelector.Length > 16)
                        throw new ArgumentOutOfRangeException(nameof(tSelector), "maximum size (16) exceeded");

                    NativeTSelector nativeTSelector;
                    nativeTSelector.size = (byte)tSelector.Length;
                    nativeTSelector.value = new byte[16];

                    for (int i = 0; i < tSelector.Length; i++)
                        nativeTSelector.value[i] = tSelector[i];

                    if (sSelector.Length > 16)
                        throw new ArgumentOutOfRangeException(nameof(sSelector), "maximum size (16) exceeded");

                    NativeSSelector nativeSSelector;
                    nativeSSelector.size = (byte)sSelector.Length;
                    nativeSSelector.value = new byte[16];

                    for (int i = 0; i < sSelector.Length; i++)
                        nativeSSelector.value[i] = sSelector[i];

                    Tase2_Server_setLocalAddresses(self, nativePSelector, nativeSSelector, nativeTSelector);
                }

                /// <summary>
                /// Set the maximum event queue size for DS transfer sets
                /// </summary>
                /// <remarks>
                /// Use this function to limit the memory required to buffer events.
                /// </remarks>
                /// <param name="maxSize">a positive number to limit the event queue size or -1 (unlimited size)</param>
                public void SetMaxDSTSEventQueueSize(int maxSize)
                {
                    Tase2_Server_setMaxDSTSEventQueueSize(self, maxSize);
                }

            }

        }

    }
}

