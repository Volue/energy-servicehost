/*
 *  tase2_server_internal.h
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef SRC_TASE2_SERVER_TASE2_SERVER_INTERNAL_H_
#define SRC_TASE2_SERVER_TASE2_SERVER_INTERNAL_H_

#include "tase2_common_internal.h"
#include "tase2_endpoint_internal.h"
#include "mms_server.h"
#include "hal_thread.h"
#include "string_utilities.h"
#include "tase2_server.h"
#include "tase2_model.h"
#include "lib_memory.h"
#include "avl_tree.h"

#define BLT_ACCESS_ALLOW_READ 1
#define BLT_ACCESS_ALLOW_WRITE 2
#define BLT_ACCESS_ALLOW_SET_TAG 4
#define BLT_ACCESS_ALLOW_GET_TAG 8
#define BLT_ACCESS_ALLOW_OPERATE 16

#define DEFAULT_SELECT_TIMEOUT 10000 /* ms */
#define DEFAULT_CRITICAL_TIMEOUT 5000 /* ms */

struct sTase2_Server {
    MmsServer mmsServer;
    Tase2_DataModel dataModel;
    MmsDevice* device;
    MmsValue* supportedFeatures;

    Tase2_Endpoint endpoint;
    bool createdEndpoint;

    int selectTimeout; /* select timeout in ms */
    int criticalReportTimeout; /* timeout for confirmation of critical reports in ms */

    Tase2_SelectHandler selectHandler;
    void* selectHandlerParameter;

    Tase2_OperateHandler operateHandler;
    void* operateHandlerParameter;

    Tase2_OperateHandlerEx operateHandlerEx;

    Tase2_SetTagHandler setTagHandler;
    void* setTagHandlerParameter;

    Tase2_WriteDataHandler writeDataHandler;
    void* writeDataHandlerParameter;

    Tase2_TSCriticalNotConfirmedHandler criticalNotConfirmedHandler;
    void* criticalNotConfirmedHandlerParameter;

    Tase2_DSTransferSetStateChangedHandler dstsStateChangedHandler;
    void* dstsStateChangedHandlerParameter;

    Tase2_DataSetEventHandler dataSetChangedHandler;
    void* dataSetChangedHandlerParameter;

    Tase2_DSTransferSetUpdateHandler dstsUpdateHandler;
    void* dstsUpdateHandlerParameter;

    Tase2_IMTransferSetEventHandler imEventHandler;
    void* imEventHandlerParameter;

    Tase2_DSTransferSetReportSentHandler dstsReportSentHandler;
    void* dstsReportSentHandlerParameter;

    Tase2_ClientConnectionHandler clientConnectionHandler;
    void* clientConnectionHandlerParameter;

    /** list of active (enabled) transfer sets */
    LinkedList activeTransferSets;
    Semaphore activeTransferSetsLock;

    /** list of bilateral tables */
    LinkedList blts;

    /** list of connection with enabled IM transfer set */
    LinkedList imEnabledConnections;
    Semaphore imEnabledConnectionsLock;

    /** Counter for unconfirmed critical reports (for statistical purposes) */
    uint64_t unconfirmedCriticalReportCounter;

    int maxDSTSEventQueueSize;
};

struct sTase2_DataPoint {
    Tase2_DataPointType pointType;
    char* pointName;
    Tase2_DataPoint sibling;
    Tase2_Domain domain;

    Semaphore valueLock; /* protect value access */
    struct sTase2_PointValue value; /* local value */
    MmsValue* serverCachedValue; /* server cached value */

    LinkedList activeTransferSets;  /* list of active/enabled transfer sets that reference this data point */
};

struct sTase2_SentPointValue {
    Tase2_DataPoint dataPoint;
    struct sTase2_PointValue sentPointValue;
};

struct sTase2_IndicationPoint {
    struct sTase2_DataPoint base;

    Tase2_IndicationPointType pointType;
    Tase2_QualityClass qualityClass;
    Tase2_TimeStampClass timeStampClass;

    int hasCov:1; /* IndicationPoint has COV field */
    int readOnly:1; /* IndicationPoint is read-only */
    int hasDeadband:1; /* indication point has dead-band value */
    int hasChanged:1; /* indicate that status/value/quality changed - for RBE */
};

struct sTase2_ProtectionEquipment {
    struct sTase2_DataPoint base;

    bool isPacked;
};

struct sTase2_ControlPoint {
    struct sTase2_DataPoint base;

    Tase2_ControlPointType pointType;
    Tase2_DeviceClass deviceClass;
    bool hasTag; /* Indicate whether the device has a tag */
    int16_t checkBackId; /* check back ID of the device according to the bilateral table */

    uint64_t selectedTimestamp; /* time when selected - for timeout handling */
    void* selectedBy; /* handle to the entity that selected the device - only for SBO control */

    MmsValue* tagValue;
    Tase2_BilateralTable tagOwner;

    MmsValue* sboValue;
};



struct sTase2_TransferSet {
    Tase2_TransferSetType transferSetType;
    char* name;
    Tase2_Domain domain;
    Tase2_TransferSet sibling;
    MmsValue* mmsValue;

    /** connection (association) to send the reports (is void* to avoid dependency to MMS API */
    void* connectionObject;
};

struct sTase2_DSTransferSet {
    struct sTase2_TransferSet base;
    LinkedList dataPoints;
    void* activeContextInfo; /* private information - added internally when transfer set is enabled */
    Tase2_Edition edition; /* 1 = 1996-08, 2 = 2000-08 */
};


typedef struct sTase2_BLT_Entry* Tase2_BLT_Entry;

struct sTase2_BLT_Entry {
    Tase2_DataPoint dataPoint;
    uint8_t accessFlags;
    Tase2_BLT_Entry next;
    int16_t checkBackId;
};

typedef struct sTase2_BLT_InfoMessageEntry* Tase2_BLT_InfoMessageEntry;

struct sTase2_BLT_InfoMessageEntry {
    int32_t infoRef;
    int32_t localRef;
    bool vccScope; /* true --> VCC scope, otherwise ICC scope */

    Tase2_BLT_InfoMessageEntry next;
};

struct sTase2_BilateralTable {

    char* bltId;

    struct sIsoApplicationAddress appAddr;

    Tase2_Domain domain;

    AvlTree dataPoints; /* list of data points */

    AvlTree controlPoints; /* list of control points */

    Tase2_BLT_InfoMessageEntry informationMessages;
};

void
Tase2_BilateralTable_destroy(Tase2_BilateralTable self);

void
tase2_ControlPoint_setTagValue(Tase2_ControlPoint cp, Tase2_TagValue tagValue);

Tase2_TagValue
tase2_ControlPoint_getTagValue(Tase2_ControlPoint cp);

void
tase2_ControlPoint_setArmedFlag(Tase2_ControlPoint cp, bool value);

void
tase2_DSTransferSet_deleteAllCriticalReports(Tase2_DSTransferSet dsTs);

bool
BilateralTable_checkDataPointAccess(Tase2_BilateralTable self, Tase2_Domain domain, Tase2_DataPoint dataPoint, int checkFlags);

bool
BilateralTable_checkControlPointAccess(Tase2_BilateralTable self, Tase2_Domain domain, Tase2_ControlPoint dataPoint, int checkFlags);

bool
BilateralTable_checkInformationMessageAccess(Tase2_BilateralTable self, Tase2_Domain domain, int32_t infoRef, int32_t localRef);

int16_t
BilateralTable_getCheckBackId(Tase2_BilateralTable self, Tase2_Domain domain, Tase2_ControlPoint dataPoint);

TASE2_INTERNAL void
Tase2_Server_mmsConnectionHandler(Tase2_Server self, MmsEndpointConnection connection, Tase2_Endpoint_Connection peer, MmsEndpointEvent event);

TASE2_INTERNAL AcseServiceUserDiagnostics
Tase2_Server_authenticator(Tase2_Server self, AcseAuthenticationParameter authParameter, Tase2_BilateralTable* securityToken, IsoApplicationAddress remoteAppRef);

TASE2_INTERNAL void
Tase2_Server_connectWithEndpoint(Tase2_Server self, Tase2_Endpoint endpoint);


#endif /* SRC_TASE2_SERVER_TASE2_SERVER_INTERNAL_H_ */
