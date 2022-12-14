/*
 *  ber_decoder.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "ber_decode.h"
#include "libmms_platform_includes.h"

static int
getIndefiniteLength(uint8_t* buffer, int bufPos, int maxBufPos)
{
    int length = 0;

    while (bufPos < maxBufPos) {
        if ((buffer[bufPos] == 0) && (buffer[bufPos+1] == 0)) {
            return length + 2;
        }
        else {
            length++;

            if ((buffer[bufPos++] & 0x1f) == 0x1f) {
                /* handle extended tags */
                bufPos++;
                length++;
            }

            int subLength = -1;

            int newBufPos = BerDecoder_decodeLength(buffer, &subLength, bufPos, maxBufPos);

            if (newBufPos == -1)
                return -1;

            length += subLength + newBufPos - bufPos;

            bufPos = newBufPos + subLength;
        }
    }

    return -1;
}

int
BerDecoder_decodeLength(uint8_t* buffer, int* length, int bufPos, int maxBufPos)
{
    if (bufPos >= maxBufPos)
        return -1;

    uint8_t len1 = buffer[bufPos++];

    if (len1 & 0x80) {
        int lenLength = len1 & 0x7f;

        if (lenLength == 0) { /* indefinite length form */
            *length = getIndefiniteLength(buffer, bufPos, maxBufPos);
        }
        else {
            *length = 0;

            int i;
            for (i = 0; i < lenLength; i++) {
                if (bufPos >= maxBufPos)
                    return -1;

                *length <<= 8;
                *length += buffer[bufPos++];
            }
        }

    }
    else {
        *length = len1;
    }

    if (*length < 0)
        return -1;

    if (bufPos + (*length) > maxBufPos)
        return -1;

    return bufPos;
}

char*
BerDecoder_decodeString(uint8_t* buffer, int strlen, int bufPos, int maxBufPos)
{
    char* string = (char*) GLOBAL_MALLOC(strlen + 1);
    memcpy(string, buffer + bufPos, strlen);
    string[strlen] = 0;

    return string;
}

uint32_t
BerDecoder_decodeUint32(uint8_t* buffer, int intLen, int bufPos)
{
    uint32_t value = 0;

    int i;
    for (i = 0; i < intLen; i++) {
        value <<= 8;
        value += buffer[bufPos + i];
    }

    return value;
}

int32_t
BerDecoder_decodeInt32(uint8_t* buffer, int intlen, int bufPos)
{
    int32_t value;
    int i;

    bool isNegative = ((buffer[bufPos] & 0x80) == 0x80);

    if (isNegative)
        value = -1;
    else
        value = 0;

    for (i = 0; i < intlen; i++) {
        value <<= 8;
        value += buffer[bufPos + i];
    }

    return value;
}

float
BerDecoder_decodeFloat(uint8_t* buffer, int bufPos)
{
    float value;
    uint8_t* valueBuf = (uint8_t*) &value;

    int i;

    bufPos += 1; /* skip exponentWidth field */

#if (ORDER_LITTLE_ENDIAN == 1)
    for (i = 3; i >= 0; i--) {
        valueBuf[i] = buffer[bufPos++];
    }
#else
    for (i = 0; i < 4; i++) {
        valueBuf[i] = buffer[bufPos++];
    }
#endif

    return value;
}

double
BerDecoder_decodeDouble(uint8_t* buffer, int bufPos)
{
    double value;
    uint8_t* valueBuf = (uint8_t*) &value;

    int i;

    bufPos += 1; /* skip exponentWidth field */

#if (ORDER_LITTLE_ENDIAN == 1)
    for (i = 7; i >= 0; i--) {
        valueBuf[i] = buffer[bufPos++];
    }
#else
    for (i = 0; i < 8; i++) {
        valueBuf[i] = buffer[bufPos++];
    }
#endif

    return value;
}

bool
BerDecoder_decodeBoolean(uint8_t* buffer, int bufPos) {
    if (buffer[bufPos] != 0)
        return true;
    else
        return false;
}

void
BerDecoder_decodeOID(uint8_t* buffer, int bufPos, int length, ItuObjectIdentifier* oid)
{
    int startPos = bufPos;
    int currentArc = 0;

    /* clear all arcs */
    int i;
    for (i = 0; i < 10; i++)
        oid->arc[i] = 0;

    /* parse first two arcs */
    if (length > 0) {
        oid->arc[0] = buffer[bufPos] / 40;
        oid->arc[1] = buffer[bufPos] % 40;

        currentArc = 2;
        bufPos++;
    }

    /* parse remaining arcs */
    while ((bufPos - startPos < length) && (currentArc < 10)) {
        oid->arc[currentArc] = oid->arc[currentArc]<<7;

        if (buffer[bufPos] < 0x80)
            oid->arc[currentArc++] += buffer[bufPos];
        else
            oid->arc[currentArc] += (buffer[bufPos] & 0x7f);

        bufPos++;
    }

    oid->arcCount = currentArc;
}
