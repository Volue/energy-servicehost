/*
 *  asn1_ber_primitive_value.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */


#include "asn1_ber_primitive_value.h"
#include "libmms_platform_includes.h"

Asn1PrimitiveValue*
Asn1PrimitiveValue_create(int size)
{
	Asn1PrimitiveValue* self = (Asn1PrimitiveValue*) GLOBAL_MALLOC(sizeof(Asn1PrimitiveValue));

	self->size = 1;
	self->maxSize = size;
	self->octets = (uint8_t*) GLOBAL_CALLOC(1, size);

	return self;
}

Asn1PrimitiveValue*
Asn1PrimitiveValue_clone(Asn1PrimitiveValue* self)
{
	Asn1PrimitiveValue* clone = (Asn1PrimitiveValue*) GLOBAL_MALLOC(sizeof(Asn1PrimitiveValue));

	clone->size = self->size;
	clone->maxSize = self->maxSize;

	clone->octets = (uint8_t*) GLOBAL_MALLOC(self->maxSize);

	memcpy(clone->octets, self->octets, clone->maxSize);

	return clone;
}

bool
Asn1PrimitivaValue_compare(Asn1PrimitiveValue* self, Asn1PrimitiveValue* otherValue)
{
    if (self->size == otherValue->size) {
        if (memcmp(self->octets, otherValue->octets, self->size) == 0)
            return true;
        else
            return false;
    }
    else
        return false;
}

int
Asn1PrimitiveValue_getSize(Asn1PrimitiveValue* self)
{
	return self->size;
}

int
Asn1PrimitiveValue_getMaxSize(Asn1PrimitiveValue* self)
{
	return self->maxSize;
}

void
Asn1PrimitiveValue_destroy(Asn1PrimitiveValue* self)
{
    GLOBAL_FREEMEM(self->octets);
    GLOBAL_FREEMEM(self);
}
