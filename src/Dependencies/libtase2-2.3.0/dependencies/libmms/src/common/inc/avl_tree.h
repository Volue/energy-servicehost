/*
 *  avl_tree.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef DEPENDENCIES_LIBMMS_SRC_COMMON_INC_AVL_TREE_H_
#define DEPENDENCIES_LIBMMS_SRC_COMMON_INC_AVL_TREE_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sAvlTree* AvlTree;

AvlTree
AvlTree_create();

bool
AvlTree_add(AvlTree self, void* key, void* value);

void
AvlTree_remove(AvlTree self, void* key);

void
AvlTree_removeAndDelete(AvlTree self, void* key, void(*valueDeleteFunction)(void*));

void*
AvlTree_find(AvlTree self, void* key);

void
AvlTree_destroy(AvlTree self);

void
AvlTree_destroyDeep(AvlTree self, void(*valueDeleteFunction)(void*));

#ifdef __cplusplus
}
#endif

#endif /* DEPENDENCIES_LIBMMS_SRC_COMMON_INC_AVL_TREE_H_ */
