/*
 *  mms_types.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_TYPES_H_
#define MMS_TYPES_H_

#include "libmms_common_api.h"

typedef enum  {
    MMS_VALUE_NO_RESPONSE,
	MMS_VALUE_OK,
	MMS_VALUE_ACCESS_DENIED,
	MMS_VALUE_VALUE_INVALID,
	MMS_VALUE_TEMPORARILY_UNAVAILABLE,
	MMS_VALUE_OBJECT_ACCESS_UNSUPPORTED
} MmsValueIndication;

/**
 * \addtogroup MMS_VAR_SPEC
 */
/**@{*/

/**
 * Type definition for MMS Named Variables
 */
typedef struct sMmsVariableSpecification MmsVariableSpecification;

/**@}*/

struct ATTRIBUTE_PACKED sMmsVariableSpecification {
    MmsType type;
    char* name;
    union uMmsTypeSpecification
    {
        struct sMmsArray {
            int elementCount; /* number of array elements */
            MmsVariableSpecification* elementTypeSpec;
        } array;
        struct sMmsStructure {
            int elementCount;
            MmsVariableSpecification** elements;
        } structure;
        int boolean; /* dummy - not required */
        int integer; /* size of integer in bits */
        int unsignedInteger; /* size of integer in bits */
        struct sMmsFloat
        {
            uint8_t exponentWidth;
            uint8_t formatWidth;
        } floatingpoint;
        int bitString; /* Number of bits in bitstring */
        int octetString; /* Number of octets in octet string */
        int visibleString; /* Maximum size of string */
        int mmsString;
        int utctime; /* dummy - not required */
        int binaryTime; /* size: either 4 or 6 */
    } typeSpec;
};


#endif /* MMS_TYPES_H_ */
