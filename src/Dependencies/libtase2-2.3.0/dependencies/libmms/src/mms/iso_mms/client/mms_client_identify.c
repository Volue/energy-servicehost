/*
 *  mms_client_identify.c
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
mmsClient_createIdentifyRequest(uint32_t invokeId, ByteBuffer* request)
{
    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize(invokeId);
    uint32_t confirmedRequestPduSize = 2 + 2 + invokeIdSize;

    int bufPos = 0;
    uint8_t* buffer = request->buffer;

    bufPos = BerEncoder_encodeTL(0xa0, confirmedRequestPduSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x02, invokeIdSize, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x82, 0, buffer, bufPos);

    request->size = bufPos;
}

bool
mmsClient_parseIdentifyResponse(MmsEndpointConnection self, ByteBuffer* response, uint32_t respBufPos, uint32_t invokeId, MmsConnection_IdentifyHandler handler, void* parameter)
{
    uint8_t* buffer = ByteBuffer_getBuffer(response);
    int maxBufPos = ByteBuffer_getSize(response);
    int length;
    int bufPos = (int) respBufPos;
    char* vendorName = NULL;
    char* modelName = NULL;
    char* revision = NULL;
    int endPos;

    uint8_t tag = buffer[bufPos++];
    if (tag != 0xa2)
        goto exit_error;

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
    if (bufPos < 0) goto exit_error;

    endPos = bufPos + length;

    if (endPos > maxBufPos) {
        if (DEBUG_MMS_CLIENT)
            libmms_log(MMS_LOG_ERROR, self, "MMS_CLIENT: identify-response - message too short");
        goto exit_error;
    }

    char vendorNameBuf[100];
    char modelNameBuf[100];
    char revisionBuf[100];

    while (bufPos < endPos) {
        tag = buffer[bufPos++];

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
        if (bufPos < 0) goto exit_error;

        switch (tag) {
        case 0x80: /* vendorName */
            vendorName = StringUtils_createStringFromBufferInBuffer(vendorNameBuf, buffer + bufPos, length);
            bufPos += length;
            break;
        case 0x81: /* modelName */
            modelName = StringUtils_createStringFromBufferInBuffer(modelNameBuf, buffer + bufPos, length);
            bufPos += length;
            break;
        case 0x82: /* revision */
            revision = StringUtils_createStringFromBufferInBuffer(revisionBuf, buffer + bufPos, length);
            bufPos += length;
            break;
        case 0x83: /* list of abstract syntaxes */
            bufPos += length;
            break;
        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* ignore unknown tags */
        	bufPos += length;
        	break;
        }
    }

    handler(invokeId, parameter, MMS_ERROR_NONE, vendorName, modelName, revision);

    return true;

exit_error:
    return false;
}

