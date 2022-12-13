/*
 *  tase2_object_values.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017 MZ Automation GmbH
 *
 *  All rights reserved.
 */


#include "tase2_common_internal.h"


MmsValue*
tase2_createObjectNameValue(const char* domainName, const char* name)
{
    MmsValue* objectNameValue = MmsValue_createEmptyStructure(3);

    MmsValue_setElement(objectNameValue, 0, MmsValue_newUnsignedFromUint32(domainName == NULL ? 0 : 1));
    MmsValue_setElement(objectNameValue, 1, MmsValue_newVisibleString((domainName == NULL) ? "" : domainName));
    MmsValue_setElement(objectNameValue, 2, MmsValue_newVisibleString(name));

    return objectNameValue;
}

MmsValue*
tase2_createDSConditionsValue(bool intervalTimeout, bool integrityTimeout, bool objectChange, bool operatorRequest, bool otherEvent)
{
    MmsValue* dsConditionsValue = MmsValue_newBitString(5);

    MmsValue_setBitStringBit(dsConditionsValue, 0, intervalTimeout);
    MmsValue_setBitStringBit(dsConditionsValue, 1, integrityTimeout);
    MmsValue_setBitStringBit(dsConditionsValue, 2, objectChange);
    MmsValue_setBitStringBit(dsConditionsValue, 3, operatorRequest);
    MmsValue_setBitStringBit(dsConditionsValue, 4, otherEvent);

    return dsConditionsValue;
}
