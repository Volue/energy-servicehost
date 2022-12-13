/*
 *  mms_identify_service.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_server_internal.h"

#if MMS_IDENTIFY_SERVICE == 1

void
mmsServer_handleIdentifyRequest(
        uint32_t invokeId,
        ByteBuffer* response,
        const char* vendorName,
        const char* modelName,
        const char* revision)
{
    int bufPos = 0;
    uint8_t* buffer = response->buffer;

    uint32_t invokeIdLength = BerEncoder_UInt32determineEncodedSize(invokeId);

    uint32_t vendorNameLength = strlen(vendorName);
    uint32_t modelNameLength = strlen(modelName);
    uint32_t revisionLength = strlen(revision);

    uint32_t identityLength = 3 +  BerEncoder_determineLengthSize(vendorNameLength)
            + BerEncoder_determineLengthSize(modelNameLength) + BerEncoder_determineLengthSize(revisionLength)
            + vendorNameLength + modelNameLength + revisionLength;

    uint32_t identifyResponseLength = invokeIdLength + 2 + 1 + BerEncoder_determineLengthSize(identityLength)
            + identityLength;

    /* Identify response pdu */
    bufPos = BerEncoder_encodeTL(0xa1, identifyResponseLength, buffer, bufPos);

    /* invokeId */
    bufPos = BerEncoder_encodeTL(0x02, invokeIdLength, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0xa2, identityLength, buffer, bufPos);
    bufPos = BerEncoder_encodeStringWithTag(0x80, vendorName, buffer, bufPos);
    bufPos = BerEncoder_encodeStringWithTag(0x81, modelName, buffer, bufPos);
    bufPos = BerEncoder_encodeStringWithTag(0x82, revision, buffer, bufPos);

    response->size = bufPos;
}

#endif /* MMS_IDENTIFY_SERVICE == 1 */

