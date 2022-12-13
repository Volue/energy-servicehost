/*
 * passive_client.c
 *
 * Client example for libtase2 C API
 *
 * Show how to use a TASE.2 client with a passive (TCP server) endpoint.
 *
 * Can be used with active_server example.
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

#include "tase2_client.h"
#include "tase2_server.h"
#include "hal_thread.h"

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

static void
connectionClosedHandler(void* parameter, Tase2_Client connection)
{
    printf("  Connection closed by server!\n");
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
     * TASE.2 client connection setup
     ***************************************************************/

    char* apTitle = "1.1.1.998";
    int aeQualifier = 12;

    if (argc > 1) {
        apTitle = argv[1];

        if (argc > 2)
            aeQualifier = atoi(argv[2]);
    }

    Tase2_ClientDSTransferSet ts;

    Tase2_Endpoint endpoint = Tase2_Endpoint_create(useTLS ? tlsConfig : NULL, true);
    Tase2_Endpoint_setLocalApTitle(endpoint, apTitle, aeQualifier);
    Tase2_ApplicationAddress localAppAddr = Tase2_Endpoint_getLocalApplicationAddress(endpoint);

    localAppAddr->pSelector.size = 4;
    localAppAddr->pSelector.value[0] = 0;
    localAppAddr->pSelector.value[1] = 0;
    localAppAddr->pSelector.value[2] = 1;
    localAppAddr->pSelector.value[3] = 2;


    Tase2_Server server = Tase2_Server_createEx(dataModel, endpoint);

    Tase2_Client client = Tase2_Client_createEx(endpoint);

    Tase2_Client_setRemoteApTitle(client, "1.1.1.997", 12);
    Tase2_Client_setLocalApTitle(client, apTitle, 12);

    Tase2_Client_installDSTransferSetValueHandler(client, dsTransferSetValueHandler, NULL);

    /* this is optional to be informed when a new report is received and report processing is finished */
    Tase2_Client_installDSTransferSetReportHandler(client, dsTransferSetReportHandler, NULL);

    /* Get informed when the server closes the connection */
    Tase2_Client_installConnectionClosedHandler(client, connectionClosedHandler, NULL);

    Tase2_Endpoint_connect(endpoint);

    if (Tase2_Endpoint_waitForState(endpoint, TASE2_ENDPOINT_STATE_LISTENING, 1000)) {

        printf("endpoint is listening for incoming connections\n");

        while (Tase2_Client_getState(client) != TASE2_CLIENT_STATE_CONNECTED)
            Thread_sleep(200);


        if (Tase2_Client_getState(client) == TASE2_CLIENT_STATE_CONNECTED) {

            Tase2_ClientError err;

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

            /***************************************************************
             * Handle IM (information message) transfer sets
             **************************************************************/

            Tase2_Client_installIMTransferSetHandler(client, imTransferSetHandler, NULL);

            Tase2_Client_IMTransferSetEnable(client, &err);

            if (err != TASE2_CLIENT_ERROR_OK)
                printf("Failed to enable IM transfer set (error=%d)\n", err);

            /***************************************************************
             * Delete old dynamic data set (if existing)
             ***************************************************************/

            bool isDeleted = Tase2_Client_deleteDataSet(client, &err, "icc1", "DSR_AUTO_0000");

            if (isDeleted == false) {
                printf("Failed to delete data set (error: %d)\n", err);
            }

            /***************************************************************
             * Create a new dynamic data set
             ***************************************************************/

            LinkedList newDataSet = LinkedList_create();

            LinkedList_add(newDataSet, "icc1/Transfer_Set_Name");
            LinkedList_add(newDataSet, "icc1/Transfer_Set_Time_Stamp");
            LinkedList_add(newDataSet, "icc1/DSConditions_Detected");
            LinkedList_add(newDataSet, "VCCDiscrete1");
            LinkedList_add(newDataSet, "VCCDiscreteQTimeTag1");
            LinkedList_add(newDataSet, "icc1/Discrete1");
            LinkedList_add(newDataSet, "icc1/Discrete2");
            LinkedList_add(newDataSet, "icc1/Discrete3");
            LinkedList_add(newDataSet, "icc1/DiscreteQTimeTag4");
            LinkedList_add(newDataSet, "icc1/Real1");
            LinkedList_add(newDataSet, "icc1/RealQ2");
            LinkedList_add(newDataSet, "icc1/RealQTimeTag3");
            LinkedList_add(newDataSet, "icc1/State1");
            LinkedList_add(newDataSet, "State1");
            LinkedList_add(newDataSet, "State2");
            LinkedList_add(newDataSet, "State3");
            LinkedList_add(newDataSet, "State4");

            if (Tase2_Client_createDataSet(client, &err, "icc1", "DSR_AUTO_0000", newDataSet) == false)
                printf("Failed to create new data set!\n");

            LinkedList_destroyStatic(newDataSet);

            /***************************************************************
             * Read data set information and values
             ***************************************************************/

            Tase2_ClientDataSet dataSet = Tase2_Client_getDataSet(client, &err, "icc1", "DSR_AUTO_0000");

            if (dataSet) {
                printf("Data set has %i elements\n", Tase2_ClientDataSet_getSize(dataSet));

                if (Tase2_ClientDataSet_read(dataSet, client) == TASE2_CLIENT_ERROR_OK) {

                    int i;

                    for (i = 0; i < Tase2_ClientDataSet_getSize(dataSet); i++) {
                        Tase2_PointName pointName = Tase2_ClientDataSet_getPointName(dataSet, i);
                        Tase2_PointValue pointValue = Tase2_ClientDataSet_getPointValue(dataSet, i);

                        if (pointValue != NULL) {
                            printf("  domain: %s point-name: %s type: %i\n", pointName->domainName, pointName->pointName, Tase2_PointValue_getType(pointValue));

                            if (Tase2_PointValue_getType(pointValue) == TASE2_VALUE_TYPE_DISCRETE) {
                                printf("  value: %i\n", Tase2_PointValue_getValueDiscrete(pointValue));
                            }

                            Tase2_PointValue_destroy(pointValue);
                        }
                    }

                }
                else {
                    printf("Read data set failed\n");
                }
            }
            else {
                printf("Failed to read data set information!\n");
            }

            /***************************************************************
             * Configure and enable DS transfer set
             ***************************************************************/

            ts = Tase2_Client_getNextDSTransferSet(client, "icc1", &err);

            if (ts) {

                Tase2_ClientDSTransferSet_setDataSet(ts, dataSet);

                printf("DSTransferSet %s:%s\n", Tase2_ClientDSTransferSet_getDomain(ts), Tase2_ClientDSTransferSet_getName(ts));

                //Tase2_ClientDSTransferSet_readValues(ts, client);
                printf("Read data set\n");

                Tase2_ClientDSTransferSet_setDataSetName(ts, "icc1", "DSR_AUTO_0000");
                Tase2_ClientDSTransferSet_setInterval(ts, 5);
                Tase2_ClientDSTransferSet_setTLE(ts, 60);
                Tase2_ClientDSTransferSet_setBufferTime(ts, 2);
                Tase2_ClientDSTransferSet_setIntegrityCheck(ts, 30);
                Tase2_ClientDSTransferSet_setRBE(ts, true);
                Tase2_ClientDSTransferSet_setCritical(ts, false);
                Tase2_ClientDSTransferSet_setDSConditionsRequested(ts, TASE2_DS_CONDITION_INTERVAL | TASE2_DS_CONDITION_CHANGE);
                Tase2_ClientDSTransferSet_setStatus(ts, true);

                printf("Start DSTransferSet\n");
                if (Tase2_ClientDSTransferSet_writeValues(ts, client) != TASE2_CLIENT_ERROR_OK)
                    printf("Failed to write transfer set!\n");

            }
            else
                printf("GetNextDSTransferSet operation failed!\n");

            Thread_sleep(20000);

            //Tase2_Client_disconnect(client);

            if (ts)
                Tase2_ClientDSTransferSet_destroy(ts);

            if (dataSet)
                Tase2_ClientDataSet_destroy(dataSet);

        }
        else {
            printf("client: not connected with server\n");
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
