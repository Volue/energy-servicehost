/*
 *  mms_value_internal.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_VALUE_INTERNAL_H_
#define MMS_VALUE_INTERNAL_H_

#include "mms_value.h"
#include "ber_integer.h"

struct ATTRIBUTE_PACKED sMmsValue {
    MmsType type;
    uint8_t deleteValue;
    union uMmsValue {
        MmsDataAccessError dataAccessError;
        struct {
            int size;
            MmsValue** components;
        } structure;
        bool boolean;
        Asn1PrimitiveValue* integer;
        struct {
            uint8_t exponentWidth;
            uint8_t formatWidth; /* number of bits - either 32 or 64)  */
            uint8_t* buf;
        } floatingPoint;
        struct {
            uint16_t size;
            uint16_t maxSize;
            uint8_t* buf;
        } octetString;
        struct {
            int size;     /* Number of bits */
            uint8_t* buf;
        } bitString;
        struct {
            char* buf;
            int16_t size; /* size of the string, equals the amount of allocated memory - 1 */
        } visibleString;
        uint8_t utcTime[8];
        struct {
            uint8_t size;
            uint8_t buf[6];
        } binaryTime;
    } value;
};


LIBMMS_INTERNAL MmsValue*
MmsValue_newIntegerFromBerInteger(Asn1PrimitiveValue* berInteger);

LIBMMS_INTERNAL MmsValue*
MmsValue_newUnsignedFromBerInteger(Asn1PrimitiveValue* berInteger);

#endif /* MMS_VALUE_INTERNAL_H_ */
