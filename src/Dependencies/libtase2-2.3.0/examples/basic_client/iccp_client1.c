/*
 * iccp_client1.c
 *
 * Client example for libtase2 C API
 *
 * Can be used with basic_server example.
 *
 * Shows how to:
 * - setup TLS
 * - Connect to a server
 * - Set tags
 * - Send commands
 * - Read point values
 * - Read data sets
 * - Configure and activate DS transfer sets
 * - disconnect and cleanup
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "tase2_client.h"
#include "hal_thread.h"
#include "iso_connection_parameters.h"

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
    printf("  Connection to server closed server!\n");
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
    case TASE2_LOG_SOURCE_TCP:
        sourceStr = "TCP";
        break;
    case TASE2_LOG_SOURCE_ISO_LAYERS:
        sourceStr = "ISO";
        break;
    default:
        sourceStr = "UNKOWN";
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

    if (TLSConfiguration_addCRLFromFile(tlsConfig, "blocked_certs.crl") == false) {
        printf("WARNING: Failed to load CRL file\n");
    }

    /****************************************************************
     * TASE.2 client connection setup
     ***************************************************************/

    char* hostname = "localhost";
    char* apTitle = "1.1.1.999";
    int aeQualifier = 12;

    if (argc > 1) {
        hostname = argv[1];

        if (argc > 2)
            apTitle = argv[2];

        if (argc > 3)
            aeQualifier = atoi(argv[3]);
    }

    Tase2_ClientDSTransferSet ts = NULL;

    Tase2_Client client = Tase2_Client_create(useTLS ? tlsConfig : NULL);

    Tase2_Client_setLocalApTitle(client, "1.1.1.998", 12);
    Tase2_Client_setRemoteApTitle(client, "1.1.1.999", 12);

    Tase2_Client_installDSTransferSetValueHandler(client, dsTransferSetValueHandler, NULL);

    /* this is optional to be informed when a new report is received and report processing is finished */
    Tase2_Client_installDSTransferSetReportHandler(client, dsTransferSetReportHandler, NULL);

    /* Get informed when the server closes the connection */
    Tase2_Client_installConnectionClosedHandler(client, connectionClosedHandler, NULL);

    Tase2_ClientError err = Tase2_Client_connect(client, hostname, apTitle, 12);

    if (err == TASE2_CLIENT_ERROR_OK) {


        /****************************************************************
         * Set a tag on a server device
         ***************************************************************/
        if (Tase2_Client_setTag(client, &err, NULL, "Command2", TASE2_TAG_CLOSE_ONLY_INHIBIT, "maintenance work in progress") == false)
        {
            printf("Set tag for Command2 failed! (error code=%i)\n", err);
        }

        /****************************************************************
         * Send a command to a server device
         ***************************************************************/
        if (Tase2_Client_sendCommand(client, &err, NULL, "Command2", TASE2_COMMAND_OPEN) == false)
        {
            printf("Send command failed !(error code=%i)\n", err);
        }
        else {
            printf("Send command success!\n");
        }

        /***************************************************************
         * Read a single (indication) point value
         ***************************************************************/

        Tase2_PointValue pointValue = Tase2_Client_readPointValue(client, &err, NULL, "VCCDiscreteQTimeTag1");

        if (err == TASE2_CLIENT_ERROR_OK) {
            printf("Read VCCDiscreteQTimeTag1: %d\n", Tase2_PointValue_getValueDiscrete(pointValue));

            Tase2_PointValue_destroy(pointValue);
        }
        else {
            printf("Failed to read value !(error code=%i)\n", err);
        }

        /***************************************************************
         * Read data set information and values
         ***************************************************************/

        Tase2_ClientDataSet dataSet = Tase2_Client_getDataSet(client, &err, "icc1", "ds1");

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

            /***************************************************************
             * Create a new dynamic data set
             ***************************************************************/

            LinkedList newDataSet = LinkedList_create();

            LinkedList_add(newDataSet, "icc1/Transfer_Set_Name");
            LinkedList_add(newDataSet, "icc1/Transfer_Set_Time_Stamp");
            LinkedList_add(newDataSet, "icc1/DSConditions_Detected");
            LinkedList_add(newDataSet, "VCCDiscreteQTimeTag1");
            LinkedList_add(newDataSet, "icc1/Discrete1");
            LinkedList_add(newDataSet, "icc1/Discrete2");

            if (Tase2_Client_createDataSet(client, &err, "icc1", "DSTest1", newDataSet) == false)
                printf("Failed to create new data set!\n");

            LinkedList_destroyStatic(newDataSet);

            /***************************************************************
             * Configure and enable DS transfer set
             ***************************************************************/

            ts = Tase2_Client_getNextDSTransferSet(client, "icc1", &err);

            if (ts) {

                Tase2_ClientDSTransferSet_setDataSet(ts, dataSet);

                printf("DSTransferSet %s:%s\n", Tase2_ClientDSTransferSet_getDomain(ts), Tase2_ClientDSTransferSet_getName(ts));

                Tase2_ClientDSTransferSet_readValues(ts, client);

                printf("  data-set: %s:%s\n", Tase2_ClientDSTransferSet_getDataSetDomain(ts), Tase2_ClientDSTransferSet_getDataSetName(ts));

                if (Tase2_ClientDSTransferSet_writeDataSetName(ts, client, "icc1", "ds1") != TASE2_CLIENT_ERROR_OK)
                    printf("Write data set failed!\n");

                Tase2_ClientDSTransferSet_readValues(ts, client);

                printf("  data-set: %s:%s\n", Tase2_ClientDSTransferSet_getDataSetDomain(ts), Tase2_ClientDSTransferSet_getDataSetName(ts));

                if (Tase2_ClientDSTransferSet_writeInterval(ts, client, 1) != TASE2_CLIENT_ERROR_OK)
                    printf("Write interval failed!\n");

                if (Tase2_ClientDSTransferSet_writeRBE(ts, client, true) != TASE2_CLIENT_ERROR_OK)
                    printf("Write RBE failed!\n");

                if (Tase2_ClientDSTransferSet_writeCritical(ts, client, true) != TASE2_CLIENT_ERROR_OK)
                    printf("Write critical failed!\n");

                if (Tase2_ClientDSTransferSet_writeDSConditionsRequested(ts, client, TASE2_DS_CONDITION_INTERVAL | TASE2_DS_CONDITION_CHANGE) != TASE2_CLIENT_ERROR_OK)
                    printf("Failed to write DS conditions requested!\n");

                printf("Start DSTransferSet\n");

                if (Tase2_ClientDSTransferSet_writeStatus(ts, client, true) != TASE2_CLIENT_ERROR_OK)
                    printf("  failed!\n");

            }
            else
                printf("GetNextDSTransferSet operation failed!\n");

        }
        else {
            printf("Failed to read data set information!\n");
        }


        Thread_sleep(10000);

        printf("Disconnect from server\n");
        Tase2_Client_disconnect(client);

        if (ts)
            Tase2_ClientDSTransferSet_destroy(ts);

        if (dataSet)
            Tase2_ClientDataSet_destroy(dataSet);
    }
    else {
        printf("Failed to connect\n");
    }

    /*****************
     * Cleanup
     *****************/

    Tase2_Client_destroy(client);

    TLSConfiguration_destroy(tlsConfig);
}
