/*
 *  mms_status_service.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_server_internal.h"
#include "mms_endpoint_internal.h"

#if (MMS_STATUS_SERVICE == 1)

void
mmsServer_handleStatusRequest(
        MmsEndpointConnection connection,
        uint8_t* requestBuffer,
        int bufPos,
        uint32_t invokeId)
{
    /* check for extended derivation */
    bool extendedDerivation = BerDecoder_decodeBoolean(requestBuffer, bufPos);

    if (DEBUG_MMS_SERVER)
        libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: status (invoke-id: %u) - extended-derivation: %i", invokeId,
                extendedDerivation);

    uint32_t invokeIdLength = BerEncoder_UInt32determineEncodedSize(invokeId);

    MmsServer mmsServer = connection->serverConnection->server;

    /* Invoke user provided callback */
    if (mmsServer->statusRequestListener != NULL)
        mmsServer->statusRequestListener(mmsServer->statusRequestListenerParameter, mmsServer, connection, extendedDerivation);

    ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

    if (response) {
        bufPos = 0;
        uint8_t* buffer = response->buffer;

        uint32_t vmdPhysicalStatusLength = BerEncoder_UInt32determineEncodedSize((uint32_t) mmsServer->vmdPhysicalStatus);
        uint32_t vmdLogicalStatusLength = BerEncoder_UInt32determineEncodedSize((uint32_t) mmsServer->vmdLogicalStatus);

        uint32_t statusLength = 2 + vmdPhysicalStatusLength + 2 + vmdLogicalStatusLength;

        uint32_t statusResponseLength = invokeIdLength + 2 + 1 + BerEncoder_determineLengthSize(statusLength) + statusLength;

        /* Status response pdu */
        bufPos = BerEncoder_encodeTL(0xa1, statusResponseLength, buffer, bufPos);

        /* invokeId */
        bufPos = BerEncoder_encodeTL(0x02, invokeIdLength, buffer, bufPos);
        bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

        bufPos = BerEncoder_encodeTL(0xa0, statusLength, buffer, bufPos);
        bufPos = BerEncoder_encodeTL(0x80, vmdLogicalStatusLength, buffer, bufPos);
        bufPos = BerEncoder_encodeUInt32((uint32_t) mmsServer->vmdLogicalStatus, buffer, bufPos);
        bufPos = BerEncoder_encodeTL(0x81, vmdPhysicalStatusLength, buffer, bufPos);
        bufPos = BerEncoder_encodeUInt32((uint32_t) mmsServer->vmdPhysicalStatus, buffer, bufPos);

        response->size = bufPos;

        MmsEndpointConnection_sendMessage(connection, response);

        MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
    }
}

void
MmsServer_setVMDStatus(MmsServer self, int vmdLogicalStatus, int vmdPhysicalStatus)
{
    self->vmdLogicalStatus = vmdLogicalStatus;
    self->vmdPhysicalStatus = vmdPhysicalStatus;
}

int
MmsServer_getVMDLogicalStatus(MmsServer self)
{
    return self->vmdLogicalStatus;
}

int
MmsServer_getVMDPhysicalStatus(MmsServer self)
{
    return self->vmdPhysicalStatus;
}

void
MmsServer_setStatusRequestListener(MmsServer self, MmsStatusRequestListener listener, void* parameter)
{
    self->statusRequestListener = listener;
    self->statusRequestListenerParameter = parameter;
}

#endif /* MMS_STATUS_SERVICE == 1 */
