/*
 * tase2_client_internal.h
 *
 * This file is part of libtase2
 *
 * Copyright 2014-2019 MZ Automation GmbH
 *
 * All rights reserved.
 *
 */

#ifndef SRC_TASE2_CLIENT_TASE2_CLIENT_INTERNAL_H_
#define SRC_TASE2_CLIENT_TASE2_CLIENT_INTERNAL_H_

#include "mms_client.h"
#include "tase2_client.h"

#include "tase2_common_internal.h"
#include "hal_thread.h"

struct sTase2_Client {

    MmsClient mmsClient;
    Tase2_Endpoint endpoint;

    /* true -> endpoint was created by client as has to be released */
    bool createdEndpoint;

    int tase2MajorVersion;
    int tase2MinorVersion;

    int supportedFeatures;

    MmsError lastErrorCode;

    Semaphore activeTransferSetsLock;
    LinkedList /* <Tase2_ClientDSTransferSet> */ activeTransferSets;

    Tase2_ClientDSTransferSet currentRptTransferSet;

    Tase2_DSTransferSetValueHandler dsTransferSetValueHandler;
    void* dsTransferSetValueHandlerParameter;

    Tase2_DSTransferSetReportHandler dsTransferSetReportHandler;
    void* dsTransferSetReportHandlerParameter;

    Tase2_ConnectionClosedHandler connectionClosedHandler;
    void* connectionClosedHandlerParameter;

    Tase2_Client_StateChangedHandler stateChangedHandler;
    void* stateChangedHandlerParameter;

    Tase2_InformationMessageHandler imHandler;
    void* imHandlerParameter;

    /* indicates if received report is information message */
    bool isInfoMessage;

    /* indicates if received report is DSTS report */
    bool isDSTSReport;

    /* parameters of last received information message */
    int32_t imInfoRef;
    int32_t imLocalRef;
    int32_t imMsgId;
    int32_t imSize;

    /* sequence number that is increased for each received report */
    uint32_t reportSeqNo;

    Tase2_ClientState state;
    Semaphore stateLock;
};

struct sTase2_ClientDataSet {
    char domainName[33];
    char dataSetName[33];

    bool isDeletable;

    int dataItems; /* number of data points */

    struct sTase2_PointName* dataPoints;

    struct sTase2_PointValue* pointValues;

    Semaphore pointValuesLock;
};

struct sTase2_ClientDSTransferSet {
    char domainName[33];
    char transferSetName[33];

    char dataSetDomain[33];
    char dataSetName[33];

    int32_t startTime;
    int32_t interval;

    int32_t tle;
    int32_t bufferTime;

    int32_t integrityCheck;

    int dsConditionsRequested;

    bool blockData;
    bool critical;
    bool rbe;

    bool allChangesReported; /* optional attribute */

    bool status; /* isEnabled */

    int16_t eventCodeRequested;

    Tase2_ClientDataSet dataSet;

    /* special values received by last report */
    uint8_t dsConditionsDetected;
    int16_t eventCodeDetected;
    int32_t transferSetTimestamp;

    /* local buffer of the server side values */
    MmsValue* tsValues;
};

#define DATA_SET_MAX_NAME_LENGTH 64 /* is 32 according to standard! */

TASE2_INTERNAL void
Tase2_Client_mmsConnectionHandler(Tase2_Client self, MmsEndpointConnection connection, MmsEndpointEvent event);

TASE2_INTERNAL IsoApplicationAddress
Tase2_Client_getRemoteApplicationAddress(Tase2_Client self);

#endif /* SRC_TASE2_CLIENT_TASE2_CLIENT_INTERNAL_H_ */
