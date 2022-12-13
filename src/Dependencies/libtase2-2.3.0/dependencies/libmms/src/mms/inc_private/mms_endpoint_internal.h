/*
 *  mms_endpoint_internal.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_ENDPOINT_INTERNAL_H_
#define MMS_ENDPOINT_INTERNAL_H_

#include "mms_endpoint.h"
#include "iso_server.h"
#include "iso_client_connection.h"
#include "iso_connection_parameters.h"
#include "mms_common_internal.h"
#include "mms_client_internal.h"
#include "stack_config.h"
#include "hal_thread.h"

#ifndef DEBUG_MMS_ENDPOINT
#define DEBUG_MMS_ENDPOINT 0
#endif

#if (MMS_OBTAIN_FILE_SERVICE == 1)
#include "hal_filesystem.h"
#endif


#ifndef DEBUG_MMS_SERVER
#define DEBUG_MMS_SERVER 0
#endif

#ifndef MMS_STATUS_SERVICE
#define MMS_STATUS_SERVICE 1
#endif

#ifndef MMS_IDENTIFY_SERVICE
#define MMS_IDENTIFY_SERVICE 1
#endif

#ifndef MMS_FILE_SERVICE
#define MMS_FILE_SERVICE 1
#endif

#ifndef CONFIG_MMS_SERVER_MAX_GET_FILE_TASKS
#define CONFIG_MMS_SERVER_MAX_GET_FILE_TASKS 5
#endif


#if (MMS_OBTAIN_FILE_SERVICE == 1)

#define MMS_FILE_UPLOAD_STATE_NOT_USED 0
#define MMS_FILE_UPLOAD_STATE_READY 1
#define MMS_FILE_UPLOAD_STATE_FILE_OPEN_SENT 2

#define MMS_FILE_UPLOAD_STATE_SEND_FILE_READ 3
#define MMS_FILE_UPLOAD_STATE_FILE_READ_SENT 4

#define MMS_FILE_UPLOAD_STATE_SEND_FILE_CLOSE 5
#define MMS_FILE_UPLOAD_STATE_FILE_CLOSE_SENT 6

#define MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_SOURCE 8
#define MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_DESTINATION 9
#define MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_RESPONSE 10

typedef struct sMmsObtainFileTask* MmsObtainFileTask;

struct sMmsObtainFileTask {
    MmsEndpointConnection connection;
    uint32_t lastRequestInvokeId;
    uint32_t obtainFileRequestInvokeId;
    FileHandle fileHandle;
    char destinationFilename[256];
    uint64_t nextTimeout;
    int32_t frmsId;
    int state;
};

#endif /* (MMS_OBTAIN_FILE_SERVICE == 1) */

struct sMmsEndpoint
{
    bool passiveMode; /* mode: true = passive, false = active */
    TLSConfiguration tlsConfig;

    MmsEndpointLogHandler logHandler;
    void* logHandlerParameter;
    MmsLogLevel logLevel;

    LinkedList /* <MmsClient> */ mmsClients;

    IsoServer isoServer;

    LinkedList openConnections;
    Semaphore openConnectionsLock;

    Semaphore appTickLock;
    bool appTickEnabled;

    struct sIsoApplicationAddress localAppAddr;

    struct sIsoApplicationAddress defaultRemoteAppAddr;
    char* defaultRemoteHostname;
    int defaultRemoteTcpPort;

    IsoClientConnection isoClient;

    MmsConnectionParameters mmsConnectionParameters;

#if (CONFIG_MMS_THREADLESS_STACK == 0)
    /* handling thread for IsoClientConnection */
    Thread connectionHandlingThread;
    bool createThread;
    bool connectionThreadRunning;
#endif

    MmsEndpointState state;
#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore stateLock;
#endif

    ByteBuffer* transmitBuffer; /* global buffer for encoding reports, delayed responses... */

    uint32_t requestTimeout;
    uint32_t connectTimeout; /* TODO implement handling! */

    MmsAppTickHandler appTickHandler;
    void* appTickHandlerParameter;

    MmsEndpointStateChangedHandler stateChangedHandler;
    void* stateChangedHandlerParameter;

    LinkedList waitHandles;
    Semaphore waitHandlesLock;

    MmsConnectionHandler connectionHandler;
    void* connectionHandlerParameter;

    MmsInformationReportHandler reportHandler;
    void* reportHandlerParameter;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore reportHandlerMutex;
    Semaphore transmitBufferMutex;
#endif

#if (CONFIG_MMS_RAW_MESSAGE_LOGGING == 1)
    void* rawMmsMessageHandler;
    void* rawMmsMessageHandlerParameter;
#endif

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
    uint64_t statAplMessagesSent;
    uint64_t statAplMessagesRcvd;
    uint64_t statConfErrorPduRcvd;
    uint64_t statConfErrorPduSend;
    uint64_t statRejectPduRcvd;
    uint64_t statRejectPduSent;
    uint64_t statRequestTimeouts;
#endif

#if (MMS_IDENTIFY_SERVICE == 1)
    const char* vendorName;
    const char* modelName;
    const char* revision;
#endif

#if (MMS_OBTAIN_FILE_SERVICE == 1)
    MmsObtainFileHandler obtainFileHandler;
    void* obtainFileHandlerParameter;

    MmsGetFileCompleteHandler getFileCompleteHandler;
    void* getFileCompleteHandlerParameter;

    struct sMmsObtainFileTask fileUploadTasks[CONFIG_MMS_SERVER_MAX_GET_FILE_TASKS];
#endif

#if (MMS_FILE_SERVICE == 1)
    MmsFileAccessHandler fileAccessHandler;
    void* fileAccessHandlerParameter;
#endif

#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    char* filestoreBasepath;
#endif


};

struct sMmsOutstandingService
{
    uint8_t* mmsMsgBuffer;
    int bufPos;
    int mmsMsgSize;
    uint64_t timeout;
};

typedef struct sMmsOutstandingService* MmsOutstandingService;

struct sMmsEndpointConnection
{
    MmsEndpoint endpoint;

    /* peer addresses */
    char* peerIpAddress;

    /* TODO add access functions */
    int maxServOutstandingCalling; /* negotiated max. outstanding calling */
    int maxServOutstandingCalled; /* negotiated max. outstanding called */
    int dataStructureNestingLevel; /* negotiated max. data structure nesting level */
    uint32_t maxPduSize; /* local detail */
    uint8_t servicesSupported[11]; /* services supported by peer */

    /* ISO layer for passive endpoint (NULL if no passive endpoint) */
    IsoConnection isoConnection;

    /* ISO layer for active endpoint (NULL if no active endpoint) */
    IsoClientConnection isoClient;

    /* connection specific information for MMS server (NULL if no MMS server is associated with this endpoint) */
    MmsEndpointServerConnection serverConnection;

    /* connection specific information for MMS client (NULL if no MMS client is associated with this endpoint) */
    MmsEndpointClientConnection clientConnection;

    /* invoke ID for client side services (last used invoke ID for sent request messages */
    Semaphore lastRequestInvokeIdLock;
    uint32_t lastRequestInvokeId;

    /* management of outstanding client requests to remote servers */
    Semaphore outstandingCallsLock;
    MmsOutstandingCall outstandingCalls;

    /* management of outstanding service requests from remote clients */
    Semaphore outstandingServicesLock;
    MmsOutstandingService outstandingServices;
    int nextOutstandingService;
    int oldestOutstandingService;

#if (MMS_FILE_SERVICE == 1)
    int32_t nextFrsmId;
    MmsFileReadStateMachine frsms[CONFIG_MMS_MAX_NUMBER_OF_OPEN_FILES_PER_CONNECTION];
#endif
};

struct sMmsEndpointServerConnection
{
    MmsServer server;

#if (MMS_DYNAMIC_DATA_SETS == 1)
    LinkedList /*<MmsNamedVariableList>*/namedVariableLists; /* aa-specific named variable lists */
#endif
};

struct sMmsEndpointClientConnection
{
    MmsClient mmsClient;
};

struct sMmsClient
{
    MmsEndpoint endpoint;

    MmsEndpointConnection connection;

    /*
     * Protect access to connection to avoid null pointer dereference
     * when connection is closed or otherwise used.
     */
    Semaphore connectionLock;

    struct sIsoApplicationAddress peerAddress;
};


#if (MMS_OBTAIN_FILE_SERVICE == 0)
LIBMMS_INTERNAL MmsObtainFileTask
MmsEndpoint_getObtainFileTask(MmsServer self);

LIBMMS_INTERNAL void
MmsEndpoint_fileUploadTask(MmsServer self, MmsObtainFileTask task);
#endif

LIBMMS_INTERNAL bool
MmsEndpointServerConnection_addNamedVariableList(MmsEndpointServerConnection self, MmsNamedVariableList variableList);

LIBMMS_INTERNAL MmsNamedVariableList
MmsEndpointServerConnection_getNamedVariableList(MmsEndpointServerConnection self, const char* variableListName);

LIBMMS_INTERNAL LinkedList
MmsEndpointServerConnection_getNamedVariableLists(MmsEndpointServerConnection self);

LIBMMS_INTERNAL void
MmsEndpointServerConnection_deleteNamedVariableList(MmsEndpointServerConnection self, char* listName);

LIBMMS_INTERNAL uint32_t
MmsEndpointConnection_getNextInvokeId(MmsEndpointConnection self);

LIBMMS_INTERNAL MmsError
MmsEndpointConnection_sendAsyncRequest(MmsEndpointConnection self, uint32_t invokeId, ByteBuffer* message, eMmsOutstandingCallType type,
        void* userCallback, void* userParameter, void* internalParameter);

LIBMMS_INTERNAL MmsError
MmsEndpointConnection_sendAsyncRequestInHandlerMode(MmsEndpointConnection self, uint32_t invokeId, ByteBuffer* message, eMmsOutstandingCallType type,
        void* userCallback, void* userParameter, void* internalParameter);

LIBMMS_INTERNAL void
MmsEndpointConnection_handleFileDirectoryRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
MmsEndpointConnection_handleFileOpenRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
MmsEndpointConnection_handleFileDeleteRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
MmsEndpointConnection_handleFileRenameRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
MmsEndpointConnection_handleFileReadRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
MmsEndpointConnection_handleFileCloseRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
MmsEndpointConnection_handleObtainFileRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
MmsEndpointConnection_handleAppTick(MmsEndpointConnection self);

LIBMMS_INTERNAL void
MmsEndpointConnection_handleConfirmedRequestPdu(
        MmsEndpointConnection self,
        uint8_t* buffer, int bufPos, int maxBufPos);

LIBMMS_INTERNAL MmsServer
MmsEndpointConnection_getServer(MmsEndpointConnection self);

/* Send request message, ... route either to IsoConnection (for passive MMS endpoint) or
 * IsoClientConnection (for active MMS endpoint).
 * Use also for response messages?
 * TODO also update statistics in function!
 */
LIBMMS_INTERNAL void
MmsEndpointConnection_sendMessage(MmsEndpointConnection self, ByteBuffer* message);

LIBMMS_INTERNAL void
MmsEndpointConnection_parseMessage(MmsEndpointConnection self, IsoIndication indication,
        ByteBuffer* message, ByteBuffer* response);

LIBMMS_INTERNAL ByteBuffer*
MmsEndpoint_reserveTransmitBuffer(MmsEndpoint self);

LIBMMS_INTERNAL void
MmsEndpoint_releaseTransmitBuffer(MmsEndpoint self);

LIBMMS_INTERNAL void
MmsEndpointConnection_sendRejectPdu(MmsEndpointConnection self, uint32_t* invokeId, int reason);

LIBMMS_INTERNAL void
MmsEndpointConnection_sendServiceErrorPdu(MmsEndpointConnection self, uint32_t invokeId, MmsError err);

LIBMMS_INTERNAL MmsObtainFileTask
MmsEndpoint_getObtainFileTask(MmsEndpoint self);


#endif /* MMS_ENDPOINT_INTERNAL_H_ */
