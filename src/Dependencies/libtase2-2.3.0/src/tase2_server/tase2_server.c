/*
 *  tase2_server.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "mms_server.h"
#include "hal_thread.h"
#include "string_utilities.h"
#include "tase2_server.h"
#include "tase2_model.h"
#include "lib_memory.h"
#include "hash_tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ds_transfer_set.h"
#include "tase2_server_internal.h"

#include "tase2_runtime_check.h"

#ifndef TASE2_LIB_CHECK_RUNTIME
#define TASE2_LIB_CHECK_RUNTIME() {do {} while(0);}
#endif

#define MAX_MMS_NAME_LENGTH 32

static MmsVariableSpecification*
createIntVariable(const char* name, int bitSize)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(name);
    varSpec->type = MMS_INTEGER;
    varSpec->typeSpec.integer = bitSize;

    return varSpec;
}

static MmsVariableSpecification*
createInt16Variable(const char* name)
{
    return createIntVariable(name, 16);
}

static MmsVariableSpecification*
createUIntVariable(const char* name, int bitSize)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(name);
    varSpec->type = MMS_UNSIGNED;
    varSpec->typeSpec.unsignedInteger = bitSize;

    return varSpec;
}

static MmsVariableSpecification*
createUInt8Variable(const char* name)
{
    return createUIntVariable(name, 8);
}

static MmsVariableSpecification*
createVisibleStringVariable(const char* name, int size)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(name);
    varSpec->type = MMS_VISIBLE_STRING;
    varSpec->typeSpec.visibleString = size;

    return varSpec;
}

static MmsVariableSpecification*
createBitStringVariable(const char* name, int bitSize)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(name);
    varSpec->type = MMS_BIT_STRING;
    varSpec->typeSpec.bitString = bitSize;

    return varSpec;
}

static MmsVariableSpecification*
createBooleanVariable(const char* name)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(name);
    varSpec->type = MMS_BOOLEAN;

    return varSpec;
}

static MmsVariableSpecification*
createDataReal(const char* name)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(name);
    varSpec->type = MMS_FLOAT;
    varSpec->typeSpec.floatingpoint.formatWidth = 32;
    varSpec->typeSpec.floatingpoint.exponentWidth = 8;

    return varSpec;
}

static MmsVariableSpecification*
createDataFlags(const char* name)
{
    return createBitStringVariable(name, 8);
}

static MmsVariableSpecification*
createMmsObjectName(const char* name)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(name);

    varSpec->type = MMS_STRUCTURE;

    varSpec->typeSpec.structure.elementCount = 3;
    varSpec->typeSpec.structure.elements = (MmsVariableSpecification**) GLOBAL_CALLOC(3, sizeof(MmsVariableSpecification*));

    varSpec->typeSpec.structure.elements[0] = createUInt8Variable("Scope");
    varSpec->typeSpec.structure.elements[1] = createVisibleStringVariable("DomainName", -32);
    varSpec->typeSpec.structure.elements[2] = createVisibleStringVariable("Name", -32);

    return varSpec;
}


static MmsVariableSpecification*
createTimeStampExtended(const char* name)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));

    varSpec->name = StringUtils_copyString(name);
    varSpec->type = MMS_STRUCTURE;
    varSpec->typeSpec.structure.elementCount = 2;
    varSpec->typeSpec.structure.elements = (MmsVariableSpecification** )GLOBAL_CALLOC(2, sizeof(MmsVariableSpecification*));

    varSpec->typeSpec.structure.elements[0] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->typeSpec.structure.elements[0]->name = StringUtils_copyString("GMTBasedS");
    varSpec->typeSpec.structure.elements[0]->type = MMS_INTEGER;
    varSpec->typeSpec.structure.elements[0]->typeSpec.integer = 32;

    varSpec->typeSpec.structure.elements[1] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->typeSpec.structure.elements[1]->name = StringUtils_copyString("Milliseconds");
    varSpec->typeSpec.structure.elements[1]->type = MMS_INTEGER;
    varSpec->typeSpec.structure.elements[1]->typeSpec.integer = 16;

    return varSpec;
}

static MmsVariableSpecification*
createDSTransferSet(const char* name, Tase2_Edition edition)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(name);

    varSpec->type = MMS_STRUCTURE;

    int numberOfElements = 13;
    int element = 0;

    if (edition == TASE2_EDITION_1996_08) {
        numberOfElements = 12;
    }

    varSpec->typeSpec.structure.elementCount = numberOfElements;
    varSpec->typeSpec.structure.elements = (MmsVariableSpecification**) GLOBAL_CALLOC(numberOfElements, sizeof(MmsVariableSpecification*));

    varSpec->typeSpec.structure.elements[element++] = createMmsObjectName("DataSetName");
    varSpec->typeSpec.structure.elements[element++] = createIntVariable("StartTime", 32);
    varSpec->typeSpec.structure.elements[element++] = createIntVariable("Interval", 32);
    varSpec->typeSpec.structure.elements[element++] = createIntVariable("TLE", 32);
    varSpec->typeSpec.structure.elements[element++] = createIntVariable("BufferTime", 32);
    varSpec->typeSpec.structure.elements[element++] = createIntVariable("IntegrityCheck", 32);
    varSpec->typeSpec.structure.elements[element++] = createBitStringVariable("DSConditionsRequested", 5);
    varSpec->typeSpec.structure.elements[element++] = createBooleanVariable("BlockData");
    varSpec->typeSpec.structure.elements[element++] = createBooleanVariable("Critical");
    varSpec->typeSpec.structure.elements[element++] = createBooleanVariable("RBE");

    if (edition != TASE2_EDITION_1996_08) {
        varSpec->typeSpec.structure.elements[element++] = createBooleanVariable("AllChangesReported");
    }

    varSpec->typeSpec.structure.elements[element++] = createBooleanVariable("Status");
    varSpec->typeSpec.structure.elements[element++] = createInt16Variable("EventCodeRequested");

    return varSpec;
}

static MmsVariableSpecification*
createTase2Version()
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_MALLOC(sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString("TASE2_Version");

    varSpec->type = MMS_STRUCTURE;

    varSpec->typeSpec.structure.elementCount = 2;
    varSpec->typeSpec.structure.elements = (MmsVariableSpecification**) GLOBAL_CALLOC(2, sizeof(MmsVariableSpecification*));

    varSpec->typeSpec.structure.elements[0] = createInt16Variable("MajorVersionNumber");
    varSpec->typeSpec.structure.elements[1] = createInt16Variable("MinorVersionNumber");

    return varSpec;
}

static MmsVariableSpecification*
createSupportedFeatures(const char* name)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_MALLOC(sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(name);
    varSpec->type = MMS_BIT_STRING;
    varSpec->typeSpec.bitString = 12;

    return varSpec;
}

static MmsVariableSpecification*
createBilateralTableId()
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString("Bilateral_Table_ID");
    varSpec->type = MMS_VISIBLE_STRING;
    varSpec->typeSpec.visibleString = 32;

    return varSpec;
}

static MmsVariableSpecification*
createInfoHeader()
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString("Info_Mess_Header");

    varSpec->type = MMS_STRUCTURE;

    varSpec->typeSpec.structure.elementCount = 4;
    varSpec->typeSpec.structure.elements = (MmsVariableSpecification**) GLOBAL_CALLOC(4, sizeof(MmsVariableSpecification*));

    varSpec->typeSpec.structure.elements[0] = createIntVariable("InfoReference", 32);
    varSpec->typeSpec.structure.elements[1] = createIntVariable("LocalReference", 32);
    varSpec->typeSpec.structure.elements[2] = createIntVariable("MessageId", 32);
    varSpec->typeSpec.structure.elements[3] = createIntVariable("Size", 32);

    return varSpec;
}

static MmsVariableSpecification*
createInfoBuf(int size)
{
    char infoBufferName[65];

    sprintf(infoBufferName, "Info_Buff_%d", size);

    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->name = (char*) StringUtils_copyString(infoBufferName);

    varSpec->type = MMS_OCTET_STRING;
    varSpec->typeSpec.octetString = size;

    return varSpec;
}

static MmsValue*
createNewTase2VersionValue(int16_t major, int16_t minor)
{
    MmsValue* value = MmsValue_createEmptyStructure(2);

    MmsValue_setElement(value, 0, MmsValue_newIntegerFromInt16(major));
    MmsValue_setElement(value, 1, MmsValue_newIntegerFromInt16(minor));

    return value;
}

void
tase2_ControlPoint_setTagValue(Tase2_ControlPoint cp, Tase2_TagValue tagValue)
{
    if (cp->hasTag) {

        MmsValue* mmsTagValue = MmsValue_getElement(cp->tagValue, 0);

        MmsValue_setBitStringBit(mmsTagValue, 1, (tagValue & 1) != 0);
        MmsValue_setBitStringBit(mmsTagValue, 0, (tagValue & 2) != 0);

    }
}

/**
 * \brief returns the tag value of a tagged control point object
 */
Tase2_TagValue
tase2_ControlPoint_getTagValue(Tase2_ControlPoint cp)
{
    Tase2_TagValue tagValue = TASE2_TAG_NO_TAG;

    if (cp->hasTag) {

        MmsValue* mmsTagValue = MmsValue_getElement(cp->tagValue, 0);

        int intVal = 0;

        if (MmsValue_getBitStringBit(mmsTagValue, 0))
            intVal += 2;

        if (MmsValue_getBitStringBit(mmsTagValue, 1))
            intVal += 1;


        tagValue = (Tase2_TagValue) intVal;

    }

    return tagValue;
}

static bool
isArmed(Tase2_Server self, Tase2_ControlPoint cp, uint64_t currentTime)
{
    if (currentTime - cp->selectedTimestamp > self->selectTimeout)
        return false;
    else
        return true;
}

void
tase2_ControlPoint_setArmedFlag(Tase2_ControlPoint cp, bool value)
{
    if (cp->hasTag) {
        MmsValue* flags = MmsValue_getElement(cp->tagValue, 0);

        MmsValue_setBitStringBit(flags, 2, value);
    }
}

static MmsDataAccessError
selectControl(Tase2_Server self, Tase2_ControlPoint cp, MmsEndpointConnection connection)
{
    uint64_t currentTime = Hal_getTimeInMs();

    if (isArmed(self, cp, currentTime)) {
        return DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE;
    }
    else {
        if (tase2_ControlPoint_getTagValue(cp) == TASE2_TAG_OPEN_AND_CLOSE_INHIBIT)
            return DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE;

        if (self->selectHandler != NULL) {
            Tase2_HandlerResult result = self->selectHandler(self->selectHandlerParameter, cp);

            if (result != TASE2_RESULT_SUCCESS)
                return (MmsDataAccessError) result;
        }

        cp->selectedTimestamp = currentTime;
        cp->selectedBy = (void*) connection;

        tase2_ControlPoint_setArmedFlag(cp, true);

    }

    return DATA_ACCESS_ERROR_SUCCESS;
}

static MmsDataAccessError
operateControl(Tase2_Server self, Tase2_ControlPoint cp, MmsEndpointConnection connection, MmsValue* value)
{
    Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

    tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "operate %s", cp->base.pointName);

    Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

    uint64_t currentTime = Hal_getTimeInMs();

    if (cp->deviceClass == TASE2_DEVICE_CLASS_SBO) {

        if (isArmed(self, cp, currentTime) == false)
            return DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE;
        else {
            cp->selectedTimestamp = 0;
            tase2_ControlPoint_setArmedFlag(cp, false);
        }
    }

    if (cp->pointType == TASE2_CONTROL_TYPE_COMMAND) {

        int commandValue = MmsValue_toInt32(value);

        /* Check for valid command value */
        if ((commandValue != TASE2_COMMAND_CLOSE) && (commandValue != TASE2_COMMAND_TRIP))
            return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

        /* If tag is available check if tag value inhibits command execution */
        if (cp->hasTag) {

            Tase2_TagValue tagValue = tase2_ControlPoint_getTagValue(cp);

            if (tagValue == TASE2_TAG_OPEN_AND_CLOSE_INHIBIT)
                return DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE;

            if (tagValue == TASE2_TAG_CLOSE_ONLY_INHIBIT) {

                if (commandValue == TASE2_COMMAND_CLOSE)
                    return DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE;
            }

        }

    }

    if (self->operateHandler != NULL) {
        Tase2_OperateValue operValue;

        if (MmsValue_getType(value) == MMS_INTEGER)
            operValue.discreteValue = (int16_t) MmsValue_toInt32(value);
        else if (MmsValue_getType(value) == MMS_FLOAT)
            operValue.realValue = MmsValue_toFloat(value);

        Tase2_HandlerResult result = self->operateHandler(self->operateHandlerParameter, cp, operValue);

        if (result != TASE2_RESULT_SUCCESS)
            return (MmsDataAccessError) result;
    }
    else if (self->operateHandlerEx != NULL) {
        int16_t discreteValue = 0;
        float realValue = 0.f;

        if (MmsValue_getType(value) == MMS_INTEGER)
            discreteValue = (int16_t) MmsValue_toInt32(value);
        else if (MmsValue_getType(value) == MMS_FLOAT)
            realValue = MmsValue_toFloat(value);

        Tase2_HandlerResult result = self->operateHandlerEx(self->operateHandlerParameter, cp, discreteValue, realValue);

        if (result != TASE2_RESULT_SUCCESS)
            return (MmsDataAccessError) result;
    }

    MmsValue_update(cp->base.value.pointValue, value);
    MmsValue_update(cp->base.serverCachedValue, value);
    Tase2_Server_updateOnlineValue(self, (Tase2_DataPoint)cp);

    return DATA_ACCESS_ERROR_SUCCESS;
}

Tase2_TagValue
getTagValue(MmsValue* value)
{
    MmsValue* mmsTagValue = MmsValue_getElement(value, 0);

    int intVal = 0;

    if (MmsValue_getBitStringBit(mmsTagValue, 0))
        intVal += 2;

    if (MmsValue_getBitStringBit(mmsTagValue, 1))
        intVal += 1;

    return (Tase2_TagValue) intVal;
}

static MmsDataAccessError
setTag(Tase2_Server self, Tase2_ControlPoint cp, MmsEndpointConnection connection, MmsValue* value)
{
    Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

    tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "set tag %s", cp->base.pointName);

    Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

    if (cp->hasTag == false)
        return DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT;

    Tase2_TagValue tagValue = getTagValue(value);

    if (tagValue != TASE2_TAG_INVALID) {

        if (isArmed(self, cp, Hal_getTimeInMs())) {
            return DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE;
        }

        Tase2_TagValue currentTagValue = tase2_ControlPoint_getTagValue(cp);

        Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

        if (currentTagValue != TASE2_TAG_NO_TAG) {

            if (blt != NULL) {
                if (blt != cp->tagOwner)  { /* tag was set locally or by other client */

                    peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

                    tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "tag was already set by other client");

                    Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

                    return DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE;
                }
            }
        }

        if (MmsValue_equals(cp->tagValue, value) == false) {

            if (self->setTagHandler) {

                const char* reason = MmsValue_toString(MmsValue_getElement(value, 1));

                Tase2_HandlerResult result = self->setTagHandler(self->setTagHandlerParameter, cp, tagValue, reason);

                if (result != TASE2_RESULT_SUCCESS)
                    return (MmsDataAccessError) result;
            }

            MmsValue_update(cp->tagValue, value);

            cp->tagOwner = blt;

            /* trigger a change report when tag is part of a DS transfer set */
            tase2_Server_triggerTransferSets(self, (Tase2_DataPoint) cp);

        }

    }
    else
        return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;

    return DATA_ACCESS_ERROR_SUCCESS;
}

struct sMmsVariableSpecification imTransferSetVarSpec;

static MmsVariableSpecification*
mmsAASpecVarSpecHandler (void* parameter, const char* variableName, MmsEndpointConnection connection)
{
    if (!strcmp(variableName, "IM_Transfer_Set")) {
        imTransferSetVarSpec.name = (char*) "IM_Transfer_Set";
        imTransferSetVarSpec.type = MMS_BOOLEAN;

        /* TODO check if connection is allowed according to BLT */

        return &imTransferSetVarSpec;
    }
    else
        return NULL;
}

static MmsValue*
mmsReadVariableHandler(void* parameter, MmsDomain* domain, char* variableId, MmsEndpointConnection connection)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Server self = (Tase2_Server) parameter;

    Tase2_Domain tase2Domain = NULL;

    /* create a local copy of the variable ID */
    char variableIdBuf[100];

    int c = 0;
    while (variableId[c] != 0) {
        if (c == 99)
            break;

        variableIdBuf[c] = variableId[c];

        c++;
    }

    int variableIdLen = c;
    variableIdBuf[c] = 0;
    variableId = variableIdBuf;

    Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

    {
        Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Read access to (%s):%s", domain == NULL ? "" : domain->domainName, variableId);

        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);
    }

    if (domain == NULL)
        tase2Domain = self->dataModel->vcc;
    else {

        Tase2_Domain nextDomain = self->dataModel->domains;

        while (nextDomain != NULL) {
            /* Can be replaced by pointer comparison ???? */
            if (!strcmp(nextDomain->name, domain->domainName)) {
                tase2Domain = nextDomain;
                break;
            }

            nextDomain = nextDomain->sibling;
        }

        if (blt) {
            if (blt->domain != tase2Domain) {

                Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

                tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Read access denied - wrong domain for BLT ID: %s", blt->bltId);

                Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

                MmsValue* errorValue = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED);
                MmsValue_setDeletable(errorValue);

                return errorValue;
            }
        }
    }

    if (tase2Domain != NULL) {
        /* search requested variable */

        Tase2_DataPoint dataPoint = NULL;

        /* Check if it is a tag or SBO */
        int variableIdLen = strlen(variableId);

        bool isTag = false;
        bool isSBO = false;

        if (variableIdLen > 4) {
            if (variableId[variableIdLen - 4] == '_') {

                if (StringUtils_endsWith(variableId, "_SBO")) {
                    isSBO = true;
                }
                else if (StringUtils_endsWith(variableId, "_TAG")) {
                    isTag = true;
                }

                if (isTag || isSBO) {
                    variableId[variableIdLen - 4] = 0;
                }
            }
        }

        dataPoint = HashTree_find((HashTree)tase2Domain->dataPointHashTree, variableId);

        if (dataPoint) {
            if (blt) {

                bool checkResult = false;

                if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL)
                    checkResult = BilateralTable_checkControlPointAccess(blt, tase2Domain, (Tase2_ControlPoint) dataPoint, BLT_ACCESS_ALLOW_READ);
                else
                    checkResult = BilateralTable_checkDataPointAccess(blt, tase2Domain, dataPoint, BLT_ACCESS_ALLOW_READ);

                if (checkResult == false) {

                    Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

                    tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Read access - object access denied by BLT");

                    Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

                    MmsValue* errorValue = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED);
                    MmsValue_setDeletable(errorValue);

                    return errorValue;
                }

            }

            if (isSBO) {
                if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {
                    Tase2_ControlPoint cp = (Tase2_ControlPoint) dataPoint;

                    if (cp->sboValue == NULL) {

                        Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

                        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Invalid select access to control %s:%s", cp->base.domain->name ? cp->base.domain->name : "VCC", variableId);

                        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

                        MmsValue* errorValue = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT);
                        MmsValue_setDeletable(errorValue);

                        return errorValue;
                    }

                    if (blt) {

                        if (BilateralTable_checkControlPointAccess(blt, tase2Domain, cp, BLT_ACCESS_ALLOW_OPERATE) == false) {

                            MmsValue* errorValue = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED);
                            MmsValue_setDeletable(errorValue);

                            return errorValue;
                        }
                        else {
                            int16_t checkBackId = BilateralTable_getCheckBackId(blt, tase2Domain, cp);

                            MmsValue_setInt16(cp->sboValue, checkBackId);
                        }

                    }

                    MmsDataAccessError selectResult = selectControl(self, cp, connection);

                    if (selectResult == DATA_ACCESS_ERROR_SUCCESS)
                        return cp->sboValue;
                    else {
                        MmsValue* errorValue = MmsValue_newDataAccessError(selectResult);
                        MmsValue_setDeletable(errorValue);

                        return errorValue;
                    }
                }
                else {
                    /* error - data point is not a control! */

                    MmsValue* errorValue = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_OBJECT_ACCESS_UNSUPPORTED);
                    MmsValue_setDeletable(errorValue);

                    return errorValue;
                }
            }
            else if (isTag) {
                if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {
                    Tase2_ControlPoint cp = (Tase2_ControlPoint) dataPoint;

                    if (blt) {

                        if (BilateralTable_checkControlPointAccess(blt, tase2Domain, cp, BLT_ACCESS_ALLOW_GET_TAG) == false) {

                            MmsValue* errorValue = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED);
                            MmsValue_setDeletable(errorValue);

                            return errorValue;
                        }

                    }

                    if (cp->selectedTimestamp != 0) {
                        if (isArmed(self, cp, Hal_getTimeInMs()) == false) {
                            cp->selectedTimestamp = 0;
                            tase2_ControlPoint_setArmedFlag(cp, false);
                        }
                    }

                    return cp->tagValue;
                }
                else {
                    /* error - data point is not a control! */


                    MmsValue* errorValue = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_OBJECT_ACCESS_UNSUPPORTED);
                    MmsValue_setDeletable(errorValue);

                    return errorValue;
                }
            }
            else {
                return dataPoint->value.pointValue;
            }
        }

        if (!strcmp(variableId, "Bilateral_Table_ID")) {
            MmsValue* bltVal = MmsValue_newVisibleString(tase2Domain->bilateralTableId);
            MmsValue_setDeletable(bltVal);

            return bltVal;
        };

        if (!strcmp(variableId, "Next_DSTransfer_Set")) {
            Tase2_DSTransferSet dsTs =
                    tase2_Server_getNextDSTransferSet(self, tase2Domain, connection);

            if (dsTs == NULL) {
                MmsValue* errorValue = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE);
                MmsValue_setDeletable(errorValue);

                return errorValue;
            }
            else {
                MmsValue* retValue = tase2_createObjectNameValue(tase2Domain->name, dsTs->base.name);
                MmsValue_setDeletableRecursive(retValue);

                return retValue;
            }
        }

        /* search transfer sets */

        Tase2_TransferSet transferSet = tase2Domain->transferSets;;

        while (transferSet != NULL) {

            if (!strcmp(transferSet->name, variableId)) {
                if (transferSet->transferSetType == TASE2_TS_TYPE_DATA_SET)
                    return transferSet->mmsValue;
            }

            transferSet = transferSet->sibling;
        }
    }

    {
        Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Read access - data point not found");

        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);
    }

    MmsValue* errorValue = MmsValue_newDataAccessError(DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED);
    MmsValue_setDeletable(errorValue);

    return errorValue;
}

static Tase2_PointValueType
getPointValueType(Tase2_DataPoint dataPoint)
{
    Tase2_PointValueType type;

    if (dataPoint->pointType == TASE2_POINT_TYPE_INDICATION) {
        Tase2_IndicationPoint ip = (Tase2_IndicationPoint)dataPoint;

        if (ip->pointType == TASE2_IND_POINT_TYPE_DISCRETE)
            type = TASE2_VALUE_TYPE_DISCRETE;
        else if (ip->pointType == TASE2_IND_POINT_TYPE_REAL)
            type = TASE2_VALUE_TYPE_REAL;
        else if (ip->pointType == TASE2_IND_POINT_TYPE_STATE)
                   type = TASE2_VALUE_TYPE_STATE;
        else if (ip->pointType == TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL)
                   type = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
    }
    else if (dataPoint->pointType == TASE2_POINT_TYPE_PROTECTION_EVENT) {
        Tase2_ProtectionEquipment pe = (Tase2_ProtectionEquipment)dataPoint;

        if (pe->isPacked)
            type = TASE2_VALUE_TYPE_PACKED_EVENT;
        else
            type = TASE2_VALUE_TYPE_SINGLE_EVENT;
    }

    return type;
}


bool
tase2_server_IsIMTransferSetEnabled(Tase2_Server self, MmsEndpointConnection connection)
{
    bool retVal;

    Semaphore_wait(self->imEnabledConnectionsLock);
    retVal = LinkedList_contains(self->imEnabledConnections, (void*) connection);
    Semaphore_post(self->imEnabledConnectionsLock);
    return retVal;
}

void
tase2_server_EnableIMTransferSet(Tase2_Server self, MmsEndpointConnection connection, Tase2_Endpoint_Connection peer)
{
    tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "enable IM transfer set");

    Semaphore_wait(self->imEnabledConnectionsLock);
    LinkedList_add(self->imEnabledConnections, connection);
    Semaphore_post(self->imEnabledConnectionsLock);

    if (self->imEventHandler) {
        Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

        self->imEventHandler(self->imEventHandlerParameter, peer, blt, true);
    }
}

void
tase2_server_DisableIMTransferSet(Tase2_Server self, MmsEndpointConnection connection, Tase2_Endpoint_Connection peer)
{
    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "disable IM transfer set");

    Semaphore_wait(self->imEnabledConnectionsLock);
    LinkedList_remove(self->imEnabledConnections, connection);
    Semaphore_post(self->imEnabledConnectionsLock);

    if (self->imEventHandler) {
        Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

        self->imEventHandler(self->imEventHandlerParameter, peer, blt, false);
    }
}

static MmsDataAccessError
mmsWriteVariableHandler(void* parameter, MmsObjectScope objectScope,
        MmsDomain* domain, const char* variableId, const char* componentName, MmsValue* value,
        MmsEndpointConnection connection)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Server self = (Tase2_Server) parameter;

    Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

    char valueStr[256];

    if (objectScope == MMS_ASSOCIATION_SPECIFIC) {

        if (!strcmp(variableId, "IM_Transfer_Set")) {
            bool enable = MmsValue_getBoolean(value);

            Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Activate IM_Transfer_Set: %s", enable ? "true" : "false");

            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

            if (enable) {
                if (tase2_server_IsIMTransferSetEnabled(self, connection) == false)
                    tase2_server_EnableIMTransferSet(self, connection, peer);
            }
            else {
                tase2_server_DisableIMTransferSet(self, connection, peer);
            }

            return DATA_ACCESS_ERROR_SUCCESS;
        }
        else {
            Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

            tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Write access to unknown association specific variable %s", variableId);

            Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

            return DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT;
        }
    }
    else {
        Tase2_Domain tase2Domain = NULL;

        int variableIdLen = -1;

        MmsValue_printToBuffer(value, valueStr, sizeof(valueStr));

        Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

        if (componentName)
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "write access to %s:%s.%s: %s", domain == NULL ? NULL : domain->domainName, variableId, componentName, valueStr);
        else
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "write access to %s:%s: %s", domain == NULL ? NULL : domain->domainName, variableId, valueStr);

        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

        if (domain == NULL)
           tase2Domain = self->dataModel->vcc;
        else {

           Tase2_Domain nextDomain = self->dataModel->domains;

           while (nextDomain != NULL) {
               if (!strcmp(nextDomain->name, domain->domainName)) {
                   tase2Domain = nextDomain;
                   break;
               }

               nextDomain = nextDomain->sibling;
           }

           if (blt) {
               if (blt->domain != tase2Domain)
                   return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
           }

        }

        if (tase2Domain) {
            /* search requested variable */

            Tase2_DataPoint dataPoint = HashTree_find(tase2Domain->dataPointHashTree, variableId);

            if (dataPoint) {
                if (componentName != NULL)
                    return DATA_ACCESS_ERROR_OBJECT_ACCESS_UNSUPPORTED;

                if (dataPoint->pointType == TASE2_POINT_TYPE_INDICATION) {

                    Tase2_IndicationPoint indPoint = (Tase2_IndicationPoint) dataPoint;

                    if (indPoint->readOnly) {
                        return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
                    }
                    else {

                        if (blt) {

                            if (BilateralTable_checkDataPointAccess(blt, tase2Domain, dataPoint, BLT_ACCESS_ALLOW_WRITE) == false) {

                                Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

                                tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "write access denied to %s:%s", domain == NULL ? NULL : domain->domainName, variableId);

                                Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

                                return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
                            }

                        }

                        MmsDataAccessError retCode = DATA_ACCESS_ERROR_SUCCESS;

                        struct sTase2_PointValue _pointValue;

                        _pointValue.pointValue = value;
                        _pointValue.type = getPointValueType(dataPoint);

                        if (self->writeDataHandler != NULL) {
                            retCode = (MmsDataAccessError) self->writeDataHandler(self->writeDataHandlerParameter,
                                    tase2Domain, dataPoint, &_pointValue);
                        }

                        if (retCode == DATA_ACCESS_ERROR_SUCCESS)
                            MmsValue_update(dataPoint->value.pointValue, value);

                        return retCode;
                    }
                }
                else if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {

                    if (blt) {

                        if (BilateralTable_checkControlPointAccess(blt, tase2Domain, (Tase2_ControlPoint) dataPoint, BLT_ACCESS_ALLOW_OPERATE) == false) {
                            return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
                        }

                    }

                    return operateControl(self, (Tase2_ControlPoint) dataPoint, connection, value);
                }
                else
                    return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;

            }
            else {
                /* check if requested variable is a tag */
                if (StringUtils_endsWith(variableId, "_TAG")) {

                    /* create a local copy of the variable ID */
                    char variableIdBuf[130];

                    int c = 0;
                    while (variableId[c] != 0) {
                        if (c == 129)
                            break;

                        variableIdBuf[c] = variableId[c];

                        c++;
                    }

                    int variableIdLen = c;
                    variableIdBuf[c] = 0;

                    if (variableIdLen > 4) {
                        /* strip tag part from variable name */
                        variableIdBuf[variableIdLen - 4] = 0;

                        dataPoint = HashTree_find(tase2Domain->dataPointHashTree, variableIdBuf);

                        if (dataPoint) {

                            if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {

                                Tase2_ControlPoint cp = (Tase2_ControlPoint) dataPoint;

                                if (cp->hasTag) {

                                    if (componentName != NULL)
                                        return DATA_ACCESS_ERROR_OBJECT_ACCESS_UNSUPPORTED;

                                    if (blt) {
                                        if (BilateralTable_checkControlPointAccess(blt, tase2Domain, cp, BLT_ACCESS_ALLOW_SET_TAG) == false) {
                                            return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
                                        }
                                    }

                                    return setTag(self, cp, connection, value);
                                }
                            }
                        }
                    }
                }
            }

            /* search transfer sets */

            Tase2_TransferSet transferSet = tase2Domain->transferSets;

            while (transferSet != NULL) {

                if (!strcmp(transferSet->name, variableId)) {
                    if (transferSet->transferSetType == TASE2_TS_TYPE_DATA_SET) {
                        return tase2_Server_handleDSTransferSetWrite(self, domain, (Tase2_DSTransferSet) transferSet, componentName, value, connection, blt, peer);
                    }
                }

                transferSet = transferSet->sibling;
            }

        }

    } /* else/if (varType == MMS_ASSOCIATION_SPECIFIC) */

    return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
}

static MmsError
mmsNamedVariableListChangedHandler(void* parameter, bool create, MmsObjectScope listType, MmsNamedVariableList variableList, MmsEndpointConnection connection)
{
    MmsError retVal = MMS_ERROR_NONE;

    Tase2_Server self = (Tase2_Server) parameter;

    if (variableList == NULL)
        return MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED;

    MmsDomain* domain = variableList->domain;
    char* listName = variableList->name;

    Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

    if (peer) {
        Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

        Tase2_Domain tase2Domain = NULL;

        if (blt) {

            if (variableList->domain == NULL)
               retVal = MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED;
            else {

               Tase2_Domain nextDomain = self->dataModel->domains;

               while (nextDomain != NULL) {
                   if (!strcmp(nextDomain->name, variableList->domain->domainName)) {
                       tase2Domain = nextDomain;
                       break;
                   }

                   nextDomain = nextDomain->sibling;
               }

               if (blt) {
                   if (blt->domain != tase2Domain)
                       retVal = MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED;
               }

            }
        }

        if (retVal == MMS_ERROR_NONE) {
            if (self->dataSetChangedHandler) {

                LinkedList dataPoints = LinkedList_create();

                LinkedList mmsVariableElement = LinkedList_getNext(variableList->listOfVariables);

                while (mmsVariableElement) {

                    MmsNamedVariableListEntry entry = (MmsNamedVariableListEntry) LinkedList_getData(mmsVariableElement);

                    if (entry->domain) {

                        Tase2_Domain domain = Tase2_DataModel_getDomain(self->dataModel, entry->domain->domainName);

                        if (domain) {
                            Tase2_DataPoint dataPoint = Tase2_Domain_getDataPoint(domain, entry->variableName);

                             if (dataPoint) {
                                 LinkedList_add(dataPoints, dataPoint);
                             }
                        }
                    }
                    else {
                        Tase2_DataPoint dataPoint = Tase2_Domain_getDataPoint(self->dataModel->vcc, entry->variableName);

                        if (dataPoint) {
                            LinkedList_add(dataPoints, dataPoint);
                        }
                    }

                    mmsVariableElement = LinkedList_getNext(mmsVariableElement);
                }

                self->dataSetChangedHandler(self->dataSetChangedHandlerParameter, create, peer, blt, tase2Domain, variableList->name, dataPoints);

                LinkedList_destroyStatic(dataPoints);
            }
        }

        if (retVal == MMS_ERROR_NONE)
            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "create data set %s in domain %s", listName, domain == NULL ? "VCC" : domain->domainName);
        else
            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "create data set %s in domain %s failed --> object-access-denied", listName, domain == NULL ? "VCC" : domain->domainName);

        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);
    }

    return retVal;
}

void /* is called by MMS server layer via Tase2_Endpoint */
Tase2_Server_mmsConnectionHandler(Tase2_Server self, MmsEndpointConnection connection, Tase2_Endpoint_Connection peer, MmsEndpointEvent event)
{
    TASE2_LIB_CHECK_RUNTIME();

    if (event == MMS_ENDPOINT_EVENT_CONNECTION_CLOSED) {

        tase2_log(TASE2_LOG_ERROR, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "tase2-server: client connection closed");

        tase2_Server_deactivateTransferSets(self, connection, peer);

        tase2_server_DisableIMTransferSet(self, connection, peer);

        if (self->clientConnectionHandler) {

            Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

            char* clientAddress = MmsEndpointConnection_getPeerAddress(connection);

            self->clientConnectionHandler(self->clientConnectionHandlerParameter, clientAddress, blt, false);
        }

    }
    else if (event == MMS_ENDPOINT_EVENT_NEW_CONNECTION) {

        Tase2_Endpoint_Connection newPeer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, newPeer, "tase2-server: new client connection");

        Tase2_Endpoint_unlockEndpointConnection(self->endpoint, newPeer);

        MmsEndpointConnection_setServer(connection, self->mmsServer);

        if (self->clientConnectionHandler) {

            Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

            char* clientAddress = MmsEndpointConnection_getPeerAddress(connection);

            self->clientConnectionHandler(self->clientConnectionHandlerParameter, clientAddress, blt, true);
        }
    }
}

static int
getDomainCount(Tase2_DataModel dataModel)
{
    int domainCount = 0;

    Tase2_Domain nextDomain = dataModel->domains;

    while (nextDomain) {
        domainCount++;
        nextDomain = nextDomain->sibling;
    }

    return domainCount;
}

static int
getInfoMessageCount(Tase2_Domain domain)
{
    int count = 0;

    Tase2_InformationMessage im = domain->informationMessages;

    while (im) {
        count++;

        im = im->sibling;
    }

    return count;
}

static int
getVariablesCount(Tase2_Domain domain)
{
    int variablesCount = 0;

    Tase2_DataPoint nextPoint = domain->dataPoints;

    while (nextPoint) {

        variablesCount++;

        if (nextPoint->pointType == TASE2_POINT_TYPE_CONTROL) {
            Tase2_ControlPoint cp = (Tase2_ControlPoint) nextPoint;

            if (cp->hasTag)
                variablesCount++; /* add space for "_TAG" variable */

            if (cp->deviceClass == TASE2_DEVICE_CLASS_SBO)
                variablesCount++; /* add space for "_SBO" variable */
        }

        nextPoint = nextPoint->sibling;
    }

    Tase2_TransferSet nextTs = domain->transferSets;

    while (nextTs) {
        variablesCount++;

        nextTs = nextTs->sibling;
    }

    return variablesCount;
}

static void
CreateValueType(Tase2_IndicationPoint point, MmsVariableSpecification* varSpec)
{
    if (point->pointType == TASE2_IND_POINT_TYPE_REAL) {
        varSpec->type = MMS_FLOAT;
        varSpec->typeSpec.floatingpoint.formatWidth = 32;
        varSpec->typeSpec.floatingpoint.exponentWidth = 8;
    }
    else if (point->pointType == TASE2_IND_POINT_TYPE_DISCRETE) {
        varSpec->type = MMS_INTEGER;
        varSpec->typeSpec.integer = 32;
    }
    else if (point->pointType == TASE2_IND_POINT_TYPE_STATE) {
        varSpec->type = MMS_BIT_STRING;
        varSpec->typeSpec.bitString = 8;
    }
    else if (point->pointType == TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL) {
        varSpec->type = MMS_BIT_STRING;
        varSpec->typeSpec.bitString = 8;
    }
}

static void
CreateTimeStamp(MmsVariableSpecification* varSpec)
{
    varSpec->name = StringUtils_copyString("TimeStamp");
    varSpec->type = MMS_INTEGER;
    varSpec->typeSpec.integer = 32;
}

static void
CreateTimeStampExtended(const char* name, MmsVariableSpecification* varSpec)
{
    varSpec->name = StringUtils_copyString(name);
    varSpec->type = MMS_STRUCTURE;
    varSpec->typeSpec.structure.elementCount = 2;
    varSpec->typeSpec.structure.elements = (MmsVariableSpecification** )GLOBAL_CALLOC(2, sizeof(MmsVariableSpecification*));

    varSpec->typeSpec.structure.elements[0] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->typeSpec.structure.elements[0]->name = StringUtils_copyString("GMTBasedS");
    varSpec->typeSpec.structure.elements[0]->type = MMS_INTEGER;
    varSpec->typeSpec.structure.elements[0]->typeSpec.integer = 32;

    varSpec->typeSpec.structure.elements[1] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
    varSpec->typeSpec.structure.elements[1]->name = StringUtils_copyString("Milliseconds");
    varSpec->typeSpec.structure.elements[1]->type = MMS_INTEGER;
    varSpec->typeSpec.structure.elements[1]->typeSpec.integer = 16;
}

static int
createProtectionEventVariable(MmsDomain* domain, Tase2_ProtectionEquipment point, int variableIndex)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));

    varSpec->name = StringUtils_copyString(point->base.pointName);
    varSpec->type = MMS_STRUCTURE;

    int structSize = (point->isPacked) ? 4 : 3;

    varSpec->typeSpec.structure.elementCount = structSize;
    varSpec->typeSpec.structure.elements = (MmsVariableSpecification**) GLOBAL_CALLOC(structSize, sizeof(MmsVariableSpecification*));

    if (point->isPacked) {
        varSpec->typeSpec.structure.elements[0] = createBitStringVariable("PackedEvent", 8);
        varSpec->typeSpec.structure.elements[1] = createBitStringVariable("PackedEventFlags", 8);
        varSpec->typeSpec.structure.elements[2] = createIntVariable("OperatingTime", 16);
        varSpec->typeSpec.structure.elements[3] = createTimeStampExtended("EventTime");
    }
    else {
        varSpec->typeSpec.structure.elements[0] = createBitStringVariable("SingleEventFlags", 8);
        varSpec->typeSpec.structure.elements[1] = createIntVariable("OperatingTime", 16);
        varSpec->typeSpec.structure.elements[2] = createTimeStampExtended("EventTime");
    }

    MmsDomain_addNamedVariable(domain, variableIndex, varSpec);

    point->base.value.pointValue = MmsValue_newDefaultValue(varSpec);
    point->base.serverCachedValue = MmsValue_newDefaultValue(varSpec);
    point->base.valueLock = Semaphore_create(1);

    return (variableIndex + 1);
}

static int
createIndicationPointVariable(MmsDomain* domain, Tase2_IndicationPoint point, int variableIndex)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));

    varSpec->name = StringUtils_copyString(point->base.pointName);

    if ((point->hasCov == false) && (point->qualityClass == TASE2_NO_QUALITY) && (point->timeStampClass == TASE2_NO_TIMESTAMP)) {
        CreateValueType(point, varSpec);
    }
    else {
        varSpec->type = MMS_STRUCTURE;

        int structSize = 1;

        if (point->hasCov) {
            if (point->pointType == TASE2_IND_POINT_TYPE_STATE)
                structSize = 3;
            else
                structSize = 4;
        }
        else {

            if (point->qualityClass != TASE2_NO_QUALITY) {

                if ((point->pointType == TASE2_IND_POINT_TYPE_STATE) && (point->timeStampClass != TASE2_NO_TIMESTAMP))
                {
                    /* value and flags are merged in this case */
                }
                else
                    structSize += 1;
            }

            if (point->timeStampClass != TASE2_NO_TIMESTAMP)
                structSize += 1;
        }

        varSpec->typeSpec.structure.elementCount = structSize;
        varSpec->typeSpec.structure.elements = (MmsVariableSpecification**) GLOBAL_CALLOC(structSize, sizeof(MmsVariableSpecification*));

        int currentElementIndex = 0;

        if ((point->pointType == TASE2_IND_POINT_TYPE_STATE) && (point->timeStampClass != TASE2_NO_TIMESTAMP)) {
            /* value and flags are merged */
        }
        else {
            varSpec->typeSpec.structure.elements[currentElementIndex] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
            varSpec->typeSpec.structure.elements[currentElementIndex]->name = StringUtils_copyString("Value");
            CreateValueType(point, varSpec->typeSpec.structure.elements[currentElementIndex]);
            currentElementIndex += 1;
        }

        if (point->timeStampClass != TASE2_NO_TIMESTAMP) {
            varSpec->typeSpec.structure.elements[currentElementIndex] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));

            if (point->timeStampClass == TASE2_TIMESTAMP) {
                CreateTimeStamp(varSpec->typeSpec.structure.elements[currentElementIndex]);
            }
            else {
                CreateTimeStampExtended("TimeStamp", varSpec->typeSpec.structure.elements[currentElementIndex]);
            }
            currentElementIndex += 1;
        }

        if ((point->qualityClass != TASE2_NO_QUALITY) || (point->pointType == TASE2_IND_POINT_TYPE_STATE)) {
            varSpec->typeSpec.structure.elements[currentElementIndex] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
            varSpec->typeSpec.structure.elements[currentElementIndex]->name = StringUtils_copyString("Flags");
            varSpec->typeSpec.structure.elements[currentElementIndex]->type = MMS_BIT_STRING;
            varSpec->typeSpec.structure.elements[currentElementIndex]->typeSpec.bitString = 8;

            currentElementIndex += 1;
        }

        if (point->hasCov) {
            varSpec->typeSpec.structure.elements[currentElementIndex] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
            varSpec->typeSpec.structure.elements[currentElementIndex]->name = StringUtils_copyString("COV");
            varSpec->typeSpec.structure.elements[currentElementIndex]->type = MMS_UNSIGNED;
            varSpec->typeSpec.structure.elements[currentElementIndex]->typeSpec.unsignedInteger = 16;
        }

    }

    MmsDomain_addNamedVariable(domain, variableIndex, varSpec);

    point->base.value.pointValue = MmsValue_newDefaultValue(varSpec);
    point->base.serverCachedValue = MmsValue_newDefaultValue(varSpec);
    point->base.valueLock = Semaphore_create(1);

    variableIndex++;

    return variableIndex;
}

static int
createControlPointVariables(MmsDomain* domain, Tase2_ControlPoint point, int variableIndex)
{
    MmsVariableSpecification* varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));

    varSpec->name = StringUtils_copyString(point->base.pointName);

    if ((point->pointType == TASE2_CONTROL_TYPE_COMMAND) || (point->pointType == TASE2_CONTROL_TYPE_SETPOINT_DESCRETE)){
        varSpec->type = MMS_INTEGER;
        varSpec->typeSpec.integer = 16;
    }
    else if (point->pointType == TASE2_CONTROL_TYPE_SETPOINT_REAL) {
        varSpec->type = MMS_FLOAT;
        varSpec->typeSpec.floatingpoint.formatWidth = 32;
        varSpec->typeSpec.floatingpoint.exponentWidth = 8;
    }

    MmsDomain_addNamedVariable(domain, variableIndex, varSpec);
    point->base.value.pointValue = MmsValue_newDefaultValue(varSpec);
    point->base.serverCachedValue = MmsValue_newDefaultValue(varSpec);

    variableIndex++;

    if (point->deviceClass == TASE2_DEVICE_CLASS_SBO) {
        varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));

        varSpec->name = StringUtils_createString(2, point->base.pointName, "_SBO");

        varSpec->type = MMS_INTEGER;
        varSpec->typeSpec.integer = 16;

        point->sboValue = MmsValue_newIntegerFromInt16(point->checkBackId);

        MmsDomain_addNamedVariable(domain, variableIndex, varSpec);

        variableIndex++;
    }

    if (point->hasTag) {
        varSpec = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));

        varSpec->name = StringUtils_createString(2, point->base.pointName, "_TAG");
        varSpec->type = MMS_STRUCTURE;
        varSpec->typeSpec.structure.elementCount = 2;
        varSpec->typeSpec.structure.elements = (MmsVariableSpecification** )GLOBAL_CALLOC(2, sizeof(MmsVariableSpecification*));

        varSpec->typeSpec.structure.elements[0] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
        varSpec->typeSpec.structure.elements[0]->name = StringUtils_copyString("Flags");
        varSpec->typeSpec.structure.elements[0]->type = MMS_BIT_STRING;
        varSpec->typeSpec.structure.elements[0]->typeSpec.bitString = 3;

        varSpec->typeSpec.structure.elements[1] = (MmsVariableSpecification*) GLOBAL_CALLOC(1, sizeof(MmsVariableSpecification));
        varSpec->typeSpec.structure.elements[1]->name = StringUtils_copyString("Reason");
        varSpec->typeSpec.structure.elements[1]->type = MMS_VISIBLE_STRING;
        varSpec->typeSpec.structure.elements[1]->typeSpec.visibleString = 255;

        point->tagValue = MmsValue_newDefaultValue(varSpec);

        MmsDomain_addNamedVariable(domain, variableIndex, varSpec);

        variableIndex++;
    }

    return variableIndex;
}

static int
addPointVariables(MmsDomain* domain, Tase2_DataPoint point, int variableIndex)
{
    if (point->pointType == TASE2_POINT_TYPE_INDICATION)
        return createIndicationPointVariable(domain, (Tase2_IndicationPoint) point, variableIndex);
    else if (point->pointType == TASE2_POINT_TYPE_CONTROL)
        return createControlPointVariables(domain, (Tase2_ControlPoint) point, variableIndex);
    else if (point->pointType == TASE2_POINT_TYPE_PROTECTION_EVENT)
        return createProtectionEventVariable(domain, (Tase2_ProtectionEquipment) point, variableIndex);
    else
        return variableIndex;
}

static int
addTransferSet(MmsDomain* domain, Tase2_TransferSet ts, int variableIndex, Tase2_Edition edition)
{
    if (ts->transferSetType == TASE2_TS_TYPE_DATA_SET) {
        MmsDomain_addNamedVariable(domain, variableIndex, createDSTransferSet(ts->name, edition));

        ((Tase2_DSTransferSet)ts)->edition = edition;

        ts->mmsValue = MmsValue_newDefaultValue(domain->namedVariables[variableIndex]);
    }

    return (variableIndex + 1);
}

static void
addDataSets(Tase2_Domain iccpDomain, MmsDevice* mmsDevice, MmsDomain* mmsDomain)
{
    Tase2_DataSet nextDataSet = iccpDomain->dataSets;

    while (nextDataSet) {

        MmsNamedVariableList mmsVarList = MmsNamedVariableList_create(mmsDomain, nextDataSet->name, false);

        Tase2_DataSetEntry nextDsEntry = nextDataSet->entries;

        while (nextDsEntry) {

            MmsAccessSpecifier varAccessSpec;

            varAccessSpec.arrayIndex = -1;
            varAccessSpec.componentName = NULL;

            if (nextDsEntry->domain == iccpDomain)
                varAccessSpec.domain = mmsDomain;
            else
                varAccessSpec.domain = NULL; /* VMD scope */

            varAccessSpec.variableName = nextDsEntry->variableName;

            MmsNamedVariableListEntry entry = MmsNamedVariableListEntry_create(varAccessSpec);

            MmsNamedVariableList_addVariable(mmsVarList, entry);

            nextDsEntry = nextDsEntry->sibling;
        }

        if (mmsDomain)
            MmsDomain_addNamedVariableList(mmsDomain, mmsVarList);
        else
            MmsDevice_addNamedVariableList(mmsDevice, mmsVarList);

        nextDataSet = nextDataSet->sibling;
    }
}

static void
createMmsDeviceDataModel(Tase2_Server self, Tase2_DataModel dataModel)
{
    MmsDevice* device = MmsDevice_create((char*)"VCC");

    int specialTSVariablesCount = 4;

    if (dataModel->hideSpecialTSVariables)
        specialTSVariablesCount = 0;

    self->device = device;

    if (self->device != NULL) {

        /* Add domains */

        int domainCount = getDomainCount(dataModel);

        device->domainCount = domainCount;
        device->domains =  (MmsDomain**) GLOBAL_MALLOC(domainCount * sizeof(MmsDomain*));

        Tase2_Domain nextDomain = dataModel->domains;

        int domainIndex = 0;

        while (nextDomain) {

            tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL,
                    "Domain [%s] setup complete - number of datapoints: %i avl-tree-depth: %i", nextDomain->name, nextDomain->dataPointCount,
                    HashTree_getMaxDepth((HashTree)nextDomain->dataPointHashTree));

            MmsDomain* mmsDomain = MmsDomain_create(nextDomain->name);

            /* Add system variables and data points */

            mmsDomain->namedVariablesCount = getVariablesCount(nextDomain) + specialTSVariablesCount + 2;

            if (nextDomain->informationMessages) {
                mmsDomain->namedVariablesCount ++; /* Info_Mess_Header */

                mmsDomain->namedVariablesCount += getInfoMessageCount(nextDomain);
            }

            mmsDomain->namedVariables = (MmsVariableSpecification**) GLOBAL_MALLOC(mmsDomain->namedVariablesCount * sizeof(MmsVariableSpecification*));
            mmsDomain->namedVariablesHashTree = HashTree_create();

            int variableIndex = 0;

            if (dataModel->hideSpecialTSVariables == false) {
                /* Add transfer set special variables */
                MmsDomain_addNamedVariable(mmsDomain, variableIndex++, createMmsObjectName("Transfer_Set_Name"));
                MmsDomain_addNamedVariable(mmsDomain, variableIndex++, createBitStringVariable("DSConditions_Detected", 5));   /* DSConditions_Detected */
                MmsDomain_addNamedVariable(mmsDomain, variableIndex++, createInt16Variable("Event_Code_Detected"));
                MmsDomain_addNamedVariable(mmsDomain, variableIndex++, createIntVariable("Transfer_Set_Time_Stamp", 32));
            }

            MmsDomain_addNamedVariable(mmsDomain, variableIndex++, createMmsObjectName("Next_DSTransfer_Set"));
            MmsDomain_addNamedVariable(mmsDomain, variableIndex++, createBilateralTableId());

            if (nextDomain->informationMessages) {
                MmsDomain_addNamedVariable(mmsDomain, variableIndex++, createInfoHeader());

                Tase2_InformationMessage im = nextDomain->informationMessages;

                while (im) {
                    MmsDomain_addNamedVariable(mmsDomain, variableIndex++, createInfoBuf(im->maxSize));

                    im = im->sibling;
                }
            }

            Tase2_DataPoint nextPoint = nextDomain->dataPoints;

            while (nextPoint) {
                variableIndex = addPointVariables(mmsDomain, nextPoint, variableIndex);

                nextPoint = nextPoint->sibling;
            }

            /* Add data sets */
            addDataSets(nextDomain, device, mmsDomain);

            /* Add transfer sets */

            Tase2_TransferSet nextTs = nextDomain->transferSets;

            while (nextTs) {
                variableIndex = addTransferSet(mmsDomain, nextTs, variableIndex, dataModel->edition);

                nextTs = nextTs->sibling;
            }

            device->domains[domainIndex++] = mmsDomain;

            nextDomain = nextDomain->sibling;
        }

        /* Add VCC data points */

        Tase2_Domain vcc = dataModel->vcc;

        device->namedVariablesCount = getVariablesCount(vcc);
        device->namedVariablesCount += 2; /* System variables */

        if (vcc->informationMessages) {
            device->namedVariablesCount ++; /* Info_Mess_Header */

            device->namedVariablesCount += getInfoMessageCount(vcc);
        }

        int variableIndex = 0;

        device->namedVariables = (MmsVariableSpecification**) GLOBAL_MALLOC(device->namedVariablesCount * sizeof(MmsVariableSpecification*));

        /* Add system variables */

        MmsDomain_addNamedVariable((MmsDomain*)device, variableIndex++, createSupportedFeatures("Supported_Features")); /* Supported_Features */
        MmsDomain_addNamedVariable((MmsDomain*)device, variableIndex++, createTase2Version()); /* TASE2_Version */

        if (vcc->informationMessages) {
            MmsDomain_addNamedVariable((MmsDomain*)device, variableIndex++, createInfoHeader());

            Tase2_InformationMessage im = vcc->informationMessages;

            while (im) {
                MmsDomain_addNamedVariable((MmsDomain*)device, variableIndex++, createInfoBuf(im->maxSize));

                im = im->sibling;
            }
        }

        /* Add user specific data points */

        Tase2_DataPoint nextPoint = vcc->dataPoints;

        while (nextPoint) {
            variableIndex = addPointVariables((MmsDomain*)device, nextPoint, variableIndex);

            nextPoint = nextPoint->sibling;
        }

        /* Add VCC scope data sets */
        addDataSets(vcc, device, NULL);

        tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL,
                "VCC setup complete - number of datapoints: %i avl-tree-depth: %i", device->namedVariablesCount,
                HashTree_getMaxDepth((HashTree)device->namedVariablesHashTree));
    }
}

static void
initializeSystemVariables(Tase2_Server self)
{
    if (self->dataModel->edition == TASE2_EDITION_2000_08) {
        MmsServer_insertIntoCache(self->mmsServer, (MmsDomain*) self->device, "TASE2_Version",
                createNewTase2VersionValue(2000, 8));
    }
    else {
        MmsServer_insertIntoCache(self->mmsServer, (MmsDomain*) self->device, "TASE2_Version",
                createNewTase2VersionValue(1996, 8));
    }

    self->supportedFeatures = MmsValue_newBitString(12);
    MmsValue_setBitStringFromInteger(self->supportedFeatures, TASE2_SUPPORTED_FB_1 | TASE2_SUPPORTED_FB_2 | TASE2_SUPPORTED_FB_4 | TASE2_SUPPORTED_FB_5);

    MmsServer_insertIntoCache(self->mmsServer, (MmsDomain*) self->device, "Supported_Features", self->supportedFeatures);

    /* Add domain specific variable values */

    Tase2_Domain domain = self->dataModel->domains;

    while (domain) {

        if (domain->bilateralTableId != NULL) {
            MmsDomain* mmsDomain = MmsDevice_getDomain(self->device, domain->name);

            MmsServer_insertIntoCache(self->mmsServer, mmsDomain, "Bilateral_Table_ID", MmsValue_newVisibleString(domain->bilateralTableId));
        }

        domain = domain->sibling;
    }
}

AcseServiceUserDiagnostics
Tase2_Server_authenticator(Tase2_Server self, AcseAuthenticationParameter authParameter, Tase2_BilateralTable* securityToken, IsoApplicationAddress remoteAppRef)
{
    if (LinkedList_size(self->blts) != 0) {

        LinkedList bltElement = LinkedList_getNext(self->blts);

        while (bltElement) {

            Tase2_BilateralTable blt = (Tase2_BilateralTable) LinkedList_getData(bltElement);

            bool appRefMatches = Tase2_Endpoint_matchAddresses(self->endpoint, remoteAppRef, &(blt->appAddr));

            if (appRefMatches) {
                *securityToken = blt;

                tase2_log(TASE2_LOG_INFO, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "Found matching BLT: %s", blt->bltId);

                return ACSE_USER_OK;
            }

            bltElement = LinkedList_getNext(bltElement);
        }

        return ACSE_USER_CALLING_AP_TITLE_NOT_RECOGNIZED;
    }
    else {
        *securityToken = NULL;
    }

    return ACSE_USER_OK;
}

static bool
checkIfTypeIsMmsObjectName(MmsValue* value)
{
    if (MmsValue_getType(value) == MMS_STRUCTURE) {
        if (MmsValue_getArraySize(value) == 3) {

            if (MmsValue_getType(MmsValue_getElement(value, 0)) != MMS_UNSIGNED)
                return false;

            if (MmsValue_getType(MmsValue_getElement(value, 1)) != MMS_VISIBLE_STRING)
                return false;

            if (MmsValue_getStringSize(MmsValue_getElement(value, 1)) > MAX_MMS_NAME_LENGTH)
                return false;

            if (MmsValue_getType(MmsValue_getElement(value, 2)) != MMS_VISIBLE_STRING)
                return false;

            if (MmsValue_getStringSize(MmsValue_getElement(value, 2)) > MAX_MMS_NAME_LENGTH)
                return false;

            return true;
        }
    }

   return false;
}

static Tase2_TransferSet
lookupActiveTransferSet(Tase2_Server self, const char* domainName, const char* tsName)
{
    Tase2_TransferSet transferSet = NULL;

    LinkedList element;

    for (element = LinkedList_getNext(self->activeTransferSets); element; element = LinkedList_getNext(element)) {

        Tase2_TransferSet ts = (Tase2_TransferSet) LinkedList_getData(element);

        if (strlen(domainName) == 0) {
            if (ts->domain != NULL)
                continue;
        }
        else {
            if (strcmp(ts->domain->name, domainName) != 0)
                continue;
        }

        /* domain is matching */

        if (!strcmp(ts->name, tsName)) {
            transferSet = ts;
            break;
        }
    }

    return transferSet;
}

static void
mmsInformationReportHandler (MmsEndpointConnection connection, void* parameter, char* domainName,
        char* variableName, MmsValue* value, uint8_t flags)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Server self = (Tase2_Server) parameter;

    Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

    if (flags == MMS_INFO_HANDLER_IS_VARIABLE_LIST_NAME) {
        /* not used by server - ignore */
    }
    else {

        if (flags == MMS_INFO_HANDLER_REPORT_START) {

        }

        if (!strcmp(variableName, "Transfer_Report_ACK")) {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Transfer_Report_ACK");

            if (checkIfTypeIsMmsObjectName(value)) {

                int scope = MmsValue_toUint32(MmsValue_getElement(value, 0));

                const char* objectNameDomain;

                if (scope == 0)
                    objectNameDomain = "";
                else
                    objectNameDomain = MmsValue_toString(MmsValue_getElement(value, 1));

                const char* objectName = MmsValue_toString(MmsValue_getElement(value, 2));

                Tase2_TransferSet transferSet = lookupActiveTransferSet(self, objectNameDomain, objectName);

                if (transferSet) {

                    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Transfer_Report_ACK: remove outstanding critical reports for transfer set");

                    /* remove outstanding critical reports from list */
                    if (transferSet->transferSetType == TASE2_TS_TYPE_DATA_SET) {
                        Tase2_DSTransferSet dsTransferSet = (Tase2_DSTransferSet) transferSet;

                        tase2_DSTransferSet_deleteAllCriticalReports(dsTransferSet);
                    }

                }
                else {
                    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "No transfer set found for Transfer_Report_ACK");
                }
            }
            else {
                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "MMS information report: invalid type for Transfer_Report_ACK\n");
            }
        }
        else if (!strcmp(variableName, "Transfer_Report_NACK")) {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "Transfer_Report_NACK\n");

            if (checkIfTypeIsMmsObjectName(value)) {

                int scope = MmsValue_toUint32(MmsValue_getElement(value, 0));

                const char* objectNameDomain;

                if (scope == 0)
                    objectNameDomain = "";
                else
                    objectNameDomain = MmsValue_toString(MmsValue_getElement(value, 1));

                const char* objectName = MmsValue_toString(MmsValue_getElement(value, 2));

                Tase2_TransferSet transferSet = lookupActiveTransferSet(self, objectNameDomain, objectName);

                if (transferSet) {

                    /* remove outstanding critical reports from list */
                    if (transferSet->transferSetType == TASE2_TS_TYPE_DATA_SET) {
                        Tase2_DSTransferSet dsTransferSet = (Tase2_DSTransferSet) transferSet;

                        tase2_DSTransferSet_deleteAllCriticalReports(dsTransferSet);
                    }

                    self->unconfirmedCriticalReportCounter++;

                    if (self->criticalNotConfirmedHandler) {
                        self->criticalNotConfirmedHandler(self->criticalNotConfirmedHandlerParameter, transferSet, 0, true);
                    }
                }
                else {
                    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "No transfer set found for Transfer_Report_NACK");
                }
            }
            else {
                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "MMS information report: invalid type for Transfer_Report_NACK");
            }
        }
        else if (!strcmp(variableName, "Transfer_Set_Time_Stamp")) {

        }
        else
        {
            /* ignore - maybe sent to a client at the same endpoint */
        }

        if (flags == MMS_INFO_HANDLER_REPORT_END) {

        }

    }

exit_function:

    Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);

    return;
}

/**
 * \brief Perform server background tasks (has to be called frequently - multiple times per second).
 *
 * Only required for none-threaded stack operation (not implemented)
 *
 * \param self Tase2_Server instance
 */
void
Tase2_Server_tick(Tase2_Server self)
{
    tase2_Server_checkTransferSets(self);
}

static void
mmsAppTickHandler(void* parameter)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Server self = (Tase2_Server) parameter;

    Tase2_Server_tick(self);
}

static Tase2_Server
Tase2_Server_createInternal(Tase2_DataModel dataModel, Tase2_Endpoint endpoint)
{
    TASE2_LIB_CHECK_RUNTIME();

    Tase2_Server self = (Tase2_Server) GLOBAL_CALLOC(1, sizeof(struct sTase2_Server));

    if (self != NULL) {

        self->endpoint = endpoint;
        self->createdEndpoint = false;

        self->dataModel = dataModel;

        self->activeTransferSets = LinkedList_create();
        self->activeTransferSetsLock = Semaphore_create(1);

        self->blts = LinkedList_create();
        self->imEnabledConnections = LinkedList_create();
        self->imEnabledConnectionsLock = Semaphore_create(1);

        self->operateHandler = NULL;
        self->operateHandlerParameter = NULL;
        self->operateHandlerEx = NULL;
        self->selectHandler = NULL;
        self->selectHandlerParameter = NULL;
        self->setTagHandler = NULL;
        self->setTagHandlerParameter = NULL;
        self->maxDSTSEventQueueSize = -1;

        self->writeDataHandler = NULL;
        self->writeDataHandlerParameter = NULL;

        self->criticalNotConfirmedHandler = NULL;
        self->criticalNotConfirmedHandlerParameter = NULL;
        self->unconfirmedCriticalReportCounter = 0;

        self->dstsStateChangedHandler = NULL;
        self->dstsStateChangedHandlerParameter = NULL;

        self->clientConnectionHandler = NULL;
        self->clientConnectionHandlerParameter = NULL;

        createMmsDeviceDataModel(self, dataModel);

        self->mmsServer = MmsServer_create(self->device);

        self->selectTimeout = DEFAULT_SELECT_TIMEOUT;
        self->criticalReportTimeout = DEFAULT_CRITICAL_TIMEOUT;

        initializeSystemVariables(self);

        MmsServer_installAASpecificVarSpecHandler(self->mmsServer, mmsAASpecVarSpecHandler, (void*) self);
        MmsServer_installReadHandler(self->mmsServer, mmsReadVariableHandler, (void*) self);
        MmsServer_installWriteHandler(self->mmsServer, mmsWriteVariableHandler, (void*) self);
        MmsServer_installVariableListChangedHandler(self->mmsServer, mmsNamedVariableListChangedHandler, (void*) self);

        Tase2_Endpoint_installInformationReportHandler(self->endpoint, NULL, mmsInformationReportHandler, (void*) self);

        Tase2_Endpoint_addServer(self->endpoint, self);

        Tase2_ApplicationAddress endpointLocalAddress = Tase2_Endpoint_getLocalApplicationAddress(self->endpoint);

        IsoApplicationAddress serverLocalAddress = MmsServer_getLocalApplicationAddress(self->mmsServer);

        memcpy(serverLocalAddress, endpointLocalAddress, sizeof(struct sIsoApplicationAddress));
    }

    return self;
}

void
Tase2_Server_connectWithEndpoint(Tase2_Server self, Tase2_Endpoint endpoint)
{
    Tase2_Endpoint_installInformationReportHandler(endpoint, NULL, mmsInformationReportHandler, (void*) self);

    Tase2_Endpoint_installAppTickHandler(endpoint, mmsAppTickHandler, (void*) self);

    Tase2_ApplicationAddress endpointLocalAddress = Tase2_Endpoint_getLocalApplicationAddress(endpoint);
}

Tase2_Server
Tase2_Server_createEx(Tase2_DataModel dataModel, Tase2_Endpoint endpoint)
{
    return Tase2_Server_createInternal(dataModel, endpoint);
}

Tase2_Server
Tase2_Server_create(Tase2_DataModel dataModel, TLSConfiguration tlsConfiguration)
{
    Tase2_Endpoint endpoint = Tase2_Endpoint_create(tlsConfiguration, true);

    if (endpoint) {
        Tase2_Server self = Tase2_Server_createInternal(dataModel, endpoint);

        self->createdEndpoint = true;

        return self;
    }
    else {
        return NULL;
    }
}

Tase2_Endpoint
Tase2_Server_getEndpoint(Tase2_Server self)
{
    return self->endpoint;
}

void
Tase2_Server_setLocalIpAddress(Tase2_Server self, const char* localIpAddress)
{
    if (self->endpoint)
        Tase2_Endpoint_setLocalIpAddress(self->endpoint, localIpAddress);
}

void
Tase2_Server_setTcpPort(Tase2_Server self, int tcpPort)
{
    if (self->endpoint)
        Tase2_Endpoint_setLocalTcpPort(self->endpoint, tcpPort);
}

void
Tase2_Server_setLocalApTitle(Tase2_Server self, const char* apTitle, int aeQualifier)
{
    IsoApplicationAddress isoAppAddr = MmsServer_getLocalApplicationAddress(self->mmsServer);

    IsoApplicationAddress_setApTitle(isoAppAddr, apTitle);
    IsoApplicationAddress_setAeQualifier(isoAppAddr, aeQualifier);
}

void
Tase2_Server_setLocalAddresses(Tase2_Server self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector)
{
    IsoApplicationAddress isoAppAddr = MmsServer_getLocalApplicationAddress(self->mmsServer);

    PSelector pSel;
    SSelector sSel;
    TSelector tSel;

    memcpy(&pSel, &pSelector, sizeof(PSelector));
    memcpy(&sSel, &sSelector, sizeof(SSelector));
    memcpy(&tSel, &tSelector, sizeof(TSelector));

    IsoApplicationAddress_setLowerLayerAddresses(isoAppAddr, pSel, sSel, tSel);
}

IsoApplicationAddress
Tase2_Server_getLocalAddresses(Tase2_Server self)
{
    return MmsServer_getLocalApplicationAddress(self->mmsServer);
}

void
Tase2_Server_setSelectTimeout(Tase2_Server self, int timeoutInMs)
{
    self->selectTimeout = timeoutInMs;
}

void
Tase2_Server_setCriticalReportTimeout(Tase2_Server self, int timeoutInMs)
{
    self->criticalReportTimeout = timeoutInMs;
}

void
Tase2_Server_setMaxDSTSEventQueueSize(Tase2_Server self, int maxSize)
{
    if ((maxSize == -1) || (maxSize > 0)) {
        self->maxDSTSEventQueueSize = maxSize;
    }
}

void
Tase2_Server_addBilateralTable(Tase2_Server self, Tase2_BilateralTable blt)
{
    LinkedList_add(self->blts, blt);
}

void
Tase2_Server_setIdentity(Tase2_Server self, char* vendor, char* model, char* revision)
{
    if (self->endpoint)
        Tase2_Endpoint_setIdentity(self->endpoint, vendor, model, revision);
}

void
Tase2_Server_setSupportedFeatures(Tase2_Server self, int value)
{
    MmsValue_setBitStringFromInteger(self->supportedFeatures, value);
}

void
Tase2_Server_setClientConnectionHandler(Tase2_Server self, Tase2_ClientConnectionHandler handler, void* parameter)
{
    self->clientConnectionHandler = handler;
    self->clientConnectionHandlerParameter = parameter;
}

void
Tase2_Server_setSelectHandler(Tase2_Server self, Tase2_SelectHandler handler, void* parameter)
{
    self->selectHandler = handler;
    self->selectHandlerParameter = parameter;
}

void
Tase2_Server_setOperateHandler(Tase2_Server self, Tase2_OperateHandler handler, void* parameter)
{
    self->operateHandler = handler;
    self->operateHandlerParameter = parameter;
}

void
Tase2_Server_setOperateHandlerEx(Tase2_Server self, Tase2_OperateHandlerEx handler, void* parameter)
{
    self->operateHandlerEx = handler;
    self->operateHandlerParameter = parameter;
}

void
Tase2_Server_setSetTagHandler(Tase2_Server self, Tase2_SetTagHandler handler, void* parameter)
{
    self->setTagHandler = handler;
    self->setTagHandlerParameter = parameter;
}

void
Tase2_Server_setWriteDataHandler(Tase2_Server self, Tase2_WriteDataHandler handler, void* parameter)
{
    self->writeDataHandler = handler;
    self->writeDataHandlerParameter = parameter;
}

void
Tase2_Server_setTSCriticalNotConfirmedHandler(Tase2_Server self, Tase2_TSCriticalNotConfirmedHandler handler, void* parameter)
{
    self->criticalNotConfirmedHandler = handler;
    self->criticalNotConfirmedHandlerParameter = parameter;
}

void
Tase2_Server_setDSTransferSetStateChangedHandler(Tase2_Server self, Tase2_DSTransferSetStateChangedHandler handler, void* parameter)
{
    self->dstsStateChangedHandler = handler;
    self->dstsStateChangedHandlerParameter = parameter;
}

void
Tase2_Server_setDataSetEventHandler(Tase2_Server self, Tase2_DataSetEventHandler handler, void* parameter)
{
    self->dataSetChangedHandler = handler;
    self->dataSetChangedHandlerParameter = parameter;
}

void
Tase2_Server_setDSTransferSetUpdateHandler(Tase2_Server self, Tase2_DSTransferSetUpdateHandler handler, void* parameter)
{
    self->dstsUpdateHandler = handler;
    self->dstsUpdateHandlerParameter = parameter;
}

void
Tase2_Server_setIMTransferSetEventHandler(Tase2_Server self, Tase2_IMTransferSetEventHandler handler, void* parameter)
{
    self->imEventHandler = handler;
    self->imEventHandlerParameter = parameter;
}

void
Tase2_Server_setDSTransferSetReportSentHandler(Tase2_Server self, Tase2_DSTransferSetReportSentHandler handler, void* parameter)
{
    self->dstsReportSentHandler = handler;
    self->dstsReportSentHandlerParameter = parameter;
}

void
Tase2_Server_start(Tase2_Server self)
{
    if (self->endpoint) {
        Tase2_Endpoint_connect(self->endpoint);

        Tase2_Endpoint_waitForState(self->endpoint, TASE2_ENDPOINT_STATE_LISTENING, 1000);
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "cannot start server: not connected to endpoint");
    }
}

bool
Tase2_Server_isRunning(Tase2_Server self)
{
    if (self->endpoint == NULL) {
        return false;
    }

    Tase2_Endpoint_State state = Tase2_Endpoint_getState(self->endpoint);

    if ((state == TASE2_ENDPOINT_STATE_LISTENING) || (state == TASE2_ENDPOINT_STATE_CONNECTED))
        return true;
    else
        return false;
}

void
Tase2_Server_stop(Tase2_Server self)
{
    if (self->endpoint) {
        Tase2_Endpoint_disconnect(self->endpoint, 0);

        Tase2_Endpoint_waitForState(self->endpoint, TASE2_ENDPOINT_STATE_IDLE, 2000);
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "cannot stop server: not connected to endpoint");
    }
}

Tase2_Statistics
Tase2_Server_getStatistics(Tase2_Server self)
{
    Tase2_Statistics currentStats;

    if (self->endpoint) {
        currentStats.messagesSent = MmsEndpoint_getStatistics(Tase2_Endpoint_getMmsEndpoint(self->endpoint), MMS_STAT_ID_APL_MESSAGES_SENT);
        currentStats.messagesRcvd = MmsEndpoint_getStatistics(Tase2_Endpoint_getMmsEndpoint(self->endpoint), MMS_STAT_ID_APL_MESSAGES_RCVD);
    }
    else {
        currentStats.messagesSent = 0;
        currentStats.messagesRcvd = 0;
    }

    currentStats.errors = self->unconfirmedCriticalReportCounter;

    return currentStats;
}

void
Tase2_Server_updateOnlineValue(Tase2_Server self, Tase2_DataPoint dataPoint)
{
    if (dataPoint->pointType == TASE2_POINT_TYPE_CONTROL) {
        tase2_Server_triggerTransferSets(self, dataPoint);
    }
    else {
        Semaphore_wait(dataPoint->valueLock);

        if (dataPoint->pointType == TASE2_POINT_TYPE_PROTECTION_EVENT) {
            if (MmsValue_equals(dataPoint->value.pointValue, dataPoint->serverCachedValue) == false) {

                MmsValue_update(dataPoint->serverCachedValue, dataPoint->value.pointValue);

                tase2_Server_triggerTransferSets(self, dataPoint);
            }
        }
        else { /* indication point type */
            MmsValue_update(dataPoint->serverCachedValue, dataPoint->value.pointValue);

            Tase2_IndicationPoint indicationPoint = (Tase2_IndicationPoint) dataPoint;

            if (indicationPoint->hasChanged) {

                tase2_Server_triggerTransferSets(self, dataPoint);

                indicationPoint->hasChanged = false;
            }
        }

        Semaphore_post(dataPoint->valueLock);
    }
}

void
Tase2_Server_updateOnlineValueNoTrigger(Tase2_Server self, Tase2_DataPoint dataPoint)
{
    if (dataPoint->pointType != TASE2_POINT_TYPE_CONTROL) {
        Semaphore_wait(dataPoint->valueLock);

        MmsValue_update(dataPoint->serverCachedValue, dataPoint->value.pointValue);

        Semaphore_post(dataPoint->valueLock);
    }
}

void
Tase2_Server_updateByOnlineValue(Tase2_Server self, Tase2_DataPoint dataPoint)
{
    Semaphore_wait(dataPoint->valueLock);

    MmsValue_update(dataPoint->value.pointValue, dataPoint->serverCachedValue);

    Semaphore_post(dataPoint->valueLock);
}

void
Tase2_Server_destroy(Tase2_Server self)
{
    if (self->endpoint) {
        if (self->createdEndpoint) {
            Tase2_Endpoint_destroy(self->endpoint);
        }
        else {
            Tase2_Endpoint_removeServer(self->endpoint, self);
        }
    }

    LinkedList_destroyStatic(self->activeTransferSets);
    LinkedList_destroyStatic(self->imEnabledConnections);
    LinkedList_destroyDeep(self->blts, (LinkedListValueDeleteFunction) Tase2_BilateralTable_destroy);

    Semaphore_destroy(self->activeTransferSetsLock);
    Semaphore_destroy(self->imEnabledConnectionsLock);

    MmsServer_destroy(self->mmsServer);

    MmsDevice_destroy(self->device);

    GLOBAL_FREEMEM(self);
}

Tase2_DataPoint
Tase2_SentPointValue_getDataPoint(Tase2_SentPointValue self)
{
    return self->dataPoint;
}

Tase2_PointValue
Tase2_SentPointValue_getPointValue(Tase2_SentPointValue self)
{
    return &(self->sentPointValue);
}
