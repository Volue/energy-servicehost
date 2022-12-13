/*
 *  hash_tree.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef DEPENDENCIES_LIBMMS_SRC_COMMON_INC_HASH_TREE_H_
#define DEPENDENCIES_LIBMMS_SRC_COMMON_INC_HASH_TREE_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sHashTree* HashTree;

HashTree
HashTree_create(void);

bool
HashTree_add(HashTree self, const char* key, void* value);

uint32_t
HashTree_getMaxDepth(HashTree self);

void*
HashTree_find(HashTree self, const char* key);

void
HashTree_destroy(HashTree self);

void
HashTree_destroyDeep(HashTree self, void(*valueDeleteFunction)(void*));

#ifdef __cplusplus
}
#endif

#endif /* DEPENDENCIES_LIBMMS_SRC_COMMON_INC_HASH_TREE_H_ */
