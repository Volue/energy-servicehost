/*
 *  tase2_model.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2018 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "tase2_model.h"
#include "lib_memory.h"
#include "string_utilities.h"
#include "hash_tree.h"

#include "tase2_common_internal.h"
#include "tase2_server_internal.h"


static Tase2_Domain
Tase2_Domain_create(const char* domainName)
{
    Tase2_Domain self = (Tase2_Domain) GLOBAL_MALLOC(sizeof(struct sTase2_Domain));

    if (self != NULL) {
        self->dataPoints = NULL;
        self->dataPointHashTree = HashTree_create();
        self->dataPointCount = 0;
        self->lastDataPoint = NULL;
        self->dataSets = NULL;
        self->transferSets = NULL;
        self->sibling = NULL;
        self->bilateralTableId = NULL;
        self->informationMessages = NULL;

        if (domainName != NULL)
            self->name = StringUtils_copyString(domainName);
        else
            self->name = NULL;
    }

    return self;
}

static void
Tase2_ControlPoint_destroy(Tase2_ControlPoint self)
{
    if (self->sboValue)
        MmsValue_delete(self->sboValue);

    if (self->tagValue)
        MmsValue_delete(self->tagValue);
}

static void
Tase2_DataPoint_destroy(Tase2_DataPoint self)
{
    if (self->pointName != NULL)
        GLOBAL_FREEMEM(self->pointName);

    if (self->valueLock)
        Semaphore_destroy(self->valueLock);

    if (self->value.pointValue)
        MmsValue_delete(self->value.pointValue);

    if (self->serverCachedValue)
        MmsValue_delete(self->serverCachedValue);

    if (self->pointType == TASE2_POINT_TYPE_CONTROL)
        Tase2_ControlPoint_destroy((Tase2_ControlPoint) self);

    if (self->activeTransferSets)
        LinkedList_destroy(self->activeTransferSets);

    GLOBAL_FREEMEM(self);
}

static void
Tase2_DSTransferSet_destroy(Tase2_DSTransferSet self)
{
    GLOBAL_FREEMEM(self->base.name);

    if (self->base.mmsValue != NULL)
        MmsValue_delete(self->base.mmsValue);

    if (self->dataPoints != NULL)
        LinkedList_destroyStatic(self->dataPoints);

    GLOBAL_FREEMEM(self);
}

static void
Tase2_DataSet_destroy(Tase2_DataSet self)
{
    GLOBAL_FREEMEM(self->name);

    Tase2_DataSetEntry entry = self->entries;

    while (entry) {
        Tase2_DataSetEntry sibling = entry->sibling;

        GLOBAL_FREEMEM(entry->variableName);
        GLOBAL_FREEMEM(entry);

        entry = sibling;
    }

    GLOBAL_FREEMEM(self);
}

static void
Tase2_Domain_destroy(Tase2_Domain self)
{
    Tase2_DataSet dataSet = self->dataSets;

    while (dataSet) {
        Tase2_DataSet sibling = dataSet->sibling;

        Tase2_DataSet_destroy(dataSet);

        dataSet = sibling;
    }

    if (self->bilateralTableId)
        GLOBAL_FREEMEM(self->bilateralTableId);

    Tase2_DataPoint dp = self->dataPoints;

    while (dp) {
        Tase2_DataPoint sibling = dp->sibling;

        Tase2_DataPoint_destroy(dp);

        dp = sibling;
    }

    HashTree_destroy(self->dataPointHashTree);

    Tase2_TransferSet ts = self->transferSets;

    while (ts) {
        Tase2_TransferSet sibling = ts->sibling;

        if (ts->transferSetType == TASE2_TS_TYPE_DATA_SET)
            Tase2_DSTransferSet_destroy((Tase2_DSTransferSet) ts);

        ts = sibling;
    }

    Tase2_InformationMessage im = self->informationMessages;

    while (im) {
        Tase2_InformationMessage sibling = im->sibling;
        MmsValue_delete((MmsValue*)(im->infoBuf));
        GLOBAL_FREEMEM(im);

        im = sibling;
    }

    if (self->name != NULL)
        GLOBAL_FREEMEM(self->name);

    GLOBAL_FREEMEM(self);
}

Tase2_DataModel
Tase2_DataModel_create()
{
    Tase2_DataModel self = (Tase2_DataModel) GLOBAL_MALLOC(sizeof(struct sTase2_DataModel));

    if (self != NULL) {
        self->vcc = Tase2_Domain_create(NULL);
        self->domains = NULL;
        self->hideSpecialTSVariables = false;
        self->edition = 2;
    }

    return self;
}

void
Tase2_DataModel_setEdition(Tase2_DataModel self, Tase2_Edition edition)
{
    self->edition = edition;
}

void
Tase2_DataModel_destroy(Tase2_DataModel self)
{
    if (self->vcc != NULL)
        Tase2_Domain_destroy(self->vcc);

    Tase2_Domain domain = self->domains;

    while (domain) {
        Tase2_Domain sibling = domain->sibling;

        Tase2_Domain_destroy(domain);

        domain = sibling;
    }

    GLOBAL_FREEMEM(self);
}

void
Tase2_DataModel_hideSpecialTSVariables(Tase2_DataModel self, bool value)
{
    self->hideSpecialTSVariables = value;
}

Tase2_Domain
Tase2_DataModel_addDomain(Tase2_DataModel self, const char* domainName)
{
    Tase2_Domain domain = Tase2_Domain_create(domainName);

    if (domain != NULL) {

        /* append the new domain to the domain list */

        if (self->domains == NULL)
            self->domains = domain;
        else {
            Tase2_Domain lastDomain = self->domains;

            while (lastDomain->sibling != NULL)
                lastDomain = lastDomain->sibling;

            lastDomain->sibling = domain;
        }
    }

    return domain;
}

Tase2_Domain
Tase2_DataModel_getDomain(Tase2_DataModel self, const char* domainName)
{
    Tase2_Domain domain = self->domains;

    while (domain) {
        if (!strcmp(domain->name, domainName))
            return domain;

        domain = domain->sibling;
    }

    return NULL;
}

Tase2_Domain
Tase2_DataModel_getVCC(Tase2_DataModel self)
{
    return self->vcc;
}

static Tase2_IndicationPoint
Tase2_IndicationPoint_create(Tase2_Domain domain, const char* pointName,
        Tase2_IndicationPointType pointType,
        Tase2_QualityClass qualityClass,
        Tase2_TimeStampClass timeStampClass,
        bool hasCOV,
        bool readOnly)
{
    Tase2_IndicationPoint self = (Tase2_IndicationPoint) GLOBAL_MALLOC(sizeof(struct sTase2_IndicationPoint));

    if (self != NULL) {
        if (pointName != NULL)
            self->base.pointName = StringUtils_copyString(pointName);
        else
            self->base.pointName = NULL;

        self->base.pointType = TASE2_POINT_TYPE_INDICATION;

        switch(pointType)
        {
        case TASE2_IND_POINT_TYPE_REAL:
            self->base.value.type = TASE2_VALUE_TYPE_REAL;
            break;
        case TASE2_IND_POINT_TYPE_STATE:
            self->base.value.type = TASE2_VALUE_TYPE_STATE;
            break;
        case TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL:
            self->base.value.type = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
            break;
        case TASE2_IND_POINT_TYPE_DISCRETE:
            self->base.value.type = TASE2_VALUE_TYPE_DISCRETE;
            break;
        }

        self->base.value.pointValue = NULL;
        self->base.serverCachedValue = NULL;
        self->base.sibling = NULL;
        self->base.domain = domain;
        self->base.activeTransferSets = NULL;
        self->base.valueLock = NULL;

        self->hasCov = hasCOV;
        self->readOnly = readOnly;
        self->pointType = pointType;
        self->qualityClass = qualityClass;
        self->timeStampClass = timeStampClass;

        self->hasChanged = false;
        self->hasDeadband = false;
    }

    return self;
}

const char*
Tase2_DataPoint_getName(Tase2_DataPoint self)
{
    return self->pointName;
}

Tase2_Domain
Tase2_DataPoint_getDomain(Tase2_DataPoint self)
{
    return self->domain;
}

Tase2_PointValue
Tase2_DataPoint_getPointValue(Tase2_DataPoint self)
{
    return &(self->value);
}

const char*
Tase2_IndicationPoint_getName(Tase2_IndicationPoint self)
{
    return Tase2_DataPoint_getName(&(self->base));
}

static void
updateFloatValue(Tase2_IndicationPoint self, MmsValue* mmsValue, float value)
{
    float oldValue = MmsValue_toFloat(mmsValue);

    if (oldValue != value) {
        MmsValue_setFloat(mmsValue, value);
        self->hasChanged = true;
    }
}

static void
updateDiscreteValue(Tase2_IndicationPoint self, MmsValue* mmsValue, int32_t value)
{
    int32_t oldValue = MmsValue_toInt32(mmsValue);

    if (oldValue != value) {
        MmsValue_setInt32(mmsValue, value);
        self->hasChanged = true;
    }
}

static void
updateFlagsValue(Tase2_IndicationPoint self, MmsValue* mmsValue, Tase2_DataState flags)
{
    Tase2_DataState oldFlags = (Tase2_DataState) MmsValue_getBitStringAsInteger(mmsValue);

    if (oldFlags != flags) {
        MmsValue_setBitStringFromInteger(mmsValue, flags);
        self->hasChanged = true;
    }
}

void
Tase2_IndicationPoint_setReal(Tase2_IndicationPoint self, float value)
{
    if (self->pointType == TASE2_IND_POINT_TYPE_REAL) {

        MmsValue* mmsValue;

        if (self->base.value.pointValue != NULL) {

            if (MmsValue_getType(self->base.value.pointValue) == MMS_STRUCTURE) {
                mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);
            }
            else  {
                mmsValue = self->base.value.pointValue;
            }

            updateFloatValue(self, mmsValue, value);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setRealQ(Tase2_IndicationPoint self, float value, Tase2_DataState flags)
{
    if ((self->pointType == TASE2_IND_POINT_TYPE_REAL) && (self->qualityClass == TASE2_QUALITY)) {

        if (self->base.value.pointValue != NULL) {
            MmsValue* mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);

            updateFloatValue(self, mmsValue, value);

            MmsValue* flagsValue;

            if (self->timeStampClass == TASE2_NO_TIMESTAMP)
                flagsValue = MmsValue_getElement(self->base.value.pointValue, 1);
            else
                flagsValue = MmsValue_getElement(self->base.value.pointValue, 2);

            updateFlagsValue(self, flagsValue, flags);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }

    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setRealQTimeStamp(Tase2_IndicationPoint self, float value, Tase2_DataState flags, uint64_t timestamp)
{
    if ((self->pointType == TASE2_IND_POINT_TYPE_REAL) && (self->qualityClass == TASE2_QUALITY) && (self->timeStampClass != TASE2_NO_TIMESTAMP)) {

        if (self->base.value.pointValue != NULL) {
            MmsValue* mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);

            updateFloatValue(self, mmsValue, value);

            MmsValue* flagsValue =  MmsValue_getElement(self->base.value.pointValue, 2);

            updateFlagsValue(self, flagsValue, flags);

            MmsValue* timestampValue = MmsValue_getElement(self->base.value.pointValue, 1);

            if (MmsValue_getType(timestampValue) == MMS_STRUCTURE) {
                MmsValue_setInt32(MmsValue_getElement(timestampValue, 0), timestamp / 1000);
                MmsValue_setInt16(MmsValue_getElement(timestampValue, 1), timestamp % 1000);
            }
            else {
                MmsValue_setInt32(timestampValue, timestamp / 1000);
            }
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }

    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}


void
Tase2_IndicationPoint_setDiscrete(Tase2_IndicationPoint self, int32_t value)
{
    if (self->pointType == TASE2_IND_POINT_TYPE_DISCRETE) {

        MmsValue* mmsValue;

        if (self->base.value.pointValue != NULL) {

            if (MmsValue_getType(self->base.value.pointValue) == MMS_STRUCTURE) {
                mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);
            }
            else  {
                mmsValue = self->base.value.pointValue;
            }

            updateDiscreteValue(self, mmsValue, value);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                   self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setDiscreteQ(Tase2_IndicationPoint self, int32_t value, Tase2_DataState flags)
{
    if ((self->pointType == TASE2_IND_POINT_TYPE_DISCRETE) && (self->qualityClass == TASE2_QUALITY)) {

        if (self->base.value.pointValue != NULL) {
            MmsValue* mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);

            updateDiscreteValue(self, mmsValue, value);

            MmsValue* flagsValue;

            if (self->timeStampClass == TASE2_NO_TIMESTAMP)
                flagsValue = MmsValue_getElement(self->base.value.pointValue, 1);
            else
                flagsValue = MmsValue_getElement(self->base.value.pointValue, 2);

            updateFlagsValue(self, flagsValue, flags);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }

    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setDiscreteQTimeStamp(Tase2_IndicationPoint self, int32_t value, Tase2_DataState flags, uint64_t timestamp)
{
    if ((self->pointType == TASE2_IND_POINT_TYPE_DISCRETE) && (self->qualityClass == TASE2_QUALITY) && (self->timeStampClass != TASE2_NO_TIMESTAMP)) {

        if (self->base.value.pointValue != NULL) {
            MmsValue* mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);

            updateDiscreteValue(self, mmsValue, value);

            MmsValue* flagsValue =  MmsValue_getElement(self->base.value.pointValue, 2);

            updateFlagsValue(self, flagsValue, flags);

            MmsValue* timestampValue = MmsValue_getElement(self->base.value.pointValue, 1);

            if (MmsValue_getType(timestampValue) == MMS_STRUCTURE) {
                MmsValue_setInt32(MmsValue_getElement(timestampValue, 0), timestamp / 1000);
                MmsValue_setInt16(MmsValue_getElement(timestampValue, 1), timestamp % 1000);
            }
            else {
                MmsValue_setInt32(timestampValue, timestamp / 1000);
            }
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }

    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setState(Tase2_IndicationPoint self, Tase2_DataState value)
{
    if (self->pointType == TASE2_IND_POINT_TYPE_STATE) {

        MmsValue* mmsValue;

        if (self->base.value.pointValue != NULL) {

            if (MmsValue_getType(self->base.value.pointValue) == MMS_STRUCTURE) {

                if (self->timeStampClass != TASE2_NO_TIMESTAMP)
                    mmsValue = MmsValue_getElement(self->base.value.pointValue, 1);
                else
                    mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);
            }
            else  {
                mmsValue = self->base.value.pointValue;
            }

            updateFlagsValue(self, mmsValue, value);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setStateTimeStamp(Tase2_IndicationPoint self, Tase2_DataState value, uint64_t timestamp)
{
    if ((self->pointType == TASE2_IND_POINT_TYPE_STATE) && (self->timeStampClass != TASE2_NO_TIMESTAMP)){

        MmsValue* mmsValue;

        if (self->base.value.pointValue != NULL) {

            if (MmsValue_getType(self->base.value.pointValue) == MMS_STRUCTURE) {
                mmsValue = MmsValue_getElement(self->base.value.pointValue, 1);

                MmsValue* timestampValue = MmsValue_getElement(self->base.value.pointValue, 0);

                if (MmsValue_getType(timestampValue) == MMS_STRUCTURE) {
                    MmsValue_setInt32(MmsValue_getElement(timestampValue, 0), timestamp / 1000);
                    MmsValue_setInt16(MmsValue_getElement(timestampValue, 1), timestamp % 1000);
                }
                else {
                    MmsValue_setInt32(timestampValue, timestamp / 1000);
                }
            }
            else  {
                mmsValue = self->base.value.pointValue;
            }

            updateFlagsValue(self, mmsValue, value);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setStateSupplemental(Tase2_IndicationPoint self, Tase2_DataStateSupplemental value)
{
    if (self->pointType == TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL) {

        MmsValue* mmsValue;

        if (self->base.value.pointValue != NULL) {

            if (MmsValue_getType(self->base.value.pointValue) == MMS_STRUCTURE) {
                mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);
            }
            else {
                mmsValue = self->base.value.pointValue;
            }

            updateFlagsValue(self, mmsValue, (Tase2_DataState) value);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setStateSupplementalQ(Tase2_IndicationPoint self, Tase2_DataStateSupplemental value, Tase2_DataFlags flags)
{
    if ((self->pointType == TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL) && (self->qualityClass == TASE2_QUALITY)) {

        if (self->base.value.pointValue != NULL) {

            if (MmsValue_getType(self->base.value.pointValue) == MMS_STRUCTURE) {

                MmsValue* mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);

                updateFlagsValue(self, mmsValue, (Tase2_DataState) value);

                MmsValue* q;

                if (self->timeStampClass == TASE2_NO_TIMESTAMP) {
                    q = MmsValue_getElement(self->base.value.pointValue, 1);
                }
                else {
                    q = MmsValue_getElement(self->base.value.pointValue, 2);
                }

                updateFlagsValue(self, q, value);
            }
            else {
                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Wrong type!");
            }

        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setStateSupplementalQTimeStamp(Tase2_IndicationPoint self, Tase2_DataStateSupplemental value, Tase2_DataFlags flags, uint64_t timestamp)
{
    if ((self->pointType == TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL) && (self->qualityClass == TASE2_QUALITY)
            && (self->timeStampClass != TASE2_NO_TIMESTAMP))
    {
        if (self->base.value.pointValue != NULL) {

            if (MmsValue_getType(self->base.value.pointValue) == MMS_STRUCTURE) {

                MmsValue* mmsValue = MmsValue_getElement(self->base.value.pointValue, 0);

                updateFlagsValue(self, mmsValue, (Tase2_DataState) value);

                MmsValue* q = MmsValue_getElement(self->base.value.pointValue, 2);

                updateFlagsValue(self, q, value);

                MmsValue* timestampValue = MmsValue_getElement(self->base.value.pointValue, 1);

                if (MmsValue_getType(timestampValue) == MMS_STRUCTURE) {
                    MmsValue_setInt32(MmsValue_getElement(timestampValue, 0), timestamp / 1000);
                    MmsValue_setInt16(MmsValue_getElement(timestampValue, 1), timestamp % 1000);
                }
                else {
                    MmsValue_setInt32(timestampValue, timestamp / 1000);
                }

            }
            else {
                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Wrong type!");
            }

        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}


void
Tase2_IndicationPoint_setQuality(Tase2_IndicationPoint self, Tase2_DataFlags flags)
{
    if (self->pointType == TASE2_IND_POINT_TYPE_STATE) {

        MmsValue* stateVal;

        if (self->base.value.pointValue != NULL) {
            if (MmsValue_getType(self->base.value.pointValue) == MMS_STRUCTURE) {
                stateVal = MmsValue_getElement(self->base.value.pointValue, 1);
            }
            else {
                stateVal = self->base.value.pointValue;
            }

            if (MmsValue_getType(stateVal) == MMS_BIT_STRING) {
                Tase2_DataFlags value = MmsValue_getBitStringAsInteger(stateVal);

                flags = flags & 0xfc;

                Tase2_DataFlags oldFlags = value & 0xfc;

                if (flags != oldFlags) {
                    value = value & 0x03;

                    value = value | flags;

                    MmsValue_setBitStringFromInteger(stateVal, (uint32_t) value);
                    self->hasChanged = true;
                }
            }
            else {
                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                        self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
            }
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else if (self->qualityClass != TASE2_NO_QUALITY) {

        if (self->base.value.pointValue != NULL) {
            int index = 1;

            if (self->timeStampClass != TASE2_NO_TIMESTAMP) {
                if (self->pointType != TASE2_IND_POINT_TYPE_STATE)
                    index = 2;
            }

            MmsValue* flagsValue = MmsValue_getElement(self->base.value.pointValue, index);

            updateFlagsValue(self, flagsValue, flags);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }

    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setTimeStamp(Tase2_IndicationPoint self, uint64_t timestamp)
{
    if ((self->timeStampClass != TASE2_NO_TIMESTAMP)) {
        if (self->base.value.pointValue != NULL) {

            MmsValue* timestampValue;

            if (self->pointType == TASE2_IND_POINT_TYPE_STATE)
                timestampValue = MmsValue_getElement(self->base.value.pointValue, 0);
            else
                timestampValue = MmsValue_getElement(self->base.value.pointValue, 1);

            if (MmsValue_getType(timestampValue) == MMS_STRUCTURE) {
                MmsValue_setInt32(MmsValue_getElement(timestampValue, 0), timestamp / 1000);
                MmsValue_setInt16(MmsValue_getElement(timestampValue, 1), timestamp % 1000);
            }
            else {
                MmsValue_setInt32(timestampValue, timestamp / 1000);
            }

        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }

    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_IndicationPoint_setCOV(Tase2_IndicationPoint self, uint16_t cov)
{
    if (self->hasCov) {
        if (self->base.value.pointValue != NULL) {

            int covIndex = 3;

            if (self->pointType == TASE2_IND_POINT_TYPE_STATE)
                covIndex = 2;

            if (self->qualityClass == TASE2_NO_QUALITY) {

                if ((self->pointType == TASE2_IND_POINT_TYPE_STATE) && (self->timeStampClass != TASE2_NO_TIMESTAMP)) {

                }
                else
                    covIndex--;
            }

            if (self->timeStampClass == TASE2_NO_TIMESTAMP)
                covIndex--;

            MmsValue* covValue = MmsValue_getElement(self->base.value.pointValue, covIndex);

            MmsValue_setUint16(covValue, cov);

        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else  {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

const char*
Tase2_ProtectionEquipment_getName(Tase2_ProtectionEquipment self)
{
    return Tase2_DataPoint_getName(&(self->base));
}


void
Tase2_ProtectionEquipment_setSingleEventFlags(Tase2_ProtectionEquipment self, Tase2_SingleEventFlags eventFlags)
{
    if (self->isPacked == false) {
        MmsValue* eventFlagsValue = MmsValue_getElement(self->base.value.pointValue, 0);

        MmsValue_setBitStringFromInteger(eventFlagsValue, eventFlags);
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_ProtectionEquipment_setPackedEvent(Tase2_ProtectionEquipment self, Tase2_EventFlags packedEvent)
{
    if (self->isPacked == true) {

        if (self->base.value.pointValue) {
            MmsValue* packedEventValue = MmsValue_getElement(self->base.value.pointValue, 0);

            MmsValue_setBitStringFromInteger(packedEventValue, packedEvent);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_ProtectionEquipment_setPackedEventFlags(Tase2_ProtectionEquipment self, Tase2_PackedEventFlags flags)
{
    if (self->isPacked == true) {
        if (self->base.value.pointValue) {
            MmsValue* packedEventFlagsValue = MmsValue_getElement(self->base.value.pointValue, 1);

            MmsValue_setBitStringFromInteger(packedEventFlagsValue, flags);
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                    self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s has wrong type",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

void
Tase2_ProtectionEquipment_setOperatingTime(Tase2_ProtectionEquipment self, int32_t operatingTime)
{
    MmsValue* operatingTimeValue;

    if (self->base.value.pointValue) {
        if (self->isPacked == false)
            operatingTimeValue = MmsValue_getElement(self->base.value.pointValue, 1);
        else
            operatingTimeValue = MmsValue_getElement(self->base.value.pointValue, 2);

        MmsValue_setInt32(operatingTimeValue, operatingTime);
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}

const char*
Tase2_ControlPoint_getName(Tase2_ControlPoint self)
{
    return Tase2_DataPoint_getName(&(self->base));
}

Tase2_Domain
Tase2_ControlPoint_getDomain(Tase2_ControlPoint self)
{
    return Tase2_DataPoint_getDomain(&(self->base));
}

Tase2_ControlPointType
Tase2_ControlPoint_getType(Tase2_ControlPoint self)
{
    return self->pointType;
}

Tase2_TagValue
Tase2_ControlPoint_getTag(Tase2_ControlPoint self, char** reason, Tase2_BilateralTable* tagOwner)
{
    if (self && self->hasTag) {

        Tase2_TagValue tagValue = tase2_ControlPoint_getTagValue(self);

        if (tagOwner)
            *tagOwner = self->tagOwner;

        if (reason)
            *reason = StringUtils_copyString(MmsValue_toString(MmsValue_getElement(self->tagValue, 1)));

        return tagValue;
    }

    return TASE2_TAG_INVALID;
}

void
Tase2_ControlPoint_setTag(Tase2_ControlPoint self, Tase2_TagValue value, const char* reason, Tase2_BilateralTable tagOwner)
{
    if (self && self->hasTag) {

        tase2_ControlPoint_setTagValue(self, value);

        /* deselect if selected */

        if (self->selectedTimestamp != 0) {
            self->selectedTimestamp = 0;

            tase2_ControlPoint_setArmedFlag(self, false);
        }

        if (reason) {

            MmsValue* mmsReasonVal = MmsValue_getElement(self->tagValue, 1);

            MmsValue_setVisibleString(mmsReasonVal, reason);
        }

        self->tagOwner = tagOwner;
    }
}


void
Tase2_ProtectionEquipment_setEventTime(Tase2_ProtectionEquipment self, uint64_t eventTime)
{
    MmsValue* eventTimeValue;

    if (self->base.value.pointValue) {
        if (self->isPacked == false)
            eventTimeValue = MmsValue_getElement(self->base.value.pointValue, 2);
        else
            eventTimeValue = MmsValue_getElement(self->base.value.pointValue, 3);

        MmsValue_setInt32(MmsValue_getElement(eventTimeValue, 0), eventTime / 1000);
        MmsValue_setInt16(MmsValue_getElement(eventTimeValue, 1), eventTime % 1000);
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Variable %s:%s not initialized",
                self->base.domain->name ? self->base.domain->name : "(VCC)", self->base.pointName);
    }
}


static void
appendPointToPointList(Tase2_Domain self, Tase2_DataPoint newPoint)
{
    if (self->dataPoints == NULL) {
        self->dataPoints = newPoint;
    }
    else {
        self->lastDataPoint->sibling = newPoint;
    }

    HashTree_add((HashTree)self->dataPointHashTree, newPoint->pointName, newPoint);

    self->lastDataPoint = newPoint;

    self->dataPointCount++;
}

void
Tase2_Domain_setBilateralTableId(Tase2_Domain self, const char* id)
{
    if (self->bilateralTableId)
        GLOBAL_FREEMEM(self->bilateralTableId);

    self->bilateralTableId = StringUtils_copyString(id);
}

Tase2_IndicationPoint
Tase2_Domain_addIndicationPoint(Tase2_Domain self, const char* name, Tase2_IndicationPointType type, Tase2_QualityClass qualityClass, Tase2_TimeStampClass timeStampClass, bool hasCOV, bool readOnly)
{
    Tase2_IndicationPoint newPoint = Tase2_IndicationPoint_create(self, name, type, qualityClass, timeStampClass, hasCOV, readOnly);

    if (newPoint != NULL)
        appendPointToPointList(self, (Tase2_DataPoint) newPoint);

    return newPoint;
}

static Tase2_ProtectionEquipment
Tase2_ProtectionEquipment_create(Tase2_Domain domain, const char* equipmentName, bool isPacked)
{
    Tase2_ProtectionEquipment self = (Tase2_ProtectionEquipment) GLOBAL_MALLOC(sizeof(struct sTase2_ProtectionEquipment));

    if (self != NULL) {
        self->base.pointName = StringUtils_copyString(equipmentName);


        self->base.pointType = TASE2_POINT_TYPE_PROTECTION_EVENT;

        if (isPacked)
            self->base.value.type = TASE2_VALUE_TYPE_PACKED_EVENT;
        else
            self->base.value.type = TASE2_VALUE_TYPE_SINGLE_EVENT;

        self->base.value.pointValue = NULL;
        self->base.serverCachedValue = NULL;
        self->base.sibling = NULL;
        self->base.domain = domain;
        self->base.activeTransferSets = NULL;
        self->base.valueLock = NULL;

        self->isPacked = isPacked;
    }

    return self;
}


Tase2_ProtectionEquipment
Tase2_Domain_addProtectionEquipment(Tase2_Domain self, const char* equipmentName, bool isPacked)
{
    Tase2_ProtectionEquipment newEquipment = Tase2_ProtectionEquipment_create(self, equipmentName, isPacked);

    if (newEquipment != NULL)
        appendPointToPointList(self, (Tase2_DataPoint) newEquipment);

    return newEquipment;
}


Tase2_ControlPoint
Tase2_ControlPoint_create(Tase2_Domain domain, const char* deviceName, Tase2_ControlPointType pointType, Tase2_DeviceClass deviceClass, bool hasTag, int16_t checkBackId)
{
    Tase2_ControlPoint self = (Tase2_ControlPoint) GLOBAL_MALLOC(sizeof(struct sTase2_ControlPoint));

    if (self != NULL) {
        if (deviceName != NULL)
            self->base.pointName = StringUtils_copyString(deviceName);
        else
            self->base.pointName = NULL;

        self->base.pointType = TASE2_POINT_TYPE_CONTROL;
        self->base.value.type = TASE2_VALUE_TYPE_UNKNOWN;
        self->base.value.pointValue = NULL;
        self->base.serverCachedValue = NULL;
        self->base.sibling = NULL;
        self->base.domain = domain;
        self->base.activeTransferSets = NULL;
        self->base.valueLock = NULL;

        self->pointType = pointType;
        self->deviceClass = deviceClass;
        self->hasTag = hasTag;
        self->checkBackId = checkBackId;
        self->selectedTimestamp = 0;

        self->tagOwner = NULL;

        self->tagValue = NULL;
        self->sboValue = NULL;
    }

    return self;
}

Tase2_ControlPoint
Tase2_Domain_addControlPoint(Tase2_Domain self, const char* deviceName, Tase2_ControlPointType pointType, Tase2_DeviceClass deviceClass, bool hasTag, int16_t checkBackId)
{
    Tase2_ControlPoint newPoint = Tase2_ControlPoint_create(self, deviceName, pointType, deviceClass, hasTag, checkBackId);

    if (newPoint != NULL)
        appendPointToPointList(self, (Tase2_DataPoint) newPoint);

    return newPoint;
}

Tase2_DataPoint
Tase2_Domain_getDataPoint(Tase2_Domain self, const char* pointName)
{
    return (Tase2_DataPoint)HashTree_find((HashTree)self->dataPointHashTree, pointName);
}

const char*
Tase2_Domain_getName(Tase2_Domain self)
{
    return self->name;
}

static void
appendTransferSet(Tase2_Domain self, Tase2_TransferSet newTs)
{
    if (self->transferSets == NULL)
        self->transferSets = (Tase2_TransferSet) newTs;
    else {
        Tase2_TransferSet lastTs = self->transferSets;

        while (lastTs->sibling != NULL)
            lastTs = lastTs->sibling;

        lastTs->sibling = newTs;
    }
}

static Tase2_DSTransferSet
Tase2_DSTransferSet_create(const char* name, Tase2_Domain domain)
{
    Tase2_DSTransferSet self = (Tase2_DSTransferSet) GLOBAL_MALLOC(sizeof(struct sTase2_DSTransferSet));

    if (self != NULL) {
        self->base.name = StringUtils_copyString(name);
        self->base.domain = domain;
        self->base.transferSetType = TASE2_TS_TYPE_DATA_SET;
        self->base.sibling = NULL;
        self->base.mmsValue = NULL;
        self->dataPoints = NULL;
        self->activeContextInfo = NULL;
        self->edition = TASE2_EDITION_2000_08;
    }

    return self;
}

Tase2_DSTransferSet
Tase2_Domain_addDSTransferSet(Tase2_Domain self, const char* name)
{
    /* don't allow VCC scope DS transfer sets */
    if (self->name == NULL)
        return NULL;

    Tase2_DSTransferSet newDsTransferSet = Tase2_DSTransferSet_create(name, self);

    if (newDsTransferSet != NULL)
        appendTransferSet(self, (Tase2_TransferSet) newDsTransferSet);

    return newDsTransferSet;
}

static void
appendDataSet(Tase2_Domain self, Tase2_DataSet newDataSet)
{
    if (self->dataSets == NULL)
        self->dataSets = newDataSet;
    else {
        Tase2_DataSet lastDataSet = self->dataSets;

        while (lastDataSet->sibling != NULL)
            lastDataSet = lastDataSet->sibling;

        lastDataSet->sibling = newDataSet;
    }
}

static Tase2_DataSet
Tase2_DataSet_create(const char* name)
{
    Tase2_DataSet self = (Tase2_DataSet) GLOBAL_MALLOC(sizeof(struct sTase2_DataSet));

    if (self != NULL) {
        self->name = StringUtils_copyString(name);
        self->entries = NULL;
        self->lastEntry = NULL;
        self->sibling = NULL;
    }

    return self;
}

Tase2_DataSet
Tase2_Domain_addDataSet(Tase2_Domain self, const char* dsName)
{
    Tase2_DataSet newDataSet = Tase2_DataSet_create(dsName);

    if (newDataSet != NULL)
        appendDataSet(self, newDataSet);

    return newDataSet;
}

static void
appendDataSetEntry(Tase2_DataSet self, Tase2_DataSetEntry newEntry)
{
    if (self->entries == NULL) {
        self->entries = newEntry;
        self->lastEntry = newEntry;
    }
    else {
        self->lastEntry->sibling = newEntry;
        self->lastEntry = newEntry;
    }
}

static Tase2_DataSetEntry
tase2_DataSetEntry_create(Tase2_Domain domain, const char* variableName)
{
    Tase2_DataSetEntry self = (Tase2_DataSetEntry) GLOBAL_MALLOC(sizeof(struct sTase2_DataSetEntry));

    if (self != NULL) {
        self->domain = domain;
        self->variableName = StringUtils_copyString(variableName);
        self->sibling = NULL;
    }

    return self;
}

Tase2_DataSetEntry
Tase2_DataSet_addEntry(Tase2_DataSet self, Tase2_Domain domain, const char* variableName)
{
    Tase2_DataSetEntry newDsEntry = tase2_DataSetEntry_create(domain, variableName);

    if (newDsEntry != NULL)
        appendDataSetEntry(self, newDsEntry);

    return newDsEntry;
}

void
Tase2_Domain_addInformationMessageObject(Tase2_Domain self, int maxSize)
{
    Tase2_InformationMessage infoMessage = (Tase2_InformationMessage) GLOBAL_MALLOC(sizeof(struct sTase2_InformationMessage));

    if (infoMessage) {
        infoMessage->maxSize = maxSize;
        infoMessage->sibling = NULL;
        infoMessage->infoBuf = (void*) MmsValue_newOctetString(maxSize, maxSize);

        if (self->informationMessages == NULL)
            self->informationMessages = infoMessage;
        else {
            Tase2_InformationMessage lastEntry = self->informationMessages;

            while (lastEntry->sibling != NULL)
                lastEntry = lastEntry->sibling;

            lastEntry->sibling = infoMessage;
        }
    }
}

const char*
Tase2_TransferSet_getName(Tase2_TransferSet self)
{
    return self->name;
}

Tase2_Domain
Tase2_TransferSet_getDomain(Tase2_TransferSet self)
{
    return self->domain;
}

char*
Tase2_DSTransferSet_getDataSetName(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* dataSet = MmsValue_getElement(self->base.mmsValue, 0);

        if (dataSet) {
            MmsValue* domainName = MmsValue_getElement(dataSet, 1);
            MmsValue* dataSetName = MmsValue_getElement(dataSet, 2);

            char dsNameStr[65];

            if (domainName) {
                strncpy(dsNameStr, MmsValue_toString(domainName), 64);
                dsNameStr[64] = 0;
                strncat(dsNameStr, "/", 64);
                dsNameStr[64] = 0;
            }

            if (dataSetName) {
                strncat(dsNameStr, MmsValue_toString(dataSetName), 64);
                dsNameStr[64] = 0;
            }

            if (dataSetName) {
                return StringUtils_copyString(dsNameStr);
            }
            else {
                return NULL;
            }

        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

int32_t
Tase2_DSTransferSet_getStartTime(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* startTime = MmsValue_getElement(self->base.mmsValue, 1);

        return MmsValue_toInt32(startTime);
    }

    return 0;
}

int32_t
Tase2_DSTransferSet_getInterval(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* interval = MmsValue_getElement(self->base.mmsValue, 2);

        return MmsValue_toInt32(interval);
    }

    return 0;
}

int32_t
Tase2_DSTransferSet_getTLE(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* tle = MmsValue_getElement(self->base.mmsValue, 3);

        return MmsValue_toInt32(tle);
    }

    return 0;
}


int32_t
Tase2_DSTransferSet_getBufferTime(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* bufferTime = MmsValue_getElement(self->base.mmsValue, 4);

        return MmsValue_toInt32(bufferTime);
    }

    return 0;
}

int32_t
Tase2_DSTransferSet_getIntegrityCheck(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* integrityPeriod = MmsValue_getElement(self->base.mmsValue, 5);

        return MmsValue_toInt32(integrityPeriod);
    }

    return 0;
}

bool
Tase2_DSTransferSet_getBlockData(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* blockData = MmsValue_getElement(self->base.mmsValue, 7);

        return MmsValue_getBoolean(blockData);
    }

    return false;
}

bool
Tase2_DSTransferSet_getCritical(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* critical = MmsValue_getElement(self->base.mmsValue, 8);

        return MmsValue_getBoolean(critical);
    }

    return false;
}

bool
Tase2_DSTransferSet_getRBE(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* rbe = MmsValue_getElement(self->base.mmsValue, 9);

        return MmsValue_getBoolean(rbe);
    }

    return false;
}

bool
Tase2_DSTransferSet_getAllChangesReported(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* allChangesReported = MmsValue_getElement(self->base.mmsValue, 10);

        return MmsValue_getBoolean(allChangesReported);
    }

    return false;
}

bool
Tase2_DSTransferSet_getStatus(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* status = MmsValue_getElement(self->base.mmsValue, 11);

        return MmsValue_getBoolean(status);
    }

    return false;
}

int16_t
Tase2_DSTransferSet_getEventCodeRequested(Tase2_DSTransferSet self)
{
    if (self->base.mmsValue) {
        MmsValue* eventCodeRequested = MmsValue_getElement(self->base.mmsValue, 12);

        return (int16_t)MmsValue_toInt32(eventCodeRequested);
    }

    return false;
}

