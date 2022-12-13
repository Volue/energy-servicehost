/*
 *  iso_presentation.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "iso_presentation.h"
#include "libmms_platform_includes.h"
#include "mms_common_internal.h"
#include "stack_config.h"
#include "ber_encoder.h"
#include "ber_decode.h"
#include "buffer_chain.h"

#if ((DEBUG_ISO_SERVER == 1) || (DEBUG_ISO_CLIENT == 1))
#define DEBUG_PRES 1
#else
#define DEBUG_PRES 0
#endif

static uint8_t asn_id_as_acse[] = { 0x52, 0x01, 0x00, 0x01 };

static uint8_t asn_id_mms[] = { 0x28, 0xca, 0x22, 0x02, 0x01 };

static uint8_t ber_id[] = { 0x51, 0x01 };

static int
encodeAcceptBer(uint8_t* buffer, int bufPos)
{
    bufPos = BerEncoder_encodeTL(0x30, 7, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x80, 1, buffer, bufPos);
    buffer[bufPos++] = 0;
    bufPos = BerEncoder_encodeTL(0x81, 2, buffer, bufPos);
    buffer[bufPos++] = 0x51;
    buffer[bufPos++] = 0x01;

    return bufPos;
}

static int
encodeUserData(uint8_t* buffer, int bufPos,
        BufferChain payload, bool encode, uint8_t contextId)
{
    int payloadLength = payload->length;

    int encodedDataSetLength = 3; /* presentation-selector */

    /* presentation-data */
    encodedDataSetLength += payloadLength + 1;
    encodedDataSetLength += BerEncoder_determineLengthSize(payloadLength);

    int fullyEncodedDataLength = encodedDataSetLength;

    fullyEncodedDataLength += BerEncoder_determineLengthSize(encodedDataSetLength) + 1;

    if (encode) {
        /* fully-encoded-data */
        bufPos = BerEncoder_encodeTL(0x61, fullyEncodedDataLength, buffer, bufPos);
        bufPos = BerEncoder_encodeTL(0x30, encodedDataSetLength, buffer, bufPos);

        /* presentation-selector acse */
        bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
        buffer[bufPos++] = contextId;

        /* presentation-data (= acse payload) */
        bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);

        return bufPos;
    }
    else {
        int encodedUserDataLength = fullyEncodedDataLength + 1;
        encodedUserDataLength += BerEncoder_determineLengthSize(fullyEncodedDataLength);

        return encodedUserDataLength;
    }
}

/**
 * \brief Create CP message (Connect Presentation PPDU)
 */
void
IsoPresentation_createConnectPdu(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    self->acseContextId = 1;
    self->mmsContextId = 3;

    int contentLength = 0;
    int i;

    /* mode-selector */
    contentLength += 5;

    int normalModeLength = 0;

    /* called- and calling-presentation-selector */
    normalModeLength += 12;

    int pclLength = 27; /* 35; */
    pclLength += self->localAddress->pSelector.size;
    pclLength += self->remoteAddress->pSelector.size;

    normalModeLength += pclLength;

    normalModeLength += encodeUserData(NULL, 0, payload, false, self->acseContextId);

    normalModeLength += 2;

    contentLength += normalModeLength;

    contentLength += 1 + BerEncoder_determineLengthSize(normalModeLength);

    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0x31, contentLength, buffer, bufPos);

    /* mode-selector */
    bufPos = BerEncoder_encodeTL(0xa0, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x80, 1, buffer, bufPos);
    buffer[bufPos++] = 1; /* 1 = normal-mode */

    /* normal-mode-parameters */
    bufPos = BerEncoder_encodeTL(0xa2, normalModeLength, buffer, bufPos);

    /* calling-presentation-selector */
    bufPos = BerEncoder_encodeTL(0x81, self->localAddress->pSelector.size, buffer, bufPos);
    for (i = 0; i < self->localAddress->pSelector.size; i++)
        buffer[bufPos++] = self->localAddress->pSelector.value[i];

    /* called-presentation-selector */
    bufPos = BerEncoder_encodeTL(0x82, self->remoteAddress->pSelector.size, buffer, bufPos);
    for (i = 0; i < self->remoteAddress->pSelector.size; i++)
        buffer[bufPos++] = self->remoteAddress->pSelector.value[i];

    /* presentation-context-id list */
    bufPos = BerEncoder_encodeTL(0xa4, 35, buffer, bufPos);

    /* acse context list item */
    bufPos = BerEncoder_encodeTL(0x30, 15, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
    buffer[bufPos++] = 1;

    bufPos = BerEncoder_encodeTL(0x06, 4, buffer, bufPos);
    memcpy(buffer + bufPos, asn_id_as_acse, 4);
    bufPos += 4;

    bufPos = BerEncoder_encodeTL(0x30, 4, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x06, 2, buffer, bufPos);
    memcpy(buffer + bufPos, ber_id, 2);
    bufPos += 2;

    /* mms context list item */
    bufPos = BerEncoder_encodeTL(0x30, 16, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
    buffer[bufPos++] = 3;

    bufPos = BerEncoder_encodeTL(0x06, 5, buffer, bufPos);
    memcpy(buffer + bufPos, asn_id_mms, 5);
    bufPos += 5;

    bufPos = BerEncoder_encodeTL(0x30, 4, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x06, 2, buffer, bufPos);
    memcpy(buffer + bufPos, ber_id, 2);
    bufPos += 2;

    /* encode user data */
    bufPos = encodeUserData(buffer, bufPos, payload, true, self->acseContextId);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payload->length;
    writeBuffer->nextPart = payload;
}

static int
parseFullyEncodedData(IsoPresentation* self, uint8_t* buffer, int len, int bufPos)
{
    int presentationSelector = -1;
    bool userDataPresent = false;

    int endPos = bufPos + len;

    if (buffer[bufPos++] != 0x30) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: user-data parse error");
        return -1;
    }

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, endPos);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: wrong parameter length");
        return -1;
    }

    endPos = bufPos + len;

    while (bufPos < endPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, endPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_ERROR, "PRES: wrong parameter length");
            return -1;
        }

        switch (tag) {
        case 0x02: /* presentation-context-identifier */
            {
                presentationSelector = BerDecoder_decodeUint32(buffer, length, bufPos);
                self->nextContextId = presentationSelector;
                bufPos += length;

                if (DEBUG_PRES)
                    if (DEBUG_PRES)
                        libmms_iso_log(MMS_LOG_DEBUG, "PRES: presentation-context-identifier %i", presentationSelector);
            }
            break;

        case 0xa0:
            userDataPresent = true;

            self->nextPayload.buffer = buffer + bufPos;
            self->nextPayload.size = length;

            bufPos += length;
            break;
        case 0x00: /* indefinite length end tag -> ignore */
            break;
        default:
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_WARNING, "PRES: unknown tag %02x", tag);

            bufPos += length;
            break;
        }
    }

    if (!userDataPresent) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: user-data not present");

        return -1;
    }

    return bufPos;
}

static int
parsePCDLEntry(IsoPresentation* self, uint8_t* buffer, int totalLength, int bufPos)
{
    int endPos = bufPos + totalLength;

    int contextId = -1;
    bool isAcse = false;
    bool isMms = false;

    while (bufPos < endPos) {
        uint8_t tag = buffer[bufPos++];
        int len;

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, endPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_ERROR, "PRES: Invalid PDU");
            return -1;
        }

        switch (tag) {
        case 0x02: /* presentation-context-identifier */
            contextId = BerDecoder_decodeUint32(buffer, len, bufPos);
            bufPos += len;
            break;
        case 0x06: /* abstract-syntax-name */

            if (len == 5) {
                if (memcmp(buffer + bufPos, asn_id_mms, 5) == 0)
                    isMms = true;
            }
            else if (len == 4) {
                if (memcmp(buffer + bufPos, asn_id_as_acse, 4) == 0)
                    isAcse = true;
            }
            else {
                libmms_iso_log(MMS_LOG_ERROR, "PRES: unknown abstract-syntax-name");
            }

            bufPos += len;

            break;
        case 0x30: /* transfer-syntax-name */

            bufPos += len;
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default:
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_WARNING, "PRES: unknown tag in presentation-context-definition-list-entry");
            bufPos += len;
            break;
        }
    }

    if (contextId < 0) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: ContextId not defined!");
        return -1;
    }

    if ((isAcse == false) && (isMms == false)) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: not an ACSE or MMS context definition");

        return -1;
    }

    if (isMms) {
        self->mmsContextId = (uint8_t) contextId;
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_DEBUG, "PRES: MMS context id is %i", contextId);
    }
    else {
        self->acseContextId = (uint8_t) contextId;
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_DEBUG, "PRES: ACSE context id is %i", contextId);
    }

    return bufPos;
}

static int
parsePresentationContextDefinitionList(IsoPresentation* self, uint8_t* buffer, int totalLength, int bufPos)
{
    int endPos = bufPos + totalLength;

    while (bufPos < endPos) {
        uint8_t tag = buffer[bufPos++];
        int len;

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, endPos);
        if (bufPos < 0)
            return -1;

        switch (tag) {
        case 0x30:
            bufPos = parsePCDLEntry(self, buffer, len, bufPos);
            if (bufPos < 0)
                return -1;
            break;
        case 0x00: /* indefinite length end tag -> ignore */
            break;
        default:
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_DEBUG, "PRES: unknown tag in presentation-context-definition-list");
            bufPos += len;
            break;
        }
    }

    return bufPos;
}

static int
parseNormalModeParameters(IsoPresentation* self, uint8_t* buffer, int totalLength, int bufPos)
{
    int endPos = bufPos + totalLength;

    self->calledPresentationSelector.size = 0;
    self->callingPresentationSelector.size = 0;

    while (bufPos < endPos) {
        uint8_t tag = buffer[bufPos++];
        int len;

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, endPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_ERROR, "PRES: wrong parameter length");
            return -1;
        }

        switch (tag) {
        case 0x81: /* calling-presentation-selector */

            if (len > 16) {
                if (DEBUG_PRES)
                    libmms_iso_log(MMS_LOG_ERROR, "PRES: calling-presentation-sel too large");
            }
            else {
                self->callingPresentationSelector.size = len;
                int i;
                for (i = 0; i < len; i++)
                    self->callingPresentationSelector.value[i] = buffer[bufPos + i];
            }

            bufPos += len;
            break;

        case 0x82: /* called-presentation-selector */

            if (len > 16) {
                if (DEBUG_PRES)
                    libmms_iso_log(MMS_LOG_ERROR, "PRES: called-presentation-sel too large");
            }
            else {
                self->calledPresentationSelector.size = len;
                int i;
                for (i = 0; i < len; i++)
                    self->calledPresentationSelector.value[i] = buffer[bufPos + i];
            }

            bufPos += len;
            break;

        case 0x83: /* responding-presentation-selector */

            if (len > 16) {
                if (DEBUG_PRES)
                    libmms_iso_log(MMS_LOG_WARNING, "PRES: responding-presentation-sel too large");
            }

            bufPos += len;
            break;

        case 0xa4: /* presentation-context-definition list */
            bufPos = parsePresentationContextDefinitionList(self, buffer, len, bufPos);
            break;

        case 0xa5: /* context-definition-result-list */

            bufPos += len;
            break;

        case 0x61: /* user data */
            bufPos = parseFullyEncodedData(self, buffer, len, bufPos);

            if (bufPos < 0) {
                libmms_iso_log(MMS_LOG_ERROR, "PRES: failed to parse user data");
                return -1;
            }

            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default:
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_WARNING, "PRES: unknown tag %02x in normal-mode", tag);
            bufPos += len;
            break;
        }
    }

    return bufPos;
}

int
IsoPresentation_parseAcceptMessage(IsoPresentation* self, ByteBuffer* byteBuffer)
{
    uint8_t* buffer = byteBuffer->buffer;
    int maxBufPos = byteBuffer->size;

    int bufPos = 0;

    uint8_t cpTag = buffer[bufPos++];

    if (cpTag != 0x31) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: not a CPA message");
        return 0;
    }

    int len;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: Invalid message");
        return 0;
    }

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_ERROR, "PRES: wrong parameter length");
            return 0;
        }

        switch (tag) {
        case 0xa0: /* mode-selector */
            bufPos += len; /* ignore content since only normal mode is allowed */
            break;
        case 0xa2: /* normal-mode-parameters */
            bufPos = parseNormalModeParameters(self, buffer, len, bufPos);

            if (bufPos < 0) {
                if (DEBUG_PRES)
                    libmms_iso_log(MMS_LOG_ERROR, "PRES: error parsing normal-mode-parameters");
                return 0;
            }

            break;
        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default:
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_WARNING, "PRES: CPA unknown tag %i", tag);
            bufPos += len;
            break;
        }
    }

    return 1;
}

void
IsoPresentation_initActive(IsoPresentation* self, IsoApplicationAddress localAddress, IsoApplicationAddress remoteAddress, IsoApplicationAddress peerAddress)
{
    self->localAddress = localAddress;
    self->remoteAddress = remoteAddress;
    self->peerAddress = peerAddress;
}

void
IsoPresentation_initPassive(IsoPresentation* self, IsoApplicationAddress localAddress, IsoApplicationAddress peerAddress)
{
    self->localAddress = localAddress;
    self->peerAddress = peerAddress;
}

void
IsoPresentation_createUserData(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    int bufPos = 0;
    uint8_t* buffer = writeBuffer->buffer;

    int payloadLength = payload->length;

    int userDataLengthFieldSize = BerEncoder_determineLengthSize(payloadLength);
    ;
    int pdvListLength = payloadLength + (userDataLengthFieldSize + 4);

    int pdvListLengthFieldSize = BerEncoder_determineLengthSize(pdvListLength);
    int presentationLength = pdvListLength + (pdvListLengthFieldSize + 1);

    bufPos = BerEncoder_encodeTL(0x61, presentationLength, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x30, pdvListLength, buffer, bufPos);

    buffer[bufPos++] = (uint8_t) 0x02;
    buffer[bufPos++] = (uint8_t) 0x01;
    buffer[bufPos++] = (uint8_t) self->mmsContextId; /* mms context id */

    bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payloadLength;
    writeBuffer->nextPart = payload;
}

void
IsoPresentation_createUserDataACSE(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    int bufPos = 0;
    uint8_t* buffer = writeBuffer->buffer;

    int payloadLength = payload->length;

    int userDataLengthFieldSize = BerEncoder_determineLengthSize(payloadLength);
    ;
    int pdvListLength = payloadLength + (userDataLengthFieldSize + 4);

    int pdvListLengthFieldSize = BerEncoder_determineLengthSize(pdvListLength);
    int presentationLength = pdvListLength + (pdvListLengthFieldSize + 1);

    bufPos = BerEncoder_encodeTL(0x61, presentationLength, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x30, pdvListLength, buffer, bufPos);

    buffer[bufPos++] = (uint8_t) 0x02;
    buffer[bufPos++] = (uint8_t) 0x01;
    buffer[bufPos++] = (uint8_t) self->acseContextId; /* ACSE context id */

    bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payloadLength;
    writeBuffer->nextPart = payload;
}

int
IsoPresentation_parseUserData(IsoPresentation* self, ByteBuffer* readBuffer)
{
    int length = readBuffer->size;
    uint8_t* buffer = readBuffer->buffer;

    int bufPos = 0;

    if (length < 9)
        return 0;

    if (buffer[bufPos++] != 0x61)
        return 0;

    int len;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, length);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: invalid message!");
        return 0;
    }

    if (buffer[bufPos++] != 0x30)
        return 0;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, length);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: invalid message!");
        return 0;
    }

    if (buffer[bufPos++] != 0x02)
        return 0;

    if (buffer[bufPos++] != 0x01)
        return 0;

    self->nextContextId = buffer[bufPos++];

    if (buffer[bufPos++] != 0xa0)
        return 0;

    int userDataLength;

    uint8_t lengthByte = buffer[bufPos];

    bufPos = BerDecoder_decodeLength(buffer, &userDataLength, bufPos, length);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: invalid message!");
        return 0;
    }

    if (lengthByte == 0x80) {
        /* remove end element from user data length when indefinite length encoded */
        userDataLength = userDataLength - 2;
    }

    ByteBuffer_wrap(&(self->nextPayload), buffer + bufPos, userDataLength, userDataLength);

    return 1;
}

int
IsoPresentation_parseConnect(IsoPresentation* self, ByteBuffer* byteBuffer)
{
    uint8_t* buffer = byteBuffer->buffer;
    int maxBufPos = byteBuffer->size;

    int bufPos = 0;

    uint8_t cpTag = buffer[bufPos++];

    if (cpTag != 0x31) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: not a CP type");
        return 0;
    }

    int len;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

    if (bufPos < 0) {
        if (DEBUG_PRES)
            libmms_iso_log(MMS_LOG_ERROR, "PRES: invalid message!");
        return 0;
    }

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

        if (bufPos < 0) {
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_ERROR, "PRES: invalid message!");
            return 0;
        }

        switch (tag) {
        case 0xa0: /* mode-selection */
            {
                if (buffer[bufPos++] != 0x80) {
                    if (DEBUG_PRES)
                        libmms_iso_log(MMS_LOG_ERROR, "PRES: mode-value of wrong type!");
                    return 0;
                }

                bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

                if (bufPos < 0) {
                    if (DEBUG_PRES)
                        libmms_iso_log(MMS_LOG_ERROR, "PRES: invalid message!");
                    return 0;
                }

                uint32_t modeSelector = BerDecoder_decodeUint32(buffer, len, bufPos);

                if (DEBUG_PRES)
                    libmms_iso_log(MMS_LOG_DEBUG, "PRES: modesel %u", modeSelector);

                bufPos += len;
            }
            break;
        case 0xa2: /* normal-mode-parameters */
            bufPos = parseNormalModeParameters(self, buffer, len, bufPos);

            if (bufPos < 0) {
                if (DEBUG_PRES)
                    libmms_iso_log(MMS_LOG_ERROR, "PRES: error parsing normal-mode-parameters");
                return 0;
            }

            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* unsupported element */
            if (DEBUG_PRES)
                libmms_iso_log(MMS_LOG_ERROR, "PRES: tag %02x not recognized", tag);
            bufPos += len;
            break;
        }
    }

    return 1;
}

void
IsoPresentation_createAbortUserMessage(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    int contentLength = 0;

    contentLength = +encodeUserData(NULL, 0, payload, false, self->acseContextId);

    contentLength += BerEncoder_determineLengthSize(contentLength) + 1;

    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0xa0, contentLength, buffer, bufPos);

    /* encode user data */
    bufPos = encodeUserData(buffer, bufPos, payload, true, self->acseContextId);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payload->length;
    writeBuffer->nextPart = payload;
}

/**
 * \brief Create CPA message (Connect Presentation Accept PPDU)
 */
void
IsoPresentation_createCpaMessage(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    int contentLength = 0;

    /* mode-selector */
    contentLength += 5;

    int normalModeLength = 0;

    if (self->localAddress->pSelector.size > 0) {
        int respondingPSelectorLength = 2 + (self->localAddress->pSelector.size);

        normalModeLength += respondingPSelectorLength; /* responding-presentation-selector */
    }

    normalModeLength += 20; /* context-definition-result-list */

    normalModeLength += encodeUserData(NULL, 0, payload, false, self->acseContextId);

    contentLength += normalModeLength;

    contentLength += BerEncoder_determineLengthSize(normalModeLength) + 1;

    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0x31, contentLength, buffer, bufPos);

    /* mode-selector */
    bufPos = BerEncoder_encodeTL(0xa0, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x80, 1, buffer, bufPos);
    buffer[bufPos++] = 1; /* 1 = normal-mode */

    /* normal-mode-parameters */
    bufPos = BerEncoder_encodeTL(0xa2, normalModeLength, buffer, bufPos);

    if (self->localAddress->pSelector.size > 0) {
        /* responding-presentation-selector */
        bufPos = BerEncoder_encodeTL(0x83, self->localAddress->pSelector.size, buffer, bufPos);
        memcpy(buffer + bufPos, self->localAddress->pSelector.value, self->localAddress->pSelector.size);
        bufPos += self->localAddress->pSelector.size;
    }

    /* context-definition-result-list */
    bufPos = BerEncoder_encodeTL(0xa5, 18, buffer, bufPos);
    bufPos = encodeAcceptBer(buffer, bufPos); /* accept for acse */
    bufPos = encodeAcceptBer(buffer, bufPos); /* accept for mms */

    /* encode user data */
    bufPos = encodeUserData(buffer, bufPos, payload, true, self->acseContextId);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payload->length;
    writeBuffer->nextPart = payload;
}
