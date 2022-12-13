/*
 *  buffer_chain.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef BUFFER_CHAIN_H_
#define BUFFER_CHAIN_H_

#include <stdint.h>

typedef struct sBufferChain* BufferChain;

struct sBufferChain {
    int length;
    int partLength;
    int partMaxLength;
    uint8_t* buffer;
    BufferChain nextPart;
};

void
BufferChain_init(BufferChain self, int length, int partLength, BufferChain nextPart, uint8_t* buffer);

void
BufferChain_destroy(BufferChain self);

int /* returns the number of bytes written to the buffer */
BufferChain_dumpToBuffer(BufferChain self, uint8_t* buffer, int bufferMaxSize);

#endif /* BUFFER_CHAIN_H_ */
