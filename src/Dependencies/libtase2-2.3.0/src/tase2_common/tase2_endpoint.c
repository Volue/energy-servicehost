/*
 *  tase2_endpoint.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "tase2_common_internal.h"
#include "tase2_server_internal.h"
#include "tase2_client_internal.h"
#include "tase2_endpoint.h"
#include "mms_endpoint.h"

#include "tase2_runtime_check.h"

#define MATCH_AP_TITLE 1
#define MATCH_AE_QUALIFIER 2
#define MATCH_P_SEL 4
#define MATCH_S_SEL 8
#define MATCH_T_SEL 16

static int endpointId = 0;

typedef struct sInformationReportHandlerInfo* InformationReportHandlerInfo;

struct sInformationReportHandlerInfo
{
    MmsInformationReportHandler handler;
    void* parameter;
    Tase2_Client client;
};

typedef struct sAppTickHandlerInfo* AppTickHandlerInfo;

struct sAppTickHandlerInfo
{
    MmsAppTickHandler handler;
    void* parameter;
};

struct sTase2_Endpoint
{
    MmsEndpoint mmsEndpoint;

	char* id;

    char* idVendor;
    char* idModel;
    char* idRevision;

    LinkedList servers; /* list of Tase2_Server instances */
    Semaphore serversLock;

    LinkedList clients; /* list of Tase2_Client instances */
    Semaphore clientsLock;

    LinkedList reportHandlers;
    LinkedList appTickHandlers;

    Semaphore endpointConnectionsLock;
    LinkedList endpointConnections; /* list of active endpoint connections */

    uint8_t matchingRules;

    Tase2_Endpoint_StateChangedHandler stateChangedHandler;
    void* stateChangedHandlerParameter;

    Tase2_Endpoint_ConnectionHandler connectionHandler;
    void* connectionHandlerParameter;

    Tase2_Endpoint_ConnectionAcceptHandler connectionAcceptHandler;
    void* connectionAcceptHandlerParameter;
};

struct sTase2_Endpoint_Connection
{
    MmsEndpointConnection mmsConnection;
    struct sIsoApplicationAddress applicationAddress;
    char* apTitleStr;
};

static void
mmsInformationReportHandler (MmsEndpointConnection connection, void* parameter, char* domainName,
        char* variableName, MmsValue* value, uint8_t flags)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Endpoint self = (Tase2_Endpoint) parameter;

    LinkedList element = LinkedList_getNext(self->reportHandlers);

    while (element) {

        InformationReportHandlerInfo reportHandlerInfo = (InformationReportHandlerInfo) LinkedList_getData(element);

        if (reportHandlerInfo->client) {
            /* check if MmsEndpointConnection is matching with client */

            if (MmsClient_getEndpointConnection(reportHandlerInfo->client->mmsClient) == connection) {
                reportHandlerInfo->handler(connection, reportHandlerInfo->parameter, domainName, variableName, value, flags);
            }
        }
        else {
            reportHandlerInfo->handler(connection, reportHandlerInfo->parameter, domainName, variableName, value, flags);
        }

        element = LinkedList_getNext(element);
    }

    MmsValue_deleteIfNotNull(value);
}

static void
mmsAppTickHandler(void* parameter)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Endpoint self = (Tase2_Endpoint) parameter;

    Semaphore_wait(self->serversLock);

    LinkedList element = LinkedList_getNext(self->appTickHandlers);

    while (element) {

        AppTickHandlerInfo handlerInfo = (AppTickHandlerInfo) LinkedList_getData(element);

        handlerInfo->handler(handlerInfo->parameter);

        element = LinkedList_getNext(element);
    }

    Semaphore_post(self->serversLock);
}

#ifdef TASE2_DEBUG_OUTPUT
static void
printAppTitle(ItuObjectIdentifier* oid, char* buf, int bufSize)
{
    int remainingSize = bufSize;
    char* curBuf = buf;

#if defined(_MSC_VER) && _MSC_VER < 1900
    memset(buf, 0, bufSize);
#endif

    int i;

    for (i = 0; i < oid->arcCount; i++) {
#if defined(_MSC_VER) && _MSC_VER < 1900
        int size = _snprintf(curBuf, remainingSize, "%i", oid->arc[i]);
#else
        int size = snprintf(curBuf, remainingSize, "%i", oid->arc[i]);
#endif

        remainingSize = remainingSize - size;

        curBuf = curBuf + size;

        if (i != (oid->arcCount - 1)) {
            if (remainingSize > 0) {
                *curBuf = '.';
                curBuf++;
                remainingSize--;
            }
            else
                break;
        }

        if (remainingSize <= 0)
            break;
    }
}

static void
printAddressSelector(int addrSize, uint8_t* addr, char* buf, int bufSize)
{
    int remainingSize = bufSize;
    char* curBuf = buf;

    buf[0] = 0;

    int i;
    for (i = 0; i < addrSize; i++) {
#if defined(_MSC_VER) && _MSC_VER < 1900
        memset(buf, 0, bufSize);
        int size = _snprintf(curBuf, remainingSize, "%02x", addr[i]);
#else
        int size = snprintf(curBuf, remainingSize, "%02x", addr[i]);
#endif

        remainingSize = remainingSize - size;

        curBuf = curBuf + size;

        if (i != (addrSize - 1)) {
            if (remainingSize > 0) {
                *curBuf = ' ';
                curBuf++;
                remainingSize--;
            }
            else
                break;
        }

        if (remainingSize <= 0)
            break;
    }
}

#endif /* TASE2_DEBUG_OUTPUT */

bool
Tase2_Endpoint_matchAddresses(Tase2_Endpoint self, IsoApplicationAddress addr1, IsoApplicationAddress addr2)
{
    if (self->matchingRules & MATCH_AP_TITLE) {
        if (addr1->apTitle.arcCount == addr2->apTitle.arcCount) {
            int i;

            for (i = 0; i < addr1->apTitle.arcCount; i++) {
                if (addr1->apTitle.arc[i] != addr2->apTitle.arc[i]) {
                    return false;
                }
            }
        }
        else
            return false;
    }

    if (self->matchingRules & MATCH_AE_QUALIFIER) {
        if (addr1->aeQualifier != addr2->aeQualifier)
            return false;
    }

    if (self->matchingRules & MATCH_P_SEL) {
        if (addr1->pSelector.size == addr2->pSelector.size) {
            if (memcmp(addr1->pSelector.value, addr2->pSelector.value, addr1->pSelector.size))
                return false;
        }
        else
            return false;
    }

    if (self->matchingRules & MATCH_S_SEL) {
        if (addr1->sSelector.size == addr2->sSelector.size) {
            if (memcmp(addr1->sSelector.value, addr2->sSelector.value, addr1->sSelector.size))
                return false;
        }
        else
            return false;
    }

    if (self->matchingRules & MATCH_T_SEL) {
        if (addr1->tSelector.size == addr2->tSelector.size) {
            if (memcmp(addr1->tSelector.value, addr2->tSelector.value, addr1->tSelector.size))
                return false;
        }
        else
            return false;
    }

    return true;
}

static void
mmsConnectionHandler (void* parameter, MmsEndpointConnection connection, MmsEndpointEvent event)
{
    Tase2_Endpoint self = (Tase2_Endpoint) parameter;

    Tase2_Endpoint_Connection peer = NULL;

    if (event == MMS_ENDPOINT_EVENT_NEW_CONNECTION)
    {
        peer = (Tase2_Endpoint_Connection) GLOBAL_CALLOC(1, sizeof(struct sTase2_Endpoint_Connection));

        if (peer) {
            peer->mmsConnection = connection;
            peer->applicationAddress = *(MmsEndpointConnection_getPeerApplicationAddress(connection));
            peer->apTitleStr = NULL;

            Semaphore_wait(self->endpointConnectionsLock);
            bool success = LinkedList_addEx(self->endpointConnections, peer);
            Semaphore_post(self->endpointConnectionsLock);

            if (success == false) {
                GLOBAL_FREEMEM(peer);
                tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self, NULL, "out of memory");
            }

        }
        else {
            tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self, NULL, "out of memory");
        }


    }
    else if (event == MMS_ENDPOINT_EVENT_CONNECTION_CLOSED)
    {
        Semaphore_wait(self->endpointConnectionsLock);

        LinkedList epConElement = LinkedList_getNext(self->endpointConnections);

        while (epConElement) {
            Tase2_Endpoint_Connection con = (Tase2_Endpoint_Connection) LinkedList_getData(epConElement);

            if (con->mmsConnection == connection) {
                peer = con;
                break;
            }

            epConElement = LinkedList_getNext(epConElement);
        }

        if (peer)
            LinkedList_remove(self->endpointConnections, peer);

        Semaphore_post(self->endpointConnectionsLock);
    }

    if (peer) {

        /* forward connection events to Tase2_Server instances */

        Semaphore_wait(self->serversLock);

        LinkedList element = LinkedList_getNext(self->servers);

        while (element) {
            Tase2_Server server = (Tase2_Server) LinkedList_getData(element);

            IsoApplicationAddress localAppAddr = MmsEndpointConnection_getCalledApplicationAddress(connection);

            if (localAppAddr) {

                if (Tase2_Endpoint_matchAddresses(self, localAppAddr, Tase2_Server_getLocalAddresses(server))) {
                    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self, peer, "Connection matched with TASE.2 server");

                    Tase2_Server_mmsConnectionHandler(server, connection, peer, event);

                    break;
                }
            }
            else {
                /* associate with first server available */
                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self, peer, "Connection matched with default TASE.2 server");

                Tase2_Server_mmsConnectionHandler(server, connection, peer, event);

                break;
            }

            element = LinkedList_getNext(element);
        }

        Semaphore_post(self->serversLock);

        /* forward connection events to Tase2_Client instances when application address matches */

        Semaphore_wait(self->clientsLock);

        element = LinkedList_getNext(self->clients);

        while (element) {
            Tase2_Client client = (Tase2_Client) LinkedList_getData(element);

            IsoApplicationAddress conAppAddr = MmsEndpointConnection_getPeerApplicationAddress(connection);

            IsoApplicationAddress clientPeerAddr = Tase2_Client_getRemoteApplicationAddress(client);

            if (Tase2_Endpoint_matchAddresses(self, conAppAddr, clientPeerAddr)) {
                Tase2_Client_mmsConnectionHandler(client, connection, event);

                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self, peer, "Connection matched with TASE.2 client");
            }
            else {
                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self, peer, "Connection doesn't match TASE.2 client");
            }

            element = LinkedList_getNext(element);
        }

        Semaphore_post(self->clientsLock);

        if (self->connectionHandler) {
            self->connectionHandler(self, self->connectionHandlerParameter, peer,
                    (event == MMS_ENDPOINT_EVENT_NEW_CONNECTION) ? true : false);
        }

        if (event == MMS_ENDPOINT_EVENT_CONNECTION_CLOSED)
            Tase2_Endpoint_Connection_destroy(peer);
    }
    else {
        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self, peer, "endpoint: connect to remote failed.");
        return;
    }
}

static void
mmsEndpointStateChangedHandler(MmsEndpoint endpoint, void* parameter, MmsEndpointState newState)
{
    Tase2_Endpoint self = (Tase2_Endpoint) parameter;

    const char* stateStr = "INVALID";

    if (newState == (MmsEndpointState)TASE2_ENDPOINT_STATE_IDLE)
        stateStr = "IDLE";
    else if (newState == (MmsEndpointState)TASE2_ENDPOINT_STATE_CONNECTED)
        stateStr = "CONNECTED";
    else if (newState == (MmsEndpointState)TASE2_ENDPOINT_STATE_CONNECTING)
        stateStr = "CONNECTING";
    else if (newState == (MmsEndpointState)TASE2_ENDPOINT_STATE_LISTENING)
        stateStr = "LISTENING";
    else if (newState == (MmsEndpointState)TASE2_ENDPOINT_STATE_ERROR)
        stateStr = "ERROR";

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self, NULL, "endpoint state changed: %s(%i)", stateStr, newState);

    if (self->stateChangedHandler) {
        self->stateChangedHandler(self, self->stateChangedHandlerParameter, (Tase2_Endpoint_State) newState);
    }
}

static void
mmsLogHandler(void* parameter, MmsLogLevel logLevel, MmsEndpointConnection connection, const char* message)
{
    Tase2_Endpoint self = (Tase2_Endpoint) parameter;

    Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self, connection);

    tase2_log((Tase2_LogLevel) logLevel, TASE2_LOG_SOURCE_MMS, self, peer, message);

    Tase2_Endpoint_unlockEndpointConnection(self, peer);
}

Tase2_Endpoint_Connection
Tase2_Endpoint_getMatchingEndpointConnection(Tase2_Endpoint self, IsoApplicationAddress remoteAddress)
{
    Tase2_Endpoint_Connection retCon = NULL;

    Semaphore_wait(self->endpointConnectionsLock);

    LinkedList epConElement = LinkedList_getNext(self->endpointConnections);

    while (epConElement) {
        Tase2_Endpoint_Connection con = (Tase2_Endpoint_Connection) LinkedList_getData(epConElement);


        if (Tase2_Endpoint_matchAddresses(self, remoteAddress, &(con->applicationAddress))) {

            char remoteApTitleStr[255];
            printAppTitle(&(remoteAddress->apTitle), remoteApTitleStr, 255);

            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "Found matching TASE.2 connection instance for remote AP-title %s", remoteApTitleStr);

            retCon = con;

            break;
        }

        epConElement = LinkedList_getNext(epConElement);
    }

    Semaphore_post(self->endpointConnectionsLock);

    return retCon;
}

static AcseServiceUserDiagnostics
authenticator(void* parameter, AcseAuthenticationParameter authParameter, void** securityToken, IsoApplicationAddress localAppRef, IsoApplicationAddress remoteAppRef, Socket socket)
{
    Tase2_Endpoint self = (Tase2_Endpoint) parameter;

//#ifdef TASE2_DEBUG_OUTPUT
#if 1

    char remoteApTitleStr[255];
    char localApTitleStr[255];

    if (remoteAppRef) {
        printAppTitle(&(remoteAppRef->apTitle), remoteApTitleStr, 255);

        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "ACSE Authenticator:");
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  peer AP-title: %s", remoteApTitleStr);
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  peer AE-qualifier: %i", remoteAppRef->aeQualifier);
        if (remoteAppRef->hasAeInvocationId)
            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  peer AE-invocation-ID: %i", remoteAppRef->aeInvocationId);
        if (remoteAppRef->hasApInvocationId)
            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  peer AP-invocation-ID: %i", remoteAppRef->apInvocationId);
        printAddressSelector(remoteAppRef->pSelector.size, remoteAppRef->pSelector.value, remoteApTitleStr, 255);
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  peer P-sel: %s", remoteApTitleStr);
        printAddressSelector(remoteAppRef->sSelector.size, remoteAppRef->sSelector.value, remoteApTitleStr, 255);
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  peer S-sel: %s", remoteApTitleStr);
        printAddressSelector(remoteAppRef->tSelector.size, remoteAppRef->tSelector.value, remoteApTitleStr, 255);
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  peer T-sel: %s", remoteApTitleStr);
    }
    else {
        tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self, NULL, "No remote ISO address available");
    }

    if (localAppRef) {
        printAppTitle(&(localAppRef->apTitle), localApTitleStr, 255);

        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  local AP-title: %s", localApTitleStr);
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  local AE-qualifier: %i", localAppRef->aeQualifier);
        if (localAppRef->hasAeInvocationId)
            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  local AE-invocation-ID: %i", localAppRef->aeInvocationId);
        if (localAppRef->hasApInvocationId)
            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  local AP-invocation-ID: %i", localAppRef->apInvocationId);
        printAddressSelector(localAppRef->pSelector.size, localAppRef->pSelector.value, localApTitleStr, 255);
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  local P-sel: %s", localApTitleStr);
        printAddressSelector(localAppRef->sSelector.size, localAppRef->sSelector.value, localApTitleStr, 255);
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  local S-sel: %s", localApTitleStr);
        printAddressSelector(localAppRef->tSelector.size, localAppRef->tSelector.value, localApTitleStr, 255);
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "  local T-sel: %s", localApTitleStr);
    }
    else {
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "No local ISO address available");
    }


#endif

    if (authParameter->mechanism == ACSE_AUTH_PASSWORD) {
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "Peer provided password");
    }
    else if (authParameter->mechanism == ACSE_AUTH_TLS) {
        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "Peer TLS certificate available");
    }

    Tase2_BilateralTable blt = NULL;

    /* Check for matching servers */

    Tase2_Server matchingServer = NULL;

    Semaphore_wait(self->serversLock);

    LinkedList serverElem = LinkedList_getNext(self->servers);

    while (serverElem) {
        Tase2_Server server = (Tase2_Server) LinkedList_getData(serverElem);

        if (localAppRef) {
            /* check if server is matching with provided local ISO address */

            IsoApplicationAddress serverlocalAppAddr = Tase2_Server_getLocalAddresses(server);

            if (Tase2_Endpoint_matchAddresses(self, serverlocalAppAddr, localAppRef)) {

                tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "Found matching TASE.2 server instance");

                AcseServiceUserDiagnostics authResult = Tase2_Server_authenticator(server, authParameter, &blt, remoteAppRef);

                if (authResult != ACSE_USER_OK) {
                    Semaphore_post(self->serversLock);

                    tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "server rejected connection");
                    return authResult;
                }
                else {
                    // TODO associate server to endpoint connection (otherwise only first server is called!!!)

                    matchingServer = server;

                    break;
                }
            }
        }

        serverElem = LinkedList_getNext(serverElem);
    }

    Semaphore_post(self->serversLock);

    /* Check for matching clients */

    Tase2_Client matchingClient = NULL;

    Semaphore_wait(self->clientsLock);

    LinkedList clientElem = LinkedList_getNext(self->clients);

    while (clientElem) {
        Tase2_Client client = (Tase2_Client) LinkedList_getData(clientElem);

        if (remoteAppRef) {
            /* check if client is matching with provided remote ISO address */

            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "check client");

            IsoApplicationAddress clientRemoteAddress = Tase2_Client_getRemoteApplicationAddress(client);

            if (Tase2_Endpoint_matchAddresses(self, clientRemoteAddress, remoteAppRef)) {
                printAppTitle(&(remoteAppRef->apTitle), remoteApTitleStr, 255);

                tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "Found matching TASE.2 client instance for remote AP-title %s", remoteApTitleStr);

                matchingClient = client;

                break;
            }

        }
        else {
            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self, NULL, "no remote ref");
        }

        clientElem = LinkedList_getNext(clientElem);
    }

    Semaphore_post(self->clientsLock);

    if ((matchingServer == NULL) && (matchingClient == NULL)) {
        tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self, NULL, "No matching server or client found");

        return ACSE_USER_CALLED_AP_TITLE_NOT_RECOGNIZED;
    }

    *securityToken = blt;

    /* call user provided authentication function */
    if (self->connectionAcceptHandler) {

        char peerAddrStr[100];

        Socket_getPeerAddressStatic(socket, peerAddrStr);

        uint8_t* authValueBuf = NULL;
        int authValLen = 0;

        if (authParameter->mechanism == ACSE_AUTH_PASSWORD) {
            authValueBuf = authParameter->value.password.octetString;
            authValLen = authParameter->value.password.passwordLength;
        }
        else if ((authParameter->mechanism == ACSE_AUTH_CERTIFICATE) || (authParameter->mechanism == ACSE_AUTH_TLS)) {
            authValueBuf = authParameter->value.certificate.buf;
            authValLen = authParameter->value.certificate.length;
        }

        if (self->connectionAcceptHandler(self, self->connectionAcceptHandlerParameter, peerAddrStr, blt,
                (Tase2_AuthenticationMethod) authParameter->mechanism, authValueBuf, authValLen))
            return ACSE_USER_OK;
        else
            return ACSE_USER_NO_REASON_GIVEN;
    }

    return ACSE_USER_OK;
}

Tase2_Endpoint
Tase2_Endpoint_create(TLSConfiguration tlsConfig, bool passive)
{
    Tase2_Endpoint self = (Tase2_Endpoint) GLOBAL_CALLOC(1, sizeof(struct sTase2_Endpoint));

	if (self) {

        int newEndpointId = ++endpointId;

        char endpointIdStr[50];
        if (passive)
            snprintf(endpointIdStr, 30, "PASSIVE ENDPOINT %i", newEndpointId);
        else
            snprintf(endpointIdStr, 30, "ACTIVE ENDPOINT %i", newEndpointId);

        self->id = StringUtils_copyString(endpointIdStr);

        self->mmsEndpoint = MmsEndpoint_create(tlsConfig, passive);

        self->idVendor = StringUtils_copyString("MZ Automation");
        self->idModel = StringUtils_copyString("TASE.2");
        self->idRevision = StringUtils_copyString(TASE2_LIBRARY_VERSION_STRING);

        self->servers = LinkedList_create();
        self->serversLock = Semaphore_create(1);

        self->clients = LinkedList_create();
        self->clientsLock = Semaphore_create(1),

        self->endpointConnectionsLock = Semaphore_create(1);
        self->endpointConnections = LinkedList_create();

        self->reportHandlers = LinkedList_create();
        self->appTickHandlers = LinkedList_create();

        self->matchingRules = MATCH_AP_TITLE + MATCH_AE_QUALIFIER;

        MmsEndpoint_setLogHandler(self->mmsEndpoint, mmsLogHandler, self);
        MmsEndpoint_setLogLevel(self->mmsEndpoint, MMS_LOG_DEBUG);

        MmsEndpoint_setIdentity(self->mmsEndpoint, self->idVendor, self->idModel, self->idRevision);

        MmsEndpoint_installInformationReportHandler(self->mmsEndpoint, mmsInformationReportHandler, (void*) self);

        MmsEndpoint_installConnectionHandler(self->mmsEndpoint, mmsConnectionHandler, (void*) self);

        MmsEndpoint_installAppTickHandler(self->mmsEndpoint, mmsAppTickHandler, (void*) self);

        MmsEndpoint_setEndpointStateChangedHandler(self->mmsEndpoint, mmsEndpointStateChangedHandler, self);

        MmsEndpoint_setClientAuthenticator(self->mmsEndpoint, authenticator, (void*) self);
    }

    return self;
}

void
Tase2_Endpoint_setTLSConfiguration(Tase2_Endpoint self, TLSConfiguration tlsConfig)
{
    if (self->mmsEndpoint) {
        MmsEndpoint_setTLSConfiguration(self->mmsEndpoint, tlsConfig);
    }
}

static void removeAllServers(Tase2_Endpoint self)
{
    Semaphore_wait(self->serversLock);

    LinkedList elem = LinkedList_getNext(self->servers);

    while (elem) {
        Tase2_Server server = (Tase2_Server)LinkedList_getData(elem);

        server->endpoint = NULL;

        elem = LinkedList_getNext(elem);
    }

    LinkedList_destroyStatic(self->servers);

    Semaphore_post(self->serversLock);
}

void
Tase2_Endpoint_destroy(Tase2_Endpoint self)
{
    Tase2_Endpoint_disconnect(self, 0);

    MmsEndpoint_destroy(self->mmsEndpoint);

    removeAllServers(self);

    Semaphore_wait(self->clientsLock);

    /* remove all clients */
    LinkedList clientElem = LinkedList_getNext(self->clients);

    while (clientElem) {
        Tase2_Client client = (Tase2_Client)LinkedList_getData(clientElem);

        client->endpoint = NULL;

        clientElem = LinkedList_getNext(clientElem);
    }

    LinkedList_destroyStatic(self->clients);

    Semaphore_post(self->clientsLock);

    Semaphore_destroy(self->clientsLock);
    Semaphore_destroy(self->serversLock);

    GLOBAL_FREEMEM(self->id);

    GLOBAL_FREEMEM(self->idVendor);
    GLOBAL_FREEMEM(self->idModel);
    GLOBAL_FREEMEM(self->idRevision);

    LinkedList_destroy(self->endpointConnections);
    Semaphore_destroy(self->endpointConnectionsLock);

    LinkedList_destroy(self->reportHandlers);
    LinkedList_destroy(self->appTickHandlers);

    GLOBAL_FREEMEM(self);
}

const char*
Tase2_Endpoint_getId(Tase2_Endpoint self)
{
    return self->id;
}

void
Tase2_Endpoint_setId(Tase2_Endpoint self, const char* id)
{
    GLOBAL_FREEMEM(self->id);

    self->id = StringUtils_copyString(id);
}

void
Tase2_Endpoint_setStateChangedHandler(Tase2_Endpoint self, Tase2_Endpoint_StateChangedHandler handler, void* parameter)
{
    self->stateChangedHandler = handler;
    self->stateChangedHandlerParameter = parameter;
}

void
Tase2_Endpoint_setConnectionHandler(Tase2_Endpoint self, Tase2_Endpoint_ConnectionHandler handler, void* parameter)
{
    self->connectionHandler = handler;
    self->connectionHandlerParameter = parameter;
}

void
Tase2_Endpoint_setConnectionAcceptHandler(Tase2_Endpoint self, Tase2_Endpoint_ConnectionAcceptHandler handler, void* parameter)
{
    self->connectionAcceptHandler = handler;
    self->connectionAcceptHandlerParameter = parameter;
}

Tase2_Endpoint_Connection
Tase2_Endpoint_lookupConnection(Tase2_Endpoint self, MmsEndpointConnection mmsEndpointConnection)
{
    Tase2_Endpoint_Connection retVal = NULL;

    Semaphore_wait(self->endpointConnectionsLock);

    LinkedList epConElement = LinkedList_getNext(self->endpointConnections);

    while (epConElement) {
        Tase2_Endpoint_Connection con = (Tase2_Endpoint_Connection) LinkedList_getData(epConElement);

        if (con->mmsConnection == mmsEndpointConnection) {
            retVal = con;
            break;
        }

        epConElement = LinkedList_getNext(epConElement);
    }

    Semaphore_post(self->endpointConnectionsLock);

    return retVal;
}

Tase2_Endpoint_Connection
Tase2_Endpoint_getLockedEndpointConnection(Tase2_Endpoint self, MmsEndpointConnection mmsEndpointConnection)
{
    Semaphore_wait(self->endpointConnectionsLock);

    LinkedList epConElement = LinkedList_getNext(self->endpointConnections);

    while (epConElement) {
        Tase2_Endpoint_Connection con = (Tase2_Endpoint_Connection) LinkedList_getData(epConElement);

        if (con->mmsConnection == mmsEndpointConnection) {

            Semaphore_post(self->endpointConnectionsLock);

            return con;
        }

        epConElement = LinkedList_getNext(epConElement);
    }

    Semaphore_post(self->endpointConnectionsLock);

    return NULL;
}

TASE2_INTERNAL void
Tase2_Endpoint_unlockEndpointConnection(Tase2_Endpoint self, Tase2_Endpoint_Connection connection)
{
    if (connection)
        Semaphore_post(self->endpointConnectionsLock);
}

void
Tase2_Endpoint_addClient(Tase2_Endpoint self, Tase2_Client client)
{
    if (client->endpoint && (client->endpoint != self)) {
        /* remove client from associated endpoint */
        Tase2_Endpoint_removeClient(client->endpoint, client);
    }

    Semaphore_wait(self->clientsLock);

    client->endpoint = self;

    LinkedList_add(self->clients, client);
    MmsEndpoint_addClient(self->mmsEndpoint, client->mmsClient);

    Semaphore_post(self->clientsLock);
}

void
Tase2_Endpoint_addServer(Tase2_Endpoint self, Tase2_Server server)
{
    if (server->endpoint && (server->endpoint != self)) {
        /* remove server from associated endpoint */
        Tase2_Endpoint_removeServer(server->endpoint, server);
    }

    Semaphore_wait(self->serversLock);

    server->endpoint = self;

    LinkedList_add(self->servers, server);

    Tase2_Server_connectWithEndpoint(server, self);

    Semaphore_post(self->serversLock);
}

static void
removeFromAppTickHandlers(Tase2_Endpoint self, void* parameter)
{
    AppTickHandlerInfo handlerToRemove = NULL;

    LinkedList element = LinkedList_getNext(self->appTickHandlers);

    while (element) {
        AppTickHandlerInfo info = (AppTickHandlerInfo)LinkedList_getData(element);

        if (info->parameter == parameter) {
            handlerToRemove = info;
            break;
        }

        element = LinkedList_getNext(element);    
    }

    if (handlerToRemove) {
        if (LinkedList_remove(self->appTickHandlers, handlerToRemove) == false) {
            tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self, NULL, "failed to remove application tick handler for client/server entity");
        }

        GLOBAL_FREEMEM(handlerToRemove);
    }
}

static void
removeFromReportHandlers(Tase2_Endpoint self, void* parameter)
{
    InformationReportHandlerInfo handlerToRemove = NULL;

    LinkedList element = LinkedList_getNext(self->reportHandlers);

    while (element) {
        InformationReportHandlerInfo info = (InformationReportHandlerInfo)LinkedList_getData(element);

        if (info->parameter == parameter) {
            handlerToRemove = info;
            break;
        }

        element = LinkedList_getNext(element);    
    }

    if (handlerToRemove) {
        LinkedList_remove(self->reportHandlers, handlerToRemove);
        GLOBAL_FREEMEM(handlerToRemove);
    }
}

Tase2_Server
Tase2_Endpoint_removeServer(Tase2_Endpoint self, Tase2_Server server)
{
    Tase2_Server removedServer = NULL;

    Semaphore_wait(self->serversLock);

    if (LinkedList_remove(self->servers, server)) {

        removeFromAppTickHandlers(self, server);
        removeFromReportHandlers(self, server);

        server->endpoint = NULL;

        Semaphore_post(self->serversLock);

        /* close connections */
        MmsEndpoint_disconnectPeersForServer(self->mmsEndpoint, server->mmsServer);

        removedServer = server;
    }
    else {
        Semaphore_post(self->serversLock);
    }

    return removedServer;
}

Tase2_Client
Tase2_Endpoint_removeClient(Tase2_Endpoint self, Tase2_Client client)
{
    Tase2_Client removedClient = NULL;

    Semaphore_wait(self->clientsLock);

    if (LinkedList_remove(self->clients, client)) {

        removeFromAppTickHandlers(self, client);

        client->endpoint = NULL;

        removedClient = client;
    }

    Semaphore_post(self->clientsLock);

    return removedClient;
}

void
Tase2_Endpoint_setAddressMatchingRules(Tase2_Endpoint self,
        bool apTitle, bool aeQualifier, bool pSelector, bool sSelector, bool tSelector)
{
    self->matchingRules = 0;

    if (apTitle)
        self->matchingRules += MATCH_AP_TITLE;

    if (aeQualifier)
        self->matchingRules += MATCH_AE_QUALIFIER;

    if (pSelector)
        self->matchingRules += MATCH_P_SEL;

    if (sSelector)
        self->matchingRules += MATCH_S_SEL;

    if (tSelector)
        self->matchingRules += MATCH_T_SEL;
}

void
Tase2_Endpoint_installInformationReportHandler(Tase2_Endpoint self, Tase2_Client client, MmsInformationReportHandler handler, void* parameter)
{
    InformationReportHandlerInfo info = (InformationReportHandlerInfo) GLOBAL_MALLOC(sizeof(struct sInformationReportHandlerInfo));

    info->handler = handler;
    info->parameter = parameter;
    info->client = client;

    LinkedList_add(self->reportHandlers, info);
}

void
Tase2_Endpoint_installAppTickHandler(Tase2_Endpoint self, MmsAppTickHandler handler, void* parameter)
{
    AppTickHandlerInfo info = (AppTickHandlerInfo) GLOBAL_MALLOC(sizeof(struct sAppTickHandlerInfo));

    info->handler = handler;
    info->parameter = parameter;

    LinkedList_add(self->appTickHandlers, info);
}

void
Tase2_Endpoint_setIdentity(Tase2_Endpoint self, const char* vendor, const char* model, const char* revision)
{
    GLOBAL_FREEMEM(self->idVendor);
    GLOBAL_FREEMEM(self->idModel);
    GLOBAL_FREEMEM(self->idRevision);

    self->idVendor = StringUtils_copyString(vendor);
    self->idModel = StringUtils_copyString(model);
    self->idRevision = StringUtils_copyString(revision);

    MmsEndpoint_setIdentity(self->mmsEndpoint, self->idVendor, self->idModel, self->idRevision);
}

Tase2_ApplicationAddress
Tase2_Endpoint_getLocalApplicationAddress(Tase2_Endpoint self)
{
    return (Tase2_ApplicationAddress) MmsEndpoint_getLocalApplicationAddress(self->mmsEndpoint);
}

void
Tase2_Endpoint_setLocalApTitle(Tase2_Endpoint self, const char* apTitle, int aeQualifier)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getLocalApplicationAddress(self->mmsEndpoint);

    ItuObjectIdentifier_setFromString(&(appAddr->apTitle), apTitle);
    appAddr->aeQualifier = aeQualifier;
}

void
Tase2_Endpoint_setLocalAddresses(Tase2_Endpoint self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector)
{
    IsoApplicationAddress isoAppAddr = MmsEndpoint_getLocalApplicationAddress(self->mmsEndpoint);

    PSelector pSel;
    SSelector sSel;
    TSelector tSel;

    memcpy(&pSel, &pSelector, sizeof(PSelector));
    memcpy(&sSel, &sSelector, sizeof(SSelector));
    memcpy(&tSel, &tSelector, sizeof(TSelector));

    IsoApplicationAddress_setLowerLayerAddresses(isoAppAddr, pSel, sSel, tSel);
}

void
Tase2_Endpoint_setLocalApTitleEx(Tase2_Endpoint self, const char* apTitle)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getLocalApplicationAddress(self->mmsEndpoint);

    ItuObjectIdentifier_setFromString(&(appAddr->apTitle), apTitle);
}

void
Tase2_Endpoint_setLocalAeQualifier(Tase2_Endpoint self, int aeQualifier)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getLocalApplicationAddress(self->mmsEndpoint);

    IsoApplicationAddress_setAeQualifier(appAddr, aeQualifier);
}

void
Tase2_Endpoint_setLocalAeInvocationId(Tase2_Endpoint self, int aeInvocationId)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getLocalApplicationAddress(self->mmsEndpoint);

    IsoApplicationAddress_setAeInvocationId(appAddr, aeInvocationId);
}

void
Tase2_Endpoint_setLocalApInvocationId(Tase2_Endpoint self, int apInvocationId)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getLocalApplicationAddress(self->mmsEndpoint);

    IsoApplicationAddress_setApInvocationId(appAddr, apInvocationId);
}

void
Tase2_Endpoint_setLocalIpAddress(Tase2_Endpoint self, const char* hostname)
{
    MmsEndpoint_setLocalIpAddress(self->mmsEndpoint, hostname);
}

void
Tase2_Endpoint_setLocalTcpPort(Tase2_Endpoint self, int tcpPort)
{
    MmsEndpoint_setLocalTcpPort(self->mmsEndpoint, tcpPort);
}

Tase2_ApplicationAddress
Tase2_Endpoint_getRemoteApplicationAddress(Tase2_Endpoint self)
{
    return (Tase2_ApplicationAddress) MmsEndpoint_getRemoteApplicationAddress(self->mmsEndpoint);
}

void
Tase2_Endpoint_setRemoteIpAddress(Tase2_Endpoint self, const char* hostname)
{
    MmsEndpoint_setRemoteIpAddress(self->mmsEndpoint, hostname);
}

void
Tase2_Endpoint_setRemoteTcpPort(Tase2_Endpoint self, int tcpPort)
{
    MmsEndpoint_setRemoteTcpPort(self->mmsEndpoint, tcpPort);
}

void
Tase2_Endpoint_setRemoteApTitle(Tase2_Endpoint self, const char* apTitle, int aeQualifier)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getRemoteApplicationAddress(self->mmsEndpoint);

    ItuObjectIdentifier_setFromString(&(appAddr->apTitle), apTitle);
    appAddr->aeQualifier = aeQualifier;
}

void
Tase2_Endpoint_setRemoteAddresses(Tase2_Endpoint self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector)
{
    IsoApplicationAddress isoAppAddr = MmsEndpoint_getRemoteApplicationAddress(self->mmsEndpoint);

    PSelector pSel;
    SSelector sSel;
    TSelector tSel;

    memcpy(&pSel, &pSelector, sizeof(PSelector));
    memcpy(&sSel, &sSelector, sizeof(SSelector));
    memcpy(&tSel, &tSelector, sizeof(TSelector));

    IsoApplicationAddress_setLowerLayerAddresses(isoAppAddr, pSel, sSel, tSel);
}

void
Tase2_Endpoint_setRemoteApTitleEx(Tase2_Endpoint self, const char* apTitle)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getRemoteApplicationAddress(self->mmsEndpoint);

    ItuObjectIdentifier_setFromString(&(appAddr->apTitle), apTitle);
}

void
Tase2_Endpoint_setRemoteAeQualifier(Tase2_Endpoint self, int aeQualifier)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getRemoteApplicationAddress(self->mmsEndpoint);

    IsoApplicationAddress_setAeQualifier(appAddr, aeQualifier);
}

void
Tase2_Endpoint_setRemoteAeInvocationId(Tase2_Endpoint self, int aeInvocationId)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getRemoteApplicationAddress(self->mmsEndpoint);

    IsoApplicationAddress_setAeInvocationId(appAddr, aeInvocationId);
}

void
Tase2_Endpoint_setRemoteApInvocationId(Tase2_Endpoint self, int apInvocationId)
{
    IsoApplicationAddress appAddr = MmsEndpoint_getRemoteApplicationAddress(self->mmsEndpoint);

    IsoApplicationAddress_setApInvocationId(appAddr, apInvocationId);
}

void
Tase2_Endpoint_setRequestTimeout(Tase2_Endpoint self, int timeoutInMs)
{
    MmsEndpoint_setRequestTimeout(self->mmsEndpoint, (uint32_t) timeoutInMs);
}

void
Tase2_Endpoint_setConnectTimeout(Tase2_Endpoint self, int timeoutInMs)
{
    MmsEndpoint_setConnectTimeout(self->mmsEndpoint, (uint32_t) timeoutInMs);
}

void
Tase2_Endpoint_connect(Tase2_Endpoint self)
{
    MmsEndpoint_connect(self->mmsEndpoint);
}

void
Tase2_Endpoint_disconnect(Tase2_Endpoint self, int disconnectMethod)
{
    MmsEndpoint_disconnect(self->mmsEndpoint, disconnectMethod);

    /* Call disconnect for all clients to do some cleanup */
    LinkedList element = LinkedList_getNext(self->clients);

    while (element) {
        Tase2_Client client = (Tase2_Client) LinkedList_getData(element);

        Tase2_Client_disconnect(client);

        element = LinkedList_getNext(element);
    }
}

void
Tase2_Endpoint_disconnectPeer(Tase2_Endpoint self, const char* peerIpAddress)
{
    MmsEndpoint_disconnectPeer(self->mmsEndpoint, peerIpAddress);
}

bool
Tase2_Endpoint_isConnected(Tase2_Endpoint self)
{
    if (MmsEndpoint_getNumberOfActiveConnections(self->mmsEndpoint) > 0)
        return true;
    else
        return false;
}

int
Tase2_Endpoint_connectedPeers(Tase2_Endpoint self)
{
    return MmsEndpoint_getNumberOfActiveConnections(self->mmsEndpoint);
}

bool
Tase2_Endpoint_waitForState(Tase2_Endpoint self, Tase2_Endpoint_State state, int timeoutInMs)
{
    return MmsEndpoint_waitForState(self->mmsEndpoint, (MmsEndpointState) state, timeoutInMs);
}

Tase2_Endpoint_State
Tase2_Endpoint_getState(Tase2_Endpoint self)
{
    return (Tase2_Endpoint_State) MmsEndpoint_getState(self->mmsEndpoint);
}

void
Tase2_Endpoint_setMaxMmsPduSize(Tase2_Endpoint self, int maxPduSize)
{
    MmsConnectionParameters* parameters = MmsEndpoint_getConnectionParameters(self->mmsEndpoint);

    if (maxPduSize > 0) {
        parameters->maxPduSize = maxPduSize;
    }
}

void
Tase2_Endpoint_setMaxServOutstandingCalling(Tase2_Endpoint self, int value)
{
    MmsConnectionParameters* parameters = MmsEndpoint_getConnectionParameters(self->mmsEndpoint);

    if (value > 0) {
        parameters->maxServOutstandingCalling = value;
    }
}

void
Tase2_Endpoint_setMaxServOutstandingCalled(Tase2_Endpoint self, int value)
{
    MmsConnectionParameters* parameters = MmsEndpoint_getConnectionParameters(self->mmsEndpoint);

    if (value > 0) {
        parameters->maxServOutstandingCalled = value;
    }
}

MmsEndpoint
Tase2_Endpoint_getMmsEndpoint(Tase2_Endpoint self)
{
    return self->mmsEndpoint;
}

bool
Tase2_Endpoint_isPassive(Tase2_Endpoint self)
{
    return MmsEndpoint_isPassive(self->mmsEndpoint);
}

char*
Tase2_Endpoint_Connection_getPeerIpAddress(Tase2_Endpoint_Connection self)
{
    return MmsEndpointConnection_getPeerAddress(self->mmsConnection);
}

char*
Tase2_Endpoint_Connection_getPeerApTitle(Tase2_Endpoint_Connection self)
{

    if (self->apTitleStr == NULL)
        self->apTitleStr = ItuObjectIdentifier_toString(&(self->applicationAddress.apTitle));

    return self->apTitleStr;
}

int
Tase2_Endpoint_Connection_getPeerAeQualifier(Tase2_Endpoint_Connection self)
{
    return self->applicationAddress.aeQualifier;
}

Tase2_PSelector*
Tase2_Endpoint_Connection_getPeerPSel(Tase2_Endpoint_Connection self)
{
    return  (Tase2_PSelector*) &(self->applicationAddress.pSelector);
}

Tase2_TSelector*
Tase2_Endpoint_Connection_getPeerTSel(Tase2_Endpoint_Connection self)
{
    return  (Tase2_TSelector*) &(self->applicationAddress.tSelector);
}

Tase2_SSelector*
Tase2_Endpoint_Connection_getPeerSSel(Tase2_Endpoint_Connection self)
{
    return  (Tase2_SSelector*) &(self->applicationAddress.sSelector);
}

MmsEndpointConnection
Tase2_Endpoint_Connection_getMmsConnection(Tase2_Endpoint_Connection self)
{
    return self->mmsConnection;
}

int
Tase2_Endpoint_Connection_getMaxPduSize(Tase2_Endpoint_Connection self)
{
    if (self->mmsConnection)
        return MmsEndpointConnection_getMaxPduSize(self->mmsConnection);
    else
        return 0;
}

void
Tase2_Endpoint_Connection_destroy(Tase2_Endpoint_Connection self)
{
    if (self) {
        GLOBAL_FREEMEM(self->apTitleStr);
        GLOBAL_FREEMEM(self);
    }
}
