/*
 *  iso_client_connection.h
 *
 *  This is an internal interface of MMS client connection that connects MMS to the ISO client
 *  protocol stack. It is used as an abstraction layer to isolate the MMS code from the lower
 *  protocol layers.
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef ISO_CLIENT_CONNECTION_H_
#define ISO_CLIENT_CONNECTION_H_

#include "byte_buffer.h"
#include "iso_connection_parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ISO_IND_ASSOCIATION_SUCCESS,
    ISO_IND_ASSOCIATION_FAILED,
    ISO_IND_CLOSED,
    ISO_IND_DATA,
    ISO_IND_TICK,
    ISO_IND_IGNORE
} IsoIndication;

typedef void*
(*IsoIndicationCallback)(IsoIndication indication, void* param, ByteBuffer* payload);

/**
 * opaque data structure to represent an ISO client connection.
 */
typedef struct sIsoClientConnection* IsoClientConnection;

LIBMMS_INTERNAL IsoClientConnection
IsoClientConnection_create(TLSConfiguration tlsConfiguration, IsoApplicationAddress localAddr, IsoApplicationAddress remoteAddr, IsoIndicationCallback callback, void* callbackParameter);

LIBMMS_INTERNAL void
IsoClientConnection_setAuthenticator(IsoClientConnection self, AcseAuthenticator acseAuthenticator, void* acseAuthenticatorParameter);

LIBMMS_INTERNAL void
IsoClientConnection_setTLSConfiguration(IsoClientConnection self, TLSConfiguration tlsConfig);

LIBMMS_INTERNAL void
IsoClientConnection_destroy(IsoClientConnection self);

LIBMMS_INTERNAL bool
IsoClientConnection_associateAsync(IsoClientConnection self, const char* hostname, int tcpPort, uint32_t connectTimeoutInMs, uint32_t requestTimeout);

/**
 * called by tick function
 *
 * \return value indicates that connection is currently waiting and calling thread can be suspended
 */
LIBMMS_INTERNAL bool
IsoClientConnection_handleConnection(IsoClientConnection self);

LIBMMS_INTERNAL void
IsoClientConnection_associate(IsoClientConnection self, uint32_t connectTimeoutInMs);

/**
 * Send message and release the transmit buffer
 *
 * \param payload message to send
 */
LIBMMS_INTERNAL void
IsoClientConnection_sendMessage(IsoClientConnection self, ByteBuffer* payload);

LIBMMS_INTERNAL void
IsoClientConnection_release(IsoClientConnection self);

/**
 * \brief Send ACSE abort message and wait until connection is closed by server or timeout occurred
 */
LIBMMS_INTERNAL void
IsoClientConnection_abortAsync(IsoClientConnection self);

LIBMMS_INTERNAL void
IsoClientConnection_close(IsoClientConnection self);

/**
 * This function should be called by the API client (usually the MmsConnection) to reserve(allocate)
 * the payload buffer. This is used to prevent concurrent access to the send buffer of the IsoClientConnection
 */
LIBMMS_INTERNAL ByteBuffer*
IsoClientConnection_allocateTransmitBuffer(IsoClientConnection self);

/**
 * This function is used to release the transmit buffer in case a formerly allocated transmit buffer cannot
 * be sent.
 */
LIBMMS_INTERNAL void
IsoClientConnection_releaseTransmitBuffer(IsoClientConnection self);

LIBMMS_INTERNAL char*
IsoClientConnection_getPeerAddress(IsoClientConnection self);

LIBMMS_INTERNAL IsoApplicationAddress
IsoClientConnection_getPeerApplicationAddress(IsoClientConnection self);

LIBMMS_INTERNAL void*
IsoClientConnection_getSecurityToken(IsoClientConnection self);

LIBMMS_INTERNAL bool
IsoClientConnection_isRunning(IsoClientConnection self);

#ifdef __cplusplus
}
#endif

#endif /* ISO_CLIENT_CONNECTION_H_ */
