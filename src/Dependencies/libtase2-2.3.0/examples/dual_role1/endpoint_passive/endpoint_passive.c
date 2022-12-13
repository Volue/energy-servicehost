/*
 * endpoint_passive.c
 *
 * Passive endpoint with client and server role (dual-role).
 *
 * Show how to use a passive (TCP server) dual-role endpoint
 *
 * Shows how to:
 * - setup TLS
 * - Connect to a server
 * - Enable an IM transfer set
 * - Create a new data set to be used by DS transfer set
 * - Configure and activate DS transfer sets
 * - disconnect and cleanup
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "tase2_client.h"
#include "tase2_server.h"
#include "hal_thread.h"

static bool running = true;
static bool peerIMEnabled = false;

static void
sigint_handler(int signalId)
{
    running = false;
}

static void
endpointConnectionHandler (Tase2_Endpoint endpoint, void* parameter, Tase2_Endpoint_Connection connection, bool connect)
{
    char* ipAddress = Tase2_Endpoint_Connection_getPeerIpAddress(connection);
    char* apTitle = Tase2_Endpoint_Connection_getPeerApTitle(connection);
    int aeQualifier = Tase2_Endpoint_Connection_getPeerAeQualifier(connection);
    int maxMmsPduSize = Tase2_Endpoint_Connection_getMaxPduSize(connection);

    if (connect) {
        printf("endpoint (IP: %s AP-title: %s AE-qualifier: %i max. MMS PDU: %i) connected\n", ipAddress, apTitle, aeQualifier, maxMmsPduSize);
    }
    else {
        printf("endpoint (IP: %s AP-title: %s AE-qualifier: %i) disconnected\n", ipAddress, apTitle, aeQualifier);
    }
}

static void
clientStateChangedHandler(void* parameter, Tase2_Client client, Tase2_ClientState newState)
{
    char* stateStr;

    if (newState == TASE2_CLIENT_STATE_IDLE)
        stateStr = "IDLE";
    else if (newState == TASE2_CLIENT_STATE_CONNECTED)
        stateStr = "CONNECTED";
    else if (newState == TASE2_CLIENT_STATE_CLOSED) {
        peerIMEnabled = false;
        stateStr = "CLOSED";
    }

    printf("Client %p state changed to %s\n", client, stateStr);
}


static void
imTransferSetHandler (void* parameter, int32_t infoRef, int32_t localRef, int32_t msgId, int32_t size, uint8_t* messageBuffer)
{
    printf("Received information message:\n");
    printf("  infoRef:  %d\n", infoRef);
    printf("  localRef: %d\n", localRef);
    printf("  msgId:    %d\n", msgId);
    printf("  message: (%.*s)\n", size, messageBuffer);
}

/**
 * callback handler for library log messages
 */
static void
logHandler (Tase2_LogLevel logLevel, Tase2_LogSource source, Tase2_Endpoint endpoint, Tase2_Endpoint_Connection peer, const char* message)
{
    const char* levelStr = "UNKNOWN";

    switch (logLevel) {
    case TASE2_LOG_DEBUG:
        levelStr = "DEBUG";
        break;
    case TASE2_LOG_INFO:
        levelStr = "INFO";
        break;
    case TASE2_LOG_WARNING:
        levelStr = "WARNING";
        break;
    case TASE2_LOG_ERROR:
        levelStr = "ERROR";
        break;
    default:
        break;
    }

    const char* sourceStr = "UNKNOWN";

    switch (source) {
    case TASE2_LOG_SOURCE_ICCP:
        sourceStr = "ICCP";
        break;
    case TASE2_LOG_SOURCE_MMS:
        sourceStr = "MMS";
        break;
    case TASE2_LOG_SOURCE_TLS:
        sourceStr = "TLS";
        break;
    case TASE2_LOG_SOURCE_ISO_LAYERS:
        sourceStr = "ISO";
        break;
    case TASE2_LOG_SOURCE_TCP:
        sourceStr = "TCP";
        break;
    }

    printf("-- LOG (%s|%s) [%s][%s]: %s\n", levelStr, sourceStr, endpoint ? Tase2_Endpoint_getId(endpoint) : "-",
            peer ? Tase2_Endpoint_Connection_getPeerIpAddress(peer) : "-", message);
}

int
main (int argc, char** argv)
{
    signal(SIGINT, sigint_handler);

    /* Set log level to INFO and redirect log messages to user provided function */
    Tase2_Library_setLogLevel(TASE2_LOG_DEBUG);
    Tase2_Library_setLogFunctionEx(logHandler);

    /****************************************************************
     * Setup TLS configuration
     ***************************************************************/

    bool useTLS = true;

    TLSConfiguration tlsConfig = TLSConfiguration_create();

    TLSConfiguration_setChainValidation(tlsConfig, true);
    TLSConfiguration_setAllowOnlyKnownCertificates(tlsConfig, false);

    if (TLSConfiguration_setOwnKeyFromFile(tlsConfig, "client1-key.pem", NULL) == false) {
        printf("Failed to load server private key file!\n");
        useTLS = false;
    }

    if (TLSConfiguration_setOwnCertificateFromFile(tlsConfig, "client1.cer") == false) {
        printf("Failed to load server certificate file!\n");
        useTLS = false;
    }

    if (TLSConfiguration_addCACertificateFromFile(tlsConfig, "root.cer") == false) {
        printf("Failed to load CA certificate file!\n");
        useTLS = false;
    }

    /****************************************************************
     * TASE.2 simple server setup
     ***************************************************************/
    Tase2_DataModel dataModel = Tase2_DataModel_create();

    /* get VCC pseudo domain to populate the VCC scope data model part */
    Tase2_Domain vcc = Tase2_DataModel_getVCC(dataModel);

    /* Add information buffer objects for IM */
    Tase2_Domain_addInformationMessageObject(vcc, 64);
    Tase2_Domain_addInformationMessageObject(vcc, 1024);

    /****************************************************************
     * TASE.2 endpoint setup
     ***************************************************************/

    char* localIpAddress = "0.0.0.0";
    char* apTitle = "1.1.1.999.1";
    int aeQualifier = 12;

    if (argc > 1) {
        localIpAddress = argv[1];

        if (argc > 2)
            apTitle = argv[2];

        if (argc > 3)
            aeQualifier = atoi(argv[3]);
    }

    Tase2_ClientDSTransferSet ts;

    Tase2_Endpoint endpoint = Tase2_Endpoint_create(useTLS ? tlsConfig : NULL, true);
    Tase2_Endpoint_setLocalApTitle(endpoint, apTitle, aeQualifier);
    Tase2_Endpoint_setLocalIpAddress(endpoint, localIpAddress);

    Tase2_Endpoint_setConnectionHandler(endpoint, endpointConnectionHandler, NULL);

    Tase2_Server server = Tase2_Server_createEx(dataModel, endpoint);

    Tase2_TSelector tSelector = { 2, { 0, 2 } };
    Tase2_SSelector sSelector = { 2, { 0, 3 } };
    Tase2_PSelector pSelector = { 4, { 0, 0, 0, 4 } };

    Tase2_Server_setLocalAddresses(server, pSelector, sSelector, tSelector);

    Tase2_Client client = Tase2_Client_createEx(endpoint);

    /* remote address is required for the client to associate with the correct incoming TCP client connection */
    Tase2_Client_setRemoteApTitle(client, "1.1.1.999.2", 12);

    /* Get informed when the client state changes */
    Tase2_Client_installStateChangedHandler(client, clientStateChangedHandler, NULL);

    Tase2_Client_installIMTransferSetHandler(client, imTransferSetHandler, NULL);

    Tase2_Endpoint_connect(endpoint);

    Tase2_Endpoint_setAddressMatchingRules(endpoint, true, true, true, true, true);

    if (Tase2_Endpoint_waitForState(endpoint, TASE2_ENDPOINT_STATE_LISTENING, 1000)) {

        printf("endpoint is listening for incoming connections\n");

        bool firstConnected = true;
        int32_t msgId = 0;

        while (running) {


            if (Tase2_Client_getState(client) == TASE2_CLIENT_STATE_CONNECTED) {

                Tase2_ClientError err;

                if (firstConnected) {

                    /* request TASE.2 version and supported feature information from server */
                    Tase2_Client_connectEx(client);

                    /***************************************************************
                     * Get peer identity
                     **************************************************************/

                    char* peerVendor;
                    char* peerModel;
                    char* peerRevision;

                    Tase2_Client_readPeerIdentity(client, &err, &peerVendor, &peerModel, &peerRevision);

                    if (err == TASE2_CLIENT_ERROR_OK) {
                        printf("Peer identity\n");
                        printf("  vendor: %s\n", peerVendor);
                        printf("  model: %s\n", peerModel);
                        printf("  revision: %s\n", peerRevision);

                        free(peerVendor);
                        free(peerModel);
                        free(peerRevision);
                    }
                    else
                        printf("Failed to get peer identity (errpr=%d)\n", err);

                    firstConnected = false;
                }

                /* Enable IM (information message) transfer sets */
                if (peerIMEnabled == false) {

                    peerIMEnabled = true;

                    Tase2_Client_IMTransferSetEnable(client, &err);

                    if (err != TASE2_CLIENT_ERROR_OK)
                        printf("Failed to enable IM transfer set (error=%d)\n", err);
                }

            }

            /* Send information reports */

            char* testMessage = "test info message (from passive endpoint)";
            Tase2_Server_sendInformationMessage(server, NULL, 1, 1, msgId, strlen(testMessage), (uint8_t*) testMessage);

            msgId++;


            Thread_sleep(1000);
        }

        Tase2_Endpoint_disconnect(endpoint, 0);
    }
    else {
        printf("Failed to connect\n");
    }

    /*****************
     * Cleanup
     *****************/

    Tase2_Endpoint_destroy(endpoint);

    /* Tase2_Client and Tase2_Server have to be released after calling Tase2_Endpoint_destroy! */
    Tase2_Client_destroy(client);

    Tase2_Server_destroy(server);

    Tase2_DataModel_destroy(dataModel);

    TLSConfiguration_destroy(tlsConfig);
}
