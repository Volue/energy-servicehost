/*
 * browse_client.c
 *
 * Client example for libtase2 C API
 *
 * Can be used with basic_server example.
 *
 * Shows how to:
 * - browse the namespace of a server
 * - show data sets and data set directories
 */
#include <stdio.h>
#include <stdlib.h>
#include "tase2_client.h"
#include "hal_thread.h"

/**
 * callback handler for library log messages
 */
static void
logHandler (Tase2_LogLevel logLevel, Tase2_LogSource source, Tase2_Endpoint endpoint, Tase2_Endpoint_Connection peer, const char* message)
{
    printf("-- LOG (%i) [%s][%s]: %s\n", logLevel, endpoint ? Tase2_Endpoint_getId(endpoint) : "-",
            peer ? Tase2_Endpoint_Connection_getPeerIpAddress(peer) : "-", message);
}

int
main (int argc, char** argv)
{
    /* Setup logging */
    Tase2_Library_setLogLevel(TASE2_LOG_INFO);
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

    Tase2_ClientError err = Tase2_Client_connect(client, hostname, apTitle, 12);

    if (err ==  TASE2_CLIENT_ERROR_OK) {

        LinkedList vmdVariables = Tase2_Client_getVCCVariables(client, &err);

        if (err == TASE2_CLIENT_ERROR_OK) {

            printf("VCC variables:\n");

            LinkedList variable = LinkedList_getNext(vmdVariables);

            while (variable) {

                char* variableName = (char*) LinkedList_getData(variable);

                printf("  %s\n", variableName);

                variable = LinkedList_getNext(variable);

            }

            LinkedList_destroy(vmdVariables);

        }
        else {
            printf("ERROR: Failed to read VCC variables!\n");
        }

        LinkedList domainNames = Tase2_Client_getDomainNames(client, &err);

        if (err == TASE2_CLIENT_ERROR_OK) {

            LinkedList domain = LinkedList_getNext(domainNames);

            while (domain) {

                char* domainName = (char*) LinkedList_getData(domain);

                LinkedList domainVariables = Tase2_Client_getDomainVariables(client, domainName, &err);

                if (err == TASE2_CLIENT_ERROR_OK) {

                    printf("Domain %s variables:\n", domainName);

                    LinkedList variable = LinkedList_getNext(domainVariables);

                    while (variable) {

                        char* variableName = (char*) LinkedList_getData(variable);

                        printf("  %s\n", variableName);

                        variable = LinkedList_getNext(variable);

                    }

                    LinkedList_destroy(domainVariables);
                }
                else {
                    printf("ERROR: Failed to read variables for domain %s\n", domainName);
                }

                LinkedList domainDataSets = Tase2_Client_getDomainDataSets(client, domainName, &err);

                if (err == TASE2_CLIENT_ERROR_OK) {

                    printf("Domain %s data sets:\n", domainName);

                    LinkedList dataset = LinkedList_getNext(domainDataSets);

                    while (dataset) {

                        char* datasetName = (char*) LinkedList_getData(dataset);

                        printf("  %s\n", datasetName);

                        LinkedList datasetDirectory = Tase2_Client_getDataSetDirectory(client, domainName, datasetName, &err);

                        if (err == TASE2_CLIENT_ERROR_OK) {
                            LinkedList dataSetEntry = LinkedList_getNext(datasetDirectory);

                            while (dataSetEntry) {

                                Tase2_DataSetEntrySpec entry = (Tase2_DataSetEntrySpec) LinkedList_getData(dataSetEntry);

                                printf("    %s:%s\n", entry->domainId == NULL ? "VCC" : entry->domainId, entry->itemId);

                                dataSetEntry = LinkedList_getNext(dataSetEntry);
                            }

                            LinkedList_destroyDeep(datasetDirectory, (LinkedListValueDeleteFunction) Tase2_DataSetEntrySpec_destroy);
                        }
                        else {
                            printf("ERROR: Failed to read data set directory for %s/%s\n", domainName, datasetName);
                        }


                        dataset = LinkedList_getNext(dataset);

                    }

                    LinkedList_destroy(domainDataSets);
                }
                else {
                    printf("ERROR: Failed to read data sets for domain %s\n", domainName);
                }


                domain = LinkedList_getNext(domain);

            }

            LinkedList_destroy(domainNames);

        }
        else {
            printf("ERROR: Failed to read domain names!\n");
        }

        Tase2_Client_disconnect(client);
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
