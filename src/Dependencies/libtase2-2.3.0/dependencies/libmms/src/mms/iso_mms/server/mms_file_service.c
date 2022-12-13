/*
 *  mms_file_service.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_server_internal.h"
#include "mms_client_internal.h"
#include "mms_endpoint_internal.h"

#if (MMS_FILE_SERVICE == 1)

#include "hal_filesystem.h"
#include "conversions.h"

#define CONFIG_MMS_FILE_SERVICE_MAX_FILENAME_LENGTH 256

static void
createNullResponseExtendedTag(uint32_t invokeId, ByteBuffer* response, uint8_t tag)
{
    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize(invokeId) + 2;

    uint32_t confirmedResponsePDUSize = invokeIdSize + 3 ;

    uint8_t* buffer = response->buffer;

    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0xa1, confirmedResponsePDUSize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    buffer[bufPos++] = 0x9f; /* Context specific / primitive / extended */
    bufPos = BerEncoder_encodeTL(tag, 0, buffer, bufPos);

    response->size = bufPos;
}

static MmsFileReadStateMachine*
getFreeFrsm(MmsEndpointConnection connection)
{
    int i;

    MmsFileReadStateMachine* freeFrsm = NULL;

    for (i = 0; i < CONFIG_MMS_MAX_NUMBER_OF_OPEN_FILES_PER_CONNECTION; i++) {
        if (connection->frsms[i].fileHandle == NULL) {
            freeFrsm = &(connection->frsms[i]);
            break;
        }
    }

    return freeFrsm;
}


static MmsFileReadStateMachine*
getFrsm(MmsEndpointConnection connection, int32_t frsmId)
{
    int i;

    MmsFileReadStateMachine* frsm = NULL;

    for (i = 0; i < CONFIG_MMS_MAX_NUMBER_OF_OPEN_FILES_PER_CONNECTION; i++) {
        if (connection->frsms[i].fileHandle != NULL) {
            if (connection->frsms[i].frsmId == frsmId) {
                frsm = &(connection->frsms[i]);
                break;
            }
        }
    }

    return frsm;
}

static int32_t
getNextFrsmId(MmsEndpointConnection connection)
{
    uint32_t nextFrsmId = connection->nextFrsmId;
    connection->nextFrsmId++;

    return nextFrsmId;
}

static int
encodeFileAttributes(uint8_t tag, uint32_t fileSize, char* gtString, uint8_t* buffer, int bufPos)
{
    uint32_t sizeOfFileSize = BerEncoder_UInt32determineEncodedSize(fileSize);

    uint32_t gtStringSize = strlen(gtString);

    uint32_t fileAttributesSize = 1 + BerEncoder_determineLengthSize(sizeOfFileSize) + sizeOfFileSize
            + 2 + gtStringSize;

    if (buffer == NULL) {
        return fileAttributesSize;
    }
    else {
        bufPos = BerEncoder_encodeTL(tag, fileAttributesSize, buffer, bufPos); /* file attributes */
        bufPos = BerEncoder_encodeTL(0x80, sizeOfFileSize, buffer, bufPos); /* file size */
        bufPos = BerEncoder_encodeUInt32(fileSize, buffer, bufPos);
        bufPos = BerEncoder_encodeOctetString(0x81, (uint8_t*) gtString, gtStringSize, buffer, bufPos);
        return bufPos;
    }
}

static bool
getFileInfo(const char* basepath, char* filename, uint32_t* fileSize, uint64_t* lastModificationTimestamp)
{
#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    char extendedFileName[512];
#else
    char extendedFileName[sizeof(CONFIG_VIRTUAL_FILESTORE_BASEPATH) + 256];
#endif

    mmsMsg_createExtendedFilename(basepath, extendedFileName, filename);

    return FileSystem_getFileInfo(extendedFileName, fileSize, lastModificationTimestamp);
}

static FileHandle
openFile(const char* basepath, char* fileName, bool readWrite)
{
#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    char extendedFileName[512];
#else
    char extendedFileName[sizeof(CONFIG_VIRTUAL_FILESTORE_BASEPATH) + 256];
#endif

    mmsMsg_createExtendedFilename(basepath, extendedFileName, fileName);

    return FileSystem_openFile(extendedFileName, readWrite);
}

static DirectoryHandle
openDirectory(const char* basepath, char* directoryName)
{
#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    char extendedFileName[512];
#else
    char extendedFileName[sizeof(CONFIG_VIRTUAL_FILESTORE_BASEPATH) + 256];
#endif

    mmsMsg_createExtendedFilename(basepath, extendedFileName, directoryName);

    return FileSystem_openDirectory(extendedFileName);
}

static bool
renameFile(const char* basepath, char* oldFilename, char* newFilename) {

#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    char extendedOldFileName[512];
    char extendedNewFileName[512];
#else
    char extendedOldFileName[sizeof(CONFIG_VIRTUAL_FILESTORE_BASEPATH) + 256];
    char extendedNewFileName[sizeof(CONFIG_VIRTUAL_FILESTORE_BASEPATH) + 256];
#endif

    mmsMsg_createExtendedFilename(basepath, extendedOldFileName, oldFilename);
    mmsMsg_createExtendedFilename(basepath, extendedNewFileName, newFilename);

    return FileSystem_renameFile(extendedOldFileName, extendedNewFileName);
}

static bool
deleteFile(const char* basepath, char* fileName) {
#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    char extendedFileName[512];
#else
    char extendedFileName[sizeof(CONFIG_VIRTUAL_FILESTORE_BASEPATH) + 256];
#endif

    mmsMsg_createExtendedFilename(basepath, extendedFileName, fileName);

    return FileSystem_deleteFile(extendedFileName);
}

static bool
mmsMsg_parseFileName(MmsEndpointConnection connection, char* filename, uint8_t* buffer, int* bufPos, int maxBufPos , uint32_t invokeId)
{
    if (*bufPos == maxBufPos)
        return false;

    uint8_t tag = buffer[(*bufPos)++];

    if (tag != 0x19) {
        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_INVALID_PDU, response);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

        return false;
    }

    int length;

    *bufPos = BerDecoder_decodeLength(buffer, &length, *bufPos, maxBufPos);

    if (*bufPos < 0)  {
          ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

          if (response) {
              mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_INVALID_PDU, response);

              MmsEndpointConnection_sendMessage(connection, response);

              MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
          }

          return false;
    }

    if (length > 255) {
          ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

          if (response) {
              mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT, response);

              MmsEndpointConnection_sendMessage(connection, response);

              MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
          }

          return false;
    }

    memcpy(filename, buffer + *bufPos, length);
    filename[length] = 0;
    *bufPos += length;

    /* Check if path contains invalid characters (prevent escaping the virtual filestore by using "..")
     * TODO this may be platform dependent. Also depending of the platform there might be other evil
     * characters.
     */
    if (strstr(filename, "..") != NULL) {
        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_FILE_FILENAME_SYNTAX_ERROR);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

        return false;
    }

    return true;
}

void
mmsMsg_createFileOpenResponse(const char* basepath, uint32_t invokeId, ByteBuffer* response,
        char* filename, MmsFileReadStateMachine* frsm)
{
    uint64_t msTime;

    getFileInfo(basepath, filename, &(frsm->fileSize), &msTime);

    char gtString[30];

    Conversions_msTimeToGeneralizedTime(msTime, (uint8_t*) gtString);

    uint32_t fileAttributesSize = encodeFileAttributes(0xa1, frsm->fileSize, gtString, NULL, 0) + 2;

    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize(invokeId) + 2;

    uint32_t frsmIdSize = BerEncoder_UInt32determineEncodedSize(frsm->frsmId) + 2;

    uint32_t openFileResponseSize = fileAttributesSize + frsmIdSize;

    uint32_t confirmedResponsePDUSize = invokeIdSize + 2 + BerEncoder_determineLengthSize(openFileResponseSize)
               + openFileResponseSize;

    uint8_t* buffer = response->buffer;

    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0xa1, confirmedResponsePDUSize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    buffer[bufPos++] = 0xbf;
    bufPos = BerEncoder_encodeTL(0x48, openFileResponseSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x80, frsmIdSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeInt32(frsm->frsmId, buffer, bufPos);
    bufPos = encodeFileAttributes(0xa1, frsm->fileSize, gtString, buffer, bufPos);

    response->size = bufPos;
}



void
MmsEndpointConnection_handleFileDeleteRequest(
    MmsEndpointConnection connection,
    uint8_t* buffer, int bufPos, int maxBufPos,
    uint32_t invokeId)
{
    if (buffer[bufPos++] != 0x19)
        goto exit_reject_invalid_pdu;

    int length;

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

    if (length > 255) {
        MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT);
        return;
    }

    char filename[256];

    memcpy(filename, buffer + bufPos, length);
    filename[length] = 0;

    if (DEBUG_MMS_SERVER)
        printf("MMS_SERVER: mms_file_service.c: Delete file (%s)\n", filename);

    if (connection->endpoint->fileAccessHandler != NULL) {
        MmsError access = connection->endpoint->fileAccessHandler(connection->endpoint->fileAccessHandlerParameter,
                            connection, MMS_FILE_ACCESS_TYPE_DELETE, filename, NULL);

        if (access != MMS_ERROR_NONE) {
            MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, access);
            return;
        }
    }

    if (!getFileInfo(MmsEndpoint_getFilestoreBasepath(connection->endpoint), filename, NULL, NULL)) {
        if (DEBUG_MMS_SERVER)
            printf("MMS_SERVER: mms_file_service.c:  File (%s) not found\n", filename);

        MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_FILE_FILE_NON_EXISTENT);
        return;
    }

    if (!deleteFile(MmsEndpoint_getFilestoreBasepath(connection->endpoint), filename)) {
        if (DEBUG_MMS_SERVER)
            printf("MMS_SERVER: mms_file_service.c:  Delete file (%s) failed\n", filename);

        MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_FILE_FILE_ACCESS_DENIED);
        return;
    }

    ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

    if (response) {
        createNullResponseExtendedTag(invokeId, response, 0x4c);

        MmsEndpointConnection_sendMessage(connection, response);

        MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
    }

    return;

exit_reject_invalid_pdu:
MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
}

void
MmsEndpointConnection_handleFileOpenRequest(
    MmsEndpointConnection connection,
    uint8_t* buffer, int bufPos, int maxBufPos,
    uint32_t invokeId)
{
    char filename[256];
    bool hasFileName = false;
    uint32_t filePosition = 0;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if (bufPos < 0) goto exit_reject_invalid_pdu;

        if (bufPos + length > maxBufPos) goto exit_reject_invalid_pdu;

        switch(tag) {
        case 0xa0: /* filename */

            if (!mmsMsg_parseFileName(connection, filename, buffer, &bufPos, bufPos + length, invokeId))
                return;

            hasFileName = true;

            break;

        case 0x81: /* initial position */
            filePosition = BerDecoder_decodeUint32(buffer, length, bufPos);
            bufPos += length;
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* unrecognized parameter */
            bufPos += length;
            goto exit_reject_invalid_pdu;
        }
    }

    if (hasFileName) {

        if (connection->endpoint->fileAccessHandler != NULL) {
            MmsError access = connection->endpoint->fileAccessHandler(connection->endpoint->fileAccessHandlerParameter,
                                connection, MMS_FILE_ACCESS_TYPE_OPEN, filename, NULL);

            if (access != MMS_ERROR_NONE) {
                MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, access);
                return;
            }
        }

        MmsFileReadStateMachine* frsm = getFreeFrsm(connection);

        if (frsm != NULL) {
            FileHandle fileHandle = openFile(MmsEndpoint_getFilestoreBasepath(connection->endpoint), filename, false);

            if (fileHandle != NULL) {
                frsm->fileHandle = fileHandle;
                frsm->readPosition = filePosition;
                frsm->frsmId = getNextFrsmId(connection);

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    mmsMsg_createFileOpenResponse(MmsEndpoint_getFilestoreBasepath(connection->endpoint),
                            invokeId, response, filename, frsm);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }

            }
            else
                MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_FILE_FILE_NON_EXISTENT);


        }
        else
            MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_RESOURCE_OTHER);
    }
    else
        goto exit_invalid_parameter;

    return;

exit_invalid_parameter:
    MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT);
    return;

exit_reject_invalid_pdu:
    MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
    return;
}

#if (MMS_OBTAIN_FILE_SERVICE == 1)


static void /* Confirmed service error (ServiceError) */
createServiceErrorObtainFileError(uint32_t invokeId, ByteBuffer* response,
        MmsError errorType, uint32_t value)
{
    uint8_t buffer[8];

    int size = BerEncoder_encodeUInt32WithTL(0x80, value, buffer, 0);

    mmsServer_createServiceErrorPduWithServiceSpecificInfo(invokeId, response, errorType,
            buffer, size);
}


static void
createObtainFileResponse(uint32_t invokeId, ByteBuffer* response)
{
    createNullResponseExtendedTag(invokeId, response, 0x2e);
}

void
MmsEndpoint_fileUploadTask(MmsEndpoint self, MmsObtainFileTask task)
{
    switch (task->state) {

        case MMS_FILE_UPLOAD_STATE_NOT_USED:
            break;

        case MMS_FILE_UPLOAD_STATE_FILE_OPEN_SENT:
            {
                if (Hal_getTimeInMs() > task->nextTimeout) {

                    if (DEBUG_MMS_SERVER)
                        printf("MMS_SERVER: file open timeout!\n");

                    task->state = MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_SOURCE;

                    FileSystem_closeFile(task->fileHandle);
                    deleteFile(MmsEndpoint_getFilestoreBasepath(task->connection->endpoint), task->destinationFilename);
                }
            }
            break;

        case MMS_FILE_UPLOAD_STATE_SEND_FILE_READ:
            {
                MmsEndpointConnection_lockConnection(task->connection);

                ByteBuffer* request = MmsEndpoint_reserveTransmitBuffer(self);

                task->lastRequestInvokeId = MmsEndpointConnection_getNextInvokeId(task->connection);

                mmsClient_createFileReadRequest(task->lastRequestInvokeId, request, task->frmsId);

                /* TODO replace by MmsEndpointConnection_sendAsyncRequest */
                MmsEndpointConnection_sendMessage(task->connection, request);

                MmsEndpoint_releaseTransmitBuffer(self);

                MmsEndpointConnection_unlockConnection(task->connection);

                task->nextTimeout = Hal_getTimeInMs() + 2000; /* timeout 2000 ms */

                task->state = MMS_FILE_UPLOAD_STATE_FILE_READ_SENT;
            }

            break;

        case MMS_FILE_UPLOAD_STATE_FILE_READ_SENT:

            if (Hal_getTimeInMs() > task->nextTimeout) {

                if (DEBUG_MMS_SERVER)
                    printf("MMS_SERVER: file read timeout!\n");

                task->state = MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_SOURCE;

                FileSystem_closeFile(task->fileHandle);
                deleteFile(MmsEndpoint_getFilestoreBasepath(task->connection->endpoint), task->destinationFilename);
            }

            break;

        case MMS_FILE_UPLOAD_STATE_SEND_FILE_CLOSE:
            {
                ByteBuffer* request = MmsEndpoint_reserveTransmitBuffer(self);

                task->lastRequestInvokeId = MmsEndpointConnection_getNextInvokeId(task->connection);

                mmsClient_createFileCloseRequest(task->lastRequestInvokeId, request, task->frmsId);

                /* TODO replace by MmsEndpointConnection_sendAsyncRequest */
                MmsEndpointConnection_sendMessage(task->connection, request);

                MmsEndpoint_releaseTransmitBuffer(self);

                task->nextTimeout = Hal_getTimeInMs() + 2000; /* timeout 2000 ms */

                task->state = MMS_FILE_UPLOAD_STATE_FILE_CLOSE_SENT;
            }
            break;

        case MMS_FILE_UPLOAD_STATE_FILE_CLOSE_SENT:

            if (Hal_getTimeInMs() > task->nextTimeout) {

                if (DEBUG_MMS_SERVER)
                    printf("MMS_SERVER: file close timeout!\n");

                task->state = MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_SOURCE;

                FileSystem_closeFile(task->fileHandle);
                deleteFile(MmsEndpoint_getFilestoreBasepath(task->connection->endpoint), task->destinationFilename);
            }

            break;

        case MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_SOURCE:

            {
                /* send ObtainFileError */

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(self);

                createServiceErrorObtainFileError(task->obtainFileRequestInvokeId, response, MMS_ERROR_FILE_FILE_NON_EXISTENT, 0);

                /* TODO replace by MmsEndpointConnection_sendAsyncRequest */
                MmsEndpointConnection_sendMessage(task->connection, response);

                MmsEndpoint_releaseTransmitBuffer(self);

                if (DEBUG_MMS_SERVER)
                    printf("MMS_SERVER: ObtainFile service: failed to open file from client\n");

                task->state = MMS_FILE_UPLOAD_STATE_NOT_USED;
            }
            break;

        case MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_DESTINATION:
            {
                /* send ObtainFileError */

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(self);

                createServiceErrorObtainFileError(task->obtainFileRequestInvokeId, response, MMS_ERROR_FILE_OTHER, 1);

                /* TODO replace by MmsEndpointConnection_sendAsyncRequest */
                MmsEndpointConnection_sendMessage(task->connection, response);

                MmsEndpoint_releaseTransmitBuffer(self);

                if (DEBUG_MMS_SERVER)
                    printf("MMS_SERVER: ObtainFile service: failed to create local file\n");

                task->state = MMS_FILE_UPLOAD_STATE_NOT_USED;
            }

            break;

        case MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_RESPONSE:
            {
                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(self);

                createObtainFileResponse(task->obtainFileRequestInvokeId, response);

                /* TODO replace by MmsEndpointConnection_sendAsyncRequest */
                MmsEndpointConnection_sendMessage(task->connection, response);

                MmsEndpoint_releaseTransmitBuffer(self);

                if (self->getFileCompleteHandler)
                    self->getFileCompleteHandler(self->getFileCompleteHandlerParameter, task->connection, task->destinationFilename);

                task->state = MMS_FILE_UPLOAD_STATE_NOT_USED;
            }
            break;
    }
}

void
MmsEndpointConnection_handleObtainFileRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId)
{
    char sourceFilename[256];
    bool hasSourceFileName = false;

    char destinationFilename[256];
    bool hasDestinationFilename = false;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if (bufPos < 0) goto exit_reject_invalid_pdu;

        if (bufPos + length > maxBufPos) goto exit_reject_invalid_pdu;

        switch(tag) {

        case 0xa1: /* source filename */

            if (!mmsMsg_parseFileName(connection, sourceFilename, buffer, &bufPos, bufPos + length, invokeId))
                return;

            hasSourceFileName = true;

            break;

        case 0xa2: /* destination filename */

            if (!mmsMsg_parseFileName(connection, destinationFilename, buffer, &bufPos, bufPos + length, invokeId))
                return;

            hasDestinationFilename = true;

            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* unrecognized parameter */
            bufPos += length;
            goto exit_reject_invalid_pdu;
        }
    }

    if (hasSourceFileName && hasDestinationFilename) {

        /* Call user to check if access is allowed */
        if (connection->endpoint->fileAccessHandler != NULL) {
            MmsError access = connection->endpoint->fileAccessHandler(connection->endpoint->fileAccessHandlerParameter,
                                connection, MMS_FILE_ACCESS_TYPE_OBTAIN, destinationFilename, sourceFilename);

            if (access != MMS_ERROR_NONE) {
                MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, access);
                return;
            }
        }

        /* call callback to check if access is allowed -- DEPRECATED */
        if (connection->endpoint->obtainFileHandler)
            if (connection->endpoint->obtainFileHandler(connection->endpoint->obtainFileHandlerParameter, connection, sourceFilename, destinationFilename) == false)
                goto exit_access_denied;

        /*  check if destination file already exists. If exists return error message */

#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
        char extendedFileName[512];
#else
        char extendedFileName[sizeof(CONFIG_VIRTUAL_FILESTORE_BASEPATH) + 256];
#endif
        mmsMsg_createExtendedFilename(MmsEndpoint_getFilestoreBasepath(connection->endpoint),
                extendedFileName, destinationFilename);

        if (FileSystem_getFileInfo(extendedFileName, NULL, NULL)) {
            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: obtainFile - file already exists on server\n");
            goto exit_file_already_exists;
        }

        if (DEBUG_MMS_SERVER)
            printf("MMS_SERVER: Start download file %s from client to local file %s...\n", sourceFilename, destinationFilename);

        MmsObtainFileTask task = MmsEndpoint_getObtainFileTask(connection->endpoint);

        if (task != NULL) {

            FileHandle fileHandle = openFile(MmsEndpoint_getFilestoreBasepath(connection->endpoint),
                    destinationFilename, true);

            if (fileHandle == NULL) {
                task->state = MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_DESTINATION;
            }
            else {
                /* send file open request */
                task->lastRequestInvokeId = MmsEndpointConnection_getNextInvokeId(connection);
                task->connection = connection;
                task->fileHandle = fileHandle;
                task->obtainFileRequestInvokeId = invokeId;

                strcpy(task->destinationFilename, destinationFilename);

                MmsEndpointConnection_lockConnection(connection);

                ByteBuffer* request = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                mmsClient_createFileOpenRequest(task->lastRequestInvokeId, request, sourceFilename, 0);

                /* TODO replace by MmsEndpointConnection_sendAsyncRequest */
                MmsEndpointConnection_sendMessage(connection, request);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);

                MmsEndpointConnection_lockConnection(connection);

                task->nextTimeout = Hal_getTimeInMs() + 2000; /* timeout 2000 ms */

                task->state = MMS_FILE_UPLOAD_STATE_FILE_OPEN_SENT;
            }
        }
        else
            goto exit_unavailable;
    }
    else
        goto exit_invalid_parameter;

    return;

exit_invalid_parameter:
    MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT);
    return;

exit_access_denied:

    {
        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            createServiceErrorObtainFileError(invokeId, response, MMS_ERROR_FILE_FILE_ACCESS_DENIED, 1);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }
    }

    return;

exit_file_already_exists:

    {
        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            createServiceErrorObtainFileError(invokeId, response, MMS_ERROR_FILE_DUPLICATE_FILENAME, 1);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }
    }

    return;

exit_unavailable:
    MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_ACCESS_TEMPORARILY_UNAVAILABLE);
    return;

exit_reject_invalid_pdu:
    MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
    return;
}

#endif /* (MMS_OBTAIN_FILE_SERVICE == 1) */

void
mmsMsg_createFileReadResponse(int maxPduSize, uint32_t invokeId,
        ByteBuffer* response, MmsFileReadStateMachine* frsm)
{
     /* determine remaining bytes in file */
     uint32_t bytesLeft = frsm->fileSize - frsm->readPosition;

     uint32_t fileChunkSize = 0;

     uint32_t maxFileChunkSize = maxPduSize - 20;

     uint32_t fileReadResponseSize = 1; /* for tag */

     bool moreFollows = true;

     if (bytesLeft > maxFileChunkSize) {
         fileChunkSize = maxFileChunkSize;
     }
     else {
         fileChunkSize = bytesLeft;
         moreFollows = false;
         fileReadResponseSize += 3; /* for moreFollows */
     }

     fileReadResponseSize += fileChunkSize;
     fileReadResponseSize += BerEncoder_determineLengthSize(fileChunkSize);

     frsm->readPosition += fileChunkSize;

     uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize(invokeId) + 2;

     uint32_t confirmedResponsePDUSize = invokeIdSize + 2 + BerEncoder_determineLengthSize(fileReadResponseSize)
                + fileReadResponseSize;

     uint8_t* buffer = response->buffer;

     int bufPos = 0;

     bufPos = BerEncoder_encodeTL(0xa1, confirmedResponsePDUSize, buffer, bufPos);

     bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
     bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

     buffer[bufPos++] = 0xbf;
     bufPos = BerEncoder_encodeTL(0x49, fileReadResponseSize, buffer, bufPos);

     bufPos = BerEncoder_encodeTL(0x80, fileChunkSize, buffer, bufPos);
     FileSystem_readFile(frsm->fileHandle, buffer + bufPos, fileChunkSize);
     bufPos += fileChunkSize;

     if (!moreFollows)
         bufPos = BerEncoder_encodeBoolean(0x81, false, buffer, bufPos);

     response->size = bufPos;
}

void
MmsEndpointConnection_handleFileReadRequest(
    MmsEndpointConnection connection,
    uint8_t* buffer, int bufPos, int maxBufPos,
    uint32_t invokeId)
{
    int32_t frsmId = BerDecoder_decodeInt32(buffer, maxBufPos - bufPos, bufPos);

    if (DEBUG_MMS_SERVER)
        printf("MMS_SERVER: mmsServer_handleFileReadRequest read request for frsmId: %i\n", frsmId);

    MmsFileReadStateMachine* frsm = getFrsm(connection, frsmId);

    if (frsm != NULL) {
        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createFileReadResponse(connection->maxPduSize, invokeId, response, frsm);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }
    }
    else
        MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_FILE_OTHER);
}

void
mmsMsg_createFileCloseResponse(uint32_t invokeId, ByteBuffer* response)
{
    createNullResponseExtendedTag(invokeId, response, 0x4a);
}

void
MmsEndpointConnection_handleFileCloseRequest(
    MmsEndpointConnection connection,
    uint8_t* buffer, int bufPos, int maxBufPos,
    uint32_t invokeId)
{
    int32_t frsmId = BerDecoder_decodeInt32(buffer, maxBufPos - bufPos, bufPos);

    MmsFileReadStateMachine* frsm = getFrsm(connection, frsmId);

    if (frsm != NULL) {
        FileSystem_closeFile(frsm->fileHandle);
        frsm->fileHandle = NULL;
        frsm->frsmId = 0;

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createFileCloseResponse(invokeId, response);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }
    }
    else
        MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_FILE_OTHER);
}



static int /* TODO remove redundancy - same as in client code! */
encodeFileSpecification(uint8_t tag, char* fileSpecification, uint8_t* buffer, int bufPos)
{
    uint32_t fileNameStringSize = strlen(fileSpecification);
    uint32_t fileNameSeqSize = 1 + BerEncoder_determineLengthSize(fileNameStringSize) + fileNameStringSize;

    if (buffer != NULL) {

        bufPos = BerEncoder_encodeTL(tag, fileNameSeqSize, buffer, bufPos);
        bufPos = BerEncoder_encodeOctetString(0x19, (uint8_t*) fileSpecification, fileNameStringSize, buffer, bufPos);

        return bufPos;
    }
    else {
        return fileNameSeqSize + 1 + BerEncoder_determineLengthSize(fileNameSeqSize);
    }
}

static int
addFileEntriesToResponse(const char* basepath, uint8_t* buffer, int bufPos, int maxBufSize, char* directoryName, char** continueAfterFileName, bool* moreFollows)
{
	int directoryNameLength = strlen(directoryName);

    DirectoryHandle directory = openDirectory(basepath, directoryName);

    if (directory != NULL) {

        bool isDirectory;
        char* fileName = FileSystem_readDirectory(directory, &isDirectory);

        while (fileName != NULL) {
        	directoryName[directoryNameLength] = 0;

        	if (directoryNameLength > 0) {
        		if (directoryName[directoryNameLength - 1] != '/')
        			strcat(directoryName, "/");
        	}

        	strcat(directoryName, fileName);

            bufPos = addFileEntriesToResponse(basepath, buffer, bufPos, maxBufSize, directoryName, continueAfterFileName, moreFollows);

            if (*moreFollows == true)
                break;

            fileName = FileSystem_readDirectory(directory, &isDirectory);
        }

        FileSystem_closeDirectory(directory);
    }
    else {

        if (*continueAfterFileName != NULL) {
            if (strcmp(*continueAfterFileName, directoryName) == 0) {
                *continueAfterFileName = NULL;
            }
        }
        else {
            uint64_t msTime;

            uint32_t fileSize;

            if (getFileInfo(basepath, directoryName, &fileSize, &msTime)) {
                char gtString[30];

                Conversions_msTimeToGeneralizedTime(msTime, (uint8_t*) gtString);

                int fileAttributesSize = encodeFileAttributes(0xa1, fileSize, gtString, NULL, 0);

                int filenameSize = encodeFileSpecification(0xa0, directoryName, NULL, 0);

                int dirEntrySize = 2 + fileAttributesSize + filenameSize;

                int overallEntrySize = 1 + BerEncoder_determineLengthSize(dirEntrySize) + dirEntrySize;

                int bufferSpaceLeft = maxBufSize - bufPos;

                if (overallEntrySize > bufferSpaceLeft) {
                    *moreFollows = true;
                }
                else {

                    bufPos = BerEncoder_encodeTL(0x30, dirEntrySize, buffer, bufPos); /* SEQUENCE (DirectoryEntry) */
                    bufPos = encodeFileSpecification(0xa0, directoryName, buffer, bufPos); /* fileName */
                    bufPos = encodeFileAttributes(0xa1, fileSize, gtString, buffer, bufPos); /* file attributes */
                }
            }
            else
                bufPos = -1;

        }

    }

    directoryName[directoryNameLength] = 0;

    return bufPos;
}

static void
createFileDirectoryResponse(const char* basepath, uint32_t invokeId, ByteBuffer* response, int maxPduSize, char* directoryName, char* continueAfterFileName)
{
    int maxSize = maxPduSize - 3; /* reserve space for moreFollows */
    uint8_t* buffer = response->buffer;

    bool moreFollows = false;

    int tempStartPos = 30; /* estimated header part with safety margin */
    int tempCurPos = tempStartPos;
    int tempEncoded = 0;

    if (continueAfterFileName != NULL) {
        if (strlen(continueAfterFileName) == 0)
            continueAfterFileName = NULL;
    }

    tempCurPos = addFileEntriesToResponse(basepath, buffer, tempCurPos, maxSize, directoryName, &continueAfterFileName, &moreFollows);

	if (tempCurPos < 0) {

       if (DEBUG_MMS_SERVER)
            printf("MMS_SERVER: Error opening directory!\n");

       mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_FILE_FILE_NON_EXISTENT);

       return;
    }

    tempEncoded = tempCurPos - tempStartPos;

    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize((uint32_t) invokeId) + 2;

    uint32_t listOfDirectoryEntrySeqSize = tempEncoded;

    uint32_t listOfDirectoryEntrySize = 1 + BerEncoder_determineLengthSize(tempEncoded) + tempEncoded;

    uint32_t fileDirectoryResponseSize = 1 + BerEncoder_determineLengthSize(listOfDirectoryEntrySize)
            + listOfDirectoryEntrySize;

    if (moreFollows)
        fileDirectoryResponseSize += 3; /* for moreFollows */

    uint32_t confirmedResponsePDUSize = invokeIdSize + 2 + BerEncoder_determineLengthSize(fileDirectoryResponseSize)
            + fileDirectoryResponseSize;

    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0xa1, confirmedResponsePDUSize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    buffer[bufPos++] = 0xbf;
    bufPos = BerEncoder_encodeTL(0x4d, fileDirectoryResponseSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0xa0, listOfDirectoryEntrySize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x30, listOfDirectoryEntrySeqSize, buffer, bufPos); /* SEQUENCE OF (DirectoryEntry) */

    /* memmove is required instead of memcpy since memory areas overlap */
    memmove(buffer + bufPos, buffer + tempStartPos, tempEncoded);

    bufPos += tempEncoded;

    if (moreFollows)
        bufPos = BerEncoder_encodeBoolean(0x81, moreFollows, buffer, bufPos);

    response->size = bufPos;
}

void
MmsEndpointConnection_handleFileRenameRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId)
{
    char currentFileName[256] = "";
    char newFileName[256] = "";

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if ((bufPos < 0) || (bufPos + length > maxBufPos)) {
            MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
            return;
        }

        switch(tag) {
        case 0xa0: /* currentFilename */
            if (!mmsMsg_parseFileName(connection, currentFileName, buffer, &bufPos, bufPos + length, invokeId))
                return;

            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: currentFileName: (%s)\n", currentFileName);

            break;

        case 0xa1: /* newFilename */
            if (!mmsMsg_parseFileName(connection, newFileName, buffer, &bufPos, bufPos + length, invokeId))
                return;

            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: newFileName: (%s)\n", newFileName);

            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* ignore unknown tag */
            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: unknown tag: (%02x)\n", tag);

            bufPos += length;
            break;
        }
    }

    if ((strlen(currentFileName) != 0) && (strlen(newFileName) != 0)) {

        /* Call user to check if access is allowed */
        if (connection->endpoint->fileAccessHandler != NULL) {
            MmsError access = connection->endpoint->fileAccessHandler(connection->endpoint->fileAccessHandlerParameter,
                                connection, MMS_FILE_ACCESS_TYPE_RENAME, currentFileName, newFileName);

            if (access != MMS_ERROR_NONE) {
                MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, access);
                return;
            }
        }

        if (renameFile(MmsEndpoint_getFilestoreBasepath(connection->endpoint), currentFileName, newFileName)){
            /* send positive response */

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                createNullResponseExtendedTag(invokeId, response, 0x4b);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
        }
        else
        {
            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: rename file failed!\n");

            MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_FILE_OTHER);
        }
    }
    else
        MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);

}

void
MmsEndpointConnection_handleFileDirectoryRequest(
    MmsEndpointConnection connection,
    uint8_t* buffer, int bufPos, int maxBufPos,
    uint32_t invokeId)
{
    if (DEBUG_MMS_SERVER)
        printf("MMS_SERVER: handleFileDirectoryRequest bufPos:%i, maxBufPus:%i\n", bufPos, maxBufPos);

    char filename[256] = "";

    char continueAfterFileName[256];

    char* continueAfter = NULL;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if ((bufPos < 0) || (bufPos + length > maxBufPos)) {

            MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);

            return;
        }

        switch(tag) {
        case 0xa0: /* filename */

            if (!mmsMsg_parseFileName(connection, filename, buffer, &bufPos, bufPos + length, invokeId))
                return;

            /* check for wildcard character(*) */
            if ((strcmp(filename, "*") == 0) || (strcmp(filename, "/") == 0) || (strcmp(filename, "\\") == 0))
            	filename[0] = 0;

            break;

        case 0xa1: /* continue-after */
            if (!mmsMsg_parseFileName(connection, continueAfterFileName, buffer, &bufPos, bufPos + length, invokeId))
                return;

            continueAfter = continueAfterFileName;

            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* unrecognized parameter */
            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: handleFileDirectoryRequest: unrecognized parameter\n");

            MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);

            return;
        }


    }

    int maxPduSize = connection->maxPduSize;

    /* Call user to check if access is allowed */
    if (connection->endpoint->fileAccessHandler != NULL) {
        MmsError access = connection->endpoint->fileAccessHandler(connection->endpoint->fileAccessHandlerParameter,
                            connection, MMS_FILE_ACCESS_TYPE_READ_DIRECTORY, filename, continueAfter);

        if (access != MMS_ERROR_NONE) {
            MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, access);
            return;
        }
    }

    ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

    if (response) {
        createFileDirectoryResponse(MmsEndpoint_getFilestoreBasepath(connection->endpoint),
                invokeId, response, maxPduSize, filename, continueAfter);

        MmsEndpointConnection_sendMessage(connection, response);

        MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
    }

}

#endif /* MMS_FILE_SERVICE == 1 */

