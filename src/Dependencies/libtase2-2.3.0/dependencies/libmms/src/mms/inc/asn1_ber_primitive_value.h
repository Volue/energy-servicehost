/*
 *  asn1_ber_primitive_value.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef ASN1_BER_PRIMITIVE_VALUE_H_
#define ASN1_BER_PRIMITIVE_VALUE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "libmms_common_api.h"

typedef struct ATTRIBUTE_PACKED {
	uint8_t size;
	uint8_t maxSize;
	uint8_t* octets;
} Asn1PrimitiveValue;

Asn1PrimitiveValue*
Asn1PrimitiveValue_create(int size);

int
Asn1PrimitiveValue_getSize(Asn1PrimitiveValue* self);

int
Asn1PrimitiveValue_getMaxSize(Asn1PrimitiveValue* self);

Asn1PrimitiveValue*
Asn1PrimitiveValue_clone(Asn1PrimitiveValue* self);

bool
Asn1PrimitivaValue_compare(Asn1PrimitiveValue* self, Asn1PrimitiveValue* otherValue);

void
Asn1PrimitiveValue_destroy(Asn1PrimitiveValue* self);

#ifdef __cplusplus
}
#endif

#endif /* ASN1_BER_PRIMITIVE_VALUE_H_ */
