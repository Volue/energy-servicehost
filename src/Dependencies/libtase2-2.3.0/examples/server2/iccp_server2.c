/*
 * iccp_server1.c
 * 
 * Server example for libtase2 C API
 *
 * Shows how to:
 * - define data models and bilateral tables
 * - setup TLS
 * - setup, start, and shutdown server
 * - update measurements and status values
 * - using callback functions 
 */

#include <signal.h>

#include "tase2_model.h"
#include "tase2_server.h"
#include "hal_thread.h"
#include "hal_time.h"
#include <stdio.h>
#include <string.h>

static bool running = true;

void sigint_handler(int signalId)
{
    running = false;
}

Tase2_IndicationPoint vccVCCDiscreteQTimeTag1;
Tase2_IndicationPoint vccVCCDiscreteExtended;

Tase2_IndicationPoint icc1Discrete1;
Tase2_IndicationPoint icc1Discrete2;
Tase2_IndicationPoint icc1Discrete3;
Tase2_IndicationPoint icc1DiscreteQTimetag4;

Tase2_ControlPoint vccCommand1;
Tase2_ControlPoint vccCommand2;

static int blt1ConnectionCount = 0;

/**
 * callback handler to accept or reject new client connections
 *
 * In this example the handler ensures that only a single client with matching the
 * BLT "BLT_MZA_001_V1" can connect.
 */
static bool
connectionAcceptHandler (Tase2_Endpoint endpoint, void* parameter, const char* peerAddress, Tase2_BilateralTable clientBlt,
        Tase2_AuthenticationMethod authType, uint8_t* authValue, int authValLen)
{
    if (!strcmp(Tase2_BilateralTable_getID(clientBlt), "BLT_MZA_001_V1")) {
        if (blt1ConnectionCount > 0)
            return false;

        blt1ConnectionCount++;
    }

    return true;
}

/**
 * callback handler for client connect/disconnect events
 */
static void
clientConnectionHandler (void* parameter, const char* clientAddress, Tase2_BilateralTable clientBlt, bool connect)
{
    if (connect) {
        printf("Client from %s connected\n", clientAddress);
    }
    else {
        printf("Client from %s disconnected\n", clientAddress);

        if (!strcmp(Tase2_BilateralTable_getID(clientBlt), "BLT_MZA_001_V1")) {
            blt1ConnectionCount--;
        }
    }

    if (clientBlt)
        printf("   client BLT: %s\n", Tase2_BilateralTable_getID(clientBlt));
}

/**
 * callback handler for critical report confirmation problems
 */
static void
criticalReportTimeoutHandler(void* parameter, Tase2_TransferSet transferSet, uint64_t timeSent, bool nack)
{
    printf("Critical report for %s sent %lu not handled by client - ", Tase2_TransferSet_getName(transferSet) , timeSent);

    if (nack)
        printf(" NACK\n");
    else
        printf(" TIMEOUT\n");
}

/**
 * callback handler for write access to data model
 */
static Tase2_HandlerResult
writeDataHandler(void* parameter, Tase2_Domain domain, Tase2_DataPoint dataPoint, Tase2_PointValue value)
{
    if (dataPoint == (Tase2_DataPoint) icc1Discrete3) {
        printf("Write access to ICC1/Discrete3 - value:%i\n", Tase2_PointValue_getValueDiscrete(value));

        if (Tase2_PointValue_getValueDiscrete(value) > 1000)
            return TASE2_RESULT_OBJECT_VALUE_INVALID;

    }
    else if (dataPoint == (Tase2_DataPoint) icc1DiscreteQTimetag4) {
        printf("Write access to ICC1/Discrete4 - value:%i\n", Tase2_PointValue_getValueDiscrete(value));
        printf("   COV: %u\n", Tase2_PointValue_getCOV(value));
        printf("   Flags: %u\n", Tase2_PointValue_getFlags(value));
        printf("   Time: %lu\n", Tase2_PointValue_getTimeStamp(value));

        if (Tase2_PointValue_getValueDiscrete(value) > 2000)
            return TASE2_RESULT_OBJECT_VALUE_INVALID;
    }

    return TASE2_RESULT_SUCCESS;
}

/**
 * callback handler for received select requests (device model)
 */
static Tase2_HandlerResult
selectHandler (void* parameter, Tase2_ControlPoint controlPoint)
{
    printf("Received operate command\n");

    if (controlPoint == vccCommand1) {
        printf("Received select for VCC:Command1\n");

        return TASE2_RESULT_SUCCESS;
    }

    return TASE2_RESULT_OBJECT_ACCESS_DENIED;
}

/**
 * callback handler for received operate requests (device model)
 */
static Tase2_HandlerResult
operateHandler(void* parameter, Tase2_ControlPoint controlPoint, Tase2_OperateValue value)
{
    printf("Received operate command\n");

    if (controlPoint == vccCommand1) {
        printf("Received operate command for VCC:Command1\n");

        return TASE2_RESULT_SUCCESS;
    }

    if (controlPoint == vccCommand2) {
        printf("Received operate command for VCC:Command2\n");

        return TASE2_RESULT_SUCCESS;
    }

    return TASE2_RESULT_OBJECT_ACCESS_DENIED;
}

/**
 * callback handler for received set tag requests
 */
static Tase2_HandlerResult
setTagHandler (void* parameter, Tase2_ControlPoint controlPoint, Tase2_TagValue value, const char* reason)
{
    printf("Set tag value %i for control %s\n", value, Tase2_DataPoint_getName((Tase2_DataPoint) controlPoint));

    printf("   reason given by client: %s\n", reason);

    return TASE2_RESULT_SUCCESS;
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
    }

    if (source == TASE2_LOG_SOURCE_ICCP)
        printf("-- LOG (%s|%s) [%s][%s]: %s\n", levelStr, sourceStr, endpoint ? Tase2_Endpoint_getId(endpoint) : "-",
                peer ? Tase2_Endpoint_Connection_getPeerIpAddress(peer) : "-", message);
}

int
main (int argc, char** argv)
{

    signal(SIGINT, sigint_handler);

    /*********************************************************************
     * Definition of the data model
     * - indication/control points
     * - protection equipment
     * - domains
     * - transfer sets
     * - data sets
     *
     *********************************************************************/

    /* Set log level to INFO and redirect log messages to user provided function */
    Tase2_Library_setLogLevel(TASE2_LOG_DEBUG);
    Tase2_Library_setLogFunctionEx(logHandler);

    Tase2_DataModel dataModel = Tase2_DataModel_create();

    /* get VCC pseudo domain to populate the VCC scope data model part */
    Tase2_Domain vcc = Tase2_DataModel_getVCC(dataModel);

    /* Add information buffer objects for IM */
    Tase2_Domain_addInformationMessageObject(vcc, 64);
    Tase2_Domain_addInformationMessageObject(vcc, 1024);

    /* Add some data points and devices to VCC scope */
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

    /* Create a new domain "icc1" */
    Tase2_Domain icc1 = Tase2_DataModel_addDomain(dataModel, "icc1");

    /* Add information buffer objects for IM */
    Tase2_Domain_addInformationMessageObject(icc1, 64);
    Tase2_Domain_addInformationMessageObject(icc1, 1024);

    /* Add some data points and devices to domain "icc1" */
    icc1Discrete1 = Tase2_Domain_addIndicationPoint(icc1, "Discrete1", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    icc1Discrete2 = Tase2_Domain_addIndicationPoint(icc1, "Discrete2", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, true);
    icc1Discrete3 = Tase2_Domain_addIndicationPoint(icc1, "Discrete3", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_NO_QUALITY, TASE2_NO_TIMESTAMP, false, false);
    icc1DiscreteQTimetag4 = Tase2_Domain_addIndicationPoint(icc1, "DiscreteQTimeTag4", TASE2_IND_POINT_TYPE_DISCRETE, TASE2_QUALITY, TASE2_TIMESTAMP, true, false);

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
    Tase2_BilateralTable blt1 = Tase2_BilateralTable_create("BLT_MZA_001_V1", icc1, "1.1.1.999", 12);

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
    Tase2_BilateralTable blt2 = Tase2_BilateralTable_create("BLT_MZA_002_V1", icc2, "1.1.999.1", 12);

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
     * Setup TLS configuration
     ***************************************************************/

    bool useTLS = true;

    TLSConfiguration tlsConfig = TLSConfiguration_create();

    TLSConfiguration_setChainValidation(tlsConfig, true);
    TLSConfiguration_setAllowOnlyKnownCertificates(tlsConfig, false);

    if (TLSConfiguration_setOwnKeyFromFile(tlsConfig, "server-key.pem", NULL) == false) {
    	printf("Failed to load server private key file!\n");
    	useTLS = false;
    }

    if (TLSConfiguration_setOwnCertificateFromFile(tlsConfig, "server.cer") == false) {
    	printf("Failed to load server certificate file!\n");
    	useTLS = false;
    }

    if (TLSConfiguration_addCACertificateFromFile(tlsConfig, "root.cer") == false) {
    	printf("Failed to load CA certificate file!\n");
    	useTLS = false;
    }

    //TLSConfiguration_addAllowedCertificateFromFile(tlsConfig, "client1.cer");

    /****************************************************************
     * TASE.2 server setup
     ***************************************************************/

    Tase2_Server server;

    if (useTLS)
    	server = Tase2_Server_create(dataModel, tlsConfig);
    else
        server = Tase2_Server_create(dataModel, NULL);

    if (server) {

		Tase2_Server_addBilateralTable(server, blt1);
		Tase2_Server_addBilateralTable(server, blt2);

		//Tase2_Server_setIdentity(server, "MZA", "TASE.2", "1.2.0");

		Tase2_Server_setSupportedFeatures(server, TASE2_SUPPORTED_FB_1 | TASE2_SUPPORTED_FB_2 | TASE2_SUPPORTED_FB_4 | TASE2_SUPPORTED_FB_5);

		Tase2_Server_setWriteDataHandler(server, writeDataHandler, NULL);
		Tase2_Server_setSetTagHandler(server, setTagHandler, NULL);
		Tase2_Server_setOperateHandler(server, operateHandler, NULL);
		Tase2_Server_setSelectHandler(server, selectHandler, NULL);
		Tase2_Server_setClientConnectionHandler(server, clientConnectionHandler, NULL);

		Tase2_Endpoint_setConnectionAcceptHandler(Tase2_Server_getEndpoint(server),
		        connectionAcceptHandler, NULL);

		Tase2_Server_setTSCriticalNotConfirmedHandler(server, criticalReportTimeoutHandler, NULL);

		/****************************************************************
		 * TASE.2 server start
		 ***************************************************************/

		Tase2_Server_start(server);

		int32_t msgId = 1;

		if (Tase2_Server_isRunning(server)) {

			int32_t intVal = 0;

			uint64_t nextSendTime = Hal_getTimeInMs() + 1000;

			bool singleEvent = true;

			int cov = 0;

			bool stateVal = false;

			while (running) {
				Thread_sleep(100);

				/* update some data values */
				if (nextSendTime <= Hal_getTimeInMs()) {
					Tase2_IndicationPoint_setDiscrete(icc1Discrete1, intVal++);
					Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1Discrete1);

					if (singleEvent) {
						singleEvent = false;

						Tase2_IndicationPoint_setDiscrete(icc1Discrete2, intVal++);
						Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1Discrete2);

						Tase2_IndicationPoint_setDiscrete(icc1DiscreteQTimetag4, cov);
						Tase2_IndicationPoint_setQuality(icc1DiscreteQTimetag4, TASE2_DATA_FLAGS_NORMAL_VALUE | TASE2_DATA_FLAGS_TIME_STAMP_QUALITY);
						Tase2_IndicationPoint_setTimeStamp(icc1DiscreteQTimetag4, Hal_getTimeInMs());
						Tase2_IndicationPoint_setCOV(icc1DiscreteQTimetag4, cov++);
						Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1DiscreteQTimetag4);


						Tase2_DataFlags flags;

						if (stateVal)
						    flags = TASE2_DATA_STATE_HI;
						else
						    flags = TASE2_DATA_STATE_LO;

						Tase2_IndicationPoint_setState(icc1State1, flags);


						Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) icc1State1);

						Tase2_IndicationPoint_setStateTimeStamp(vccState4, stateVal, Hal_getTimeInMs());
						Tase2_Server_updateOnlineValue(server, (Tase2_DataPoint) vccState4);

						stateVal = !stateVal;
					}
					else
						singleEvent = true;

					nextSendTime = Hal_getTimeInMs() + 2000;

					/* send information messages */

					char* testMessage = "test message";
					Tase2_Server_sendInformationMessage(server, NULL, 1, 1, msgId, strlen(testMessage), (uint8_t*) testMessage);

					char* testMessage2 = "test message for ICC1";
					Tase2_Server_sendInformationMessage(server, icc1, 2, 2, msgId, strlen(testMessage2), (uint8_t*) testMessage2);

					msgId++;
				}
			}

			/****************************************************************
			 * TASE.2 server stop and cleanup
			 ***************************************************************/

			Tase2_Server_stop(server);
		}
		else {
			printf("ERROR: Failed to start server. Reason can be that the binary/user requires root permissions"
					" or another server using the same port is already running.\n");
		}

		/* Release all server resources. Will also release the bilateral tables that were added */
		Tase2_Server_destroy(server);
    }
    else {
    	printf("Failed to create TASE.2 server object!\n");
    }

    Tase2_DataModel_destroy(dataModel);

    TLSConfiguration_destroy(tlsConfig);
}
