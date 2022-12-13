/*
 *  iso_connection.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "stack_config.h"
#include "mms_common_internal.h"
#include "buffer_chain.h"
#include "cotp.h"
#include "iso_session.h"
#include "iso_presentation.h"
#include "acse.h"
#include "iso_server.h"
#include "hal_socket.h"
#include "hal_thread.h"

#include "iso_server_private.h"

#ifndef DEBUG_ISO_SERVER
#ifdef DEBUG
#define DEBUG_ISO_SERVER 1
#else
#define DEBUG_ISO_SERVER 0
#endif /*DEBUG */
#endif /* DEBUG_ISO_SERVER */

#define RECEIVE_BUF_SIZE CONFIG_MMS_MAXIMUM_PDU_SIZE + 100
#define SEND_BUF_SIZE CONFIG_MMS_MAXIMUM_PDU_SIZE + 100

#define TPKT_RFC1006_HEADER_SIZE 4

#define ISO_CON_STATE_RUNNING 1
#define ISO_CON_STATE_STOPPED 0

struct sIsoConnection
{
    uint8_t* receiveBuffer;
    ByteBuffer rcvBuffer;

    uint8_t* sendBuffer;

    uint8_t* cotpReadBuf;
    uint8_t* cotpWriteBuf;
    ByteBuffer cotpReadBuffer;
    ByteBuffer cotpWriteBuffer;

    MessageReceivedHandler msgRcvdHandler;
    void* msgRcvdHandlerParameter;

    IsoServer isoServer;

    Socket socket;

#if (CONFIG_MMS_SUPPORT_TLS == 1)
    TLSSocket tlsSocket;
#endif

    int state;
    IsoSession* session;
    IsoPresentation* presentation;
    CotpConnection* cotpConnection;

    AcseConnection* acseConnection;

    struct sIsoApplicationAddress peerApplicationAddress;
    struct sIsoApplicationAddress localApplicationAddress;

    char* clientAddress;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Thread thread;
    Semaphore conMutex;
#endif
};

static void
finalizeIsoConnection(IsoConnection self)
{
    if (DEBUG_ISO_SERVER)
        libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: connection termination --> close transport connection");

    if (self->isoServer)
        IsoServer_closeConnection(self->isoServer, self);

#if (CONFIG_MMS_SUPPORT_TLS == 1)
    if (self->tlsSocket) {
        TLSSocket_close(self->tlsSocket);
        self->tlsSocket = NULL;
    }
#endif

    if (self->socket != NULL)
        Socket_destroy(self->socket);

    GLOBAL_FREEMEM(self->session);
    GLOBAL_FREEMEM(self->presentation);
    AcseConnection_destroy(self->acseConnection);
    GLOBAL_FREEMEM(self->acseConnection);

    GLOBAL_FREEMEM(self->cotpReadBuf);
    GLOBAL_FREEMEM(self->cotpWriteBuf);

    GLOBAL_FREEMEM(self->cotpConnection);

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    if (self->conMutex)
        Semaphore_destroy(self->conMutex);
#endif

    if (DEBUG_ISO_SERVER)
        libmms_iso_log(MMS_LOG_INFO, "ISO_SERVER: connection from %s closed", self->clientAddress);

    GLOBAL_FREEMEM(self->receiveBuffer);
    GLOBAL_FREEMEM(self->sendBuffer);
    GLOBAL_FREEMEM(self->clientAddress);
    IsoServer isoServer = self->isoServer;
    GLOBAL_FREEMEM(self);

    if (isoServer)
        private_IsoServer_decreaseConnectionCounter(isoServer);
}

void
IsoConnection_addHandleSet(const IsoConnection self, HandleSet handles)
{
   Handleset_addSocket(handles, self->socket);
}

void
IsoConnection_handleTcpConnection(IsoConnection self)
{
#if (CONFIG_MMS_SINGLE_THREADED == 0)
    if (IsoServer_waitReady(self->isoServer, 10) < 1)
        goto exit_function;
#endif /* (CONFIG_MMS_SINGLE_THREADED == 0) */

    TpktState tpktState = CotpConnection_readToTpktBuffer(self->cotpConnection);
    CotpIndication cotpIndication;

    if (tpktState == TPKT_ERROR)
        self->state = ISO_CON_STATE_STOPPED;

    if (tpktState != TPKT_PACKET_COMPLETE)
        goto exit_function;

    cotpIndication = CotpConnection_parseIncomingMessage(self->cotpConnection);

    switch (cotpIndication) {
    case COTP_MORE_FRAGMENTS_FOLLOW:
        goto exit_function;

    case COTP_CONNECT_INDICATION:
        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_DEBUG, "COTP: CONNECT REQUEST");

        /* copy peer T selector */
        memcpy(&(self->peerApplicationAddress.tSelector), &(self->cotpConnection->options.tSelSrc), sizeof(TSelector));

        /* copy called T selector */
        memcpy(&(self->localApplicationAddress.tSelector), &(self->cotpConnection->options.tSelDst), sizeof(TSelector));

#if (CONFIG_MMS_THREADLESS_STACK != 1)
        Semaphore_wait(self->conMutex);
#endif

        CotpConnection_sendConnectionResponseMessage(self->cotpConnection);

#if (CONFIG_MMS_THREADLESS_STACK != 1)
        Semaphore_post(self->conMutex);
#endif

        break;
    case COTP_DATA_INDICATION:
        {
            ByteBuffer* cotpPayload = CotpConnection_getPayload(self->cotpConnection);

            IsoSessionIndication sIndication = IsoSession_parseMessage(self->session, cotpPayload);

            ByteBuffer* sessionUserData = IsoSession_getUserData(self->session);

            switch (sIndication) {
            case SESSION_CONNECT:
                if (DEBUG_ISO_SERVER)
                    libmms_iso_log(MMS_LOG_DEBUG, "SESSION: CONNECT SPDU");

                /* copy peer S selector */
                memcpy(&(self->peerApplicationAddress.sSelector), &(self->session->callingSessionSelector), sizeof(SSelector));

                /* copy called S selector */
                memcpy(&(self->localApplicationAddress.sSelector), &(self->session->calledSessionSelector), sizeof(SSelector));

                if (IsoPresentation_parseConnect(self->presentation, sessionUserData)) {

                    /* copy peer P selector */
                    memcpy(&(self->peerApplicationAddress.pSelector), &(self->presentation->callingPresentationSelector), sizeof(PSelector));

                    /* copy called P selector */
                    memcpy(&(self->localApplicationAddress.pSelector), &(self->presentation->calledPresentationSelector), sizeof(PSelector));

                    ByteBuffer* acseBuffer = &(self->presentation->nextPayload);

                    AcseIndication aIndication = AcseConnection_parseMessage(self->acseConnection, acseBuffer, self->socket);

                    IsoServer_callConnectionHandler(self->isoServer, self, ISO_CONNECTION_OPENED);

                    if (aIndication == ACSE_ASSOCIATE) {

#if (CONFIG_MMS_THREADLESS_STACK != 1)
                        Semaphore_wait(self->conMutex);
#endif

                        if (DEBUG_ISO_SERVER)
                            libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: ACSE associate");

                        ByteBuffer mmsRequest;

                        ByteBuffer_wrap(&mmsRequest, self->acseConnection->userDataBuffer,
                                self->acseConnection->userDataBufferSize, self->acseConnection->userDataBufferSize);
                        ByteBuffer mmsResponseBuffer; /* new */

                        ByteBuffer_wrap(&mmsResponseBuffer, self->sendBuffer, 0, SEND_BUF_SIZE);

                        if (self->msgRcvdHandler != NULL) {
                            self->msgRcvdHandler(self->msgRcvdHandlerParameter,
                                    &mmsRequest, &mmsResponseBuffer);
                        }

                        struct sBufferChain mmsBufferPartStruct;
                        BufferChain mmsBufferPart = &mmsBufferPartStruct;

                        BufferChain_init(mmsBufferPart, mmsResponseBuffer.size, mmsResponseBuffer.size, NULL,
                                self->sendBuffer);

                        if (mmsResponseBuffer.size > 0) {

                            struct sBufferChain acseBufferPartStruct;
                            BufferChain acseBufferPart = &acseBufferPartStruct;

                            acseBufferPart->buffer = self->sendBuffer + mmsBufferPart->length;
                            acseBufferPart->partMaxLength = SEND_BUF_SIZE - mmsBufferPart->length;

                            if (self->localApplicationAddress.apTitle.arcCount != 0) {
                                AcseConnection_createAssociateResponseMessage(self->acseConnection,
                                        &(self->localApplicationAddress),
                                        ACSE_RESULT_ACCEPT, acseBufferPart, mmsBufferPart);
                            }
                            else {
                                AcseConnection_createAssociateResponseMessage(self->acseConnection,
                                        IsoServer_getLocalAppAddr(self->isoServer),
                                        ACSE_RESULT_ACCEPT, acseBufferPart, mmsBufferPart);
                            }

                            struct sBufferChain presentationBufferPartStruct;
                            BufferChain presentationBufferPart = &presentationBufferPartStruct;

                            presentationBufferPart->buffer = self->sendBuffer + acseBufferPart->length;
                            presentationBufferPart->partMaxLength = SEND_BUF_SIZE - acseBufferPart->length;

                            IsoPresentation_createCpaMessage(self->presentation, presentationBufferPart,
                                    acseBufferPart);

                            struct sBufferChain sessionBufferPartStruct;
                            BufferChain sessionBufferPart = &sessionBufferPartStruct;
                            sessionBufferPart->buffer = self->sendBuffer + presentationBufferPart->length;
                            sessionBufferPart->partMaxLength = SEND_BUF_SIZE - presentationBufferPart->length;

                            IsoSession_createAcceptSpdu(self->session, sessionBufferPart, presentationBufferPart);

                            CotpConnection_sendDataMessage(self->cotpConnection, sessionBufferPart);

#if (CONFIG_MMS_THREADLESS_STACK != 1)
                            Semaphore_post(self->conMutex);
#endif

                            IsoServer_callConnectionHandler(self->isoServer, self, ISO_CONNECTION_READY);
                        }
                        else {
                            if (DEBUG_ISO_SERVER)
                                libmms_iso_log(MMS_LOG_ERROR, "ISO_SERVER: association error. No response from application!");

#if (CONFIG_MMS_THREADLESS_STACK != 1)
                                Semaphore_post(self->conMutex);
#endif
                        }

                    }
                    else {
                        if (DEBUG_ISO_SERVER)
                            libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: ACSE association failed");
                        self->state = ISO_CON_STATE_STOPPED;
                    }

                }
                break;
            case SESSION_DATA:

                if (!IsoPresentation_parseUserData(self->presentation, sessionUserData)) {
                    if (DEBUG_ISO_SERVER)
                        libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: presentation layer error");
					self->state = ISO_CON_STATE_STOPPED;
                    break;
                }

                if (self->presentation->nextContextId == self->presentation->mmsContextId) {

                    ByteBuffer* mmsRequest = &(self->presentation->nextPayload);

                    ByteBuffer mmsResponseBuffer;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
                    IsoServer_userLock(self->isoServer);
                    Semaphore_wait(self->conMutex);
#endif

                    ByteBuffer_wrap(&mmsResponseBuffer, self->sendBuffer, 0, SEND_BUF_SIZE);

                    if (self->msgRcvdHandler != NULL) {

                        self->msgRcvdHandler(self->msgRcvdHandlerParameter,
                                mmsRequest, &mmsResponseBuffer);
                    }

                    // >> REMOVE CODE?
                    // handler code can call a sendMessage Method when required! (IsoConnection_sendMessage?)
                    /* send a response if required */
                    if (mmsResponseBuffer.size > 0) {

                        struct sBufferChain mmsBufferPartStruct;
						BufferChain mmsBufferPart = &mmsBufferPartStruct;

                        BufferChain_init(mmsBufferPart, mmsResponseBuffer.size,
                                mmsResponseBuffer.size, NULL, self->sendBuffer);

                        struct sBufferChain presentationBufferPartStruct;
                        BufferChain presentationBufferPart = &presentationBufferPartStruct;
                        presentationBufferPart->buffer = self->sendBuffer + mmsBufferPart->length;
                        presentationBufferPart->partMaxLength = SEND_BUF_SIZE - mmsBufferPart->length;

                        IsoPresentation_createUserData(self->presentation,
                                presentationBufferPart, mmsBufferPart);

                        struct sBufferChain sessionBufferPartStruct;
                        BufferChain sessionBufferPart = &sessionBufferPartStruct;
                        sessionBufferPart->buffer = self->sendBuffer + presentationBufferPart->length;
                        sessionBufferPart->partMaxLength = SEND_BUF_SIZE - presentationBufferPart->length;

                        IsoSession_createDataSpdu(self->session, sessionBufferPart, presentationBufferPart);

                        CotpConnection_sendDataMessage(self->cotpConnection, sessionBufferPart);
                    }
                    // << REMOVE CODE?

#if (CONFIG_MMS_THREADLESS_STACK != 1)
                    Semaphore_post(self->conMutex);
                    IsoServer_userUnlock(self->isoServer);
#endif
                }
                else {
                    if (DEBUG_ISO_SERVER)
                        libmms_iso_log(MMS_LOG_WARNING, "ISO_SERVER: unknown presentation layer context!");
                }

                break;

            case SESSION_FINISH:
                if (DEBUG_ISO_SERVER)
                    libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: session finish indication received");

                if (IsoPresentation_parseUserData(self->presentation, sessionUserData)) {

#if (CONFIG_MMS_THREADLESS_STACK != 1)
                    IsoServer_userLock(self->isoServer);
                    Semaphore_wait(self->conMutex);
#endif

                    struct sBufferChain acseBufferPartStruct;
                    BufferChain acseBufferPart = &acseBufferPartStruct;
                    acseBufferPart->buffer = self->sendBuffer;
                    acseBufferPart->partMaxLength = SEND_BUF_SIZE;

                    AcseConnection_createReleaseResponseMessage(self->acseConnection, acseBufferPart);

                    struct sBufferChain presentationBufferPartStruct;
                    BufferChain presentationBufferPart = &presentationBufferPartStruct;
                    presentationBufferPart->buffer = self->sendBuffer + acseBufferPart->length;
                    presentationBufferPart->partMaxLength = SEND_BUF_SIZE - acseBufferPart->length;

                    IsoPresentation_createUserDataACSE(self->presentation, presentationBufferPart, acseBufferPart);

                    struct sBufferChain sessionBufferPartStruct;
                    BufferChain sessionBufferPart = &sessionBufferPartStruct;
                    sessionBufferPart->buffer = self->sendBuffer + presentationBufferPart->length;
                    sessionBufferPart->partMaxLength = SEND_BUF_SIZE - presentationBufferPart->length;

                    IsoSession_createDisconnectSpdu(self->session, sessionBufferPart, presentationBufferPart);

                    CotpConnection_sendDataMessage(self->cotpConnection, sessionBufferPart);

#if (CONFIG_MMS_THREADLESS_STACK != 1)
                    Semaphore_post(self->conMutex);
                    IsoServer_userUnlock(self->isoServer);
#endif
                }

                self->state = ISO_CON_STATE_STOPPED;

                break;

            case SESSION_ABORT:
                if (DEBUG_ISO_SERVER)
                    libmms_iso_log(MMS_LOG_INFO, "ISO_SERVER: session abort received");
                self->state = ISO_CON_STATE_STOPPED;
                break;

            case SESSION_ERROR:
                if (DEBUG_ISO_SERVER)
                    libmms_iso_log(MMS_LOG_ERROR, "ISO_SERVER: session layer error");
                self->state = ISO_CON_STATE_STOPPED;
                break;

            default: /* illegal state */
                if (DEBUG_ISO_SERVER)
                    libmms_iso_log(MMS_LOG_ERROR, "ISO_SERVER: connection - session in illegal state");

                self->state = ISO_CON_STATE_STOPPED;
                break;
            }

            CotpConnection_resetPayload(self->cotpConnection);
        }
        break;
    case COTP_ERROR:
        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: Connection closed");
        self->state = ISO_CON_STATE_STOPPED;
        break;
    default:
        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_ERROR, "ISO_SERVER: COTP unknown indication: %i", cotpIndication);
        self->state = ISO_CON_STATE_STOPPED;
        break;
    }

exit_function:
    return;
}

#if ((CONFIG_MMS_SINGLE_THREADED == 0) && (CONFIG_MMS_THREADLESS_STACK == 0))
static void
handleTcpConnection(void* parameter)
{
    IsoConnection self = (IsoConnection) parameter;

    while(self->state == ISO_CON_STATE_RUNNING)
        IsoConnection_handleTcpConnection(self);

    finalizeIsoConnection(self);
}
#endif /* (CONFIG_MMS_SINGLE_THREADED == 0) */

IsoConnection
IsoConnection_create(Socket socket, IsoServer isoServer)
{
    IsoConnection self = (IsoConnection) GLOBAL_CALLOC(1, sizeof(struct sIsoConnection));

    if (self) {
        self->socket = socket;

    #if (CONFIG_MMS_SUPPORT_TLS == 1)
        if (IsoServer_getTLSConfiguration(isoServer) != NULL) {
            self->tlsSocket = TLSSocket_create(socket, IsoServer_getTLSConfiguration(isoServer), true);

            if (self->tlsSocket == NULL) {
                if (DEBUG_ISO_SERVER)
                    libmms_iso_log(MMS_LOG_WARNING, "ISO_SERVER: connection - TLS initialization failed");

                Socket_destroy(socket);

                GLOBAL_FREEMEM(self);

                return NULL;
            }
        }
    #endif /* (CONFIG_MMS_SUPPORT_TLS == 1) */

        self->receiveBuffer = (uint8_t*) GLOBAL_MALLOC(RECEIVE_BUF_SIZE);

        if (self->receiveBuffer == NULL)
            goto exit_memory_allocation_failed;

        self->sendBuffer = (uint8_t*) GLOBAL_MALLOC(SEND_BUF_SIZE);

        if (self->sendBuffer == NULL)
            goto exit_memory_allocation_failed;

        self->msgRcvdHandler = NULL;
        self->msgRcvdHandlerParameter = NULL;
        self->isoServer = isoServer;
        self->state = ISO_CON_STATE_RUNNING;
        self->clientAddress = Socket_getPeerAddress(self->socket);

    #if (CONFIG_MMS_THREADLESS_STACK != 1)
        self->conMutex = Semaphore_create(1);
    #endif

        ByteBuffer_wrap(&(self->rcvBuffer), self->receiveBuffer, 0, RECEIVE_BUF_SIZE);

        self->cotpReadBuf = (uint8_t*) GLOBAL_MALLOC(CONFIG_COTP_MAX_TPDU_SIZE + TPKT_RFC1006_HEADER_SIZE);

        if (self->cotpReadBuf == NULL)
            goto exit_memory_allocation_failed;

        self->cotpWriteBuf = (uint8_t*) GLOBAL_MALLOC(CONFIG_COTP_MAX_TPDU_SIZE + TPKT_RFC1006_HEADER_SIZE);

        if (self->cotpWriteBuf == NULL)
            goto exit_memory_allocation_failed;

        ByteBuffer_wrap(&(self->cotpReadBuffer), self->cotpReadBuf, 0, CONFIG_COTP_MAX_TPDU_SIZE + TPKT_RFC1006_HEADER_SIZE);
        ByteBuffer_wrap(&(self->cotpWriteBuffer), self->cotpWriteBuf, 0, CONFIG_COTP_MAX_TPDU_SIZE + TPKT_RFC1006_HEADER_SIZE);

        self->cotpConnection = (CotpConnection*) GLOBAL_CALLOC(1, sizeof(CotpConnection));

        if (self->cotpConnection == NULL)
            goto exit_memory_allocation_failed;

        CotpConnection_init(self->cotpConnection, self->socket, &(self->rcvBuffer), &(self->cotpReadBuffer), &(self->cotpWriteBuffer));

    #if (CONFIG_MMS_SUPPORT_TLS == 1)
        if (self->tlsSocket)
            self->cotpConnection->tlsSocket = self->tlsSocket;
    #endif /* (CONFIG_MMS_SUPPORT_TLS == 1) */

        self->session = (IsoSession*) GLOBAL_CALLOC(1, sizeof(IsoSession));

        if (self->session == NULL)
            goto exit_memory_allocation_failed;

        IsoSession_init(self->session);

        self->presentation = (IsoPresentation*) GLOBAL_CALLOC(1, sizeof(IsoPresentation));

        if (self->presentation == NULL)
            goto exit_memory_allocation_failed;

        IsoPresentation_initPassive(self->presentation, IsoServer_getLocalAppAddr(self->isoServer), &(self->peerApplicationAddress));

        self->acseConnection = (AcseConnection*) GLOBAL_CALLOC(1, sizeof(AcseConnection));

        if (self->acseConnection == NULL)
            goto exit_memory_allocation_failed;

    #if (CONFIG_MMS_SUPPORT_TLS == 1)
        AcseConnection_init(self->acseConnection, &(self->peerApplicationAddress), &(self->localApplicationAddress), IsoServer_getAuthenticator(self->isoServer),
                IsoServer_getAuthenticatorParameter(self->isoServer), self->tlsSocket);
    #else
        AcseConnection_init(self->acseConnection, &(self->peerApplicationAddress), &(self->localApplicationAddress), IsoServer_getAuthenticator(self->isoServer),
                IsoServer_getAuthenticatorParameter(self->isoServer), NULL);
    #endif

        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_INFO, "ISO_SERVER: New connection from client %s", self->clientAddress);

    #if (CONFIG_MMS_SINGLE_THREADED == 0)
    #if (CONFIG_MMS_THREADLESS_STACK == 0)
        self->thread = Thread_create((ThreadExecutionFunction) handleTcpConnection, self, true);

        Thread_start(self->thread);
    #endif
    #endif

        return self;
    }
    else {
        Socket_destroy(socket);
    }

exit_memory_allocation_failed:

    if (DEBUG_ISO_SERVER)
        libmms_iso_log(MMS_LOG_ERROR, "ISO_SERVER: IsoConnection - failed to allocate memory!");

    if (self)
        IsoConnection_destroy(self);

    return NULL;
}

void
IsoConnection_lockConnection(IsoConnection self)
{
#if (CONFIG_MMS_THREADLESS_STACK != 1)
        Semaphore_wait(self->conMutex);
#endif
}

void
IsoConnection_unlockConnection(IsoConnection self)
{
#if (CONFIG_MMS_THREADLESS_STACK != 1)
        Semaphore_post(self->conMutex);
#endif
}

void
IsoConnection_destroy(IsoConnection self)
{
#if (CONFIG_MMS_SUPPORT_TLS == 1)
   if (self->tlsSocket) {
       TLSSocket_close(self->tlsSocket);
       self->tlsSocket = NULL;
   }
#endif

    finalizeIsoConnection(self);
}

char*
IsoConnection_getPeerAddress(IsoConnection self)
{
    return self->clientAddress;
}

IsoApplicationAddress
IsoConnection_getPeerApplicationAddress(IsoConnection self)
{
    return &(self->peerApplicationAddress);
}

IsoApplicationAddress
IsoConnection_getCalledApplicationAddress(IsoConnection self)
{
    return &(self->localApplicationAddress);
}

void
IsoConnection_sendMessage(IsoConnection self, ByteBuffer* message)
{
    BufferChain payloadBuffer;
    BufferChain presentationBuffer;
    BufferChain sessionBuffer;
    CotpIndication indication;

    if (self->state == ISO_CON_STATE_STOPPED)
        goto exit_error;

#if 0
    bool locked = false;


#if (CONFIG_MMS_THREADLESS_STACK != 1)
    if (handlerMode == false) {
        Semaphore_wait(self->conMutex);
        locked = true;
    }
#endif
#endif

    struct sBufferChain payloadBufferStruct;
    payloadBuffer = &payloadBufferStruct;
    payloadBuffer->length = message->size;
    payloadBuffer->partLength = message->size;
    payloadBuffer->partMaxLength = message->size;
    payloadBuffer->buffer = message->buffer;
    payloadBuffer->nextPart = NULL;

    struct sBufferChain presentationBufferStruct;
    presentationBuffer = &presentationBufferStruct;
    presentationBuffer->buffer = self->sendBuffer;
    presentationBuffer->partMaxLength = SEND_BUF_SIZE;

    IsoPresentation_createUserData(self->presentation,
            presentationBuffer, payloadBuffer);

    struct sBufferChain sessionBufferStruct;
    sessionBuffer = &sessionBufferStruct;
    sessionBuffer->buffer = self->sendBuffer + presentationBuffer->partLength;

    IsoSession_createDataSpdu(self->session, sessionBuffer, presentationBuffer);

    indication = CotpConnection_sendDataMessage(self->cotpConnection, sessionBuffer);

    if (indication != COTP_OK) {
        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_WARNING, "ISO_SERVER: IsoConnection_sendMessage failed! --> close connection\n");

        IsoConnection_close(self);
    }

#if 0
#if (CONFIG_MMS_THREADLESS_STACK != 1)
    if (locked)
        Semaphore_post(self->conMutex);
#endif
#endif

exit_error:
    return;
}

void
IsoConnection_close(IsoConnection self)
{
    if (self->state != ISO_CON_STATE_STOPPED) {
        Socket socket = self->socket;
        self->state = ISO_CON_STATE_STOPPED;
        self->socket = NULL;

#if (CONFIG_MMS_SUPPORT_TLS == 1)
        if (self->tlsSocket) {
            TLSSocket_close(self->tlsSocket);
            self->tlsSocket = NULL;
        }
#endif

        Socket_destroy(socket);
    }
}

void
IsoConnection_installListener(IsoConnection self, MessageReceivedHandler handler,
        void* parameter)
{
    self->msgRcvdHandler = handler;
    self->msgRcvdHandlerParameter = parameter;
}

void*
IsoConnection_getSecurityToken(IsoConnection self)
{
    return self->acseConnection->securityToken;
}

bool
IsoConnection_isRunning(IsoConnection self)
{
    if (self->state == ISO_CON_STATE_RUNNING)
        return true;
    else
        return false;
}
