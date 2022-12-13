/*
 *  iso_client_connection.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "stack_config.h"
#include "mms_common_internal.h"
#include "hal_socket.h"
#include "hal_thread.h"
#include "cotp.h"
#include "iso_session.h"
#include "iso_presentation.h"
#include "iso_client_connection.h"

#include "libmms_platform_includes.h"
#include "tls_config.h"
#include "acse.h"


#ifndef DEBUG_ISO_CLIENT
#ifdef DEBUG
#define DEBUG_ISO_CLIENT 1
#else
#define DEBUG_ISO_CLIENT 0
#endif /*DEBUG */
#endif /* DEBUG_ISO_SERVER */

#define STATE_IDLE 0
#define STATE_CONNECTED 1
#define STATE_ERROR 2
#define STATE_CONNECTING 3

#define TPKT_RFC1006_HEADER_SIZE 4

#define ISO_CLIENT_BUFFER_SIZE CONFIG_MMS_MAXIMUM_PDU_SIZE + 100

typedef enum {
    INT_STATE_IDLE,
    INT_STATE_TCP_CONNECTING,
    INT_STATE_WAIT_FOR_COTP_CONNECT_RESP,
    INT_STATE_WAIT_FOR_ACSE_RESP,
    INT_STATE_WAIT_FOR_DATA_MSG,
    INT_STATE_CLOSING_CONNECTION,
    INT_STATE_CLOSE_ON_ERROR,
    INT_STATE_ERROR
} eIsoClientInternalState;

struct sIsoClientConnection
{
    IsoApplicationAddress localAppAddr;
    IsoApplicationAddress remoteAppAddr;

    TLSConfiguration tlsConfiguration;

    AcseAuthenticationParameter authParameter;

    IsoIndicationCallback callback;
    void* callbackParameter;

    volatile eIsoClientInternalState intState;
    volatile int state;
    Semaphore stateMutex;

    uint64_t nextReadTimeout; /* timeout value for read and connect */

    Socket socket;

#if (CONFIG_MMS_SUPPORT_TLS == 1)
    TLSSocket tlsSocket;
#endif

    CotpConnection* cotpConnection;
    IsoPresentation* presentation;
    IsoSession* session;
    AcseConnection acseConnection;

    uint8_t* sendBuffer; /* ISO/MMS send buffer */
    uint8_t* receiveBuf; /* ISO/MMS receive buffer */
    ByteBuffer* receiveBuffer;

    ByteBuffer* transmitPayloadBuffer;
    Semaphore transmitBufferMutex;

    ByteBuffer* receivePayloadBuffer;

    Semaphore tickMutex;

    /* remote app address provided by peer (responded-AP-title, ...) */
    struct sIsoApplicationAddress peerAppAddr;

    AcseAuthenticator acseAuthenticator;
    void* acseAuthenticatorParameter;

    /* prevent conflict when messages are sent from different threads */
    Semaphore sendMessageMutex;

    uint8_t* cotpReadBuf;
    uint8_t* cotpWriteBuf;

    ByteBuffer* cotpReadBuffer;
    ByteBuffer* cotpWriteBuffer;

    uint32_t requestTimeout;
};

static void
setState(IsoClientConnection self, int newState)
{
    Semaphore_wait(self->stateMutex);
    self->state = newState;
    Semaphore_post(self->stateMutex);
}

static int
getState(IsoClientConnection self)
{
    int stateVal;

    Semaphore_wait(self->stateMutex);
    stateVal = self->state;
    Semaphore_post(self->stateMutex);

    return stateVal;
}

static inline void
setIntState(IsoClientConnection self, eIsoClientInternalState newState)
{
    self->intState = newState;
}

static inline eIsoClientInternalState
getIntState(IsoClientConnection self)
{
    return self->intState;
}

IsoClientConnection
IsoClientConnection_create(TLSConfiguration tlsConfiguration, IsoApplicationAddress localAddr, IsoApplicationAddress remoteAddr, IsoIndicationCallback callback, void* callbackParameter)
{
    IsoClientConnection self = (IsoClientConnection) GLOBAL_CALLOC(1, sizeof(struct sIsoClientConnection));

    if (self) {
        self->localAppAddr = localAddr;
        self->remoteAppAddr = remoteAddr;

        self->tlsConfiguration = tlsConfiguration;

        self->callback = callback;
        self->callbackParameter = callbackParameter;

        self->authParameter = NULL;

        self->intState = INT_STATE_IDLE;
        self->state = STATE_IDLE;
        self->stateMutex = Semaphore_create(1);

        self->sendBuffer = (uint8_t*) GLOBAL_MALLOC(ISO_CLIENT_BUFFER_SIZE);

        self->transmitPayloadBuffer = (ByteBuffer*) GLOBAL_CALLOC(1, sizeof(ByteBuffer));
        self->transmitPayloadBuffer->buffer = self->sendBuffer;
        self->transmitPayloadBuffer->maxSize = ISO_CLIENT_BUFFER_SIZE;

        self->receivePayloadBuffer = (ByteBuffer*) GLOBAL_CALLOC(1, sizeof(ByteBuffer));

        self->transmitBufferMutex = Semaphore_create(1);

        self->tickMutex = Semaphore_create(1);

        self->sendMessageMutex = Semaphore_create(1);

        self->receiveBuf = (uint8_t*) GLOBAL_MALLOC(ISO_CLIENT_BUFFER_SIZE);
        self->receiveBuffer = (ByteBuffer*) GLOBAL_CALLOC(1, sizeof(ByteBuffer));
        ByteBuffer_wrap(self->receiveBuffer, self->receiveBuf, 0, ISO_CLIENT_BUFFER_SIZE);

        self->presentation = (IsoPresentation*) GLOBAL_CALLOC(1, sizeof(IsoPresentation));

        self->session = (IsoSession*) GLOBAL_CALLOC(1, sizeof(IsoSession));

        self->cotpReadBuf = (uint8_t*) GLOBAL_MALLOC(CONFIG_COTP_MAX_TPDU_SIZE + TPKT_RFC1006_HEADER_SIZE);
        self->cotpWriteBuf = (uint8_t*) GLOBAL_MALLOC(CONFIG_COTP_MAX_TPDU_SIZE + TPKT_RFC1006_HEADER_SIZE);

        self->cotpReadBuffer = (ByteBuffer*) GLOBAL_CALLOC(1, sizeof(ByteBuffer));
        ByteBuffer_wrap(self->cotpReadBuffer, self->cotpReadBuf, 0, CONFIG_COTP_MAX_TPDU_SIZE + TPKT_RFC1006_HEADER_SIZE);

        self->cotpWriteBuffer = (ByteBuffer*) GLOBAL_CALLOC(1, sizeof(ByteBuffer));
        ByteBuffer_wrap(self->cotpWriteBuffer, self->cotpWriteBuf, 0, CONFIG_COTP_MAX_TPDU_SIZE + TPKT_RFC1006_HEADER_SIZE);

        self->cotpConnection = (CotpConnection*) GLOBAL_CALLOC(1, sizeof(CotpConnection));

        self->requestTimeout = CONFIG_TCP_READ_TIMEOUT_MS;
    }

    return self;
}

void
IsoClientConnection_setAuthenticator(IsoClientConnection self, AcseAuthenticator acseAuthenticator, void* acseAuthenticatorParameter)
{
    self->acseAuthenticator = acseAuthenticator;
    self->acseAuthenticatorParameter = acseAuthenticatorParameter;
}

void
IsoClientConnection_setTLSConfiguration(IsoClientConnection self, TLSConfiguration tlsConfig)
{
    self->tlsConfiguration = tlsConfig;
}

static bool
sendConnectionRequestMessage(IsoClientConnection self)
{
    /* COTP (ISO transport) handshake */
    CotpConnection_init(self->cotpConnection, self->socket, self->receiveBuffer, self->cotpReadBuffer, self->cotpWriteBuffer);

#if (CONFIG_MMS_SUPPORT_TLS == 1)
    if (self->tlsConfiguration) {

        TLSConfiguration_setClientMode(self->tlsConfiguration);

        /* create TLSSocket and start TLS authentication */
        TLSSocket tlsSocket = TLSSocket_create(self->socket, self->tlsConfiguration, false);

        if (tlsSocket)
            self->cotpConnection->tlsSocket = tlsSocket;
        else {

            if (DEBUG_ISO_CLIENT)
                libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: TLS handshake failed!");

            return false;
        }
    }
#endif /* (CONFIG_MMS_SUPPORT_TLS == 1) */


    /* COTP (ISO transport) handshake */
    CotpIndication cotpIndication =
            CotpConnection_sendConnectionRequestMessage(self->cotpConnection, self->remoteAppAddr, self->localAppAddr);

    if (cotpIndication != COTP_OK)
        return false;
    else
        return true;
}

static void
sendAcseInitiateRequest(IsoClientConnection self)
{
    Semaphore_wait(self->sendMessageMutex);

    /* Upper layers handshake */
    struct sBufferChain sAcsePayload;
    BufferChain acsePayload = &sAcsePayload;
    acsePayload->buffer = self->transmitPayloadBuffer->buffer;
    acsePayload->partLength = self->transmitPayloadBuffer->size;
    acsePayload->length = self->transmitPayloadBuffer->size;
    acsePayload->nextPart = NULL;

    if (self->remoteAppAddr) {
        /* allow server to be authenticated even when no response addresses are sent */
        memcpy(&(self->peerAppAddr), self->remoteAppAddr, sizeof(struct sIsoApplicationAddress));
    }
    else {
        memset(&(self->peerAppAddr), 0, sizeof(struct sIsoApplicationAddress));
    }

    AcseConnection_init(&(self->acseConnection), &(self->peerAppAddr), self->localAppAddr, self->acseAuthenticator, self->acseAuthenticatorParameter, NULL);

    struct sBufferChain sAcseBuffer;
    BufferChain acseBuffer = &sAcseBuffer;

    acseBuffer->buffer = self->sendBuffer + self->transmitPayloadBuffer->size;
    acseBuffer->partMaxLength = ISO_CLIENT_BUFFER_SIZE - acsePayload->length;

    AcseConnection_createAssociateRequestMessage(&(self->acseConnection), self->remoteAppAddr, self->localAppAddr, acseBuffer, acsePayload,
            self->authParameter);

    struct sBufferChain sPresentationBuffer;
    BufferChain presentationBuffer = &sPresentationBuffer;

    presentationBuffer->buffer = self->sendBuffer + acseBuffer->length;
    presentationBuffer->partMaxLength = ISO_CLIENT_BUFFER_SIZE - acseBuffer->length;

    IsoPresentation_initActive(self->presentation, self->localAppAddr, self->remoteAppAddr, &(self->peerAppAddr));
    IsoPresentation_createConnectPdu(self->presentation, presentationBuffer, acseBuffer);

    struct sBufferChain sSessionBuffer;
    BufferChain sessionBuffer = &sSessionBuffer;
    sessionBuffer->buffer = self->sendBuffer + presentationBuffer->length;

    IsoSession_init(self->session);
    IsoSession_createConnectSpdu(self->session, self->remoteAppAddr, self->localAppAddr, sessionBuffer,
            presentationBuffer);

    CotpConnection_sendDataMessage(self->cotpConnection, sessionBuffer);

    Semaphore_post(self->sendMessageMutex);

    Semaphore_post(self->transmitBufferMutex);
}


static void
releaseSocket(IsoClientConnection self)
{
    if (self->socket) {

#if (CONFIG_MMS_SUPPORT_TLS == 1)
    if (self->cotpConnection->tlsSocket) {
        TLSSocket_close(self->cotpConnection->tlsSocket);
        self->cotpConnection->tlsSocket = NULL;
    }
#endif

        Socket_destroy(self->socket);
        self->socket = NULL;
    }
}

/*
 * Connection state machine
 *
 * called by tick function
 *
 * \return value indicates that connection is currently waiting and calling thread can be suspended
 */
bool
IsoClientConnection_handleConnection(IsoClientConnection self)
{
    Semaphore_wait(self->tickMutex);

    bool waits = false;

    eIsoClientInternalState currentState = getIntState(self);

    eIsoClientInternalState nextState = currentState;

    switch (currentState) {

    case INT_STATE_IDLE:
    case INT_STATE_ERROR:
        waits = true;
        break;

    case INT_STATE_TCP_CONNECTING:
        {
            SocketState socketState = Socket_checkAsyncConnectState(self->socket);

            if (socketState == SOCKET_STATE_CONNECTED) {

                if (DEBUG_ISO_CLIENT)
                    libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: TCP connection established");

                if (sendConnectionRequestMessage(self)) {
                    self->nextReadTimeout = Hal_getTimeInMs() + self->requestTimeout;
                    nextState = INT_STATE_WAIT_FOR_COTP_CONNECT_RESP;
                }
                else {
                    if (DEBUG_ISO_CLIENT)
                        libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Failed to send COTP CR");

                    IsoClientConnection_releaseTransmitBuffer(self);
                    self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);
                    nextState = INT_STATE_CLOSE_ON_ERROR;
                }
            }
            else if (socketState == SOCKET_STATE_FAILED) {
                IsoClientConnection_releaseTransmitBuffer(self);
                self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);
                nextState = INT_STATE_CLOSE_ON_ERROR;
            }
            else {

                /* check connect timeout */

                uint64_t currentTime = Hal_getTimeInMs();

                if (currentTime > self->nextReadTimeout) {

                    if (DEBUG_ISO_CLIENT)
                        libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: connect timeout");

                    IsoClientConnection_releaseTransmitBuffer(self);
                    self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);
                    nextState = INT_STATE_CLOSE_ON_ERROR;
                }
                else {
                    waits = true;
                }

            }

        }
        break;

    case INT_STATE_WAIT_FOR_COTP_CONNECT_RESP:
        {
            uint64_t currentTime = Hal_getTimeInMs();

            if (currentTime > self->nextReadTimeout) {

                if (DEBUG_ISO_CLIENT)
                    libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Timeout waiting for COTP CR");

                IsoClientConnection_releaseTransmitBuffer(self);

                self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

                nextState = INT_STATE_CLOSE_ON_ERROR;
            }
            else {

                TpktState packetState = CotpConnection_readToTpktBuffer(self->cotpConnection);

                if (packetState == TPKT_PACKET_COMPLETE) {

                    CotpIndication cotpIndication = CotpConnection_parseIncomingMessage(self->cotpConnection);

                    if (cotpIndication != COTP_CONNECT_INDICATION) {
                        if (DEBUG_ISO_CLIENT)
                            libmms_iso_log(MMS_LOG_DEBUG, "ISO_CLIENT: Unexpected COTP state (%i)", cotpIndication);

                        /* TODO get COTP error diagnostics */

                        IsoClientConnection_releaseTransmitBuffer(self);

                        self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

                        nextState = INT_STATE_CLOSE_ON_ERROR;
                    }
                    else {
                        if (DEBUG_ISO_CLIENT)
                            libmms_iso_log(MMS_LOG_DEBUG, "ISO_CLIENT: Received COTP CR");

                        sendAcseInitiateRequest(self);

                        self->nextReadTimeout = Hal_getTimeInMs() + self->requestTimeout;

                        nextState = INT_STATE_WAIT_FOR_ACSE_RESP;
                    }
                }
                else if (packetState == TPKT_ERROR) {
                    if (DEBUG_ISO_CLIENT)
                        libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Error receiving COTP message");

                    IsoClientConnection_releaseTransmitBuffer(self);

                    self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

                    nextState = INT_STATE_CLOSE_ON_ERROR;
                }
                else {
                    waits = true;
                }

            }
        }
        break;

    case INT_STATE_WAIT_FOR_ACSE_RESP:
        {
            uint64_t currentTime = Hal_getTimeInMs();

            if (currentTime > self->nextReadTimeout) {

                if (DEBUG_ISO_CLIENT)
                    libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Timeout waiting for ACSE initiate response");

                self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

                nextState = INT_STATE_CLOSE_ON_ERROR;
            }
            else {

                TpktState packetState = CotpConnection_readToTpktBuffer(self->cotpConnection);

                if (packetState == TPKT_PACKET_COMPLETE) {

                    CotpIndication cotpIndication = CotpConnection_parseIncomingMessage(self->cotpConnection);

                    if (cotpIndication == COTP_DATA_INDICATION) {

                        /* parse ACSE response */

                       IsoSessionIndication sessionIndication;

                       sessionIndication =
                               IsoSession_parseMessage(self->session, CotpConnection_getPayload(self->cotpConnection));

                       if (sessionIndication != SESSION_CONNECT) {
                           if (DEBUG_ISO_CLIENT)
                               libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: no session connect indication");

                           self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

                           nextState = INT_STATE_CLOSE_ON_ERROR;
                       }
                       else {

                           if (IsoPresentation_parseAcceptMessage(self->presentation, IsoSession_getUserData(self->session)) == false) {

                               if (DEBUG_ISO_CLIENT)
                                   libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: no presentation ok indication");

                               /* TODO get presentation error diagnostics */

                               self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

                               nextState = INT_STATE_CLOSE_ON_ERROR;
                           }
                           else {

                               AcseIndication acseIndication = AcseConnection_parseMessage(&(self->acseConnection), &self->presentation->nextPayload, self->socket);

                               if (acseIndication != ACSE_ASSOCIATE) {
                                   if (DEBUG_ISO_CLIENT)
                                       libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: no ACSE_ASSOCIATE indication");

                                   self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

                                   nextState = INT_STATE_CLOSE_ON_ERROR;
                               }
                               else {
                                   if (DEBUG_ISO_CLIENT)
                                       libmms_iso_log(MMS_LOG_INFO, "ISO_CLIENT: ACSE AARE - association accepted");
                               }

                           }

                           ByteBuffer_wrap(self->receivePayloadBuffer, self->acseConnection.userDataBuffer,
                                   self->acseConnection.userDataBufferSize, self->acseConnection.userDataBufferSize);

                           setState(self, STATE_CONNECTED);
                           nextState = INT_STATE_WAIT_FOR_DATA_MSG;

                           self->callback(ISO_IND_ASSOCIATION_SUCCESS, self->callbackParameter, self->receivePayloadBuffer);

                           CotpConnection_resetPayload(self->cotpConnection);
                       }
                    }
                    else if (cotpIndication == COTP_MORE_FRAGMENTS_FOLLOW) {
                        waits = true;
                    }
                    else {
                        if (DEBUG_ISO_CLIENT)
                            libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Unexpected COTP state (%i)", cotpIndication);

                        self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

                        nextState = INT_STATE_CLOSE_ON_ERROR;
                    }
                }
                else if (packetState == TPKT_ERROR) {
                    if (DEBUG_ISO_CLIENT)
                        libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Error receiving COTP message");

                    self->callback(ISO_IND_ASSOCIATION_FAILED, self->callbackParameter, NULL);

                    nextState = INT_STATE_CLOSE_ON_ERROR;
                }
                else {
                    waits = true;
                }

            }
        }
        break;

    case INT_STATE_WAIT_FOR_DATA_MSG:
        {
            TpktState packetState = CotpConnection_readToTpktBuffer(self->cotpConnection);

            if (packetState == TPKT_ERROR) {
                nextState = INT_STATE_CLOSE_ON_ERROR;
            }
            else if (packetState == TPKT_PACKET_COMPLETE) {

                CotpIndication cotpIndication = CotpConnection_parseIncomingMessage(self->cotpConnection);

                switch (cotpIndication) {

                case COTP_MORE_FRAGMENTS_FOLLOW:
                    break;

                case COTP_DISCONNECT_INDICATION:
                    {
                        nextState = INT_STATE_CLOSING_CONNECTION;
                    }
                    break;

                case COTP_DATA_INDICATION:
                    {
                        IsoSessionIndication sessionIndication =
                                IsoSession_parseMessage(self->session,
                                        CotpConnection_getPayload(self->cotpConnection));

                        if (sessionIndication != SESSION_DATA) {
                            if (DEBUG_ISO_CLIENT)
                                libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Invalid session message");

                            nextState = INT_STATE_CLOSE_ON_ERROR;
                        }
                        else {

                            if (!IsoPresentation_parseUserData(self->presentation, IsoSession_getUserData(self->session))) {

                                if (DEBUG_ISO_CLIENT)
                                    libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Invalid presentation message");

                                nextState = INT_STATE_CLOSE_ON_ERROR;
                            }
                            else {

                                self->callback(ISO_IND_DATA, self->callbackParameter,
                                        &(self->presentation->nextPayload));

                                CotpConnection_resetPayload(self->cotpConnection);
                            }
                        }
                    }
                    break;

                default:
                    {
                        nextState = INT_STATE_CLOSE_ON_ERROR;
                    }
                    break;

                }
            }
            else {
                waits = true;
            }

        }
        break;

    case INT_STATE_CLOSE_ON_ERROR:
        {
            setState(self, STATE_ERROR);

            self->callback(ISO_IND_CLOSED, self->callbackParameter, NULL);

            releaseSocket(self);

            nextState = INT_STATE_ERROR;
        }
        break;

    case INT_STATE_CLOSING_CONNECTION:
        {
            setState(self, STATE_IDLE);

            self->callback(ISO_IND_CLOSED, self->callbackParameter, NULL);;

            releaseSocket(self);

            nextState = INT_STATE_IDLE;
        }
        break;

    default:

        if (DEBUG_ISO_CLIENT)
            libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Illegal state");

        break;

    }

    self->callback(ISO_IND_TICK, self->callbackParameter, NULL);

    setIntState(self, nextState);

    Semaphore_post(self->tickMutex);

    return waits;
}


bool
IsoClientConnection_associateAsync(IsoClientConnection self, const char* hostname, int tcpPort, uint32_t connectTimeoutInMs, uint32_t requestTimeout)
{
    bool success = true;

    self->requestTimeout = requestTimeout;

    /* Create socket and start connect */
    setState(self, STATE_CONNECTING);

    Semaphore_wait(self->tickMutex);

    self->socket = TcpSocket_create();

#if (CONFIG_ACTIVATE_TCP_KEEPALIVE == 1)
    Socket_activateTcpKeepAlive(self->socket,
            CONFIG_TCP_KEEPALIVE_IDLE,
            CONFIG_TCP_KEEPALIVE_INTERVAL,
            CONFIG_TCP_KEEPALIVE_CNT);
#endif

    setIntState(self, INT_STATE_TCP_CONNECTING);

    /* set timeout for connect */
    self->nextReadTimeout = Hal_getTimeInMs() + connectTimeoutInMs;

    if (Socket_connectAsync(self->socket, hostname, tcpPort) == false) {

        Socket_destroy(self->socket);
        self->socket = NULL;

        setIntState(self, INT_STATE_ERROR);
        setState(self, STATE_ERROR);

        IsoClientConnection_releaseTransmitBuffer(self);

        success = false;
    }

    Semaphore_post(self->tickMutex);

    return success;
}

void
IsoClientConnection_sendMessage(IsoClientConnection self, ByteBuffer* payloadBuffer)
{
    uint8_t _buf[100];

    if (getState(self) == STATE_CONNECTED) {
        Semaphore_wait(self->sendMessageMutex);

        struct sBufferChain payloadBCMemory;
        BufferChain payload = &payloadBCMemory;

        BufferChain_init(payload, payloadBuffer->size, payloadBuffer->size, NULL, payloadBuffer->buffer);

        struct sBufferChain presentationBCMemory;
        BufferChain presentationBuffer = &presentationBCMemory;

        presentationBuffer->buffer = _buf;
        presentationBuffer->partMaxLength = 100;

        IsoPresentation_createUserData(self->presentation, presentationBuffer, payload);

        struct sBufferChain sessionBufferBCMemory;
        BufferChain sessionBuffer = &sessionBufferBCMemory;

        IsoSession_createDataSpdu(self->session, sessionBuffer, presentationBuffer);

        CotpIndication indication = CotpConnection_sendDataMessage(self->cotpConnection, sessionBuffer);

        Semaphore_post(self->sendMessageMutex);

        if (indication != COTP_OK) {
            if (DEBUG_ISO_CLIENT)
                libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: IsoClientConnection_sendMessage: send message failed!");

            /* close connection */
            setIntState(self, INT_STATE_CLOSE_ON_ERROR);
        }
    }
    else {
        if (DEBUG_ISO_CLIENT)
            libmms_iso_log(MMS_LOG_ERROR, "ISO_CLIENT: Not connected --> cannot send message\n");
    }
}

void
IsoClientConnection_close(IsoClientConnection self)
{
    if (DEBUG_ISO_CLIENT)
        libmms_iso_log(MMS_LOG_INFO, "ISO_CLIENT: IsoClientConnection_close");

    Semaphore_wait(self->tickMutex);

    eIsoClientInternalState intState = getIntState(self);

    if ((intState != INT_STATE_IDLE) && (intState != INT_STATE_ERROR) && (intState != INT_STATE_CLOSE_ON_ERROR)) {

        if ((intState == INT_STATE_TCP_CONNECTING) || (intState == INT_STATE_WAIT_FOR_COTP_CONNECT_RESP))
            IsoClientConnection_releaseTransmitBuffer(self);

        setIntState(self, INT_STATE_CLOSING_CONNECTION);

        Semaphore_post(self->tickMutex);

        IsoClientConnection_handleConnection(self);
        setState(self, STATE_IDLE);
    }
    else {
        Semaphore_post(self->tickMutex);
    }
}


void
IsoClientConnection_destroy(IsoClientConnection self)
{
    if (DEBUG_ISO_CLIENT)
        libmms_iso_log(MMS_LOG_DEBUG, "ISO_CLIENT: IsoClientConnection_destroy");

    int state = getState(self);

    if (state == STATE_CONNECTED) {

        IsoClientConnection_close(self);
    }

    releaseSocket(self);

    if (self->receiveBuf != NULL)
        GLOBAL_FREEMEM(self->receiveBuf);
    if (self->receiveBuffer != NULL)
        GLOBAL_FREEMEM(self->receiveBuffer);
    if (self->cotpConnection != NULL)
        GLOBAL_FREEMEM(self->cotpConnection);

    if (self->cotpReadBuffer != NULL)
        GLOBAL_FREEMEM(self->cotpReadBuffer);

    if (self->cotpReadBuf != NULL)
        GLOBAL_FREEMEM(self->cotpReadBuf);

    if (self->cotpWriteBuffer != NULL)
        GLOBAL_FREEMEM(self->cotpWriteBuffer);

    if (self->cotpWriteBuf != NULL)
        GLOBAL_FREEMEM(self->cotpWriteBuf);

    if (self->session != NULL)
        GLOBAL_FREEMEM(self->session);
    if (self->presentation != NULL)
        GLOBAL_FREEMEM(self->presentation);

    GLOBAL_FREEMEM(self->transmitPayloadBuffer);
    GLOBAL_FREEMEM(self->receivePayloadBuffer);

    Semaphore_destroy(self->transmitBufferMutex);
    Semaphore_destroy(self->stateMutex);
    Semaphore_destroy(self->tickMutex);
    Semaphore_destroy(self->sendMessageMutex);

    GLOBAL_FREEMEM(self->sendBuffer);
    GLOBAL_FREEMEM(self);
}

static void
sendAbortMessage(IsoClientConnection self)
{
    IsoClientConnection_allocateTransmitBuffer(self);

    struct sBufferChain sAcseBuffer;
    BufferChain acseBuffer = &sAcseBuffer;
    acseBuffer->partMaxLength = ISO_CLIENT_BUFFER_SIZE;
    acseBuffer->buffer = self->sendBuffer;
    acseBuffer->nextPart = NULL;

    AcseConnection_createAbortMessage(NULL, acseBuffer, false);

    struct sBufferChain sPresentationBuffer;
    BufferChain presentationBuffer = &sPresentationBuffer;
    presentationBuffer->partMaxLength = ISO_CLIENT_BUFFER_SIZE - acseBuffer->length;
    presentationBuffer->buffer = self->sendBuffer + acseBuffer->length;
    presentationBuffer->nextPart = acseBuffer;

    IsoPresentation_createAbortUserMessage(self->presentation, presentationBuffer, acseBuffer);

    struct sBufferChain sSessionBuffer;
    BufferChain sessionBuffer = &sSessionBuffer;
    sessionBuffer->partMaxLength = ISO_CLIENT_BUFFER_SIZE - presentationBuffer->length;
    sessionBuffer->buffer = self->sendBuffer + presentationBuffer->length;
    sessionBuffer->nextPart = presentationBuffer;

    IsoSession_createAbortSpdu(self->session, sessionBuffer, presentationBuffer);

    CotpConnection_sendDataMessage(self->cotpConnection, sessionBuffer);

    Semaphore_post(self->transmitBufferMutex);
}

void
IsoClientConnection_abortAsync(IsoClientConnection self)
{
    sendAbortMessage(self);
}

void
IsoClientConnection_release(IsoClientConnection self)
{
    /* TODO block other messages from being sent */
    IsoClientConnection_allocateTransmitBuffer(self);

    struct sBufferChain sAcseBuffer;
    BufferChain acseBuffer = &sAcseBuffer;
    acseBuffer->partMaxLength = ISO_CLIENT_BUFFER_SIZE;
    acseBuffer->buffer = self->sendBuffer;
    acseBuffer->nextPart = NULL;

    AcseConnection_createReleaseRequestMessage(NULL, acseBuffer);

    struct sBufferChain sPresentationBuffer;
    BufferChain presentationBuffer = &sPresentationBuffer;
    presentationBuffer->partMaxLength = ISO_CLIENT_BUFFER_SIZE - acseBuffer->length;
    presentationBuffer->buffer = self->sendBuffer + acseBuffer->length;
    presentationBuffer->nextPart = acseBuffer;

    IsoPresentation_createUserDataACSE(self->presentation, presentationBuffer, acseBuffer);

    struct sBufferChain sSessionBuffer;
    BufferChain sessionBuffer = &sSessionBuffer;
    sessionBuffer->partMaxLength = ISO_CLIENT_BUFFER_SIZE - presentationBuffer->length;
    sessionBuffer->buffer = self->sendBuffer + presentationBuffer->length;
    sessionBuffer->nextPart = presentationBuffer;

    IsoSession_createFinishSpdu(NULL, sessionBuffer, presentationBuffer);

    CotpConnection_sendDataMessage(self->cotpConnection, sessionBuffer);

    Semaphore_post(self->transmitBufferMutex);
}


ByteBuffer*
IsoClientConnection_allocateTransmitBuffer(IsoClientConnection self)
{
    Semaphore_wait(self->transmitBufferMutex);
    self->transmitPayloadBuffer->size = 0;
    self->transmitPayloadBuffer->maxSize = ISO_CLIENT_BUFFER_SIZE;
    return self->transmitPayloadBuffer;
}

void
IsoClientConnection_releaseTransmitBuffer(IsoClientConnection self)
{
    Semaphore_post(self->transmitBufferMutex);
}

char*
IsoClientConnection_getPeerAddress(IsoClientConnection self)
{
    return Socket_getPeerAddress(self->socket);
}

IsoApplicationAddress
IsoClientConnection_getPeerApplicationAddress(IsoClientConnection self)
{
    return self->remoteAppAddr;
}

void*
IsoClientConnection_getSecurityToken(IsoClientConnection self)
{
    return self->acseConnection.securityToken;
}

bool
IsoClientConnection_isRunning(IsoClientConnection self)
{
    if ((getState(self) == STATE_CONNECTING) || (getState(self) == STATE_CONNECTED))
        return true;
    else
        return false;
}
