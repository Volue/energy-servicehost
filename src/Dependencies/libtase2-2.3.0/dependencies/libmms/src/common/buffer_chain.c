/*
 *  buffer_chain.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "buffer_chain.h"
#include "libmms_platform_includes.h"

void
BufferChain_init(BufferChain self, int length, int partLength, BufferChain nextPart, uint8_t* buffer)
{
    self->length = length;
    self->partLength = partLength;
    self->partMaxLength = partLength;
    self->nextPart = nextPart;
    self->buffer = buffer;
}

void
BufferChain_destroy(BufferChain self)
{
    BufferChain currentChainElement = self;

    while (currentChainElement != NULL) {
        BufferChain nextChainElement = currentChainElement->nextPart;
        GLOBAL_FREEMEM(currentChainElement);
        currentChainElement = nextChainElement;
    }
}

int /* returns the number of bytes written to the buffer */
BufferChain_dumpToBuffer(BufferChain self, uint8_t* buffer, int bufferMaxSize)
{
    if (self->length > bufferMaxSize)
        return 0;

    BufferChain currentChain = self;

    int currentBufferIndex = 0;

    do {
        int currentChainIndex = 0;
        int currentPartLength = self->partLength;

        while (currentChainIndex < currentPartLength)
            buffer[currentBufferIndex++] = self->buffer[currentChainIndex++];

        currentChain = currentChain->nextPart;
    } while (currentChain != NULL);

    return currentBufferIndex;
}

