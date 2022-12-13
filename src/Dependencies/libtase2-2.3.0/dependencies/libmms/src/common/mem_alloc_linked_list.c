/*
 *  mem_alloc_linked_list.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "mem_alloc_linked_list.h"
#include "libmms_platform_includes.h"

struct sMemAllocLinkedList {
    void* data;
    struct sLinkedList* next;
    MemoryAllocator* ma;
};

MemAllocLinkedList
MemAllocLinkedList_create(MemoryAllocator* ma)
{
    MemAllocLinkedList self =
            (MemAllocLinkedList) MemoryAllocator_allocate(ma, sizeof(struct sMemAllocLinkedList));

    if (self == NULL)
        return NULL;

    self->ma = ma;
    self->data = NULL;
    self->next = NULL;

    return self;
}

LinkedList
MemAllocLinkedList_add(MemAllocLinkedList self, void* data)
{
    LinkedList newElement = (LinkedList)
            MemoryAllocator_allocate(self->ma, sizeof(struct sLinkedList));

    if (newElement == NULL)
        return NULL;

    newElement->data = data;
    newElement->next = NULL;

    LinkedList listEnd = LinkedList_getLastElement((LinkedList) self);

    listEnd->next = newElement;

    return newElement;
}

