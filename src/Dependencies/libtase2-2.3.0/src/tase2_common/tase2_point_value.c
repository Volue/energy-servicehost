/*
 *  tase2_point_value.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "tase2_common.h"
#include "tase2_common_internal.h"

#include "lib_memory.h"

#include <math.h>
#include <stdio.h>

Tase2_PointValueType
Tase2_PointValue_getType(Tase2_PointValue self)
{
    return self->type;
}

void
Tase2_PointValue_setType(Tase2_PointValue self, Tase2_PointValueType type)
{
    self->type = type;
}

Tase2_PointValue
Tase2_PointValue_getCopy(Tase2_PointValue self)
{
    Tase2_PointValue copy = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (copy) {
        copy->type = self->type;
        copy->pointValue = MmsValue_clone(self->pointValue);
    }

    return copy;
}

Tase2_PointValue
Tase2_PointValue_createReal(float value)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_REAL;
        self->pointValue = MmsValue_newFloat(value);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createDiscrete(int32_t value)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_DISCRETE;
        self->pointValue = MmsValue_newIntegerFromInt32(value);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createState(Tase2_DataState value)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_STATE;
        self->pointValue = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(self->pointValue, (uint32_t) value);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createStateSupplemental(Tase2_DataStateSupplemental value)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
        self->pointValue = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(self->pointValue, value);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createRealQ(float value, Tase2_DataFlags flags)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_REAL;
        self->pointValue = MmsValue_createEmptyStructure(2);

        MmsValue* mmsValue = MmsValue_newFloat(value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 1, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createDiscreteQ(int32_t value, Tase2_DataFlags flags)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_DISCRETE;
        self->pointValue = MmsValue_createEmptyStructure(2);

        MmsValue* mmsValue = MmsValue_newIntegerFromInt32(value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 1, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createStateQ(Tase2_DataState value, Tase2_DataFlags flags)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
        self->pointValue = MmsValue_createEmptyStructure(2);

        MmsValue* mmsValue = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsValue, value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 1, mmsFlags);
    }

    return self;
}


Tase2_PointValue
Tase2_PointValue_createStateSupplementalQ(Tase2_DataStateSupplemental value, Tase2_DataFlags flags)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
        self->pointValue = MmsValue_createEmptyStructure(2);

        MmsValue* mmsValue = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsValue, value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 1, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createRealQTimeTag(float value, Tase2_DataFlags flags, int32_t timeTag)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_REAL;
        self->pointValue = MmsValue_createEmptyStructure(3);

        MmsValue* mmsValue = MmsValue_newFloat(value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsTimeTag = MmsValue_newIntegerFromInt32(timeTag);
        MmsValue_setElement(self->pointValue, 1, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 2, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createStateQTimeTag(Tase2_DataFlags valueAndFlags, int32_t timeTag)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_STATE;
        self->pointValue = MmsValue_createEmptyStructure(2);

        MmsValue* mmsTimeTag = MmsValue_newIntegerFromInt32(timeTag);
        MmsValue_setElement(self->pointValue, 0, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, valueAndFlags);
        MmsValue_setElement(self->pointValue, 1, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createDiscreteQTimeTag(int32_t value, Tase2_DataFlags flags, int32_t timeTag)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_DISCRETE;
        self->pointValue = MmsValue_createEmptyStructure(3);

        MmsValue* mmsValue = MmsValue_newIntegerFromInt32(value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsTimeTag = MmsValue_newIntegerFromInt32(timeTag);
        MmsValue_setElement(self->pointValue, 1, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 2, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createStateSupplementalQTimeTag(Tase2_DataStateSupplemental value, Tase2_DataFlags flags, int32_t timeTag)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
        self->pointValue = MmsValue_createEmptyStructure(3);

        MmsValue* mmsValue = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsValue, value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsTimeTag = MmsValue_newIntegerFromInt32(timeTag);
        MmsValue_setElement(self->pointValue, 1, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 2, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createRealExtended(float value, Tase2_DataFlags flags, int32_t timeTag, uint16_t cov)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_REAL;
        self->pointValue = MmsValue_createEmptyStructure(4);

        MmsValue* mmsValue = MmsValue_newFloat(value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsTimeTag = MmsValue_newIntegerFromInt32(timeTag);
        MmsValue_setElement(self->pointValue, 1, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 2, mmsFlags);

        MmsValue* mmsCov = MmsValue_newUnsigned(16);
        MmsValue_setUint16(mmsCov, cov);
        MmsValue_setElement(self->pointValue, 3, mmsCov);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createStateExtended(Tase2_DataFlags valueAndFlags, int32_t timeTag, uint16_t cov)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_STATE;
        self->pointValue = MmsValue_createEmptyStructure(3);

        MmsValue* mmsTimeTag = MmsValue_newIntegerFromInt32(timeTag);
        MmsValue_setElement(self->pointValue, 0, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, valueAndFlags);
        MmsValue_setElement(self->pointValue, 1, mmsFlags);

        MmsValue* mmsCov = MmsValue_newUnsigned(16);
        MmsValue_setUint16(mmsCov, cov);
        MmsValue_setElement(self->pointValue, 2, mmsCov);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createDiscreteExtended(int32_t value, Tase2_DataFlags flags, int32_t timeTag, uint16_t cov)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_DISCRETE;
        self->pointValue = MmsValue_createEmptyStructure(4);

        MmsValue* mmsValue = MmsValue_newIntegerFromInt32(value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsTimeTag = MmsValue_newIntegerFromInt32(timeTag);
        MmsValue_setElement(self->pointValue, 1, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 2, mmsFlags);

        MmsValue* mmsCov = MmsValue_newUnsigned(16);
        MmsValue_setUint16(mmsCov, cov);
        MmsValue_setElement(self->pointValue, 3, mmsCov);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createStateSupplementalExtended(Tase2_DataStateSupplemental value, Tase2_DataFlags flags, int32_t timeTag, uint16_t cov)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
        self->pointValue = MmsValue_createEmptyStructure(4);

        MmsValue* mmsValue = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsValue, value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsTimeTag = MmsValue_newIntegerFromInt32(timeTag);
        MmsValue_setElement(self->pointValue, 1, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 2, mmsFlags);

        MmsValue* mmsCov = MmsValue_newUnsigned(16);
        MmsValue_setUint16(mmsCov, cov);
        MmsValue_setElement(self->pointValue, 3, mmsCov);
    }

    return self;
}


static MmsValue*
createTimeTagExtended(uint64_t timeTagExtended)
{
    MmsValue* mmsTimeTagExtended = MmsValue_createEmptyStructure(2);

    MmsValue* element1 = MmsValue_newIntegerFromInt32(timeTagExtended / 1000);
    MmsValue_setElement(mmsTimeTagExtended, 0, element1);

    MmsValue* element2 = MmsValue_newIntegerFromInt16(timeTagExtended % 1000);
    MmsValue_setElement(mmsTimeTagExtended, 1, element2);

    return mmsTimeTagExtended;
}

Tase2_PointValue
Tase2_PointValue_createRealQTimeTagExtended(float value, Tase2_DataFlags flags, uint64_t timeTagExtended)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_REAL;
        self->pointValue = MmsValue_createEmptyStructure(3);

        MmsValue* mmsValue = MmsValue_newFloat(value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsTimeTag = createTimeTagExtended(timeTagExtended);
        MmsValue_setElement(self->pointValue, 1, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 2, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createStateQTimeTagExtended(Tase2_DataFlags valueAndFlags, uint64_t timeTagExtended)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_STATE;
        self->pointValue = MmsValue_createEmptyStructure(2);

        MmsValue* mmsTimeTag = createTimeTagExtended(timeTagExtended);
        MmsValue_setElement(self->pointValue, 0, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, valueAndFlags);
        MmsValue_setElement(self->pointValue, 1, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createDiscreteQTimeTagExtended(int32_t value, Tase2_DataFlags flags, uint64_t timeTagExtended)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

    if (self) {
        self->type = TASE2_VALUE_TYPE_DISCRETE;
        self->pointValue = MmsValue_createEmptyStructure(3);

        MmsValue* mmsValue = MmsValue_newIntegerFromInt32(value);
        MmsValue_setElement(self->pointValue, 0, mmsValue);

        MmsValue* mmsTimeTag = createTimeTagExtended(timeTagExtended);
        MmsValue_setElement(self->pointValue, 1, mmsTimeTag);

        MmsValue* mmsFlags = MmsValue_newBitString(8);
        MmsValue_setBitStringFromInteger(mmsFlags, flags);
        MmsValue_setElement(self->pointValue, 2, mmsFlags);
    }

    return self;
}

Tase2_PointValue
Tase2_PointValue_createStateSupplementalQTimeTagExtended(Tase2_DataStateSupplemental value, Tase2_DataFlags flags, uint64_t timeTagExtended)
{
    Tase2_PointValue self = (Tase2_PointValue) GLOBAL_MALLOC(sizeof(struct sTase2_PointValue));

   if (self) {
       self->type = TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL;
       self->pointValue = MmsValue_createEmptyStructure(3);

       MmsValue* mmsValue = MmsValue_newBitString(8);
       MmsValue_setBitStringFromInteger(mmsValue, value);
       MmsValue_setElement(self->pointValue, 0, mmsValue);

       MmsValue* mmsTimeTag = createTimeTagExtended(timeTagExtended);
       MmsValue_setElement(self->pointValue, 1, mmsTimeTag);

       MmsValue* mmsFlags = MmsValue_newBitString(8);
       MmsValue_setBitStringFromInteger(mmsFlags, flags);
       MmsValue_setElement(self->pointValue, 2, mmsFlags);
   }

   return self;
}

void
Tase2_PointValue_destroy(Tase2_PointValue self)
{
    if (self->pointValue)
        MmsValue_delete(self->pointValue);

    free(self);
}

float
Tase2_PointValue_getValueReal(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_FLOAT) {
        return MmsValue_toFloat(self->pointValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* valueElement = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(valueElement) == MMS_FLOAT) {
            return MmsValue_toFloat(valueElement);
        }
    }

    return NAN;
}

float
Tase2_PointValue_getValueRealEx(Tase2_PointValue self, Tase2_PointValueError* err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_FLOAT) {
        return MmsValue_toFloat(self->pointValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* valueElement = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(valueElement) == MMS_FLOAT) {
            return MmsValue_toFloat(valueElement);
        }
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return NAN;
}

void
Tase2_PointValue_setValueReal(Tase2_PointValue self, float value)
{
    if (MmsValue_getType(self->pointValue) == MMS_FLOAT) {
        MmsValue_setFloat(self->pointValue, value);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* valueElement = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(valueElement) == MMS_FLOAT) {
            MmsValue_setFloat(valueElement, value);
        }
    }
}

Tase2_PointValueError
Tase2_PointValue_setValueRealEx(Tase2_PointValue self, float value)
{
    if (MmsValue_getType(self->pointValue) == MMS_FLOAT) {
        MmsValue_setFloat(self->pointValue, value);
        return TASE2_POINT_VALUE_ERROR_OK;
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* valueElement = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(valueElement) == MMS_FLOAT) {
            MmsValue_setFloat(valueElement, value);
            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }

   return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

int32_t
Tase2_PointValue_getValueDiscrete(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_INTEGER) {
        return MmsValue_toInt32(self->pointValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* valueElement = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(valueElement) == MMS_INTEGER) {
            return MmsValue_toInt32(valueElement);
        }
    }

    return 0;
}

int32_t
Tase2_PointValue_getValueDiscreteEx(Tase2_PointValue self, Tase2_PointValueError* err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_INTEGER) {
        return MmsValue_toInt32(self->pointValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* valueElement = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(valueElement) == MMS_INTEGER) {
            return MmsValue_toInt32(valueElement);
        }
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0;
}

void
Tase2_PointValue_setValueDiscrete(Tase2_PointValue self, int32_t value)
{
    if (MmsValue_getType(self->pointValue) == MMS_INTEGER) {
        MmsValue_setInt32(self->pointValue, value);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* valueElement = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(valueElement) == MMS_INTEGER) {
            MmsValue_setInt32(valueElement, value);
        }
    }
}

Tase2_PointValueError
Tase2_PointValue_setValueDiscreteEx(Tase2_PointValue self, int32_t value)
{
    if (MmsValue_getType(self->pointValue) == MMS_INTEGER) {
        MmsValue_setInt32(self->pointValue, value);
        return TASE2_POINT_VALUE_ERROR_OK;
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* valueElement = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(valueElement) == MMS_INTEGER) {
            MmsValue_setInt32(valueElement, value);
            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

Tase2_DataState
Tase2_PointValue_getValueState(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        return (Tase2_DataState) MmsValue_getBitStringAsInteger(self->pointValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* stateValue = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING)
            return (Tase2_DataState) MmsValue_getBitStringAsInteger(stateValue);

        stateValue = MmsValue_getElement(self->pointValue, 1);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING)
            return (Tase2_DataState) MmsValue_getBitStringAsInteger(stateValue);
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getValueState: wrong type (%d)", MmsValue_getType(self->pointValue));

    return 0xff;
}

Tase2_DataState
Tase2_PointValue_getValueStateEx(Tase2_PointValue self, Tase2_PointValueError* err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        return (Tase2_DataState) MmsValue_getBitStringAsInteger(self->pointValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* stateValue = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING)
            return (Tase2_DataState) MmsValue_getBitStringAsInteger(stateValue);

        stateValue = MmsValue_getElement(self->pointValue, 1);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING)
            return (Tase2_DataState) MmsValue_getBitStringAsInteger(stateValue);
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0xff;
}

void
Tase2_PointValue_setValueState(Tase2_PointValue self, Tase2_DataState value)
{
    if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        MmsValue_setBitStringFromInteger(self->pointValue, value);
        return;
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* stateValue = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING) {
            MmsValue_setBitStringFromInteger(stateValue, value);
            return;
        }

        stateValue = MmsValue_getElement(self->pointValue, 1);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING) {
            MmsValue_setBitStringFromInteger(stateValue, value);
            return;
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setValueState: wrong type");
}

Tase2_PointValueError
Tase2_PointValue_setValueStateEx(Tase2_PointValue self, Tase2_DataState value)
{
    if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        MmsValue_setBitStringFromInteger(self->pointValue, value);
        return TASE2_POINT_VALUE_ERROR_OK;
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* stateValue = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING) {
            MmsValue_setBitStringFromInteger(stateValue, value);
            return TASE2_POINT_VALUE_ERROR_OK;
        }

        stateValue = MmsValue_getElement(self->pointValue, 1);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING) {
            MmsValue_setBitStringFromInteger(stateValue, value);
            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

Tase2_DataStateSupplemental
Tase2_PointValue_getValueStateSupplemental(Tase2_PointValue self)
{

    if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        return (Tase2_DataStateSupplemental) MmsValue_getBitStringAsInteger(self->pointValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* stateValue = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING)
            return (Tase2_DataStateSupplemental) MmsValue_getBitStringAsInteger(stateValue);
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getValueStateSupplemental: wrong type");

    return 0xff;
}

Tase2_DataStateSupplemental
Tase2_PointValue_getValueStateSupplementalEx(Tase2_PointValue self, Tase2_PointValueError* err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        return (Tase2_DataStateSupplemental) MmsValue_getBitStringAsInteger(self->pointValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* stateValue = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING)
            return (Tase2_DataStateSupplemental) MmsValue_getBitStringAsInteger(stateValue);
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0xff;
}

void
Tase2_PointValue_setValueStateSupplemental(Tase2_PointValue self, Tase2_DataStateSupplemental value)
{
    if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        MmsValue_setBitStringFromInteger(self->pointValue, value);
        return;
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* stateValue = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING) {
            MmsValue_setBitStringFromInteger(stateValue, value);
            return;
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setValueStateSupplemental: wrong type");
}

Tase2_PointValueError
Tase2_PointValue_setValueStateSupplementalEx(Tase2_PointValue self, Tase2_DataStateSupplemental value)
{
    if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        MmsValue_setBitStringFromInteger(self->pointValue, value);
        return TASE2_POINT_VALUE_ERROR_OK;
    }
    else if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {

        MmsValue* stateValue = MmsValue_getElement(self->pointValue, 0);

        if (MmsValue_getType(stateValue) == MMS_BIT_STRING) {
            MmsValue_setBitStringFromInteger(stateValue, value);
            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

static bool
containsStructOrInt(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        int i;
        for (i = 0; i < MmsValue_getArraySize(self->pointValue); i++) {
            MmsValue* element = MmsValue_getElement(self->pointValue, i);

            if (MmsValue_getType(element) == MMS_INTEGER)
                return true;
            else if (MmsValue_getType(element) == MMS_STRUCTURE)
                return true;
        }

        return false;
    }
    else
        return false;
}

static uint64_t
convertMmsTimestampToMsTime(MmsValue* tsValue)
{
    if (tsValue) {
        /* Check if extended time stamp */

        if (MmsValue_getType(tsValue) == MMS_STRUCTURE) {
            uint64_t timestamp = (uint64_t) MmsValue_toInt32(MmsValue_getElement(tsValue, 0)) * (uint64_t) 1000;
            timestamp += (uint64_t) MmsValue_toInt32(MmsValue_getElement(tsValue, 1));

            return timestamp;
        }
        else {
            return ((uint64_t) MmsValue_toInt32(tsValue) * (uint64_t) 1000);
        }
    }
    else {
        return 0;
    }
}

static MmsValue*
getMmsTimestamp(Tase2_PointValue self)
{
    MmsValue* tsValue = NULL;

    if ((MmsValue_getType(self->pointValue) == MMS_STRUCTURE) && (MmsValue_getArraySize(self->pointValue) > 1)) {


        if ((self->type == TASE2_VALUE_TYPE_STATE) || (self->type == TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL) ||
                (self->type == TASE2_VALUE_TYPE_STATE_OR_STATE_SUPPLEMENTAL) ||
                (self->type == TASE2_VALUE_TYPE_STATE_OR_DISCRETE))
        {
            MmsValue* firstElement = MmsValue_getElement(self->pointValue, 0);

            if ((MmsValue_getType(firstElement) == MMS_INTEGER) || (MmsValue_getType(firstElement) == MMS_STRUCTURE)) {
                tsValue = firstElement;
            }
            else {
                MmsValue* secondElement = MmsValue_getElement(self->pointValue, 1);

                if ((MmsValue_getType(secondElement) == MMS_INTEGER) || (MmsValue_getType(secondElement) == MMS_STRUCTURE)) {
                    tsValue = secondElement;
                }
            }
        }
        else if ((self->type == TASE2_VALUE_TYPE_DISCRETE) || (self->type == TASE2_VALUE_TYPE_REAL)) {
            MmsValue* secondElement = MmsValue_getElement(self->pointValue, 1);

            if ((MmsValue_getType(secondElement) == MMS_INTEGER) || (MmsValue_getType(secondElement) == MMS_STRUCTURE)) {
                tsValue = secondElement;
            }
        }
        else if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT) {
            tsValue = MmsValue_getElement(self->pointValue, 1);
        }
        else if (self->type == TASE2_VALUE_TYPE_PACKED_EVENT) {
            tsValue = MmsValue_getElement(self->pointValue, 2);
        }
    }

    return tsValue;
}

Tase2_TimeStampClass
Tase2_PointValue_getTimeStampClass(Tase2_PointValue self)
{
    MmsValue* tsValue = getMmsTimestamp(self);

    if (tsValue) {
        if (MmsValue_getType(tsValue) == MMS_STRUCTURE)
            return TASE2_TIMESTAMP_EXTENDED;
        else
            return TASE2_TIMESTAMP;
    }
    else {
        return TASE2_NO_TIMESTAMP;
    }
}

uint64_t
Tase2_PointValue_getTimeStamp(Tase2_PointValue self)
{
    MmsValue* tsValue = getMmsTimestamp(self);

    if (tsValue == NULL)
    {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getTimeStamp: wrong type (no timestamp)");
    }

    return convertMmsTimestampToMsTime(tsValue);
}

uint64_t
Tase2_PointValue_getTimeStampEx(Tase2_PointValue self, Tase2_PointValueError *err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    MmsValue* tsValue = getMmsTimestamp(self);

    if (tsValue == NULL)
    {
        if (err)
            *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
    }

    return convertMmsTimestampToMsTime(tsValue);
}

bool
Tase2_PointValue_hasTimeStamp(Tase2_PointValue self)
{
    if (self->type == TASE2_VALUE_TYPE_STATE) {
        if (containsStructOrInt(self))
            return true;
        else
            return false;
    }
    else if (self->type == TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL) {
        if (containsStructOrInt(self))
            return true;
        else
            return false;
    }
    else if (self->type == TASE2_VALUE_TYPE_STATE_OR_STATE_SUPPLEMENTAL) {
        if (containsStructOrInt(self))
            return true;
        else
            return false;
    }
    else if (self->type == TASE2_VALUE_TYPE_REAL) {
        if (containsStructOrInt(self))
            return true;
        else
            return false;
    }
    else if (self->type == TASE2_VALUE_TYPE_DISCRETE) {
        if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
            if (MmsValue_getArraySize(self->pointValue) > 2)
                return true;
            else
                return false;
        }
        else
            return false;
    }
    else if (self->type == TASE2_VALUE_TYPE_STATE_OR_DISCRETE) {
        /* Not possible to determine if the value contains a timestamp! */
        return false;
    }
    else if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT) {
        return true;
    }
    else if (self->type == TASE2_VALUE_TYPE_PACKED_EVENT) {
        return true;
    }
    else {
        return false;
    }
}

void
Tase2_PointValue_setTimeStamp(Tase2_PointValue self, uint64_t timestamp)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* tsValue;

        if (self->type == TASE2_VALUE_TYPE_STATE)
            tsValue = MmsValue_getElement(self->pointValue, 0);
        else if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            tsValue = MmsValue_getElement(self->pointValue, 2);
        else if (self->type == TASE2_VALUE_TYPE_PACKED_EVENT)
            tsValue = MmsValue_getElement(self->pointValue, 3);
        else
            tsValue = MmsValue_getElement(self->pointValue, 1);

        /* Check if extended time stamp */
        if (MmsValue_getType(tsValue) == MMS_STRUCTURE) {
            MmsValue_setInt32(MmsValue_getElement(tsValue, 0), timestamp / 1000);
            MmsValue_setInt16(MmsValue_getElement(tsValue, 1), timestamp % 1000);
        }
        else {
            MmsValue_setInt32(tsValue, timestamp / 1000);
        }
    }
    else {
        tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setTimeStamp: wrong type");
    }
}

Tase2_PointValueError
Tase2_PointValue_setTimeStampEx(Tase2_PointValue self, uint64_t timestamp)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* tsValue;

        if (self->type == TASE2_VALUE_TYPE_STATE)
            tsValue = MmsValue_getElement(self->pointValue, 0);
        else if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            tsValue = MmsValue_getElement(self->pointValue, 2);
        else if (self->type == TASE2_VALUE_TYPE_PACKED_EVENT)
            tsValue = MmsValue_getElement(self->pointValue, 3);
        else
            tsValue = MmsValue_getElement(self->pointValue, 1);

        /* Check if extended time stamp */
        if (MmsValue_getType(tsValue) == MMS_STRUCTURE) {
            MmsValue_setInt32(MmsValue_getElement(tsValue, 0), timestamp / 1000);
            MmsValue_setInt16(MmsValue_getElement(tsValue, 1), timestamp % 1000);
        }
        else {
            MmsValue_setInt32(tsValue, timestamp / 1000);
        }

        return TASE2_POINT_VALUE_ERROR_OK;
    }
    else {
        return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
    }
}

Tase2_DataFlags
Tase2_PointValue_getFlags(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* flagsValue = MmsValue_getElement(self->pointValue, 1);

        if (self->type != TASE2_VALUE_TYPE_STATE) {
             if (MmsValue_getType(flagsValue) != MMS_BIT_STRING)
                flagsValue = MmsValue_getElement(self->pointValue, 2);
        }

        if (MmsValue_getType(flagsValue) == MMS_BIT_STRING)
            return MmsValue_getBitStringAsInteger(flagsValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        if (self->type != TASE2_VALUE_TYPE_STATE) {
            return MmsValue_getBitStringAsInteger(self->pointValue);
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getFlags: wrong type");

    return 0xff;
}

Tase2_DataFlags
Tase2_PointValue_getFlagsEx(Tase2_PointValue self, Tase2_PointValueError *err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* flagsValue = MmsValue_getElement(self->pointValue, 1);

        if (self->type != TASE2_VALUE_TYPE_STATE) {
             if (MmsValue_getType(flagsValue) != MMS_BIT_STRING)
                flagsValue = MmsValue_getElement(self->pointValue, 2);
        }

        if (MmsValue_getType(flagsValue) == MMS_BIT_STRING)
            return MmsValue_getBitStringAsInteger(flagsValue);
    }
    else if (MmsValue_getType(self->pointValue) == MMS_BIT_STRING) {
        if (self->type != TASE2_VALUE_TYPE_STATE) {
            return MmsValue_getBitStringAsInteger(self->pointValue);
        }
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0xff;
}

static void
setFlags(MmsValue* flagsValue, Tase2_DataFlags flags)
{
    Tase2_DataFlags value = MmsValue_getBitStringAsInteger(flagsValue);

    flags = flags & 0xfc;
    value = value & 0x03;
    value = value | flags;

    MmsValue_setBitStringFromInteger(flagsValue, (uint32_t) value);
}

void
Tase2_PointValue_setFlags(Tase2_PointValue self, Tase2_DataFlags flags)
{
    if (self->type == TASE2_VALUE_TYPE_STATE) {

        MmsValue* flagsValue = NULL;

        if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
            MmsValue* flagsValue = MmsValue_getElement(self->pointValue, 1);

            if (MmsValue_getType(flagsValue) != MMS_BIT_STRING) {
                flagsValue = MmsValue_getElement(self->pointValue, 2);
            }
        }
        else {
            flagsValue = self->pointValue;
        }

        if (flagsValue && (MmsValue_getType(flagsValue) == MMS_BIT_STRING)) {
            setFlags(flagsValue, flags);
            return;
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setFlags: wrong type");
        }
    }
    else {
        if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
            MmsValue* flagsValue = MmsValue_getElement(self->pointValue, 1);

            if (flagsValue && MmsValue_getType(flagsValue) != MMS_BIT_STRING) {
                flagsValue = MmsValue_getElement(self->pointValue, 2);
            }

            if (flagsValue && MmsValue_getType(flagsValue) == MMS_BIT_STRING) {
                setFlags(flagsValue, flags);
                return;
            }
        }
        else {
            tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setFlags: wrong type");
        }
    }
}

Tase2_PointValueError
Tase2_PointValue_setFlagsEx(Tase2_PointValue self, Tase2_DataFlags flags)
{
    if (self->type == TASE2_VALUE_TYPE_STATE) {

        MmsValue* flagsValue = NULL;

        if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
            MmsValue* flagsValue = MmsValue_getElement(self->pointValue, 1);

            if (MmsValue_getType(flagsValue) != MMS_BIT_STRING) {
                flagsValue = MmsValue_getElement(self->pointValue, 2);
            }
        }
        else {
            flagsValue = self->pointValue;
        }

        if (flagsValue && (MmsValue_getType(flagsValue) == MMS_BIT_STRING)) {
            setFlags(flagsValue, flags);
            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }
    else {
        if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
            MmsValue* flagsValue = MmsValue_getElement(self->pointValue, 1);

            if (flagsValue && MmsValue_getType(flagsValue) != MMS_BIT_STRING) {
                flagsValue = MmsValue_getElement(self->pointValue, 2);
            }

            if (flagsValue && MmsValue_getType(flagsValue) == MMS_BIT_STRING) {
                setFlags(flagsValue, flags);
                return TASE2_POINT_VALUE_ERROR_OK;
            }
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

uint16_t
Tase2_PointValue_getCOV(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* covValue;

        if (self->type == TASE2_VALUE_TYPE_STATE)
            covValue = MmsValue_getElement(self->pointValue, 2);
        else
            covValue = MmsValue_getElement(self->pointValue, 3);

        if ((covValue != NULL) && (MmsValue_getType(covValue) == MMS_UNSIGNED)) {
            return (uint16_t) MmsValue_toUint32(covValue);
        }
    }

	tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getCOV: wrong type");

    return 0xffff;
}

uint16_t
Tase2_PointValue_getCOVEx(Tase2_PointValue self, Tase2_PointValueError *err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* covValue;

        if (self->type == TASE2_VALUE_TYPE_STATE)
            covValue = MmsValue_getElement(self->pointValue, 2);
        else
            covValue = MmsValue_getElement(self->pointValue, 3);

        if ((covValue != NULL) && (MmsValue_getType(covValue) == MMS_UNSIGNED)) {
            return (uint16_t) MmsValue_toUint32(covValue);
        }
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0xffff;
}

void
Tase2_PointValue_setCOV(Tase2_PointValue self, uint16_t cov)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* covValue;

        if (self->type == TASE2_VALUE_TYPE_STATE)
            covValue = MmsValue_getElement(self->pointValue, 2);
        else
            covValue = MmsValue_getElement(self->pointValue, 3);

        if ((covValue != NULL) && (MmsValue_getType(covValue) == MMS_UNSIGNED)) {
            MmsValue_setUint16(covValue, cov);
            return;
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setCOV: wrong type");
}

Tase2_PointValueError
Tase2_PointValue_setCOVEx(Tase2_PointValue self, uint16_t cov)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* covValue;

        if (self->type == TASE2_VALUE_TYPE_STATE)
            covValue = MmsValue_getElement(self->pointValue, 2);
        else
            covValue = MmsValue_getElement(self->pointValue, 3);

        if ((covValue != NULL) && (MmsValue_getType(covValue) == MMS_UNSIGNED)) {
            MmsValue_setUint16(covValue, cov);
            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

Tase2_SingleEventFlags
Tase2_PointValue_getSingleEventFlags(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* seValue = MmsValue_getElement(self->pointValue, 0);

        if ((seValue != NULL) && (MmsValue_getType(seValue) == MMS_BIT_STRING))
            return (Tase2_SingleEventFlags) MmsValue_getBitStringAsInteger(seValue);
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getSingleEventFlags: wrong type");

    return 0xff;
}

Tase2_SingleEventFlags
Tase2_PointValue_getSingleEventFlagsEx(Tase2_PointValue self, Tase2_PointValueError *err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* seValue = MmsValue_getElement(self->pointValue, 0);

        if ((seValue != NULL) && (MmsValue_getType(seValue) == MMS_BIT_STRING))
            return (Tase2_SingleEventFlags) MmsValue_getBitStringAsInteger(seValue);
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0xff;
}

void
Tase2_PointValue_setSingleEventFlags(Tase2_PointValue self, Tase2_SingleEventFlags eventFlags)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* seValue = MmsValue_getElement(self->pointValue, 0);

        if ((seValue != NULL) && (MmsValue_getType(seValue) == MMS_BIT_STRING)) {
            MmsValue_setBitStringFromInteger(seValue, eventFlags);
            return;
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setSingleEventFlags: wrong type");
}

Tase2_PointValueError
Tase2_PointValue_setSingleEventFlagsEx(Tase2_PointValue self, Tase2_SingleEventFlags eventFlags)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* seValue = MmsValue_getElement(self->pointValue, 0);

        if ((seValue != NULL) && (MmsValue_getType(seValue) == MMS_BIT_STRING)) {
            MmsValue_setBitStringFromInteger(seValue, eventFlags);
            return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

Tase2_EventFlags
Tase2_PointValue_getPackedEvent(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* peValue = MmsValue_getElement(self->pointValue, 0);

        if ((peValue != NULL) && (MmsValue_getType(peValue) == MMS_BIT_STRING))
            return (Tase2_SingleEventFlags) MmsValue_getBitStringAsInteger(peValue);
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getPackedEvent: wrong type");

    return 0xff;
}

Tase2_EventFlags
Tase2_PointValue_getPackedEventEx(Tase2_PointValue self, Tase2_PointValueError *err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* peValue = MmsValue_getElement(self->pointValue, 0);

        if ((peValue != NULL) && (MmsValue_getType(peValue) == MMS_BIT_STRING))
            return (Tase2_SingleEventFlags) MmsValue_getBitStringAsInteger(peValue);
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0xff;
}

void
Tase2_PointValue_setPackedEvent(Tase2_PointValue self, Tase2_EventFlags packedEvent)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* peValue = MmsValue_getElement(self->pointValue, 0);

        if ((peValue != NULL) && (MmsValue_getType(peValue) == MMS_BIT_STRING)) {
            MmsValue_setBitStringFromInteger(peValue, packedEvent);
            return;
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setPackedEvent: wrong type");
}

Tase2_PointValueError
Tase2_PointValue_setPackedEventEx(Tase2_PointValue self, Tase2_EventFlags packedEvent)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* peValue = MmsValue_getElement(self->pointValue, 0);

        if ((peValue != NULL) && (MmsValue_getType(peValue) == MMS_BIT_STRING)) {
            MmsValue_setBitStringFromInteger(peValue, packedEvent);
            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

Tase2_PackedEventFlags
Tase2_PointValue_getPackedEventFlags(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* peFLags = MmsValue_getElement(self->pointValue, 1);

        if ((peFLags != NULL) && (MmsValue_getType(peFLags) == MMS_BIT_STRING)) {

            return (Tase2_PackedEventFlags) MmsValue_getBitStringAsInteger(peFLags);
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getPackedEventFlags: wrong type");

    return 0xff;
}

Tase2_PackedEventFlags
Tase2_PointValue_getPackedEventFlagsEx(Tase2_PointValue self, Tase2_PointValueError *err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* peFLags = MmsValue_getElement(self->pointValue, 1);

        if ((peFLags != NULL) && (MmsValue_getType(peFLags) == MMS_BIT_STRING)) {

            return (Tase2_PackedEventFlags) MmsValue_getBitStringAsInteger(peFLags);
        }
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0xff;
}

void
Tase2_PointValue_setPackedEventFlags(Tase2_PointValue self, Tase2_PackedEventFlags flags)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* peFLags = MmsValue_getElement(self->pointValue, 1);

        if ((peFLags != NULL) && (MmsValue_getType(peFLags) == MMS_BIT_STRING)) {
            MmsValue_setBitStringFromInteger(peFLags, flags);
            return;
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setPackedEventFlags: wrong type");
}

Tase2_PointValueError
Tase2_PointValue_setPackedEventFlagsEx(Tase2_PointValue self, Tase2_PackedEventFlags flags)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* peFLags = MmsValue_getElement(self->pointValue, 1);

        if ((peFLags != NULL) && (MmsValue_getType(peFLags) == MMS_BIT_STRING)) {
            MmsValue_setBitStringFromInteger(peFLags, flags);
            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

int32_t
Tase2_PointValue_getOperatingTime(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* opTimeValue;

        if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            opTimeValue = MmsValue_getElement(self->pointValue, 1);
        else
            opTimeValue = MmsValue_getElement(self->pointValue, 2);

        if ((opTimeValue != NULL) && (MmsValue_getType(opTimeValue) == MMS_INTEGER)) {
            return MmsValue_toInt32(opTimeValue);
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getOperatingTime: wrong type");

    return 0;
}

int32_t
Tase2_PointValue_getOperatingTimeEx(Tase2_PointValue self, Tase2_PointValueError *err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* opTimeValue;

        if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            opTimeValue = MmsValue_getElement(self->pointValue, 1);
        else
            opTimeValue = MmsValue_getElement(self->pointValue, 2);

        if ((opTimeValue != NULL) && (MmsValue_getType(opTimeValue) == MMS_INTEGER)) {
            return MmsValue_toInt32(opTimeValue);
        }
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0;
}

void
Tase2_PointValue_setOperatingTime(Tase2_PointValue self, int32_t operatingTime)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* opTimeValue;

        if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            opTimeValue = MmsValue_getElement(self->pointValue, 1);
        else
            opTimeValue = MmsValue_getElement(self->pointValue, 2);

        if ((opTimeValue != NULL) && (MmsValue_getType(opTimeValue) == MMS_INTEGER)) {
            MmsValue_setInt32(opTimeValue, operatingTime);
            return;
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_setOperatingTime: wrong type");
}

Tase2_PointValueError
Tase2_PointValue_setOperatingTimeEx(Tase2_PointValue self, int32_t operatingTime)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* opTimeValue;

        if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            opTimeValue = MmsValue_getElement(self->pointValue, 1);
        else
            opTimeValue = MmsValue_getElement(self->pointValue, 2);

        if ((opTimeValue != NULL) && (MmsValue_getType(opTimeValue) == MMS_INTEGER)) {
            MmsValue_setInt32(opTimeValue, operatingTime);
            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

uint64_t
Tase2_PointValue_getEventTime(Tase2_PointValue self)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* eventTimeValue;

        if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            eventTimeValue = MmsValue_getElement(self->pointValue, 2);
        else
            eventTimeValue = MmsValue_getElement(self->pointValue, 3);

        if ((eventTimeValue != NULL) && (MmsValue_getType(eventTimeValue) == MMS_STRUCTURE)) {
            uint64_t timestamp = (uint64_t) MmsValue_toInt32(MmsValue_getElement(eventTimeValue, 0)) * (uint64_t) 1000;
            timestamp += (uint64_t) MmsValue_toInt32(MmsValue_getElement(eventTimeValue, 1));

            return timestamp;
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getOperatingTime: wrong type");

    return 0;
}

uint64_t
Tase2_PointValue_getEventTimeEx(Tase2_PointValue self, Tase2_PointValueError *err)
{
    if (err)
        *err = TASE2_POINT_VALUE_ERROR_OK;

    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* eventTimeValue;

        if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            eventTimeValue = MmsValue_getElement(self->pointValue, 2);
        else
            eventTimeValue = MmsValue_getElement(self->pointValue, 3);

        if ((eventTimeValue != NULL) && (MmsValue_getType(eventTimeValue) == MMS_STRUCTURE)) {
            uint64_t timestamp = (uint64_t) MmsValue_toInt32(MmsValue_getElement(eventTimeValue, 0)) * (uint64_t) 1000;
            timestamp += (uint64_t) MmsValue_toInt32(MmsValue_getElement(eventTimeValue, 1));

            return timestamp;
        }
    }

    if (err)
        *err = TASE2_POINT_VALUE_ERROR_WRONG_TYPE;

    return 0;
}


void
Tase2_PointValue_setEventTime(Tase2_PointValue self, uint64_t eventTime)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* eventTimeValue;

        if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            eventTimeValue = MmsValue_getElement(self->pointValue, 2);
        else
            eventTimeValue = MmsValue_getElement(self->pointValue, 3);

        if ((eventTimeValue != NULL) && (MmsValue_getType(eventTimeValue) == MMS_STRUCTURE)) {

            MmsValue_setInt32(MmsValue_getElement(eventTimeValue, 0), eventTime / 1000);
            MmsValue_setInt16(MmsValue_getElement(eventTimeValue, 1), eventTime % 1000);

            return;
        }
    }

    tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, NULL, NULL, "Tase2_PointValue_getOperatingTime: wrong type");
}

Tase2_PointValueError
Tase2_PointValue_setEventTimeEx(Tase2_PointValue self, uint64_t eventTime)
{
    if (MmsValue_getType(self->pointValue) == MMS_STRUCTURE) {
        MmsValue* eventTimeValue;

        if (self->type == TASE2_VALUE_TYPE_SINGLE_EVENT)
            eventTimeValue = MmsValue_getElement(self->pointValue, 2);
        else
            eventTimeValue = MmsValue_getElement(self->pointValue, 3);

        if ((eventTimeValue != NULL) && (MmsValue_getType(eventTimeValue) == MMS_STRUCTURE)) {

            MmsValue_setInt32(MmsValue_getElement(eventTimeValue, 0), eventTime / 1000);
            MmsValue_setInt16(MmsValue_getElement(eventTimeValue, 1), eventTime % 1000);

            return TASE2_POINT_VALUE_ERROR_OK;
        }
    }

    return TASE2_POINT_VALUE_ERROR_WRONG_TYPE;
}

static Tase2_ClientError
convertDataAccessErrorToClientError(MmsDataAccessError dataAccessError)
{

    switch (dataAccessError) {

    case DATA_ACCESS_ERROR_SUCCESS:
        return TASE2_CLIENT_ERROR_OK;

    case DATA_ACCESS_ERROR_TEMPORARILY_UNAVAILABLE:
        return TASE2_CLIENT_ERROR_TEMPORARILY_UNAVAILABLE;

    case DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED:
        return TASE2_CLIENT_ERROR_ACCESS_DENIED;

    case DATA_ACCESS_ERROR_INVALID_ADDRESS:
    case DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT:
    case DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID:
        return TASE2_CLIENT_ERROR_INVALID_ARGUMENT;

    case DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT:
        return TASE2_CLIENT_ERROR_OBJECT_NOT_EXISTING;

    default:
        return TASE2_CLIENT_ERROR_OTHER;

    }
}

Tase2_ClientError
Tase2_PointValue_getError(Tase2_PointValue self)
{
    if (self->type == TASE2_VALUE_TYPE_ERROR) {
        MmsDataAccessError err = MmsValue_getDataAccessError(self->pointValue);

        return convertDataAccessErrorToClientError(err);
    }
    else {
        return TASE2_CLIENT_ERROR_OK;
    }
}
