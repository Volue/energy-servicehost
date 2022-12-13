/*
 *  mms_client_initiate.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "mms_client.h"
#include "libmms_platform_includes.h"
#include "mms_common.h"
#include "mms_common_internal.h"
#include "mms_endpoint_internal.h"
#include "byte_buffer.h"

#include "stack_config.h"

#include "mms_client_internal.h"

#include "ber_encoder.h"

/* servicesSupported = {GetNameList} - required by initiate request, other services are required by some servers to work properly */
static uint8_t servicesSupported[] = { 0xee, 0x1c, 0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x79, 0xef, 0x18 };

void
mmsClient_createInitiateRequest(MmsEndpointConnection self, ByteBuffer* message)
{
    int maxServerOutstandingCalling = self->maxServOutstandingCalling;
    int maxServerOutstandingCalled = self->maxServOutstandingCalled;
    int dataStructureNestingLevel = DEFAULT_DATA_STRUCTURE_NESTING_LEVEL;

    uint32_t localDetailSize =
            BerEncoder_UInt32determineEncodedSize(self->maxPduSize);

    uint32_t proposedMaxServerOutstandingCallingSize =
            BerEncoder_UInt32determineEncodedSize(maxServerOutstandingCalling);

    uint32_t proposedMaxServerOutstandingCalledSize =
            BerEncoder_UInt32determineEncodedSize(maxServerOutstandingCalled);

    uint32_t dataStructureNestingLevelSize =
            BerEncoder_UInt32determineEncodedSize(dataStructureNestingLevel);

    uint32_t initRequestDetailSize = 3 + 5 + 14;

    uint32_t initiateRequestPduSize = 2 + localDetailSize +
                             2 + proposedMaxServerOutstandingCallingSize +
                             2 + proposedMaxServerOutstandingCalledSize +
                             2 + dataStructureNestingLevelSize +
                             2 + initRequestDetailSize;

    /* encode message (InitiateRequestPdu) */

    int bufPos = 0;
    uint8_t* buffer = message->buffer;

    bufPos = BerEncoder_encodeTL(0xa8, initiateRequestPduSize, buffer, bufPos);

    /* localDetail */
    bufPos = BerEncoder_encodeTL(0x80, localDetailSize, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(self->maxPduSize, buffer, bufPos);

    /* proposedMaxServerOutstandingCalling */
    bufPos = BerEncoder_encodeTL(0x81, proposedMaxServerOutstandingCallingSize, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(maxServerOutstandingCalling, buffer, bufPos);

    /* proposedMaxServerOutstandingCalled */
    bufPos = BerEncoder_encodeTL(0x82, proposedMaxServerOutstandingCalledSize, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(maxServerOutstandingCalled, buffer, bufPos);

    /* proposedDataStructureNestingLevel */
    bufPos = BerEncoder_encodeTL(0x83, dataStructureNestingLevelSize, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(dataStructureNestingLevel, buffer, bufPos);

    /* initRequestDetail */
    bufPos = BerEncoder_encodeTL(0xa4, initRequestDetailSize, buffer, bufPos);

    /* proposedVersionNumber = 1 */
    buffer[bufPos++] = 0x80;
    buffer[bufPos++] = 0x01;
    buffer[bufPos++] = 0x01;

    /* proposedParameterCBC */
    buffer[bufPos++] = 0x81;
    buffer[bufPos++] = 0x03;
    buffer[bufPos++] = 0x05; /* padding */
    buffer[bufPos++] = 0xf1;
    buffer[bufPos++] = 0x00;

    /* servicesSupportedCalling */
    bufPos = BerEncoder_encodeBitString(0x82, 85, servicesSupported, buffer, bufPos);

    message->size = bufPos;
}

int
mmsClient_createConcludeRequest(MmsConnection self, ByteBuffer* message)
{
    if (message->maxSize > 1) {
        message->buffer[0] = 0x8b;
        message->buffer[1] = 0;
        message->size = 2;
        return 2;
    }
    else
        return -1;
}

static bool
parseInitResponseDetail(MmsEndpointConnection self, uint8_t* buffer, int bufPos, int maxBufPos)
{
    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if (bufPos < 0)
            return false;

        switch (tag) {

        case 0x80: /* negotiated protocol version */
            break;

        case 0x81: /* parameter CBB */
            break;

        case 0x82: /* services supported called */
            {
                int i;

                for (i = 0; i < 11; i++)
                     self->servicesSupported[i] = buffer[bufPos + i];
            }
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default:
            break;
        }

        bufPos += length;
    }

    return true;
}

bool
mmsClient_parseInitiateResponse(MmsEndpointConnection self, ByteBuffer* response)
{
    int bufPos = 1; /* ignore tag - already checked */

    int maxBufPos = ByteBuffer_getSize(response);
    uint8_t* buffer = ByteBuffer_getBuffer(response);

    int length;
    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

    if (bufPos < 0)
        return false;

    if (bufPos + length > maxBufPos)
        return false;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if (bufPos < 0)
            return false;

        if (bufPos + length > maxBufPos)
            return false;

        switch (tag) {
        case 0x80: /* local-detail-calling */
            self->maxPduSize = BerDecoder_decodeUint32(buffer, length, bufPos);

            if (self->maxPduSize > CONFIG_MMS_MAXIMUM_PDU_SIZE)
                self->maxPduSize = CONFIG_MMS_MAXIMUM_PDU_SIZE;

            break;

        case 0x81:  /* proposed-max-serv-outstanding-calling */
            self->maxServOutstandingCalling = BerDecoder_decodeUint32(buffer, length, bufPos);

            if (self->maxServOutstandingCalling > DEFAULT_MAX_SERV_OUTSTANDING_CALLING)
                self->maxServOutstandingCalling = DEFAULT_MAX_SERV_OUTSTANDING_CALLING;

            break;

        case 0x82:  /* proposed-max-serv-outstanding-called */
            self->maxServOutstandingCalled = BerDecoder_decodeUint32(buffer, length, bufPos);

            if (self->maxServOutstandingCalled > DEFAULT_MAX_SERV_OUTSTANDING_CALLED)
                self->maxServOutstandingCalled = DEFAULT_MAX_SERV_OUTSTANDING_CALLED;

            break;
        case 0x83: /* proposed-data-structure-nesting-level */
            self->dataStructureNestingLevel = BerDecoder_decodeUint32(buffer, length, bufPos);
            break;

        case 0xa4: /* mms-init-request-detail */
            {
                if (parseInitResponseDetail(self, buffer, bufPos, bufPos + length) == false)
                    return false;
            }
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default:
            break; /* Ignore unknown tags */
        }

        bufPos += length;
    }


    return true;
}
