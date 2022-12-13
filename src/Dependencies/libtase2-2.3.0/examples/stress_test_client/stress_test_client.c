/*
 * stress_test_client.c
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

void
logFunction (int logLevel, const char* message)
{

}

int
main (int argc, char** argv)
{
    /****************************************************************
     * Setup TLS configuration
     ***************************************************************/

    Tase2_Library_setLogFunction(logFunction);

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

    Tase2_Client* clients = NULL;

    int numberOfClients = 100;

    if (argc > 1) {
        hostname = argv[1];

        if (argc > 2)
            numberOfClients = atoi(argv[2]);
    }

    printf("Create %i client connections\n", numberOfClients);

    clients = (Tase2_Client*) calloc(numberOfClients, sizeof(Tase2_Client));

    Tase2_ClientDSTransferSet ts = NULL;

    for (int i = 0; i < numberOfClients; i++) {
        Tase2_Client client = Tase2_Client_create(useTLS ? tlsConfig : NULL);

        Tase2_Client_setRemoteApTitle(client, "1.1.1.999.1", 12);

        Tase2_Client_installDSTransferSetValueHandler(client, dsTransferSetValueHandler, NULL);

        /* this is optional to be informed when a new report is received and report processing is finished */
        Tase2_Client_installDSTransferSetReportHandler(client, dsTransferSetReportHandler, NULL);

        /* Get informed when the server closes the connection */
        Tase2_Client_installConnectionClosedHandler(client, connectionClosedHandler, NULL);

        Tase2_ClientError err = Tase2_Client_connect(client, hostname, apTitle, 12);

        if (err != TASE2_CLIENT_ERROR_OK) {
            printf("Client #%i failed to connect\n", i);
        }
        else {
            printf("Connection %i opened\n", i);
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

            char dsName[100];

            sprintf(dsName, "DSTest%i", i);

            if (Tase2_Client_createDataSet(client, &err, "icc1", dsName, newDataSet) == false)
                printf("Failed to create new data set!\n");
            else
                printf("Created data set %s\n", dsName);

            LinkedList_destroyStatic(newDataSet);
        }

        clients[i] = client;
    }

    printf("Waiting...\n");
    Thread_sleep(5000);


    /*****************
     * Cleanup
     *****************/

    for (int i = 0; i < numberOfClients; i++) {
        Tase2_Client client = clients[i];

        if (client) {
            Tase2_Client_destroy(client);
        }
    }

    free(clients);

    TLSConfiguration_destroy(tlsConfig);

    printf("Finished\n");
}
