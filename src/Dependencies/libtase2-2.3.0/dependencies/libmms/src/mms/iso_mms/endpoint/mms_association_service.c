/*
 *  mms_association_service.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_endpoint_internal.h"
#include "ber_encoder.h"

/**********************************************************************************************
 * MMS Server Capabilities
 *********************************************************************************************/

#define MMS_SERVICE_STATUS 0x80
#define MMS_SERVICE_GET_NAME_LIST 0x40
#define MMS_SERVICE_IDENTIFY 0x20
#define MMS_SERVICE_RENAME 0x10
#define MMS_SERVICE_READ 0x08
#define MMS_SERVICE_WRITE 0x04
#define MMS_SERVICE_GET_VARIABLE_ACCESS_ATTRIBUTES 0x02
#define MMS_SERVICE_DEFINE_NAMED_VARIABLE 0x01

#define MMS_SERVICE_DEFINE_SCATTERED_ACCESS 0x80
#define MMS_SERVICE_GET_SCATTERED_ACCESS_ATTRIBUTES 0x40
#define MMS_SERVICE_DELETE_VARIABLE_ACCESS 0x20
#define MMS_SERVICE_DEFINE_NAMED_VARIABLE_LIST 0x10
#define MMS_SERVICE_GET_NAMED_VARIABLE_LIST_ATTRIBUTES 0x08
#define MMS_SERVICE_DELETE_NAMED_VARIABLE_LIST 0x04
#define MMS_SERVICE_DEFINE_NAMED_TYPE 0x02
#define MMS_SERVICE_GET_NAMED_TYPE_ATTRIBUTES 0x01

#define MMS_SERVICE_OBTAIN_FILE 0x02

#define MMS_SERVICE_READ_JOURNAL 0x40

#define MMS_SERVICE_FILE_OPEN 0x80
#define MMS_SERVICE_FILE_READ 0x40
#define MMS_SERVICE_FILE_CLOSE 0x20
#define MMS_SERVICE_FILE_RENAME 0x01
#define MMS_SERVICE_FILE_DELETE 0x08
#define MMS_SERVICE_FILE_DIRECTORY 0x04
#define MMS_SERVICE_UNSOLICITED_STATUS 0x02
#define MMS_SERVICE_INFORMATION_REPORT 0x01

#define MMS_SERVICE_CONCLUDE 0x10
#define MMS_SERVICE_CANCEL 0x08

/* servicesSupported MMS bitstring */
static uint8_t servicesSupported[] =
{
        0x00
#if (MMS_STATUS_SERVICE == 1)
        | MMS_SERVICE_STATUS
#endif
        | MMS_SERVICE_GET_NAME_LIST
#if (MMS_IDENTIFY_SERVICE == 1)
        | MMS_SERVICE_IDENTIFY
#endif
        | MMS_SERVICE_READ
        | MMS_SERVICE_WRITE
        | MMS_SERVICE_GET_VARIABLE_ACCESS_ATTRIBUTES
        ,
        0x00
        | MMS_SERVICE_DEFINE_NAMED_VARIABLE_LIST
        | MMS_SERVICE_DELETE_NAMED_VARIABLE_LIST
        | MMS_SERVICE_GET_NAMED_VARIABLE_LIST_ATTRIBUTES
        ,
        0x00,
        0x00,
        0x00,
        0x00
#if (MMS_OBTAIN_FILE_SERVICE == 1)
        | MMS_SERVICE_OBTAIN_FILE
#endif
        ,
        0x00,
        0x00,
        0x00
#if (MMS_JOURNAL_SERVICE == 1)
        | MMS_SERVICE_READ_JOURNAL
#endif
        ,
        0x00
#if (MMS_FILE_SERVICE == 1)
        | MMS_SERVICE_FILE_OPEN
        | MMS_SERVICE_FILE_READ
        | MMS_SERVICE_FILE_CLOSE
        | MMS_SERVICE_FILE_RENAME
        | MMS_SERVICE_FILE_DELETE
        | MMS_SERVICE_FILE_DIRECTORY
#endif
        | MMS_SERVICE_INFORMATION_REPORT
        ,
        0x00
        | MMS_SERVICE_CONCLUDE
        | MMS_SERVICE_CANCEL
};

/* negotiated parameter CBB */
static uint8_t parameterCBB[] =
{
        0xf1,
        0x00
};


/**********************************************************************************************
 * MMS Initiate Service
 *********************************************************************************************/

static int
encodeInitResponseDetail(uint8_t* buffer, int bufPos, bool encode)
{
    int initResponseDetailSize = 14 + 5 + 3;

    if (encode == false)
        return initResponseDetailSize + 2;

    bufPos = BerEncoder_encodeTL(0xa4, initResponseDetailSize, buffer, bufPos);

    bufPos = BerEncoder_encodeUInt32WithTL(0x80, 1, buffer, bufPos); /* negotiated protocol version */

    bufPos = BerEncoder_encodeBitString(0x81, 11, parameterCBB, buffer, bufPos);

    bufPos = BerEncoder_encodeBitString(0x82, 85, servicesSupported, buffer, bufPos);

    return bufPos;
}


static int
createInitiateResponse(MmsEndpointConnection self, ByteBuffer* writeBuffer)
{
    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    int initiateResponseLength = 0;

    initiateResponseLength += 2 + BerEncoder_UInt32determineEncodedSize(self->maxPduSize);
    initiateResponseLength += 2 + BerEncoder_UInt32determineEncodedSize(self->maxServOutstandingCalling);
    initiateResponseLength += 2 + BerEncoder_UInt32determineEncodedSize(self->maxServOutstandingCalled);
    initiateResponseLength += 2 + BerEncoder_UInt32determineEncodedSize(self->dataStructureNestingLevel);

    initiateResponseLength += encodeInitResponseDetail(NULL, 0, false);

    /* Initiate response pdu */
    bufPos = BerEncoder_encodeTL(0xa9, initiateResponseLength, buffer, bufPos);

    bufPos = BerEncoder_encodeUInt32WithTL(0x80, self->maxPduSize, buffer, bufPos);

    bufPos = BerEncoder_encodeUInt32WithTL(0x81, self->maxServOutstandingCalling, buffer, bufPos);

    bufPos = BerEncoder_encodeUInt32WithTL(0x82, self->maxServOutstandingCalled, buffer, bufPos);

    bufPos = BerEncoder_encodeUInt32WithTL(0x83, self->dataStructureNestingLevel, buffer, bufPos);

    bufPos = encodeInitResponseDetail(buffer, bufPos, true);

    writeBuffer->size = bufPos;

    return bufPos;
}

static bool
parseInitiateRequestPdu(MmsEndpointConnection self, uint8_t* buffer, int bufPos, int maxBufPos)
{
    self->maxPduSize = CONFIG_MMS_MAXIMUM_PDU_SIZE;

    self->dataStructureNestingLevel =
                    DEFAULT_DATA_STRUCTURE_NESTING_LEVEL;

    self->maxServOutstandingCalled = DEFAULT_MAX_SERV_OUTSTANDING_CALLED;

    self->maxServOutstandingCalling = DEFAULT_MAX_SERV_OUTSTANDING_CALLING;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if (bufPos < 0)  {
            /* TODO write initiate error PDU! */
            return false;
        }

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
            /* we ignore this */
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

void
mmsEndpointConnection_handleInitiateRequest (
        MmsEndpointConnection self,
        uint8_t* buffer, int bufPos, int maxBufPos,
        ByteBuffer* response)
{

    if (parseInitiateRequestPdu(self, buffer, bufPos, maxBufPos))
        createInitiateResponse(self, response);
    else {
        /* TODO send initiate error PDU */
    }

}

/**********************************************************************************************
 * MMS Conclude Service
 *********************************************************************************************/

void
mmsEndpointConnection_writeConcludeResponsePdu(ByteBuffer* response)
{
    ByteBuffer_appendByte(response, 0x8c);
    ByteBuffer_appendByte(response, 0x00);
}
