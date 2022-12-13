/*
 * endpoint2.c
 *
 * Active endpoint with client and server role (dual-role).
 *
 * Show how to use an active (TCP client) dual-role endpoint
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
static bool peerDSTSEnabled = false;

static void
sigint_handler(int signalId)
{
    running = false;
}

static void
endpointStateChangedHandler(Tase2_Endpoint endpoint, void* parameter, Tase2_Endpoint_State state)
{
    char* stateStr;

    if (state == TASE2_ENDPOINT_STATE_IDLE) {
        stateStr = "IDLE";
        peerIMEnabled = false;
        peerDSTSEnabled = false;
    }
    else if (state == TASE2_ENDPOINT_STATE_CONNECTED)
        stateStr = "CONNECTED";
    else if (state == TASE2_ENDPOINT_STATE_CONNECTING)
        stateStr = "CONNECTING";
    else if (state == TASE2_ENDPOINT_STATE_LISTENING)
        stateStr = "LISTENING";
    else if (state == TASE2_ENDPOINT_STATE_ERROR) {
        peerIMEnabled = false;
        peerDSTSEnabled = false;
        stateStr = "ERROR";
    }

    printf("endpoint state changed: %s(%i)\n", stateStr, state);
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
        stateStr = "CLOSED";
    }

   // printf("Client %p state changed to %s\n", client, stateStr);
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

/* callback handler that is called twice for each received transfer set report */
static void
dsTransferSetReportHandler(void* parameter, bool finished, uint32_t seq, Tase2_ClientDSTransferSet transferSet)
{
    if (finished) {
        printf("--> (%i) report processing finished\n", seq);
    }
    else {
        printf("New report received with seq no: %u\n", seq);
    }
}

/* callback handler that is called for each data point of a received transfer set report */
static void
dsTransferSetValueHandler(void* parameter, Tase2_ClientDSTransferSet transferSet, const char* domainName, const char* pointName, Tase2_PointValue pointValue)
{
    printf("  Received value for %s:%s\n", domainName, pointName);
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

static int intVal = 0;
Tase2_IndicationPoint icc1Discrete1 = NULL;

static void*
updateValueThread(void* parameter)
{
    Tase2_Server server = (Tase2_Server) parameter;

    while (running) {

        if (icc1Discrete1) {
            Tase2_IndicationPoint_setDiscrete(icc1Discrete1, intVal++);
            Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1Discrete1);
        }

        Thread_sleep(100);
    }

    return NULL;
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

    /* Create a new domain "icc2" */
    Tase2_Domain icc1 = Tase2_DataModel_addDomain(dataModel, "icc1");

    /* Add some data points to domain "icc2" */
    icc1Discrete1 = Tase2_Domain_addIndicationPoint(icc1, "Discrete1", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc1, "Discrete2", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc1, "Discrete3", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, false);
    Tase2_Domain_addIndicationPoint(icc1, "DiscreteQTimeTag4", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_QUALITY, TASE2_TIMESTAMP, true, false);

    Tase2_Domain_addIndicationPoint(icc1, "Real1", TASE2_IND_POINT_TYPE_REAL, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc1, "RealQ2", TASE2_IND_POINT_TYPE_REAL, TASE2_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc1, "RealQTimeTag3", TASE2_IND_POINT_TYPE_REAL, TASE2_QUALITY, TASE2_TIMESTAMP, false, true);

    /* add a data set transfer set to domain "icc1" */
    Tase2_Domain_addDSTransferSet(icc1, "DSTrans1");

    /* add a new data set to domain "icc1" */
    Tase2_DataSet ds1 = Tase2_Domain_addDataSet(icc1, "ds1");

    Tase2_DataSet_addEntry(ds1, icc1, "Transfer_Set_Name");
    Tase2_DataSet_addEntry(ds1, icc1, "DSConditions_Detected");
    Tase2_DataSet_addEntry(ds1, icc1, "Event_Code_Detected");
    Tase2_DataSet_addEntry(ds1, icc1, "Transfer_Set_Time_Stamp");
    Tase2_DataSet_addEntry(ds1, icc1, "Discrete1");
    Tase2_DataSet_addEntry(ds1, icc1, "Discrete2");
    Tase2_DataSet_addEntry(ds1, icc1, "DiscreteQTimeTag4");
    Tase2_DataSet_addEntry(ds1, icc1, "Real1");
    Tase2_DataSet_addEntry(ds1, icc1, "RealQ2");
    Tase2_DataSet_addEntry(ds1, icc1, "RealQTimeTag3");

    /* create bilateral table with ID "BLT_MZA_002_V1" */
    Tase2_BilateralTable blt1 = Tase2_BilateralTable_create("BLT_MZA_001_V1", icc1, "1.1.999.1", 12);

    /* Add information message objects to bilateral table */
    Tase2_BilateralTable_addInformationMessage(blt1, 1, 1, true);
    Tase2_BilateralTable_addInformationMessage(blt1, 2, 2, false);

    /****************************************************************
     * TASE.2 endpoint setup
     ***************************************************************/

    char* hostname = "127.0.0.1";
    char* apTitle = "1.1.999.2";
    int aeQualifier = 12;

    if (argc > 1) {
        hostname = argv[1];

        if (argc > 2)
            apTitle = argv[2];

        if (argc > 3)
            aeQualifier = atoi(argv[3]);
    }

    Tase2_Endpoint endpoint = Tase2_Endpoint_create(useTLS ? tlsConfig : NULL, false);
    Tase2_Endpoint_setStateChangedHandler(endpoint, endpointStateChangedHandler, NULL);
    Tase2_Endpoint_setLocalApTitle(endpoint, apTitle, aeQualifier);
    Tase2_Endpoint_setRemoteIpAddress(endpoint, hostname);
    Tase2_Endpoint_setRemoteApTitle(endpoint, "1.1.999.1", 12);

    Tase2_Server server = Tase2_Server_createEx(dataModel, endpoint);

    Tase2_Server_addBilateralTable(server, blt1);

    Tase2_Client client = Tase2_Client_createEx(endpoint);

    Tase2_Client_setRemoteApTitle(client, "1.1.999.1", 12);
    Tase2_Client_setLocalApTitle(client, "1.1.999.2", 12);

    /* Get informed when the client state changes */
    Tase2_Client_installStateChangedHandler(client, clientStateChangedHandler, NULL);

    Tase2_Client_installIMTransferSetHandler(client, imTransferSetHandler, NULL);

    Tase2_Client_installDSTransferSetValueHandler(client, dsTransferSetValueHandler, NULL);

    /* this is optional to be informed when a new report is received and report processing is finished */
    Tase2_Client_installDSTransferSetReportHandler(client, dsTransferSetReportHandler, NULL);

    Tase2_ClientDataSet dataSet = NULL;
    Tase2_ClientDSTransferSet ts = NULL;

    Thread updateThread = Thread_create(updateValueThread, (void*) server, false);
    Thread_start(updateThread);

    bool firstConnected = true;
    int32_t msgId = 0;

    while (running) {

        printf("Connect...\n");
        Tase2_Endpoint_connect(endpoint);

        firstConnected = true;

        if (Tase2_Endpoint_waitForState(endpoint, TASE2_ENDPOINT_STATE_CONNECTED, 10000)) {

            printf("endpoint is connected\n");

            while (running) {

                if (Tase2_Client_getState(client) == TASE2_CLIENT_STATE_CONNECTED) {

                    Tase2_ClientError err;

                    /* request TASE.2 version and supported feature information from server */
                    if (firstConnected) {
                        Tase2_Client_connectEx(client);
                        firstConnected = false;
                    }

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

                        /* Enable IM (information message) transfer sets */
                        if (peerIMEnabled == false) {

                            Tase2_Client_IMTransferSetEnable(client, &err);

                            if (err != TASE2_CLIENT_ERROR_OK)
                                printf("Failed to enable IM transfer set (error=%d)\n", err);
                            else
                                peerIMEnabled = true;
                        }

                        /* Enable DSTS */
                        if (peerDSTSEnabled == false) {

                            if (dataSet)
                                Tase2_ClientDataSet_destroy(dataSet);

                            if (ts)
                                Tase2_ClientDSTransferSet_destroy(ts);

                            dataSet = Tase2_Client_getDataSet(client, &err, "icc1", "ds1");

                            if (err != TASE2_CLIENT_ERROR_OK) {
                                printf("Failed to get data set (error=%d)\n", err);
                            }

                            ts = Tase2_Client_getNextDSTransferSet(client, "icc1", &err);

                            if (err != TASE2_CLIENT_ERROR_OK) {
                                printf("Failed to get next DS transfer set (error=%d)\n", err);
                            }
                            else {

                                Tase2_ClientDSTransferSet_setDataSet(ts, dataSet);

                                printf("DSTransferSet %s:%s\n", Tase2_ClientDSTransferSet_getDomain(ts), Tase2_ClientDSTransferSet_getName(ts));

                                Tase2_ClientDSTransferSet_readValues(ts, client);

                                printf("  data-set: %s:%s\n", Tase2_ClientDSTransferSet_getDataSetDomain(ts), Tase2_ClientDSTransferSet_getDataSetName(ts));

                                if (Tase2_ClientDSTransferSet_writeDataSetName(ts, client, "icc1", "ds1") != TASE2_CLIENT_ERROR_OK)
                                    printf("Write data set failed!\n");

                                if (Tase2_ClientDSTransferSet_writeInterval(ts, client, 1) != TASE2_CLIENT_ERROR_OK)
                                    printf("Write interval failed!\n");

                                if (Tase2_ClientDSTransferSet_writeRBE(ts, client, true) != TASE2_CLIENT_ERROR_OK)
                                    printf("Write RBE failed!\n");

                                if (Tase2_ClientDSTransferSet_writeCritical(ts, client, true) != TASE2_CLIENT_ERROR_OK)
                                    printf("Write critical failed!\n");

                                if (Tase2_ClientDSTransferSet_writeDSConditionsRequested(ts, client, TASE2_DS_CONDITION_INTERVAL) != TASE2_CLIENT_ERROR_OK)
                                    printf("Failed to write DS conditions requested!\n");

                                printf("Start DSTransferSet\n");

                                if (Tase2_ClientDSTransferSet_writeStatus(ts, client, true) != TASE2_CLIENT_ERROR_OK)
                                    printf("  failed!\n");
                                else
                                    peerDSTSEnabled = true;
                            }

                        }

                        LinkedList domainNames = Tase2_Client_getDomainDataSets(client, "icc1", &err);

                        if (err == TASE2_CLIENT_ERROR_OK)
                        {
                            LinkedList_destroy(domainNames);
                        }
                        else {
                            printf("Failed to read domain data sets (error=%i)\n", err);
                            Thread_sleep(500);
                        }

                    }
                    else {
                        printf("Failed to get peer identity (error=%d)\n", err);
                        Tase2_Endpoint_disconnect(endpoint, 0);
                        break;
                    }

                }

                /* Send information reports */

                char* testMessage = "test info message (from active endpoint)";
                Tase2_Server_sendInformationMessage(server, NULL, 1, 1, msgId, strlen(testMessage), (uint8_t*) testMessage);

                msgId++;

                if (Tase2_Endpoint_getState(endpoint) == TASE2_ENDPOINT_STATE_CONNECTED)
                    Thread_sleep(1000);
                else
                    break;
            }

            Tase2_Endpoint_disconnect(endpoint, 0);
        }
        else {
            printf("Failed to connect\n");
        }

        Thread_sleep(1000);
    }

    /*****************
     * Cleanup
     *****************/

    Thread_destroy(updateThread);

    if (dataSet)
        Tase2_ClientDataSet_destroy(dataSet);

    if (ts)
        Tase2_ClientDSTransferSet_destroy(ts);

    Tase2_Endpoint_destroy(endpoint);

    /* Tase2_Client and Tase2_Server have to be released after calling Tase2_Endpoint_destroy! */
    Tase2_Client_destroy(client);

    Tase2_Server_destroy(server);

    Tase2_DataModel_destroy(dataModel);

    TLSConfiguration_destroy(tlsConfig);
}
