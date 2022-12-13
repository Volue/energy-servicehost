/*
 *  tase2_blt.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "tase2_server_internal.h"

Tase2_BilateralTable
Tase2_BilateralTable_create(const char* bltId, Tase2_Domain domain, const char* apTitle, int aeQualifier)
{
    Tase2_BilateralTable self = (Tase2_BilateralTable) GLOBAL_MALLOC(sizeof(struct sTase2_BilateralTable));

    if (self) {
        self->bltId = StringUtils_copyString(bltId);
        self->domain = domain;

        memset(&(self->appAddr), 0, sizeof(struct sIsoApplicationAddress));

        self->appAddr.aeQualifier = aeQualifier;

        ItuObjectIdentifier_setFromString(&(self->appAddr.apTitle), apTitle);

        self->dataPoints = AvlTree_create();

        self->controlPoints = AvlTree_create();

        self->informationMessages = NULL;

        if (domain)
            Tase2_Domain_setBilateralTableId(domain, self->bltId);
    }

    return self;
}

/* internal function. Only called by Tase2_Server */
void
Tase2_BilateralTable_destroy(Tase2_BilateralTable self)
{
    GLOBAL_FREEMEM(self->bltId);

    AvlTree_destroy(self->dataPoints);
    AvlTree_destroy(self->controlPoints);

    Tase2_BLT_InfoMessageEntry imEntry = self->informationMessages;

    while (imEntry) {
        Tase2_BLT_InfoMessageEntry next = imEntry->next;
        GLOBAL_FREEMEM(imEntry);
        imEntry = next;
    }

    GLOBAL_FREEMEM(self);
}

void
Tase2_BilateralTable_setIsoParameters(Tase2_BilateralTable self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector)
{
    PSelector pSel;
    SSelector sSel;
    TSelector tSel;

    memcpy(&pSel, &pSelector, sizeof(PSelector));
    memcpy(&sSel, &sSelector, sizeof(SSelector));
    memcpy(&tSel, &tSelector, sizeof(Tase2_TSelector));

    IsoApplicationAddress_setLowerLayerAddresses(&(self->appAddr), pSel, sSel, tSel);
}

const char*
Tase2_BilateralTable_getID(Tase2_BilateralTable self)
{
    return self->bltId;
}

static void
appendEntryToDataPointList(Tase2_BilateralTable self, Tase2_BLT_Entry newEntry)
{
    AvlTree_add(self->dataPoints, newEntry->dataPoint, newEntry);
}

static void
appendEntryToInformationMessageList(Tase2_BilateralTable self, Tase2_BLT_InfoMessageEntry newEntry)
{
    if (self->informationMessages == NULL)
        self->informationMessages = newEntry;
    else {
        Tase2_BLT_InfoMessageEntry lastEntry = self->informationMessages;

        while (lastEntry->next != NULL)
            lastEntry = lastEntry->next;

        lastEntry->next = newEntry;
    }
}


void
Tase2_BilateralTable_addDataPoint(Tase2_BilateralTable self, Tase2_DataPoint dataPoint, bool allowRead, bool allowWrite)
{
    Tase2_BLT_Entry newEntry = (Tase2_BLT_Entry) GLOBAL_MALLOC(sizeof(struct sTase2_BLT_Entry));

    if (newEntry) {
        newEntry->dataPoint = dataPoint;

        newEntry->accessFlags = 0;

        if (allowRead)
            newEntry->accessFlags += BLT_ACCESS_ALLOW_READ;

        if (allowWrite)
            newEntry->accessFlags += BLT_ACCESS_ALLOW_WRITE;

        newEntry->next = NULL;

        appendEntryToDataPointList(self, newEntry);
    }
}

/* internal function. Only called by Tase2_Server */
bool
BilateralTable_checkDataPointAccess(Tase2_BilateralTable self, Tase2_Domain domain, Tase2_DataPoint dataPoint, int checkFlags)
{
    if (domain == self->domain)
        return true;

    Tase2_BLT_Entry entry = AvlTree_find(self->dataPoints, dataPoint);

    if (entry) {
        if (entry->accessFlags & checkFlags)
            return true;
    }

    entry = AvlTree_find(self->controlPoints, dataPoint);

    if (entry) {
        if (entry->accessFlags & checkFlags)
            return true;
    }

    return false;
}

static void
appendEntryToControlPointList(Tase2_BilateralTable self, Tase2_BLT_Entry newEntry)
{
    AvlTree_add(self->controlPoints, newEntry->dataPoint, newEntry);
}

void
Tase2_BilateralTable_addControlPoint(Tase2_BilateralTable self, Tase2_ControlPoint controlPoint, int16_t checkBackId, bool allowSetTag, bool allowGetTag, bool allowOperate, bool allowRead)
{
    Tase2_BLT_Entry newEntry = (Tase2_BLT_Entry) GLOBAL_MALLOC(sizeof(struct sTase2_BLT_Entry));

    if (newEntry) {
        newEntry->dataPoint = (Tase2_DataPoint) controlPoint;

        newEntry->accessFlags = 0;

        newEntry->checkBackId = checkBackId;

        if (allowRead)
            newEntry->accessFlags += BLT_ACCESS_ALLOW_READ;

        if (allowOperate)
            newEntry->accessFlags += BLT_ACCESS_ALLOW_OPERATE;

        if (allowSetTag)
            newEntry->accessFlags += BLT_ACCESS_ALLOW_SET_TAG;

        if (allowGetTag)
            newEntry->accessFlags += BLT_ACCESS_ALLOW_GET_TAG;

        newEntry->next = NULL;

        appendEntryToControlPointList(self, newEntry);
    }
}

void
Tase2_BilateralTable_addInformationMessage(Tase2_BilateralTable self, int32_t infoRef, int32_t localRef, bool vccScope)
{
    Tase2_BLT_InfoMessageEntry newEntry = (Tase2_BLT_InfoMessageEntry) GLOBAL_MALLOC(sizeof(struct sTase2_BLT_InfoMessageEntry));

    if (newEntry) {
        newEntry->infoRef = infoRef;
        newEntry->localRef = localRef;
        newEntry->vccScope = vccScope;

        newEntry->next = NULL;

        appendEntryToInformationMessageList(self, newEntry);
    }
}

/* internal function. Only called by Tase2_Server */
bool
BilateralTable_checkInformationMessageAccess(Tase2_BilateralTable self, Tase2_Domain domain, int32_t infoRef, int32_t localRef)
{
    if (domain == self->domain)
        return true;

    Tase2_BLT_InfoMessageEntry entry = self->informationMessages;

    while (entry) {

        if ((domain == NULL) && (entry->vccScope == true)) {
            if ((entry->infoRef == infoRef) && (entry->localRef == localRef))
                return true;
        }
        else if ((domain == self->domain) && (entry->vccScope == false)) {
            if ((entry->infoRef == infoRef) && (entry->localRef == localRef))
                return true;
        }

        entry = entry->next;
    }

    return false;
}

/* internal function. Only called by Tase2_Server */
bool
BilateralTable_checkControlPointAccess(Tase2_BilateralTable self, Tase2_Domain domain, Tase2_ControlPoint dataPoint, int checkFlags)
{
    if (domain == self->domain)
        return true;

    Tase2_BLT_Entry entry = AvlTree_find(self->controlPoints, dataPoint);

    if (entry) {
        if ((entry->accessFlags & checkFlags) == checkFlags)
            return true;
    }

    return false;
}

/* internal function. Only called by Tase2_Server */
int16_t
BilateralTable_getCheckBackId(Tase2_BilateralTable self, Tase2_Domain domain, Tase2_ControlPoint dataPoint)
{
    int16_t checkBackId = -1;

    Tase2_BLT_Entry entry = AvlTree_find(self->controlPoints, dataPoint);

    if (entry) {
        checkBackId = entry->checkBackId;
    }

    return checkBackId;
}
