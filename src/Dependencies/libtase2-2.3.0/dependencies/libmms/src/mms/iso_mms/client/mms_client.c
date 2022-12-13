/*
 *  mms_client.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "mms_client.h"

#include "iso_client_connection.h"
#include "mms_client_internal.h"
#include "mms_endpoint_internal.h"
#include "stack_config.h"

#include <MmsPdu.h>
#include "libmms_platform_includes.h"

#include "byte_buffer.h"
#include "ber_decode.h"
#include "ber_encoder.h"

#include "tls_config.h"

bool
MmsClient_lockConnection(MmsClient self)
{
    Semaphore_wait(self->connectionLock);

    if (self->connection) {

        MmsEndpointConnection_lockConnection(self->connection);

        return true;
    }
    else {
        Semaphore_post(self->connectionLock);
        return false;
    }
}

void
MmsClient_unlockConnection(MmsClient self)
{
    MmsEndpointConnection_unlockConnection(self->connection);
    Semaphore_post(self->connectionLock);
}

MmsClient
MmsClient_create()
{
    MmsClient self = (MmsClient) GLOBAL_MALLOC(sizeof(struct sMmsClient));

    if (self) {
        self->endpoint = NULL;
        self->connection = NULL;
        self->connectionLock = Semaphore_create(1);

        /* Load default values for connection parameters */
        TSelector tSelector = { 2, { 0, 1 } };
        SSelector sSelector = { 2, { 0, 1 } };
        PSelector pSelector = { 2, { 0, 1 } };

        IsoApplicationAddress_setLowerLayerAddresses(&(self->peerAddress), pSelector, sSelector, tSelector);
        IsoApplicationAddress_setApTitle(&(self->peerAddress), "1.1.1.999.1");
        IsoApplicationAddress_setAeQualifier(&(self->peerAddress), 12);
    }

    return self;
}

void
MmsClient_setEndpointConnection(MmsClient self, MmsEndpointConnection connection)
{
    Semaphore_wait(self->connectionLock);
    self->connection = connection;
    Semaphore_post(self->connectionLock);
}

MmsEndpointConnection
MmsClient_getEndpointConnection(MmsClient self)
{
    return self->connection;
}

void
MmsClient_destroy(MmsClient self)
{
    if (self) {
        Semaphore_destroy(self->connectionLock);
    }

    GLOBAL_FREEMEM(self);
}

IsoApplicationAddress
MmsClient_getRemoteApplicationAddress(MmsClient self)
{
    return &(self->peerAddress);
}

void
MmsClient_setRemoteApplicationAddress(MmsClient self, IsoApplicationAddress address)
{
    memcpy(&(self->peerAddress), address, sizeof(struct sIsoApplicationAddress));
}

static MmsError
convertRejectCodesToMmsError(int rejectType, int rejectReason)
{
    if ((rejectType == 1) && (rejectReason == 1))
        return MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE;
    else if ((rejectType == 5) && (rejectReason == 0))
        return MMS_ERROR_REJECT_UNKNOWN_PDU_TYPE;
    else if ((rejectType == 1) && (rejectReason == 4))
        return MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT;
    else if ((rejectType == 5) && (rejectReason == 1))
        return MMS_ERROR_REJECT_INVALID_PDU;
    else
        return MMS_ERROR_REJECT_OTHER;
}

static int
parseServiceError(uint8_t* buffer, int bufPos, int maxLength, MmsServiceError* error)
{
    int endPos = bufPos + maxLength;
    int length;

    while (bufPos < endPos) {
        uint8_t tag = buffer[bufPos++];
        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, endPos);

        if (bufPos < 0)
            return -1;

        switch (tag)
        {
        case 0xa0: /* errorClass */
            {
                uint8_t errorClassTag = buffer[bufPos++];
                bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, endPos);

                if (bufPos < 0)
                    return -1;

                error->errorClass = errorClassTag - 0x80;
                error->errorCode = BerDecoder_decodeUint32(buffer, length, bufPos);

                bufPos += length;
            }
            break;
        case 0x81: /* additionalCode */
            bufPos += length; /* ignore */
            break;
        case 0x82: /* additionalDescription */
            bufPos += length; /* ignore */
            break;
        case 0xa3: /* serviceSpecificInfo */
            bufPos += length; /* ignore */
            break;
        case 0x00: /* indefinite length end tag -> ignore */
            break;
        default:
            bufPos += length; /* ignore */
            break;
        }
    }

    return bufPos;
}

int
mmsMsg_parseConfirmedErrorPDU(MmsEndpointConnection self, uint8_t* buffer, int bufPos, int maxBufPos, uint32_t* invokeId, MmsServiceError* serviceError)
{
    int length;
    int endPos;

    uint8_t tag = buffer[bufPos++];
    if (tag != 0xa2)
        goto exit_error;

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
    if (bufPos < 0)
        goto exit_error;

    endPos = bufPos + length;

    if (endPos > maxBufPos)
        goto exit_error;

    while (bufPos < endPos) {
        tag = buffer[bufPos++];

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if (bufPos < 0)
            goto exit_error;

        switch (tag)
        {
        case 0x80: /* invoke Id */
            if (invokeId != NULL)
                *invokeId = BerDecoder_decodeUint32(buffer, length, bufPos);
            bufPos += length;
            break;
        case 0x81: /* modifierPosition */
            bufPos += length; /* ignore */
            break;
        case 0xa2: /* serviceError */
            bufPos = parseServiceError(buffer, bufPos, length, serviceError);
            if (bufPos < 0)
                goto exit_error;
            break;
        case 0x00: /* indefinite length end tag -> ignore */
            break;
        default:
            bufPos += length; /* ignore */
            break;
        }
    }

    return bufPos;

exit_error:
    if (DEBUG_MMS_CLIENT)
        libmms_log(MMS_LOG_ERROR, self, "MMS_CLIENT: error parsing confirmed error PDU");

    return -1;
}

int
mmsMsg_parseRejectPDU(MmsEndpointConnection self, uint8_t* buffer, int bufPos, int maxBufPos, uint32_t* invokeId, int* rejectType, int* rejectReason)
{
    int length;
    int endPos;

    uint8_t tag = buffer[bufPos++];

    if (tag != 0xa4)
        goto exit_error;

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
    if (bufPos < 0)
        goto exit_error;

    if (bufPos + length > maxBufPos)
        goto exit_error;

    endPos = bufPos + length;

    if (endPos > maxBufPos)
        goto exit_error;

    while (bufPos < endPos) {
        tag = buffer[bufPos++];

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if (bufPos < 0)
            goto exit_error;

        if (tag == 0x80) { /* invoke id */
            if (invokeId != NULL)
                *invokeId = BerDecoder_decodeUint32(buffer, length, bufPos);
        }
        else if (tag > 0x80 && tag < 0x8c) {
            *rejectType = tag - 0x80;
            *rejectReason = BerDecoder_decodeInt32(buffer, length, bufPos);
        }
        else {
            /* unknown - ignore */
        }

        bufPos += length;
    }

    return bufPos;

exit_error:

    if (DEBUG_MMS_CLIENT)
        libmms_log(MMS_LOG_ERROR, self, "MMS_CLIENT: error parsing reject PDU");

    return -1;
}


#if 0
struct concludeParameters
{
    Semaphore sem;
    MmsError err;
    bool success;
};

static void
concludeHandler(void* parameter, MmsError mmsError, bool success)
{
    struct concludeParameters* parameters = (struct concludeParameters*) parameter;

    parameters->err = mmsError;
    parameters->success = success;

    /* unblock user thread */
    Semaphore_post(parameters->sem);
}

void
MmsConnection_conclude(MmsConnection self, MmsError* mmsError)
{
    MmsError err = MMS_ERROR_NONE;

    struct concludeParameters parameter;

    parameter.sem = Semaphore_create(1);;
    parameter.success = false;
    parameter.err = MMS_ERROR_NONE;

    Semaphore_wait(parameter.sem);

    MmsConnection_concludeAsync(self, &err, concludeHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.sem);
        err = parameter.err;
    }

    Semaphore_destroy(parameter.sem);

    if (mmsError)
        *mmsError = err;
}

void
MmsConnection_concludeAsync(MmsConnection self, MmsError* mmsError, MmsClient_ConcludeAbortHandler handler, void* parameter)
{
    if (getConnectionState(self) != MMS_CONNECTION_STATE_CONNECTED) {
        *mmsError = MMS_ERROR_CONNECTION_LOST;
        goto exit_function;
    }

    *mmsError = MMS_ERROR_NONE;

    ByteBuffer* concludeMessage = IsoClientConnection_allocateTransmitBuffer(self->isoClient);

    mmsClient_createConcludeRequest(self, concludeMessage);

    self->concludeHandler = handler;
    self->concludeHandlerParameter = parameter;
    self->concludeTimeout = Hal_getTimeInMs() + self->requestTimeout;

    IsoClientConnection_sendMessage(self->isoClient, concludeMessage);

exit_function:
    return;
}
#endif

MmsError
MmsClient_sendInformationReportListOfVariables(MmsClient self,
        LinkedList /* MmsVariableAccessSpecification */ variableAccessDeclarations,
        LinkedList /* MmsValue */ values)
{
    MmsError retVal = MMS_ERROR_NONE;

    if (self->connection)
        MmsEndpointConnection_sendInformationReportListOfVariables(self->connection, variableAccessDeclarations, values);
    else
        retVal = MMS_ERROR_CONNECTION_LOST;

    return retVal;
}

static uint32_t
mmsClient_getNameListSingleRequestAsync(
        MmsClient self,
        MmsError* mmsError,
        const char* domainId,
        MmsObjectClass objectClass,
        bool associationSpecific,
        const char* continueAfter,
        MmsClient_GetNameListHandler handler,
        void* parameter,
        LinkedList nameList)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        if (associationSpecific)
            mmsClient_createMmsGetNameListRequestAssociationSpecific(invokeId,
                    payload, continueAfter);
        else {

            if (objectClass == MMS_OBJECT_CLASS_DOMAIN)
                mmsClient_createMmsGetNameListRequestVMDspecific(invokeId,
                        payload, continueAfter);
            else
                mmsClient_createGetNameListRequestDomainOrVMDSpecific(invokeId, domainId,
                        payload, objectClass, continueAfter);
        }

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_GET_NAME_LIST, (void*) handler, parameter, nameList);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

struct getNameListParameters
{
    Semaphore sem;
    MmsError err;
    LinkedList nameList;
    bool moreFollows;
};

static void
getNameListHandler(uint32_t invokeId, void* parameter, MmsError mmsError, LinkedList nameList, bool moreFollows)
{
    struct getNameListParameters* parameters = (struct getNameListParameters*) parameter;

    parameters->err = mmsError;
    parameters->nameList = nameList;
    parameters->moreFollows = moreFollows;

    /* unblock user thread */
    Semaphore_post(parameters->sem);
}


static LinkedList /* <char*> */
mmsClient_getNameList(MmsClient self, MmsError *mmsError,
        const char* domainId,
        MmsObjectClass objectClass,
        bool associationSpecific)
{
    LinkedList list = NULL;
    bool moreFollows = false;

    struct getNameListParameters parameter;

    MmsError err;

    parameter.sem = Semaphore_create(1);;
    parameter.moreFollows = false;
    parameter.nameList = NULL;

    Semaphore_wait(parameter.sem);

    mmsClient_getNameListSingleRequestAsync(self, &err, domainId, objectClass, associationSpecific, NULL,
            getNameListHandler, &parameter, NULL);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.sem);
        err = parameter.err;
        list = parameter.nameList;
        moreFollows = parameter.moreFollows;
    }

    Semaphore_destroy(parameter.sem);

    while (moreFollows) {
        parameter.sem = Semaphore_create(1);

        char* continueAfter = NULL;

        if (list)
            continueAfter = (char*) LinkedList_getLastElement(list)->data;

        Semaphore_wait(parameter.sem);

        mmsClient_getNameListSingleRequestAsync(self, &err, domainId, objectClass, associationSpecific, continueAfter,
               getNameListHandler, &parameter, list);

        if (err == MMS_ERROR_NONE) {
            Semaphore_wait(parameter.sem);
            err = parameter.err;
            list = parameter.nameList;
            moreFollows = parameter.moreFollows;
        }

        Semaphore_destroy(parameter.sem);
    }

    if (mmsError)
        *mmsError = err;

    return list;
}

LinkedList /* <char*> */
MmsClient_getVMDVariableNames(MmsClient self, MmsError* mmsError)
{
    return mmsClient_getNameList(self, mmsError, NULL, MMS_OBJECT_CLASS_NAMED_VARIABLE, false);
}

uint32_t
MmsClient_getVMDVariableNamesAsync(MmsClient self, MmsError* mmsError, const char* continueAfter,
        MmsClient_GetNameListHandler handler, void* parameter)
{
    return mmsClient_getNameListSingleRequestAsync(self, mmsError, NULL, MMS_OBJECT_CLASS_NAMED_VARIABLE,
            false, continueAfter, handler, parameter, NULL);
}

LinkedList /* <char*> */
MmsClient_getDomainNames(MmsClient self, MmsError* mmsError)
{
    return mmsClient_getNameList(self, mmsError, NULL, MMS_OBJECT_CLASS_DOMAIN, false);
}

uint32_t
MmsClient_getDomainNamesAsync(MmsClient self, MmsError* mmsError, const char* continueAfter, LinkedList result,
        MmsClient_GetNameListHandler handler, void* parameter)
{
    return mmsClient_getNameListSingleRequestAsync(self, mmsError, NULL, MMS_OBJECT_CLASS_DOMAIN, false,
            continueAfter, handler, parameter, result);
}

LinkedList /* <char*> */
MmsClient_getDomainVariableNames(MmsClient self, MmsError* mmsError, const char* domainId)
{
    return mmsClient_getNameList(self, mmsError, domainId, MMS_OBJECT_CLASS_NAMED_VARIABLE, false);
}

uint32_t
MmsClient_getDomainVariableNamesAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* continueAfter, LinkedList result, MmsClient_GetNameListHandler handler, void* parameter)
{
    return mmsClient_getNameListSingleRequestAsync(self, mmsError, domainId, MMS_OBJECT_CLASS_NAMED_VARIABLE, false,
            continueAfter, handler, parameter, result);
}

LinkedList /* <char*> */
MmsClient_getDomainVariableListNames(MmsClient self, MmsError* mmsError, const char* domainId)
{
    return mmsClient_getNameList(self, mmsError, domainId, MMS_OBJECT_CLASS_NAMED_VARIABLE_LIST, false);
}

uint32_t
MmsClient_getDomainVariableListNamesAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* continueAfter, LinkedList result, MmsClient_GetNameListHandler handler, void* parameter)
{
    return mmsClient_getNameListSingleRequestAsync(self, mmsError, domainId, MMS_OBJECT_CLASS_NAMED_VARIABLE_LIST, false,
            continueAfter, handler, parameter, result);
}

LinkedList /* <char*> */
MmsClient_getDomainJournals(MmsClient self, MmsError* mmsError, const char* domainId)
{
    return mmsClient_getNameList(self, mmsError, domainId, MMS_OBJECT_CLASS_JOURNAL, false);
}

uint32_t
MmsClient_getDomainJournalsAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* continueAfter, MmsClient_GetNameListHandler handler, void* parameter)
{
    return mmsClient_getNameListSingleRequestAsync(self, mmsError, domainId, MMS_OBJECT_CLASS_JOURNAL, false,
            continueAfter, handler, parameter, NULL);
}

LinkedList /* <char*> */
MmsClient_getVariableListNamesAssociationSpecific(MmsClient self, MmsError* mmsError)
{
    return mmsClient_getNameList(self, mmsError, NULL, MMS_OBJECT_CLASS_NAMED_VARIABLE_LIST, true);
}

uint32_t
MmsClient_getVariableListNamesAssociationSpecificAsync(MmsClient self, MmsError* mmsError,
        const char* continueAfter, MmsClient_GetNameListHandler handler, void* parameter)
{
    return mmsClient_getNameListSingleRequestAsync(self, mmsError, NULL, MMS_OBJECT_CLASS_NAMED_VARIABLE_LIST, true,
            continueAfter, handler, parameter, NULL);
}


struct readNVParameters
{
    Semaphore sem;
    MmsError err;
    MmsValue* value;
};

static void
readVariableHandler(uint32_t invokeId, void* parameter, MmsError mmsError, MmsValue* value)
{
    struct readNVParameters* parameters = (struct readNVParameters*) parameter;

    parameters->err = mmsError;
    parameters->value = value;

    /* unblock user thread */
    Semaphore_post(parameters->sem);
}

uint32_t
MmsClient_readVariableAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId, bool specificationWithResult,
        MmsClient_ReadVariableHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createReadRequest(invokeId, domainId, itemId, specificationWithResult, payload);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_READ_VARIABLE, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

MmsValue*
MmsClient_readVariable(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, bool specificationWithResult)
{
    MmsValue* value = NULL;
    MmsError err = MMS_ERROR_NONE;

    struct readNVParameters parameter;

    parameter.sem = Semaphore_create(1);;
    parameter.value = NULL;
    parameter.err = MMS_ERROR_NONE;

    Semaphore_wait(parameter.sem);

    MmsClient_readVariableAsync(self, &err, domainId, itemId, specificationWithResult, readVariableHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.sem);

        value = parameter.value;
        err = parameter.err;
    }

    Semaphore_destroy(parameter.sem);

    if (mmsError)
        *mmsError = err;

    return value;
}

MmsValue*
MmsClient_readArrayElements(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId,
        uint32_t startIndex, uint32_t numberOfElements)
{
    MmsValue* value = NULL;
    MmsError err = MMS_ERROR_NONE;

    struct readNVParameters parameter;

    parameter.sem = Semaphore_create(1);;
    parameter.value = NULL;
    parameter.err = MMS_ERROR_NONE;

    Semaphore_wait(parameter.sem);

    MmsClient_readArrayElementsAsync(self, &err, domainId, itemId, startIndex, numberOfElements,
            readVariableHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.sem);

        value = parameter.value;
        err = parameter.err;
    }

    Semaphore_destroy(parameter.sem);

    if (mmsError)
        *mmsError = err;

    return value;
}

uint32_t
MmsClient_readArrayElementsAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        uint32_t startIndex, uint32_t numberOfElements,
        MmsClient_ReadVariableHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createReadRequestAlternateAccessIndex(invokeId, domainId, itemId, startIndex,
                numberOfElements, payload);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_READ_VARIABLE, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

MmsValue*
MmsClient_readMultipleVariables(MmsClient self, MmsError* mmsError,
        const char* domainId, LinkedList /*<char*>*/items, bool specificationWithResult)
{
    MmsValue* value = NULL;
    MmsError err = MMS_ERROR_NONE;

    struct readNVParameters parameter;

    parameter.sem = Semaphore_create(1);;
    parameter.value = NULL;
    parameter.err = MMS_ERROR_NONE;

    Semaphore_wait(parameter.sem);

    MmsClient_readMultipleVariablesAsync(self, &err, domainId, items, specificationWithResult,
            readVariableHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.sem);

        value = parameter.value;
        err = parameter.err;
    }

    Semaphore_destroy(parameter.sem);

    if (mmsError)
        *mmsError = err;

    return value;
}

uint32_t
MmsClient_readMultipleVariablesAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, LinkedList /*<char*>*/items, bool specificationWithResult,
        MmsClient_ReadVariableHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createReadRequestMultipleValues(invokeId, domainId, items, specificationWithResult, payload);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_READ_MULTIPLE_VARIABLES, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

MmsValue*
MmsClient_readNamedVariableListValues(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* listName,
        bool specWithResult)
{
    MmsValue* value = NULL;
    MmsError err = MMS_ERROR_NONE;

    struct readNVParameters parameter;

    parameter.sem = Semaphore_create(1);;
    parameter.value = NULL;
    parameter.err = MMS_ERROR_NONE;

    Semaphore_wait(parameter.sem);

    MmsClient_readNamedVariableListValuesAsync(self, &err, domainId, listName, specWithResult,
            readVariableHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.sem);

        value = parameter.value;
        err = parameter.err;
    }

    Semaphore_destroy(parameter.sem);

    if (mmsError)
        *mmsError = err;

    return value;
}

uint32_t
MmsClient_readNamedVariableListValuesAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* listName, bool specWithResult,
        MmsClient_ReadVariableHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createReadNamedVariableListRequest(invokeId, domainId, listName,
                payload, specWithResult);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_READ_MULTIPLE_VARIABLES, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

MmsValue*
MmsClient_readNamedVariableListValuesAssociationSpecific(
        MmsClient self, MmsError* mmsError,
        const char* listName,
        bool specWithResult)
{
    MmsValue* value = NULL;
    MmsError err = MMS_ERROR_NONE;

    struct readNVParameters parameter;

    parameter.sem = Semaphore_create(1);;
    parameter.value = NULL;
    parameter.err = MMS_ERROR_NONE;

    Semaphore_wait(parameter.sem);

    MmsClient_readNamedVariableListValuesAssociationSpecificAsync(self, &err, listName, specWithResult,
            readVariableHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.sem);

        value = parameter.value;
        err = parameter.err;
    }

    Semaphore_destroy(parameter.sem);

    if (mmsError)
        *mmsError = err;

    return value;
}

uint32_t
MmsClient_readNamedVariableListValuesAssociationSpecificAsync(MmsClient self, MmsError* mmsError,
        const char* listName, bool specWithResult,
        MmsClient_ReadVariableHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createReadAssociationSpecificNamedVariableListRequest(invokeId, listName,
                payload, specWithResult);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_READ_MULTIPLE_VARIABLES, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

struct readNVLDirectoryParameters
{
    Semaphore waitForResponse;
    MmsError err;
    LinkedList specs;
    bool deletable;
};

static void
readNVLDirectoryHandler(uint32_t invokeId, void* parameter, MmsError mmsError, LinkedList /* <MmsVariableAccessSpecification*> */ specs, bool deletable)
{
    struct readNVLDirectoryParameters* parameters = (struct readNVLDirectoryParameters*) parameter;

    parameters->err = mmsError;
    parameters->deletable = deletable;
    parameters->specs = specs;

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}

LinkedList /* <MmsVariableAccessSpecification*> */
MmsClient_readNamedVariableListDirectory(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* listName, bool* deletable)
{
    LinkedList specs = NULL;

    Semaphore waitForResponse = Semaphore_create(1);

    Semaphore_wait(waitForResponse);

    struct readNVLDirectoryParameters parameter;

    MmsError err;

    parameter.waitForResponse = waitForResponse;

    MmsClient_readNamedVariableListDirectoryAsync(self, &err, domainId, listName,
            readNVLDirectoryHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(waitForResponse);
        err = parameter.err;
        specs = parameter.specs;

        if (deletable)
            *deletable = parameter.deletable;
    }

    Semaphore_destroy(waitForResponse);

    if (mmsError)
        *mmsError = err;

    return specs;
}

uint32_t
MmsClient_readNamedVariableListDirectoryAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* listName,
        MmsConnection_ReadNVLDirectoryHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createGetNamedVariableListAttributesRequest(invokeId, payload, domainId,
                listName);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_READ_NVL_DIRECTORY, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

LinkedList /* <MmsVariableAccessSpecification*> */
MmsClient_readNamedVariableListDirectoryAssociationSpecific(MmsClient self, MmsError* mmsError,
        const char* listName, bool* deletable)
{
    LinkedList specs = NULL;

    Semaphore waitForResponse = Semaphore_create(1);

    Semaphore_wait(waitForResponse);

    struct readNVLDirectoryParameters parameter;

    MmsError err;

    parameter.waitForResponse = waitForResponse;

    MmsClient_readNamedVariableListDirectoryAssociationSpecificAsync(self, &err, listName,
            readNVLDirectoryHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(waitForResponse);
        err = parameter.err;
        specs = parameter.specs;

        if (deletable)
            *deletable = parameter.deletable;
    }

    Semaphore_destroy(waitForResponse);

    if (mmsError)
        *mmsError = err;

    return specs;
}

uint32_t
MmsClient_readNamedVariableListDirectoryAssociationSpecificAsync(MmsClient self, MmsError* mmsError,
        const char* listName,
        MmsConnection_ReadNVLDirectoryHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createGetNamedVariableListAttributesRequestAssociationSpecific(invokeId, payload,
                listName);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_READ_NVL_DIRECTORY, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

struct defineNVLParameters
{
    Semaphore waitForResponse;
    MmsError err;
    bool success;
};

static void
defineNVLHandler(uint32_t invokeId, void* parameter, MmsError mmsError, bool success)
{
    struct defineNVLParameters* parameters = (struct defineNVLParameters*) parameter;

    parameters->err = mmsError;
    parameters->success = success;

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}


void
MmsClient_defineNamedVariableList(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* listName, LinkedList variableSpecs)
{
    struct defineNVLParameters parameter;

    MmsError err;

    parameter.waitForResponse = Semaphore_create(1);

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_defineNamedVariableListAsync(self, &err, domainId, listName, variableSpecs,
            defineNVLHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);
        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;
}

uint32_t
MmsClient_defineNamedVariableListAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* listName, LinkedList variableSpecs,
        MmsClient_GenericServiceHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createDefineNamedVariableListRequest(invokeId, payload, domainId,
                listName, variableSpecs, false);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_DEFINE_NVL, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

void
MmsClient_defineNamedVariableListAssociationSpecific(MmsClient self,
        MmsError* mmsError, const char* listName, LinkedList variableSpecs)
{
    struct defineNVLParameters parameter;

    MmsError err;

    parameter.waitForResponse = Semaphore_create(1);

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_defineNamedVariableListAssociationSpecificAsync(self, &err, listName, variableSpecs,
            defineNVLHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);
        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;
}

uint32_t
MmsClient_defineNamedVariableListAssociationSpecificAsync(MmsClient self, MmsError* mmsError,
        const char* listName, LinkedList variableSpecs,
        MmsClient_GenericServiceHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createDefineNamedVariableListRequest(invokeId, payload, NULL,
                listName, variableSpecs, true);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_DEFINE_NVL, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

bool
MmsClient_deleteNamedVariableList(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* listName)
{
    bool isDeleted = false;

    struct defineNVLParameters parameter;

    MmsError err;

    parameter.waitForResponse = Semaphore_create(1);

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_deleteNamedVariableListAsync(self, &err, domainId, listName, defineNVLHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);
        err = parameter.err;
        isDeleted = parameter.success;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return isDeleted;
}

uint32_t
MmsClient_deleteNamedVariableListAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* listName,
        MmsClient_GenericServiceHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createDeleteNamedVariableListRequest(invokeId, payload, domainId, listName);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_DELETE_NVL, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

bool
MmsClient_deleteAssociationSpecificNamedVariableList(MmsClient self,
        MmsError* mmsError, const char* listName)
{
    bool isDeleted = false;

    struct defineNVLParameters parameter;

    MmsError err;

    parameter.waitForResponse = Semaphore_create(1);

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_deleteAssociationSpecificNamedVariableListAsync(self, &err, listName, defineNVLHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);
        err = parameter.err;
        isDeleted = parameter.success;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return isDeleted;
}

uint32_t
MmsClient_deleteAssociationSpecificNamedVariableListAsync(MmsClient self, MmsError* mmsError, const char* listName,
        MmsClient_GenericServiceHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createDeleteAssociationSpecificNamedVariableListRequest(
                invokeId, payload, listName);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_DELETE_NVL, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

struct getVarAccessAttrParameters
{
    Semaphore waitForResponse;
    MmsError err;
    MmsVariableSpecification* typeSpec;
};

static void
getAccessAttrHandler(uint32_t invokeId, void* parameter, MmsError mmsError, MmsVariableSpecification* typeSpec)
{
    struct getVarAccessAttrParameters* parameters = (struct getVarAccessAttrParameters*) parameter;

    parameters->err = mmsError;
    parameters->typeSpec = typeSpec;

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}

MmsVariableSpecification*
MmsClient_getVariableAccessAttributes(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId)
{
    MmsVariableSpecification* typeSpec = NULL;

    struct getVarAccessAttrParameters parameter;

    MmsError err;

    parameter.waitForResponse = Semaphore_create(1);

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_getVariableAccessAttributesAsync(self, &err, domainId, itemId, getAccessAttrHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);
        err = parameter.err;
        typeSpec = parameter.typeSpec;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return typeSpec;
}

uint32_t
MmsClient_getVariableAccessAttributesAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId,
        MmsConnection_GetVariableAccessAttributesHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createGetVariableAccessAttributesRequest(invokeId, domainId, itemId, payload);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_GET_VAR_ACCESS_ATTR, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

struct identifyParameters
{
    Semaphore waitForResponse;
    MmsError err;
    MmsServerIdentity* identify;
};

static void
identifyHandler(uint32_t invokeId, void* parameter, MmsError mmsError, char* vendorName, char* modelName, char* revision)
{
    struct identifyParameters* parameters = (struct identifyParameters*) parameter;

    parameters->err = mmsError;

    if (mmsError == MMS_ERROR_NONE) {
        parameters->identify = (MmsServerIdentity*)GLOBAL_MALLOC(sizeof(MmsServerIdentity));
        parameters->identify->vendorName = StringUtils_copyString(vendorName);
        parameters->identify->modelName = StringUtils_copyString(modelName);
        parameters->identify->revision = StringUtils_copyString(revision);
    }

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}


MmsServerIdentity*
MmsClient_identify(MmsClient self, MmsError* mmsError)
{
    MmsServerIdentity* identity = NULL;

    struct identifyParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.identify = NULL;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_identifyAsync(self, &err, identifyHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);
        err = parameter.err;
        identity = parameter.identify;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return identity;
}

uint32_t
MmsClient_identifyAsync(MmsClient self, MmsError* mmsError,
        MmsConnection_IdentifyHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createIdentifyRequest(invokeId, payload);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_IDENTIFY, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

struct getServerStatusParameters
{
    Semaphore waitForResponse;
    MmsError err;
    int vmdLogicalStatus;
    int vmdPhysicalStatus;
};

static void
getServerStatusHandler(uint32_t invokeId, void* parameter, MmsError mmsError, int vmdLogicalStatus, int vmdPhysicalStatus)
{
    struct getServerStatusParameters* parameters = (struct getServerStatusParameters*) parameter;

    parameters->err = mmsError;
    parameters->vmdLogicalStatus = vmdLogicalStatus;
    parameters->vmdPhysicalStatus = vmdPhysicalStatus;

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}


void
MmsClient_getServerStatus(MmsClient self, MmsError* mmsError, int* vmdLogicalStatus, int* vmdPhysicalStatus,
        bool extendedDerivation)
{
    struct getServerStatusParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.vmdLogicalStatus = 0;
    parameter.vmdPhysicalStatus = 0;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_getServerStatusAsync(self, &err, extendedDerivation, getServerStatusHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);
        err = parameter.err;

        if (vmdLogicalStatus)
            *vmdLogicalStatus = parameter.vmdLogicalStatus;

        if (vmdPhysicalStatus)
            *vmdPhysicalStatus = parameter.vmdPhysicalStatus;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;
}

uint32_t
MmsClient_getServerStatusAsync(MmsClient self, MmsError* mmsError, bool extendedDerivation,
        MmsConnection_GetServerStatusHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createStatusRequest(invokeId, payload, extendedDerivation);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_GET_SERVER_STATUS, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

static void
MmsJournalVariable_destroy(MmsJournalVariable self)
{
    if (self != NULL) {
        GLOBAL_FREEMEM(self->tag);
        MmsValue_delete(self->value);
        GLOBAL_FREEMEM(self);
    }
}

void
MmsJournalEntry_destroy(MmsJournalEntry self)
{
    if (self != NULL) {
        MmsValue_delete(self->entryID);
        MmsValue_delete(self->occurenceTime);
        LinkedList_destroyDeep(self->journalVariables,
                (LinkedListValueDeleteFunction) MmsJournalVariable_destroy);
        GLOBAL_FREEMEM(self);
    }
}

const MmsValue*
MmsJournalEntry_getEntryID(MmsJournalEntry self)
{
    return self->entryID;
}

const MmsValue*
MmsJournalEntry_getOccurenceTime(MmsJournalEntry self)
{
    return self->occurenceTime;
}

const LinkedList /* <MmsJournalVariable> */
MmsJournalEntry_getJournalVariables(MmsJournalEntry self)
{
    return self->journalVariables;
}

const char*
MmsJournalVariable_getTag(MmsJournalVariable self)
{
    return self->tag;
}

const MmsValue*
MmsJournalVariable_getValue(MmsJournalVariable self)
{
    return self->value;
}

struct readJournalParameters
{
    Semaphore waitForResponse;
    MmsError err;
    LinkedList entries;
    bool moreFollows;
};

static void
readJournalHandler(uint32_t invokeId, void* parameter, MmsError mmsError, LinkedList entries, bool moreFollows)
{
    struct readJournalParameters* parameters = (struct readJournalParameters*) parameter;

    parameters->err = mmsError;
    parameters->entries = entries;
    parameters->moreFollows = moreFollows;

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}


LinkedList
MmsClient_readJournalTimeRange(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        MmsValue* startTime, MmsValue* endTime, bool* moreFollows)
{
    struct readJournalParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.entries = NULL;
    parameter.moreFollows = false;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_readJournalTimeRangeAsync(self, &err, domainId, itemId, startTime, endTime, readJournalHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;

        if (moreFollows)
            *moreFollows = parameter.moreFollows;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return parameter.entries;
}

uint32_t
MmsClient_readJournalTimeRangeAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        MmsValue* startTime, MmsValue* endTime, MmsClient_ReadJournalHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if ((MmsValue_getType(startTime) != MMS_BINARY_TIME) ||
            (MmsValue_getType(endTime) != MMS_BINARY_TIME)) {

        *mmsError = MMS_ERROR_INVALID_ARGUMENTS;
        goto exit_function;
    }

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createReadJournalRequestWithTimeRange(invokeId, payload, domainId, itemId, startTime, endTime);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_READ_JOURNAL, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

exit_function:
    return invokeId;
}

LinkedList
MmsClient_readJournalStartAfter(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        MmsValue* timeSpecification, MmsValue* entrySpecification, bool* moreFollows)
{
    struct readJournalParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.entries = NULL;
    parameter.moreFollows = false;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_readJournalStartAfterAsync(self, &err, domainId, itemId, timeSpecification, entrySpecification, readJournalHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;

        if (moreFollows)
            *moreFollows = parameter.moreFollows;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return parameter.entries;
}

uint32_t
MmsClient_readJournalStartAfterAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        MmsValue* timeSpecification, MmsValue* entrySpecification, MmsClient_ReadJournalHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if ((MmsValue_getType(timeSpecification) != MMS_BINARY_TIME) ||
            (MmsValue_getType(entrySpecification) != MMS_OCTET_STRING)) {

        *mmsError = MMS_ERROR_INVALID_ARGUMENTS;
        goto exit_function;
    }

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createReadJournalRequestStartAfter(invokeId, payload, domainId, itemId, timeSpecification, entrySpecification);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_READ_JOURNAL, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

exit_function:
    return invokeId;
}

struct fileOpenParameters
{
    Semaphore waitForResponse;
    MmsError err;
    int32_t frsmId;
    uint32_t fileSize;
    uint64_t lastModified;
};

static void
fileOpenHandler(uint32_t invokeId, void* parameter, MmsError mmsError, int32_t frsmId, uint32_t fileSize, uint64_t lastModified)
{
    struct fileOpenParameters* parameters = (struct fileOpenParameters*) parameter;

    parameters->err = mmsError;
    parameters->frsmId = frsmId;
    parameters->fileSize = fileSize;
    parameters->lastModified = lastModified;

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}


int32_t
MmsClient_fileOpen(MmsClient self, MmsError* mmsError, const char* filename, uint32_t initialPosition,
        uint32_t* fileSize, uint64_t* lastModified)
{
#if (MMS_FILE_SERVICE == 1)

    struct fileOpenParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.frsmId = 0;
    parameter.fileSize = 0;
    parameter.lastModified = 0;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_fileOpenAsync(self, &err, filename, initialPosition, fileOpenHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (fileSize)
        *fileSize = parameter.fileSize;

    if (lastModified)
        *lastModified = parameter.lastModified;

    if (mmsError)
        *mmsError = err;

    return parameter.frsmId;
#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-open - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return 0;
#endif
}

uint32_t
MmsClient_fileOpenAsync(MmsClient self, MmsError* mmsError, const char* filename, uint32_t initialPosition, MmsConnection_FileOpenHandler handler,
        void* parameter)
{
#if (MMS_FILE_SERVICE == 1)
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createFileOpenRequest(invokeId, payload, filename, initialPosition);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_FILE_OPEN, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-open - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return 0;
#endif
}

struct fileOperationParameters
{
    Semaphore waitForResponse;
    MmsError err;
    bool success;
};

static void
fileOperationHandler(uint32_t invokeId, void* parameter, MmsError mmsError, bool success)
{
    struct fileOperationParameters* parameters = (struct fileOperationParameters*) parameter;

    parameters->err = mmsError;
    parameters->success = success;

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}

void
MmsClient_fileClose(MmsClient self, MmsError* mmsError, int32_t frsmId)
{
#if (MMS_FILE_SERVICE == 1)

    struct fileOperationParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.success = false;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_fileCloseAsync(self, &err, frsmId, fileOperationHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

#else

    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-close - service not supported");

    *mmsError = MMS_ERROR_OTHER;
#endif
}

uint32_t
MmsClient_fileCloseAsync(MmsClient self, MmsError* mmsError, uint32_t frsmId, MmsClient_GenericServiceHandler handler, void* parameter)
{
#if (MMS_FILE_SERVICE == 1)
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createFileCloseRequest(invokeId, payload, frsmId);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_FILE_CLOSE, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
#else

    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-close - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return 0;
#endif
}

void
MmsClient_fileDelete(MmsClient self, MmsError* mmsError, const char* fileName)
{
#if (MMS_FILE_SERVICE == 1)

    struct fileOperationParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.success = false;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_fileDeleteAsync(self, &err, fileName, fileOperationHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-delete - service not supported");

    *mmsError = MMS_ERROR_OTHER;
#endif
}

uint32_t
MmsClient_fileDeleteAsync(MmsClient self, MmsError* mmsError, const char* fileName,
        MmsClient_GenericServiceHandler handler, void* parameter)
{
#if (MMS_FILE_SERVICE == 1)
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createFileDeleteRequest(invokeId, payload, fileName);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_FILE_DELETE, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-delete - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return 0;
#endif
}

struct fileReadParameters
{
    Semaphore waitForResponse;
    MmsError err;
    int32_t frsmId;
    MmsFileReadHandler handler;
    void* handlerParameter;
    bool moreFollows;
};

static void
fileReadHandler(uint32_t invokeId, void* parameter, MmsError mmsError, uint8_t* buffer, uint32_t byteReceived,
        bool moreFollows)
{
    struct fileReadParameters* parameters = (struct fileReadParameters*) parameter;

    parameters->err = mmsError;

    if (mmsError == MMS_ERROR_NONE)
        parameters->handler(parameters->handlerParameter, parameters->frsmId, buffer, byteReceived);

    parameters->moreFollows = moreFollows;

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}

bool
MmsClient_fileRead(MmsClient self, MmsError* mmsError, int32_t frsmId, MmsFileReadHandler handler,
        void* handlerParameter)
{
#if (MMS_FILE_SERVICE == 1)

    struct fileReadParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.frsmId = frsmId;
    parameter.handler = handler;
    parameter.handlerParameter = handlerParameter;
    parameter.moreFollows = false;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_fileReadAsync(self, &err, frsmId, fileReadHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return parameter.moreFollows;

#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-read - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return false;
#endif
}

uint32_t
MmsClient_fileReadAsync(MmsClient self, MmsError* mmsError, int32_t frsmId, MmsConnection_FileReadHandler handler, void* parameter)
{
#if (MMS_FILE_SERVICE == 1)
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createFileReadRequest(invokeId, payload, frsmId);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_FILE_READ, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-read - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return 0;
#endif
}

struct getFileDirParameters
{
    Semaphore waitForResponse;
    MmsError err;
    bool moreFollows;
    MmsFileDirectoryHandler handler;
    void* handlerParameter;
};

static void
getFileDirHandler(uint32_t invokeId, void* parameter, MmsError mmsError, char* filename, uint32_t size, uint64_t lastModified,
        bool moreFollows)
{
    struct getFileDirParameters* parameters = (struct getFileDirParameters*) parameter;

    parameters->err = mmsError;

    if ((mmsError != MMS_ERROR_NONE) || (filename == NULL)) {
        parameters->moreFollows = moreFollows;

        /* last call --> unblock user thread */
        Semaphore_post(parameters->waitForResponse);
    }
    else {
        parameters->handler(parameters->handlerParameter, filename, size, lastModified);
    }
}

bool
MmsClient_getFileDirectory(MmsClient self, MmsError* mmsError, const char* fileSpecification, const char* continueAfter,
        MmsFileDirectoryHandler handler, void* handlerParameter)
{
#if (MMS_FILE_SERVICE == 1)

    bool moreFollows = false;

    struct getFileDirParameters parameter;
    parameter.handler = handler;
    parameter.handlerParameter = handlerParameter;

    MmsError err;

    parameter.waitForResponse = Semaphore_create(1);

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_getFileDirectoryAsync(self, &err, fileSpecification, continueAfter, getFileDirHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);
        err = parameter.err;
        moreFollows = parameter.moreFollows;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return moreFollows;
#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-directory - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return false;
#endif
}

uint32_t
MmsClient_getFileDirectoryAsync(MmsClient self, MmsError* mmsError, const char* fileSpecification, const char* continueAfter,
        MmsConnection_FileDirectoryHandler handler, void* parameter)
{
#if (MMS_FILE_SERVICE == 1)
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createFileDirectoryRequest(invokeId, payload, fileSpecification, continueAfter);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_GET_FILE_DIR, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-directory - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return 0;
#endif
}

void
MmsClient_fileRename(MmsClient self, MmsError* mmsError, const char* currentFileName, const char* newFileName)
{
#if (MMS_FILE_SERVICE == 1)

    struct fileOperationParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.success = false;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_fileRenameAsync(self, &err, currentFileName, newFileName, fileOperationHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;
#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-rename - service not supported");

    *mmsError = MMS_ERROR_OTHER;
#endif
}

uint32_t
MmsClient_fileRenameAsync(MmsClient self, MmsError* mmsError, const char* currentFileName, const char* newFileName,
        MmsClient_GenericServiceHandler handler, void* parameter)
{
#if (MMS_FILE_SERVICE == 1)
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createFileRenameRequest(invokeId, payload, currentFileName, newFileName);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_FILE_RENAME, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: file-rename - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return 0;
#endif
}

void
MmsClient_obtainFile(MmsClient self, MmsError* mmsError, const char* sourceFile, const char* destinationFile)
{
#if ((MMS_FILE_SERVICE == 1) && (MMS_OBTAIN_FILE_SERVICE == 1))

    struct fileOperationParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.success = false;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_obtainFileAsync(self, &err, sourceFile, destinationFile, fileOperationHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: obtain-file - service not supported");

    *mmsError = MMS_ERROR_OTHER;
#endif
}

uint32_t
MmsClient_obtainFileAsync(MmsClient self, MmsError* mmsError, const char* sourceFile, const char* destinationFile,
        MmsClient_GenericServiceHandler handler, void* parameter)
{
#if (MMS_FILE_SERVICE == 1)
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createObtainFileRequest(invokeId, payload, sourceFile, destinationFile);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_OBTAIN_FILE, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
#else
    if (DEBUG_MMS_CLIENT)
        libmms_log2(MMS_LOG_WARNING, self->endpoint, "MMS_CLIENT: obtain-file - service not supported");

    *mmsError = MMS_ERROR_OTHER;
    return 0;
#endif
}

struct writeVariableParameters
{
    Semaphore waitForResponse;
    MmsError err;
    MmsDataAccessError accessError;
};

static void
writeVariableHandler(uint32_t invokeId, void* parameter, MmsError mmsError, MmsDataAccessError accessError)
{
    struct writeVariableParameters* parameters = (struct writeVariableParameters*) parameter;

    parameters->err = mmsError;
    parameters->accessError = accessError;

    /* unblock user thread */
    Semaphore_post(parameters->waitForResponse);
}

MmsDataAccessError
MmsClient_writeVariable(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId,
        MmsValue* value)
{
    struct writeVariableParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.accessError = DATA_ACCESS_ERROR_SUCCESS;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_writeVariableAsync(self, &err, domainId, itemId, value, writeVariableHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return parameter.accessError;
}

uint32_t
MmsClient_writeVariableAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, MmsValue* value,
        MmsConnection_WriteVariableHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createWriteRequest(invokeId, domainId, itemId, value, payload, false);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_WRITE_VARIABLE, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

MmsDataAccessError
MmsClient_writeVariableAASpecific(MmsClient self, MmsError* mmsError,
        const char* itemId, MmsValue* value)
{
    struct writeVariableParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.accessError = DATA_ACCESS_ERROR_SUCCESS;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_writeVariableAASpecificAsync(self, &err, itemId, value, writeVariableHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return parameter.accessError;
}

uint32_t
MmsClient_writeVariableAASpecificAsync(MmsClient self, MmsError* mmsError,
        const char* itemId, MmsValue* value,
        MmsConnection_WriteVariableHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createWriteRequest(invokeId, NULL, itemId, value, payload, true);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_WRITE_VARIABLE, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

MmsDataAccessError
MmsClient_writeVariableComponent(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, const char* componentName,
        MmsValue* value)
{
    struct writeVariableParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.accessError = DATA_ACCESS_ERROR_SUCCESS;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_writeVariableComponentAsync(self, &err, domainId, itemId, componentName, value, writeVariableHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return parameter.accessError;
}

uint32_t
MmsClient_writeVariableComponentAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, const char* componentName, MmsValue* value,
        MmsConnection_WriteVariableHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createWriteComponenteRequest(invokeId, domainId, itemId, componentName, value, payload);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_WRITE_VARIABLE, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}

struct writeMultipleVariablesParameter
{
    Semaphore sem;
    MmsError err;
    LinkedList result;
};

static void
writeMultipleVariablesHandler(uint32_t invokeId, void* parameter, MmsError mmsError, LinkedList /* <MmsValue*> */ accessResults)
{
    struct writeMultipleVariablesParameter* parameters = (struct writeMultipleVariablesParameter*) parameter;

    parameters->err = mmsError;
    parameters->result = accessResults;

    /* unblock user thread */
    Semaphore_post(parameters->sem);
}

void
MmsClient_writeMultipleVariables(MmsClient self, MmsError* mmsError, const char* domainId,
        LinkedList /*<char*>*/items,
        LinkedList /* <MmsValue*> */values,
        /* OUTPUT */LinkedList* /* <MmsValue*> */accessResults)
{
    MmsError err = MMS_ERROR_NONE;

    struct writeMultipleVariablesParameter parameter;

    parameter.sem = Semaphore_create(1);;
    parameter.err = MMS_ERROR_NONE;
    parameter.result = NULL;

    Semaphore_wait(parameter.sem);

    MmsClient_writeMultipleVariablesAsync(self, &err, domainId, items, values, writeMultipleVariablesHandler, &parameter);

    if (err == MMS_ERROR_NONE) {

        Semaphore_wait(parameter.sem);

        err = parameter.err;

        if (accessResults)
            *accessResults = parameter.result;
        else
            LinkedList_destroyDeep(parameter.result, (LinkedListValueDeleteFunction) MmsValue_delete);
    }
    else {
        if (accessResults)
            *accessResults = NULL;
    }

    if (mmsError)
        *mmsError = err;

    Semaphore_destroy(parameter.sem);
}

uint32_t
MmsClient_writeMultipleVariablesAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        LinkedList /*<char*>*/ items, LinkedList /* <MmsValue*> */ values,
        MmsClient_WriteMultipleVariablesHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createWriteMultipleItemsRequest(invokeId, domainId, items, values, payload);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_WRITE_MULTIPLE_VARIABLES, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

    return invokeId;
}


MmsDataAccessError
MmsClient_writeArrayElements(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, int index, int numberOfElements,
        MmsValue* value)
{
    struct writeVariableParameters parameter;

    MmsError err = MMS_ERROR_NONE;

    parameter.waitForResponse = Semaphore_create(1);
    parameter.err = MMS_ERROR_NONE;
    parameter.accessError = DATA_ACCESS_ERROR_SUCCESS;

    Semaphore_wait(parameter.waitForResponse);

    MmsClient_writeArrayElementsAsync(self, &err, domainId, itemId, index, numberOfElements, value, writeVariableHandler, &parameter);

    if (err == MMS_ERROR_NONE) {
        Semaphore_wait(parameter.waitForResponse);

        err = parameter.err;
    }

    Semaphore_destroy(parameter.waitForResponse);

    if (mmsError)
        *mmsError = err;

    return parameter.accessError;
}

uint32_t
MmsClient_writeArrayElementsAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, int index, int numberOfElements,
        MmsValue* value,
        MmsConnection_WriteVariableHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {

        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createWriteRequestArray(invokeId, domainId, itemId, index, numberOfElements, value, payload);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_WRITE_VARIABLE, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

exit_function:
    return invokeId;
}

void
MmsClient_writeNamedVariableList(MmsClient self, MmsError* mmsError, bool isAssociationSpecific,
        const char* domainId, const char* itemId, LinkedList /* <MmsValue*> */values,
        /* OUTPUT */LinkedList* /* <MmsValue*> */accessResults)
{
    MmsError err = MMS_ERROR_NONE;

    struct writeMultipleVariablesParameter parameter;

    parameter.sem = Semaphore_create(1);;
    parameter.err = MMS_ERROR_NONE;
    parameter.result = NULL;

    Semaphore_wait(parameter.sem);

    MmsClient_writeNamedVariableListAsync(self, &err, isAssociationSpecific, domainId, itemId, values, writeMultipleVariablesHandler, &parameter);

    if (err == MMS_ERROR_NONE) {

        Semaphore_wait(parameter.sem);

        err = parameter.err;

        if (accessResults)
            *accessResults = parameter.result;
        else
            LinkedList_destroyDeep(parameter.result, (LinkedListValueDeleteFunction) MmsValue_delete);
    }
    else {
        if (accessResults)
            *accessResults = NULL;
    }

    if (mmsError)
        *mmsError = err;

    Semaphore_destroy(parameter.sem);
}

uint32_t
MmsClient_writeNamedVariableListAsync(MmsClient self, MmsError* mmsError, bool isAssociationSpecific,
        const char* domainId, const char* itemId, LinkedList /* <MmsValue*> */values,
        MmsClient_WriteMultipleVariablesHandler handler, void* parameter)
{
    uint32_t invokeId = 0;

    if (MmsClient_lockConnection(self)) {
        ByteBuffer* payload = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

        invokeId = MmsEndpointConnection_getNextInvokeId(self->connection);

        mmsClient_createWriteRequestNamedVariableList(invokeId, isAssociationSpecific, domainId, itemId, values, payload);

        MmsError err = MmsEndpointConnection_sendAsyncRequest(self->connection, invokeId, payload, MMS_CALL_TYPE_WRITE_MULTIPLE_VARIABLES, (void*) handler, parameter, NULL);

        MmsClient_unlockConnection(self);

        if (mmsError)
            *mmsError = err;
    }
    else {
        if (mmsError)
            *mmsError = MMS_ERROR_CONNECTION_LOST;
    }

exit_function:
    return invokeId;
}

void
MmsServerIdentity_destroy(MmsServerIdentity* self)
{
    if (self->modelName != NULL)
        GLOBAL_FREEMEM(self->modelName);

    if (self->vendorName != NULL)
        GLOBAL_FREEMEM(self->vendorName);

    if (self->revision != NULL)
        GLOBAL_FREEMEM(self->revision);

    GLOBAL_FREEMEM(self);
}
