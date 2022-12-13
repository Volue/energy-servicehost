/*
 *  simple_allocator.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "simple_allocator.h"

#include "libmms_platform_includes.h"
#include "stack_config.h"

void
MemoryAllocator_init(MemoryAllocator* self, char* memoryBlock, int size)
{
    self->memoryBlock = memoryBlock;
    self->currentPtr = memoryBlock;
    self->size = size;
}

int
MemoryAllocator_getAlignedSize(int size)
{
#if (CONFIG_IEC61850_FORCE_MEMORY_ALIGNMENT == 1)
    if ((size % sizeof(void*)) > 0)
        return sizeof(void*) * ((size + sizeof(void*) - 1) / sizeof(void*));
    else
        return size;
#else
    return size;
#endif
}

char*
MemoryAllocator_allocate(MemoryAllocator* self, int size)
{
    size = MemoryAllocator_getAlignedSize(size);

    if (((self->currentPtr - self->memoryBlock) + size) <= self->size) {
        char* ptr = self->currentPtr;
        self->currentPtr += size;
        return ptr;
    }
    else
        return NULL;
}
