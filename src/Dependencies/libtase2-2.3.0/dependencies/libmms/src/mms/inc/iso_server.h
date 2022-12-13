/*
 *  iso_server.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef ISO_SERVER_H_
#define ISO_SERVER_H_

#include "byte_buffer.h"
#include "iso_connection_parameters.h"
#include "mms_endpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup mms_server_api_group
 *  @{
 */

typedef enum
{
    ISO_SVR_STATE_IDLE,
    ISO_SVR_STATE_RUNNING,
    ISO_SVR_STATE_STOPPED,
    ISO_SVR_STATE_ERROR
} IsoServerState;

typedef struct sIsoServer* IsoServer;

typedef enum
{
    /* new connection opened by client */
    ISO_CONNECTION_OPENED,

    /* connection is ready to be used by MMS user */
    ISO_CONNECTION_READY,

    /* connection is closed */
    ISO_CONNECTION_CLOSED
} IsoConnectionIndication;

typedef struct sIsoConnection* IsoConnection;

typedef struct sIsoServerCallbacks
{
    void
    (*clientConnected)(IsoConnection connection);
} IsoServerCallbacks;

typedef void
(*ConnectionIndicationHandler)(IsoConnectionIndication indication,
        void* parameter, IsoConnection connection);

typedef void
(*MessageReceivedHandler)(void* parameter, ByteBuffer* message, ByteBuffer* response);

LIBMMS_INTERNAL char*
IsoConnection_getPeerAddress(IsoConnection self);

LIBMMS_INTERNAL IsoApplicationAddress
IsoConnection_getPeerApplicationAddress(IsoConnection self);

LIBMMS_INTERNAL IsoApplicationAddress
IsoConnection_getCalledApplicationAddress(IsoConnection self);

LIBMMS_INTERNAL void
IsoConnection_close(IsoConnection self);

LIBMMS_INTERNAL void
IsoConnection_installListener(IsoConnection self, MessageReceivedHandler handler,
        void* parameter);

LIBMMS_INTERNAL void*
IsoConnection_getSecurityToken(IsoConnection self);

/**
 * \brief send a message over an ISO connection
 *
 * \param handlerMode specifies if this function is used in the context of the connection handling thread
 *        (handlerMode)
 */
LIBMMS_INTERNAL void
IsoConnection_sendMessage(IsoConnection self, ByteBuffer* message);

LIBMMS_INTERNAL void
IsoConnection_lockConnection(IsoConnection self);

LIBMMS_INTERNAL void
IsoConnection_unlockConnection(IsoConnection self);

LIBMMS_INTERNAL IsoServer
IsoServer_create(TLSConfiguration tlsConfiguration, IsoApplicationAddress localAppAddr);

LIBMMS_INTERNAL void
IsoServer_setTcpPort(IsoServer self, int port);

LIBMMS_INTERNAL void
IsoServer_setLocalIpAddress(IsoServer self, const char* ipAddress);

LIBMMS_INTERNAL IsoServerState
IsoServer_getState(IsoServer self);

LIBMMS_INTERNAL void
IsoServer_setAppTickHandler(IsoServer self, MmsAppTickHandler handler, void* parameter);

LIBMMS_INTERNAL void
IsoServer_setConnectionHandler(IsoServer self, ConnectionIndicationHandler handler,
        void* parameter);

LIBMMS_INTERNAL void
IsoServer_setAuthenticator(IsoServer self, AcseAuthenticator authenticator, void* authenticatorParameter);

LIBMMS_INTERNAL AcseAuthenticator
IsoServer_getAuthenticator(IsoServer self);

LIBMMS_INTERNAL void*
IsoServer_getAuthenticatorParameter(IsoServer self);

LIBMMS_INTERNAL TLSConfiguration
IsoServer_getTLSConfiguration(IsoServer self);

LIBMMS_INTERNAL void
IsoServer_setTLSConfiguration(IsoServer self, TLSConfiguration tlsConfig);

LIBMMS_INTERNAL void
IsoServer_startListening(IsoServer self);

LIBMMS_INTERNAL void
IsoServer_stopListening(IsoServer self);

LIBMMS_INTERNAL void
IsoServer_startListeningThreadless(IsoServer self);

/**
 * for non-threaded operation
 */
LIBMMS_INTERNAL void
IsoServer_processIncomingMessages(IsoServer self);

LIBMMS_INTERNAL int
IsoServer_waitReady(IsoServer self, unsigned int timeoutMs);

LIBMMS_INTERNAL void
IsoServer_stopListeningThreadless(IsoServer self);

LIBMMS_INTERNAL void
IsoServer_closeConnection(IsoServer self, IsoConnection isoConnection);

LIBMMS_INTERNAL void
IsoServer_destroy(IsoServer self);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* ISO_SERVER_H_ */
