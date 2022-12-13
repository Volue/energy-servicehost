/*
 *  mms_endpoint.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_ENDPOINT_H_
#define MMS_ENDPOINT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "libmms_platform_includes.h"
#include "iso_connection_parameters.h"
#include "mms_common.h"
#include "mms_value.h"
#include "tls_config.h"

typedef void (*MmsAppTickHandler) (void* parameter);

typedef enum {
    MMS_ENDPOINT_STATE_IDLE = 0,
    MMS_ENDPOINT_STATE_LISTENING = 1,
    MMS_ENDPOINT_STATE_CONNECTED = 2,
    MMS_ENDPOINT_STATE_CONNECTING = 3,
    MMS_ENDPOINT_STATE_ERROR = 4
} MmsEndpointState;

/**
 * Contains MMS layer specific parameters
 */
typedef struct sMmsConnectionParameters {
    int maxServOutstandingCalling;
    int maxServOutstandingCalled;
    int dataStructureNestingLevel;
    int maxPduSize; /* local detail */
    uint8_t servicesSupported[11];
} MmsConnectionParameters;

typedef void
(*libmms_Log_Function) (void* parameter, int logSource, int logLevel, const char* message);

LIBMMS_API void
libmms_setLogFunction(libmms_Log_Function logFunction, void* parameter);

/**
 * MmsEndpoint is "router" and abstraction layer between IsoServer, IsoClient on the lower layer side and
 * MmsServer, and MmsClient at the higher layer (API side).
 */
typedef struct sMmsEndpoint* MmsEndpoint;

typedef struct sMmsEndpointConnection* MmsEndpointConnection;

typedef struct sMmsClient* MmsClient;

typedef struct sMmsServer* MmsServer;

typedef void (*MmsEndpointLogHandler)(void* parameter, MmsLogLevel logLevel, MmsEndpointConnection connection, const char* message);

/**
 * \brief Create a new MMS endpoint
 *
 * \param tlsConfiguration TLS configuration or NULL when TLS is not to be used
 * \param passive true for passive mode (TCP server), false for active mode (TCP client)
 */
LIBMMS_API MmsEndpoint
MmsEndpoint_create(TLSConfiguration tlsConfiguration, bool passive);

LIBMMS_API void
MmsEndpoint_destroy(MmsEndpoint self);

LIBMMS_API void
MmsEndpoint_setLogHandler(MmsEndpoint self, MmsEndpointLogHandler handler, void* parameter);

LIBMMS_API void
MmsEndpoint_setLogLevel(MmsEndpoint self, MmsLogLevel logLevel);

LIBMMS_API void
MmsEndpoint_setTLSConfiguration(MmsEndpoint self, TLSConfiguration tlsConfig);

/**
 * \brief Wait until endpoint enters the expected state or times out
 */
LIBMMS_API bool
MmsEndpoint_waitForState(MmsEndpoint self, MmsEndpointState state, int timeoutInMs);

/**
 * \brief Access to the MMS connection parameters
 *
 * These are the proposed values used for negotiation. The actual chosen value depends
 * on the values accepted by the peer!
 */
LIBMMS_API MmsConnectionParameters*
MmsEndpoint_getConnectionParameters(MmsEndpoint self);

/**
 * \brief Get the current state of the endpoint
 */
LIBMMS_API MmsEndpointState
MmsEndpoint_getState(MmsEndpoint self);

LIBMMS_API void
MmsEndpoint_addClient(MmsEndpoint self, MmsClient client);

LIBMMS_API void
MmsEndpoint_installAppTickHandler(MmsEndpoint self, MmsAppTickHandler handler, void* parameter);

/**
 * \brief Get the number of connected peers
 *
 * NOTE: This is especially useful in passive mode. In active
 * mode there can be only one or no active connection.
 */
LIBMMS_API int
MmsEndpoint_getNumberOfActiveConnections(MmsEndpoint self);

/* active and passive mode */
LIBMMS_API void
MmsEndpoint_setLocalIpAddress(MmsEndpoint self, const char* localIpAddress);

/* only for passive mode */
LIBMMS_API void
MmsEndpoint_setLocalTcpPort(MmsEndpoint self, int localTcpPort);

LIBMMS_API void
MmsEndpoint_setLocalApplicationAddress(MmsEndpoint self, IsoApplicationAddress localAddress);

LIBMMS_API IsoApplicationAddress
MmsEndpoint_getLocalApplicationAddress(MmsEndpoint self);


/**
 * \brief Set the request timeout in ms for this endpoint
 *
 * \param self MmsEndpoint instance to operate on
 * \param timeoutInMs request timeout in milliseconds
 */
LIBMMS_API void
MmsEndpoint_setRequestTimeout(MmsEndpoint self, uint32_t timeoutInMs);

/**
 * \brief Set the connect timeout in ms for this endpoint instance
 *
 * When setting up an active connection the connection attempt will
 * timeout after timeoutInMs
 *
 * \param self MmsEndpoint instance to operate on
 * \param timeoutInMs connect timeout in milliseconds
 */
LIBMMS_API void
MmsEndpoint_setConnectTimeout(MmsEndpoint self, uint32_t timeoutInMs);


/**
 * \brief Access to local application address (required for active endpoint)
 */
LIBMMS_API IsoApplicationAddress
MmsEndpoint_getLocalApplicationAddress(MmsEndpoint self);

/**
 * \brief Access to local application address (required for active endpoint)
 */
LIBMMS_API void
MmsEndpoint_setLocalApplicationAddress(MmsEndpoint self, IsoApplicationAddress address);

/* only passive mode? */
LIBMMS_API void
MmsEndpoint_setClientAuthenticator(MmsEndpoint self, AcseAuthenticator authenticator, void* authenticatorParameter);

/* what about the ConnectionStateChangedHandler??? */
/* state (one of connected/closed) */
LIBMMS_API void
MmsEndpoint_setConnectionHandler(MmsEndpoint self, MmsEndpointConnection connection, int state);

/* only for active mode */
LIBMMS_API void
MmsEndpoint_setRemoteIpAddress(MmsEndpoint self, const char* remoteIpAddress);

/* only for active mode */
LIBMMS_API void
MmsEndpoint_setRemoteTcpPort(MmsEndpoint self, int remoteTcpPort);

LIBMMS_API void
MmsEndpoint_setRemoteApplicationAddress(MmsEndpoint self, IsoApplicationAddress remoteAddress);

LIBMMS_API IsoApplicationAddress
MmsEndpoint_getRemoteApplicationAddress(MmsEndpoint self);

LIBMMS_API void
MmsEndpoint_setClientAuthenticator(MmsEndpoint self, AcseAuthenticator authenticator, void* authenticatorParameter);


/**
 * \brief Set inactivity timeout for sending heart beat messages
 *
 * NOTE: Heart beat messages (MMS identify requests) are sent when
 * the inactivity timeout elapsed. Default timeout value is 0 (heart beat disabled).
 *
 * \param timeoutInMs timeout in ms or 0 for not using heartbeat functionality
 */
LIBMMS_API void
MmsEndpoint_setHeartbeatTimeout(MmsEndpoint self, int timeoutInMs);

typedef  enum {
    MMS_FILE_ACCESS_TYPE_READ_DIRECTORY,
    MMS_FILE_ACCESS_TYPE_OPEN,
    MMS_FILE_ACCESS_TYPE_OBTAIN,
    MMS_FILE_ACCESS_TYPE_DELETE,
    MMS_FILE_ACCESS_TYPE_RENAME
} MmsFileServiceType;

/**
 * \brief MmsFileAccessHandler callback function. Use to monitor and control file access
 *
 * \param parameter user provided parameter that is passed to the callback handler
 * \param connection the connection that requested the service
 * \param service the requested file service
 * \param localFilename the requested file or directory name at the server
 * \param otherFilename a second file name parameter (e.g. source file of the ObtainFile or new file of rename file)
 *
 * \return MMS_ERROR_NONE when the request is accepted, otherwise use the appropriate error code (e.g. MMS_ERROR_FILE_FILE_ACCESS_DENIED)
 */
typedef MmsError (*MmsFileAccessHandler) (void* parameter, MmsEndpointConnection connection, MmsFileServiceType service,
                                          const char* localFilename, const char* otherFilename);

/**
 * \brief Install a callback handler that is invoked when the client/peer requests a local file. This function can be
 *        used to monitor and control file access
 *
 * \param self the MmsEndpoint instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
LIBMMS_API void
MmsEndpoint_installFileAccessHandler(MmsEndpoint self, MmsFileAccessHandler handler, void* parameter);

/**
 * \brief ObtainFile service callback handler
 *
 * This is invoked when the obtainFile service is requested by the client. It can be used to accept or deny the
 * write access to the file system based on the client connection.
 *
 * \param parameter user provided parameter that is passed to the callback handler
 * \param connection the connection that requested the service
 * \param sourceFilename the source file name on the client side system
 * \param destinationFilename the target file name on the server side system
 */
typedef bool (*MmsObtainFileHandler)(void* parameter, MmsEndpointConnection connection, const char* sourceFilename, const char* destinationFilename);

/**
 * \brief Install callback handler that is invoked when the file upload (obtainFile service) is invoked by the client
 *
 * This handler can be used to apply access restrictions on the file upload (obtainFile) service
 *
 * \param self the MmsServer instance to operate on
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
LIBMMS_API void
MmsEndpoint_installObtainFileHandler(MmsEndpoint self, MmsObtainFileHandler handler, void* parameter);

/**
 * \brief Set the virtual filestore basepath for the MMS file services
 *
 * All external file service accesses will be mapped to paths relative to the base directory.
 * NOTE: This function is only available when the CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME
 * option in stack_config.h is set.
 *
 * \param self the MmsEndpoint instance
 * \param basepath the new virtual filestore basepath
 */
LIBMMS_API void
MmsEndpoint_setFilestoreBasepath(MmsEndpoint self, const char* basepath);

LIBMMS_API const char*
MmsEndpoint_getFilestoreBasepath(MmsEndpoint self);

/**
 * \brief Callback function to intercept raw MMS messages
 *
 * IMPORTANT: the message buffer is only valid in the context of the the callback function. If the
 * message data is required elsewhere it has to be copied here!
 *
 * \param parameter user provided parameter that is passed to the callback function
 * \param message buffer of the message.
 * \param messageLength length of the message in bytes
 * \param received if true message has been received, false when message has been sent.
 */
typedef void (*MmsRawMessageHandler) (void* parameter, uint8_t* message, int messageLength, bool received);

/**
 * \brief Set the callback handler to intercept the raw MMS messages of the connection
 *
 * This function can be used to log raw MMS messages. It may be useful for debugging purposes
 * or advanced test tools. This function will only work when the flag CONFIG_MMS_RAW_MESSAGE_LOGGING
 * it set in stack_config.h
 *
 * \param self MmsEndpoint instance to operate on
 * \param handler the callback function
 * \param a user provided parameter passed to the callback function (use NULL if not required).
 */
LIBMMS_API void
MmsEndpoint_setRawMessageHandler(MmsEndpoint self, MmsRawMessageHandler handler, void* parameter);

/**
 * \brief Get file complete (obtainFile service terminated) callback handler
 *
 * This handler can be used to invoke some actions after a file upload is complete.
 *
 * \param parameter user provided parameter that is passed to the callback handler
 * \param connection the connection that requested the service
 * \param destinationFilename the target file name on the server side system
 */
typedef void (*MmsGetFileCompleteHandler)(void* parameter, MmsEndpointConnection connection, const char* destinationFilename);

/**
 * \brief Install callback handler that is invoked when the file upload (server side of obtainFile service) is completed and the
 *        file has been uploaded.
 *
 * \param self the MmsEndpoint instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
LIBMMS_API void
MmsEndpoint_installGetFileCompleteHandler(MmsEndpoint self, MmsGetFileCompleteHandler handler, void* parameter);


/**
 * \brief Start connecting to TCP server (active mode) or start listening
 * for TCP client connections (passive mode)
 *
 * NOTE: In active mode connect has to be called again manually whenever a connection
 * was disconnected. There is no automatic reconnect.
 */
LIBMMS_API void
MmsEndpoint_connect(MmsEndpoint self);

#define MMS_ENDPOINT_DISCONNECT_CLOSE 0
#define MMS_ENDPOINT_DISCONNECT_ABORT 1
#define MMS_ENDPOINT_DISCONNECT_RELEASE 2

/**
 * \brief Close a connection to a TCP server (active mode) or stop listening
 * for TCP client connections (passive mode).
 *
 * NOTE: In passive mode also all existing TCP connections will be closed.
 *
 * \param disconnectMode one of CLOSE, ABORT, RELEASE
 */
LIBMMS_API void
MmsEndpoint_disconnect(MmsEndpoint self, int disconnectMode);

LIBMMS_API void
MmsEndpoint_disconnectPeer(MmsEndpoint self, const char* ipAddr);

LIBMMS_API void
MmsEndpoint_disconnectPeersForServer(MmsEndpoint self, MmsServer server);

/**
 * \brief Check if endpoint is running (listing to client connections or connecting/connected to a server)
 *
 * \return true when endpoint is running, false otherwise
 */
LIBMMS_API bool
MmsEndpoint_isRunning(MmsEndpoint self);

/**
 * \brief Check if the endpoint is in passive mode
 *
 * \return true when endpoint is in passive mode, false otherwise
 */
LIBMMS_API bool
MmsEndpoint_isPassive(MmsEndpoint self);

typedef void (*MmsEndpointStateChangedHandler) (MmsEndpoint endpoint, void* parameter, MmsEndpointState newState);

LIBMMS_API void
MmsEndpoint_setEndpointStateChangedHandler(MmsEndpoint self, MmsEndpointStateChangedHandler handler, void* parameter);


typedef enum {
    MMS_ENDPOINT_EVENT_NEW_CONNECTION,
    MMS_ENDPOINT_EVENT_CONNECTION_CLOSED
} MmsEndpointEvent;

typedef void (*MmsConnectionHandler)(void* parameter,
        MmsEndpointConnection connection, MmsEndpointEvent event);

/**
 * A connection handler will be invoked whenever a new peer connection is opened or closed
 */
LIBMMS_API void
MmsEndpoint_installConnectionHandler(MmsEndpoint self, MmsConnectionHandler handler,
        void* parameter);

/** Handler call indicates the first element of an information report containing multiple named variables */
#define MMS_INFO_HANDLER_REPORT_START          1

/** Handler call indicated the last element of an information report containing multiple named variables */
#define MMS_INFO_HANDLER_REPORT_END            2

/** Information report contains a single named variable list */
#define MMS_INFO_HANDLER_IS_VARIABLE_LIST_NAME 4

typedef void (*MmsInformationReportHandler) (MmsEndpointConnection connection,
        void* parameter, char* domainName,
        char* variableListName, MmsValue* value, uint8_t flags);

LIBMMS_API void
MmsEndpoint_installInformationReportHandler(MmsEndpoint self, MmsInformationReportHandler handler, void* parameter);

LIBMMS_API uint64_t
MmsEndpoint_getStatistics(MmsEndpoint self, int statisticValueId);

/***************************************************
 * Functions for MMS identify service
 ***************************************************/

/**
 * \brief set the values that the endpoint will give as response for an MMS identify request
 *
 * With this function the VMD identity attributes can be set programmatically. If not set by this
 * function the default values form stack_config.h are used.
 *
 * \param self the MmsEndpoint instance to operate on
 * \param vendorName the vendor name attribute of the VMD
 * \param modelName the model name attribute of the VMD
 * \param revision the revision attribute of the VMD
 */
LIBMMS_API void
MmsEndpoint_setIdentity(MmsEndpoint self, char* vendorName, char* modelName, char* revision);

/**
 * \brief get the vendor name attribute of the VMD identity
 *
 * \param self the MmsEndpoint instance to operate on
 * \return the vendor name attribute of the VMD as C string
 */
LIBMMS_API const char*
MmsEndpoint_getVendorName(MmsEndpoint self);

/**
 * \brief get the model name attribute of the VMD identity
 *
 * \param self the MmsEndpoint instance to operate on
 * \return the model name attribute of the VMD as C string
 */
LIBMMS_API const char*
MmsEndpoint_getModelName(MmsEndpoint self);

/**
 * \brief get the revision attribute of the VMD identity
 *
 * \param self the MmsEndpoint instance to operate on
 * \return the revision attribute of the VMD as C string
 */
LIBMMS_API const char*
MmsEndpoint_getRevision(MmsEndpoint self);



typedef struct sMmsEndpointServerConnection* MmsEndpointServerConnection;
typedef struct sMmsEndpointClientConnection* MmsEndpointClientConnection;

/**
 * \brief Lock the underlying communication connection
 *
 * NOTE: This function has to be called BEFORE a send function whenever it is used outside of a MMS callback
 */
LIBMMS_API void
MmsEndpointConnection_lockConnection(MmsEndpointConnection self);

/**
 * \brief Unlock the underlying communication connection
 *
 * NOTE: This function has to be called AFTER a send function whenever it is used outside of a MMS callback
 */
LIBMMS_API void
MmsEndpointConnection_unlockConnection(MmsEndpointConnection self);

/* TODO add MmsObjectScope parameters???? */
LIBMMS_API void
MmsEndpointConnection_sendInformationReportNamedVariableList(MmsEndpointConnection self,
        const char* domainId, const char* itemId, LinkedList values);

LIBMMS_API void
MmsEndpointConnection_sendInformationReportSingleVariableVMDSpecific(MmsEndpointConnection self,
        char* itemId, MmsValue* value);

LIBMMS_API void
MmsEndpointConnection_sendInformationReportListOfVariables(
        MmsEndpointConnection self,
        LinkedList /* MmsVariableAccessSpecification */ variableAccessDeclarations,
        LinkedList /* MmsValue */ values
);

/**
 * \brief Get the address of the peer application (IP address and port number)
 *
 * NOTE: The memory of the returned string has to be released by the caller.
 *
 * \param self the MmsEndpointConnection instance
 *
 * \return the IP address and port number as strings separated by the ':' character.
 */
LIBMMS_API char*
MmsEndpointConnection_getPeerAddress(MmsEndpointConnection self);

/**
 * \brief Associate an endpoint connection with MmsServer instance
 */
LIBMMS_API void
MmsEndpointConnection_setServer(MmsEndpointConnection self, MmsServer server);

/**
 * \brief Get the maximum MMS PDU size used by this connection
 */
LIBMMS_API int
MmsEndpointConnection_getMaxPduSize(MmsEndpointConnection self);

/**
 * \brief Get ISO ACSE and lower layer addresses of the connected peer
 */
LIBMMS_API IsoApplicationAddress
MmsEndpointConnection_getPeerApplicationAddress(MmsEndpointConnection self);

/**
 * TODO rename to getLocalApplicationAddress?
 */
LIBMMS_API IsoApplicationAddress
MmsEndpointConnection_getCalledApplicationAddress(MmsEndpointConnection self);

LIBMMS_API void*
MmsEndpointConnection_getSecurityToken(MmsEndpointConnection self);

/**
 * \brief Get the invoke ID of the last received request from client
 *
 * NOTE: only to be used in callback functions!
 *
 * \param self the MmsEndpointConnection instance
 *
 * \return invoke ID
 */
LIBMMS_API uint32_t
MmsEndpointConnection_getCurrentInvokeId(MmsEndpointConnection self);

#ifdef __cplusplus
}
#endif

#endif /* MMS_ENDPOINT_H_ */
