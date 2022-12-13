/*
 *  acse.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "stack_config.h"
#include "byte_buffer.h"
#include "buffer_chain.h"
#include "iso_connection_parameters.h"
#include "tls_socket.h"

#ifndef ACSE_H_
#define ACSE_H_

typedef enum
{
    idle, requestIndicated, connected
} AcseConnectionState;

typedef enum
{
    ACSE_ERROR,
    ACSE_ASSOCIATE,
    ACSE_ASSOCIATE_FAILED,
    ACSE_OK,
    ACSE_ABORT,
    ACSE_RELEASE_REQUEST,
    ACSE_RELEASE_RESPONSE
} AcseIndication;

typedef struct sAcseConnection
{
    AcseConnectionState state;
    long nextReference;
    uint8_t* userDataBuffer;
    int userDataBufferSize;
    AcseAuthenticationParameter authentication;
    AcseAuthenticator authenticator;
    IsoApplicationAddress localAppAddr; /* when present fill with expected local ACSE address provided by remote endpoint */
    IsoApplicationAddress remoteAppAddr; /* when present fill with ACSE address provided by remote endpoint */
    void* authenticatorParameter;
    void* securityToken;

#if (CONFIG_MMS_SUPPORT_TLS == 1)
    TLSSocket tlsSocket;
#endif
} AcseConnection;

#define ACSE_RESULT_ACCEPT 0
#define ACSE_RESULT_REJECT_PERMANENT 1
#define ACSE_RESULT_REJECT_TRANSIENT 2

LIBMMS_INTERNAL void
AcseConnection_init(AcseConnection* self, IsoApplicationAddress remoteAppAddr, IsoApplicationAddress localAppAddr, AcseAuthenticator authenticator, void* parameter, TLSSocket tlsSocket);

LIBMMS_INTERNAL void
AcseConnection_destroy(AcseConnection* self);

LIBMMS_INTERNAL AcseIndication
AcseConnection_parseMessage(AcseConnection* self, ByteBuffer* message, Socket socket);

LIBMMS_INTERNAL void
AcseConnection_createAssociateFailedMessage(AcseConnection* connection, BufferChain writeBuffer);

LIBMMS_INTERNAL void
AcseConnection_createAssociateResponseMessage(AcseConnection* self,
        IsoApplicationAddress localAppAddr,
        uint8_t acseResult,
        BufferChain writeBuffer,
        BufferChain payload
        );

LIBMMS_INTERNAL void
AcseConnection_createAssociateRequestMessage(AcseConnection* self,
        IsoApplicationAddress remoteAddr,
        IsoApplicationAddress localAddr,
        BufferChain writeBuffer,
        BufferChain payload,
        AcseAuthenticationParameter authParameter);

LIBMMS_INTERNAL void
AcseConnection_createAbortMessage(AcseConnection* self, BufferChain writeBuffer, bool isProvider);

LIBMMS_INTERNAL void
AcseConnection_createReleaseRequestMessage(AcseConnection* self, BufferChain writeBuffer);

LIBMMS_INTERNAL void
AcseConnection_createReleaseResponseMessage(AcseConnection* self, BufferChain writeBuffer);

#endif /* ACSE_H_ */
