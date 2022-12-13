/*
 *  mms_journal.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_device_model.h"
#include "mms_server_internal.h"

MmsJournal
MmsJournal_create(const char* name)
{
    if (DEBUG_MMS_SERVER)
        printf("MMS_SERVER: create new journal %s\n", name);

    MmsJournal self = (MmsJournal) GLOBAL_MALLOC(sizeof(struct sMmsJournal));

    self->name = StringUtils_copyString(name);

    return self;
}

void
MmsJournal_destroy(MmsJournal self)
{
    GLOBAL_FREEMEM(self->name);
    GLOBAL_FREEMEM(self);
}
