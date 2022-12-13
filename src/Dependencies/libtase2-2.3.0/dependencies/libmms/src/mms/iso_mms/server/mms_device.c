/*
 *  mms_device.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_server_internal.h"
#include "mms_device_model.h"
#include "stack_config.h"

MmsDevice*
MmsDevice_create(char* deviceName)
{
    MmsDevice* self = (MmsDevice*) GLOBAL_CALLOC(1, sizeof(MmsDevice));
    self->deviceName = deviceName;

    self->namedVariableLists = LinkedList_create();
    self->namedVariablesHashTree = HashTree_create();

    return self;
}

void
MmsDevice_destroy(MmsDevice* self)
{

    int i;
    for (i = 0; i < self->domainCount; i++) {
        MmsDomain_destroy(self->domains[i]);
    }

#if (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1)
    if (self->namedVariables != NULL) {
        for (i = 0; i < self->namedVariablesCount; i++) {
            MmsVariableSpecification_destroy(self->namedVariables[i]);
        }

        GLOBAL_FREEMEM(self->namedVariables);
    }
#endif /* (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1) */

    HashTree_destroy(self->namedVariablesHashTree);

    LinkedList_destroyDeep(self->namedVariableLists, (LinkedListValueDeleteFunction) MmsNamedVariableList_destroy);

    GLOBAL_FREEMEM(self->domains);
    GLOBAL_FREEMEM(self);
}

MmsDomain*
MmsDevice_getDomain(MmsDevice* self, const char* domainId)
{
    int i;

    for (i = 0; i < self->domainCount; i++) {
        if (strcmp(self->domains[i]->domainName, domainId) == 0) {
            return self->domains[i];
        }
    }

    return NULL;
}

#if (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1)
MmsVariableSpecification*
MmsDevice_getNamedVariable(MmsDevice* self, const char* variableName)
{
    if (self->namedVariablesHashTree) {

        MmsVariableSpecification* varSpec = NULL;

#if (CONFIG_MMS_SUPPORT_FLATTED_NAME_SPACE == 1)
        const char* separator = strchr(variableName, CONFIG_MMS_SEPARATOR_CHAR);

        if (separator) {
            char varName[130];

            int varNameLen = separator - variableName;

            memcpy(varName, variableName, varNameLen);

            varName[varNameLen] = 0;

            MmsVariableSpecification* namedVariable = (MmsVariableSpecification*) HashTree_find(self->namedVariablesHashTree, varName);

            if (namedVariable != NULL) {
                varSpec = MmsVariableSpecification_getNamedVariableRecursive(namedVariable, separator + 1);
            }
        }
        else {
            varSpec = (MmsVariableSpecification*) HashTree_find(self->namedVariablesHashTree, variableName);
        }
#else
        varSpec = (MmsVariableSpecification*) HashTree_find(self->namedVariablesHashTree, variableName);
#endif

        return varSpec;
    }

    return NULL;
}
#endif /* (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1) */

LinkedList
MmsDevice_getNamedVariableLists(MmsDevice* self)
{
    return self->namedVariableLists;
}

void
MmsDevice_addNamedVariableList(MmsDevice* self, MmsNamedVariableList variableList)
{
    if (self->namedVariableLists) {
        LinkedList_add(self->namedVariableLists, variableList);
    }
}

MmsNamedVariableList
MmsDevice_getNamedVariableListWithName(MmsDevice* self, const char* variableListName)
{
    return mmsServer_getNamedVariableListWithName(self->namedVariableLists, variableListName);
}

