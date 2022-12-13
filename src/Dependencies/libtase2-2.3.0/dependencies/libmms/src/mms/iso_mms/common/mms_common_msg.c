/*
 *  mms_common_msg.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_common_internal.h"
#include "stack_config.h"
#include "mms_value_internal.h"
#include "ber_decode.h"
#include "ber_encoder.h"

static void
mmsMsg_createFloatData(MmsValue* value, int* size, uint8_t** buf)
{
    if (value->value.floatingPoint.formatWidth == 64) {
        *size = 9;
        *buf = (uint8_t*) GLOBAL_MALLOC(9);
        (*buf)[0] = 11;
#if (ORDER_LITTLE_ENDIAN == 1)
        memcpyReverseByteOrder((*buf) + 1, value->value.floatingPoint.buf, 8);
#else
        memcpy((*buf) + 1, value->value.floatingPoint.buf, 8);
#endif
    } else {
        *size = 5;
        *buf = (uint8_t*) GLOBAL_MALLOC(5);
        (*buf)[0] = 8;
#if (ORDER_LITTLE_ENDIAN == 1)
        memcpyReverseByteOrder((*buf) + 1, value->value.floatingPoint.buf, 4);
#else
        memcpy((*buf) + 1, value->value.floatingPoint.buf, 4);
#endif
    }
}

Data_t*
mmsMsg_createBasicDataElement(MmsValue* value)
{
    Data_t* dataElement = (Data_t*) GLOBAL_CALLOC(1, sizeof(Data_t));

    switch (value->type) {
    case MMS_ARRAY:
        {
            int size = MmsValue_getArraySize(value);
            dataElement->present = Data_PR_array;
            dataElement->choice.array = (DataSequence_t*) GLOBAL_CALLOC(1, sizeof(DataSequence_t));
            dataElement->choice.array->list.count = size;
            dataElement->choice.array->list.size = size;
            dataElement->choice.array->list.array = (Data_t**) GLOBAL_CALLOC(size, sizeof(Data_t*));
            int i;
            for (i = 0; i < size; i++) {
                dataElement->choice.array->list.array[i] =
                        mmsMsg_createBasicDataElement(MmsValue_getElement(value, i));
            }
        }
        break;

    case MMS_STRUCTURE:
        {
            int size = value->value.structure.size;

            dataElement->present = Data_PR_structure;
            dataElement->choice.structure = (DataSequence_t*) GLOBAL_CALLOC(1, sizeof(DataSequence_t));
            dataElement->choice.structure->list.count = size;
            dataElement->choice.structure->list.size = size;
            dataElement->choice.structure->list.array = (Data_t**) GLOBAL_CALLOC(size, sizeof(Data_t*));
            int i;
            for (i = 0; i < size; i++) {
                dataElement->choice.structure->list.array[i] = mmsMsg_createBasicDataElement(
                        value->value.structure.components[i]);
            }
        }
        break;

    case MMS_BIT_STRING: 
		{
			dataElement->present = Data_PR_bitstring;
			dataElement->choice.bitstring.buf = value->value.bitString.buf;
			int size = (value->value.bitString.size / 8) + ((value->value.bitString.size % 8) > 0);
			int unused = 8 - (value->value.bitString.size % 8);
			dataElement->choice.bitstring.size = size; /* size in bytes */
			dataElement->choice.bitstring.bits_unused = unused;
		}
        break;

    case MMS_BOOLEAN:
        dataElement->present = Data_PR_boolean;
        dataElement->choice.boolean = value->value.boolean;
        break;

    case MMS_FLOAT:
        dataElement->present = Data_PR_floatingpoint;

        mmsMsg_createFloatData(value, &dataElement->choice.floatingpoint.size,
                &dataElement->choice.floatingpoint.buf);
        break;

    case MMS_UTC_TIME:
        dataElement->present = Data_PR_utctime;

        dataElement->choice.utctime.buf = (uint8_t*) GLOBAL_MALLOC(8);
        memcpy(dataElement->choice.utctime.buf, value->value.utcTime, 8);
        dataElement->choice.utctime.size = 8;
        break;

    case MMS_INTEGER:
        dataElement->present = Data_PR_integer;

        dataElement->choice.integer.size = value->value.integer->size;
        dataElement->choice.integer.buf = value->value.integer->octets;

        break;

    case MMS_UNSIGNED:
        dataElement->present = Data_PR_unsigned;

        dataElement->choice.Unsigned.size = value->value.integer->size;
        dataElement->choice.Unsigned.buf = value->value.integer->octets;

        break;

    case MMS_VISIBLE_STRING:
        dataElement->present = Data_PR_visiblestring;
        if (value->value.visibleString.buf != NULL ) {
            dataElement->choice.visiblestring.buf = (uint8_t*) value->value.visibleString.buf;
            dataElement->choice.visiblestring.size = strlen(value->value.visibleString.buf);
        } else
            dataElement->choice.visiblestring.size = 0;
        break;

    case MMS_OCTET_STRING:
        dataElement->present = Data_PR_octetstring;
        if (value->value.octetString.buf != NULL ) {
            dataElement->choice.octetstring.buf = value->value.octetString.buf;
            dataElement->choice.octetstring.size =
                    value->value.octetString.size;
        } else
            dataElement->choice.octetstring.size = 0;
        break;

    case MMS_STRING:
        dataElement->present = Data_PR_mMSString;
        if (value->value.visibleString.buf != NULL ) {
            dataElement->choice.mMSString.buf = (uint8_t*) value->value.visibleString.buf;
            dataElement->choice.mMSString.size = strlen(value->value.visibleString.buf);
        } else
            dataElement->choice.mMSString.size = 0;
        break;

    case MMS_BINARY_TIME:
        dataElement->present = Data_PR_binarytime;
        dataElement->choice.binarytime.size = value->value.binaryTime.size;
        dataElement->choice.binarytime.buf = value->value.binaryTime.buf;
        break;

    default:
        dataElement->present = Data_PR_NOTHING;

        break;
    }

    return dataElement;
}

MmsValue*
mmsMsg_parseDataElement(Data_t* dataElement)
{
    MmsValue* value = NULL;

    if (dataElement->present == Data_PR_structure) {
        value = (MmsValue*) GLOBAL_CALLOC(1, sizeof(MmsValue));

        int componentCount = dataElement->choice.structure->list.count;

        value->type = MMS_STRUCTURE;
        value->value.structure.size = componentCount;
        value->value.structure.components = (MmsValue**) GLOBAL_CALLOC(componentCount, sizeof(MmsValue*));

        int i;

        for (i = 0; i < componentCount; i++) {
            value->value.structure.components[i] =
                    mmsMsg_parseDataElement(dataElement->choice.structure->list.array[i]);
        }
    }
    else if (dataElement->present == Data_PR_array) {
        value = (MmsValue*) GLOBAL_CALLOC(1, sizeof(MmsValue));

        int componentCount = dataElement->choice.array->list.count;

        value->type = MMS_ARRAY;
        value->value.structure.size = componentCount;
        value->value.structure.components = (MmsValue**) GLOBAL_CALLOC(componentCount, sizeof(MmsValue*));

        int i;

        for (i = 0; i < componentCount; i++) {
            value->value.structure.components[i] =
                    mmsMsg_parseDataElement(dataElement->choice.array->list.array[i]);
        }
    }
    else {
        if (dataElement->present == Data_PR_integer) {
            Asn1PrimitiveValue* berInteger = BerInteger_createFromBuffer(
                    dataElement->choice.integer.buf, dataElement->choice.integer.size);

            value = MmsValue_newIntegerFromBerInteger(berInteger);
        }
        else if (dataElement->present == Data_PR_unsigned) {
            Asn1PrimitiveValue* berInteger = BerInteger_createFromBuffer(
                    dataElement->choice.Unsigned.buf, dataElement->choice.Unsigned.size);

            value = MmsValue_newUnsignedFromBerInteger(berInteger);
        }
        else if (dataElement->present == Data_PR_visiblestring) {
            value = MmsValue_newVisibleStringFromByteArray(dataElement->choice.visiblestring.buf,
                    dataElement->choice.visiblestring.size);
        }
        else if (dataElement->present == Data_PR_mMSString) {
            value = MmsValue_newMmsStringFromByteArray(dataElement->choice.mMSString.buf,
                    dataElement->choice.mMSString.size);
        }
        else if (dataElement->present == Data_PR_bitstring) {
            value = (MmsValue*) GLOBAL_CALLOC(1, sizeof(MmsValue));

            value->type = MMS_BIT_STRING;
            int size = dataElement->choice.bitstring.size;

            value->value.bitString.size = (size * 8)
                    - dataElement->choice.bitstring.bits_unused;

            value->value.bitString.buf = (uint8_t*) GLOBAL_MALLOC(size);
            memcpy(value->value.bitString.buf,
                    dataElement->choice.bitstring.buf, size);

        }
        else if (dataElement->present == Data_PR_floatingpoint) {
            value = (MmsValue*) GLOBAL_CALLOC(1, sizeof(MmsValue));
            int size = dataElement->choice.floatingpoint.size;

            value->type = MMS_FLOAT;

            if (size == 5) { /* FLOAT32 */
                value->value.floatingPoint.formatWidth = 32;
                value->value.floatingPoint.exponentWidth = dataElement->choice.floatingpoint.buf[0];

                uint8_t* floatBuf = (dataElement->choice.floatingpoint.buf + 1);

                value->value.floatingPoint.buf = (uint8_t*) GLOBAL_MALLOC(4);
#if (ORDER_LITTLE_ENDIAN == 1)
                memcpyReverseByteOrder(value->value.floatingPoint.buf, floatBuf, 4);
#else
                memcpy(value->value.floatingPoint.buf, floatBuf, 4);
#endif
            }

            if (size == 9) { /* FLOAT64 */
                value->value.floatingPoint.formatWidth = 64;
                value->value.floatingPoint.exponentWidth = dataElement->choice.floatingpoint.buf[0];

                uint8_t* floatBuf = (dataElement->choice.floatingpoint.buf + 1);

                value->value.floatingPoint.buf = (uint8_t*) GLOBAL_MALLOC(8);
#if (ORDER_LITTLE_ENDIAN == 1)
                memcpyReverseByteOrder(value->value.floatingPoint.buf, floatBuf, 8);
#else
                memcpy(value->value.floatingPoint.buf, floatBuf, 8);
#endif
            }
        }
        else if (dataElement->present == Data_PR_utctime) {
            value = (MmsValue*) GLOBAL_CALLOC(1, sizeof(MmsValue));
            value->type = MMS_UTC_TIME;
            memcpy(value->value.utcTime, dataElement->choice.utctime.buf, 8);
        }
        else if (dataElement->present == Data_PR_octetstring) {
            value = (MmsValue*) GLOBAL_CALLOC(1, sizeof(MmsValue));
            value->type = MMS_OCTET_STRING;
            int size = dataElement->choice.octetstring.size;
            value->value.octetString.size = size;
            value->value.octetString.maxSize = size;
            value->value.octetString.buf = (uint8_t*) GLOBAL_MALLOC(size);
            memcpy(value->value.octetString.buf, dataElement->choice.octetstring.buf, size);
        }
        else if (dataElement->present == Data_PR_binarytime) {
            int size = dataElement->choice.binarytime.size;

            if (size <= 6) {
                value = (MmsValue*) GLOBAL_CALLOC(1, sizeof(MmsValue));
                value->type = MMS_BINARY_TIME;
                value->value.binaryTime.size = size;
                memcpy(value->value.binaryTime.buf, dataElement->choice.binarytime.buf, size);
            }
        }
        else if (dataElement->present == Data_PR_boolean) {
            value = MmsValue_newBoolean(dataElement->choice.boolean);
        }

    }

    return value;
}

char*
mmsMsg_createStringFromAsnIdentifier(Identifier_t identifier)
{
    char* str = StringUtils_createStringFromBuffer(identifier.buf, identifier.size);

    return str;
}


void
mmsMsg_copyAsn1IdentifierToStringBuffer(Identifier_t identifier, char* buffer, int bufSize)
{
    if (identifier.size < bufSize) {
        memcpy(buffer, identifier.buf, identifier.size);
        buffer[identifier.size] = 0;
    }
    else {
        /* TODO add return value */
        buffer[0] = 0;
    }
}

#if (MMS_FILE_SERVICE == 1)

void
mmsMsg_createExtendedFilename(const char* basepath, char* extendedFileName, char* fileName)
{
#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    strncpy(extendedFileName, basepath, 512);
    strncat(extendedFileName, fileName, 512);
#else
    strcpy(extendedFileName, CONFIG_VIRTUAL_FILESTORE_BASEPATH);
    strncat(extendedFileName, fileName, sizeof(CONFIG_VIRTUAL_FILESTORE_BASEPATH) + 256);
#endif
}


FileHandle
mmsMsg_openFile(const char* basepath, char* fileName, bool readWrite)
{
#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    char extendedFileName[512];
#else
    char extendedFileName[sizeof(CONFIG_VIRTUAL_FILESTORE_BASEPATH) + 256];
#endif

    mmsMsg_createExtendedFilename(basepath, extendedFileName, fileName);

    return FileSystem_openFile(extendedFileName, readWrite);
}

#endif /* (MMS_FILE_SERVICE == 1) */

/**********************************************************************************************
 * MMS Common support functions
 *********************************************************************************************/

#define MMS_REJECT_CONFIRMED_REQUEST 1
#define MMS_REJECT_CONFIRMED_RESPONSE 2
#define MMS_REJECT_CONFIRMED_ERROR 3
#define MMS_REJECT_UNCONFIRMED 4
#define MMS_REJECT_PDU_ERROR 5
#define MMS_REJECT_CANCEL_REQUEST 6
#define MMS_REJECT_CANCEL_RESPONSE 7
#define MMS_REJECT_CANCEL_ERROR 8
#define MMS_REJECT_CONCLUDE_REQUEST 9
#define MMS_REJECT_CONCLUDE_RESPONSE 10
#define MMS_REJECT_CONCLUDE_ERROR 11

#define MMS_REJECT_CONFIRMED_REQUEST_OTHER 0
#define MMS_REJECT_CONFIRMED_REQUEST_UNRECOGNIZED_SERVICE 1
#define MMS_REJECT_CONFIRMED_REQUEST_UNRECOGNIZED_MODIFIER 2
#define MMS_REJECT_CONFIRMED_REQUEST_INVALID_INVOKE_ID 3
#define MMS_REJECT_CONFIRMED_REQUEST_INVALID_ARGUMENT 4
#define MMS_REJECT_CONFIRMED_REQUEST_INVALID_MODIFIER 5
#define MMS_REJECT_CONFIRMED_REQUEST_MAX_SERV_OUTSTANDING_EXCEEDED 6
#define MMS_REJECT_CONFIRMED_REQUEST_MAX_RECURSION_EXCEEDED 8
#define MMS_REJECT_CONFIRMED_REQUEST_VALUE_OUT_OF_RANGE 9

#define MMS_REJECT_PDU_ERROR_UNKNOWN_PDU_TYPE 0
#define MMS_REJECT_PDU_ERROR_INVALID_PDU 1
#define MMS_REJECT_PDU_ERROR_ILLEGAL_ACSI_MAPPING 2


static void
mmsMsg_encodeMmsRejectPdu(uint32_t* invokeId, int rejectType, int rejectReason, ByteBuffer* response)
{
    int bufPos = 0;
    uint8_t* buffer = response->buffer;

    uint32_t invokeIdLength = 0;

    uint32_t rejectPduLength = 3;

    if (invokeId != NULL) {
        invokeIdLength = BerEncoder_UInt32determineEncodedSize(*invokeId);
        rejectPduLength += 2 + invokeIdLength;
    }

    /* Encode reject PDU */
    bufPos = BerEncoder_encodeTL(0xa4, rejectPduLength, buffer, bufPos);

    if (invokeId != NULL) {
        /* original invokeId */
        bufPos = BerEncoder_encodeTL(0x80, invokeIdLength, buffer, bufPos);
        bufPos = BerEncoder_encodeUInt32(*invokeId, buffer, bufPos);
    }

    buffer[bufPos++] = (uint8_t) (0x80 + rejectType);
    buffer[bufPos++] = 0x01;
    buffer[bufPos++] = (uint8_t) rejectReason;

    response->size = bufPos;
}

void
mmsMsg_createMmsRejectPdu(uint32_t* invokeId, int reason, ByteBuffer* response)
{
    int rejectType = 0;
    int rejectReason = 0;

    switch (reason)
    {

    case MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE:
        rejectType = MMS_REJECT_CONFIRMED_REQUEST;
        rejectReason = MMS_REJECT_CONFIRMED_REQUEST_UNRECOGNIZED_SERVICE;
        break;

    case MMS_ERROR_REJECT_UNKNOWN_PDU_TYPE:
        rejectType = MMS_REJECT_PDU_ERROR;
        rejectReason = MMS_REJECT_PDU_ERROR_UNKNOWN_PDU_TYPE;
        break;

    case MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT:
        rejectType = MMS_REJECT_CONFIRMED_REQUEST;
        rejectReason = MMS_REJECT_CONFIRMED_REQUEST_INVALID_ARGUMENT;
        break;

    case MMS_ERROR_REJECT_INVALID_PDU:
        rejectType = MMS_REJECT_PDU_ERROR;
        rejectReason = MMS_REJECT_PDU_ERROR_INVALID_PDU;
        break;

    case MMS_ERROR_REJECT_MAX_OUTSTANDING_SERVICES_EXCEEDED:
        rejectType = MMS_REJECT_CONFIRMED_REQUEST;
        rejectReason = MMS_REJECT_CONFIRMED_REQUEST_MAX_SERV_OUTSTANDING_EXCEEDED;
        break;

    default:
        rejectType = MMS_REJECT_CONFIRMED_REQUEST;
        rejectReason = MMS_REJECT_CONFIRMED_REQUEST_OTHER;
    }

    mmsMsg_encodeMmsRejectPdu(invokeId, rejectType, rejectReason, response);
}


