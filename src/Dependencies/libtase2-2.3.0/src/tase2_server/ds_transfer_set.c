/*
 *  transfer_set_handling.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2021 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "tase2_server_internal.h"
#include "tase2_common_internal.h"
#include "ds_transfer_set.h"

#include "lib_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#define TRANSFER_SET_NAME 1
#define DS_CONDITIONS_DETECTED 2
#define EVENT_CODE_DETECTED 4
#define TRANSFER_SET_TIMESTAMP 8

typedef struct sCriticalReport* CriticalReport;

struct sCriticalReport {
    uint64_t reportSent;
};

typedef struct sBufferedEvent* BufferedEvent;

struct sBufferedEvent
{
    Tase2_DataPoint dataPoint;
    MmsValue* value;
};

typedef struct sActiveDSTSInfoForDataPoint* ActiveDSTSInfoForDataPoint;

struct sActiveDSTSInfoForDataPoint {
    Tase2_DSTransferSet transferSet; /* DS Transfer set that is referencing the data point */
    int dataSetIdx; /* index of the data point in the DSTS data set */
};

typedef struct sActiveDSTransferSetInfo* ActiveDSTransferSetInfo;

struct sActiveDSTransferSetInfo {
    uint64_t lastSent;
    uint64_t lastSentIntegrityReport;
    uint64_t nextSendTime;
    int period;
    int integrityPeriod;
    bool isCritical;

    int bufferTime;
    uint64_t bufferStarted;
    MmsValue* bufferedDataPoints;

    uint8_t includedSpecialObjects;
    MmsValue* tsNameValue;
    MmsValue* dsConditionsValue;
    MmsValue* eventCodeDetectedValue;
    MmsValue* tsTimestampValue;

    LinkedList outstandingCriticalReports;
    Semaphore criticalReportsMutex;

    /* buffered events for RBE with AllChangesReported */
    LinkedList /* <BufferedEvent> */ bufferedEvents;
    int eventQueueSize;

    char* dataSetName;
    char* dataSetDomainName;
};

static void
BufferedEvent_destroy(BufferedEvent self)
{
    MmsValue_delete(self->value);
    GLOBAL_FREEMEM(self);
}

bool
tase2_DSTransferSet_isEnabled(Tase2_DSTransferSet ts)
{
    if (ts->edition == TASE2_EDITION_1996_08) {
        return MmsValue_getBoolean(MmsValue_getElement(ts->base.mmsValue, 10));
    }
    else {
        return MmsValue_getBoolean(MmsValue_getElement(ts->base.mmsValue, 11));
    }

}

static bool
isRBEEnabled(Tase2_DSTransferSet ts)
{
    return MmsValue_getBoolean(MmsValue_getElement(ts->base.mmsValue, 9));
}

static bool
isAllChangesReportedEnabled(Tase2_DSTransferSet ts)
{
    if (ts->edition == TASE2_EDITION_1996_08) {
        return false;
    }
    else {
        return MmsValue_getBoolean(MmsValue_getElement(ts->base.mmsValue, 10));
    }
}

static void
removeTransferSetReferenceFromDataPoint(Tase2_DSTransferSet transferSet, LinkedList dataPoints)
{
    ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo)(transferSet->activeContextInfo);

    LinkedList dataPointElement = LinkedList_getNext(dataPoints);

    while (dataPointElement) {
        void* data =  LinkedList_getData(dataPointElement);

        if (!((data == info->tsNameValue) ||
            (data == info->tsTimestampValue) ||
            (data == info->dsConditionsValue) ||
            (data == info->eventCodeDetectedValue)))
        {
            Tase2_DataPoint dataPoint = (Tase2_DataPoint) data;

            if (dataPoint->activeTransferSets) {
                ActiveDSTSInfoForDataPoint dpInfoToRemove = NULL;

                LinkedList activeTSElem = LinkedList_getNext(dataPoint->activeTransferSets);

                while (activeTSElem) {
                    ActiveDSTSInfoForDataPoint dpInfo = (ActiveDSTSInfoForDataPoint)LinkedList_getData(activeTSElem);

                    if (dpInfo->transferSet == transferSet) {
                        dpInfoToRemove = dpInfo;
                        break;
                    }

                    activeTSElem = LinkedList_getNext(activeTSElem);
                }

                if (dpInfoToRemove) {
                    LinkedList_remove(dataPoint->activeTransferSets, dpInfoToRemove);

                    GLOBAL_FREEMEM(dpInfoToRemove);
                }
            }
        }

        dataPointElement = LinkedList_getNext(dataPointElement);
    }
}

static inline bool
isIntervalTimeoutEnabled(MmsValue* dsConditions)
{
    return (MmsValue_getBitStringBit(dsConditions, 0));
}

static inline bool
isObjectChangedEnabled(MmsValue* dsConditions)
{
    return (MmsValue_getBitStringBit(dsConditions, 2));
}

static inline bool
isIntegrityTimeoutEnabled(MmsValue* dsConditions)
{
    return (MmsValue_getBitStringBit(dsConditions, 1));
}

static MmsNamedVariableList
checkIfDataSetExists(Tase2_Server self, MmsValue* dataSetName)
{
    /* Check data set */
    uint32_t domainScope = MmsValue_toUint32(MmsValue_getElement(dataSetName, 0));

    const char* dsDomainName = MmsValue_toString(MmsValue_getElement(dataSetName, 1));
    const char* dsName = MmsValue_toString(MmsValue_getElement(dataSetName, 2));

    MmsNamedVariableList variableList;

    if (domainScope == 1) /* domain scope */
    {
        MmsDomain* domain = MmsDevice_getDomain(self->device, dsDomainName);

        if (domain == NULL)
          return NULL;

        variableList = MmsDomain_getNamedVariableList(domain, dsName);
    }
    else { /* VMD scope */
      variableList = MmsDevice_getNamedVariableListWithName(self->device, dsName);
    }

    return variableList;
}

static bool
checkDataSetACL(Tase2_Server self, MmsNamedVariableList dataSet, Tase2_BilateralTable blt, Tase2_Endpoint_Connection peer)
{
    LinkedList variableList = MmsNamedVariableList_getVariableList(dataSet);

    LinkedList variableEntry = LinkedList_getNext(variableList);

    bool accessOk = true;

    while (variableEntry) {
        MmsNamedVariableListEntry entry = (MmsNamedVariableListEntry) LinkedList_getData(variableEntry);

        if (entry->arrayIndex != -1) {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Array access not allowed in DS transfer set");
            accessOk = false;
            break;
        }

        if (entry->componentName != NULL) {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Component access not allowed in DS transfer set");
            accessOk = false;
            break;
        }

        bool entryFound = false;

        if (entry->domain != NULL) {
            if (!strcmp(entry->domain->domainName, blt->domain->name)) {
                entryFound = true;
            }
            else {
                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Entry is in wrong domain");
                accessOk = false;
                break;
            }
        }

        if (entryFound == false) {
            if (!strcmp(entry->variableName, "Transfer_Set_Name"))
                entryFound = true;
            else if (!strcmp(entry->variableName, "DSConditions_Detected"))
                entryFound = true;
            else if (!strcmp(entry->variableName, "Event_Code_Detected"))
                entryFound = true;
            else if (!strcmp(entry->variableName, "Transfer_Set_Time_Stamp"))
                entryFound = true;
        }

        if (entryFound == false) {

            if (entry->domain == NULL) {
                /* get data point from VCC domain */
                Tase2_DataPoint dp = (Tase2_DataPoint) HashTree_find(self->dataModel->vcc->dataPointHashTree, entry->variableName);

                if (dp) {
                    if (BilateralTable_checkDataPointAccess(blt, NULL, dp, BLT_ACCESS_ALLOW_READ))
                        entryFound = true;
                }
                else {
                    /* get control point from VCC domain */
                    Tase2_BLT_Entry dpEntry = (Tase2_BLT_Entry) AvlTree_find(blt->controlPoints, entry->variableName);

                    if (dpEntry) {
                        tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Control point %s not allowed in DS for DS transfer set", entry->variableName);
                        accessOk = false;
                    }
                    else {
                        if (StringUtils_endsWith(entry->variableName, "_TAG")) {
                            /* it seems to be a TAG */
                            int variableNameLen = strlen(entry->variableName);

                            if (variableNameLen > 4 && variableNameLen < 130) {
                                char varName[130];
                                memcpy(varName, entry->variableName, variableNameLen - 4);
                                varName[variableNameLen - 4] = 0;

                                /* Get VCC data point by name */
                                dp = (Tase2_DataPoint) HashTree_find(self->dataModel->vcc->dataPointHashTree, varName);

                                if (dp) {
                                    /* Check if data point is in BLT as control point */
                                    dpEntry = (Tase2_BLT_Entry) AvlTree_find(blt->controlPoints, dp);

                                    if (dpEntry) {
                                        entryFound = true;
                                    }
                                }
                                else {
                                    tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "VCC datapoint %s not existing", varName);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (entryFound == false) {
            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Entry %s not found in ACL", entry->variableName);
            accessOk = false;
            break;
        }

        variableEntry = LinkedList_getNext(variableEntry);
    }

    return accessOk;
}

static CriticalReport
CriticalReport_create(uint64_t timestamp)
{
    CriticalReport self = (CriticalReport) GLOBAL_MALLOC(sizeof(struct sCriticalReport));

    if (self) {
        self->reportSent = timestamp;
    }

    return self;
}

static void
tase2_DSTransferSet_destroyContextInfo(Tase2_DSTransferSet self)
{
    if (self->activeContextInfo != NULL) {

        ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) self->activeContextInfo;

        if (info->bufferedDataPoints != NULL)
            MmsValue_delete(info->bufferedDataPoints);

        if (info->dsConditionsValue)
            MmsValue_delete(info->dsConditionsValue);

        if (info->eventCodeDetectedValue)
            MmsValue_delete(info->eventCodeDetectedValue);

        if (info->tsNameValue)
            MmsValue_delete(info->tsNameValue);

        if (info->tsTimestampValue)
            MmsValue_delete(info->tsTimestampValue);

        if (info->bufferedEvents)
            LinkedList_destroyDeep(info->bufferedEvents, (LinkedListValueDeleteFunction) BufferedEvent_destroy);

        Semaphore_destroy(info->criticalReportsMutex);
        LinkedList_destroy(info->outstandingCriticalReports);

        GLOBAL_FREEMEM(info);

        self->activeContextInfo = NULL;
    }
}


static bool
enableDSTransferSet(Tase2_Server self, MmsDomain* domain, Tase2_DSTransferSet ts, MmsEndpointConnection connection, MmsValue* value, Tase2_BilateralTable blt)
{
    Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

    MmsNamedVariableList variableList = checkIfDataSetExists(self, MmsValue_getElement(value, 0));

    if (variableList == NULL) {
        char mmsValueStr[256];

        MmsValue_printToBuffer(value, mmsValueStr, sizeof(mmsValueStr));

        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Data set %s not found", mmsValueStr);

        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

        return false;
    }

    if (blt) {
        if (checkDataSetACL(self, variableList, blt, peer) == false) {
            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Access to transfer set denied");

            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

            return false;
        }
    }

    ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) GLOBAL_MALLOC(sizeof(struct sActiveDSTransferSetInfo));

    ts->activeContextInfo = (void*) info;

    info->includedSpecialObjects = 0;
    info->lastSent = 0;
    info->lastSentIntegrityReport = Hal_getTimeInMs();
    info->bufferedDataPoints = NULL;
    info->bufferStarted = 0;
    info->tsNameValue = NULL;
    info->dsConditionsValue = NULL;
    info->eventCodeDetectedValue = NULL;
    info->tsTimestampValue = NULL;
    info->outstandingCriticalReports = LinkedList_create();
    info->criticalReportsMutex = Semaphore_create(1);
    info->bufferedEvents = NULL;
    info->eventQueueSize = 0;
    info->dataSetName = variableList->name;

    if (variableList->domain)
        info->dataSetDomainName = variableList->domain->domainName;
    else
        info->dataSetDomainName = NULL;

    MmsValue* dsConditionsRequested = MmsValue_getElement(value, 6);

    if (isIntervalTimeoutEnabled(dsConditionsRequested)) {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet interval timeout enabled");

        int32_t nextSendTime = MmsValue_toInt32(MmsValue_getElement(value, 1));

        info->nextSendTime = ((uint64_t) nextSendTime) * ((uint64_t)1000);
        info->period = MmsValue_toInt32(MmsValue_getElement(value, 2)) * 1000;

        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet interval start time = %i", nextSendTime);
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet interval = %i ms", info->period);
    }

    MmsValue* criticalFlag = MmsValue_getElement(value, 8);

    info->isCritical = MmsValue_getBoolean(criticalFlag);

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet critical = %i", info->isCritical);

    info->integrityPeriod = MmsValue_toInt32(MmsValue_getElement(value, 5)) * 1000;

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet integrity period = %i", info->integrityPeriod);

    info->bufferTime = MmsValue_toInt32(MmsValue_getElement(value, 4)) * 1000;

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet buffer time = %i", info->bufferTime);

    /* create list of variables */

    if (ts->dataPoints != NULL)
        LinkedList_destroyStatic(ts->dataPoints);

    ts->dataPoints = LinkedList_create();

    LinkedList dsDataPoints = LinkedList_getNext(variableList->listOfVariables);

    int dpCount = 0;

    while (dsDataPoints) {
        MmsNamedVariableListEntry listEntry = (MmsNamedVariableListEntry) LinkedList_getData(dsDataPoints);

        Tase2_Domain tase2Domain;

        if (listEntry->domain == NULL)
            tase2Domain = self->dataModel->vcc;
        else
            tase2Domain = Tase2_DataModel_getDomain(self->dataModel, MmsDomain_getName(listEntry->domain));

        if (tase2Domain == NULL) {
            removeTransferSetReferenceFromDataPoint(ts, ts->dataPoints);

            tase2_DSTransferSet_destroyContextInfo(ts);

            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

            return false;
        }

        Tase2_DataPoint dataPoint = Tase2_Domain_getDataPoint(tase2Domain, listEntry->variableName);

        if (dataPoint == NULL) {

            if (!strcmp(listEntry->variableName, "Transfer_Set_Name")) {
                info->includedSpecialObjects += TRANSFER_SET_NAME;
                info->tsNameValue = tase2_createObjectNameValue(domain->domainName, ts->base.name);
                LinkedList_add(ts->dataPoints, info->tsNameValue);
            }
            else if (!strcmp(listEntry->variableName, "DSConditions_Detected")) {
                info->includedSpecialObjects += DS_CONDITIONS_DETECTED;
                info->dsConditionsValue = tase2_createDSConditionsValue(false, false, false, false, false);
                LinkedList_add(ts->dataPoints, info->dsConditionsValue);
            }
            else if (!strcmp(listEntry->variableName, "Event_Code_Detected")) {
                info->includedSpecialObjects += EVENT_CODE_DETECTED;
                info->eventCodeDetectedValue = MmsValue_newInteger(16);
                LinkedList_add(ts->dataPoints, info->eventCodeDetectedValue);
            }
            else if (!strcmp(listEntry->variableName, "Transfer_Set_Time_Stamp")) {
                info->includedSpecialObjects += TRANSFER_SET_TIMESTAMP;
                info->tsTimestampValue = MmsValue_newInteger(32);
                LinkedList_add(ts->dataPoints, info->tsTimestampValue);
            }
            else {
                /* Check if it is a TAG value */
                if (StringUtils_endsWith(listEntry->variableName, "_TAG")) {
                    char dpName[65];
                    int dpNameLength = strlen(listEntry->variableName);

                    /* remove four last characters */
                    strcpy(dpName, listEntry->variableName);
                    dpName[dpNameLength - 4] = 0;

                    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "dp for TAG value: %s", dpName);

                    dataPoint = Tase2_Domain_getDataPoint(tase2Domain, dpName);

                    if (dataPoint == NULL)
                        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "control point %s not found", dpName);
                }
                else
                    tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "datapoint %s not found", listEntry->variableName);
            }

        }
        else {
            if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {

                tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "control point %s not allowed in transfer set", listEntry->variableName);

                dataPoint = NULL;
            }
        }

        if (dataPoint != NULL) {
            LinkedList_add(ts->dataPoints, (void*) dataPoint);

            ActiveDSTSInfoForDataPoint dpInfo = (ActiveDSTSInfoForDataPoint) GLOBAL_CALLOC(1, sizeof(struct sActiveDSTSInfoForDataPoint));

            if (dpInfo) {
                dpInfo->dataSetIdx = dpCount;
                dpInfo->transferSet = ts;

                if (dataPoint->activeTransferSets == NULL)
                    dataPoint->activeTransferSets = LinkedList_create();

                if (dataPoint->activeTransferSets)
                    LinkedList_add(dataPoint->activeTransferSets, dpInfo);
                else
                    GLOBAL_FREEMEM(dpInfo);
            }
        }

        dsDataPoints = LinkedList_getNext(dsDataPoints);

        dpCount++;
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet data point count = %i", dpCount);

    info->bufferedDataPoints = MmsValue_newBitString(dpCount);

    ts->base.connectionObject = (void*) connection;

    tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "START DSTransferSet %s", ts->base.name);

    Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

    if (LinkedList_contains(self->activeTransferSets, (void*) ts) == false)
        LinkedList_add(self->activeTransferSets, (void*) ts);

    if (self->dstsStateChangedHandler) {
        self->dstsStateChangedHandler(self->dstsStateChangedHandlerParameter, ts, true);
    }

    return true;
}

MmsDataAccessError
tase2_Server_handleDSTransferSetWrite(Tase2_Server self, MmsDomain* domain, Tase2_DSTransferSet ts, const char* componentName,
        MmsValue* value, MmsEndpointConnection connection, Tase2_BilateralTable blt, Tase2_Endpoint_Connection peer)
{
    if (componentName != NULL) {

        if (!strcmp(componentName, "Status")) {

            if (MmsValue_getBoolean(value)) { /* ENABLE */

                if (tase2_DSTransferSet_isEnabled(ts)) {
                    tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet %s already enabled!", ts->base.name);
                    return DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
                }

                Semaphore_wait(self->activeTransferSetsLock);

                bool enableSuccess = enableDSTransferSet(self, domain, ts, connection, ts->base.mmsValue, blt);

                Semaphore_post(self->activeTransferSetsLock);

                if (enableSuccess == false)
                    return DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;

                if (self->dstsUpdateHandler) {
                    self->dstsUpdateHandler(self->dstsUpdateHandlerParameter, peer, blt, ts, true);
                }
            }
            else { /* DISABLE */

                if (tase2_DSTransferSet_isEnabled(ts)) {
                    tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "STOP DSTransferSet");

                    Semaphore_wait(self->activeTransferSetsLock);

                    tase2_Server_deactivateDSTransferSet(self, ts, peer);
                    LinkedList_remove(self->activeTransferSets, (void*) ts);

                    Semaphore_post(self->activeTransferSetsLock);

                    if (self->dstsUpdateHandler) {
                        self->dstsUpdateHandler(self->dstsUpdateHandlerParameter, peer, blt, ts, false);
                    }
                }
            }

            MmsValue* status;

            if (ts->edition == TASE2_EDITION_1996_08) {
                status = MmsValue_getElement(ts->base.mmsValue, 10);
            }
            else {
                status = MmsValue_getElement(ts->base.mmsValue, 11);
            }

            MmsValue_update(status, value);
        }
        else {
            if (tase2_DSTransferSet_isEnabled(ts)) {
                tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet %s already enabled!", ts->base.name);
                return DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
            }

            if (!strcmp(componentName, "DataSetName")) {

                MmsNamedVariableList variableList = checkIfDataSetExists(self, value);

                if (variableList == NULL)
                  return DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;

                MmsValue* dataSetName = MmsValue_getElement(ts->base.mmsValue, 0);

                MmsValue_update(dataSetName, value);
            }
            else if (!strcmp(componentName, "StartTime")) {
                MmsValue* startTime = MmsValue_getElement(ts->base.mmsValue, 1);
                MmsValue_update(startTime, value);
            }
            else if (!strcmp(componentName, "Interval")) {
                MmsValue* interval = MmsValue_getElement(ts->base.mmsValue, 2);
                MmsValue_update(interval, value);
            }
            else if (!strcmp(componentName, "TLE")) {
                MmsValue* tle = MmsValue_getElement(ts->base.mmsValue, 3);
                MmsValue_update(tle, value);
            }
            else if (!strcmp(componentName, "BufferTime")) {
                MmsValue* bufferTime = MmsValue_getElement(ts->base.mmsValue, 4);
                MmsValue_update(bufferTime, value);
            }
            else if (!strcmp(componentName, "IntegrityCheck")) {
                MmsValue* integrityCheck = MmsValue_getElement(ts->base.mmsValue, 5);
                MmsValue_update(integrityCheck, value);
            }
            else if (!strcmp(componentName, "DSConditionsRequested")) {
                MmsValue* dsConditions = MmsValue_getElement(ts->base.mmsValue, 6);
                MmsValue_update(dsConditions, value);
            }
            else if (!strcmp(componentName, "BlockData")) {
                MmsValue* blockData = MmsValue_getElement(ts->base.mmsValue, 7);
                MmsValue_update(blockData, value);
            }
            else if (!strcmp(componentName, "Critical")) {
                MmsValue* critical = MmsValue_getElement(ts->base.mmsValue, 8);
                MmsValue_update(critical, value);
            }
            else if (!strcmp(componentName, "RBE")) {
                MmsValue* rbe = MmsValue_getElement(ts->base.mmsValue, 9);
                MmsValue_update(rbe, value);
            }
            else if (!strcmp(componentName, "AllChangesReported")) {
                if (ts->edition != TASE2_EDITION_1996_08) {
                    MmsValue* rbe = MmsValue_getElement(ts->base.mmsValue, 10);
                    MmsValue_update(rbe, value);
                }
            }
            else if (!strcmp(componentName, "EventCodeRequested")) {
                if (ts->edition != TASE2_EDITION_1996_08) {
                    MmsValue* eventCode = MmsValue_getElement(ts->base.mmsValue, 11);
                    MmsValue_update(eventCode, value);
                }
                else {
                    MmsValue* eventCode = MmsValue_getElement(ts->base.mmsValue, 12);
                    MmsValue_update(eventCode, value);
                }
            }
            else {
                tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet %s access to unknown attribute %s!", ts->base.name, componentName);
                return DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
            }
        }

        return DATA_ACCESS_ERROR_SUCCESS;
    }
    else {

        if (MmsValue_equalTypes(ts->base.mmsValue, value) == false) {
            tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet %s: write update failed!", ts->base.name);
            tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "   received struct size = %i (expected = %i)", MmsValue_getArraySize(value), MmsValue_getArraySize(ts->base.mmsValue));

            return DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
        }

        bool isStart = MmsValue_getBoolean(MmsValue_getElement(value, 11));

        if (isStart) {

            if (tase2_DSTransferSet_isEnabled(ts)) {
                tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet %s already enabled!", ts->base.name);
                return DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
            }

            Semaphore_wait(self->activeTransferSetsLock);

            bool enableSuccess = enableDSTransferSet(self, domain, ts, connection, value, blt);

            if (enableSuccess) {
                if (MmsValue_update(ts->base.mmsValue, value) == false) {
                    tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet %s: value update failed! (type error in activation)", ts->base.name);
                }
            }

            Semaphore_post(self->activeTransferSetsLock);

            if (enableSuccess == false)
                return DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;

            if (self->dstsUpdateHandler) {
                self->dstsUpdateHandler(self->dstsUpdateHandlerParameter, peer, blt, ts, true);
            }
        }
        else {

            if (tase2_DSTransferSet_isEnabled(ts) != false) {
                tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "STOP DSTransferSet");

                tase2_Server_deactivateDSTransferSet(self, ts, peer);

                if (MmsValue_update(ts->base.mmsValue, value) == false) {
                    tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "DSTransferSet %s: value update failed! (type error in deactivation)", ts->base.name);
                }

                Semaphore_wait(self->activeTransferSetsLock);
                LinkedList_remove(self->activeTransferSets, (void*) ts);
                Semaphore_post(self->activeTransferSetsLock);

                if (self->dstsUpdateHandler) {
                    self->dstsUpdateHandler(self->dstsUpdateHandlerParameter, peer, blt, ts, tase2_DSTransferSet_isEnabled(ts));
                }
            }

        }

        return DATA_ACCESS_ERROR_SUCCESS;
    }
}

static void
sendDSTransferSetAsDataSet(Tase2_Server self, Tase2_DSTransferSet ts, Tase2_ReportReason reason)
{
    MmsEndpointConnection connection = (MmsEndpointConnection) ts->base.connectionObject;

    LinkedList reportDataPoints = NULL;

    if (self->dstsReportSentHandler) {
        reportDataPoints = LinkedList_create();
    }

    MmsValue* dataSetName = MmsValue_getElement(ts->base.mmsValue, 0);

    /* Check data set */
    uint32_t domainScope = MmsValue_toUint32(MmsValue_getElement(dataSetName, 0));

    const char* dsDomainName = MmsValue_toString(MmsValue_getElement(dataSetName, 1));
    const char* dsName = MmsValue_toString(MmsValue_getElement(dataSetName, 2));

    LinkedList values = LinkedList_create();

    ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) ts->activeContextInfo;

    uint64_t currentTime = Hal_getTimeInMs();

    if (info->isCritical) {
        Semaphore_wait(info->criticalReportsMutex);

        LinkedList_add(info->outstandingCriticalReports, CriticalReport_create(currentTime));

        /* TODO store list of data points and a copy of the value */

        Semaphore_post(info->criticalReportsMutex);
    }

    LinkedList dataPointList = LinkedList_getNext(ts->dataPoints);

    while (dataPointList) {
        void* data = LinkedList_getData(dataPointList);

        if (data == info->tsNameValue)
            LinkedList_add(values, info->tsNameValue);
        else if (data == info->dsConditionsValue) {
            MmsValue_setBitStringFromInteger(info->dsConditionsValue, reason);
            LinkedList_add(values, info->dsConditionsValue);
        }
        else if (data == info->eventCodeDetectedValue)
            LinkedList_add(values, info->eventCodeDetectedValue);
        else if (data == info->tsTimestampValue) {
            MmsValue_setInt32(info->tsTimestampValue, (int32_t) (currentTime / 1000));
            LinkedList_add(values, info->tsTimestampValue);
        }
        else {
            Tase2_DataPoint dp = (Tase2_DataPoint) data;

            if (dp->pointType == TASE2_POINT_TYPE_CONTROL) {
                LinkedList_add(values, ((Tase2_ControlPoint)dp)->tagValue);
            }
            else
                LinkedList_add(values, dp->value.pointValue);

            if (reportDataPoints) {
                Tase2_SentPointValue sentValue = (Tase2_SentPointValue) GLOBAL_MALLOC(sizeof(struct sTase2_SentPointValue));

                if (sentValue) {
                    sentValue->dataPoint = dp;
                    sentValue->sentPointValue.type = dp->value.type;
                    sentValue->sentPointValue.pointValue = dp->value.pointValue;

                    LinkedList_add(reportDataPoints, sentValue);
                }
            }
        }

        dataPointList = LinkedList_getNext(dataPointList);
    }

    MmsEndpointConnection_lockConnection(connection);
    MmsEndpointConnection_sendInformationReportNamedVariableList(connection, (domainScope == 1) ? dsDomainName : NULL, dsName, values);
    MmsEndpointConnection_unlockConnection(connection);

    if (reportDataPoints) {
        Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

        Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

        self->dstsReportSentHandler(self->dstsReportSentHandlerParameter, peer, blt, ts, reportDataPoints, reason);

        LinkedList_destroy(reportDataPoints);
    }

    LinkedList_destroyStatic(values);
}

bool
Tase2_Server_requestReport(Tase2_Server self, Tase2_DSTransferSet transferSet, Tase2_ReportReason reason, int16_t eventCode)
{
    ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) transferSet->activeContextInfo;

    if (info) {

        MmsValue_setInt16(info->eventCodeDetectedValue, eventCode);

        sendDSTransferSetAsDataSet(self, transferSet, reason);

        return true;
    }
    else
        return false;
}

static void
sendDSTransferSetSingleDataPoint(Tase2_Server self, Tase2_DSTransferSet ts, Tase2_DataPoint dataPoint, Tase2_ReportReason reason)
{
    MmsEndpointConnection connection = (MmsEndpointConnection) ts->base.connectionObject;

    LinkedList reportDataPoints = NULL;

    if (self->dstsReportSentHandler) {
        reportDataPoints = LinkedList_create();
    }

    LinkedList varAccessSpecs = LinkedList_create();
    LinkedList values = LinkedList_create();

    ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) ts->activeContextInfo;

    uint64_t currentTime = Hal_getTimeInMs();

    if (info->isCritical) {
        Semaphore_wait(info->criticalReportsMutex);

        /* TODO store list of data points and a copy of the value */

        LinkedList_add(info->outstandingCriticalReports, CriticalReport_create(currentTime));
        Semaphore_post(info->criticalReportsMutex);
    }

    char* domainName = ts->base.domain->name;

    if (info->includedSpecialObjects & TRANSFER_SET_NAME) {
        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "Transfer_Set_Name");
        LinkedList_add(varAccessSpecs, varAccessSpec);
        LinkedList_add(values, info->tsNameValue);
    }

    if (info->includedSpecialObjects & DS_CONDITIONS_DETECTED) {
        MmsValue_setBitStringFromInteger(info->dsConditionsValue, TASE2_REPORT_REASON_OBJECT_CHANGE);
        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "DSConditions_Detected");
        LinkedList_add(varAccessSpecs, varAccessSpec);
        LinkedList_add(values, info->dsConditionsValue);
    }

    if (info->includedSpecialObjects & EVENT_CODE_DETECTED) {
        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "Event_Code_Detected");
        LinkedList_add(varAccessSpecs, varAccessSpec);
        LinkedList_add(values, info->eventCodeDetectedValue);
    }

    if (info->includedSpecialObjects & TRANSFER_SET_TIMESTAMP) {
        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "Transfer_Set_Time_Stamp");
        LinkedList_add(varAccessSpecs, varAccessSpec);
        MmsValue_setInt32(info->tsTimestampValue, (int32_t) (currentTime / 1000));
        LinkedList_add(values, info->tsTimestampValue);
    }

    char tagName[65];

    if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {
        LinkedList_add(values, ((Tase2_ControlPoint)dataPoint)->tagValue);

        strcpy(tagName, dataPoint->pointName);
        strcat(tagName, "_TAG");

        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(dataPoint->domain->name, tagName);
        LinkedList_add(varAccessSpecs, varAccessSpec);
    }
    else {
        LinkedList_add(values, dataPoint->value.pointValue);

        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(dataPoint->domain->name, dataPoint->pointName);
        LinkedList_add(varAccessSpecs, varAccessSpec);
    }

    if (reportDataPoints) {
        Tase2_SentPointValue sentValue = (Tase2_SentPointValue) GLOBAL_MALLOC(sizeof(struct sTase2_SentPointValue));

        if (sentValue) {
            sentValue->dataPoint = dataPoint;
            sentValue->sentPointValue.type = dataPoint->value.type;
            sentValue->sentPointValue.pointValue = dataPoint->value.pointValue;

            LinkedList_add(reportDataPoints, sentValue);
        }
    }

    MmsEndpointConnection_lockConnection(connection);
    MmsEndpointConnection_sendInformationReportListOfVariables(connection, varAccessSpecs, values);
    MmsEndpointConnection_unlockConnection(connection);

    LinkedList_destroy(varAccessSpecs);
    LinkedList_destroyStatic(values);

    if (reportDataPoints) {
        Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

        Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

        self->dstsReportSentHandler(self->dstsReportSentHandlerParameter, peer, blt, ts, reportDataPoints, reason);

        LinkedList_destroy(reportDataPoints);
    }
}

static void
sendDSTransferSetChangedDataPoints(Tase2_Server self, Tase2_DSTransferSet ts, Tase2_ReportReason reportReason)
{
    MmsEndpointConnection connection = (MmsEndpointConnection) ts->base.connectionObject;

    LinkedList reportDataPoints = NULL;

    if (self->dstsReportSentHandler) {
        reportDataPoints = LinkedList_create();
    }

    LinkedList varAccessSpecs = LinkedList_create();
    LinkedList values = LinkedList_create();

    ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) ts->activeContextInfo;

    uint64_t currentTime = Hal_getTimeInMs();

    if (info->isCritical) {

        Semaphore_wait(info->criticalReportsMutex);

        /* TODO store list of data points and a copy of the value */

        LinkedList_add(info->outstandingCriticalReports, CriticalReport_create(currentTime));

        Semaphore_post(info->criticalReportsMutex);
    }

    char* domainName = ts->base.domain->name;

    if (info->includedSpecialObjects & TRANSFER_SET_NAME) {
        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "Transfer_Set_Name");
        LinkedList_add(varAccessSpecs, varAccessSpec);
        LinkedList_add(values, info->tsNameValue);
    }

    if (info->includedSpecialObjects & DS_CONDITIONS_DETECTED) {
        MmsValue_setBitStringFromInteger(info->dsConditionsValue, reportReason);
        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "DSConditions_Detected");
        LinkedList_add(varAccessSpecs, varAccessSpec);
        LinkedList_add(values, info->dsConditionsValue);
    }

    if (info->includedSpecialObjects & EVENT_CODE_DETECTED) {
        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "Event_Code_Detected");
        LinkedList_add(varAccessSpecs, varAccessSpec);
        LinkedList_add(values, info->eventCodeDetectedValue);
    }

    if (info->includedSpecialObjects & TRANSFER_SET_TIMESTAMP) {
        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "Transfer_Set_Time_Stamp");
        LinkedList_add(varAccessSpecs, varAccessSpec);
        MmsValue_setInt32(info->tsTimestampValue, (int32_t) (currentTime / 1000));
        LinkedList_add(values, info->tsTimestampValue);
    }

    /* Add changed data points to list */

    if (isAllChangesReportedEnabled(ts)) {

        if (info->bufferedEvents) {
            LinkedList eventElement = LinkedList_getNext(info->bufferedEvents);

            while (eventElement) {

                BufferedEvent event = (BufferedEvent) LinkedList_getData(eventElement);

                Tase2_DataPoint dataPoint = event->dataPoint;

                if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {
                    LinkedList_add(values, event->value);

                    char tagName[65];
                    strcpy(tagName, dataPoint->pointName);
                    strcat(tagName, "_TAG");

                    MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(dataPoint->domain->name, tagName);
                    LinkedList_add(varAccessSpecs, varAccessSpec);
                }
                else {
                    LinkedList_add(values, event->value);
                    MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(dataPoint->domain->name, dataPoint->pointName);
                    LinkedList_add(varAccessSpecs, varAccessSpec);
                }

                if (reportDataPoints) {
                    Tase2_SentPointValue sentValue = (Tase2_SentPointValue) GLOBAL_MALLOC(sizeof(struct sTase2_SentPointValue));

                    if (sentValue) {
                        sentValue->dataPoint = dataPoint;
                        sentValue->sentPointValue.type = dataPoint->value.type;
                        sentValue->sentPointValue.pointValue = event->value;

                        LinkedList_add(reportDataPoints, sentValue);
                    }
                }

                eventElement = LinkedList_getNext(eventElement);
            }
        }
    }
    else {
        int dpIndex = 0;
        LinkedList dataPointList = LinkedList_getNext(ts->dataPoints);

        while (dataPointList) {

            if (MmsValue_getBitStringBit(info->bufferedDataPoints, dpIndex)) {

                Tase2_DataPoint dataPoint = (Tase2_DataPoint) LinkedList_getData(dataPointList);

                if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {
                    LinkedList_add(values, ((Tase2_ControlPoint)dataPoint)->tagValue);

                    char tagName[65];
                    strcpy(tagName, dataPoint->pointName);
                    strcat(tagName, "_TAG");

                    MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(dataPoint->domain->name, tagName);
                    LinkedList_add(varAccessSpecs, varAccessSpec);
                }
                else {
                    LinkedList_add(values, dataPoint->value.pointValue);
                    MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(dataPoint->domain->name, dataPoint->pointName);
                    LinkedList_add(varAccessSpecs, varAccessSpec);
                }

                if (reportDataPoints) {
                    Tase2_SentPointValue sentValue = (Tase2_SentPointValue) GLOBAL_MALLOC(sizeof(struct sTase2_SentPointValue));

                    if (sentValue) {
                        sentValue->dataPoint = dataPoint;
                        sentValue->sentPointValue.type = dataPoint->value.type;
                        sentValue->sentPointValue.pointValue = dataPoint->value.pointValue;

                        LinkedList_add(reportDataPoints, sentValue);
                    }
                }
            }

            dataPointList = LinkedList_getNext(dataPointList);
            dpIndex++;
        }
    }


    MmsEndpointConnection_lockConnection(connection);
    MmsEndpointConnection_sendInformationReportListOfVariables(connection, varAccessSpecs, values);
    MmsEndpointConnection_unlockConnection(connection);

    LinkedList_destroy(varAccessSpecs);
    LinkedList_destroyStatic(values);

    if (reportDataPoints) {
        Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

        Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

        self->dstsReportSentHandler(self->dstsReportSentHandlerParameter, peer, blt, ts, reportDataPoints, reportReason);

        LinkedList_destroy(reportDataPoints);
    }

    if (isAllChangesReportedEnabled(ts)) {
        if (info->bufferedEvents) {
            LinkedList_destroyDeep(info->bufferedEvents, (LinkedListValueDeleteFunction) BufferedEvent_destroy);
            info->bufferedEvents = NULL;
            info->eventQueueSize = 0;
        }
    }
}

static void
criticalReportTimeout(Tase2_Server self, Tase2_TransferSet ts, uint64_t sentTime)
{
    self->unconfirmedCriticalReportCounter++;

    if (self->criticalNotConfirmedHandler)
        self->criticalNotConfirmedHandler(self->criticalNotConfirmedHandlerParameter, ts, sentTime, false);
}

void
tase2_DSTransferSet_deleteAllCriticalReports(Tase2_DSTransferSet dsTs)
{
    if (tase2_DSTransferSet_isEnabled(dsTs)) {

        ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) dsTs->activeContextInfo;

        if (info->isCritical) {

            Semaphore_wait(info->criticalReportsMutex);

            LinkedList element = LinkedList_getNext(info->outstandingCriticalReports);

            while (element) {

                CriticalReport criticalReport = (CriticalReport) LinkedList_getData(element);

                element = LinkedList_getNext(element);

                LinkedList_remove(info->outstandingCriticalReports, (void*) criticalReport);

                GLOBAL_FREEMEM(criticalReport);
            }

            Semaphore_post(info->criticalReportsMutex);

        }

    }
}

static void
handleTransferSet(Tase2_Server self, Tase2_TransferSet ts)
{
    uint64_t now = Hal_getTimeInMs();

    if (ts->transferSetType == TASE2_TS_TYPE_DATA_SET) {

        Tase2_DSTransferSet dsTs = (Tase2_DSTransferSet) ts;

        if (tase2_DSTransferSet_isEnabled(dsTs)) {

            ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) dsTs->activeContextInfo;

            MmsValue* dsConditionsRequested = MmsValue_getElement(ts->mmsValue, 6);

            /* prevent freeze when time was set towards the past */
            if (info->lastSent > now)
                info->lastSent = now;

            if (info->isCritical) {

                Semaphore_wait(info->criticalReportsMutex);

                LinkedList element = LinkedList_getNext(info->outstandingCriticalReports);

                while (element) {

                    CriticalReport criticalReport = (CriticalReport) LinkedList_getData(element);

                    if ((now - criticalReport->reportSent) > self->criticalReportTimeout) {

                        criticalReportTimeout(self, ts, criticalReport->reportSent);

                        LinkedList_remove(info->outstandingCriticalReports, (void*) criticalReport);

                        GLOBAL_FREEMEM(criticalReport);

                        break;
                    }

                    element = LinkedList_getNext(element);
                }

                Semaphore_post(info->criticalReportsMutex);

            }

            if (isIntervalTimeoutEnabled(dsConditionsRequested)) {

                if (info->period > 0) {
                    if (info->nextSendTime <= now) {

                        if (isRBEEnabled(dsTs)) {
                            sendDSTransferSetChangedDataPoints(self, dsTs, TASE2_REPORT_REASON_INTERVAL_TIMEOUT);
                        }
                        else {
                            sendDSTransferSetAsDataSet(self, dsTs, TASE2_REPORT_REASON_INTERVAL_TIMEOUT);
                        }

                        info->lastSent = now;

                        /* calculate next send time */
                        uint64_t missedCycles = (now - info->nextSendTime) / info->period;

                        info->nextSendTime += ((missedCycles + 1) * info->period);

                    }
                }
            }

            if (isIntegrityTimeoutEnabled(dsConditionsRequested)) {

                if (isRBEEnabled(dsTs)) {
                    /* prevent freeze when time was set towards the past */
                    if (info->lastSentIntegrityReport > now)
                        info->lastSentIntegrityReport = now;

                    if (info->lastSentIntegrityReport + info->integrityPeriod <= now) {
                        sendDSTransferSetAsDataSet(self, dsTs, TASE2_REPORT_REASON_INTEGRITY_TIMEOUT);

                        info->lastSentIntegrityReport = now;
                    }
                }
            }

            if (isObjectChangedEnabled(dsConditionsRequested)) {

                if (info->bufferStarted != 0) {

                    if ((info->bufferStarted) + info->bufferTime <= now) {
                        /* bufferTime elapsed --> send report */

                        if (isRBEEnabled(dsTs))
                            sendDSTransferSetChangedDataPoints(self, dsTs, TASE2_REPORT_REASON_OBJECT_CHANGE);
                        else
                            sendDSTransferSetAsDataSet(self, dsTs, TASE2_REPORT_REASON_OBJECT_CHANGE);

                        info->bufferStarted = 0;

                        MmsValue_deleteAllBitStringBits(info->bufferedDataPoints);
                    }

                }
            }

        }

    }
}

static void
storeDataPointEventForTransferSet(Tase2_DSTransferSet ts, Tase2_Server server, Tase2_DataPoint dataPoint, int dpIndex)
{
    ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) ts->activeContextInfo;

    if (isAllChangesReportedEnabled(ts))
    {
        if (info->bufferedEvents == NULL)
            info->bufferedEvents = LinkedList_create();

        if (server->maxDSTSEventQueueSize != -1) {
            if (info->eventQueueSize == server->maxDSTSEventQueueSize) {

                tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, server->endpoint, NULL, "event limit reached for DSTS: delete oldest event");

                LinkedList firstElement = LinkedList_get(info->bufferedEvents, 0);

                if (firstElement) {
                    BufferedEvent event = (BufferedEvent) LinkedList_getData(firstElement);

                    BufferedEvent_destroy(event);

                    LinkedList_remove(info->bufferedEvents, (void*) event);

                    info->eventQueueSize--;
                }
            }
        }

        BufferedEvent event = (BufferedEvent) GLOBAL_MALLOC(sizeof(struct sBufferedEvent));

        if (event) {
            event->dataPoint = dataPoint;

            if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {
                event->value = MmsValue_clone(((Tase2_ControlPoint)dataPoint)->tagValue);
            }
            else {
                event->value = MmsValue_clone(dataPoint->value.pointValue);
            }

            LinkedList_add(info->bufferedEvents, event);
            info->eventQueueSize++;
        }
    }
    else {
        MmsValue_setBitStringBit(info->bufferedDataPoints, dpIndex, true);
    }
}

void
tase2_checkVariable(Tase2_Server server, Tase2_DataPoint dataPoint)
{
    /* Check if data point is referenced by one or more transfer sets */

    if (dataPoint->activeTransferSets) {
        LinkedList activeTsElem = LinkedList_getNext(dataPoint->activeTransferSets);

        while (activeTsElem) {
            ActiveDSTSInfoForDataPoint dpInfo = (ActiveDSTSInfoForDataPoint)LinkedList_getData(activeTsElem);

            Tase2_DSTransferSet ts = dpInfo->transferSet;

            ActiveDSTransferSetInfo info = (ActiveDSTransferSetInfo) ts->activeContextInfo;

            if (info) {
                MmsValue* dsConditions = MmsValue_getElement(ts->base.mmsValue, 6);

                bool intervalTimeoutEnabled = isIntervalTimeoutEnabled(dsConditions);
                bool objectChangedEnabled = isObjectChangedEnabled(dsConditions);

                if (intervalTimeoutEnabled || objectChangedEnabled) {

                    if (isRBEEnabled(ts)) {

                        if (objectChangedEnabled && (info->bufferTime > 0)) {

                            if (info->bufferStarted == 0)
                                info->bufferStarted = Hal_getTimeInMs();

                            storeDataPointEventForTransferSet(ts, server, dataPoint, dpInfo->dataSetIdx);
                        }
                        else if (intervalTimeoutEnabled) {
                            storeDataPointEventForTransferSet(ts, server, dataPoint, dpInfo->dataSetIdx);
                        }
                        else
                            sendDSTransferSetSingleDataPoint(server, ts, dataPoint, TASE2_REPORT_REASON_OBJECT_CHANGE);
                    }
                    else {

                        if (objectChangedEnabled && (info->bufferTime > 0)) {

                            if (info->bufferStarted == 0)
                                info->bufferStarted = Hal_getTimeInMs();

                            storeDataPointEventForTransferSet(ts, server, dataPoint, dpInfo->dataSetIdx);
                        }
                        else if (intervalTimeoutEnabled) {
                            storeDataPointEventForTransferSet(ts, server, dataPoint, dpInfo->dataSetIdx);
                        }
                        else
                            sendDSTransferSetAsDataSet(server, ts, TASE2_REPORT_REASON_OBJECT_CHANGE);
                    }

                }

            }

            activeTsElem = LinkedList_getNext(activeTsElem);
        }
    }

}

void
tase2_Server_checkTransferSets(Tase2_Server self)
{
    Semaphore_wait(self->activeTransferSetsLock);

    LinkedList transferSet = LinkedList_getNext(self->activeTransferSets);

    while (transferSet) {

        handleTransferSet(self, (Tase2_TransferSet) LinkedList_getData(transferSet));

        transferSet = LinkedList_getNext(transferSet);
    }

    Semaphore_post(self->activeTransferSetsLock);
}

void
tase2_Server_deactivateDSTransferSet(Tase2_Server self, Tase2_DSTransferSet transferSet, Tase2_Endpoint_Connection peer)
{
    if (tase2_DSTransferSet_isEnabled(transferSet)) {

        /* decrease tsCounter value of all referenced data points */
        if (transferSet->dataPoints)
            removeTransferSetReferenceFromDataPoint(transferSet, transferSet->dataPoints);

        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Deactivate transfer set %s", transferSet->base.name);

        /* set "Status" to disabled */
        if (transferSet->edition == TASE2_EDITION_1996_08) {
            MmsValue_setBoolean(MmsValue_getElement(transferSet->base.mmsValue, 10), false);
        }
        else {
            MmsValue_setBoolean(MmsValue_getElement(transferSet->base.mmsValue, 11), false);
        }

        if (self->dstsStateChangedHandler) {
            self->dstsStateChangedHandler(self->dstsStateChangedHandlerParameter, transferSet, false);
        }
    }

    tase2_DSTransferSet_destroyContextInfo(transferSet);
}

static bool
tase2_Server_isTransferSetActive(Tase2_Server self, Tase2_TransferSet ts)
{
    bool isActive = false;

    LinkedList activeTs = LinkedList_getNext(self->activeTransferSets);

    while (activeTs) {

        if (ts == (Tase2_TransferSet) LinkedList_getData(activeTs)) {
            isActive =  true;
            break;
        }

        activeTs = LinkedList_getNext(activeTs);
    }

    return isActive;
}

Tase2_DSTransferSet
tase2_Server_getNextDSTransferSet(Tase2_Server self, Tase2_Domain domain, MmsEndpointConnection connection)
{
    Tase2_TransferSet ts = domain->transferSets;

    while (ts != NULL) {

        if (ts->transferSetType == TASE2_TS_TYPE_DATA_SET) {

            Semaphore_wait(self->activeTransferSetsLock);

            if (tase2_Server_isTransferSetActive(self, ts) == false) {

                ts->connectionObject = (void*) connection;

                LinkedList_add(self->activeTransferSets, ts);

                Semaphore_post(self->activeTransferSetsLock);

                return (Tase2_DSTransferSet) ts;
            }

            Semaphore_post(self->activeTransferSetsLock);
        }

        ts = ts->sibling;
    }

    return NULL;
}

void
tase2_Server_deactivateTransferSets(Tase2_Server self, MmsEndpointConnection connection, Tase2_Endpoint_Connection peer)
{
    Semaphore_wait(self->activeTransferSetsLock);

    LinkedList transferSet = LinkedList_getNext(self->activeTransferSets);
    LinkedList lastElement = self->activeTransferSets;

    while (transferSet) {
        Tase2_TransferSet ts = (Tase2_TransferSet) LinkedList_getData(transferSet);

        if (ts->connectionObject == ((void*) connection)) {
            lastElement->next = transferSet->next;

            if (ts->transferSetType == TASE2_TS_TYPE_DATA_SET) {
                tase2_Server_deactivateDSTransferSet(self, (Tase2_DSTransferSet) ts, peer);

                if (self->dstsUpdateHandler) {
                    Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);
                    self->dstsUpdateHandler(self->dstsUpdateHandlerParameter, peer, blt, (Tase2_DSTransferSet)ts, false);
                }
            }

            GLOBAL_FREEMEM(transferSet);

            transferSet = lastElement->next;
        }
        else {
            lastElement = transferSet;
            transferSet = LinkedList_getNext(transferSet);
        }
    }

    Semaphore_post(self->activeTransferSetsLock);
}

void
tase2_Server_triggerTransferSets(Tase2_Server self, Tase2_DataPoint dataPoint)
{
    if (dataPoint->activeTransferSets) {

       Semaphore_wait(self->activeTransferSetsLock);

       tase2_checkVariable(self, dataPoint);

       Semaphore_post(self->activeTransferSetsLock);
   }
}

