/*
 *  tase2_endpoint.h
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef TASE2_ENDPOINT_H_
#define TASE2_ENDPOINT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iso_connection_parameters.h"
#include "tls_config.h"

/**
 * \defgroup tase2_common Common API parts
 * @{
 */

typedef struct sTase2_BilateralTable* Tase2_BilateralTable;

/**
 * \brief A TASE.2 endpoint that can be configured as passive (TCP server) or active (TCP client)
 */
typedef struct sTase2_Endpoint* Tase2_Endpoint;

/**
 * \brief Represents a connection to another connected TASE.2 endpoint
 */
typedef struct sTase2_Endpoint_Connection* Tase2_Endpoint_Connection;

typedef enum
{
    /**
     * endpoint has not been used yet.
     */
    TASE2_ENDPOINT_STATE_IDLE = 0,

    /**
     * passive endpoint is listening for incoming TCP client connections.
     */
    TASE2_ENDPOINT_STATE_LISTENING = 1,

    /**
     * active endpoint is connected to remote TCP server.
     */
    TASE2_ENDPOINT_STATE_CONNECTED = 2,

    /**
     * active endpoint is connecting to remote TCP server.
     */
    TASE2_ENDPOINT_STATE_CONNECTING = 3,

    /**
     * connection attempt failed or endpoint cannot allocate local TCP port.
     */
    TASE2_ENDPOINT_STATE_ERROR = 4
} Tase2_Endpoint_State;

/**
 * \brief A TASE.2 client to communicate with a single TASE.2 server
 */
typedef struct sTase2_Client* Tase2_Client;

/**
 * \brief Handle for a Tase2_Server instance (representing a TASE.2/MMS server)
 */
typedef struct sTase2_Server* Tase2_Server;

/**
 * \brief T selector (ISO transport layer address)
 *
 * For not using T selector set size to 0.
 */
typedef struct
{
    uint8_t size; /** 0 .. 4 - 0 means T-selector is not present */
    uint8_t value[16]; /** T-selector value */
} Tase2_TSelector;

/**
 * \brief S selector (ISO session layer address)
 *
 * For not using S selector set size to 0.
 */
typedef struct
{
    uint8_t size; /** 0 .. 16 - 0 means S-selector is not present */
    uint8_t value[16]; /** S-selector value */
} Tase2_SSelector;

/**
 * \brief P selector (ISO presentation layer address)
 *
 * For not using P selector set size to 0.
 */
typedef struct
{
    uint8_t size; /** 0 .. 16 - 0 means P-selector is not present */
    uint8_t value[16]; /** P-selector value */
} Tase2_PSelector;

typedef struct sTase2_ApplicationAddress* Tase2_ApplicationAddress;

struct sTase2_ApplicationAddress
{
    unsigned int hasAeQualifier:1;
    unsigned int hasAeInvocationId:1;
    unsigned int hasApInvocationId:1;

    ItuObjectIdentifier apTitle;
    int aeQualifier;
    int aeInvocationId;
    int apInvocationId;
    Tase2_PSelector pSelector;
    Tase2_SSelector sSelector;
    Tase2_TSelector tSelector;
};

/**
 * \brief Callback function is called whenever the state of the endpoint changes
 *
 * \param endpoint the endpoint instance
 * \param parameter user provided context parameter
 * \param newState the new endpoint state
 */
typedef void (*Tase2_Endpoint_StateChangedHandler) (Tase2_Endpoint endpoint, void* parameter, Tase2_Endpoint_State newState);

/**
 * \brief Callback function is called when another endpoint is connected or diconnected to the local endpoint
 *
 * \param endpoint the local endpoint instance
 * \param param user provided context parameter
 * \param connection the connection to the remote endpoint
 * \param connect true when a new connection was opened, false when a connection was closed
 */
typedef void (*Tase2_Endpoint_ConnectionHandler) (Tase2_Endpoint endpoint, void* parameter, Tase2_Endpoint_Connection connection, bool connect);

/**
 * \brief Create a new endpoint instance.
 *
 * \param tlsConfig TLS configuration object, or null for not using TLS
 * \param passive true for passive mode (TCP server), false for active mode (TCP client).
 *
 * \return the new endpoint instance.
 */
TASE2_API Tase2_Endpoint
Tase2_Endpoint_create(TLSConfiguration tlsConfig, bool passive);

/**
 * \brief Check if endpoint is in passive mode
 *
 * \param self endpoint instance to use
 *
 * \return true when the endpoint is passive, false when active
 */
TASE2_API bool
Tase2_Endpoint_isPassive(Tase2_Endpoint self);

/**
 * \brief
 *
 * New in version 2.1.0
 *
 * \param self endpoint instance to use
 * \param tlsConfig TLS configuration object, or null for not using TLS
 */
TASE2_API void
Tase2_Endpoint_setTLSConfiguration(Tase2_Endpoint self, TLSConfiguration tlsConfig);

/**
 * \brief Set a handler that is called whenever the state of the endpoint changes.
 *
 * \param self endpoint instance to use
 * \param handler the callback function
 * \param parameter parameter that is passed to the callback function
 */
TASE2_API void
Tase2_Endpoint_setStateChangedHandler(Tase2_Endpoint self, Tase2_Endpoint_StateChangedHandler handler, void* parameter);

/**
 * \brief Set a handler that is called whenever a connection to a remote endpoint is opened or closed.
 *
 * \param self endpoint instance to use
 * \param handler the callback function
 * \param parameter parameter that is passed to the callback function
 */
TASE2_API void
Tase2_Endpoint_setConnectionHandler(Tase2_Endpoint self, Tase2_Endpoint_ConnectionHandler handler, void* parameter);

/**
 * \brief Authentication mechanism used by peer
 */
typedef enum
{
    /** Neither ACSE nor TLS authentication used */
    TASE2_AUTH_TYPE_NONE = 0,

    /** Use ACSE password for peer authentication */
    TASE2_AUTH_TYPE_PASSWORD = 1,

    /** Use ACSE certificate for peer authentication */
    TASE2_AUTH_TYPE_CERTIFICATE = 2,

    /** Use TLS certificate for peer authentication */
    TASE2_AUTH_TYPE_TLS = 3,

    /** Unknown mechanism for peer authentication */
    TASE2_AUTH_TYPE_UNKNOWN = 99
} Tase2_AuthenticationMethod;

/**
 * \brief Callback handler to accept/reject new connections
 *
 * \param peerAddress user provided parameter
 * \param clientAddress client IP address and port number (only valid in the context of the callback function)
 * \param clientBlt the BLT associated with the client
 * \param authType authentication method used by the peer
 * \param authValue buffer address of the authentication value provided by peer (e.g. password or TLS certificate)
 * \param authValLen length of the buffer of the authentication value
 *
 * \return true to accept connection, false to reject connection
 */
typedef bool (*Tase2_Endpoint_ConnectionAcceptHandler) (Tase2_Endpoint endpoint, void* parameter, const char* peerAddress, Tase2_BilateralTable clientBlt,
        Tase2_AuthenticationMethod authType, uint8_t* authValue, int authValLen);

/**
 * \brief Set the peer connection handler that is called when a peer tries to connect
 *
 * NOTE: This handler can be used to control if a new connection is accepted or rejected
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Endpoint_setConnectionAcceptHandler(Tase2_Endpoint self, Tase2_Endpoint_ConnectionAcceptHandler handler, void* parameter);

/**
 * \brief Set the address matching rules for identifying peer endpoints
 *
 * \param self endpoint instance to use
 * \param apTitle true, use AP-title for matching
 * \param aeQualifier true, use AE-qualifier for matching
 * \param pSelector true, use presentation selector for matching
 * \param sSelector true, use session selector for matching
 * \param tSelector true, use transport selector for matching
 */
TASE2_API void
Tase2_Endpoint_setAddressMatchingRules(Tase2_Endpoint self, bool apTitle, bool aeQualifier, bool pSelector, bool sSelector, bool tSelector);

/**
 * \brief Set own identity for the identity service
 *
 * \param self endpoint instance to use
 *
 * \param vendor the vendor name of the TASE.2 application
 * \param model the model name of the TASE.2 application
 * \param revision the revision string of the TASE.2 application
 */
TASE2_API void
Tase2_Endpoint_setIdentity(Tase2_Endpoint self, const char* vendor, const char* model, const char* revision);

/**
 * \brief Get the local application address instance
 *
 * NOTE: modifying the structure elements will immediately change the active address.
 * For a more comfortable handling please use the functions below.
 *
 * \param self endpoint instance to use
 *
 * \return local application address
 */
TASE2_API Tase2_ApplicationAddress
Tase2_Endpoint_getLocalApplicationAddress(Tase2_Endpoint self);

/**
 * \brief Set the local AP-title value
 *
 * \param self endpoint instance to use
 *
 * \param apTitle the new AP-title value
 * \param aeQualifier the new AE-qualifier value
 */
TASE2_API void
Tase2_Endpoint_setLocalApTitle(Tase2_Endpoint self, const char* apTitle, int aeQualifier);

/**
 * \brief Sets the local addresses for ISO layers (transport, session, presentation)
 *
 * \param self endpoint instance to use
 *
 * \param pSelector presentation layer address
 * \param sSelector session layer address
 * \param tSelector ISO COTP layer address
 */
TASE2_API void
Tase2_Endpoint_setLocalAddresses(Tase2_Endpoint self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector);

/**
 * \brief Sets the local IP address (only for passive mode)
 *
 * \param self endpoint instance to use
 *
 * \param ipAddress local IP address to use.
 */
TASE2_API void
Tase2_Endpoint_setLocalIpAddress(Tase2_Endpoint self, const char* ipAddress);

/**
 * \brief Sets the local TCP port (only for passive mode)
 *
 * If not set, the default port is used (102 for MMS, 3872 for MMS/TLS).
 *
 * \param self endpoint instance to use
 *
 * \param tcpPort local TCP port to use.
 */
TASE2_API void
Tase2_Endpoint_setLocalTcpPort(Tase2_Endpoint self, int tcpPort);

/**
 * \brief Set local AP-title
 *
 * \param self endpoint instance to use
 *
 * \param apTitle the new AP-title value
 */
TASE2_API void
Tase2_Endpoint_setLocalApTitleEx(Tase2_Endpoint self, const char* apTitle);

/**
 * \brief Set local AE-qualifier
 *
 * \param self endpoint instance to use
 *
 * \param aeQualifier the new AE-qualifier value
 */
TASE2_API void
Tase2_Endpoint_setLocalAeQualifier(Tase2_Endpoint self, int aeQualifier);

/**
 * \brief Set local AE-invocation-ID
 *
 * \param self endpoint instance to use
 *
 * \param aeInvocationId the new AE-invocation-ID value
 */
TASE2_API void
Tase2_Endpoint_setLocalAeInvocationId(Tase2_Endpoint self, int aeInvocationId);

/**
 * \brief Set local AP-invocation-ID
 *
 * \param self endpoint instance to use
 *
 * \param apInvocationId the new AP-invocation-ID value
 */
TASE2_API void
Tase2_Endpoint_setLocalApInvocationId(Tase2_Endpoint self, int apInvocationId);


/**
 * \brief Get the default remote application address instance
 *
 * NOTE: modifying the structure elements will immediately change the active address.
 * For a more comfortable handling please use the functions below.
 *
 * \param self endpoint instance to use
 *
 * \return remote application address
 */
TASE2_API Tase2_ApplicationAddress
Tase2_Endpoint_getRemoteApplicationAddress(Tase2_Endpoint self);

/**
 * \brief Set remote AP-title and AE-qualifier (for active endpoint)
 *
 * \param self endpoint instance to use
 *
 * \param apTitle the new AP-title value
 * \param aeQualifier the new AE-qualifier value
 */
TASE2_API void
Tase2_Endpoint_setRemoteApTitle(Tase2_Endpoint self, const char* apTitle, int aeQualifier);

/**
 * \brief Sets the remote addresses for ISO layers (transport, session, presentation)
 *
 * \param self endpoint instance to use
 *
 * \param pSelector presentation layer address
 * \param sSelector session layer address
 * \param tSelector ISO COTP layer address
 */
TASE2_API void
Tase2_Endpoint_setRemoteAddresses(Tase2_Endpoint self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector);

/**
 * \brief Sets the remote IP address (only for active mode)
 *
 * \param self endpoint instance to use
 *
 * \param ipAddress remote IP address to use.
 */
TASE2_API void
Tase2_Endpoint_setRemoteIpAddress(Tase2_Endpoint self, const char* ipAddress);

/**
 * \brief Sets the remote TCP port (only for active mode)
 *
 * If not set, the default port is used (102 for MMS, 3872 for MMS/TLS).
 *
 * \param self endpoint instance to use
 *
 * \param tcpPort remote TCP port to use.
 */
TASE2_API void
Tase2_Endpoint_setRemoteTcpPort(Tase2_Endpoint self, int tcpPort);

/**
 * \brief Set remote AP-title (for active endpoint)
 *
 * \param self endpoint instance to use
 *
 * \param apTitle the new AP-title value
 */
TASE2_API void
Tase2_Endpoint_setRemoteApTitleEx(Tase2_Endpoint self, const char* apTitle);

/**
 * \brief Set remote AE-qualifier (for active endpoint)
 *
 * \param self endpoint instance to use
 *
 * \param aeQualifier the new AE-qualifier value
 */
TASE2_API void
Tase2_Endpoint_setRemoteAeQualifier(Tase2_Endpoint self, int aeQualifier);

/**
 * \brief Set remote AE-invocation-ID (for active endpoint)
 *
 * \param self endpoint instance to use
 *
 * \param aeInvocationId the new AE-invocation-ID value
 */
TASE2_API void
Tase2_Endpoint_setRemoteAeInvocationId(Tase2_Endpoint self, int aeInvocationId);

/**
 * \brief Set remote AP-invocation-ID (for active endpoint)
 *
 * \param self endpoint instance to use
 *
 * \param apInvocationId the new AP-invocation-ID value
 */
TASE2_API void
Tase2_Endpoint_setRemoteApInvocationId(Tase2_Endpoint self, int apInvocationId);

/**
 * \brief Sets the maximum MMS PDU size used by the endpoint
 *
 * NOTE: Actual values are negotiated with the peer and are equal or below this
 * value.
 *
 * \param self endpoint instance to use
 * \param maxPduSize the maximum PDU size in bytes
 */
TASE2_API void
Tase2_Endpoint_setMaxMmsPduSize(Tase2_Endpoint self, int maxPduSize);

/**
 * \brief Set the maximum number of outstanding service calls
 *
 * NOTE: the effective value may depend on the other endpoint as well.
 *
 * \param self endpoint instance to use
 * \param value the maximum number of outstanding service calls
 */
TASE2_API void
Tase2_Endpoint_setMaxServOutstandingCalling(Tase2_Endpoint self, int value);

/**
 * \brief Set the maximum number of outstanding service calls
 *
 * NOTE: the effective value may depend on the other endpoint as well.
 *
 * \param self endpoint instance to use
 * \param value the maximum number of outstanding service calls
 */
TASE2_API void
Tase2_Endpoint_setMaxServOutstandingCalled(Tase2_Endpoint self, int value);

/**
 * \brief Sets the timeout for confirmed request messages
 *
 * Default value is 5000 ms.
 *
 * \param self endpoint instance to use
 * \param timeoutInMs timeout in milliseconds
 */
TASE2_API void
Tase2_Endpoint_setRequestTimeout(Tase2_Endpoint self, int timeoutInMs);

/**
 * \brief Sets the connect timeout of active endpoints
 *
 * Default value is 10000 ms.
 *
 * \param self endpoint instance to use
 * \param timeoutInMs timeout in milliseconds
 */
TASE2_API void
Tase2_Endpoint_setConnectTimeout(Tase2_Endpoint self, int timeoutInMs);

/**
 * \brief Connect the endpoint (start a passive or active TCP endpoint)
 */
TASE2_API void
Tase2_Endpoint_connect(Tase2_Endpoint self);

/**
 * \brief Disconnect (close all connections and stop listening for new connections)
 */
TASE2_API void
Tase2_Endpoint_disconnect(Tase2_Endpoint self, int disconnectMethod);

/**
 * \brief Disconnect a specific connection identified by the IP address and optionally by the port number
 *
 * NOTE: This is for passive endpoint only
 *
 * \param peerIpAddress peer IP address (e.g. "10.0.0.10") with optional port number (e.g. "10.0.0.10:34237")
 */
TASE2_API void
Tase2_Endpoint_disconnectPeer(Tase2_Endpoint self, const char* peerIpAddress);

/**
 * \brief Check if the endpoint is connected with at least one TCP client (passive endpoint)
 *        or connected to a TCP server (active endpoint)
 *
 * \return true if the endpoint is connected, false otherwise
 */
TASE2_API bool
Tase2_Endpoint_isConnected(Tase2_Endpoint self);

/**
 * \brief Get number of connected peers
 *
 * \return number of connected peers
 */
TASE2_API int
Tase2_Endpoint_connectedPeers(Tase2_Endpoint self);

/**
 * \brief Wait until the endpoint enters a specific state of state changes to TASE2_ENDPOINT_STATE_ERROR
 *
 * \param state the expected state (state to wait for)
 * \param timeout maximum time to wait for the expected state (in ms)
 *
 * \return true when state is reached, false when timeout or error occurs.
 */
TASE2_API bool
Tase2_Endpoint_waitForState(Tase2_Endpoint self, Tase2_Endpoint_State state, int timeoutInMs);

/**
 * \brief Get the current state of the endpoint
 *
 * NOTE: Possible values depend on the endpoint type (active/passive).
 *
 * \return current state of the endpoint
 */
TASE2_API Tase2_Endpoint_State
Tase2_Endpoint_getState(Tase2_Endpoint self);

/**
 * \brief Add a server to the endpoint
 * 
 * NOTE: This function can be used to add a server to an endpoint that has been removed before using the \ref Tase2_Endpoint_removeServer function
 * 
 * \param server the server to be added
 */
TASE2_API void
Tase2_Endpoint_addServer(Tase2_Endpoint self, Tase2_Server server);

/**
 * \brief Remove a server from the endpoint
 *
 * \param server the server to be removed
 *
 * \return the removed server, or NULL when server was not associated with this endpoint
 */
TASE2_API Tase2_Server
Tase2_Endpoint_removeServer(Tase2_Endpoint self, Tase2_Server server);


/**
 * \brief Remove a client from the endpoint
 *
 * \param client the client to be removed
 *
 * \return the removed client, or NULL when the client was not associated with this endpoint
 */
Tase2_Client
Tase2_Endpoint_removeClient(Tase2_Endpoint self, Tase2_Client client);

/**
 * \brief Delete the endpoint and release all resources
 *
 * NOTE: Do not use the endpoint instance after this call!
 */
TASE2_API void
Tase2_Endpoint_destroy(Tase2_Endpoint self);

/**
 * \brief Get the ID string of the endpoint
 *
 * The ID string can be used by the application to identify the endpoint.
 *
 * The returned string is managed by the endpoint instance and is only valid
 * until the \ref Tase2_Endpoint_destroy function is called.
 *
 * \return the ID string of the endpoint
 */
TASE2_API const char*
Tase2_Endpoint_getId(Tase2_Endpoint self);

/**
 * \brief Set the ID string of the endpoint
 *
 * The ID string can be used by the application to identify the endpoint.
 *
 * \param the ID string of the endpoint
 */
TASE2_API void
Tase2_Endpoint_setId(Tase2_Endpoint self, const char* id);

/**
 * \brief Get the address of the peer application (IP address and port number)
 *
 * NOTE: The memory of the returned string is managed by the library! The
 * string is only valid in the context of the callback function or when used
 * outside a callback as long as the Tase2_Endpoint_Connection object exists.
 * This changed with version 2.1!
 *
 * \param self the Tase2_Endpoint_Connection instance
 *
 * \return the IP address and port number as strings separated by the ':' character.
 */
TASE2_API char*
Tase2_Endpoint_Connection_getPeerIpAddress(Tase2_Endpoint_Connection self);

/**
 * \brief Get the ISO ACSE AP-title of the peer
 *
 * NOTE: The memory of the returned string is managed by the library! The
 * string is only valid in the context of the callback function or when used
 * outside a callback as long as the Tase2_Endpoint_Connection object exists.
 * This changed with version 2.1!
 *
 * \return the AP-title as string. The AP-title parts are separated by dots ('.')
 */
TASE2_API char*
Tase2_Endpoint_Connection_getPeerApTitle(Tase2_Endpoint_Connection self);

/**
 * \brief Get the T selector of the peer
 *
 * NOTE: The returned object is managed by the library! It
 * is only valid in the context of the callback function or when used
 * outside a callback as long as the Tase2_Endpoint_Connection object exists.
 *
 * \return the T selector
 */
TASE2_API Tase2_TSelector*
Tase2_Endpoint_Connection_getPeerTSel(Tase2_Endpoint_Connection self);

/**
 * \brief Get the S selector of the peer
 *
 * NOTE: The returned object is managed by the library! It
 * is only valid in the context of the callback function or when used
 * outside a callback as long as the Tase2_Endpoint_Connection object exists.
 *
 * \return the S selector
 */
TASE2_API Tase2_SSelector*
Tase2_Endpoint_Connection_getPeerSSel(Tase2_Endpoint_Connection self);

/**
 * \brief Get the P selector of the peer
 *
 * NOTE: The returned object is managed by the library! It
 * is only valid in the context of the callback function or when used
 * outside a callback as long as the Tase2_Endpoint_Connection object exists.
 *
 * \return the P selector
 */
TASE2_API Tase2_PSelector*
Tase2_Endpoint_Connection_getPeerPSel(Tase2_Endpoint_Connection self);

/**
 * \brief Get the ISO ACSE AE-qualifier of the peer
 *
 * \return the AE-qualifier value
 */
TASE2_API int
Tase2_Endpoint_Connection_getPeerAeQualifier(Tase2_Endpoint_Connection self);

/**
 * \brief Get the maximum MMS PDU size used by this connection
 *
 * \return the maximum MMS PDU size
 */
TASE2_API int
Tase2_Endpoint_Connection_getMaxPduSize(Tase2_Endpoint_Connection self);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* TASE2_ENDPOINT_H_ */
