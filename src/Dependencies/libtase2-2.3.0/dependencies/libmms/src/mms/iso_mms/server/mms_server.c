/*
 *  mms_server.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "mms_server.h"

#include "libmms_platform_includes.h"
#include "mms_endpoint_internal.h"
#include "mms_value_cache.h"
#include "mms_server_internal.h"
#include "iso_server_private.h"

static AvlTree
createValueCaches(MmsDevice* device)
{
    AvlTree valueCaches = AvlTree_create();

    int i;
    for (i = 0; i < device->domainCount; i++) {
        MmsValueCache valueCache = MmsValueCache_create(device->domains[i]);

        AvlTree_add(valueCaches, device->domains[i], valueCache);
    }

#if (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1)
    MmsValueCache valueCache = MmsValueCache_create((MmsDomain*) device);
    AvlTree_add(valueCaches, device, valueCache);
#endif

    return valueCaches;
}

MmsServer
MmsServer_create(MmsDevice* device)
{
    MmsServer self = (MmsServer) GLOBAL_CALLOC(1, sizeof(struct sMmsServer));

    self->device = device;
    self->valueCaches = createValueCaches(device);

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    self->modelMutex = Semaphore_create(1);
#endif

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
    self->fileServiceEnabled = true;
    self->dynamicVariableListServiceEnabled = true;
    self->journalServiceEnabled = true;
    self->maxDataSetEntries = CONFIG_MMS_MAX_NUMBER_OF_DATA_SET_MEMBERS;
    self->maxAssociationSpecificDataSets = CONFIG_MMS_MAX_NUMBER_OF_ASSOCIATION_SPECIFIC_DATA_SETS;
    self->maxDomainSpecificDataSets = CONFIG_MMS_MAX_NUMBER_OF_DOMAIN_SPECIFIC_DATA_SETS;
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

    return self;
}

bool
MmsServer_isRunning(MmsServer self)
{
    return MmsEndpoint_isRunning(self->endpoint);
}

void
MmsServer_lockModel(MmsServer self)
{
#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_wait(self->modelMutex);
#endif
}

void
MmsServer_unlockModel(MmsServer self)
{
#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_post(self->modelMutex);
#endif
}

IsoApplicationAddress
MmsServer_getLocalApplicationAddress(MmsServer self)
{
    return &(self->localApAddress);
}


void
MmsServer_installReadHandler(MmsServer self, MmsReadVariableHandler readHandler, void* parameter)
{
    self->readHandler = readHandler;
    self->readHandlerParameter = parameter;
}

void
MmsServer_installReadAccessHandler(MmsServer self, MmsReadAccessHandler readAccessHandler, void* parameter)
{
    self->readAccessHandler = readAccessHandler;
    self->readAccessHandlerParameter = parameter;
}

void
MmsServer_installWriteHandler(MmsServer self, MmsWriteVariableHandler writeHandler, void* parameter)
{
    self->writeHandler = writeHandler;
    self->writeHandlerParameter = parameter;
}

void
MmsServer_installVariableListChangedHandler(MmsServer self, MmsNamedVariableListChangedHandler handler, void* parameter)
{
    self->variableListChangedHandler = handler;
    self->variableListChangedHandlerParameter = parameter;
}

void
MmsServer_installReportHandler(MmsServer self, MmsInformationReportHandler handler, void* parameter)
{
    self->reportHandler = handler;
    self->reportHandlerParameter = parameter;
}

void
MmsServer_installAASpecificVarSpecHandler(MmsServer self, MmsVarSpecHandler handler, void* parameter)
{
    self->aaVarSpecHandler = handler;
    self->aaVarSpecHandlerParameter = parameter;
}

static void
deleteSingleCache(MmsValueCache cache)
{
    MmsValueCache_destroy(cache);
}

void
MmsServer_destroy(MmsServer self)
{
    AvlTree_destroyDeep(self->valueCaches, (void (*) (void*)) deleteSingleCache);

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_destroy(self->modelMutex);
#endif

    GLOBAL_FREEMEM(self);
}

MmsValue*
MmsServer_getValueFromCache(MmsServer self, MmsDomain* domain, const char* itemId)
{
    MmsValueCache cache = (MmsValueCache) AvlTree_find(self->valueCaches, domain);

    if (cache)
        return MmsValueCache_lookupValue(cache, itemId);

    return NULL ;
}

void
MmsServer_insertIntoCache(MmsServer self, MmsDomain* domain, const char* itemId, MmsValue* value)
{
    MmsValueCache cache = (MmsValueCache) AvlTree_find(self->valueCaches, domain);

    if (cache) {
        MmsValueCache_insertValue(cache, itemId, value);
    }
}

MmsDataAccessError
mmsServer_setComponentValue(MmsServer self, MmsObjectScope objectScope, MmsDomain* domain, const char* itemId, const char* componentName, int componentIndex,
        MmsValue* value, MmsEndpointConnection connection)
{
    MmsDataAccessError indication;

    if (self->writeHandler != NULL) {
        indication = self->writeHandler(self->writeHandlerParameter, objectScope, domain,
                itemId, componentName, value, connection);
    } else {
        MmsValue* cachedValue;

        if (domain == NULL)
            domain = (MmsDomain*) self->device;

        cachedValue = MmsServer_getValueFromCache(self, domain, itemId);

        if (cachedValue != NULL) {

            /* get child value */
            MmsValue* componentValue = MmsValue_getElement(cachedValue, componentIndex);

            MmsValue_update(componentValue, value);
            indication = DATA_ACCESS_ERROR_SUCCESS;

        } else
            indication = DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;
    }

    return indication;
}

MmsDataAccessError
mmsServer_setValue(MmsServer self, MmsObjectScope objectScope, MmsDomain* domain, const char* itemId, MmsValue* value,
        MmsEndpointConnection connection)
{
    MmsDataAccessError indication;

    if (self->writeHandler != NULL) {
        indication = self->writeHandler(self->writeHandlerParameter, objectScope, domain,
                itemId, NULL, value, connection);
    } else {
        MmsValue* cachedValue;

        if (domain == NULL)
            domain = (MmsDomain*) self->device;

        cachedValue = MmsServer_getValueFromCache(self, domain, itemId);

        if (cachedValue != NULL) {
            MmsValue_update(cachedValue, value);
            indication = DATA_ACCESS_ERROR_SUCCESS;
        } else
            indication = DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;
    }

    return indication;
}


MmsValue*
mmsServer_getValue(MmsServer self, MmsDomain* domain, char* itemId, MmsEndpointConnection connection)
{
    MmsValue* value = NULL;

    if (self->readAccessHandler != NULL) {
        MmsDataAccessError accessError =
                self->readAccessHandler(self->readAccessHandlerParameter, (domain == (MmsDomain*) self->device) ? NULL : domain,
                        itemId, connection);

        if (accessError != DATA_ACCESS_ERROR_SUCCESS) {
            value = MmsValue_newDataAccessError(accessError);
            MmsValue_setDeletable(value);
            goto exit_function;
        }
    }

    value = MmsServer_getValueFromCache(self, domain, itemId);

    if (value == NULL)
        if (self->readHandler != NULL)
            value = self->readHandler(self->readHandlerParameter, (domain == (MmsDomain*) self->device) ? NULL : domain,
                    itemId, connection);

exit_function:
    return value;
}


MmsDevice*
MmsServer_getDevice(MmsServer self)
{
    return self->device;
}
