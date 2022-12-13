/*
 *  tase2_client.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "tase2_client_internal.h"
#include "tase2_endpoint_internal.h"

#include "lib_memory.h"
#include "string_utilities.h"

#include <stdio.h>

#include "tase2_runtime_check.h"

#ifndef TASE2_LIB_CHECK_RUNTIME
#define TASE2_LIB_CHECK_RUNTIME() {do {} while(0);}
#endif

static char*
strncpy_save(char* dst, const char* src, size_t n)
{
    char* ret = strncpy(dst, src, n);

    dst[n - 1] = 0;

    return ret;
}

static Tase2_PointValueType
determineTypeFromMmsValue(MmsValue* mmsPointValue)
{
    Tase2_PointValueType pointType = TASE2_VALUE_TYPE_UNKNOWN;

    MmsType baseMmsType = MmsValue_getType(mmsPointValue);

    if (baseMmsType == MMS_STRUCTURE) {

        int structSize = MmsValue_getArraySize(mmsPointValue);

        if (structSize == 4) {
            if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_BIT_STRING) {
                if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 3)) == MMS_STRUCTURE) {
                    pointType = TASE2_VALUE_TYPE_PACKED_EVENT;
                }
                else {
                    pointType = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
                }
            }
            else if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_INTEGER) {
                pointType = TASE2_VALUE_TYPE_DISCRETE;
            }
            else if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_FLOAT) {
                pointType = TASE2_VALUE_TYPE_REAL;
            }
        }
        else if (structSize == 3) {
            if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_BIT_STRING) {
                if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 2)) == MMS_STRUCTURE) {
                    pointType = TASE2_VALUE_TYPE_SINGLE_EVENT;
                }
                else {
                    pointType = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
                }
            }
            else if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_INTEGER) {
                if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 1)) == MMS_BIT_STRING)
                    pointType = TASE2_VALUE_TYPE_STATE;
                else
                    pointType = TASE2_VALUE_TYPE_DISCRETE;
            }
            else if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_FLOAT) {
                pointType = TASE2_VALUE_TYPE_REAL;
            }
        }
        else if (structSize == 2) {
            if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_BIT_STRING) {
                pointType = TASE2_VALUE_TYPE_STATE_OR_STATE_SUPPLEMENTAL;
            }
            else if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_INTEGER) {
                pointType = TASE2_VALUE_TYPE_STATE_OR_DISCRETE;
            }
            else if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_STRUCTURE) {
                pointType = TASE2_VALUE_TYPE_STATE;
            }
            else if (MmsValue_getType(MmsValue_getElement(mmsPointValue, 0)) == MMS_FLOAT) {
                pointType = TASE2_VALUE_TYPE_REAL;
            }
        }

    }
    else if (baseMmsType == MMS_BIT_STRING) {
        if (MmsValue_getBitStringSize(mmsPointValue) == 8)
            pointType = TASE2_VALUE_TYPE_STATE_OR_STATE_SUPPLEMENTAL;
    }
    else if (baseMmsType == MMS_FLOAT) {
        pointType = TASE2_VALUE_TYPE_REAL;
    }
    else if (baseMmsType == MMS_INTEGER) {
        pointType = TASE2_VALUE_TYPE_DISCRETE;
    }
    else if (baseMmsType == MMS_DATA_ACCESS_ERROR) {
        pointType = TASE2_VALUE_TYPE_ERROR;
    }

    return pointType;
}

static Tase2_ClientError
convertDataAccessErrorToClientError(MmsDataAccessError dataAccessError)
{

    switch (dataAccessError) {

    case DATA_ACCESS_ERROR_SUCCESS:
        return TASE2_CLIENT_ERROR_OK;

    case DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE:
        return TASE2_CLIENT_ERROR_TEMPORARILY_UNAVAILABLE;

    case DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED:
        return TASE2_CLIENT_ERROR_ACCESS_DENIED;

    case DATA_ACCESS_ERROR_INVALID_ADDRESS:
    case DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT:
    case DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID:
        return TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

    case DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT:
        return TASE2_CLIENT_ERROR_OBJECT_NOT_EXISTING;

    default:
        return TASE2_CLIENT_ERROR_OTHER;

    }
}

static Tase2_ClientError
convertMmsErrorToClientError(MmsError err)
{
    switch (err) {
    case MMS_ERROR_NONE:
        return TASE2_CLIENT_ERROR_OK;

    case MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED:
        return TASE2_CLIENT_ERROR_ACCESS_DENIED;

    case MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT:
        return TASE2_CLIENT_ERROR_OBJECT_NOT_EXISTING;

    case MMS_ERROR_ACCESS_TEMPORARILY_UNAVAILABLE:
        return TASE2_CLIENT_ERROR_TEMPORARILY_UNAVAILABLE;

    case MMS_ERROR_DEFINITION_OBJECT_ATTRIBUTE_INCONSISTENT:
    case MMS_ERROR_DEFINITION_TYPE_INCONSISTENT:
    case MMS_ERROR_DEFINITION_INVALID_ADDRESS:
    case MMS_ERROR_ACCESS_OBJECT_VALUE_INVALID:
        return TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

    case MMS_ERROR_DEFINITION_OBJECT_EXISTS:
        return TASE2_CLIENT_ERROR_OBJECT_EXISTS;

    default:
        return TASE2_CLIENT_ERROR_OTHER;
    }
}

static Tase2_ClientDSTransferSet
lookupDsTransferSetByDataSet(Tase2_Client self, const char* domainName, const char* dataSetName)
{
    Tase2_ClientDSTransferSet result = NULL;

    Semaphore_wait(self->activeTransferSetsLock);

    if (self->activeTransferSets) {

        LinkedList ts = LinkedList_getNext(self->activeTransferSets);

        while (ts) {
            Tase2_ClientDSTransferSet transferSet = (Tase2_ClientDSTransferSet) LinkedList_getData(ts);

            if ((!strcmp(transferSet->dataSetDomain, domainName)) && (!strcmp(transferSet->dataSetName, dataSetName))) {
                result = transferSet;
                break;
            }

            ts = LinkedList_getNext(ts);
        }

        if (result == NULL) {
            if (LinkedList_size(self->activeTransferSets) == 0) {
                tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "List of active transfer sets is empty");
            }
        }

    }
    else {
        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "No list of active transfer sets");
    }

    Semaphore_post(self->activeTransferSetsLock);

    return result;
}

static Tase2_ClientDSTransferSet
lookupDsTransferSetByName(Tase2_Client self, const char* domainName, const char* transferSettName)
{
    Tase2_ClientDSTransferSet result = NULL;

    Semaphore_wait(self->activeTransferSetsLock);

    if (self->activeTransferSets) {

        LinkedList ts = LinkedList_getNext(self->activeTransferSets);

        while (ts) {
            Tase2_ClientDSTransferSet transferSet = (Tase2_ClientDSTransferSet) LinkedList_getData(ts);

            if ((!strcmp(transferSet->domainName, domainName)) && (!strcmp(transferSet->transferSetName, transferSettName))) {
                result = transferSet;
                break;
            }

            ts = LinkedList_getNext(ts);
        }

        if (result == NULL) {
            if (LinkedList_size(self->activeTransferSets) == 0) {
                tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "List of active transfer sets is empty");
            }
        }
    }
    else {
        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "No list of active transfer sets");
    }

    Semaphore_post(self->activeTransferSetsLock);

    return result;
}

static bool
parseObjectReference(MmsValue* value, char* domainName, char* itemName)
{
    bool result = false;

    if ((MmsValue_getType(value) == MMS_STRUCTURE) && (MmsValue_getArraySize(value) == 3)) {

       MmsValue* dsDomain = MmsValue_getElement(value, 1);
       MmsValue* dsName = MmsValue_getElement(value, 2);

       if ((MmsValue_getType(dsDomain) == MMS_VISIBLE_STRING) && (MmsValue_getType(dsName) == MMS_VISIBLE_STRING)) {
           strncpy_save(domainName, MmsValue_toString(dsDomain), 33);
           strncpy_save(itemName, MmsValue_toString(dsName), 33);

           result = true;
       }
    }

    return result;
}

static void
updateDataSetValue(Tase2_ClientDataSet ds, const char* domainName, const char* variableName, MmsValue* value)
{
    int i;

    for (i = 0; i < ds->dataItems; i++) {
        if (domainName) {
            if (strcmp(domainName, ds->dataPoints[i].domainName) != 0)
                continue;
        }

        if (strcmp(variableName, ds->dataPoints[i].pointName) == 0) {

            if (ds->pointValues[i].pointValue == NULL) {
                ds->pointValues[i].pointValue = MmsValue_clone(value);
                ds->pointValues[i].type = determineTypeFromMmsValue(value);
            }
            else
                MmsValue_update(ds->pointValues[i].pointValue, value);

            break;
        }
    }
}

static void
sendTransferSetNACK(Tase2_Client self, const char* domainName, const char* itemName)
{
    LinkedList varAccessSpecs = LinkedList_create();
    LinkedList values = LinkedList_create();

    MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "Transfer_Report_NACK");
    LinkedList_add(varAccessSpecs, varAccessSpec);

    MmsValue* transferSet = tase2_createObjectNameValue(domainName, itemName);

    LinkedList_add(values, transferSet);

    MmsClient_sendInformationReportListOfVariables(self->mmsClient, varAccessSpecs, values);

    LinkedList_destroy(varAccessSpecs);
    LinkedList_destroyDeep(values, (LinkedListValueDeleteFunction) MmsValue_delete);
}

static void
sendTransferSetACK(Tase2_Client self, Tase2_ClientDSTransferSet transferSet)
{
    LinkedList varAccessSpecs = LinkedList_create();
    LinkedList values = LinkedList_create();

    MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(transferSet->domainName, "Transfer_Report_ACK");
    LinkedList_add(varAccessSpecs, varAccessSpec);

    varAccessSpec = MmsVariableAccessSpecification_create(transferSet->domainName, "Transfer_Set_Time_Stamp");
    LinkedList_add(varAccessSpecs, varAccessSpec);

    LinkedList_add(values, tase2_createObjectNameValue(transferSet->domainName, transferSet->transferSetName));
    LinkedList_add(values, MmsValue_newIntegerFromInt32(transferSet->transferSetTimestamp));

    MmsClient_sendInformationReportListOfVariables(self->mmsClient, varAccessSpecs, values);

    LinkedList_destroy(varAccessSpecs);
    LinkedList_destroyDeep(values, (LinkedListValueDeleteFunction) MmsValue_delete);
}

static bool
parseInfoBufHeader(Tase2_Client self, MmsValue* value)
{
    bool isValid = true;

    if (MmsValue_getType(value) == MMS_STRUCTURE) {
        if (MmsValue_getArraySize(value) == 4) {

            MmsValue* element = MmsValue_getElement(value, 0);

            if (MmsValue_getType(element) == MMS_INTEGER)
                self->imInfoRef = MmsValue_toInt32(element);
            else
                isValid = false;

            element = MmsValue_getElement(value, 1);

            if (MmsValue_getType(element) == MMS_INTEGER)
                self->imLocalRef = MmsValue_toInt32(element);
            else
                isValid = false;

            element = MmsValue_getElement(value, 2);

            if (MmsValue_getType(element) == MMS_INTEGER)
                self->imMsgId = MmsValue_toInt32(element);
            else
                isValid = false;

            element = MmsValue_getElement(value, 3);

            if (MmsValue_getType(element) == MMS_INTEGER)
                self->imSize = MmsValue_toInt32(element);
            else
                isValid = false;
        }
        else
            isValid = false;
    }
    else
        isValid = false;

    return isValid;
}

static void
Tase2_Client_setState(Tase2_Client self, Tase2_ClientState newState)
{
    Tase2_ClientState oldState;

    Semaphore_wait(self->stateLock);
    oldState = self->state;
    self->state = newState;
    Semaphore_post(self->stateLock);

    if (self->stateChangedHandler) {
        if (oldState != newState)
            self->stateChangedHandler(self->stateChangedHandlerParameter, self, newState);
    }
}

static void
mmsInformationReportHandler (MmsEndpointConnection connection, void* parameter, char* domainName,
        char* variableListName, MmsValue* value, uint8_t flags)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Client self = (Tase2_Client) parameter;

    Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

    Tase2_ClientDSTransferSet transferSet = NULL;

    if (flags & MMS_INFO_HANDLER_IS_VARIABLE_LIST_NAME) {

        self->reportSeqNo++;

        transferSet = lookupDsTransferSetByDataSet(self, domainName, variableListName);

        if (transferSet == NULL) {

            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "No corresponding transfer set found for report (%s:%s) -> ignore report", domainName, variableListName);

            /* we don't know the transfer set name so we use the data set name used for the information report */

            sendTransferSetNACK(self, domainName, variableListName);

            goto exit_function; /* we cannot find a corresponding transfer set --> ignore report */
        }

        Tase2_ClientDataSet dataSet = Tase2_ClientDSTransferSet_getDataSet(transferSet);

        if (dataSet == NULL) {

            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "No data set information available for report -> ignore report");

            sendTransferSetNACK(self, transferSet->domainName, transferSet->transferSetName);

            goto exit_function; /* no data set information available, so we cannot evaluate the report --> ignore report */
        }

        int arraySize = MmsValue_getArraySize(value);

        if (arraySize != dataSet->dataItems) {
            tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Report has incompatible data set size -> ignore report");

            goto exit_function;
        }

        Semaphore_wait(dataSet->pointValuesLock);

        /* TODO add support for blocked transfer (CB 3) */

        if (self->dsTransferSetReportHandler)
            self->dsTransferSetReportHandler(self->dsTransferSetReportHandlerParameter, false, self->reportSeqNo, transferSet);

        int i;

        for (i = 0; i < arraySize; i++) {

            if (dataSet->dataPoints[i].domainName[0] == 0) {
                if (!strcmp(dataSet->dataPoints[i].pointName, "Transfer_Set_Name")) {
                    /* ignore */
                }
                else if (!strcmp(dataSet->dataPoints[i].pointName, "DSConditions_Detected")) {
                    transferSet->dsConditionsDetected = (uint8_t) MmsValue_getBitStringAsInteger(MmsValue_getElement(value, i));
                }
                else if (!strcmp(dataSet->dataPoints[i].pointName, "Event_Code_Detected")) {
                    transferSet->eventCodeDetected = MmsValue_toInt32(MmsValue_getElement(value, i));
                }
                else if (!strcmp(dataSet->dataPoints[i].pointName, "Transfer_Set_Time_Stamp")) {
                    transferSet->transferSetTimestamp = MmsValue_toInt32(MmsValue_getElement(value, i));
                }
                else {

                    if (dataSet->pointValues[i].pointValue == NULL) {
                        dataSet->pointValues[i].pointValue = MmsValue_clone(MmsValue_getElement(value, i));
                        dataSet->pointValues[i].type = determineTypeFromMmsValue(MmsValue_getElement(value, i));
                    }
                    else
                        MmsValue_update(dataSet->pointValues[i].pointValue, MmsValue_getElement(value, i));

                    if (self->dsTransferSetValueHandler) {

                        self->dsTransferSetValueHandler(self->dsTransferSetValueHandlerParameter, transferSet,
                                NULL, dataSet->dataPoints[i].pointName, &(dataSet->pointValues[i]));
                    }
                }
            }
            else {

                if (!strcmp(dataSet->dataPoints[i].pointName, "Transfer_Set_Name")) {
                    /* ignore */
                }
                else if (!strcmp(dataSet->dataPoints[i].pointName, "DSConditions_Detected")) {
                    transferSet->dsConditionsDetected = (uint8_t) MmsValue_getBitStringAsInteger(MmsValue_getElement(value, i));
                }
                else if (!strcmp(dataSet->dataPoints[i].pointName, "Event_Code_Detected")) {
                    transferSet->eventCodeDetected = MmsValue_toInt32(MmsValue_getElement(value, i));
                }
                else if (!strcmp(dataSet->dataPoints[i].pointName, "Transfer_Set_Time_Stamp")) {
                    transferSet->transferSetTimestamp = MmsValue_toInt32(MmsValue_getElement(value, i));
                }
                else {

                    if (dataSet->pointValues[i].pointValue == NULL) {
                        dataSet->pointValues[i].pointValue = MmsValue_clone(MmsValue_getElement(value, i));
                        dataSet->pointValues[i].type = determineTypeFromMmsValue(MmsValue_getElement(value, i));
                    }
                    else
                        MmsValue_update(dataSet->pointValues[i].pointValue, MmsValue_getElement(value, i));

                    if (self->dsTransferSetValueHandler) {

                        self->dsTransferSetValueHandler(self->dsTransferSetValueHandlerParameter, transferSet,
                                dataSet->dataPoints[i].domainName, dataSet->dataPoints[i].pointName, &(dataSet->pointValues[i]));
                    }

                }
            }

        }

        if (self->dsTransferSetReportHandler)
            self->dsTransferSetReportHandler(self->dsTransferSetReportHandlerParameter, true, self->reportSeqNo, transferSet);

        /* if transfer set has critical flag set send ACK */
        if (transferSet->critical)
            sendTransferSetACK(self, transferSet);

        Semaphore_post(dataSet->pointValuesLock);
    }
    else {

        if (flags & MMS_INFO_HANDLER_REPORT_START) {
            if (!strcmp(variableListName, "Info_Mess_Header")) {
                self->isInfoMessage = true;
                self->isDSTSReport = false;

                if (!parseInfoBufHeader(self, value))
                    tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "invalid IM header!");

            }
            else if (!strcmp(variableListName, "Transfer_Report_ACK")) {
                self->isInfoMessage = false;
                self->isDSTSReport = false;
            }
            else {
                self->isInfoMessage = false;
                self->isDSTSReport = true;
                self->currentRptTransferSet = NULL;
                self->reportSeqNo++;
            }
        }

        if (self->isDSTSReport) {

            transferSet = self->currentRptTransferSet;

            if (!strcmp(variableListName, "Transfer_Set_Name")) {

                char transferSetDomain[33];
                char transferSetName[33];

                if (parseObjectReference(value, transferSetDomain, transferSetName)) {

                    transferSet = lookupDsTransferSetByName(self, transferSetDomain, transferSetName);

                    if (transferSet == NULL) {
                        tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Transfer set %s:%s not in active TS list!", transferSetDomain, transferSetName);
                    }
                    else {
                        self->currentRptTransferSet = transferSet;
                    }
                }
                else {
                    tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Invalid Transfer_Set_Name value %s:%s", transferSetDomain, transferSetName);

                    sendTransferSetNACK(self, transferSetDomain ,transferSetName);
                }

                if (flags & MMS_INFO_HANDLER_REPORT_START) {
                    if (self->dsTransferSetReportHandler)
                        self->dsTransferSetReportHandler(self->dsTransferSetReportHandlerParameter, false, self->reportSeqNo, transferSet);
                }
            }
            else {

                if (flags & MMS_INFO_HANDLER_REPORT_START) {
                    if (self->dsTransferSetReportHandler)
                        self->dsTransferSetReportHandler(self->dsTransferSetReportHandlerParameter, false, self->reportSeqNo, transferSet);
                }

                if (!strcmp(variableListName, "DSConditions_Detected")) {
                    if (transferSet)
                        transferSet->dsConditionsDetected = MmsValue_getBitStringAsInteger(value);
                }
                else if (!strcmp(variableListName, "Event_Code_Detected")) {
                    if (transferSet)
                        transferSet->eventCodeDetected = MmsValue_toInt32(value);
                }
                else if (!strcmp(variableListName, "Transfer_Set_Time_Stamp")) {
                    if (transferSet)
                        transferSet->transferSetTimestamp = MmsValue_toInt32(value);

                }
                else {

                    if (self->dsTransferSetValueHandler) {

                        struct sTase2_PointValue _pointValue;

                        _pointValue.type = determineTypeFromMmsValue(value);
                        _pointValue.pointValue = value;

                        self->dsTransferSetValueHandler(self->dsTransferSetValueHandlerParameter, transferSet, domainName, variableListName, &(_pointValue));
                    }

                    if (transferSet && transferSet->dataSet) {
                        updateDataSetValue(transferSet->dataSet, domainName, variableListName, value);
                    }
                }
            }
        } /* if (self->isInfoMessage == false) */

        if (flags & MMS_INFO_HANDLER_REPORT_END) {

            if (self->isInfoMessage) {

                if (StringUtils_startsWith(variableListName, "Info_Buff_")) {
                    if (MmsValue_getType(value) == MMS_OCTET_STRING) {
                        if (MmsValue_getOctetStringSize(value) >= self->imSize) {

                            /* call IM callback handler */
                            if (self->imHandler != NULL) {
                                self->imHandler(self->imHandlerParameter, self->imInfoRef,
                                        self->imLocalRef, self->imMsgId, self->imSize,
                                        MmsValue_getOctetStringBuffer(value));
                            }

                        }
                        else
                            tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Information message invalid size");
                    }
                    else
                        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Information message has wrong type");
                }
                else
                    tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Invalid information message (wrong name of information buffer: %s)", variableListName);

            }
            else if (self->isDSTSReport) {
                if (transferSet) {

                    if (transferSet->critical)
                        sendTransferSetACK(self, transferSet);
                }

                if (self->dsTransferSetReportHandler)
                    self->dsTransferSetReportHandler(self->dsTransferSetReportHandlerParameter, true, self->reportSeqNo, transferSet);
            }
        }
    }

exit_function:

    Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

    return;
}

Tase2_ClientDSTransferSet
Tase2_Client_getNextDSTransferSet(Tase2_Client self, const char* domainName, Tase2_ClientError* err)
{
    Tase2_ClientDSTransferSet transferSet = NULL;

    if (err)
        *err = TASE2_CLIENT_ERROR_FAILED;

    MmsValue* dsTransferSetName =
            MmsClient_readVariable(self->mmsClient, &(self->lastErrorCode), domainName, "Next_DSTransfer_Set", false);

    if (dsTransferSetName) {
        char tsDomain[33];
        char tsName[33];

        if (parseObjectReference(dsTransferSetName, tsDomain, tsName)) {

            transferSet = (Tase2_ClientDSTransferSet) GLOBAL_CALLOC(1, sizeof(struct sTase2_ClientDSTransferSet));

            if (transferSet) {
                strncpy_save(transferSet->transferSetName, tsName, 33);
                strncpy_save(transferSet->domainName, tsDomain, 33);

                if (err)
                    *err = TASE2_CLIENT_ERROR_OK;
            }

        }
        else {

            if (MmsValue_getType(dsTransferSetName) == MMS_DATA_ACCESS_ERROR) {
                if (err)
                    *err = convertDataAccessErrorToClientError(MmsValue_getDataAccessError(dsTransferSetName));
            }
            else {
                if (err)
                    *err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
            }
        }

        MmsValue_delete(dsTransferSetName);
    }
    else {
        if (err)
            *err = convertMmsErrorToClientError(self->lastErrorCode);
    }

    return transferSet;
}

void
Tase2_ClientDSTransferSet_destroy(Tase2_ClientDSTransferSet self)
{
    if (self->tsValues != NULL)
        MmsValue_delete(self->tsValues);

    GLOBAL_FREEMEM(self);
}

static MmsValue*
createNewMmsObjectNameValue()
{
    MmsValue* objectName = MmsValue_createEmptyStructure(3);

    MmsValue_setElement(objectName, 0, MmsValue_newUnsigned(8));
    MmsValue_setElement(objectName, 1, MmsValue_newVisibleStringWithSize(32));
    MmsValue_setElement(objectName, 2, MmsValue_newVisibleStringWithSize(32));

    return objectName;
}

static MmsValue*
createNewDSTransferSetValue()
{
    MmsValue* dsts = MmsValue_createEmptyStructure(13);

    MmsValue_setElement(dsts, 0, createNewMmsObjectNameValue());
    MmsValue_setElement(dsts, 1, MmsValue_newInteger(32));
    MmsValue_setElement(dsts, 2, MmsValue_newInteger(32));
    MmsValue_setElement(dsts, 3, MmsValue_newInteger(32));
    MmsValue_setElement(dsts, 4, MmsValue_newInteger(32));
    MmsValue_setElement(dsts, 5, MmsValue_newInteger(32));
    MmsValue_setElement(dsts, 6, MmsValue_newBitString(5));
    MmsValue_setElement(dsts, 7, MmsValue_newBoolean(false));
    MmsValue_setElement(dsts, 8, MmsValue_newBoolean(false));
    MmsValue_setElement(dsts, 9, MmsValue_newBoolean(false));
    MmsValue_setElement(dsts, 10, MmsValue_newBoolean(false));
    MmsValue_setElement(dsts, 11, MmsValue_newBoolean(false));
    MmsValue_setElement(dsts, 12, MmsValue_newInteger(16));

    return dsts;
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeValues(Tase2_ClientDSTransferSet self, Tase2_Client client)
{
    if (self->tsValues == NULL)
        self->tsValues = createNewDSTransferSetValue();

    if (self->tsValues) {

        int arraySize = MmsValue_getArraySize(self->tsValues);

        MmsValue* dataSetName = MmsValue_getElement(self->tsValues, 0);

        MmsValue* dsScope = MmsValue_getElement(dataSetName, 0);
        MmsValue* dsDomain = MmsValue_getElement(dataSetName, 1);
        MmsValue* dsName = MmsValue_getElement(dataSetName, 2);

        if (self->dataSetDomain[0] == 0)
            MmsValue_setUint8(dsScope, 0);
        else
            MmsValue_setUint8(dsScope, 1);

        MmsValue_setVisibleString(dsDomain, self->dataSetDomain);
        MmsValue_setVisibleString(dsName, self->dataSetName);

        MmsValue* startTime = MmsValue_getElement(self->tsValues, 1);
        MmsValue_setInt32(startTime, self->startTime);

        MmsValue* interval = MmsValue_getElement(self->tsValues, 2);
        MmsValue_setInt32(interval, self->interval);

        MmsValue* tle = MmsValue_getElement(self->tsValues, 3);
        MmsValue_setInt32(tle, self->tle);

        MmsValue* bufferTime = MmsValue_getElement(self->tsValues, 4);
        MmsValue_setInt32(bufferTime, self->bufferTime);

        MmsValue* integrityCheck = MmsValue_getElement(self->tsValues, 5);
        MmsValue_setInt32(integrityCheck, self->integrityCheck);

        MmsValue* dsConditionsRequested = MmsValue_getElement(self->tsValues, 6);
        MmsValue_setBitStringFromInteger(dsConditionsRequested, self->dsConditionsRequested);

        MmsValue* blockData = MmsValue_getElement(self->tsValues, 7);
        MmsValue_setBoolean(blockData, self->blockData);

        MmsValue* critical = MmsValue_getElement(self->tsValues, 8);
        MmsValue_setBoolean(critical, self->critical);

        MmsValue* rbe = MmsValue_getElement(self->tsValues, 9);
        MmsValue_setBoolean(rbe, self->rbe);

        int statusIndex = 10;

        /* Handle optional field "AllChangesReported" when present */
        if (arraySize == 13) {

            MmsValue* allChangesReported = MmsValue_getElement(self->tsValues, 10);
            MmsValue_setBoolean(allChangesReported, self->allChangesReported);

            statusIndex = 11;
        }

        MmsValue* status = MmsValue_getElement(self->tsValues, statusIndex);
        MmsValue_setBoolean(status, self->status);

        if (self->status == false) {
            Semaphore_wait(client->activeTransferSetsLock);
            if (client->activeTransferSets)
                LinkedList_remove(client->activeTransferSets, self);
            Semaphore_post(client->activeTransferSetsLock);
        }
        else {
            Semaphore_wait(client->activeTransferSetsLock);

            if (client->activeTransferSets == NULL)
                client->activeTransferSets = LinkedList_create();

            LinkedList_add(client->activeTransferSets, self);

            Semaphore_post(client->activeTransferSetsLock);
        }

        MmsValue* eventCodeRequested = MmsValue_getElement(self->tsValues, statusIndex + 1);
        MmsValue_setInt16(eventCodeRequested, self->eventCodeRequested);

        MmsDataAccessError accessError =
                MmsClient_writeVariable(client->mmsClient, &(client->lastErrorCode), self->domainName, self->transferSetName, self->tsValues);

        if (accessError == DATA_ACCESS_ERROR_SUCCESS) {
            return TASE2_CLIENT_ERROR_OK;
        }
        else {

            if (self->status == true) {
                Semaphore_wait(client->activeTransferSetsLock);
                if (client->activeTransferSets)
                    LinkedList_remove(client->activeTransferSets, self);
                Semaphore_post(client->activeTransferSetsLock);
            }

            return convertDataAccessErrorToClientError(accessError);
        }
    }
    else {
        tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, client->endpoint, NULL, "Failed to write DS transfer set: values not initialized");

        return TASE2_CLIENT_ERROR_FAILED;
    }
}

Tase2_ClientError
Tase2_ClientDSTransferSet_readValues(Tase2_ClientDSTransferSet self, Tase2_Client client)
{
    Tase2_ClientError err = TASE2_CLIENT_ERROR_OK;

    MmsValue* tsValues = MmsClient_readVariable(client->mmsClient, &(client->lastErrorCode), self->domainName, self->transferSetName, false);

    if (tsValues) {

        int arraySize = MmsValue_getArraySize(tsValues);

        if ((MmsValue_getType(tsValues) == MMS_STRUCTURE) && ((arraySize > 11) && (arraySize < 14))) {

            MmsValue* dataSetName = MmsValue_getElement(tsValues, 0);

            if ((MmsValue_getType(dataSetName) == MMS_STRUCTURE) && (MmsValue_getArraySize(dataSetName) == 3)) {

                MmsValue* dsDomain = MmsValue_getElement(dataSetName, 1);
                MmsValue* dsName = MmsValue_getElement(dataSetName, 2);

                if ((MmsValue_getType(dsDomain) == MMS_VISIBLE_STRING) && (MmsValue_getType(dsName) == MMS_VISIBLE_STRING)) {
                    strncpy_save(self->dataSetDomain, MmsValue_toString(dsDomain), 33);
                    strncpy_save(self->dataSetName, MmsValue_toString(dsName), 33);
                }
                else {
                    err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                    goto delete_value;
                }

            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* startTime = MmsValue_getElement(tsValues, 1);

            if (MmsValue_getType(startTime) == MMS_INTEGER) {
                self->startTime = MmsValue_toInt32(startTime);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* interval = MmsValue_getElement(tsValues, 2);

            if (MmsValue_getType(interval) == MMS_INTEGER) {
                self->interval = MmsValue_toInt32(interval);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* tle = MmsValue_getElement(tsValues, 3);

            if (MmsValue_getType(tle) == MMS_INTEGER) {
                self->tle = MmsValue_toInt32(tle);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* bufferTime = MmsValue_getElement(tsValues, 4);

            if (MmsValue_getType(bufferTime) == MMS_INTEGER) {
                self->bufferTime = MmsValue_toInt32(bufferTime);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* integrityCheck = MmsValue_getElement(tsValues, 5);

            if (MmsValue_getType(integrityCheck) == MMS_INTEGER) {
                self->integrityCheck = MmsValue_toInt32(integrityCheck);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* dsConditionsRequested = MmsValue_getElement(tsValues, 6);

            if ((MmsValue_getType(dsConditionsRequested) == MMS_BIT_STRING) && (MmsValue_getBitStringSize(dsConditionsRequested) == 5)) {
                self->dsConditionsRequested = MmsValue_getBitStringAsInteger(dsConditionsRequested);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* blockData = MmsValue_getElement(tsValues, 7);

            if (MmsValue_getType(blockData) == MMS_BOOLEAN) {
                self->blockData = MmsValue_getBoolean(blockData);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* critical = MmsValue_getElement(tsValues, 8);

            if (MmsValue_getType(critical) == MMS_BOOLEAN) {
                self->critical = MmsValue_getBoolean(critical);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* rbe = MmsValue_getElement(tsValues, 9);

            if (MmsValue_getType(rbe) == MMS_BOOLEAN) {
                self->rbe = MmsValue_getBoolean(rbe);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            int statusIndex = 10;

            /* Handle optional field "AllChangesReported" when present */
            if (arraySize == 13) {

                MmsValue* allChangesReported = MmsValue_getElement(tsValues, 10);

                if (MmsValue_getType(allChangesReported) == MMS_BOOLEAN) {
                    self->allChangesReported = MmsValue_getBoolean(allChangesReported);
                }
                else {
                    err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                    goto delete_value;
                }

                statusIndex = 11;
            }

            MmsValue* status = MmsValue_getElement(tsValues, statusIndex);

            if (MmsValue_getType(status) == MMS_BOOLEAN) {
                self->status = MmsValue_getBoolean(status);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            MmsValue* eventCodeRequested = MmsValue_getElement(tsValues, statusIndex + 1);

            if (MmsValue_getType(eventCodeRequested) == MMS_INTEGER) {
                self->eventCodeRequested = MmsValue_toInt32(eventCodeRequested);
            }
            else {
                err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
                goto delete_value;
            }

            if (self->tsValues)
                MmsValue_delete(self->tsValues);

            self->tsValues = tsValues;

            return err;
        }
        else
            err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;

        delete_value:

        MmsValue_delete(tsValues);

    }
    else {
        err = TASE2_CLIENT_ERROR_FAILED;
    }

    return err;
}

const char*
Tase2_ClientDSTransferSet_getDomain(Tase2_ClientDSTransferSet self)
{
    return self->domainName;
}

const char*
Tase2_ClientDSTransferSet_getName(Tase2_ClientDSTransferSet self)
{
    return self->transferSetName;
}

const char*
Tase2_ClientDSTransferSet_getDataSetDomain(Tase2_ClientDSTransferSet self)
{
    return self->dataSetDomain;
}

const char*
Tase2_ClientDSTransferSet_getDataSetName(Tase2_ClientDSTransferSet self)
{
    return self->dataSetName;
}

Tase2_ClientDataSet
Tase2_ClientDSTransferSet_getDataSet(Tase2_ClientDSTransferSet self)
{
    return self->dataSet;
}

int32_t
Tase2_ClientDSTransferSet_getStartTime(Tase2_ClientDSTransferSet self)
{
    return self->startTime;
}

int32_t
Tase2_ClientDSTransferSet_getInterval(Tase2_ClientDSTransferSet self)
{
    return self->interval;
}

int32_t
Tase2_ClientDSTransferSet_getTLE(Tase2_ClientDSTransferSet self)
{
    return self->tle;
}

int32_t
Tase2_ClientDSTransferSet_getBufferTime(Tase2_ClientDSTransferSet self)
{
    return self->bufferTime;
}

int32_t
Tase2_ClientDSTransferSet_getIntegrityCheck(Tase2_ClientDSTransferSet self)
{
    return self->integrityCheck;
}

int
Tase2_ClientDSTransferSet_getDSConditionsRequested(Tase2_ClientDSTransferSet self)
{
    return self->dsConditionsRequested;
}

bool
Tase2_ClientDSTransferSet_getBlockData(Tase2_ClientDSTransferSet self)
{
    return self->blockData;
}

bool
Tase2_ClientDSTransferSet_getCritical(Tase2_ClientDSTransferSet self)
{
    return self->critical;
}

int16_t
Tase2_ClientDSTransferSet_getEventCodeRequested(Tase2_ClientDSTransferSet self)
{
    return self->eventCodeRequested;
}

bool
Tase2_ClientDSTransferSet_getRBE(Tase2_ClientDSTransferSet self)
{
    return self->rbe;
}

bool
Tase2_ClientDSTransferSet_getAllChangesReported(Tase2_ClientDSTransferSet self)
{
    return self->allChangesReported;
}

bool
Tase2_ClientDSTransferSet_getStatus(Tase2_ClientDSTransferSet self)
{
    return self->status;
}

void
Tase2_ClientDSTransferSet_setDataSetName(Tase2_ClientDSTransferSet self, const char* domainId, const char* dataSetId)
{
    strncpy_save(self->dataSetName, dataSetId, 33);
    strncpy_save(self->dataSetDomain, domainId, 33);
}

void
Tase2_ClientDSTransferSet_setDataSet(Tase2_ClientDSTransferSet self, Tase2_ClientDataSet dataSet)
{
    self->dataSet = dataSet;
}

void
Tase2_ClientDSTransferSet_setStartTime(Tase2_ClientDSTransferSet self, int32_t startTime)
{
    self->startTime = startTime;
}

void
Tase2_ClientDSTransferSet_setInterval(Tase2_ClientDSTransferSet self, int32_t interval)
{
    self->interval = interval;
}

void
Tase2_ClientDSTransferSet_setTLE(Tase2_ClientDSTransferSet self, int32_t tle)
{
    self->tle = tle;
}

void
Tase2_ClientDSTransferSet_setBufferTime(Tase2_ClientDSTransferSet self, int32_t bufferTime)
{
    self->bufferTime = bufferTime;
}

void
Tase2_ClientDSTransferSet_setIntegrityCheck(Tase2_ClientDSTransferSet self, int32_t integrityCheck)
{
    self->integrityCheck = integrityCheck;
}

void
Tase2_ClientDSTransferSet_setDSConditionsRequested(Tase2_ClientDSTransferSet self, int dsConditionsRequested)
{
    self->dsConditionsRequested = dsConditionsRequested;
}

void
Tase2_ClientDSTransferSet_setBlockData(Tase2_ClientDSTransferSet self, bool blockData)
{
    self->blockData = blockData;
}

void
Tase2_ClientDSTransferSet_setCritical(Tase2_ClientDSTransferSet self, bool critical)
{
    self->critical = critical;
}

/**
 * \brief Set the value of the RBE (report by exception) flag
 */
void
Tase2_ClientDSTransferSet_setRBE(Tase2_ClientDSTransferSet self, bool rbe)
{
    self->rbe = rbe;
}

void
Tase2_ClientDSTransferSet_setAllChangesReported(Tase2_ClientDSTransferSet self, bool allChangesReported)
{
    self->allChangesReported = allChangesReported;
}

void
Tase2_ClientDSTransferSet_setStatus(Tase2_ClientDSTransferSet self, bool status)
{
    self->status = status;
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeDataSetName(Tase2_ClientDSTransferSet self, Tase2_Client client, const char* domainId, const char* dataSetId)
{
    MmsValue* value = tase2_createObjectNameValue(domainId, dataSetId);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "DataSetName", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setDataSetName(self, domainId, dataSetId);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeStartTime(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t startTime)
{
    MmsValue* value = MmsValue_newIntegerFromInt32(startTime);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "StartTime", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setStartTime(self, startTime);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeInterval(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t interval)
{
    MmsValue* value = MmsValue_newIntegerFromInt32(interval);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "Interval", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setInterval(self, interval);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeTLE(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t tle)
{
    MmsValue* value = MmsValue_newIntegerFromInt32(tle);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "TLE", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setTLE(self, tle);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeBufferTime(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t bufferTime)
{
    MmsValue* value = MmsValue_newIntegerFromInt32(bufferTime);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "BufferTime", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setBufferTime(self, bufferTime);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeIntegrityCheck(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t integrityCheck)
{
    MmsValue* value = MmsValue_newIntegerFromInt32(integrityCheck);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "IntegrityCheck", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setIntegrityCheck(self, integrityCheck);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeDSConditionsRequested(Tase2_ClientDSTransferSet self, Tase2_Client client, int dsConditionsRequested)
{
    MmsValue* value = MmsValue_newBitString(5);

    MmsValue_setBitStringFromInteger(value, dsConditionsRequested);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "DSConditionsRequested", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setDSConditionsRequested(self, dsConditionsRequested);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeBlockData(Tase2_ClientDSTransferSet self, Tase2_Client client, bool blockData)
{
    MmsValue* value = MmsValue_newBoolean(blockData);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "BlockData", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setBlockData(self, blockData);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeCritical(Tase2_ClientDSTransferSet self, Tase2_Client client, bool critical)
{
    MmsValue* value = MmsValue_newBoolean(critical);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "Critical", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setCritical(self, critical);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeRBE(Tase2_ClientDSTransferSet self, Tase2_Client client, bool rbe)
{
    MmsValue* value = MmsValue_newBoolean(rbe);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "RBE", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setRBE(self, rbe);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeAllChangesReported(Tase2_ClientDSTransferSet self, Tase2_Client client, bool allChangesReported)
{
    MmsValue* value = MmsValue_newBoolean(allChangesReported);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "AllChangesReported", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS)
        Tase2_ClientDSTransferSet_setRBE(self, allChangesReported);

    return convertDataAccessErrorToClientError(accessError);
}

Tase2_ClientError
Tase2_ClientDSTransferSet_writeStatus(Tase2_ClientDSTransferSet self, Tase2_Client client, bool status)
{
    MmsValue* value = MmsValue_newBoolean(status);

    char* tsDomainName = NULL;

    if (self->domainName[0] != 0)
        tsDomainName = self->domainName;

    if (status == false) {
        Semaphore_wait(client->activeTransferSetsLock);
        if (client->activeTransferSets)
            LinkedList_remove(client->activeTransferSets, self);
        Semaphore_post(client->activeTransferSetsLock);
    }
    else {
        Semaphore_wait(client->activeTransferSetsLock);
        if (client->activeTransferSets == NULL)
            client->activeTransferSets = LinkedList_create();

        LinkedList_add(client->activeTransferSets, self);
        Semaphore_post(client->activeTransferSetsLock);
    }

    MmsDataAccessError accessError =
            MmsClient_writeVariableComponent(client->mmsClient, &(client->lastErrorCode), tsDomainName, self->transferSetName, "Status", value);

    MmsValue_delete(value);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS) {
        Tase2_ClientDSTransferSet_setStatus(self, status);
    }
    else {
        if (self->status == false) {
            Semaphore_wait(client->activeTransferSetsLock);
            if (client->activeTransferSets)
                LinkedList_remove(client->activeTransferSets, self);
            Semaphore_post(client->activeTransferSetsLock);
        }
    }

    return convertDataAccessErrorToClientError(accessError);
}

uint8_t
Tase2_ClientDSTransferSet_getDSConditionsDetected(Tase2_ClientDSTransferSet self)
{
    return self->dsConditionsDetected;
}

int16_t
Tase2_ClientDSTransferSet_getEventCodeDetected(Tase2_ClientDSTransferSet self)
{
    return self->eventCodeDetected;
}

int32_t
Tase2_ClientDSTransferSet_getTransferSetTimestamp(Tase2_ClientDSTransferSet self)
{
    return self->transferSetTimestamp;
}

static void
disconnectEvent(Tase2_Client self)
{
    MmsClient_setEndpointConnection(self->mmsClient, NULL);

    Semaphore_wait(self->activeTransferSetsLock);
    if (self->activeTransferSets) {
        LinkedList_destroyStatic(self->activeTransferSets);
        self->activeTransferSets = NULL;
    }
    Semaphore_post(self->activeTransferSetsLock);

    Tase2_Client_setState(self, TASE2_CLIENT_STATE_CLOSED);

    if (self->connectionClosedHandler) {
        self->connectionClosedHandler(self->connectionClosedHandlerParameter, self);
    }
}

static void
connectEvent(Tase2_Client self, MmsEndpointConnection connection)
{
    MmsClient_setEndpointConnection(self->mmsClient, connection);

    Tase2_Client_setState(self, TASE2_CLIENT_STATE_CONNECTED);
}


void
Tase2_Client_mmsConnectionHandler(Tase2_Client self, MmsEndpointConnection connection, MmsEndpointEvent event)
{
    if (event == MMS_ENDPOINT_EVENT_CONNECTION_CLOSED) {

        if (MmsClient_getEndpointConnection(self->mmsClient) != connection) {

            Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

            /* event for old (probably dead) connection --> ignore */
            tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "TASE.2 client: close event for old (probably dead) connection --> ignore.");

            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);
        }
        else
            disconnectEvent(self);
    }
    else if (event == MMS_ENDPOINT_EVENT_NEW_CONNECTION) {

        if (Tase2_Client_getState(self) == TASE2_CLIENT_STATE_CONNECTED) {

            Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

            tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "TASE.2 client: already connected (switch over to new endpoint connection).");

            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

            /* disconnect client from old (probably dead) connection */
            disconnectEvent(self);
        }

        connectEvent(self, connection);
    }
}

static Tase2_Client
Tase2_Client_createInternal(Tase2_Endpoint endpoint)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Client self = (Tase2_Client) GLOBAL_CALLOC(1, sizeof(struct sTase2_Client));

    if (self != NULL) {
        self->endpoint = endpoint;
        self->createdEndpoint = false;

        self->mmsClient = MmsClient_create();

        self->tase2MajorVersion = 0;
        self->tase2MinorVersion = 0;

        self->supportedFeatures = 0;

        self->lastErrorCode = MMS_ERROR_NONE;

        self->activeTransferSetsLock = Semaphore_create(1);
        self->activeTransferSets = LinkedList_create();

        self->currentRptTransferSet = NULL;

        self->dsTransferSetValueHandler = NULL;
        self->dsTransferSetValueHandlerParameter = NULL;

        self->dsTransferSetReportHandler = NULL;
        self->dsTransferSetReportHandlerParameter = NULL;

        self->stateChangedHandler = NULL;

        self->imHandler = NULL;
        self->imHandlerParameter = NULL;

        self->reportSeqNo = 0;

        self->state = TASE2_CLIENT_STATE_IDLE;
        self->stateLock = Semaphore_create(1);

        Tase2_Endpoint_addClient(self->endpoint, self);

        /* set information report handler */
        Tase2_Endpoint_installInformationReportHandler(self->endpoint, self, mmsInformationReportHandler, self);

        /* use default remote address from endpoint */
        Tase2_ApplicationAddress appAddr = Tase2_Endpoint_getRemoteApplicationAddress(endpoint);

        IsoApplicationAddress clientAppAddr = MmsClient_getRemoteApplicationAddress(self->mmsClient);

        memcpy(clientAppAddr, appAddr, sizeof(struct sIsoApplicationAddress));
    }

    return self;
}

/**
 * \brief Create a new Tase2_Client instance using an existing endpoint
 */
Tase2_Client
Tase2_Client_createEx(Tase2_Endpoint endpoint)
{
    return Tase2_Client_createInternal(endpoint);
}

Tase2_Client
Tase2_Client_create(TLSConfiguration tlsConfig)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Endpoint endpoint = Tase2_Endpoint_create(tlsConfig, false);

    if (endpoint) {
        Tase2_Client self = Tase2_Client_createInternal(endpoint);

        self->createdEndpoint = true;

        return self;
    }
    else {
        return NULL;
    }
}

void
Tase2_Client_setIdentity(Tase2_Client self, const char* vendor, const char* model, const char* revision)
{
    Tase2_Endpoint_setIdentity(self->endpoint, vendor, model, revision);
}

void
Tase2_Client_installDSTransferSetValueHandler(Tase2_Client self, Tase2_DSTransferSetValueHandler handler, void* parameter)
{
    self->dsTransferSetValueHandler = handler;
    self->dsTransferSetValueHandlerParameter = parameter;
}

void
Tase2_Client_installDSTransferSetReportHandler(Tase2_Client self, Tase2_DSTransferSetReportHandler handler, void* parameter)
{
    self->dsTransferSetReportHandler = handler;
    self->dsTransferSetReportHandlerParameter = parameter;
}

void
Tase2_Client_installIMTransferSetHandler(Tase2_Client self, Tase2_InformationMessageHandler handler, void* parameter)
{
    self->imHandler = handler;
    self->imHandlerParameter = parameter;
}

void
Tase2_Client_installConnectionClosedHandler(Tase2_Client self, Tase2_ConnectionClosedHandler handler, void* parameter)
{
    self->connectionClosedHandler = handler;
    self->connectionClosedHandlerParameter = parameter;
}

void
Tase2_Client_installStateChangedHandler(Tase2_Client self, Tase2_Client_StateChangedHandler handler, void* parameter)
{
    self->stateChangedHandler = handler;
    self->stateChangedHandlerParameter = parameter;
}

Tase2_Endpoint
Tase2_Client_getEndpoint(Tase2_Client self)
{
    return self->endpoint;
}

void
Tase2_Client_destroy(Tase2_Client self)
{
    Tase2_Client_disconnect(self);

    if (self->endpoint) {
        if (self->createdEndpoint) {
            Tase2_Endpoint_destroy(self->endpoint);
        }
        else {
            Tase2_Endpoint_removeClient(self->endpoint, self);
        }
    }

    if (self->mmsClient != NULL) {
        MmsClient_destroy(self->mmsClient);
    }

    if (self->activeTransferSets)
        LinkedList_destroyStatic(self->activeTransferSets);
    Semaphore_destroy(self->activeTransferSetsLock);

    Semaphore_destroy(self->stateLock);

    GLOBAL_FREEMEM(self);
}

Tase2_Statistics
Tase2_Client_getStatistics(Tase2_Client self)
{
    Tase2_Statistics currentStats;

    /* TODO client specific statistics? For now statistics is for the whole MmsEndpoint! */
    currentStats.messagesSent = MmsEndpoint_getStatistics(Tase2_Endpoint_getMmsEndpoint(self->endpoint), MMS_STAT_ID_APL_MESSAGES_SENT);
    currentStats.messagesRcvd = MmsEndpoint_getStatistics(Tase2_Endpoint_getMmsEndpoint(self->endpoint), MMS_STAT_ID_APL_MESSAGES_RCVD);
    currentStats.errors = MmsEndpoint_getStatistics(Tase2_Endpoint_getMmsEndpoint(self->endpoint), MMS_STAT_ID_REQUEST_TIMEOUTS);

    return currentStats;
}

void
Tase2_Client_setTcpPort(Tase2_Client self, int tcpPort)
{
    MmsEndpoint_setRemoteTcpPort(Tase2_Endpoint_getMmsEndpoint(self->endpoint), tcpPort);
}

/* INTERNAL */
IsoApplicationAddress
Tase2_Client_getRemoteApplicationAddress(Tase2_Client self)
{
    return MmsClient_getRemoteApplicationAddress(self->mmsClient);
}

void
Tase2_Client_setRemoteApplicationAddress(Tase2_Client self, Tase2_ApplicationAddress remoteAddress)
{
    MmsClient_setRemoteApplicationAddress(self->mmsClient, (IsoApplicationAddress) remoteAddress);
}

void
Tase2_Client_setRemoteApTitle(Tase2_Client self, const char* apTitle, int aeQualifier)
{
    IsoApplicationAddress isoAppAddr = MmsClient_getRemoteApplicationAddress(self->mmsClient);

    IsoApplicationAddress_setApTitle(isoAppAddr, apTitle);
    IsoApplicationAddress_setAeQualifier(isoAppAddr, aeQualifier);

    /* in case of active endpoint also set the endpoint remote address */
    if (Tase2_Endpoint_isPassive(self->endpoint) == false) {
        isoAppAddr = MmsEndpoint_getRemoteApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

        IsoApplicationAddress_setApTitle(isoAppAddr, apTitle);
        IsoApplicationAddress_setAeQualifier(isoAppAddr, aeQualifier);
    }
}

void
Tase2_Client_setRemoteAddresses(Tase2_Client self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector)
{
    IsoApplicationAddress isoAppAddr = MmsClient_getRemoteApplicationAddress(self->mmsClient);

    PSelector pSel;
    SSelector sSel;
    TSelector tSel;

    memcpy(&pSel, &pSelector, sizeof(PSelector));
    memcpy(&sSel, &sSelector, sizeof(SSelector));
    memcpy(&tSel, &tSelector, sizeof(Tase2_TSelector));

    IsoApplicationAddress_setLowerLayerAddresses(isoAppAddr, pSel, sSel, tSel);

    /* in case of active endpoint also set the endpoint remote address */
    if (Tase2_Endpoint_isPassive(self->endpoint) == false) {
        isoAppAddr = MmsEndpoint_getRemoteApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

        IsoApplicationAddress_setLowerLayerAddresses(isoAppAddr, pSel, sSel, tSel);
    }
}

void
Tase2_Client_setRemoteApInvocationId(Tase2_Client self, int apInvocationId)
{
    IsoApplicationAddress isoAppAddr = MmsEndpoint_getRemoteApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

    IsoApplicationAddress_setApInvocationId(isoAppAddr, apInvocationId);

    /* in case of active endpoint also set the endpoint remote address */
    if (Tase2_Endpoint_isPassive(self->endpoint) == false) {
        isoAppAddr = MmsEndpoint_getRemoteApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

        IsoApplicationAddress_setApInvocationId(isoAppAddr, apInvocationId);
    }
}

void
Tase2_Client_setRemoteAeInvocationId(Tase2_Client self, int aeInvocationId)
{
    IsoApplicationAddress isoAppAddr = MmsEndpoint_getRemoteApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

    IsoApplicationAddress_setAeInvocationId(isoAppAddr, aeInvocationId);

    /* in case of active endpoint also set the endpoint remote address */
    if (Tase2_Endpoint_isPassive(self->endpoint) == false) {
        isoAppAddr = MmsEndpoint_getRemoteApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

        IsoApplicationAddress_setAeInvocationId(isoAppAddr, aeInvocationId);
    }
}

void
Tase2_Client_setLocalApTitle(Tase2_Client self, const char* apTitle, int aeQualifier)
{
    IsoApplicationAddress isoAppAddr = MmsEndpoint_getLocalApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

    IsoApplicationAddress_setApTitle(isoAppAddr, apTitle);
    IsoApplicationAddress_setAeQualifier(isoAppAddr, aeQualifier);
}

void
Tase2_Client_setLocalAddresses(Tase2_Client self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector)
{
    IsoApplicationAddress isoAppAddr = MmsEndpoint_getLocalApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

    PSelector pSel;
    SSelector sSel;
    TSelector tSel;

    memcpy(&pSel, &pSelector, sizeof(PSelector));
    memcpy(&sSel, &sSelector, sizeof(SSelector));
    memcpy(&tSel, &tSelector, sizeof(TSelector));

    IsoApplicationAddress_setLowerLayerAddresses(isoAppAddr, pSel, sSel, tSel);
}

void
Tase2_Client_setLocalApInvocationId(Tase2_Client self, int apInvocationId)
{
    IsoApplicationAddress isoAppAddr = MmsEndpoint_getLocalApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

    IsoApplicationAddress_setApInvocationId(isoAppAddr, apInvocationId);
}

void
Tase2_Client_setLocalAeInvocationId(Tase2_Client self, int aeInvocationId)
{
    IsoApplicationAddress isoAppAddr = MmsEndpoint_getLocalApplicationAddress(Tase2_Endpoint_getMmsEndpoint(self->endpoint));

    IsoApplicationAddress_setAeInvocationId(isoAppAddr, aeInvocationId);
}

Tase2_ClientError
Tase2_Client_connectEx(Tase2_Client self)
{
    return Tase2_Client_connect(self, NULL, NULL, 0);
}

static Tase2_Endpoint_Connection
tase2_Client_getPeer(Tase2_Client self)
{
    Tase2_Endpoint_Connection peer = NULL;

    if (self->mmsClient && self->endpoint) {

        MmsEndpointConnection connection = MmsClient_getEndpointConnection(self->mmsClient);

        if (connection) {
            peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);
        }
    }

    return peer;
}

Tase2_ClientError
Tase2_Client_connect(Tase2_Client self, const char* hostname, const char* apTitle, int aeQualifier)
{
    TASE2_LIB_CHECK_RUNTIME();

    if (apTitle) {
        Tase2_Endpoint_setLocalApTitle(self->endpoint, apTitle, aeQualifier);
    }

    /* Connect endpoint if not already connected */

    if (Tase2_Endpoint_isConnected(self->endpoint) == false) {

        if (hostname)
            Tase2_Endpoint_setRemoteIpAddress(self->endpoint, hostname);

        Tase2_Endpoint_connect(self->endpoint);

        /* Wait until connection is ready */
        if (Tase2_Endpoint_waitForState(self->endpoint, TASE2_ENDPOINT_STATE_CONNECTED, 10000)) {

            Tase2_Endpoint_Connection peer = tase2_Client_getPeer(self);

            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Connected.");

            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

            Tase2_Client_setState(self, TASE2_CLIENT_STATE_CONNECTED);
        }
        else {
            tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "Connect failed!");
            Tase2_Client_setState(self, TASE2_CLIENT_STATE_CLOSED);
            return TASE2_CLIENT_ERROR_FAILED;
        }
    }
    else {
        Tase2_Endpoint_Connection peer = tase2_Client_getPeer(self);

        if (peer) {
            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);
        }
        else  {
            peer = Tase2_Endpoint_getMatchingEndpointConnection(self->endpoint, Tase2_Client_getRemoteApplicationAddress(self));

            if (peer) {
                Tase2_Client_mmsConnectionHandler(self, Tase2_Endpoint_Connection_getMmsConnection(peer), MMS_ENDPOINT_EVENT_NEW_CONNECTION);
            }
        }

        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

        Tase2_Client_setState(self, TASE2_CLIENT_STATE_CONNECTED);
    }

    MmsValue* tase2Version = MmsClient_readVariable(self->mmsClient, &(self->lastErrorCode), NULL, "TASE2_Version", true);

    if (tase2Version == NULL) {
        Tase2_Endpoint_Connection peer = tase2_Client_getPeer(self);

        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Failed to read TASE2_Version!");

        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

        if (self->createdEndpoint)
            Tase2_Client_disconnect(self);

        return TASE2_CLIENT_ERROR_FAILED;
    }
    else {

        if (MmsValue_getType(tase2Version) == MMS_DATA_ACCESS_ERROR) {
            Tase2_Endpoint_Connection peer = tase2_Client_getPeer(self);

            tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Failed to read TASE2_Version!");

            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

            if (self->createdEndpoint)
                Tase2_Client_disconnect(self);

            return TASE2_CLIENT_ERROR_FAILED;
        }
        else {

            /* check type */

            bool validType = true;

            if (MmsValue_getType(tase2Version) == MMS_STRUCTURE) {
                if (MmsValue_getArraySize(tase2Version) == 2) {
                    if (MmsValue_getType(MmsValue_getElement(tase2Version, 0)) != MMS_INTEGER)
                            validType = false;
                    if (MmsValue_getType(MmsValue_getElement(tase2Version, 1)) != MMS_INTEGER)
                            validType = false;
                }
                else
                    validType = false;
            }
            else
                validType = false;

            if (validType) {
                MmsValue* majorVersion = MmsValue_getElement(tase2Version, 0);
                MmsValue* minorVersion = MmsValue_getElement(tase2Version, 1);

                self->tase2MajorVersion = MmsValue_toInt32(majorVersion);
                self->tase2MinorVersion = MmsValue_toInt32(minorVersion);
            }
            else {
                Tase2_Endpoint_Connection peer = tase2_Client_getPeer(self);

                tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "TASE2_Version has wrong type!");

                Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

                if (self->createdEndpoint)
                    Tase2_Client_disconnect(self);

                return TASE2_CLIENT_ERROR_FAILED;
            }

        }

        MmsValue_delete(tase2Version);
    }

    MmsValue* supportedFeatures = MmsClient_readVariable(self->mmsClient, &(self->lastErrorCode), NULL, "Supported_Features", false);

    if (supportedFeatures == NULL) {

        Tase2_Endpoint_Connection peer = tase2_Client_getPeer(self);

        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Failed to read Supported_Features!");

        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

        return TASE2_CLIENT_ERROR_FAILED;
    }
    else {

        if ((MmsValue_getType(supportedFeatures) == MMS_BIT_STRING) && (MmsValue_getBitStringSize(supportedFeatures) == 12)) {

            self->supportedFeatures = MmsValue_getBitStringAsInteger(supportedFeatures);

        }
        else {
            Tase2_Endpoint_Connection peer = tase2_Client_getPeer(self);

            tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Supported_Features has wrong type");

            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);
        }

        MmsValue_delete(supportedFeatures);

        if (supportedFeatures == 0)
            return TASE2_CLIENT_ERROR_FAILED;
    }

    return TASE2_CLIENT_ERROR_OK;
}

Tase2_ClientState
Tase2_Client_getState(Tase2_Client self)
{
    Tase2_ClientState state;

    Semaphore_wait(self->stateLock);
    state = self->state;
    Semaphore_post(self->stateLock);

    return state;
}

Tase2_Version
Tase2_Client_getServerVersion(Tase2_Client self) {
    Tase2_Version version;

    version.major = self->tase2MajorVersion;
    version.minor = self->tase2MinorVersion;

    return version;
}

int
Tase2_Client_getServerVersionMajor(Tase2_Client self)
{
    return self->tase2MajorVersion;
}

int
Tase2_Client_getServerVersionMinor(Tase2_Client self)
{
    return self->tase2MinorVersion;
}

int
Tase2_Client_getSupportedFeatures(Tase2_Client self)
{
    return self->supportedFeatures;
}

void
Tase2_Client_readPeerIdentity(Tase2_Client self, Tase2_ClientError* err, char** vendor, char** model, char** revision)
{
    MmsServerIdentity* identity = MmsClient_identify(self->mmsClient, &(self->lastErrorCode));

    if (self->lastErrorCode == MMS_ERROR_NONE) {

        if (vendor)
            *vendor = StringUtils_copyString(identity->vendorName);

        if (model)
            *model = StringUtils_copyString(identity->modelName);

        if (revision)
            *revision = StringUtils_copyString(identity->revision);

        if (err)
            *err = TASE2_CLIENT_ERROR_OK;
    }
    else {

        if (err)
            *err = convertMmsErrorToClientError(self->lastErrorCode);
    }

    if (identity)
        MmsServerIdentity_destroy(identity);
}

char*
Tase2_Client_getBilateralTableId(Tase2_Client self, Tase2_ClientError* err, const char* domainName)
{
    MmsError mmsError;

    MmsValue* bltVal = MmsClient_readVariable(self->mmsClient, &mmsError, domainName, "Bilateral_Table_ID", false);

    if ((bltVal) &&  (MmsValue_getType(bltVal) == MMS_VISIBLE_STRING)) {

        if (err)
            *err = TASE2_CLIENT_ERROR_OK;

        char* bltStr = StringUtils_copyString(MmsValue_toString(bltVal));

        MmsValue_delete(bltVal);

        return bltStr;

    }
    else {
        if (err) {
            if (mmsError != MMS_ERROR_NONE)
                *err = convertMmsErrorToClientError(mmsError);
            else if ((bltVal) && (MmsValue_getType(bltVal) == MMS_DATA_ACCESS_ERROR)) {
                *err = convertDataAccessErrorToClientError(MmsValue_getDataAccessError(bltVal));
            }
            else
                *err = TASE2_CLIENT_ERROR_FAILED;
        }

        if (bltVal)
            MmsValue_delete(bltVal);

        return NULL;
    }
}

LinkedList /* <char*> */
Tase2_Client_getVCCVariables(Tase2_Client self, Tase2_ClientError* err)
{
    TASE2_LIB_CHECK_RUNTIME();

    self->lastErrorCode = MMS_ERROR_NONE;

    LinkedList vccVariables = MmsClient_getVMDVariableNames(self->mmsClient, &(self->lastErrorCode));

    if ((vccVariables != NULL) && (self->lastErrorCode == MMS_ERROR_NONE)) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;
    }
    else {
        if (err)
            *err = TASE2_CLIENT_ERROR_FAILED;
    }

    return vccVariables;
}

LinkedList /* <char*> */
Tase2_Client_getVCCDataSets(Tase2_Client self, Tase2_ClientError* err)
{
    TASE2_LIB_CHECK_RUNTIME();

    self->lastErrorCode = MMS_ERROR_NONE;

    LinkedList dataSetNames = MmsClient_getDomainVariableListNames(self->mmsClient, &(self->lastErrorCode), NULL);

    if ((dataSetNames != NULL) && (self->lastErrorCode == MMS_ERROR_NONE)) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;
    }
    else {
        if (err)
            *err = TASE2_CLIENT_ERROR_FAILED;
    }

    return dataSetNames;
}

LinkedList /* <char*> */
Tase2_Client_getDomainNames(Tase2_Client self, Tase2_ClientError* err)
{
    TASE2_LIB_CHECK_RUNTIME();

    self->lastErrorCode = MMS_ERROR_NONE;

    LinkedList domainNames = MmsClient_getDomainNames(self->mmsClient, &(self->lastErrorCode));

    if ((domainNames != NULL) && (self->lastErrorCode == MMS_ERROR_NONE)) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;
    }
    else {
        if (err)
            *err = TASE2_CLIENT_ERROR_FAILED;
    }

    return domainNames;
}

LinkedList /* <char*> */
Tase2_Client_getDomainVariables(Tase2_Client self, const char* domainName, Tase2_ClientError* err)
{
    self->lastErrorCode = MMS_ERROR_NONE;

    LinkedList variableNames = MmsClient_getDomainVariableNames(self->mmsClient, &(self->lastErrorCode), domainName);

    if ((variableNames != NULL) && (self->lastErrorCode == MMS_ERROR_NONE)) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;
    }
    else {
        if (err)
            *err = TASE2_CLIENT_ERROR_FAILED;
    }

    return variableNames;
}

LinkedList
Tase2_Client_getDomainDataSets(Tase2_Client self, const char* domainName, Tase2_ClientError* err)
{
    TASE2_LIB_CHECK_RUNTIME();

    self->lastErrorCode = MMS_ERROR_NONE;

    LinkedList dataSetNames = MmsClient_getDomainVariableListNames(self->mmsClient, &(self->lastErrorCode), domainName);

    if ((dataSetNames != NULL) && (self->lastErrorCode == MMS_ERROR_NONE)) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;
    }
    else {
        if (err)
            *err = TASE2_CLIENT_ERROR_FAILED;
    }

    return dataSetNames;
}

LinkedList /* <Tase2_DataSetEntrySpec> */
Tase2_Client_getDataSetDirectory(Tase2_Client self, const char* domainName, const char* dataSetName, Tase2_ClientError* err)
{
   TASE2_LIB_CHECK_RUNTIME();

   bool deletable;

   LinkedList dataSetEntries = MmsClient_readNamedVariableListDirectory(self->mmsClient, &(self->lastErrorCode), domainName, dataSetName, &deletable);

   if ((dataSetEntries != NULL) && (self->lastErrorCode == MMS_ERROR_NONE)) {
       if (err)
           *err = TASE2_CLIENT_ERROR_OK;
   }
   else {
       if (err)
           *err = TASE2_CLIENT_ERROR_FAILED;
   }

   return dataSetEntries;
}

const char*
Tase2_DataSetEntrySpec_getDomainId(Tase2_DataSetEntrySpec self)
{
    return self->domainId;
}

const char*
Tase2_DataSetEntrySpec_getItemId(Tase2_DataSetEntrySpec self)
{
    return self->itemId;
}

void
Tase2_DataSetEntrySpec_destroy(Tase2_DataSetEntrySpec self)
{
    MmsVariableAccessSpecification_destroy((MmsVariableAccessSpecification*) self);
}

void
Tase2_Client_disconnect(Tase2_Client self)
{
    TASE2_LIB_CHECK_RUNTIME();

    if (Tase2_Client_getState(self) == TASE2_CLIENT_STATE_CONNECTED) {
        /* TODO specify disconnect method */
        Tase2_Client_setState(self, TASE2_CLIENT_STATE_CLOSED);

        if (self->createdEndpoint)
            Tase2_Endpoint_disconnect(self->endpoint, 0);
    }

    Semaphore_wait(self->activeTransferSetsLock);

    if (self->activeTransferSets) {
        /* remove all elements form list of active transfer sets */
        LinkedList_destroyStatic(self->activeTransferSets);
        self->activeTransferSets = LinkedList_create();
    }

    Semaphore_post(self->activeTransferSetsLock);
}

int
Tase2_Client_selectDevice(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName)
{
    TASE2_LIB_CHECK_RUNTIME();

    char buffer[70];
    int checkBackId = 0;
    MmsValue* checkBackName;

    self->lastErrorCode = MMS_ERROR_NONE;

    if (strlen(deviceName) > 32) {
        if (err)
            *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

        goto exit_function;
    }

    const char* deviceNameStr;

    if (StringUtils_endsWith(deviceName, "_SBO") )
        deviceNameStr = deviceName;
    else {
        StringUtils_createStringInBuffer(buffer, 2, deviceName, "_SBO");
        deviceNameStr = buffer;
    }

    checkBackName = MmsClient_readVariable(self->mmsClient, &(self->lastErrorCode), domainName, deviceNameStr, false);

    if (err)
        *err = TASE2_CLIENT_ERROR_FAILED;

    if (checkBackName) {

        if (MmsValue_getType(checkBackName) == MMS_INTEGER) {
            checkBackId = MmsValue_toInt32(checkBackName);

            if (err)
                *err = TASE2_CLIENT_ERROR_OK;
        }
        else if (MmsValue_getType(checkBackName) == MMS_DATA_ACCESS_ERROR) {
            if (err)
                *err = convertDataAccessErrorToClientError(MmsValue_getDataAccessError(checkBackName));
        }
        else {
            if (err)
                *err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
        }

        MmsValue_delete(checkBackName);
    }

exit_function:

    return checkBackId;
}

bool
Tase2_Client_setTag(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName, Tase2_TagValue tagValue, const char* reason)
{
    char buffer[70];
    bool retVal = false;

    self->lastErrorCode = MMS_ERROR_NONE;

    if (strlen(deviceName) > 32) {
        if (err)
            *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

        goto exit_function;
    }

    if (domainName) {
        if (strlen(domainName) > 32) {
            if (err)
                *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

            goto exit_function;
        }
    }

    if (strlen(reason) > 255) {
        if (err)
            *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

        goto exit_function;
    }

    const char* deviceNameStr;

    /* if required, add "_TAG" to the point name */
    if (StringUtils_endsWith(deviceName, "_TAG") )
        deviceNameStr = deviceName;
    else {
        StringUtils_createStringInBuffer(buffer, 2, deviceName, "_TAG");
        deviceNameStr = buffer;
    }

    MmsValue* tagStructure;
    tagStructure = MmsValue_createEmptyStructure(2);

    MmsValue* tagVal;
    tagVal = MmsValue_newBitString(3);

    MmsValue_setBitStringBit(tagVal, 1, (tagValue & 1) != 0);
    MmsValue_setBitStringBit(tagVal, 0, (tagValue & 2) != 0);

    MmsValue* reasonVal;
    reasonVal = MmsValue_newVisibleString(reason);
    MmsValue_setElement(tagStructure, 0, tagVal);
    MmsValue_setElement(tagStructure, 1, reasonVal);

    MmsDataAccessError accessError;
    accessError = MmsClient_writeVariable(self->mmsClient, &(self->lastErrorCode), domainName, deviceNameStr, tagStructure);

    MmsValue_delete(tagStructure);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;

        retVal = true;
    }
    else {
        if (err)
            *err = convertDataAccessErrorToClientError(accessError);
    }

exit_function:

    return retVal;
}




Tase2_TagValue
Tase2_Client_getTag(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName, bool* isArmed, char* reasonBuf, int reasonBufSize)
{
    char buffer[70]; /* buffer where to store the _TAG variable name, if required */

    Tase2_TagValue retValue = TASE2_TAG_INVALID;

    self->lastErrorCode = MMS_ERROR_NONE;

    if (strlen(deviceName) > 32) {
        if (err)
            *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

        goto exit_function;
    }

    if (domainName) {
        if (strlen(domainName) > 32) {
            if (err)
                *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

            goto exit_function;
        }
    }

    const char* deviceNameStr;

    /* if required, add "_TAG" to the point name */
    if (StringUtils_endsWith(deviceName, "_TAG") )
        deviceNameStr = deviceName;
    else {
        StringUtils_createStringInBuffer(buffer, 2, deviceName, "_TAG");
        deviceNameStr = buffer;
    }

    MmsValue* mmsTagValue;
    mmsTagValue = MmsClient_readVariable(self->mmsClient, &(self->lastErrorCode), domainName, deviceNameStr, false);

    if (err)
        *err = TASE2_CLIENT_ERROR_FAILED;

    if (mmsTagValue) {

        if ((MmsValue_getType(mmsTagValue) == MMS_STRUCTURE) && (MmsValue_getArraySize(mmsTagValue) == 2)) {

            MmsValue* flags = MmsValue_getElement(mmsTagValue, 0);

            if (MmsValue_getBitStringSize(flags) != 3) {
                tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "Tase2_Client_getTag: \"Flags\" of wrong size");
                goto exit_function;
            }

            int intVal = 0;

            if (MmsValue_getBitStringBit(flags, 0))
                intVal += 2;

            if (MmsValue_getBitStringBit(flags, 1))
                intVal += 1;

            retValue = (Tase2_TagValue) intVal;

            if (isArmed)
                *isArmed = MmsValue_getBitStringBit(flags, 2);

            if (err)
                *err = TASE2_CLIENT_ERROR_OK;

            if (reasonBuf) {

                    MmsValue* reason = MmsValue_getElement(mmsTagValue, 1);

                    if (MmsValue_getType(reason) == MMS_VISIBLE_STRING) {

                        const char* reasonStrVal = MmsValue_toString(reason);
                        strncpy_save(reasonBuf, reasonStrVal, reasonBufSize);
                    }
                    else {
                        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "Tase2_Client_getTag: \"Reason\" of wrong type");

                        if (err)
                            *err = TASE2_CLIENT_ERROR_FAILED;
                    }
            }


        }
        else {
            tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "Tase2_Client_getTag: invalid return type");
        }

        MmsValue_delete(mmsTagValue);
    }


exit_function:

    return retValue;
}


bool
Tase2_Client_sendCommand(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName, int16_t commandValue)
{
    TASE2_LIB_CHECK_RUNTIME();

    bool retVal = false;

    self->lastErrorCode = MMS_ERROR_NONE;

    if (strlen(deviceName) > 32) {
        if (err)
            *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

        goto exit_function;
    }

    MmsValue* commandVal;
    commandVal = MmsValue_newIntegerFromInt16(commandValue);

    MmsDataAccessError accessError;
    accessError = MmsClient_writeVariable(self->mmsClient, &(self->lastErrorCode), domainName, deviceName, commandVal);

    MmsValue_delete(commandVal);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;

        retVal = true;
    }
    else {
        if (err)
            *err = convertDataAccessErrorToClientError(accessError);
    }

exit_function:

    return retVal;
}


bool
Tase2_Client_sendDiscreteSetPoint(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName, int16_t setPointValue)
{
    TASE2_LIB_CHECK_RUNTIME();

    return Tase2_Client_sendCommand(self, err, domainName, deviceName, setPointValue);
}


bool
Tase2_Client_sendRealSetPoint(Tase2_Client self, Tase2_ClientError* err, const char* deviceName, const char* pointName,
        float setPointValue)
{
    TASE2_LIB_CHECK_RUNTIME();

    bool retVal = false;

    self->lastErrorCode = MMS_ERROR_NONE;

    if (strlen(pointName) > 32) {
        if (err)
            *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

        goto exit_function;
    }

    MmsValue* commandVal;
    commandVal = MmsValue_newFloat(setPointValue);

    MmsDataAccessError accessError;
    accessError = MmsClient_writeVariable(self->mmsClient, &(self->lastErrorCode), deviceName, pointName, commandVal);

    MmsValue_delete(commandVal);

    if (accessError == DATA_ACCESS_ERROR_SUCCESS) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;

        retVal = true;
    }
    else {
        if (err)
            *err = convertMmsErrorToClientError(self->lastErrorCode);
    }

exit_function:

    return retVal;
}

Tase2_PointValue
Tase2_Client_readPointValue(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* pointName)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_PointValue pointValue = NULL;

    if (err)
        *err = TASE2_CLIENT_ERROR_OK;

    MmsValue* mmsPointValue = MmsClient_readVariable(self->mmsClient, &(self->lastErrorCode), domainName, pointName, false);

    if (mmsPointValue != NULL) {

        if (MmsValue_getType(mmsPointValue) == MMS_DATA_ACCESS_ERROR) {

            MmsDataAccessError dataAccessError = MmsValue_getDataAccessError(mmsPointValue);

            if (err)
                *err = convertDataAccessErrorToClientError(dataAccessError);

            MmsValue_delete(mmsPointValue);
        }
        else {
            pointValue = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

            if (pointValue) {
                pointValue->type = determineTypeFromMmsValue(mmsPointValue);
                pointValue->pointValue = mmsPointValue;
            }
            else
                MmsValue_delete(mmsPointValue);
        }
    }
    else {
        if (err)
            *err = convertMmsErrorToClientError(self->lastErrorCode);
    }

    return pointValue;
}

LinkedList /* <Tase2_PointValue> */
Tase2_Client_readMultiplePointValues(Tase2_Client self, Tase2_ClientError* err, const char* domainName, LinkedList pointNames)
{
    TASE2_LIB_CHECK_RUNTIME();

    LinkedList values = NULL;

    Tase2_PointValue pointValue = NULL;

    if (err)
        *err = TASE2_CLIENT_ERROR_OK;

    MmsError mmsError;

    MmsValue* result = MmsClient_readMultipleVariables(self->mmsClient, &mmsError, domainName, pointNames, false);

    if (mmsError == MMS_ERROR_NONE) {
        if (result && (MmsValue_getType(result) == MMS_ARRAY)) {

            values = LinkedList_create();

            int i;

            for (i = 0; i < MmsValue_getArraySize(result); i++) {
                MmsValue* element = MmsValue_getElement(result, i);

                Tase2_PointValueType type = determineTypeFromMmsValue(element);

                pointValue = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

                if (pointValue) {
                    pointValue->type = determineTypeFromMmsValue(element);
                    pointValue->pointValue = element;

                    LinkedList_add(values, pointValue);
                }
                else
                    MmsValue_delete(element);

                MmsValue_setElement(result, i, NULL);
            }

            MmsValue_delete(result);
        }
        else {
            *err = TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE;
        }
    }
    else {
        if (err)
            *err = convertMmsErrorToClientError(self->lastErrorCode);
    }

    return values;
}

void
Tase2_Client_writePointValue(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* pointName, Tase2_PointValue value)
{
    TASE2_LIB_CHECK_RUNTIME();

    MmsDataAccessError dataAccessErr = MmsClient_writeVariable(self->mmsClient, &(self->lastErrorCode), domainName, pointName, value->pointValue);

    if (err)
        *err = convertDataAccessErrorToClientError(dataAccessErr);
}

Tase2_ClientDataSet
Tase2_Client_getDataSet(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* dataSetName)
{
    TASE2_LIB_CHECK_RUNTIME();

    if ((domainName != NULL) && (strlen(domainName) > 32)) {
        if (err)
            *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

        return NULL;
    }

    if ((dataSetName == NULL) || (strlen(dataSetName) > 32)) {
        if (err)
            *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

        return NULL;
    }

    Tase2_ClientDataSet dataSet = NULL;

    if (err)
        *err = TASE2_CLIENT_ERROR_OK;

    bool isDeletable;

    LinkedList variableListDirectory = MmsClient_readNamedVariableListDirectory(self->mmsClient, &(self->lastErrorCode), domainName, dataSetName, &isDeletable);

    if (variableListDirectory) {
        dataSet = (Tase2_ClientDataSet) GLOBAL_MALLOC(sizeof(struct sTase2_ClientDataSet));

        if (dataSet) {

            if (domainName) {
                strncpy_save(dataSet->domainName, domainName, 33);
            }
            else {
                dataSet->domainName[0] = 0;
            }

            strncpy_save(dataSet->dataSetName, dataSetName, 33);

            dataSet->isDeletable = isDeletable;

            dataSet->dataItems = LinkedList_size(variableListDirectory);

            dataSet->dataPoints = (Tase2_PointName) GLOBAL_MALLOC(dataSet->dataItems * sizeof(struct sTase2_PointName));

            dataSet->pointValues = (Tase2_PointValue) GLOBAL_MALLOC(dataSet->dataItems * sizeof(struct sTase2_PointValue));

            LinkedList variableListEntry = LinkedList_getNext(variableListDirectory);

            int i = 0;

            while (variableListEntry) {

                MmsVariableAccessSpecification* entrySpec = (MmsVariableAccessSpecification*) LinkedList_getData(variableListEntry);

                if (entrySpec->domainId != NULL)
                    strncpy(dataSet->dataPoints[i].domainName, entrySpec->domainId, 32);
                else
                    dataSet->dataPoints[i].domainName[0] = 0;

                strncpy(dataSet->dataPoints[i].pointName, entrySpec->itemId, 32);

                dataSet->pointValues[i].type = TASE2_VALUE_TYPE_UNKNOWN;
                dataSet->pointValues[i].pointValue = NULL;

                variableListEntry = LinkedList_getNext(variableListEntry);
                i++;
            }

            dataSet->pointValuesLock = Semaphore_create(1);

        }

        LinkedList_destroyDeep(variableListDirectory, (LinkedListValueDeleteFunction) MmsVariableAccessSpecification_destroy);
    }
    else {
        if (err)
            *err = convertMmsErrorToClientError(self->lastErrorCode);
    }

    return dataSet;
}

static MmsVariableAccessSpecification*
objectReferenceToVariableAccessSpec(char* objectReference)
{
    bool hasDomain = true;

    char* domainIdEnd = strchr(objectReference, '/');

    if (objectReference[0] == '/' || domainIdEnd == NULL) /* no domain name present */
        hasDomain = false;

    int domainIdLen;

    if (hasDomain) {
        domainIdLen = domainIdEnd - objectReference;

        if (domainIdLen > 32)
            return NULL;
    }

    MmsVariableAccessSpecification* accessSpec =
            (MmsVariableAccessSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableAccessSpecification));

    if (hasDomain)
        accessSpec->domainId = StringUtils_createStringFromBuffer((uint8_t*) objectReference, domainIdLen);
    else
        accessSpec->domainId = NULL;

    accessSpec->arrayIndex = -1; /* -1 --> not present */

    char* itemId;

    if (domainIdEnd == NULL)
        itemId = objectReference;
    else
        itemId = domainIdEnd + 1;

    char* itemIdEnd = strchr(itemId, '.');

    if (itemIdEnd) {
        accessSpec->itemId = StringUtils_copySubString(itemId, itemIdEnd);

        if (*(itemIdEnd + 1) != 0) {
            accessSpec->componentName = StringUtils_copyString(itemIdEnd + 1);
        }
        else
            accessSpec->componentName = NULL;
    }
    else {
        accessSpec->itemId = StringUtils_copyString(itemId);
        accessSpec->componentName = NULL;
    }

    return accessSpec;
}

bool
Tase2_Client_createDataSet(Tase2_Client self, Tase2_ClientError* err,
        const char* domainName, const char* dataSetName,
        LinkedList /* <char*> */dataSetElements)
{
    TASE2_LIB_CHECK_RUNTIME();

    bool retVal = false;

    char domainIdBuffer[65];
    char itemIdBuffer[DATA_SET_MAX_NAME_LENGTH + 1];

    MmsError mmsError;

    LinkedList dataSetEntries = LinkedList_create();

    LinkedList dataSetElement = LinkedList_getNext(dataSetElements);

    while (dataSetElement != NULL) {

        MmsVariableAccessSpecification* dataSetEntry =
                objectReferenceToVariableAccessSpec((char*) dataSetElement->data);

        if (dataSetEntry == NULL) {

            if (err)
                *err = TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

            goto cleanup_list;
        }

        LinkedList_add(dataSetEntries, (void*) dataSetEntry);

        dataSetElement = LinkedList_getNext(dataSetElement);
    }

    MmsClient_defineNamedVariableList(self->mmsClient, &mmsError,
            domainName, dataSetName, dataSetEntries);

    self->lastErrorCode = mmsError;

    if (mmsError == MMS_ERROR_NONE) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;

        retVal = true;
    }
    else {
        if (err)
            *err = convertMmsErrorToClientError(mmsError);
    }

cleanup_list:
    /* delete list and all elements */
    LinkedList_destroyDeep(dataSetEntries, (LinkedListValueDeleteFunction) MmsVariableAccessSpecification_destroy);

exit_function:
    return retVal;
}

bool
Tase2_Client_deleteDataSet(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* dataSetName)
{
    TASE2_LIB_CHECK_RUNTIME();

    bool isDeleted = false;

    MmsError mmsError;

    isDeleted = MmsClient_deleteNamedVariableList(self->mmsClient, &mmsError, domainName, dataSetName);

    self->lastErrorCode = mmsError;

    if (mmsError == MMS_ERROR_NONE) {
        if (err)
            *err = TASE2_CLIENT_ERROR_OK;
    }
    else {
        if (err)
            *err = convertMmsErrorToClientError(mmsError);
    }

exit_function:
    return isDeleted;
}

const char*
Tase2_ClientDataSet_getDomainName(Tase2_ClientDataSet self)
{
    return self->domainName;
}

const char*
Tase2_ClientDataSet_getDataSetName(Tase2_ClientDataSet self)
{
    return self->dataSetName;
}

bool
Tase2_ClientDataSet_isDeletable(Tase2_ClientDataSet self)
{
    return self->isDeletable;
}

int
Tase2_ClientDataSet_getSize(Tase2_ClientDataSet self)
{
    return self->dataItems;
}

Tase2_PointName
Tase2_ClientDataSet_getPointName(Tase2_ClientDataSet self, int index)
{
    if (index >= self->dataItems)
        return NULL;
    else
        return &(self->dataPoints[index]);
}

const char*
Tase2_ClientDataSet_getPointDomainName(Tase2_ClientDataSet self, int index)
{
    if (self->dataPoints[index].domainName[0] == 0)
        return NULL;
    else
        return self->dataPoints[index].domainName;
}

const char*
Tase2_ClientDataSet_getPointVariableName(Tase2_ClientDataSet self, int index)
{
    if (self->dataPoints[index].pointName[0] == 0)
        return NULL;
    else
        return self->dataPoints[index].pointName;
}

Tase2_PointValue
Tase2_ClientDataSet_getPointValue(Tase2_ClientDataSet self, int index)
{
    if ((index < 0) || (index >= self->dataItems)) {
        tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "DataSet index out of range");
        return NULL;
    }

    Tase2_PointValue pointValue = NULL;

    Semaphore_wait(self->pointValuesLock);

    if (self->pointValues[index].pointValue != NULL)
        pointValue = Tase2_PointValue_getCopy(&(self->pointValues[index]));

    Semaphore_post(self->pointValuesLock);

    return pointValue;
}

Tase2_ClientError
Tase2_ClientDataSet_read(Tase2_ClientDataSet self, Tase2_Client client)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_ClientError err =  TASE2_CLIENT_ERROR_FAILED;

    char* domainName;

    if (self->domainName[0] == 0)
        domainName = NULL;
    else
        domainName = self->domainName;

    MmsValue* values = MmsClient_readNamedVariableListValues(client->mmsClient, &(client->lastErrorCode), domainName, self->dataSetName, false);

    if (values) {

        if ((MmsValue_getType(values) == MMS_ARRAY) && (MmsValue_getArraySize(values) == self->dataItems)) {

            Semaphore_wait(self->pointValuesLock);

            int i;

            for (i = 0; i < self->dataItems; i++) {

                if (self->pointValues[i].pointValue == NULL)
                    self->pointValues[i].type = determineTypeFromMmsValue(MmsValue_getElement(values, i));
                else
                    MmsValue_delete(self->pointValues[i].pointValue);

                self->pointValues[i].pointValue = MmsValue_getElement(values, i);

                MmsValue_setElement(values, i, NULL);
            }

            Semaphore_post(self->pointValuesLock);

            MmsValue_deleteIfNotNull(values);

            err = TASE2_CLIENT_ERROR_OK;
        }
        else
            MmsValue_delete(values);

    }

    return err;
}

void
Tase2_ClientDataSet_destroy(Tase2_ClientDataSet self)
{
    if (self) {

        GLOBAL_FREEMEM(self->dataPoints);

        int i;

        for (i = 0; i < self->dataItems; i++)
            MmsValue_deleteIfNotNull(self->pointValues[i].pointValue);


        GLOBAL_FREEMEM(self->pointValues);

        Semaphore_destroy(self->pointValuesLock);

        GLOBAL_FREEMEM(self);
    }
}

void
Tase2_Client_IMTransferSetEnable(Tase2_Client self, Tase2_ClientError* err)
{
    MmsValue* value = MmsValue_newBoolean(true);

    MmsDataAccessError dataAccessErr = MmsClient_writeVariableAASpecific(self->mmsClient, &(self->lastErrorCode), "IM_Transfer_Set", value);

    MmsValue_delete(value);

    if (err)
        *err = convertDataAccessErrorToClientError(dataAccessErr);
}

void
Tase2_Client_IMTransferSetDisable(Tase2_Client self, Tase2_ClientError* err)
{
    MmsValue* value = MmsValue_newBoolean(false);

    MmsDataAccessError dataAccessErr = MmsClient_writeVariableAASpecific(self->mmsClient, &(self->lastErrorCode), "IM_Transfer_Set", value);

    MmsValue_delete(value);

    if (err)
        *err = convertDataAccessErrorToClientError(dataAccessErr);
}

void
Tase2_FreeMemory(void* ptr)
{
    GLOBAL_FREEMEM(ptr);
}

