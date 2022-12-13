/*
 *  mms_value_cache.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "mms_value_cache.h"

#include "libmms_platform_includes.h"
#include "stack_config.h"

struct sMmsValueCache {
	MmsDomain* domain;
	HashTree cachedValues;
};

typedef struct sMmsValueCacheEntry {
	MmsValue* value;
	MmsVariableSpecification* typeSpec;
} MmsValueCacheEntry;

MmsValueCache
MmsValueCache_create(MmsDomain* domain)
{
	MmsValueCache self = (MmsValueCache) GLOBAL_CALLOC(1, sizeof(struct sMmsValueCache));

	if (self) {
	    self->domain = domain;
	    self->cachedValues = HashTree_create();
	}

	return self;
}

void
MmsValueCache_insertValue(MmsValueCache self, const char* itemId, MmsValue* value)
{
	MmsVariableSpecification* typeSpec = MmsDomain_getNamedVariable(self->domain, itemId);

	if (typeSpec != NULL) {
		MmsValueCacheEntry* cacheEntry = (MmsValueCacheEntry*) GLOBAL_MALLOC(sizeof(MmsValueCacheEntry));

		cacheEntry->value = value;
		cacheEntry->typeSpec = typeSpec;

		HashTree_add(self->cachedValues, itemId, cacheEntry);
	}
	else
		if (DEBUG) printf("Cannot insert value into cache %s : no typeSpec found!\n", itemId);
}

static char*
getParentSubString(char* itemId)
{
	int len = (int) strlen(itemId);

	char* strPos = itemId + len;

	while (--strPos > itemId) {
		if (*strPos == CONFIG_MMS_SEPARATOR_CHAR) {
			*strPos = 0;
			return itemId;
		}
	}

	return NULL;
}

static const char*
getChildSubString (const char* itemId, char* parentId)
{
	return itemId + strlen(parentId) + 1;
}

static MmsValue*
searchCacheForValue(MmsValueCache self, const char* itemId, char* parentId)
{
	MmsValueCacheEntry* cacheEntry;
	MmsValue* value = NULL;

	cacheEntry = (MmsValueCacheEntry*) HashTree_find(self->cachedValues, parentId);

	if (cacheEntry == NULL) {
		char* parentItemId = getParentSubString(parentId);

		if (parentItemId != NULL) {
			value = searchCacheForValue(self, itemId, parentItemId);
		}
	}
	else {

		const char* childId = getChildSubString(itemId, parentId);

		MmsVariableSpecification* typeSpec = MmsDomain_getNamedVariable(self->domain, parentId);
		value = MmsVariableSpecification_getChildValue(typeSpec, cacheEntry->value, childId);
	}

	return value;
}

MmsValue*
MmsValueCache_lookupValue(MmsValueCache self, const char* itemId)
{
	/* get value for first matching key substring!
	 * Then iterate the value for the exact value.
     */
	MmsValue* value = NULL;

	MmsValueCacheEntry* cacheEntry;

	cacheEntry = (MmsValueCacheEntry*) HashTree_find(self->cachedValues, itemId);

#if (CONFIG_MMS_SUPPORT_FLATTED_NAME_SPACE == 1)
	if (cacheEntry == NULL) {
		char* itemIdCopy = StringUtils_copyString(itemId);
		char* parentItemId = getParentSubString(itemIdCopy);

		if (parentItemId != NULL) {
			value = searchCacheForValue(self, itemId, parentItemId);
		}

		GLOBAL_FREEMEM(itemIdCopy);
	}
#endif

	if (cacheEntry != NULL)
		return cacheEntry->value;
	else
		return value;
}

static void
cacheEntryDelete(MmsValueCacheEntry* entry)
{
	if (entry != NULL) {
		MmsValue_delete(entry->value);
		GLOBAL_FREEMEM(entry);
	}
}

void
MmsValueCache_destroy(MmsValueCache self)
{
	HashTree_destroyDeep(self->cachedValues, (LinkedListValueDeleteFunction) cacheEntryDelete);

	GLOBAL_FREEMEM(self);
}
