/*
 *  mms_common_internal.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_COMMON_INTERNAL_H_
#define MMS_COMMON_INTERNAL_H_

#include "mms_value.h"
#include "MmsPdu.h"
#include "conversions.h"
#include "byte_buffer.h"
#include "mms_server.h"

#define MMS_MAX_LOG_MESSAGE_SIZE 512

#ifdef TASE2_DEBUG_OUTPUT

void
libmms_log(MmsLogLevel level, MmsEndpointConnection peer, const char* fmt, ...);

void
libmms_log2(MmsLogLevel level, MmsEndpoint endpoint, const char* fmt, ...);

void
libmms_iso_log(MmsLogLevel level, const char* fmt, ...);

#else
#define libmms_log(...) {do {} while(0);}
#define libmms_log2(...) {do {} while(0);}
#define libmms_iso_log(...) {do {} while(0);}
#endif

#define DEFAULT_MAX_SERV_OUTSTANDING_CALLING 5
#define DEFAULT_MAX_SERV_OUTSTANDING_CALLED 5
#define DEFAULT_DATA_STRUCTURE_NESTING_LEVEL 10

#if (MMS_FILE_SERVICE == 1)

#ifndef CONFIG_MMS_MAX_NUMBER_OF_OPEN_FILES_PER_CONNECTION
#define CONFIG_MMS_MAX_NUMBER_OF_OPEN_FILES_PER_CONNECTION 5
#endif

#include "hal_filesystem.h"

typedef struct {
    int32_t frsmId;
    uint32_t readPosition;
    uint32_t fileSize;
    FileHandle fileHandle;
} MmsFileReadStateMachine;

/* include for MmsFileReadHandler definition */
#include "mms_client.h"

LIBMMS_INTERNAL bool
mmsMsg_parseFileOpenResponse(uint8_t* buffer, int bufPos, int maxBufPos, int32_t* frsmId, uint32_t* fileSize, uint64_t* lastModified);

LIBMMS_INTERNAL bool
mmsMsg_parseFileReadResponse(uint8_t* buffer, int bufPos, int maxBufPos, bool* moreFollows, uint8_t** dataBuffer, int* dataLength);

LIBMMS_INTERNAL void
mmsMsg_createFileReadResponse(int maxPduSize, uint32_t invokeId, ByteBuffer* response,  MmsFileReadStateMachine* frsm);

LIBMMS_INTERNAL void
mmsMsg_createFileCloseResponse(uint32_t invokeId, ByteBuffer* response);

LIBMMS_INTERNAL void
mmsMsg_createFileOpenResponse(const char* basepath, uint32_t invokeId, ByteBuffer* response, char* fullPath, MmsFileReadStateMachine* frsm);

LIBMMS_INTERNAL void
mmsMsg_createExtendedFilename(const char* basepath, char* extendedFileName, char* fileName);

LIBMMS_INTERNAL FileHandle
mmsMsg_openFile(const char* basepath, char* fileName, bool readWrite);

#endif /* (MMS_FILE_SERVICE == 1) */

typedef struct sMmsServiceError
{
    int errorClass;
    int errorCode;
} MmsServiceError;


LIBMMS_INTERNAL void /* Confirmed service error (ServiceError) */
mmsMsg_createServiceErrorPdu(uint32_t invokeId, ByteBuffer* response, MmsError errorType);

LIBMMS_INTERNAL void
mmsMsg_createMmsRejectPdu(uint32_t* invokeId, int reason, ByteBuffer* response);

LIBMMS_INTERNAL int
mmsMsg_parseConfirmedErrorPDU(MmsEndpointConnection self, uint8_t* buffer, int bufPos, int maxBufPos, uint32_t* invokeId, MmsServiceError* serviceError);

LIBMMS_INTERNAL int
mmsMsg_parseRejectPDU(MmsEndpointConnection self, uint8_t* buffer, int bufPos, int maxBufPos, uint32_t* invokeId, int* rejectType, int* rejectReason);

LIBMMS_INTERNAL MmsValue*
mmsMsg_parseDataElement(Data_t* dataElement);

LIBMMS_INTERNAL Data_t*
mmsMsg_createBasicDataElement(MmsValue* value);

LIBMMS_INTERNAL AccessResult_t**
mmsMsg_createAccessResultsList(MmsPdu_t* mmsPdu, int resultsCount);

LIBMMS_INTERNAL char*
mmsMsg_createStringFromAsnIdentifier(Identifier_t identifier);

LIBMMS_INTERNAL void
mmsMsg_copyAsn1IdentifierToStringBuffer(Identifier_t identifier, char* buffer, int bufSize);

LIBMMS_INTERNAL void
mmsMsg_deleteAccessResultList(AccessResult_t** accessResult, int variableCount);

LIBMMS_INTERNAL MmsValue*
mmsMsg_parseListOfAccessResults(MmsEndpointConnection connection, AccessResult_t** accessResultList, int listSize, bool createArray);

#endif /* MMS_COMMON_INTERNAL */

