/*
 *  iso_server.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_common_internal.h"
#include "stack_config.h"

#ifndef DEBUG_ISO_SERVER
#ifdef DEBUG
#define DEBUG_ISO_SERVER 1
#else
#define DEBUG_ISO_SERVER 0
#endif /*DEBUG */
#endif /* DEBUG_ISO_SERVER */

#include "hal_thread.h"

#include "iso_server.h"

#include "iso_server_private.h"

#ifndef CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS
#define CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS 5
#endif

#define TCP_PORT 102
#define SECURE_TCP_PORT 3782
#define BACKLOG 10

struct sIsoServer {
    IsoServerState state;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore stateLock;
#endif

    ConnectionIndicationHandler connectionHandler;
    void* connectionHandlerParameter;

    AcseAuthenticator authenticator;
    void* authenticatorParameter;

    MmsAppTickHandler appTickHandler;
    void* appHandlerParameter;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Thread serverThread;
#endif

    Socket serverSocket;
    int tcpPort;
    char* localIpAddress;

    IsoApplicationAddress localAppAddr;

    TLSConfiguration tlsConfiguration;

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
    int maxConnections;
#endif

#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1)
    LinkedList openClientConnections;
#else
    IsoConnection openClientConnections[CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS];
#endif /* (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1) */

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore userLock;
#endif

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    Semaphore openClientConnectionsMutex; /* mutex for openClientConnections list */
    Semaphore connectionCounterMutex;
#endif

    int connectionCounter;
};

static void
setState(IsoServer self, IsoServerState newState)
{
#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_wait(self->stateLock);
#endif
    self->state = newState;
#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_post(self->stateLock);
#endif
}

static IsoServerState
getState(IsoServer self)
{
    IsoServerState state;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_wait(self->stateLock);
#endif
    state = self->state;
#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_post(self->stateLock);
#endif

    return state;
}

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
static inline void
lockClientConnections(IsoServer self)
{
    Semaphore_wait(self->openClientConnectionsMutex);
}

static inline void
unlockClientConnections(IsoServer self)
{
    Semaphore_post(self->openClientConnectionsMutex);
}
#endif /* (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1) */

static void
addClientConnection(IsoServer self, IsoConnection connection)
{

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    Semaphore_wait(self->connectionCounterMutex);
#endif

    self->connectionCounter++;
    if (DEBUG_ISO_SERVER)
        libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: connection counter %i", self->connectionCounter);

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    lockClientConnections(self);
#endif

#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1)
    LinkedList_add(self->openClientConnections, connection);
#else
    int i;

    for (i = 0; i < CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS; i++) {
        if (self->openClientConnections[i] == NULL) {
            self->openClientConnections[i] = connection;

            break;
        }
    }
#endif

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    unlockClientConnections(self);
#endif

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    Semaphore_post(self->connectionCounterMutex);
#endif

}

static void
removeClientConnection(IsoServer self, IsoConnection connection)
{

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    lockClientConnections(self);
#endif

#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1)


#if (CONFIG_MMS_SINGLE_THREADED == 0)

    LinkedList_remove(self->openClientConnections, connection);

#endif /* (CONFIG_MMS_SINGLE_THREADED == 0) */


#else

    int i;

    for (i = 0; i < CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS; i++) {
        if (self->openClientConnections[i] == connection) {

            self->openClientConnections[i] = NULL;
            break;
        }
    }
#endif /* (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1) */

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    unlockClientConnections(self);
#endif
}

static void
closeAllOpenClientConnections(IsoServer self)
{

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    lockClientConnections(self);
#endif

#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1)

    LinkedList openConnection = LinkedList_getNext(self->openClientConnections);
    while (openConnection != NULL) {
        IsoConnection isoConnection = (IsoConnection) openConnection->data;

        IsoConnection_close(isoConnection);

#if (CONFIG_MMS_SINGLE_THREADED == 1)
            /* if CONFIG_MMS_SINGLE_THREADED == 0 connection instance will be destroyed by connection thread. */
        IsoConnection_destroy(isoConnection);
#endif

        openConnection = LinkedList_getNext(openConnection);
    }

#if (CONFIG_MMS_SINGLE_THREADED == 1)
    LinkedList_destroyStatic(self->openClientConnections);
    self->openClientConnections = NULL;
#endif

#else
    int i;

    for (i = 0; i < CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS; i++) {
        if (self->openClientConnections[i] != NULL) {

            IsoConnection_close(self->openClientConnections[i]);

#if (CONFIG_MMS_SINGLE_THREADED == 1)
            /* if CONFIG_MMS_SINGLE_THREADED == 0 connection instance will be destroyed by connection thread. */
            IsoConnection_destroy(self->openClientConnections[i]);
#endif

        }
    }
#endif /* (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1) */

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    unlockClientConnections(self);
#endif

}

static void
handleClientConnections(IsoServer self)
{
#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1)

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    lockClientConnections(self);
#endif

    LinkedList openConnection = LinkedList_getNext(self->openClientConnections);
    LinkedList lastConnection = self->openClientConnections;

    while (openConnection != NULL) {
        IsoConnection isoConnection = (IsoConnection) openConnection->data;

        if (IsoConnection_isRunning(isoConnection))
            IsoConnection_handleTcpConnection(isoConnection);
        else {
            IsoConnection_destroy(isoConnection);

            lastConnection->next = openConnection->next;

            GLOBAL_FREEMEM(openConnection);

            openConnection = lastConnection;
        }

        openConnection = LinkedList_getNext(openConnection);
    }

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    unlockClientConnections(self);
#endif

#else

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    lockClientConnections(self);
#endif

    int i;

    for (i = 0; i < CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS; i++) {
        if (self->openClientConnections[i] != NULL) {
            if (IsoConnection_isRunning(self->openClientConnections[i])) {

                IsoConnection_handleTcpConnection(self->openClientConnections[i]);
            }
            else {
                IsoConnection_destroy(self->openClientConnections[i]);

                self->openClientConnections[i] = NULL;
            }

        }
    }

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    unlockClientConnections(self);
#endif

#endif /* (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1) */
}

static bool
setupIsoServer(IsoServer self)
{
    bool success = true;

    self->serverSocket = (Socket) TcpServerSocket_create(self->localIpAddress, self->tcpPort);

    if (self->serverSocket == NULL) {
        setState(self, ISO_SVR_STATE_ERROR);
        success = false;

        goto exit_function;
    }

#if (CONFIG_ACTIVATE_TCP_KEEPALIVE == 1)
    Socket_activateTcpKeepAlive(self->serverSocket,
            CONFIG_TCP_KEEPALIVE_IDLE,
            CONFIG_TCP_KEEPALIVE_INTERVAL,
            CONFIG_TCP_KEEPALIVE_CNT);
#endif

    ServerSocket_setBacklog((ServerSocket) self->serverSocket, BACKLOG);

    ServerSocket_listen((ServerSocket) self->serverSocket);

    setState(self, ISO_SVR_STATE_RUNNING);

exit_function:
    return success;
}


#if (CONFIG_MMS_THREADLESS_STACK == 0)
/* used by single and multi-threaded versions */
static void
handleIsoConnections(IsoServer self)
{
    Socket connectionSocket = ServerSocket_accept((ServerSocket) self->serverSocket);

    if (connectionSocket) {

        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: new TCP connection");

#if (CONFIG_ACTIVATE_TCP_KEEPALIVE == 1)
        Socket_activateTcpKeepAlive(connectionSocket,
                CONFIG_TCP_KEEPALIVE_IDLE,
                CONFIG_TCP_KEEPALIVE_INTERVAL,
                CONFIG_TCP_KEEPALIVE_CNT);
#endif

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
        if (self->maxConnections > -1) {
            if (private_IsoServer_getConnectionCounter(self) >= self->maxConnections) {
                if (DEBUG_ISO_SERVER)
                    libmms_iso_log(MMS_LOG_WARNING, "ISO_SERVER: maximum number of connections reached -> reject connection attempt.");

                Socket_destroy(connectionSocket);

                return;
            }
        }
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS != -1)
        if (private_IsoServer_getConnectionCounter(self) >= CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS) {
            if (DEBUG_ISO_SERVER)
                libmms_iso_log(MMS_LOG_WARNING, "ISO_SERVER: maximum number of connections reached -> reject connection attempt.");

            Socket_destroy(connectionSocket);

#if (CONFIG_MMS_SINGLE_THREADED == 1)
            handleClientConnections(self);
#endif

            return;
        }
#endif

        IsoConnection isoConnection = IsoConnection_create(connectionSocket, self);

        if (isoConnection) {
            addClientConnection(self, isoConnection);
        }
        else {
            if (DEBUG_ISO_SERVER)
                libmms_iso_log(MMS_LOG_INFO, "ISO_SERVER: close connection");
        }

    }

#if (CONFIG_MMS_SINGLE_THREADED == 1)
    handleClientConnections(self);
#endif
}
#endif /* (CONFIG_MMS_THREADLESS_STACK == 0) */

/* used by non-threaded version */
static void
handleIsoConnectionsThreadless(IsoServer self)
{
    Socket connectionSocket;

    if ((connectionSocket = ServerSocket_accept((ServerSocket) self->serverSocket)) != NULL) {

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
        if (self->maxConnections > -1) {
            if (private_IsoServer_getConnectionCounter(self) >= self->maxConnections) {
                if (DEBUG_ISO_SERVER)
                    libmms_iso_log(MMS_LOG_WARNING, "ISO_SERVER: maximum number of connections reached -> reject connection attempt.");

                Socket_destroy(connectionSocket);

                return;
            }
        }
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS != -1)
        if (private_IsoServer_getConnectionCounter(self) >= CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS) {
            if (DEBUG_ISO_SERVER)
                libmms_iso_log(MMS_LOG_WARNING, "ISO_SERVER: maximum number of connections reached -> reject connection attempt.");

            Socket_destroy(connectionSocket);

            handleClientConnections(self);

            return;
        }
#endif

        IsoConnection isoConnection = IsoConnection_create(connectionSocket, self);

        if (isoConnection) {

            addClientConnection(self, isoConnection);

            self->connectionHandler(ISO_CONNECTION_OPENED, self->connectionHandlerParameter,
                    isoConnection);

        }
        else {
            if (DEBUG_ISO_SERVER)
                libmms_iso_log(MMS_LOG_INFO, "ISO_SERVER: close connection");

            Socket_destroy(connectionSocket);
        }
    }

    handleClientConnections(self);
}

static void
callAppTickHandler(IsoServer self)
{
    if (self->appTickHandler) {
        self->appTickHandler(self->appHandlerParameter);
    }
}

#if (CONFIG_MMS_THREADLESS_STACK != 1)
/* only required for multi-threaded server! */
static void
isoServerThread(void* isoServerParam)
{
    IsoServer self = (IsoServer) isoServerParam;

    if (DEBUG_ISO_SERVER)
        libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: new server thread started");

    if (!setupIsoServer(self)) {
        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_ERROR, "ISO_SERVER: starting server failed!");

        goto cleanUp;
    }

    while (self->state == ISO_SVR_STATE_RUNNING)
    {
        handleIsoConnections(self);

        callAppTickHandler(self);

        Thread_sleep(1);
    }

    self->state = ISO_SVR_STATE_STOPPED;

cleanUp:
    self->serverSocket = NULL;

    if (DEBUG_ISO_SERVER)
        libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: server thread stopped", &isoServerParam);

}
#endif

IsoServer
IsoServer_create(TLSConfiguration tlsConfiguration, IsoApplicationAddress localAppAddr)
{
    IsoServer self = (IsoServer) GLOBAL_CALLOC(1, sizeof(struct sIsoServer));

    self->state = ISO_SVR_STATE_IDLE;

    if (tlsConfiguration == NULL)
        self->tcpPort = TCP_PORT;
    else
        self->tcpPort = SECURE_TCP_PORT;

    self->tlsConfiguration = tlsConfiguration;
    self->localAppAddr = localAppAddr;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    self->stateLock = Semaphore_create(1);
#endif

#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1)
    self->openClientConnections = LinkedList_create();
#endif

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
    self->maxConnections = CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS;
#endif

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    self->connectionCounterMutex = Semaphore_create(1);
    self->openClientConnectionsMutex = Semaphore_create(1);
#endif /* (CONFIG_MMS_THREADLESS_STACK != 1) */

    self->connectionCounter = 0;

    return self;
}

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
void
IsoServer_setMaxConnections(IsoServer self, int maxConnections)
{
    self->maxConnections = maxConnections;
}
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

void
IsoServer_setAppTickHandler(IsoServer self, MmsAppTickHandler handler, void* parameter)
{
    self->appTickHandler = handler;
    self->appHandlerParameter = parameter;
}

void
IsoServer_setTcpPort(IsoServer self, int port)
{
    self->tcpPort = port;
}

void
IsoServer_setLocalIpAddress(IsoServer self, const char* ipAddress)
{
    if (self->localIpAddress)
        GLOBAL_FREEMEM(self->localIpAddress);

	self->localIpAddress = StringUtils_copyString(ipAddress);
}

IsoServerState
IsoServer_getState(IsoServer self)
{
    return getState(self);
}

void
IsoServer_setAuthenticator(IsoServer self, AcseAuthenticator authenticator, void* authenticatorParameter)
{
    self->authenticator = authenticator;
    self->authenticatorParameter = authenticatorParameter;
}

AcseAuthenticator
IsoServer_getAuthenticator(IsoServer self)
{
    return self->authenticator;
}

void*
IsoServer_getAuthenticatorParameter(IsoServer self)
{
    return self->authenticatorParameter;
}

TLSConfiguration
IsoServer_getTLSConfiguration(IsoServer self)
{
    return self->tlsConfiguration;
}

void
IsoServer_setTLSConfiguration(IsoServer self, TLSConfiguration tlsConfig)
{
    self->tlsConfiguration = tlsConfig;
}

#if (CONFIG_MMS_THREADLESS_STACK != 1)
void
IsoServer_startListening(IsoServer self)
{
    if (self->state == ISO_SVR_STATE_RUNNING) {
        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: server already in RUNNING state!");

        goto exit_function;
    }

    self->state = ISO_SVR_STATE_IDLE;

    self->serverThread = Thread_create((ThreadExecutionFunction) isoServerThread, self, false);

    Thread_start(self->serverThread);

    /* wait until server is up */
    while (self->state == ISO_SVR_STATE_IDLE)
        Thread_sleep(1);

exit_function:
    return;
}
#endif /* (CONFIG_MMS_THREADLESS_STACK != 1) */

void
IsoServer_startListeningThreadless(IsoServer self)
{
    if (!setupIsoServer(self)) {
        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_ERROR, "ISO_SERVER: starting server failed!");

        self->serverSocket = NULL;
    }
    else {
        setState(self, ISO_SVR_STATE_RUNNING);

        if (DEBUG_ISO_SERVER)
            libmms_iso_log(MMS_LOG_INFO, "ISO_SERVER: new iso server (threadless) started");
    }
}

int
IsoServer_waitReady(IsoServer self, unsigned int timeoutMs)
{
   int result;

   if (getState(self) == ISO_SVR_STATE_RUNNING) {
       HandleSet handles;

       handles = Handleset_new();
       if (handles != NULL) {

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
           lockClientConnections(self);
#endif

#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1)

           LinkedList openConnection = LinkedList_getNext(self->openClientConnections);
           LinkedList lastConnection = self->openClientConnections;

           while (openConnection != NULL) {
               IsoConnection isoConnection = (IsoConnection) openConnection->data;

               if (IsoConnection_isRunning(isoConnection)) {
                   IsoConnection_addHandleSet(isoConnection, handles);
                   openConnection = LinkedList_getNext(openConnection);
               } else {
#if ((CONFIG_MMS_SINGLE_THREADED == 1) || (CONFIG_MMS_THREADLESS_STACK == 1))
                   IsoConnection_destroy(isoConnection);
#endif
                   lastConnection->next = openConnection->next;
                   GLOBAL_FREEMEM(openConnection);
                   openConnection = lastConnection->next;
               }

               lastConnection = lastConnection->next;
           }

#else
           int i;

           for (i = 0; i < CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS; i++) {
               if (self->openClientConnections[i] != NULL) {
                   if (IsoConnection_isRunning(self->openClientConnections[i])) {
                       IsoConnection_addHandleSet(self->openClientConnections[i], handles);
                   }
                   else {
#if ((CONFIG_MMS_SINGLE_THREADED == 1) || (CONFIG_MMS_THREADLESS_STACK == 1))
                       IsoConnection_destroy(self->openClientConnections[i]);
#endif
                       self->openClientConnections[i] = NULL;
                   }
               }
           }

#endif /* (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1) */

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
           unlockClientConnections(self);
#endif

           Handleset_addSocket(handles, self->serverSocket);
           result = Handleset_waitReady(handles, timeoutMs);
           Handleset_destroy(handles);
       } else {
           result = -1;
       }
   } else {
       result = -1;
   }

   return result;
}


void
IsoServer_processIncomingMessages(IsoServer self)
{
    if (getState(self) == ISO_SVR_STATE_RUNNING)
        handleIsoConnectionsThreadless(self);
}

static void
stopListening(IsoServer self)
{
    setState(self, ISO_SVR_STATE_STOPPED);

    if (self->serverSocket != NULL) {
        ServerSocket_destroy((ServerSocket) self->serverSocket);
        self->serverSocket = NULL;
    }
}

void
IsoServer_stopListeningThreadless(IsoServer self)
{
    stopListening(self);

    closeAllOpenClientConnections(self);

    if (DEBUG_ISO_SERVER)
        libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: server (non-thread) finished");
}

#if (CONFIG_MMS_THREADLESS_STACK != 1)
void
IsoServer_stopListening(IsoServer self)
{
    stopListening(self);

    if (self->serverThread != NULL) {
        Thread_destroy(self->serverThread);
        self->serverThread = NULL;
    }

    closeAllOpenClientConnections(self);

    /* Wait for connection threads to finish */
    while (private_IsoServer_getConnectionCounter(self) > 0)
        Thread_sleep(10);

    if (DEBUG_ISO_SERVER)
        libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: stop listening finished!");
}
#endif

void
IsoServer_closeConnection(IsoServer self, IsoConnection isoConnection)
{
    if (getState(self) != ISO_SVR_STATE_IDLE) {
        self->connectionHandler(ISO_CONNECTION_CLOSED, self->connectionHandlerParameter,
                isoConnection);
    }

    removeClientConnection(self, isoConnection);
}

void
IsoServer_setConnectionHandler(IsoServer self, ConnectionIndicationHandler handler,
        void* parameter)
{
    self->connectionHandler = handler;
    self->connectionHandlerParameter = parameter;
}

void
IsoServer_destroy(IsoServer self)
{

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    if (self->state == ISO_SVR_STATE_RUNNING)
        IsoServer_stopListening(self);
#endif

    if (self->serverThread) {
        Thread_destroy(self->serverThread);
    }

#if (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1)

#if (CONFIG_MMS_SINGLE_THREADED == 1)
    if (self->openClientConnections != NULL)
        LinkedList_destroy(self->openClientConnections);
#else
    if (self->openClientConnections != NULL)
        LinkedList_destroyStatic(self->openClientConnections);
#endif /* (CONFIG_MMS_SINGLE_THREADED == 1) */

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    lockClientConnections(self);
#endif

#endif /* (CONFIG_MAXIMUM_TCP_CLIENT_CONNECTIONS == -1) */

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    Semaphore_destroy(self->connectionCounterMutex);
    Semaphore_destroy(self->openClientConnectionsMutex);
#endif

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_destroy(self->stateLock);
#endif

    if (self->localIpAddress)
        GLOBAL_FREEMEM(self->localIpAddress);

    GLOBAL_FREEMEM(self);
}

void
private_IsoServer_decreaseConnectionCounter(IsoServer self)
{
#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    Semaphore_wait(self->connectionCounterMutex);
#endif

    self->connectionCounter--;

    if (DEBUG_ISO_SERVER)
        libmms_iso_log(MMS_LOG_DEBUG, "ISO_SERVER: connection counter %i", self->connectionCounter);

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    Semaphore_post(self->connectionCounterMutex);
#endif
}

int
private_IsoServer_getConnectionCounter(IsoServer self)
{
    int connectionCounter;

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    Semaphore_wait(self->connectionCounterMutex);
#endif

    connectionCounter = self->connectionCounter;

#if (CONFIG_MMS_THREADLESS_STACK != 1) && (CONFIG_MMS_SINGLE_THREADED == 0)
    Semaphore_post(self->connectionCounterMutex);
#endif

    return connectionCounter;
}

#if (CONFIG_MMS_THREADLESS_STACK != 1)

void
IsoServer_setUserLock(IsoServer self, Semaphore userLock)
{
    self->userLock = userLock;
}

void
IsoServer_userLock(IsoServer self)
{
    if (self->userLock != NULL)
        Semaphore_wait(self->userLock);
}

void
IsoServer_userUnlock(IsoServer self)
{
    if (self->userLock != NULL)
        Semaphore_post(self->userLock);
}

#endif /* (CONFIG_MMS_THREADLESS_STACK != 1) */

void
IsoServer_callConnectionHandler(IsoServer self, IsoConnection connection, IsoConnectionIndication indication)
{
    if (self->connectionHandler)
        self->connectionHandler(indication, self->connectionHandlerParameter, connection);
}

IsoApplicationAddress
IsoServer_getLocalAppAddr(IsoServer self)
{
    return self->localAppAddr;
}
