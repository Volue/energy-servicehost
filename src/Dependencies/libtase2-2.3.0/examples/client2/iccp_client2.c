/*
 * iccp_client2.c
 *
 * Client example for libtase2 C API
 *
 * Can be used with basic_server example.
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

int
main (int argc, char** argv)
{
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

    Tase2_ClientDSTransferSet ts;

    Tase2_Client client = Tase2_Client_create(useTLS ? tlsConfig : NULL);

    Tase2_Client_installDSTransferSetValueHandler(client, dsTransferSetValueHandler, NULL);

    /* this is optional to be informed when a new report is received and report processing is finished */
    Tase2_Client_installDSTransferSetReportHandler(client, dsTransferSetReportHandler, NULL);

    /* Get informed when the server closes the connection */
    Tase2_Client_installConnectionClosedHandler(client, connectionClosedHandler, NULL);

    Tase2_ClientError err = Tase2_Client_connect(client, hostname, apTitle, 12);

    if (err ==  TASE2_CLIENT_ERROR_OK) {

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
