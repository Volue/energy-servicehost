/*
 * endpoint1_passive.c
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
static bool peerDSTSEnabled = false;
static bool firstConnected = true;
static bool clientConnected = true;

static Tase2_IndicationPoint vccVCCDiscreteQTimeTag1;
static Tase2_IndicationPoint vccVCCDiscreteExtended;

static Tase2_IndicationPoint icc1Discrete1;
static Tase2_IndicationPoint icc1Discrete2;
static Tase2_IndicationPoint icc1Discrete3;
static Tase2_IndicationPoint icc1Discrete4;

static Tase2_ControlPoint vccCommand1;
static Tase2_ControlPoint vccCommand2;

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
    else if (newState == TASE2_CLIENT_STATE_CONNECTED) {
        clientConnected = true;
        stateStr = "CONNECTED";
    }
    else if (newState == TASE2_CLIENT_STATE_CLOSED) {
        peerIMEnabled = false;
        peerDSTSEnabled = false;
        firstConnected = true;
        clientConnected = false;
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

static int intVal = 0;

static void*
updateValueThread(void* parameter)
{
    Tase2_Server server = (Tase2_Server) parameter;

    while (running) {

        if (icc1Discrete1) {
            Tase2_IndicationPoint_setDiscrete(icc1Discrete1, intVal++);
            Tase2_IndicationPoint_setDiscrete(icc1Discrete2, intVal + 1);
            Tase2_IndicationPoint_setDiscrete(icc1Discrete3, intVal + 2);
            Tase2_IndicationPoint_setDiscrete(icc1Discrete4, intVal + 3);
            Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1Discrete1);
            Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1Discrete2);
            Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1Discrete3);
            Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1Discrete4);
        }

        Thread_sleep(100);
    }

    return NULL;
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
    bool longIM = false;

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

    /* Add some data points and devices to VCC scope (control center independent information) */
    Tase2_IndicationPoint vccDiscrete1 = Tase2_Domain_addIndicationPoint(vcc, "VCCDiscrete1", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    vccVCCDiscreteQTimeTag1 = Tase2_Domain_addIndicationPoint(vcc, "VCCDiscreteQTimeTag1", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_QUALITY, TASE2_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(vcc, "VCCDiscreteQExtendedTimeTag1", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_QUALITY, TASE2_TIMESTAMP_EXTENDED, true, true);
    vccVCCDiscreteExtended = Tase2_Domain_addIndicationPoint(vcc, "VCCDiscreteExtended", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_QUALITY, TASE2_TIMESTAMP, true, true);

    vccCommand1 = Tase2_Domain_addControlPoint(vcc, "Command1", TASE2_CONTROL_TYPE_COMMAND, TASE2_DEVICE_CLASS_SBO, true, 1365);
    vccCommand2 = Tase2_Domain_addControlPoint(vcc, "Command2", TASE2_CONTROL_TYPE_COMMAND, TASE2_DEVICE_CLASS_DIRECT, true, 1366);
    Tase2_ControlPoint vccCommand3 = Tase2_Domain_addControlPoint(vcc, "Command3", TASE2_CONTROL_TYPE_COMMAND, TASE2_DEVICE_CLASS_DIRECT, true, 1367);

    Tase2_IndicationPoint vccState1 = Tase2_Domain_addIndicationPoint(vcc, "State1", TASE2_IND_POINT_TYPE_STATE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_IndicationPoint vccState2 = Tase2_Domain_addIndicationPoint(vcc, "State2", TASE2_IND_POINT_TYPE_STATE, TASE2_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_IndicationPoint vccState3 = Tase2_Domain_addIndicationPoint(vcc, "State3", TASE2_IND_POINT_TYPE_STATE, TASE2_QUALITY, TASE2_TIMESTAMP, false, true);
    Tase2_IndicationPoint vccState4 = Tase2_Domain_addIndicationPoint(vcc, "State4", TASE2_IND_POINT_TYPE_STATE, TASE2_QUALITY, TASE2_TIMESTAMP_EXTENDED, false, true);

    /* Create a new domain "icc1" (for information to exchange with specific control center "icc1") */
    Tase2_Domain icc1 = Tase2_DataModel_addDomain(dataModel, "icc1");

    /* Add information buffer objects for IM */
    Tase2_Domain_addInformationMessageObject(icc1, 64);
    Tase2_Domain_addInformationMessageObject(icc1, 1024);

    /* Add some data points and devices to domain "icc1" */
    icc1Discrete1 = Tase2_Domain_addIndicationPoint(icc1, "Discrete1", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    icc1Discrete2 = Tase2_Domain_addIndicationPoint(icc1, "Discrete2", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    icc1Discrete3 = Tase2_Domain_addIndicationPoint(icc1, "Discrete3", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, false);
    icc1Discrete4 = Tase2_Domain_addIndicationPoint(icc1, "DiscreteQTimeTag4", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_QUALITY, TASE2_TIMESTAMP, true, false);

    Tase2_Domain_addIndicationPoint(icc1, "Real1", TASE2_IND_POINT_TYPE_REAL, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc1, "RealQ2", TASE2_IND_POINT_TYPE_REAL, TASE2_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc1, "RealQTimeTag3", TASE2_IND_POINT_TYPE_REAL, TASE2_QUALITY, TASE2_TIMESTAMP, false, true);

    Tase2_IndicationPoint icc1State1 = Tase2_Domain_addIndicationPoint(icc1, "State1", TASE2_IND_POINT_TYPE_STATE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);

    Tase2_Domain_addProtectionEquipment(icc1, "Protect1", false);
    Tase2_Domain_addProtectionEquipment(icc1, "ProtectPacked1", true);

    Tase2_Domain_addControlPoint(icc1, "Command1", TASE2_CONTROL_TYPE_COMMAND, TASE2_DEVICE_CLASS_SBO, true, 1365);

    /* add a data set transfer set to domain "icc1" */
    Tase2_Domain_addDSTransferSet(icc1, "DSTrans1");

    /* add a new data set to domain "icc1" */
    Tase2_DataSet ds1 = Tase2_Domain_addDataSet(icc1, "ds1");

    Tase2_DataSet_addEntry(ds1, icc1, "Transfer_Set_Name");
    Tase2_DataSet_addEntry(ds1, icc1, "DSConditions_Detected");
    Tase2_DataSet_addEntry(ds1, icc1, "Event_Code_Detected");
    Tase2_DataSet_addEntry(ds1, icc1, "Transfer_Set_Time_Stamp");
    Tase2_DataSet_addEntry(ds1, vcc, "VCCDiscrete1");
    Tase2_DataSet_addEntry(ds1, icc1, "Discrete1");
    Tase2_DataSet_addEntry(ds1, icc1, "Discrete2");
    Tase2_DataSet_addEntry(ds1, icc1, "DiscreteQTimeTag4");

    /* Create a new domain "icc2" */
    Tase2_Domain icc2 = Tase2_DataModel_addDomain(dataModel, "icc2");

    /* Add some data points to domain "icc2" */
    Tase2_Domain_addIndicationPoint(icc2, "Discrete1", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc2, "Discrete2", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc2, "Discrete3", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, false);
    Tase2_Domain_addIndicationPoint(icc2, "DiscreteQTimeTag4", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_QUALITY, TASE2_TIMESTAMP, true, false);

    Tase2_Domain_addIndicationPoint(icc2, "Real1", TASE2_IND_POINT_TYPE_REAL, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc2, "RealQ2", TASE2_IND_POINT_TYPE_REAL, TASE2_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    Tase2_Domain_addIndicationPoint(icc2, "RealQTimeTag3", TASE2_IND_POINT_TYPE_REAL, TASE2_QUALITY, TASE2_TIMESTAMP, false, true);

    /******************************************************************
    * Defining the bilateral tables (BLT)
    *
    * - if no BLT is available all data can be accessed
    * - only data/control points that are part of the BLT can be accessed by the client
    * - the client is identified by its application reference
    * - all data/control point that are part of the client's  ICC can be accessed
    *
    ******************************************************************/

    /* create bilateral table with ID "BLT_MZA_001_V1" */
    Tase2_BilateralTable blt1 = Tase2_BilateralTable_create("BLT_MZA_001_V1", icc1, "1.1.999.2", 12);

    /* add some access rules to the bilateral table */
    Tase2_BilateralTable_addDataPoint(blt1, (Tase2_DataPoint) vccDiscrete1, true, false);
    Tase2_BilateralTable_addDataPoint(blt1, (Tase2_DataPoint) vccVCCDiscreteQTimeTag1, true, false);

    Tase2_BilateralTable_addDataPoint(blt1, (Tase2_DataPoint) vccState1, true, false);
    Tase2_BilateralTable_addDataPoint(blt1, (Tase2_DataPoint) vccState2, true, false);
    Tase2_BilateralTable_addDataPoint(blt1, (Tase2_DataPoint) vccState3, true, false);
    Tase2_BilateralTable_addDataPoint(blt1, (Tase2_DataPoint) vccState4, true, false);

    Tase2_BilateralTable_addControlPoint(blt1, vccCommand1, 1234, true, true, true, true);
    Tase2_BilateralTable_addControlPoint(blt1, vccCommand2, 1236, true, true, true, true);
    Tase2_BilateralTable_addControlPoint(blt1, vccCommand3, 1235, true, true, true, true);

    /* Add information message objects to bilateral table */
    Tase2_BilateralTable_addInformationMessage(blt1, 1, 1, true);

    /* create bilateral table with ID "BLT_MZA_002_V1" */
    Tase2_BilateralTable blt2 = Tase2_BilateralTable_create("BLT_MZA_002_V1", icc2, "1.1.999.3", 12);

    /* add some access rules to the bilateral table */
    Tase2_BilateralTable_addDataPoint(blt2, (Tase2_DataPoint) vccDiscrete1, true, false);
    Tase2_BilateralTable_addControlPoint(blt2, vccCommand2, 1236, true, true, true, true);

    Tase2_BilateralTable_addDataPoint(blt2, (Tase2_DataPoint) vccState1, true, false);
    Tase2_BilateralTable_addDataPoint(blt2, (Tase2_DataPoint) vccState2, true, false);
    Tase2_BilateralTable_addDataPoint(blt2, (Tase2_DataPoint) vccState3, true, false);
    Tase2_BilateralTable_addDataPoint(blt2, (Tase2_DataPoint) vccState4, true, false);

    /* Add information message objects to bilateral table */
    Tase2_BilateralTable_addInformationMessage(blt2, 1, 1, true);
    Tase2_BilateralTable_addInformationMessage(blt2, 2, 2, false);

    /****************************************************************
     * TASE.2 endpoint setup
     ***************************************************************/

    char* localIpAddress = "0.0.0.0";
    char* apTitle = "1.1.999.1";
    int aeQualifier = 12;

    if (argc > 1) {
        localIpAddress = argv[1];

        if (argc > 2)
            apTitle = argv[2];

        if (argc > 3)
            aeQualifier = atoi(argv[3]);
    }

    Tase2_ClientDataSet dataSet = NULL;
    Tase2_ClientDSTransferSet ts = NULL;

    Tase2_Endpoint endpoint = Tase2_Endpoint_create(useTLS ? tlsConfig : NULL, true);
    Tase2_Endpoint_setLocalApTitle(endpoint, apTitle, aeQualifier);
    Tase2_Endpoint_setLocalIpAddress(endpoint, localIpAddress);

    Tase2_Endpoint_setConnectionHandler(endpoint, endpointConnectionHandler, NULL);

    Tase2_Server server = Tase2_Server_createEx(dataModel, endpoint);

    Tase2_Server_addBilateralTable(server, blt1);
    Tase2_Server_addBilateralTable(server, blt2);

    Tase2_Client client = Tase2_Client_createEx(endpoint);

    /* remote address is required for the client to associate with the correct incoming TCP client connection */
    Tase2_Client_setRemoteApTitle(client, "1.1.999.2", 12);

    /* Get informed when the client state changes */
    Tase2_Client_installStateChangedHandler(client, clientStateChangedHandler, NULL);

    Tase2_Client_installIMTransferSetHandler(client, imTransferSetHandler, NULL);

    Tase2_Client_installDSTransferSetReportHandler(client, dsTransferSetReportHandler, NULL);
    Tase2_Client_installDSTransferSetValueHandler(client, dsTransferSetValueHandler, NULL);

    Thread updateThread = Thread_create(updateValueThread, (void*) server, false);
    Thread_start(updateThread);

    Tase2_Endpoint_connect(endpoint);

    int intVal = 0;

    uint64_t nextInfoMessage = 0;

    if (Tase2_Endpoint_waitForState(endpoint, TASE2_ENDPOINT_STATE_LISTENING, 1000)) {

        printf("endpoint is listening for incoming connections\n");

        int32_t msgId = 0;

        while (running) {

            if (Tase2_Client_getState(client) == TASE2_CLIENT_STATE_CONNECTED) {

                Tase2_ClientError err;

                if (firstConnected) {

                    /* request TASE.2 version and supported feature information from server */
                    if  (Tase2_Client_connectEx(client)  == TASE2_CLIENT_ERROR_OK) {
                        clientConnected = true;

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
                            printf("Failed to get peer identity (error=%d)\n", err);
                    }

                    firstConnected = false;
                }

                if (clientConnected) {
                    /* Enable IM (information message) transfer sets */
                    if (peerIMEnabled == false) {

                        peerIMEnabled = true;

                        Tase2_Client_IMTransferSetEnable(client, &err);

                        if (err != TASE2_CLIENT_ERROR_OK)
                            printf("Failed to enable IM transfer set (error=%d)\n", err);
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

                            if (Tase2_ClientDSTransferSet_writeBufferTime(ts, client, 0) != TASE2_CLIENT_ERROR_OK)
                                printf("Write buffer time failed\n");

                            if (Tase2_ClientDSTransferSet_writeDSConditionsRequested(ts, client, TASE2_DS_CONDITION_INTERVAL | TASE2_DS_CONDITION_CHANGE) != TASE2_CLIENT_ERROR_OK)
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
            }

            Tase2_IndicationPoint_setDiscrete(icc1Discrete1, intVal++);
            Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1Discrete1);

            /* Send information reports */

            if (Hal_getTimeInMs() > nextInfoMessage) {


                char* testMessage;

                if (longIM) {
                    testMessage = "long test info message (from passive endpoint): long message with more than 64 byte that should cause a longer information report -  information buffer will be selected automatically.";
                    longIM = false;
                }
                else {
                    testMessage = "test info message (from passive endpoint)";
                    longIM = true;
                }

                Tase2_Server_sendInformationMessage(server, NULL, 1, 1, msgId, strlen(testMessage), (uint8_t*) testMessage);

                msgId++;

                nextInfoMessage = Hal_getTimeInMs() + 1000;
            }


            Thread_sleep(1);
        }

        Tase2_Endpoint_disconnect(endpoint, 0);
    }
    else {
        printf("Failed to connect\n");
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

    Tase2_Client_destroy(client);

    Tase2_Server_destroy(server);

    Tase2_DataModel_destroy(dataModel);

    TLSConfiguration_destroy(tlsConfig);
}
