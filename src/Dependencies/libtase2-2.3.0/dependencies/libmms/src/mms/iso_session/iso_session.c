/*
 *  iso_session.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "stack_config.h"
#include "iso_session.h"
#include "libmms_platform_includes.h"
#include "mms_common_internal.h"
#include "buffer_chain.h"

#if ((DEBUG_ISO_SERVER == 1) || (DEBUG_ISO_CLIENT == 1))
#define DEBUG_SESSION 1
#else
#define DEBUG_SESSION 0
#endif

static int
parseAcceptParameters(IsoSession* session, ByteBuffer* message, int startOffset, int parameterLength)
{
    uint8_t pi;
    uint8_t param_len;
    uint8_t param_val;
    uint8_t hasProtocolOptions = 0;
    uint8_t hasProtocolVersion = 0;
    int offset = startOffset;
    int maxOffset = offset + parameterLength;

    while (offset < maxOffset) {
        pi = message->buffer[offset++];
        param_len = message->buffer[offset++];

        switch (pi) {
        case 19: /* Protocol options */
            if (param_len != 1)
                return -1;
            session->protocolOptions = message->buffer[offset++];
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - Protocol Options: %02x", session->protocolOptions);
            hasProtocolOptions = 1;
            break;
        case 21: /* TSDU Maximum Size */
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - TODO TSDU Maximum Size");
            offset += 4;
            break;
        case 22: /* Version Number */
            param_val = message->buffer[offset++];
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - Version number %i", param_val);

            if (param_val != 2) {

                if (DEBUG_SESSION)
                    libmms_iso_log(MMS_LOG_ERROR, "SESSION: invalid version number");

                return -1;
            }

            hasProtocolVersion = 1;
            break;
        case 23: /* Initial Serial Number */
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - TODO Initial Serial Number");
            offset += param_len;
            break;
        case 26: /* Token Setting Item */
            param_val = message->buffer[offset++];
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - Token Setting Item: %02x", param_val);
            break;
        case 55: /* Second Initial Serial Number */
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - TODO Second Initial Serial Number");
            offset += param_len;
            break;
        case 56: /* Upper Limit Serial Number */
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - TODO Upper Limit Serial Number");
            offset += param_len;
            break;
        case 57: /* Large Initial Serial Number */
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - TODO Large Initial Serial Number");
            offset += param_len;
            break;
        case 58: /* Large Second Initial Serial Number */
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - TODO Large Second Initial Serial Number");
            offset += param_len;
            break;
        default:
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: Param - Invalid Parameter with ID %02x\n", pi);
            break;
        }
    }

    if (hasProtocolOptions && hasProtocolVersion)
        return offset - startOffset;
    else
        return -1;
}

static IsoSessionIndication
parseSessionHeaderParameters(IsoSession* session, ByteBuffer* message, int parametersOctets)
{
    int offset = 2;
    uint8_t pgi;
    uint8_t parameterLength;

    while (offset < (parametersOctets + 2)) {
        pgi = message->buffer[offset++];
        parameterLength = message->buffer[offset++];

        switch (pgi) {
        case 1: /* Connection Identifier */
            offset += parameterLength;
            break;
        case 5: /* Connection/Accept Item */
            {
                int connectAcceptLen;

                connectAcceptLen = parseAcceptParameters(session, message, offset, parameterLength);

                if (connectAcceptLen == -1)
                    return SESSION_ERROR;

                offset += connectAcceptLen;
            }
            break;
        case 17: /* Transport disconnect */
            offset += parameterLength;
            break;
        case 20: /* Session User Requirements */

            if (parameterLength != 2)
                return SESSION_ERROR;

            session->sessionRequirement = message->buffer[offset++] * 0x100;
            session->sessionRequirement += message->buffer[offset++];
            break;
        case 25: /* Enclosure item */
            offset += parameterLength;
            break;
        case 49:
            offset += parameterLength;
            break;
        case 51: /* Calling Session Selector */

            if (parameterLength > 16)
                return SESSION_ERROR;

            {
                session->callingSessionSelector.size = parameterLength;

                int i;
                for (i = 0; i < session->callingSessionSelector.size; i++) {
                    session->callingSessionSelector.value[i] = message->buffer[offset++];
                }
            }

            break;
        case 52: /* Called Session Selector */

            if (parameterLength > 16)
                return SESSION_ERROR;

            {
               session->calledSessionSelector.size = parameterLength;

               int i;
               for (i = 0; i < session->calledSessionSelector.size; i++) {
                   session->calledSessionSelector.value[i] = message->buffer[offset++];
               }
            }

            break;
        case 60: /* Data Overflow */

            offset += parameterLength;
            break;
        case 193: /* User Data */

            /* here we should return - the remaining data is for upper layers ! */
            ByteBuffer_wrap(&session->userData, message->buffer + offset,
                    message->size - offset, message->maxSize - offset);

            return SESSION_OK;

        case 194: /* Extended User Data */
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_DEBUG, "SESSION: PGI - extended user data");
            break;
        default:
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_WARNING, "SESSION: invalid parameter/PGI");
            break;
        }
    }

    return SESSION_ERROR;
}

static const uint8_t dataSpdu[] = { 0x01, 0x00, 0x01, 0x00 };

void
IsoSession_createDataSpdu(IsoSession* session, BufferChain buffer, BufferChain payload)
{
    buffer->buffer = (uint8_t*) dataSpdu;
    buffer->partLength = 4;
    buffer->length = 4 + payload->length;
    buffer->nextPart = payload;
}

static int
encodeConnectAcceptItem(uint8_t* buf, int offset, uint8_t options)
{
    buf[offset++] = 5;
    buf[offset++] = 6;
    buf[offset++] = 0x13; /* Protocol Options */
    buf[offset++] = 1;
    buf[offset++] = options;
    buf[offset++] = 0x16; /* Version Number */
    buf[offset++] = 1;
    buf[offset++] = 2; /* Version = 2 */

    return offset;
}

static int
encodeConnectionIdentifier(uint8_t* buf, int offset, uint8_t reasonCode)
{
    buf[offset++] = 1; /* Connection Identifier */
    buf[offset++] = 2; /* LEN */
    buf[offset++] = 17; /* Transport Disconnect */
    buf[offset++] = 1; /* L=1 */
    buf[offset++] = 1; /* VALUE = release transport connection */
    buf[offset++] = 50; /* Reason code */
    buf[offset++] = 1; /* L=1 */
    buf[offset++] = reasonCode;

    return offset;
}

static int
encodeSessionRequirement(IsoSession* self, uint8_t* buf, int offset)
{
    buf[offset++] = 0x14;
    buf[offset++] = 2;
    buf[offset++] = (uint8_t) (self->sessionRequirement / 0x100);
    buf[offset++] = (uint8_t) (self->sessionRequirement & 0x00ff);

    return offset;
}

static int
encodeCallingSessionSelector(IsoSession* self, uint8_t* buf, int offset)
{
    buf[offset++] = 0x33;
    buf[offset++] = self->callingSessionSelector.size;

    int i;
    for (i = 0; i < self->callingSessionSelector.size; i++)
        buf[offset++] = self->callingSessionSelector.value[i];

    return offset;
}

static int
encodeCalledSessionSelector(IsoSession* self, uint8_t* buf, int offset)
{
    buf[offset++] = 0x34;
    buf[offset++] = self->calledSessionSelector.size;

    int i;
    for (i = 0; i < self->calledSessionSelector.size; i++)
        buf[offset++] = self->calledSessionSelector.value[i];

    return offset;
}

static int
encodeSessionUserData(IsoSession* self, uint8_t* buf, int offset, uint8_t payloadLength)
{
    buf[offset++] = 0xc1;
    buf[offset++] = payloadLength;

    return offset;
}

void
IsoSession_createConnectSpdu(IsoSession* self, IsoApplicationAddress remoteAddress, IsoApplicationAddress localAddress, BufferChain buffer, BufferChain payload)
{
    int offset = 0;
    uint8_t* buf = buffer->buffer;
    int lengthOffset;

    buf[offset++] = 13; /* CONNECT SPDU */
    lengthOffset = offset;

    /* TODO add support for longer message payloads - length field can be 1 or 3 byte! */
    offset++; /* Skip byte for length - fill it later */

    self->calledSessionSelector = remoteAddress->sSelector;
    self->callingSessionSelector = localAddress->sSelector;

    offset = encodeConnectAcceptItem(buf, offset, 0);

    offset = encodeSessionRequirement(self, buf, offset);

    offset = encodeCallingSessionSelector(self, buf, offset);

    offset = encodeCalledSessionSelector(self, buf, offset);

    offset = encodeSessionUserData(self, buf, offset, payload->length);

    int spduLength = (offset - lengthOffset - 1) + payload->length;

    buf[lengthOffset] = spduLength;

    buffer->partLength = offset;
    buffer->length = offset + payload->length;
    buffer->nextPart = payload;
}

void
IsoSession_createAbortSpdu(IsoSession* self, BufferChain buffer, BufferChain payload)
{
    int offset = 0;
    uint8_t* buf = buffer->buffer;

    buf[offset++] = 25; /* ABORT-SPDU code */
    buf[offset++] = 5 + payload->length; /* LI */
    buf[offset++] = 17; /* PI-Code transport-disconnect */
    buf[offset++] = 1; /* LI = 1 */
    buf[offset++] = 11; /* transport-connection-released | user-abort | no-reason */
    buf[offset++] = 193; /* PGI-Code user data */
    buf[offset++] = payload->length; /* LI of user data */

    buffer->partLength = offset;
    buffer->length = payload->length + offset;
    buffer->nextPart = payload;
}

void
IsoSession_createFinishSpdu(IsoSession* self, BufferChain buffer, BufferChain payload)
{
    int offset = 0;
    uint8_t* buf = buffer->buffer;

    buf[offset++] = 9; /* FINISH-SPDU code */

    buf[offset++] = 2 + payload->length; /* LI */
    buf[offset++] = 193; /* PGI-Code user data */
    buf[offset++] = payload->length; /* LI of user data */

    buffer->partLength = offset;
    buffer->length = payload->length + offset;
    buffer->nextPart = payload;
}

void
IsoSession_createDisconnectSpdu(IsoSession* self, BufferChain buffer, BufferChain payload)
{
    int offset = 0;
    uint8_t* buf = buffer->buffer;

    buf[offset++] = 10; /* DISCONNECT-SPDU code */

    buf[offset++] = 2 + payload->length; /* LI */
    buf[offset++] = 193; /* PGI-Code user data */
    buf[offset++] = payload->length; /* LI of user data */

    buffer->partLength = offset;
    buffer->length = payload->length + offset;
    buffer->nextPart = payload;
}

void
IsoSession_createAcceptSpdu(IsoSession* self, BufferChain buffer, BufferChain payload)
{
    int offset = 0;
    uint8_t* buf = buffer->buffer;
    int lengthOffset;

    int payloadLength = payload->length;

    buf[offset++] = 14; /* ACCEPT SPDU */
    lengthOffset = offset;

    /* TODO add support for longer message payloads - length field can be 1 or 3 byte! */
    offset++;

    offset = encodeConnectAcceptItem(buf, offset, self->protocolOptions);

    offset = encodeSessionRequirement(self, buf, offset);

    offset = encodeCalledSessionSelector(self, buf, offset);

    offset = encodeSessionUserData(self, buf, offset, payloadLength);

    int spduLength = (offset - lengthOffset - 1) + payloadLength;

    buf[lengthOffset] = spduLength;

    buffer->partLength = offset;
    buffer->length = offset + payloadLength;
    buffer->nextPart = payload;
}

void
IsoSession_createRefuseSpdu(IsoSession* self, BufferChain buffer, BufferChain payload, uint8_t reasonCode)
{
    int offset = 0;
    uint8_t* buf = buffer->buffer;
    int lengthOffset;

    buf[offset++] = 12; /* REFUSE SPDU */
    lengthOffset = offset;
    offset++;

    offset = encodeConnectionIdentifier(buf, offset, reasonCode);

}

void
IsoSession_init(IsoSession* session)
{
    memset(session, 0, sizeof(IsoSession));
    session->sessionRequirement = 0x0002; /* default = duplex functional unit */

    session->callingSessionSelector.size = 2;
    session->callingSessionSelector.value[0] = 0;
    session->callingSessionSelector.value[1] = 1;

    session->calledSessionSelector.size = 2;
    session->calledSessionSelector.value[0] = 0;
    session->calledSessionSelector.value[1] = 1;
}

ByteBuffer*
IsoSession_getUserData(IsoSession* session)
{
    return &session->userData;
}

IsoSessionIndication
IsoSession_parseMessage(IsoSession* session, ByteBuffer* message)
{
    uint8_t* buffer = message->buffer;
    uint8_t id;
    uint8_t length;

    if (message->size > 1) {
        id = buffer[0];
        length = buffer[1];
    }
    else
        return SESSION_ERROR;

    switch (id) {
    case 13: /* CONNECT(CN) SPDU */
        if (length != (message->size - 2))
            return SESSION_ERROR;
        if (parseSessionHeaderParameters(session, message, length) == SESSION_OK)
            return SESSION_CONNECT;
        else {
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_ERROR, "SESSION: error parsing connect SPDU");
            return SESSION_ERROR;
        }
        break;
    case 14: /* ACCEPT SPDU */
        if (length != (message->size - 2))
            return SESSION_ERROR;
        if (parseSessionHeaderParameters(session, message, length) == SESSION_OK)
            return SESSION_CONNECT;
        else {
            if (DEBUG_SESSION)
                libmms_iso_log(MMS_LOG_ERROR, "SESSION: error parsing accept SPDU");
            return SESSION_ERROR;
        }

        break;
    case 1: /* Give token / data SPDU */
        if (message->size < 4)
            return SESSION_ERROR;

        if ((length == 0) && (buffer[2] == 1) && (buffer[3] == 0)) {
            ByteBuffer_wrap(&session->userData, message->buffer + 4, message->size - 4, message->maxSize - 4);

            return SESSION_DATA;
        }
        return SESSION_ERROR;

    case 8: /* NOT-FINISHED SPDU */
        return SESSION_NOT_FINISHED;

    case 9: /* FINISH SPDU */
        if (DEBUG_SESSION)
            libmms_iso_log(MMS_LOG_INFO, "SESSION: recvd FINISH SPDU");

        if (length != (message->size - 2))
            return SESSION_ERROR;

        if (parseSessionHeaderParameters(session, message, length) == SESSION_OK)
            return SESSION_FINISH;
        else
            return SESSION_ERROR;

        break;

    case 10: /* DISCONNECT SPDU */
        if (DEBUG_SESSION)
            libmms_iso_log(MMS_LOG_INFO, "SESSION: recvd DISCONNECT SPDU");

        if (length != (message->size - 2))
            return SESSION_ERROR;

        if (parseSessionHeaderParameters(session, message, length) == SESSION_OK)
            return SESSION_DISCONNECT;
        else
            return SESSION_ERROR;

        break;

    case 25: /* ABORT SPDU */
        return SESSION_ABORT;

    default:
        break;
    }

    return SESSION_ERROR;
}

