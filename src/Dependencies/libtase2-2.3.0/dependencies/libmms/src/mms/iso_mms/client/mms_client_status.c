/*
 *  mms_client_status.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "mms_client.h"
#include "libmms_platform_includes.h"
#include "stack_config.h"
#include "mms_common.h"
#include "byte_buffer.h"

#include "mms_client_internal.h"
#include "ber_encoder.h"
#include "ber_decode.h"

void
mmsClient_createStatusRequest(uint32_t invokeId, ByteBuffer* request, bool extendedDerivation)
{
    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize(invokeId);
    uint32_t confirmedRequestPduSize = 2 + 2 + 1 + invokeIdSize;

    int bufPos = 0;
    uint8_t* buffer = request->buffer;

    bufPos = BerEncoder_encodeTL(0xa0, confirmedRequestPduSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x02, invokeIdSize, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    bufPos = BerEncoder_encodeBoolean(0x80, extendedDerivation, buffer, bufPos);

    request->size = bufPos;
}

bool
mmsClient_parseStatusResponse(MmsEndpointConnection self, ByteBuffer* response, int bufPos, int* vmdLogicalStatus, int* vmdPhysicalStatus)
{
    uint8_t* buffer = ByteBuffer_getBuffer(response);
    int maxBufPos = ByteBuffer_getSize(response);
    int length;
    bool hasPhysicalStatus = false;
    bool hasLogicalStatus = false;
    int endPos;

    uint8_t tag = buffer[bufPos++];

    if (tag != 0xa0) {
        if (DEBUG_MMS_CLIENT)
            libmms_log(MMS_LOG_ERROR, self, "MMS_CLIENT: status-response: unknown tag %02x", tag);

        goto exit_error;
    }

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
    if (bufPos < 0) goto exit_error;

    endPos = bufPos + length;

    if (endPos > maxBufPos) {
        if (DEBUG_MMS_CLIENT)
            libmms_log(MMS_LOG_ERROR, self, "MMS_CLIENT: status-response: message too short");

        goto exit_error;
    }

    while (bufPos < endPos) {
        tag = buffer[bufPos++];
        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
        if (bufPos < 0) goto exit_error;

        switch (tag) {
        case 0x80: /* vmdLogicalStatus */
            if (vmdLogicalStatus != NULL)
                *vmdLogicalStatus = BerDecoder_decodeUint32(buffer, length, bufPos);

            hasLogicalStatus = true;
            bufPos += length;
            break;
        case 0x81: /* vmdPhysicalStatus */
            if (vmdPhysicalStatus != NULL)
                *vmdPhysicalStatus = BerDecoder_decodeUint32(buffer, length, bufPos);

            hasPhysicalStatus = true;
            bufPos += length;
            break;
        case 0x82: /* localDetail */
            bufPos += length;
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default:
            return false;
        }
    }

    if (hasPhysicalStatus && hasLogicalStatus)
        return true;

exit_error:
    return false;
}
