/*
 *  ber_integer.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef BER_INTEGER_H_
#define BER_INTEGER_H_

#include "asn1_ber_primitive_value.h"

#ifdef __cplusplus
extern "C" {
#endif

Asn1PrimitiveValue*
BerInteger_createFromBuffer(uint8_t* buf, int size);

Asn1PrimitiveValue*
BerInteger_createInt32(void);

int
BerInteger_setFromBerInteger(Asn1PrimitiveValue* self, Asn1PrimitiveValue* value);

int
BerInteger_setInt32(Asn1PrimitiveValue* self, int32_t value);

Asn1PrimitiveValue*
BerInteger_createFromInt32(int32_t value);

int
BerInteger_setUint8(Asn1PrimitiveValue* self, uint8_t value);

int
BerInteger_setUint16(Asn1PrimitiveValue* self, uint16_t value);

int
BerInteger_setUint32(Asn1PrimitiveValue* self, uint32_t value);

Asn1PrimitiveValue*
BerInteger_createFromUint32(uint32_t value);

Asn1PrimitiveValue*
BerInteger_createFromInt64(int64_t value);

Asn1PrimitiveValue*
BerInteger_createInt64(void);

int
BerInteger_setInt64(Asn1PrimitiveValue* self, int64_t value);

int /* 1 - if conversion is possible, 0 - out of range */
BerInteger_toInt32(Asn1PrimitiveValue* self, int32_t* nativeValue);

int /* 1 - if conversion is possible, 0 - out of range */
BerInteger_toUint32(Asn1PrimitiveValue* self, uint32_t* nativeValue);

int /* 1 - if conversion is possible, 0 - out of range */
BerInteger_toInt64(Asn1PrimitiveValue* self, int64_t* nativeValue);

#ifdef __cplusplus
}
#endif

#endif /* BER_INTEGER_H_ */
