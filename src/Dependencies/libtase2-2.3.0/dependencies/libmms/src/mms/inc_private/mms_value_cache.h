/*
 *  mms_value_cache.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_VARIABLE_CACHE_H_
#define MMS_VARIABLE_CACHE_H_

#include "mms_device_model.h"
#include "mms_value.h"

typedef struct sMmsValueCache* MmsValueCache;

LIBMMS_INTERNAL MmsValueCache
MmsValueCache_create(MmsDomain* domain);

LIBMMS_INTERNAL void
MmsValueCache_insertValue(MmsValueCache self, const char* itemId, MmsValue* value);

LIBMMS_INTERNAL MmsValue*
MmsValueCache_lookupValue(MmsValueCache self, const char* itemId);

LIBMMS_INTERNAL void
MmsValueCache_destroy(MmsValueCache self);

#endif /* MMS_VARIABLE_CACHE_H_ */
