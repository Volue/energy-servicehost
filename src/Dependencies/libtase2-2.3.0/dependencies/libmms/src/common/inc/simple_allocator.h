/*
 *  simple_allocator.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef SIMPLE_ALLOCATOR_H_
#define SIMPLE_ALLOCATOR_H_

#include "libmms_platform_includes.h"

typedef struct {
    char* memoryBlock;
    char* currentPtr;
    int size;
} MemoryAllocator;

LIBMMS_INTERNAL void
MemoryAllocator_init(MemoryAllocator* self, char* memoryBlock, int size);

LIBMMS_INTERNAL int
MemoryAllocator_getAlignedSize(int size);

LIBMMS_INTERNAL char*
MemoryAllocator_allocate(MemoryAllocator* self, int size);

#endif /* SIMPLE_ALLOCATOR_H_ */
