/*
 *  mem_alloc_linked_list.h
 *
 *  Implementation of linked list (LinkedList) that uses the provided memory buffer as
 *  memory pool for storage allocation.
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MEM_ALLOC_LINKED_LIST_H_
#define MEM_ALLOC_LINKED_LIST_H_

#include "libmms_common_api.h"
#include "linked_list.h"
#include "simple_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sMemAllocLinkedList* MemAllocLinkedList;

MemAllocLinkedList
MemAllocLinkedList_create(MemoryAllocator* ma);

LinkedList
MemAllocLinkedList_add(MemAllocLinkedList list, void* data);

#endif /* MEM_ALLOC_LINKED_LIST_H_ */

#ifdef __cplusplus
}
#endif
