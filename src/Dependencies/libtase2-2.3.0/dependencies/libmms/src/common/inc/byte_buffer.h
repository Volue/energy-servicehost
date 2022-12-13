/*
 *  byte_buffer.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef BYTE_BUFFER_H_
#define BYTE_BUFFER_H_

#include "libmms_common_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t* buffer;
    int maxSize;
    int size;
} ByteBuffer;

ByteBuffer*
ByteBuffer_create(ByteBuffer* self, int maxSize);

void
ByteBuffer_destroy(ByteBuffer* self);

void
ByteBuffer_wrap(ByteBuffer* self, uint8_t* buf, int size, int maxSize);

int
ByteBuffer_append(ByteBuffer* self, uint8_t* data, int dataSize);

int
ByteBuffer_appendByte(ByteBuffer* self, uint8_t byte);

uint8_t*
ByteBuffer_getBuffer(ByteBuffer* self);

int
ByteBuffer_getSize(ByteBuffer* self);

int
ByteBuffer_getMaxSize(ByteBuffer* self);

int
ByteBuffer_setSize(ByteBuffer* self, int size);

void
ByteBuffer_print(ByteBuffer* self, char* message);

#ifdef __cplusplus
}
#endif


#endif /* BYTE_BUFFER_H_ */
