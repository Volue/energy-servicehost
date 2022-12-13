/*
 *  mms_var_access_specification.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_common.h"

MmsVariableAccessSpecification*
MmsVariableAccessSpecification_create(const char* domainId, const char* itemId)
{
    MmsVariableAccessSpecification* self = (MmsVariableAccessSpecification*)
            GLOBAL_MALLOC(sizeof(MmsVariableAccessSpecification));

    if (self) {
        self->domainId = domainId;
        self->itemId = itemId;
        self->arrayIndex = -1;
        self->componentName = NULL;
    }

    return self;
}

MmsVariableAccessSpecification*
MmsVariableAccessSpecification_createAlternateAccess(char* domainId, char* itemId, int32_t index,
        char* componentName)
{
    MmsVariableAccessSpecification* self = (MmsVariableAccessSpecification*)
            GLOBAL_MALLOC(sizeof(MmsVariableAccessSpecification));

    if (self) {
        self->domainId = domainId;
        self->itemId = itemId;
        self->arrayIndex = index;
        self->componentName = componentName;
    }

    return self;
}

void
MmsVariableAccessSpecification_setDomainId(MmsVariableAccessSpecification* self, const char* domainId)
{
    if (self->domainId) {
        GLOBAL_FREEMEM((void*)self->domainId);
        self->domainId = NULL;
    }

    if (domainId && domainId[0]) {
        self->domainId = StringUtils_copyString(domainId);
    }
}

void
MmsVariableAccessSpecification_setItemId(MmsVariableAccessSpecification* self, const char* itemId)
{
    if (self->itemId) {
        GLOBAL_FREEMEM((void*)self->itemId);
        self->itemId = NULL;
    }

    if (itemId && itemId[0]) {
        self->itemId = StringUtils_copyString(itemId);
    }
}

void
MmsVariableAccessSpecification_setComponentName(MmsVariableAccessSpecification* self, const char* componentName)
{
    if (self->componentName) {
        GLOBAL_FREEMEM((void*)self->componentName);
        self->componentName = NULL;
    }

    if (componentName && componentName[0]) {
        self->componentName = StringUtils_copyString(componentName);
    }
}

void
MmsVariableAccessSpecification_destroy(MmsVariableAccessSpecification* self)
{
    if (self) {
        if (self->domainId != NULL)
            GLOBAL_FREEMEM((void*) self->domainId);

        if (self->itemId != NULL)
            GLOBAL_FREEMEM((void*) self->itemId);

        if (self->componentName != NULL)
            GLOBAL_FREEMEM((void*) self->componentName);

        GLOBAL_FREEMEM(self);
    }
}
