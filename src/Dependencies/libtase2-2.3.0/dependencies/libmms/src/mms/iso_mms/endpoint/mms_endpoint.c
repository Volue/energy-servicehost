/*
 *  mms_endpoint.c
 *
 *  Implementation of MmsEndpoint and MmsEndpointConnection classes.
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "mms_endpoint.h"

#include "libmms_platform_includes.h"
#include "mms_endpoint_internal.h"
#include "mms_server_internal.h"
#include "mms_client_internal.h"

#include "ber_encoder.h"
#include "ber_decode.h"
#include "string_utilities.h"

#define CONFIG_MMS_CONNECTION_DEFAULT_TIMEOUT 5000
#define CONFIG_MMS_CONNECTION_DEFAULT_CONNECT_TIMEOUT 10000
#define OUTSTANDING_CALLS 10

typedef struct sWaitHandle* WaitHandle;

struct sWaitHandle
{
    uint64_t timeout;
    Semaphore waitLock;
    MmsEndpointState state;
    bool success;
};

static libmms_Log_Function generalLogFunction = NULL;
static void* generalLogFunctionParameter = NULL;

void
libmms_setLogFunction(libmms_Log_Function logFunction, void* parameter)
{
    generalLogFunction = logFunction;
    generalLogFunctionParameter = parameter;
}

#ifndef libmms_log2
void
libmms_log2(MmsLogLevel level, MmsEndpoint endpoint, const char* fmt, ...)
{
    char msg[MMS_MAX_LOG_MESSAGE_SIZE];

    MmsLogLevel logLevel = MMS_LOG_DEBUG;

    if (level >= logLevel) {

        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(msg, MMS_MAX_LOG_MESSAGE_SIZE, fmt, argptr);
        va_end(argptr);

        if (endpoint && endpoint->logHandler) {
            endpoint->logHandler(endpoint->logHandlerParameter, level, NULL, msg);
        }
        else if (generalLogFunction) {
            generalLogFunction(generalLogFunctionParameter, 2, level, msg);
        }
        else {
            const char* levelStr = "UNKNOWN";

            switch (level) {
            case MMS_LOG_DEBUG:
                levelStr = "DEBUG";
                break;
            case MMS_LOG_INFO:
                levelStr = "INFO";
                break;
            case MMS_LOG_WARNING:
                levelStr = "WARNING";
                break;
            case MMS_LOG_ERROR:
                levelStr = "ERROR";
                break;
            default:
                break;
            }

            printf("[%s]: %s\n", levelStr, msg);
        }

    }
}
#endif

#ifndef libmms_log
void
libmms_log(MmsLogLevel level, MmsEndpointConnection peer, const char* fmt, ...)
{
    char msg[MMS_MAX_LOG_MESSAGE_SIZE];

    if (peer) {

        MmsLogLevel logLevel = MMS_LOG_DEBUG;

        MmsEndpoint endpoint = peer->endpoint;

        if (level >= logLevel) {
            va_list argptr;
            va_start(argptr, fmt);
            vsnprintf(msg, MMS_MAX_LOG_MESSAGE_SIZE, fmt, argptr);
            va_end(argptr);

            if (endpoint && endpoint->logHandler) {
                endpoint->logHandler(endpoint->logHandlerParameter, level, peer, msg);
            }
            else if (generalLogFunction) {
                generalLogFunction(generalLogFunctionParameter, 2, level, msg);
            }
            else {
                const char* levelStr = "UNKNOWN";

                switch (level) {
                case MMS_LOG_DEBUG:
                    levelStr = "DEBUG";
                    break;
                case MMS_LOG_INFO:
                    levelStr = "INFO";
                    break;
                case MMS_LOG_WARNING:
                    levelStr = "WARNING";
                    break;
                case MMS_LOG_ERROR:
                    levelStr = "ERROR";
                    break;
                default:
                    break;
                }

                printf("[%s]: %s\n", levelStr, msg);
            }

        }
    }
}
#endif

#ifndef libmms_iso_log
void
libmms_iso_log(MmsLogLevel level, const char* fmt, ...)
{
    char msg[MMS_MAX_LOG_MESSAGE_SIZE];

    MmsLogLevel logLevel = MMS_LOG_DEBUG;

    if (level >= logLevel) {
        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(msg, MMS_MAX_LOG_MESSAGE_SIZE, fmt, argptr);
        va_end(argptr);

        if (generalLogFunction) {
            generalLogFunction(generalLogFunctionParameter, 5, level, msg);
        }
        else {
            const char* levelStr = "UNKNOWN";

            switch (level) {
            case MMS_LOG_DEBUG:
                levelStr = "DEBUG";
                break;
            case MMS_LOG_INFO:
                levelStr = "INFO";
                break;
            case MMS_LOG_WARNING:
                levelStr = "WARNING";
                break;
            case MMS_LOG_ERROR:
                levelStr = "ERROR";
                break;
            default:
                break;
            }

            printf("[%s]: %s\n", levelStr, msg);
        }

    }
}
#endif

static void
setEndpointState(MmsEndpoint self, MmsEndpointState newState)
{
    Semaphore_wait(self->stateLock);
    self->state = newState;
    Semaphore_post(self->stateLock);

    if (self->stateChangedHandler)
        self->stateChangedHandler(self, self->stateChangedHandlerParameter, newState);

    Semaphore_wait(self->waitHandlesLock);

    LinkedList element = LinkedList_getNext(self->waitHandles);

    while (element) {

        WaitHandle handle = (WaitHandle) LinkedList_getData(element);

        if (newState == MMS_ENDPOINT_STATE_ERROR) {
            handle->success = false;
            Semaphore_post(handle->waitLock);
        }
        else {
            if (handle->state == newState) {
                handle->success = true;
                Semaphore_post(handle->waitLock);
            }
        }

        element = LinkedList_getNext(element);
    }

    Semaphore_post(self->waitHandlesLock);
}

static MmsEndpointState
getEndpointState(MmsEndpoint self)
{
    MmsEndpointState state;

    Semaphore_wait(self->stateLock);
    state = self->state;
    Semaphore_post(self->stateLock);

    return state;
}

static MmsEndpointConnection
getMatchingOpenTcpClientConnection(MmsEndpoint self, IsoClientConnection isoConnection)
{
    MmsEndpointConnection matchingConnection = NULL;

    Semaphore_wait(self->openConnectionsLock);

    LinkedList element = LinkedList_getNext(self->openConnections);

    while (element) {

        MmsEndpointConnection connection = (MmsEndpointConnection) LinkedList_getData(element);

        if (connection->isoClient == isoConnection) {
            matchingConnection = connection;
            break;
        }

        element = LinkedList_getNext(element);
    }

    Semaphore_post(self->openConnectionsLock);

    return matchingConnection;
}

static MmsEndpointConnection
getMatchingOpenTcpServerConnection(MmsEndpoint self, IsoConnection isoConnection)
{
    MmsEndpointConnection matchingConnection = NULL;

    Semaphore_wait(self->openConnectionsLock);

    LinkedList element = LinkedList_getNext(self->openConnections);

    while (element) {

        MmsEndpointConnection connection = (MmsEndpointConnection) LinkedList_getData(element);

        if (connection->isoConnection == isoConnection) {
            matchingConnection = connection;
            break;
        }

        element = LinkedList_getNext(element);
    }

    Semaphore_post(self->openConnectionsLock);

    return matchingConnection;
}

/**********************************************************************************************
 * MMS General service handling functions
 *********************************************************************************************/

/* generic endpoint or server related */
void
MmsEndpointConnection_handleConfirmedRequestPdu(
        MmsEndpointConnection self,
        uint8_t* buffer, int bufPos, int maxBufPos)
{
    uint32_t invokeId = 0;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bool extendedTag = false;

        if ((tag & 0x1f) == 0x1f) {
            extendedTag = true;
            tag = buffer[bufPos++];
        }

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if (bufPos < 0) {
            MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);

            return;
        }

        if (bufPos + length > maxBufPos) {
            MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);

            return;
        }

        if (extendedTag) {
            switch (tag)
            {

#if (MMS_OBTAIN_FILE_SERVICE == 1)


#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
            case 0x2e: /* obtain-file */
                if (self->server->fileServiceEnabled)
                    MmsEndpointConnection_handleObtainFileRequest(self, buffer, bufPos, bufPos + length, invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);
                }

                break;
#else
            case 0x2e: /* obtain-file */
                MmsEndpointConnection_handleObtainFileRequest(self, buffer, bufPos, bufPos + length, invokeId);
                break;
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */


#endif /* MMS_OBTAIN_FILE_SERVICE == 1 */

#if (MMS_JOURNAL_SERVICE == 1)

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
            case 0x41: /* read-journal */
                if (self->server->journalServiceEnabled)
                    mmsServer_handleReadJournalRequest(self, buffer, bufPos, bufPos + length, invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);
                }

                break;

#else
            case 0x41: /* read-journal */
                mmsServer_handleReadJournalRequest(self, buffer, bufPos, bufPos + length, invokeId);
                break;
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#endif /* (MMS_JOURNAL_SERVICE == 1) */

#if (MMS_FILE_SERVICE == 1)

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
            case 0x48: /* file-open-request */
                if (self->server->fileServiceEnabled)
                    MmsEndpointConnection_handleFileOpenRequest(self, buffer, bufPos, bufPos + length, invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);
                }

                break;

#else
            case 0x48: /* file-open-request */
                MmsEndpointConnection_handleFileOpenRequest(self, buffer, bufPos, bufPos + length, invokeId);
                break;
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
            case 0x49: /* file-read-request */
                if (self->server->fileServiceEnabled)
                    MmsEndpointConnection_handleFileReadRequest(self, buffer, bufPos, bufPos + length, invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);
                }

                break;
#else
            case 0x49: /* file-read-request */
                MmsEndpointConnection_handleFileReadRequest(self, buffer, bufPos, bufPos + length, invokeId);
                break;
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
            case 0x4a: /* file-close-request */
                if (self->server->fileServiceEnabled)
                    MmsEndpointConnection_handleFileCloseRequest(self, buffer, bufPos, bufPos + length, invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);
                }

                break;
#else
            case 0x4a: /* file-close-request */
                MmsEndpointConnection_handleFileCloseRequest(self, buffer, bufPos, bufPos + length, invokeId);
                break;
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
            case 0x4b: /* file-rename-request */
                if (self->server->fileServiceEnabled)
                    MmsEndpointConnection_handleFileRenameRequest(self, buffer, bufPos, bufPos + length, invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);
                }

                break;
#else
            case 0x4b: /* file-rename-request */
                MmsEndpointConnection_handleFileRenameRequest(self, buffer, bufPos, bufPos + length, invokeId);
                break;
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
            case 0x4c: /* file-delete-request */
                if (self->server->fileServiceEnabled)
                    MmsEndpointConnection_handleFileDeleteRequest(self, buffer, bufPos, bufPos + length, invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);
                }

                break;
#else
            case 0x4c: /* file-delete-request */
                MmsEndpointConnection_handleFileDeleteRequest(self, buffer, bufPos, bufPos + length, invokeId);
                break;
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)
            case 0x4d: /* file-directory-request */
                if (self->server->fileServiceEnabled)
                    MmsEndpointConnection_handleFileDirectoryRequest(self, buffer, bufPos, bufPos + length, invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);
                }

                break;
#else
            case 0x4d: /* file-directory-request */
                MmsEndpointConnection_handleFileDirectoryRequest(self, buffer, bufPos, bufPos + length, invokeId);
                break;
#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#endif /* MMS_FILE_SERVICE == 1 */

            default:

                MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                return;
                break;
            }
        }
        else {
            switch (tag)
            {
            case 0x02: /* invoke Id */
                invokeId = BerDecoder_decodeUint32(buffer, length, bufPos);
                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, self, "MMS_SERVER: confirmed-request (invoke-id: %u)", invokeId);
                break;

#if (MMS_STATUS_SERVICE == 1)
            case 0x80: /* status-request */
                if (self->serverConnection) {
                    mmsServer_handleStatusRequest(self, buffer, bufPos, invokeId);
                }
                else {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, self, "MMS_SERVER: status (invoke-id: %u) reject --> unrecognized-service", invokeId);

                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;
#endif /* MMS_STATUS_SERVICE == 1 */

#if (MMS_GET_NAME_LIST == 1)
            case 0xa1: /* get-name-list-request */
                if (self->serverConnection) {
                    mmsServer_handleGetNameListRequest(self, buffer, bufPos, bufPos + length, invokeId);
                }
                else {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, self, "MMS_SERVER: get-name-list (invoke-id: %u) reject --> unrecognized-service", invokeId);

                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;
#endif /* (MMS_GET_NAME_LIST == 1) */

#if (MMS_IDENTIFY_SERVICE == 1)
            case 0x82: /* identify */

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, self, "MMS_SERVER: identify (invoke-id: %u)", invokeId);

                {
                    ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

                    if (response) {
                        mmsServer_handleIdentifyRequest(invokeId, response,
                            MmsEndpoint_getVendorName(self->endpoint),
                            MmsEndpoint_getModelName(self->endpoint),
                            MmsEndpoint_getRevision(self->endpoint));

                        MmsEndpointConnection_sendMessage(self, response);

                        MmsEndpoint_releaseTransmitBuffer(self->endpoint);
                    }
                }

                break;
#endif /* MMS_IDENTIFY_SERVICE == 1 */

            case 0xa4: /* read-request */
                if (self->serverConnection) {
                    mmsServer_handleReadRequest(self, buffer, bufPos, bufPos + length,
                            invokeId);
                }
                else {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, self, "MMS_SERVER: read (invoke-id: %u) reject --> unrecognized-service", invokeId);

                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;

#if (MMS_WRITE_SERVICE == 1)
            case 0xa5: /* write-request */
                if (self->serverConnection) {
                    mmsServer_handleWriteRequest(self, buffer, bufPos, bufPos + length, invokeId);
                }
                else {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, self, "MMS_SERVER: write (invoke-id: %u) reject --> unrecognized-service", invokeId);

                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;
#endif /* (MMS_WRITE_SERVICE == 1) */

#if (MMS_GET_VARIABLE_ACCESS_ATTRIBUTES == 1)
            case 0xa6: /* get-variable-access-attributes-request */
                if (self->serverConnection) {
                    mmsServer_handleGetVariableAccessAttributesRequest(self,
                            buffer, bufPos, bufPos + length,
                            invokeId);
                }
                else {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, self, "MMS_SERVER: get-var-access-attributes (invoke-id: %u) reject --> unrecognized-service", invokeId);

                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;
#endif /* MMS_GET_VARIABLE_ACCESS_ATTRIBUTES == 1 */

#if (MMS_DYNAMIC_DATA_SETS == 1)

#if (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1)

            case 0xab: /* define-named-variable-list */
                if ((self->serverConnection) && (self->serverConnection->server->dynamicVariableListServiceEnabled))
                    mmsServer_handleDefineNamedVariableListRequest(self,
                            buffer, bufPos, bufPos + length,
                            invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;

            case 0xad: /* delete-named-variable-list-request */
                if ((self->serverConnection) && (self->serverConnection->server->dynamicVariableListServiceEnabled))
                    mmsServer_handleDeleteNamedVariableListRequest(self,
                            buffer, bufPos, bufPos + length,
                            invokeId);
                else {
                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;

#else
            case 0xab: /* define-named-variable-list */
                if (self->serverConnection) {
                    mmsServer_handleDefineNamedVariableListRequest(self,
                            buffer, bufPos, bufPos + length,
                            invokeId);
                }
                else {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, self, "MMS_SERVER: define-named-variable-list (invoke-id: %u) reject --> unrecognized-service", invokeId);

                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;

            case 0xad: /* delete-named-variable-list-request */
                if (self->serverConnection) {
                    mmsServer_handleDeleteNamedVariableListRequest(self,
                            buffer, bufPos, bufPos + length,
                            invokeId);
                }
                else {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, self, "MMS_SERVER: delete-named-variable-list (invoke-id: %u) reject --> unrecognized-service", invokeId);

                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;

#endif /* (CONFIG_MMS_SERVER_CONFIG_SERVICES_AT_RUNTIME == 1) */

#endif /* (MMS_DYNAMIC_DATA_SETS == 1) */

#if (MMS_GET_DATA_SET_ATTRIBUTES == 1)
            case 0xac: /* get-named-variable-list-attributes-request */
                if (self->serverConnection) {
                    mmsServer_handleGetNamedVariableListAttributesRequest(self,
                            buffer, bufPos, bufPos + length,
                            invokeId);
                }
                else {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, self, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) reject --> unrecognized-service", invokeId);

                    MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                    return;
                }
                break;
#endif /* (MMS_GET_DATA_SET_ATTRIBUTES == 1) */

            case 0x00: /* indefinite length end tag -> ignore */
                break;

            default:

                MmsEndpointConnection_sendRejectPdu(self, &invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE);

                return;
                break;
            }
        }

        bufPos += length;
    }

}

static void
handleUnconfirmedMmsPdu(MmsEndpointConnection self, uint8_t* buffer, int bufPos, int maxBufPos)
{
    if (self->endpoint->reportHandler != NULL) {

        MmsPdu_t* mmsPdu = 0; /* allow asn1c to allocate structure */

        asn_dec_rval_t  rval = ber_decode(NULL, &asn_DEF_MmsPdu,
                (void**) &mmsPdu, buffer + bufPos, maxBufPos - bufPos);

        if (rval.code == RC_OK) {

#if (CONFIG_MMS_THREADLESS_STACK != 1)
            Semaphore_wait(self->endpoint->reportHandlerMutex);
#endif

            if (DEBUG_MMS_ENDPOINT)
                libmms_log(MMS_LOG_DEBUG, self, "Received unconfirmed PDU(size:%i)", (int) rval.consumed);

            if (mmsPdu->present == MmsPdu_PR_unconfirmedPDU) {

                if (mmsPdu->choice.unconfirmedPDU.unconfirmedService.present ==
                        UnconfirmedService_PR_informationReport)
                {
                    InformationReport_t* report =
                            &(mmsPdu->choice.unconfirmedPDU.unconfirmedService.choice.informationReport);

                    if (report->variableAccessSpecification.present ==
                            VariableAccessSpecification_PR_variableListName)
                    {
                        if (report->variableAccessSpecification.choice.variableListName.present
                                == ObjectName_PR_vmdspecific)
                        {
                            int nameSize =
                                    report->variableAccessSpecification.choice.variableListName.choice.vmdspecific.size;

                            uint8_t* buffer =
                                    report->variableAccessSpecification.choice.variableListName.choice.vmdspecific.buf;

                            if (nameSize < 65) {
                                char variableListName[65];
                                memcpy(variableListName, buffer, nameSize);
                                variableListName[nameSize] = 0;

                                int listSize = report->listOfAccessResult.list.count;

                                MmsValue* values = mmsMsg_parseListOfAccessResults(
                                        self, report->listOfAccessResult.list.array, listSize, true);

                                self->endpoint->reportHandler(self, self->endpoint->reportHandlerParameter, NULL,
                                        variableListName, values, MMS_INFO_HANDLER_IS_VARIABLE_LIST_NAME);

                            }
                        }
                        else if (report->variableAccessSpecification.choice.variableListName.present
                                == ObjectName_PR_domainspecific)
                        {
                            int domainNameSize = report->variableAccessSpecification.choice.variableListName.choice.domainspecific.domainId.size;
                            int itemNameSize = report->variableAccessSpecification.choice.variableListName.choice.domainspecific.itemId.size;

                            if ((domainNameSize < 65) && (itemNameSize < 65)) {
                                char domainId[65];
                                char variableListName[65];

                                uint8_t* domainIdBuf = report->variableAccessSpecification.choice.variableListName.choice.domainspecific.domainId.buf;
                                uint8_t* variableListNameBuf = report->variableAccessSpecification.choice.variableListName.choice.domainspecific.itemId.buf;

                                memcpy(domainId, domainIdBuf, domainNameSize);
                                domainId[domainNameSize] = 0;
                                memcpy(variableListName, variableListNameBuf, itemNameSize);
                                variableListName[itemNameSize] = 0;

                                int listSize = report->listOfAccessResult.list.count;

                                MmsValue* values = mmsMsg_parseListOfAccessResults(
                                     self, report->listOfAccessResult.list.array, listSize, true);

                                self->endpoint->reportHandler(self, self->endpoint->reportHandlerParameter,
                                        domainId, variableListName, values, MMS_INFO_HANDLER_IS_VARIABLE_LIST_NAME);

                            }
                        }
                        else
                        {
                            /* Ignore association specific information reports (not used by IEC 61850 nor TASE.2) */
                        }

                    }
                    else if (report->variableAccessSpecification.present == VariableAccessSpecification_PR_listOfVariable)
                    {
                        int listSize = report->listOfAccessResult.list.count;
                        int variableSpecSize = report->variableAccessSpecification.choice.listOfVariable.list.count;

                        if (listSize != variableSpecSize) {
                            if (DEBUG_MMS_ENDPOINT)
                                libmms_log(MMS_LOG_ERROR, self, "report contains wrong number of access results");
                        }
                        else {

                            MmsValue* values = mmsMsg_parseListOfAccessResults(
                                    self, report->listOfAccessResult.list.array, listSize, false);

                            int i;
                            for (i = 0; i < variableSpecSize; i++) {
                                if (report->variableAccessSpecification.choice.listOfVariable.list.array[i]->variableSpecification.present
                                        == VariableSpecification_PR_name)
                                {
                                    uint8_t handlerFlags = 0;

                                    if (i == 0)
                                        handlerFlags += MMS_INFO_HANDLER_REPORT_START;

                                    if (i == (variableSpecSize - 1))
                                        handlerFlags += MMS_INFO_HANDLER_REPORT_END;

                                    if (report->variableAccessSpecification.choice.listOfVariable.list.array[i]
                                            ->variableSpecification.choice.name.present == ObjectName_PR_vmdspecific)
                                    {
                                        int nameSize =
                                                report->variableAccessSpecification.choice.listOfVariable.list.array[i]
                                                        ->variableSpecification.choice.name.choice.vmdspecific.size;

                                        uint8_t* buffer =
                                                report->variableAccessSpecification.choice.listOfVariable.list.array[i]
                                                        ->variableSpecification.choice.name.choice.vmdspecific.buf;

                                        if (nameSize < 65) {
                                            char variableName[65];
                                            memcpy(variableName, buffer, nameSize);
                                            variableName[nameSize] = 0;

                                            MmsValue* value = values;

                                            if (variableSpecSize != 1)
                                                value = MmsValue_getElement(values, i);

                                            self->endpoint->reportHandler(self, self->endpoint->reportHandlerParameter,
                                                    NULL, variableName, value, handlerFlags);

                                            /* report handler is responsible to delete the MmsValue object */
                                            if (variableSpecSize != 1)
                                                MmsValue_setElement(values, i, NULL);
                                            else
                                                values = NULL;
                                        }
                                    }
                                    else if (report->variableAccessSpecification.choice.listOfVariable.list.array[i]
                                            ->variableSpecification.choice.name.present == ObjectName_PR_domainspecific) {

                                        int domainNameSize =
                                            report->variableAccessSpecification.choice.listOfVariable.list.array[i]
                                                ->variableSpecification.choice.name.choice.domainspecific.domainId.size;

                                        int itemNameSize =
                                             report->variableAccessSpecification.choice.listOfVariable.list.array[i]
                                                 ->variableSpecification.choice.name.choice.domainspecific.itemId.size;

                                        if ((domainNameSize < 65) && (itemNameSize < 65)) {
                                            char domainNameStr[65];
                                            char itemNameStr[65];

                                            uint8_t* domainNameBuffer =
                                                report->variableAccessSpecification.choice.listOfVariable.list.array[i]
                                                       ->variableSpecification.choice.name.choice.domainspecific.domainId.buf;

                                            uint8_t* itemNamebuffer =
                                                    report->variableAccessSpecification.choice.listOfVariable.list.array[i]
                                                           ->variableSpecification.choice.name.choice.domainspecific.itemId.buf;

                                            memcpy(domainNameStr, domainNameBuffer, domainNameSize);
                                            domainNameStr[domainNameSize] = 0;
                                            memcpy(itemNameStr, itemNamebuffer, itemNameSize);
                                            itemNameStr[itemNameSize] = 0;

                                            MmsValue* value = values;

                                            if (variableSpecSize != 1)
                                               value = MmsValue_getElement(values, i);

                                            self->endpoint->reportHandler(self, self->endpoint->reportHandlerParameter,
                                                    domainNameStr, itemNameStr, value, handlerFlags);

                                            /* report handler is responsible to delete the MmsValue object */
                                            if (variableSpecSize != 1)
                                                MmsValue_setElement(values, i, NULL);
                                            else
                                                values = NULL;

                                        }
                                    }
                                }
                            }

                            if (values != NULL)
                                MmsValue_delete(values);

                        }
                    }
                    else {
                        /* Ignore */
                        if (DEBUG_MMS_ENDPOINT)
                            libmms_log(MMS_LOG_ERROR, self, "unrecognized information report");
                    }

                }

            }

#if (CONFIG_MMS_THREADLESS_STACK != 1)
            Semaphore_post(self->endpoint->reportHandlerMutex);
#endif
        }
        else {
            if (DEBUG_MMS_ENDPOINT)
                libmms_log(MMS_LOG_ERROR, self, "Error parsing unconfirmed PDU (size: %i) at %u", (maxBufPos - bufPos), (uint32_t) rval.consumed);
        }

        asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
    }
}

static MmsOutstandingCall
checkForOutstandingCall(MmsEndpointConnection self, uint32_t invokeId)
{
    int i = 0;

    Semaphore_wait(self->outstandingCallsLock);

    for (i = 0; i < OUTSTANDING_CALLS; i++) {
        if (self->outstandingCalls[i].isUsed) {
            if (self->outstandingCalls[i].invokeId == invokeId) {
                Semaphore_post(self->outstandingCallsLock);
                return &(self->outstandingCalls[i]);
            }
        }
    }

    Semaphore_post(self->outstandingCallsLock);

    return NULL;
}

static bool
addToOutstandingCalls(MmsEndpointConnection self, uint32_t invokeId, eMmsOutstandingCallType type, void* userCallback, void* userParameter, void* internalParameter)
{
    int i = 0;

    Semaphore_wait(self->outstandingCallsLock);

    for (i = 0; i < OUTSTANDING_CALLS; i++) {
        if (self->outstandingCalls[i].isUsed == false) {
            self->outstandingCalls[i].isUsed = true;
            self->outstandingCalls[i].invokeId = invokeId;
            self->outstandingCalls[i].timeout = Hal_getTimeInMs() + self->endpoint->requestTimeout;
            self->outstandingCalls[i].type = type;
            self->outstandingCalls[i].userCallback = userCallback;
            self->outstandingCalls[i].userParameter = userParameter;
            self->outstandingCalls[i].internalParameter = internalParameter;
            Semaphore_post(self->outstandingCallsLock);
            return true;
        }
    }

    Semaphore_post(self->outstandingCallsLock);

    return false;
}

static void
removeFromOutstandingCalls(MmsEndpointConnection self, uint32_t invokeId)
{
    int i = 0;

    Semaphore_wait(self->outstandingCallsLock);

    for (i = 0; i < OUTSTANDING_CALLS; i++) {
        if (self->outstandingCalls[i].isUsed) {
            if (self->outstandingCalls[i].invokeId == invokeId) {
                self->outstandingCalls[i].isUsed = false;
                break;
            }
        }
    }

    Semaphore_post(self->outstandingCallsLock);
}

static bool
addToOutstandingServices(MmsEndpointConnection self, uint8_t* buffer, int bufPos, int maxBufPos)
{
    int i = 0;
    int maxOutstandingCalls = OUTSTANDING_CALLS;

    Semaphore_wait(self->outstandingServicesLock);

    if ((self->nextOutstandingService != -1) && (self->nextOutstandingService == self->oldestOutstandingService)) {
        /* queue is full */

        Semaphore_post(self->outstandingServicesLock);

        return false;
    }

    if (self->nextOutstandingService == -1) {
        self->nextOutstandingService = 0;
        self->oldestOutstandingService = 0;
    }

    self->outstandingServices[self->nextOutstandingService].bufPos = bufPos;
    self->outstandingServices[self->nextOutstandingService].mmsMsgSize = maxBufPos;
    memcpy(self->outstandingServices[self->nextOutstandingService].mmsMsgBuffer, buffer, maxBufPos);

    self->nextOutstandingService++;

    if (self->nextOutstandingService == maxOutstandingCalls)
        self->nextOutstandingService = 0;

    Semaphore_post(self->outstandingServicesLock);

    return true;
}

static void
handleAsyncResponse(MmsEndpointConnection self, ByteBuffer* response, uint32_t bufPos, MmsOutstandingCall outstandingCall, MmsError err, bool removeOutstandingCall)
{

    if (outstandingCall->type == MMS_CALL_TYPE_READ_VARIABLE) {

        MmsClient_ReadVariableHandler handler =
                (MmsClient_ReadVariableHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE)
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL);
        else {
            if (response) {
                MmsValue* value = mmsClient_parseReadResponse(self, response, NULL, false);

                handler(outstandingCall->invokeId, outstandingCall->userParameter, MMS_ERROR_NONE, value);
            }

        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_READ_MULTIPLE_VARIABLES) {

        MmsClient_ReadVariableHandler handler =
                (MmsClient_ReadVariableHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE)
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL);
        else {
            if (response) {
                MmsValue* value = mmsClient_parseReadResponse(self, response, NULL, true);

                handler(outstandingCall->invokeId, outstandingCall->userParameter, MMS_ERROR_NONE, value);
            }

        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_WRITE_VARIABLE) {

        MmsConnection_WriteVariableHandler handler =
                (MmsConnection_WriteVariableHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, DATA_ACCESS_ERROR_NO_RESPONSE);
        }
        else {
            if (response) {
                MmsDataAccessError daError = mmsClient_parseWriteResponse(response, bufPos, &err);

                handler(outstandingCall->invokeId, outstandingCall->userParameter, err, daError);
            }
        }

    }
    else if (outstandingCall->type == MMS_CALL_TYPE_WRITE_MULTIPLE_VARIABLES) {

        MmsClient_WriteMultipleVariablesHandler handler =
                (MmsClient_WriteMultipleVariablesHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL);
        }
        else {
            if (response) {
                LinkedList accessResults = NULL;

                mmsClient_parseWriteMultipleItemsResponse(response, bufPos, &err, -1, &accessResults);

                handler(outstandingCall->invokeId, outstandingCall->userParameter, err, accessResults);
            }
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_READ_NVL_DIRECTORY) {
        MmsConnection_ReadNVLDirectoryHandler handler =
                (MmsConnection_ReadNVLDirectoryHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL, false);
        }
        else {
            if (response) {
                bool deletable = false;

                LinkedList accessSpec = mmsClient_parseGetNamedVariableListAttributesResponse(response, &deletable);

                if (accessSpec == NULL)
                    err = MMS_ERROR_PARSING_RESPONSE;

                handler(outstandingCall->invokeId, outstandingCall->userParameter, err, accessSpec, deletable);
            }
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_DEFINE_NVL) {

        MmsClient_GenericServiceHandler handler =
                (MmsClient_GenericServiceHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, false);
        }
        else {
            bool success = false;

            if (!mmsClient_parseDefineNamedVariableResponse(response, NULL))
                err = MMS_ERROR_PARSING_RESPONSE;
            else
                success = true;

            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, success);
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_DELETE_NVL) {

        MmsClient_GenericServiceHandler handler =
                (MmsClient_GenericServiceHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, false);
        }
        else {
            bool success = false;

            if (mmsClient_parseDeleteNamedVariableListResponse(response, NULL))
                success = true;

            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, success);
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_GET_VAR_ACCESS_ATTR) {
        MmsConnection_GetVariableAccessAttributesHandler handler =
                (MmsConnection_GetVariableAccessAttributesHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL);
        }
        else {
            MmsVariableSpecification* typeSpec = mmsClient_parseGetVariableAccessAttributesResponse(response, NULL);

            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, typeSpec);
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_GET_SERVER_STATUS) {
        MmsConnection_GetServerStatusHandler handler =
                (MmsConnection_GetServerStatusHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, 0, 0);
        }
        else {
            int vmdLogicalStatus;
            int vmdPhysicalStatus;

            if (mmsClient_parseStatusResponse(self, response, bufPos, &vmdLogicalStatus, &vmdPhysicalStatus) == false)
                err = MMS_ERROR_PARSING_RESPONSE;

            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, vmdLogicalStatus, vmdPhysicalStatus);
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_IDENTIFY) {
        MmsConnection_IdentifyHandler handler =
                (MmsConnection_IdentifyHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL, NULL, NULL);
        }
        else {

            if (mmsClient_parseIdentifyResponse(self, response, bufPos, outstandingCall->invokeId, handler,
                        outstandingCall->userParameter) == false)
            {
                handler(outstandingCall->invokeId, outstandingCall->userParameter, MMS_ERROR_PARSING_RESPONSE,
                        NULL, NULL, NULL);
            }

        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_READ_JOURNAL) {

        MmsClient_ReadJournalHandler handler =
                (MmsClient_ReadJournalHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL, false);
        }
        else {
            bool moreFollows = false;
            LinkedList entries = NULL;

            if (mmsClient_parseReadJournalResponse(response, bufPos, &moreFollows, &entries) == false) {
                handler(outstandingCall->invokeId, outstandingCall->userParameter, MMS_ERROR_PARSING_RESPONSE, NULL, false);
            }
            else {
                handler(outstandingCall->invokeId, outstandingCall->userParameter, err, entries, moreFollows);
            }
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_GET_NAME_LIST) {

        MmsClient_GetNameListHandler handler =
                (MmsClient_GetNameListHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL, false);
        }
        else {
            LinkedList nameList = (LinkedList) outstandingCall->internalParameter;

            bool moreFollows = mmsClient_parseGetNameListResponse(self, &nameList, response);

            if (nameList == NULL) {
                handler(outstandingCall->invokeId, outstandingCall->userParameter, MMS_ERROR_PARSING_RESPONSE, NULL, false);
            }
            else {
                handler(outstandingCall->invokeId, outstandingCall->userParameter, err, nameList, moreFollows);
            }
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_FILE_OPEN) {

        MmsConnection_FileOpenHandler handler =
                (MmsConnection_FileOpenHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, 0, 0, 0);
        }
        else {
            int32_t frsmId;
            uint32_t fileSize;
            uint64_t lastModified;


            if (mmsMsg_parseFileOpenResponse(ByteBuffer_getBuffer(response), bufPos, ByteBuffer_getSize(response),
                    &frsmId, &fileSize, &lastModified) == false)
            {
                handler(outstandingCall->invokeId, outstandingCall->userParameter, MMS_ERROR_PARSING_RESPONSE, 0, 0, 0);
            }
            else {
                handler(outstandingCall->invokeId, outstandingCall->userParameter, err, frsmId, fileSize, lastModified);
            }
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_FILE_READ) {
        MmsConnection_FileReadHandler handler =
                (MmsConnection_FileReadHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL, 0, false);
        }
        else {
            bool moreFollows;
            uint8_t* dataBuffer;
            int dataLength;

            if (mmsMsg_parseFileReadResponse(ByteBuffer_getBuffer(response), bufPos, ByteBuffer_getSize(response), &moreFollows, &dataBuffer, &dataLength) == false)
            {
                handler(outstandingCall->invokeId, outstandingCall->userParameter, MMS_ERROR_PARSING_RESPONSE, NULL, 0, false);
            }
            else {
                handler(outstandingCall->invokeId, outstandingCall->userParameter, err, dataBuffer, dataLength, moreFollows);
            }
        }
    }
    else if ((outstandingCall->type == MMS_CALL_TYPE_FILE_CLOSE) ||
             (outstandingCall->type == MMS_CALL_TYPE_FILE_DELETE) ||
             (outstandingCall->type == MMS_CALL_TYPE_FILE_RENAME) ||
             (outstandingCall->type == MMS_CALL_TYPE_OBTAIN_FILE))
    {
        MmsClient_GenericServiceHandler handler =
                (MmsClient_GenericServiceHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, false);
        }
        else {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, true);
        }
    }
    else if (outstandingCall->type == MMS_CALL_TYPE_GET_FILE_DIR) {
        MmsConnection_FileDirectoryHandler handler =
                (MmsConnection_FileDirectoryHandler) outstandingCall->userCallback;

        if (err != MMS_ERROR_NONE) {
            handler(outstandingCall->invokeId, outstandingCall->userParameter, err, NULL, 0, 0, false);
        }
        else {
            if (mmsClient_parseFileDirectoryResponse(response, bufPos, outstandingCall->invokeId, handler, outstandingCall->userParameter) == false)
                handler(outstandingCall->invokeId, outstandingCall->userParameter, MMS_ERROR_PARSING_RESPONSE, NULL, 0, 0, false);
        }
    }

    if (removeOutstandingCall)
        removeFromOutstandingCalls(self, outstandingCall->invokeId);
}

static MmsError
convertServiceErrorToMmsError(MmsServiceError serviceError)
{
    MmsError mmsError;

    switch (serviceError.errorClass)
    {
    case 0: /* class: vmd-state */
        mmsError = MMS_ERROR_VMDSTATE_OTHER;
        break;

    case 1: /* class: application-reference */
        mmsError = MMS_ERROR_APPLICATION_REFERENCE_OTHER;
        break;

    case 2: /* class: definition */
        switch (serviceError.errorCode)
        {
        case 1:
            mmsError = MMS_ERROR_DEFINITION_OBJECT_UNDEFINED;
            break;
        case 2:
            mmsError = MMS_ERROR_DEFINITION_INVALID_ADDRESS;
            break;
        case 3:
            mmsError = MMS_ERROR_DEFINITION_TYPE_UNSUPPORTED;
            break;
        case 4:
            mmsError = MMS_ERROR_DEFINITION_TYPE_INCONSISTENT;
            break;
        case 5:
            mmsError = MMS_ERROR_DEFINITION_OBJECT_EXISTS;
            break;
        case 6:
            mmsError = MMS_ERROR_DEFINITION_OBJECT_ATTRIBUTE_INCONSISTENT;
            break;
        default:
            mmsError = MMS_ERROR_DEFINITION_OTHER;
            break;
        }
        break;

    case 3: /* class: resource */
        mmsError = MMS_ERROR_RESOURCE_OTHER;
        break;

    case 4: /* class: service */
        mmsError = MMS_ERROR_SERVICE_OTHER;
        break;

    case 5: /* class: service-preempt */
        mmsError = MMS_ERROR_SERVICE_PREEMPT_OTHER;
        break;

    case 6: /* class: time-resolution */
        mmsError = MMS_ERROR_TIME_RESOLUTION_OTHER;
        break;

    case 7: /* class: access */
        switch (serviceError.errorCode)
        {
        case 1:
            mmsError = MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED;
            break;
        case 2:
            mmsError = MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT;
            break;
        case 3:
            mmsError = MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED;
            break;
        case 4:
            mmsError = MMS_ERROR_ACCESS_OBJECT_INVALIDATED;
            break;
        default:
            mmsError = MMS_ERROR_ACCESS_OTHER;
            break;
        }
        break;

    case 11: /* class: file */
        switch (serviceError.errorCode)
        {
        case 1:
            mmsError = MMS_ERROR_FILE_FILENAME_AMBIGUOUS;
            break;
        case 2:
            mmsError = MMS_ERROR_FILE_FILE_BUSY;
            break;
        case 3:
            mmsError = MMS_ERROR_FILE_FILENAME_SYNTAX_ERROR;
            break;
        case 4:
            mmsError = MMS_ERROR_FILE_CONTENT_TYPE_INVALID;
            break;
        case 5:
            mmsError = MMS_ERROR_FILE_POSITION_INVALID;
            break;
        case 6:
            mmsError = MMS_ERROR_FILE_FILE_ACCESS_DENIED;
            break;
        case 7:
            mmsError = MMS_ERROR_FILE_FILE_NON_EXISTENT;
            break;
        case 8:
            mmsError = MMS_ERROR_FILE_DUPLICATE_FILENAME;
            break;
        case 9:
            mmsError = MMS_ERROR_FILE_INSUFFICIENT_SPACE_IN_FILESTORE;
            break;
        default:
            mmsError = MMS_ERROR_FILE_OTHER;
            break;
        }
        break;

    default:
        mmsError = MMS_ERROR_OTHER;
    }

    return mmsError;
}

static void
handleConfirmedErrorPdu(
        MmsEndpointConnection self,
        uint8_t* buffer, int bufPos, int maxBufPos,
        ByteBuffer* response)
{
    uint32_t invokeId;
    MmsServiceError serviceError;

    if (mmsMsg_parseConfirmedErrorPDU(self, buffer, bufPos, maxBufPos, &invokeId, &serviceError)) {

        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_WARNING, self, "MMS_ENDPOINT: Handle confirmed error PDU (invoke-id: %u)", invokeId);

        /* processing outstanding calls */

        MmsOutstandingCall call = checkForOutstandingCall(self, invokeId);

        if (call) {

            MmsError err = convertServiceErrorToMmsError(serviceError);

            if (call->type != MMS_CALL_TYPE_NONE) {
                handleAsyncResponse(self, NULL, 0, call, err, true);
            }
            else {
                if (DEBUG_MMS_ENDPOINT)
                    libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: Handle confirmed error PDU (invoke-id: %u) - unexpected call type", invokeId);
            }
        }
        else {
            if (DEBUG_MMS_ENDPOINT)
                libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: Handle confirmed error PDU (invoke-id: %u) - unexpected message from server", invokeId);

            return;
        }

#if (MMS_OBTAIN_FILE_SERVICE == 1)
        /* check if message is related to an existing file upload task */
        /* TODO is there a conflict between processing outstanding calls and file upload tasks??? */

        int i;
        for (i = 0; i < CONFIG_MMS_SERVER_MAX_GET_FILE_TASKS; i++) {

            if (self->endpoint->fileUploadTasks[i].state != MMS_FILE_UPLOAD_STATE_NOT_USED) {

                if (self->endpoint->fileUploadTasks[i].lastRequestInvokeId == invokeId) {

                    self->endpoint->fileUploadTasks[i].state = MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_SOURCE;
                    return;
                }

            }
        }
#endif /* (MMS_OBTAIN_FILE_SERVICE == 1) */

    }
    else {

        mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_INVALID_PDU, response);

        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: error parsing confirmed error PDU");
    }
}

static void
handleConfirmedResponsePdu(MmsEndpointConnection self, ByteBuffer* message, int bufPos, ByteBuffer* response)
{
    int length;

    uint8_t* buf = message->buffer;

    bufPos = BerDecoder_decodeLength(buf, &length, bufPos, message->size);
    if (bufPos == -1)
        goto exit_with_error;

    if (buf[bufPos++] == 0x02) {
        int invokeIdLength;

        bufPos = BerDecoder_decodeLength(buf, &invokeIdLength, bufPos, message->size);
        if (bufPos == -1)
            goto exit_with_error;

        uint32_t invokeId =
                BerDecoder_decodeUint32(buf, invokeIdLength, bufPos);

        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_DEBUG, self, "MMS_ENDPOINT: rcvd confirmed-response (invoke-id: %u) size: %i",
                    invokeId, message->size);

        bufPos += invokeIdLength;

        MmsOutstandingCall call = checkForOutstandingCall(self, invokeId);

        if (call) {

            if (call->type != MMS_CALL_TYPE_NONE) {
                handleAsyncResponse(self, message, bufPos, call, MMS_ERROR_NONE, true);
            }
            else {

                if (DEBUG_MMS_ENDPOINT)
                    libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: Handle confirmed response PDU (invoke-id: %u) - unexpected call type", invokeId);
            }
        }
        else {

            if (DEBUG_MMS_ENDPOINT)
                libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: Handle confirmed response PDU (invoke-id: %u) - unexpected message from server", invokeId);

            return;
        }
    }
    else
        goto exit_with_error;

    return;

exit_with_error:

    if (DEBUG_MMS_ENDPOINT)
        libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: Handle confirmed response PDU - reject: invalid-pdu");

    mmsMsg_createMmsRejectPdu(NULL, MMS_ERROR_REJECT_INVALID_PDU, response);

    return;
}

void
MmsEndpointConnection_parseMessage(MmsEndpointConnection self, IsoIndication indication,
        ByteBuffer* message, ByteBuffer* response)
{
    uint8_t* buffer = message->buffer;
    uint8_t pduType;
    int bufPos = 0;
    int pduLength;

    if (message->size < 2)
        goto parsing_error;

    pduType = buffer[bufPos++];

    bufPos = BerDecoder_decodeLength(buffer, &pduLength, bufPos, message->size);

    if (bufPos < 0)
        goto parsing_error;

    switch (pduType)
    {
    case 0xa8: /* Initiate request PDU */
        mmsEndpointConnection_handleInitiateRequest(self, buffer, bufPos, bufPos + pduLength, response);
        break;

    case 0xa9: /* initiate response PDU */
        if (indication == ISO_IND_ASSOCIATION_SUCCESS) {
            if (mmsClient_parseInitiateResponse(self, message)) {
                setEndpointState(self->endpoint, MMS_ENDPOINT_STATE_CONNECTED);
            }
            else {
                if (DEBUG_MMS_ENDPOINT)
                    libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: Failed to parse initiate response --> close connection!");

                IsoClientConnection_close(self->isoClient);
                setEndpointState(self->endpoint, MMS_ENDPOINT_STATE_IDLE);
            }
        }
        else {
            setEndpointState(self->endpoint, MMS_ENDPOINT_STATE_IDLE);
        }
        break;

    case 0xa0: /* Confirmed request PDU */

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
        self->endpoint->statAplMessagesRcvd++;
#endif

        if (addToOutstandingServices(self, buffer, bufPos, bufPos + pduLength) == false) {

            if (DEBUG_MMS_ENDPOINT)
                libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: outstanding service limit reached -> reject request");

            /* send reject PDU */
            mmsMsg_createMmsRejectPdu(NULL, MMS_ERROR_REJECT_MAX_OUTSTANDING_SERVICES_EXCEEDED, response);
        }

        break;

    case 0xa1: /* Confirmed response PDU */

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
        self->endpoint->statAplMessagesRcvd++;
#endif

        handleConfirmedResponsePdu(self, message, 1, response);
        break;

    case 0xa2: /* Confirmed error PDU */

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
        self->endpoint->statConfErrorPduRcvd++;
#endif

        handleConfirmedErrorPdu(self, buffer, 0, bufPos + pduLength, response);
        break;

    case 0x8b: /* Conclude request PDU */

        /* TODO block all new user requests? */

        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_INFO, self, "MMS_ENDPOINT: received conclude request");

        mmsEndpointConnection_writeConcludeResponsePdu(response);
        break;

    case 0x8c: /* conclude response PDU */

        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_INFO, self, "MMS_ENDPOINT: received conclude response+");

        /* TODO call conclude handler? */

        IsoClientConnection_release(self->isoClient);

        break;

    case 0x8d:  /* conclude error PDU */

        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_INFO, self, "MMS_ENDPOINT: received conclude response-");

        /* TODO call conclude handler? (MMS_ERROR_CONCLUDE_REJECTED) */

        break;


    case 0xa3: /* Unconfirmed PDU */

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
        self->endpoint->statAplMessagesRcvd++;
#endif
        handleUnconfirmedMmsPdu(self, buffer, 0, bufPos + pduLength);
        break;

    case 0xa4: /* Reject PDU - silently ignore */

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
        self->endpoint->statRejectPduRcvd++;
#endif

        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: received reject PDU");

        break;

    case 0x00: /* indefinite length end tag -> ignore */
        break;

    default:

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
        self->endpoint->statRejectPduSent++;
#endif

        mmsMsg_createMmsRejectPdu(NULL, MMS_ERROR_REJECT_UNKNOWN_PDU_TYPE, response);
        break;
    }

    return;

parsing_error:
    if (DEBUG_MMS_ENDPOINT)
        libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: error parsing message");

    return;
}

static void /* will be called by IsoConnection (part of IsoServer) */
messageReceived(void* parameter, ByteBuffer* message, ByteBuffer* response)
{
    MmsEndpointConnection self = (MmsEndpointConnection) parameter;

    MmsEndpointConnection_parseMessage(self, ISO_IND_IGNORE, message, response);
}

void
MmsEndpointConnection_sendMessage(MmsEndpointConnection self, ByteBuffer* message)
{
#if (CONFIG_MMS_RAW_MESSAGE_LOGGING == 1)
    if (self->endpoint->rawMmsMessageHandler != NULL) {
        MmsRawMessageHandler handler = (MmsRawMessageHandler) self->endpoint->rawMmsMessageHandler;
        handler(self->endpoint->rawMmsMessageHandlerParameter, message->buffer, message->size, false);
    }
#endif /* (CONFIG_MMS_RAW_MESSAGE_LOGGING == 1) */

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
    self->endpoint->statAplMessagesSent++;
#endif

    if (self->endpoint->passiveMode) {
        IsoConnection_sendMessage(self->isoConnection, message);
    }
    else {
        IsoClientConnection_sendMessage(self->isoClient, message);
    }
}

void
MmsEndpointConnection_sendRejectPdu(MmsEndpointConnection self, uint32_t* invokeId, int reason)
{
    printf("SEND REJECT PDU %i\n", reason);

    ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

    if (response) {
        mmsMsg_createMmsRejectPdu(invokeId, reason, response);

        MmsEndpointConnection_sendMessage(self, response);

        MmsEndpoint_releaseTransmitBuffer(self->endpoint);
    }
}

void
MmsEndpointConnection_sendServiceErrorPdu(MmsEndpointConnection self, uint32_t invokeId, MmsError err)
{
    ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

    if (response) {
        mmsMsg_createServiceErrorPdu(invokeId, response, err);

        MmsEndpointConnection_sendMessage(self, response);

        MmsEndpoint_releaseTransmitBuffer(self->endpoint);
    }
}

/* INTERNAL */
static MmsEndpointConnection
MmsEndpointConnection_create(MmsEndpoint endpoint)
{
    MmsEndpointConnection self = (MmsEndpointConnection) GLOBAL_CALLOC(1, sizeof(struct sMmsEndpointConnection));

    if (self) {
        self->endpoint = endpoint;
        self->peerIpAddress = NULL;
        self->lastRequestInvokeId = 0;
        self->lastRequestInvokeIdLock = Semaphore_create(1);

        self->maxServOutstandingCalled = endpoint->mmsConnectionParameters.maxServOutstandingCalled;
        self->maxServOutstandingCalling = endpoint->mmsConnectionParameters.maxServOutstandingCalling;

        if ((endpoint->mmsConnectionParameters.maxPduSize > 0) && (endpoint->mmsConnectionParameters.maxPduSize <= CONFIG_MMS_MAXIMUM_PDU_SIZE))
            self->maxPduSize = endpoint->mmsConnectionParameters.maxPduSize;
        else
            self->maxPduSize = CONFIG_MMS_MAXIMUM_PDU_SIZE;

        self->dataStructureNestingLevel = endpoint->mmsConnectionParameters.dataStructureNestingLevel;

        self->outstandingCallsLock = Semaphore_create(1);
        self->outstandingCalls = (MmsOutstandingCall) GLOBAL_CALLOC(OUTSTANDING_CALLS, sizeof(struct sMmsOutstandingCall));

        self->outstandingServicesLock = Semaphore_create(1);
        self->outstandingServices = (MmsOutstandingService) GLOBAL_CALLOC(OUTSTANDING_CALLS, sizeof (struct sMmsOutstandingService));
        self->oldestOutstandingService = -1;
        self->nextOutstandingService = -1;

        if (DEBUG_MMS_ENDPOINT)
            libmms_log2(MMS_LOG_DEBUG, endpoint, "MMS_ENDPOINT: Create new connection with %i buffers for max PDU size: %i", self->maxServOutstandingCalled, self->maxPduSize);

        int i;

        for (i = 0; i < OUTSTANDING_CALLS; i++) {
            self->outstandingServices[i].mmsMsgBuffer = (uint8_t*) GLOBAL_MALLOC(self->maxPduSize);
        }

        self->isoConnection = NULL;
        self->isoClient = NULL;

        self->serverConnection = NULL;
        self->clientConnection = NULL;
    }

    return self;
}

static void
terminateAllOutstandingCalls(MmsEndpointConnection self)
{
    Semaphore_wait(self->outstandingCallsLock);

    int i;

    for (i = 0; i < OUTSTANDING_CALLS; i++) {
        if (self->outstandingCalls[i].isUsed) {

                if (self->outstandingCalls[i].type != MMS_CALL_TYPE_NONE)
                    handleAsyncResponse(self, NULL, 0, &(self->outstandingCalls[i]), MMS_ERROR_CONNECTION_LOST, false);

                self->outstandingCalls[i].isUsed = false;
        }
    }

    Semaphore_post(self->outstandingCallsLock);
}

/* INTERNAL */
static void
MmsEndpointConnection_destroy(MmsEndpointConnection self)
{
    terminateAllOutstandingCalls(self);

#if (MMS_FILE_SERVICE == 1)
    int frsmIndex = 0;

    for (frsmIndex = 0; frsmIndex < CONFIG_MMS_MAX_NUMBER_OF_OPEN_FILES_PER_CONNECTION; frsmIndex++)
        if (self->frsms[frsmIndex].fileHandle != NULL)
            FileSystem_closeFile(self->frsms[frsmIndex].fileHandle);
#endif

    Semaphore_destroy(self->lastRequestInvokeIdLock);

    if (self->clientConnection) {
        GLOBAL_FREEMEM(self->clientConnection);
    }

    int i;

    if (self->outstandingServices) {
        for (i = 0; i < OUTSTANDING_CALLS; i++) {
            GLOBAL_FREEMEM(self->outstandingServices[i].mmsMsgBuffer);
        }

        GLOBAL_FREEMEM(self->outstandingServices);
    }

    if (self->serverConnection) {

#if (MMS_DYNAMIC_DATA_SETS == 1)
        LinkedList_destroyDeep(self->serverConnection->namedVariableLists, (LinkedListValueDeleteFunction) MmsNamedVariableList_destroy);
#endif

        GLOBAL_FREEMEM(self->serverConnection);
    }

    Semaphore_destroy(self->outstandingCallsLock);
    Semaphore_destroy(self->outstandingServicesLock);

    if (self->peerIpAddress)
        GLOBAL_FREEMEM(self->peerIpAddress);

    GLOBAL_FREEMEM(self->outstandingCalls);

    GLOBAL_FREEMEM(self);
}

/* INTERNAL */
static void
MmsEndpointConnection_initializeAsTcpServer(MmsEndpointConnection self, IsoConnection isoConnection)
{
    self->isoConnection = isoConnection;

    IsoConnection_installListener(isoConnection, messageReceived, (void*) self);
}

/* INTERNAL */
void
MmsEndpointConnection_setServer(MmsEndpointConnection self, MmsServer server)
{
    self->serverConnection = (MmsEndpointServerConnection) GLOBAL_MALLOC(sizeof(struct sMmsEndpointServerConnection));

    if (self->serverConnection) {

#if (MMS_DYNAMIC_DATA_SETS == 1)
        self->serverConnection->namedVariableLists = LinkedList_create();
#endif

        self->serverConnection->server = server;
    }
}

char*
MmsEndpointConnection_getPeerAddress(MmsEndpointConnection self)
{
    if (self->peerIpAddress == NULL) {
        if (self->isoConnection)
            self->peerIpAddress = StringUtils_copyString(IsoConnection_getPeerAddress(self->isoConnection));
        else if (self->isoClient)
            self->peerIpAddress = IsoClientConnection_getPeerAddress(self->isoClient);
    }

    return self->peerIpAddress;
}

int
MmsEndpointConnection_getMaxPduSize(MmsEndpointConnection self)
{
    return self->maxPduSize;
}

IsoApplicationAddress
MmsEndpointConnection_getPeerApplicationAddress(MmsEndpointConnection self)
{
    if (self->isoClient) {
        return IsoClientConnection_getPeerApplicationAddress(self->isoClient);
    }
    else if (self->isoConnection) {
        return IsoConnection_getPeerApplicationAddress(self->isoConnection);
    }
    else
        return NULL;
}

IsoApplicationAddress
MmsEndpointConnection_getCalledApplicationAddress(MmsEndpointConnection self)
{
    if (self->isoConnection) {
        return IsoConnection_getCalledApplicationAddress(self->isoConnection);
    }
    else
        return NULL;
}

void*
MmsEndpointConnection_getSecurityToken(MmsEndpointConnection self)
{
    if (self->isoClient) {
        return IsoClientConnection_getSecurityToken(self->isoClient);
    }
    else if (self->isoConnection) {
        return IsoConnection_getSecurityToken(self->isoConnection);
    }
    else
        return NULL;
}

/*
 * Perform periodic tasks for the connection
 */
void
MmsEndpointConnection_handleAppTick(MmsEndpointConnection self)
{
    /* check timeouts for outstanding calls (client side) */

    uint64_t currentTime = Hal_getTimeInMs();

    int i = 0;

    Semaphore_wait(self->outstandingCallsLock);

    for (i = 0; i < OUTSTANDING_CALLS; i++) {
        if (self->outstandingCalls[i].isUsed) {
            if (currentTime > self->outstandingCalls[i].timeout) {

                if (self->outstandingCalls[i].type != MMS_CALL_TYPE_NONE) {
                    libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: timeout waiting for confirmed response (invoke-ID: %u, request-type: %i)",
                            self->outstandingCalls[i].invokeId,
                            self->outstandingCalls[i].type);

                    handleAsyncResponse(self, NULL, 0, &(self->outstandingCalls[i]), MMS_ERROR_SERVICE_TIMEOUT, false);
                }

                self->outstandingCalls[i].isUsed = false;
                break;
            }
        }
    }

    Semaphore_post(self->outstandingCallsLock);

    /* handle outstanding service requests (server side) */

    int maxOutstandingServices = OUTSTANDING_CALLS;

    Semaphore_wait(self->outstandingServicesLock);

    if (self->oldestOutstandingService != -1) {

        do {
            MmsOutstandingService service = &(self->outstandingServices[self->oldestOutstandingService]);
            
            MmsEndpointConnection_handleConfirmedRequestPdu(self, service->mmsMsgBuffer, service->bufPos, service->mmsMsgSize);

            self->oldestOutstandingService++;

            if (self->oldestOutstandingService == maxOutstandingServices)
                self->oldestOutstandingService = 0;

        } while (self->oldestOutstandingService != self->nextOutstandingService);

        self->nextOutstandingService = -1;
        self->oldestOutstandingService = -1;
    }

    Semaphore_post(self->outstandingServicesLock);

    /* TODO check for conclude timeout and call conclude handler (MMS_ERROR_SERVICE_TIMEOUT) */
}

static void
internalAppTickHandler (void* parameter)
{
    MmsEndpoint self = (MmsEndpoint) parameter;

    Semaphore_wait(self->appTickLock);

    uint64_t currentTime = Hal_getTimeInMs();

    /* call app tick handlers for all MmsEndpointConnection objects */

    Semaphore_wait(self->openConnectionsLock);

    LinkedList element = LinkedList_getNext(self->openConnections);

    while (element) {
        MmsEndpointConnection connection = (MmsEndpointConnection) LinkedList_getData(element);

        MmsEndpointConnection_handleAppTick(connection);

        element = LinkedList_getNext(element);
    }

    Semaphore_post(self->openConnectionsLock);

    /* Check for wait handles timeout */
    Semaphore_wait(self->waitHandlesLock);

    element = LinkedList_getNext(self->waitHandles);

    while (element) {

        WaitHandle handle = (WaitHandle) LinkedList_getData(element);

        if (handle->timeout <= currentTime) {
            Semaphore_post(handle->waitLock);
        }

        element = LinkedList_getNext(element);
    }

    Semaphore_post(self->waitHandlesLock);

    /* Call user layer app tick handler */
    if (self->appTickHandler) {
        self->appTickHandler(self->appTickHandlerParameter);
    }

    Semaphore_post(self->appTickLock);
}

/* called by ISO client (in active mode) */
static void
mmsIsoClientCallback(IsoIndication indication, void* parameter, ByteBuffer* payload)
{
    MmsEndpoint self = (MmsEndpoint) parameter;

    if (DEBUG_MMS_ENDPOINT) {
        if (indication != ISO_IND_TICK) {

            char* msgStr;

            switch (indication) {
            case ISO_IND_ASSOCIATION_SUCCESS:
                msgStr = "ISO association success";
                break;
            case ISO_IND_ASSOCIATION_FAILED:
                msgStr = "ISO association failed";
                break;
            case ISO_IND_CLOSED:
                msgStr = "ISO connection closed";
                break;
            case ISO_IND_DATA:
                msgStr = "ISO data";
                break;
            case ISO_IND_IGNORE:
                msgStr = "ISO ignore";
                break;

            default:
                msgStr = "ISO - unknown event";
            }

            libmms_log2(MMS_LOG_DEBUG, self, "MMS_ENDPOINT: %s", msgStr);
        }
    }

    if (indication == ISO_IND_TICK) {
        internalAppTickHandler(parameter);

        return;
    }

    if (indication == ISO_IND_CLOSED) {

        setEndpointState(self, MMS_ENDPOINT_STATE_IDLE);

        /* lookup MmsEndpointConnection */
        MmsEndpointConnection connection = getMatchingOpenTcpClientConnection(self, self->isoClient);

        if (connection != NULL) {

            if (DEBUG_MMS_ENDPOINT)
                libmms_log(MMS_LOG_WARNING, connection, "MMS_ENDPOINT: connection lost or closed by client");

            Semaphore_wait(self->openConnectionsLock);
            LinkedList_remove(self->openConnections, connection);
            Semaphore_post(self->openConnectionsLock);

            /* Call user provided callback function */
            if (self->connectionHandler)
                self->connectionHandler(self->connectionHandlerParameter, connection, MMS_ENDPOINT_EVENT_CONNECTION_CLOSED);

            MmsEndpointConnection_destroy(connection);
        }
        else {
            if (DEBUG_MMS_ENDPOINT)
                libmms_log2(MMS_LOG_ERROR, self, "MMS_ENDPOINT: connection not found");
        }

        return;
    }

    if (indication == ISO_IND_ASSOCIATION_FAILED) {
        if (DEBUG_MMS_ENDPOINT)
            libmms_log2(MMS_LOG_WARNING, self, "MMS_ENDPOINT: association failed");

        setEndpointState(self, MMS_ENDPOINT_STATE_ERROR);

        return;
    }

    if (indication == ISO_IND_ASSOCIATION_SUCCESS) {
        MmsEndpointConnection mmsCon = getMatchingOpenTcpClientConnection(self, self->isoClient);

        /* Call user provided callback function */
        if (self->connectionHandler)
            self->connectionHandler(self->connectionHandlerParameter, mmsCon, MMS_ENDPOINT_EVENT_NEW_CONNECTION);

    }

    if (payload != NULL) {
        if (ByteBuffer_getSize(payload) < 1) {

            if (DEBUG_MMS_ENDPOINT)
                libmms_log2(MMS_LOG_ERROR, self, "MMS_ENDPOINT: ISO payload size 0");

            return;
        }
    }
    else {
        if (DEBUG_MMS_ENDPOINT)
            libmms_log2(MMS_LOG_ERROR, self, "MMS_ENDPOINT: no ISO payload!");
    }

    uint8_t* buf = ByteBuffer_getBuffer(payload);

#if (CONFIG_MMS_RAW_MESSAGE_LOGGING == 1)
    if (self->rawMmsMessageHandler != NULL) {
        MmsRawMessageHandler handler = (MmsRawMessageHandler) self->rawMmsMessageHandler;
        handler(self->rawMmsMessageHandlerParameter, buf, payload->size, true);
    }
#endif /* (CONFIG_MMS_RAW_MESSAGE_LOGGING == 1) */

    /* lookup matching endpoint connection - only one can be there */

    MmsEndpointConnection mmsCon = NULL;

    Semaphore_wait(self->openConnectionsLock);

    LinkedList listElement = LinkedList_getNext(self->openConnections);
    mmsCon = (MmsEndpointConnection) LinkedList_getData(listElement);

    Semaphore_post(self->openConnectionsLock);


    if (mmsCon) {

        /* setup buffer for response */
        ByteBuffer* response = IsoClientConnection_allocateTransmitBuffer(self->isoClient);

        MmsEndpointConnection_parseMessage(mmsCon, indication, payload, response);

        if (response->size > 0) {
            IsoClientConnection_sendMessage(self->isoClient, response);
        }

        IsoClientConnection_releaseTransmitBuffer(self->isoClient);
    }
    else {
        if (DEBUG_MMS_ENDPOINT)
            libmms_log2(MMS_LOG_ERROR, self, "MMS_ENDPOINT: no open connection found!");
    }
}

static void*
isoClientConnectionHandlingThread(void* parameter)
{
    MmsEndpoint self = (MmsEndpoint) parameter;

    while (self->connectionThreadRunning) {

        if (IsoClientConnection_handleConnection(self->isoClient))
            Thread_sleep(10);

    }

    return NULL;
}

MmsEndpoint
MmsEndpoint_create(TLSConfiguration tlsConfiguration, bool passive)
{
    MmsEndpoint self = (MmsEndpoint) GLOBAL_CALLOC(1, sizeof(struct sMmsEndpoint));

    if (self) {
        self->passiveMode = passive;

        self->tlsConfig = tlsConfiguration;

        self->logHandler = NULL;
        self->logHandlerParameter = NULL;
        self->logLevel = MMS_LOG_NONE;

        self->waitHandles = LinkedList_create();
        self->waitHandlesLock = Semaphore_create(1);

        self->mmsConnectionParameters.maxServOutstandingCalling = 10;
        self->mmsConnectionParameters.maxServOutstandingCalled = 10;
        self->mmsConnectionParameters.dataStructureNestingLevel = 8;
        self->mmsConnectionParameters.maxPduSize = CONFIG_MMS_MAXIMUM_PDU_SIZE;

        self->mmsClients = NULL;

        self->requestTimeout = CONFIG_MMS_CONNECTION_DEFAULT_TIMEOUT;
        self->connectTimeout = CONFIG_MMS_CONNECTION_DEFAULT_CONNECT_TIMEOUT;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
        self->stateLock = Semaphore_create(1);
#endif
        self->state = MMS_ENDPOINT_STATE_IDLE;

        self->transmitBuffer = ByteBuffer_create(NULL, CONFIG_MMS_MAXIMUM_PDU_SIZE);
        self->transmitBufferMutex = Semaphore_create(1);

        self->reportHandlerMutex = Semaphore_create(1);

        self->reportHandler = NULL;
        self->reportHandlerParameter = NULL;

        self->stateChangedHandler = NULL;
        self->stateChangedHandlerParameter = NULL;

        self->connectionHandler = NULL;
        self->connectionHandlerParameter = NULL;

        self->rawMmsMessageHandler = NULL;
        self->rawMmsMessageHandlerParameter = NULL;

        self->appTickHandler = NULL;
        self->appTickHandlerParameter = NULL;

        self->openConnectionsLock = Semaphore_create(1);
        self->openConnections = LinkedList_create();

        self->appTickEnabled = false;
        self->appTickLock = Semaphore_create(1);

        /* Load default values for connection parameters */
        TSelector tSelector = { 2, { 0, 1 } };
        SSelector sSelector = { 2, { 0, 1 } };
        PSelector pSelector = { 4, { 0, 0, 0, 1 } };

        IsoApplicationAddress_setLowerLayerAddresses(&(self->localAppAddr), pSelector, sSelector, tSelector);
        IsoApplicationAddress_setApTitle(&(self->localAppAddr), "1.1.1.999");
        IsoApplicationAddress_setAeQualifier(&(self->localAppAddr), 12);

        IsoApplicationAddress_setLowerLayerAddresses(&(self->defaultRemoteAppAddr), pSelector, sSelector, tSelector);
        IsoApplicationAddress_setApTitle(&(self->defaultRemoteAppAddr), "1.1.1.999.1");
        IsoApplicationAddress_setAeQualifier(&(self->defaultRemoteAppAddr), 12);

        self->defaultRemoteHostname = StringUtils_copyString("localhost");

        if (passive) {
            self->isoServer = IsoServer_create(tlsConfiguration, &(self->localAppAddr));
            IsoServer_setAppTickHandler(self->isoServer, internalAppTickHandler, self);
            self->isoClient = NULL;
            self->createThread = false;
        }
        else {
            self->isoServer = NULL;

            self->createThread = true;
            self->connectionHandlingThread = NULL;
            self->connectionThreadRunning = false;

            if (tlsConfiguration) {
                self->defaultRemoteTcpPort = 3782;
            }
            else
                self->defaultRemoteTcpPort = 102;

            self->isoClient = IsoClientConnection_create(tlsConfiguration, &(self->localAppAddr), &(self->defaultRemoteAppAddr), (IsoIndicationCallback) mmsIsoClientCallback, (void*) self);
        }

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
        self->statAplMessagesSent = 0;
        self->statAplMessagesRcvd = 0;
        self->statConfErrorPduRcvd = 0;
        self->statConfErrorPduSend = 0;
        self->statRejectPduRcvd = 0;
        self->statRejectPduSent = 0;
        self->statRequestTimeouts = 0;
#endif
    }

    return self;
}

static void
closeConnection(void* con)
{
    MmsEndpointConnection connection = (MmsEndpointConnection) con;

    MmsEndpointConnection_destroy(connection);
}

static void
WaitHandle_destroy(WaitHandle self)
{
    Semaphore_destroy(self->waitLock);
    GLOBAL_FREEMEM(self);
}

static WaitHandle
addWaitHandle(MmsEndpoint self, MmsEndpointState state, int timeoutInMs)
{
    WaitHandle handle = (WaitHandle) GLOBAL_MALLOC(sizeof(struct sWaitHandle));

    if (handle) {
        handle->waitLock = Semaphore_create(1);
        handle->timeout = Hal_getTimeInMs() + timeoutInMs;
        handle->state = state;
        handle->success = false;

        Semaphore_wait(handle->waitLock);

        Semaphore_wait(self->waitHandlesLock);
        LinkedList_add(self->waitHandles, handle);
        Semaphore_post(self->waitHandlesLock);
    }

    return handle;
}

static bool
waitForState(MmsEndpoint self, WaitHandle handle)
{
    Semaphore_wait(handle->waitLock);

    bool success = handle->success;

    Semaphore_wait(self->waitHandlesLock);
    LinkedList_remove(self->waitHandles, handle);
    Semaphore_post(self->waitHandlesLock);

    WaitHandle_destroy(handle);

    return success;
}

static void
timeoutAllWaitHandles(MmsEndpoint self)
{
    Semaphore_wait(self->waitHandlesLock);

    LinkedList element = LinkedList_getNext(self->waitHandles);

    while (element) {
        WaitHandle handle = (WaitHandle) LinkedList_getData(element);

        handle->timeout = 0;

        element = LinkedList_getNext(element);
    }

    Semaphore_post(self->waitHandlesLock);
}

static bool
isWaitHandlesListEmpty(MmsEndpoint self)
{
    bool isEmpty = false;

    Semaphore_wait(self->waitHandlesLock);

    if (LinkedList_size(self->waitHandles) == 0)
        return true;

    Semaphore_post(self->waitHandlesLock);

    return isEmpty;
}

void
MmsEndpoint_destroy(MmsEndpoint self)
{
    ByteBuffer_destroy(self->transmitBuffer);

    Semaphore_destroy(self->transmitBufferMutex);
    Semaphore_destroy(self->reportHandlerMutex);
    Semaphore_destroy(self->stateLock);

    if (self->passiveMode) {
        IsoServer_destroy(self->isoServer);
        LinkedList_destroyDeep(self->openConnections, (LinkedListValueDeleteFunction) closeConnection);
    }
    else {

        if (self->connectionThreadRunning) {
            self->connectionThreadRunning = false;
            Thread_destroy(self->connectionHandlingThread);
        }

        IsoClientConnection_destroy(self->isoClient);

        LinkedList_destroyDeep(self->openConnections, (LinkedListValueDeleteFunction) closeConnection);
    }

    if (self->mmsClients)
        LinkedList_destroyStatic(self->mmsClients);

#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    if (self->filestoreBasepath != NULL)
        GLOBAL_FREEMEM(self->filestoreBasepath);
#endif

    timeoutAllWaitHandles(self);

    /* wait for list of wait handles to be empty */
    while (isWaitHandlesListEmpty(self) == false)
        Thread_sleep(10);

    LinkedList_destroyDeep(self->waitHandles, (LinkedListValueDeleteFunction) WaitHandle_destroy);
    Semaphore_destroy(self->waitHandlesLock);

    Semaphore_destroy(self->openConnectionsLock);

    Semaphore_destroy(self->appTickLock);

    GLOBAL_FREEMEM(self->defaultRemoteHostname);

    GLOBAL_FREEMEM(self);
}

void
MmsEndpoint_setLogHandler(MmsEndpoint self, MmsEndpointLogHandler handler, void* parameter)
{
    self->logHandler = handler;
    self->logHandlerParameter = parameter;
}

void
MmsEndpoint_setLogLevel(MmsEndpoint self, MmsLogLevel logLevel)
{
    self->logLevel = logLevel;
}

void
MmsEndpoint_setTLSConfiguration(MmsEndpoint self, TLSConfiguration tlsConfig)
{
    if (self->isoServer) {
        IsoServer_setTLSConfiguration(self->isoServer, tlsConfig);
    }
    else if (self->isoClient) {
        IsoClientConnection_setTLSConfiguration(self->isoClient, tlsConfig);
    }
}

bool
MmsEndpoint_waitForState(MmsEndpoint self, MmsEndpointState state, int timeoutInMs)
{
    MmsEndpointState currentState = getEndpointState(self);

    if (currentState == state)
        return true;

    WaitHandle waitHandle = NULL;

    Semaphore_wait(self->appTickLock);

    if (self->appTickEnabled)
        waitHandle = addWaitHandle(self, state, timeoutInMs);

    Semaphore_post(self->appTickLock);

    if (waitHandle)
        return waitForState(self, waitHandle);
    else {
        /* active wait loop */

        uint64_t currentTime = Hal_getTimeInMs();

        uint64_t timeout = currentTime + timeoutInMs;

        while (currentTime < timeout) {

            MmsEndpointState newState = getEndpointState(self);

            if (newState == MMS_ENDPOINT_STATE_ERROR)
                return false;

            if (newState == state)
                return true;

            currentTime = Hal_getTimeInMs();
        }

        return false;
    }
}

MmsEndpointState
MmsEndpoint_getState(MmsEndpoint self)
{
    return getEndpointState(self);
}

MmsConnectionParameters*
MmsEndpoint_getConnectionParameters(MmsEndpoint self)
{
    return &(self->mmsConnectionParameters);
}

void
MmsEndpoint_addClient(MmsEndpoint self, MmsClient client)
{
    if (self->mmsClients == NULL)
        self->mmsClients = LinkedList_create();

    client->endpoint = self;

    LinkedList_add(self->mmsClients, client);
}

void
MmsEndpoint_setRequestTimeout(MmsEndpoint self, uint32_t timeoutInMs)
{
    self->requestTimeout = timeoutInMs;
}

void
MmsEndpoint_setConnectTimeout(MmsEndpoint self, uint32_t timeoutInMs)
{
    self->connectTimeout = timeoutInMs;
}

ByteBuffer* /* INTERNAL */
MmsEndpoint_reserveTransmitBuffer(MmsEndpoint self)
{
#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_wait(self->transmitBufferMutex);
#endif

    return self->transmitBuffer;
}

void /* INTERNAL */
MmsEndpoint_releaseTransmitBuffer(MmsEndpoint self)
{
    self->transmitBuffer->size = 0;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore_post(self->transmitBufferMutex);
#endif
}

void
MmsEndpoint_installAppTickHandler(MmsEndpoint self, MmsAppTickHandler handler, void* parameter)
{
    self->appTickHandler = handler;
    self->appTickHandlerParameter = parameter;
}

int
MmsEndpoint_getNumberOfActiveConnections(MmsEndpoint self)
{
    int connectionCount = 0;

    Semaphore_wait(self->openConnectionsLock);

    if (self->passiveMode) {
        if (self->openConnections != NULL) {
            connectionCount = LinkedList_size(self->openConnections);
        }
    }
    else {
        if (self->state == MMS_ENDPOINT_STATE_CONNECTED)
            connectionCount = 1;
    }

    Semaphore_post(self->openConnectionsLock);

    return connectionCount;
}

/* only for passive mode */
void
MmsEndpoint_setLocalTcpPort(MmsEndpoint self, int localTcpPort)
{
    if (self->passiveMode) {
        IsoServer_setTcpPort(self->isoServer, localTcpPort);
    }
    else {
        /* nothing to do here */
    }
}

void
MmsEndpoint_setLocalIpAddress(MmsEndpoint self, const char* localIpAddress)
{
    if (self->passiveMode) {
        IsoServer_setLocalIpAddress(self->isoServer, localIpAddress);
    }
    else {
        /* nothing to do here */
    }
}

void
MmsEndpoint_setLocalApplicationAddress(MmsEndpoint self, IsoApplicationAddress localAddress)
{
    memcpy(&(self->localAppAddr), localAddress, sizeof(struct sIsoApplicationAddress));
}

IsoApplicationAddress
MmsEndpoint_getLocalApplicationAddress(MmsEndpoint self)
{
    return &(self->localAppAddr);
}

void
MmsEndpoint_setRemoteTcpPort(MmsEndpoint self, int remoteTcpPort)
{
    if (self->passiveMode) {
        if (DEBUG_MMS_ENDPOINT)
            libmms_log2(MMS_LOG_WARNING, self, "MMS_ENDPOINT: ignore setting remote TCP port in passive mode");
    }
    else {
        self->defaultRemoteTcpPort = remoteTcpPort;
    }
}

void
MmsEndpoint_setRemoteIpAddress(MmsEndpoint self, const char* remoteIpAddress)
{
    if (self->passiveMode) {
        if (DEBUG_MMS_ENDPOINT)
            libmms_log2(MMS_LOG_WARNING, self, "MMS_ENDPOINT: ignore setting remote IP address in passive mode");
    }
    else {
        GLOBAL_FREEMEM(self->defaultRemoteHostname);

        self->defaultRemoteHostname = StringUtils_copyString(remoteIpAddress);
    }
}

void
MmsEndpoint_setRemoteApplicationAddress(MmsEndpoint self, IsoApplicationAddress remoteAddress)
{
    memcpy(&(self->defaultRemoteAppAddr), remoteAddress, sizeof(struct sIsoApplicationAddress));
}

IsoApplicationAddress
MmsEndpoint_getRemoteApplicationAddress(MmsEndpoint self)
{
    return &(self->defaultRemoteAppAddr);
}

void
MmsEndpoint_setRawMessageHandler(MmsEndpoint self, MmsRawMessageHandler handler, void* parameter)
{
    self->rawMmsMessageHandler = (void*) handler;
    self->rawMmsMessageHandlerParameter = parameter;
}

void
MmsEndpoint_setEndpointStateChangedHandler(MmsEndpoint self, MmsEndpointStateChangedHandler handler, void* parameter)
{
    self->stateChangedHandler = handler;
    self->stateChangedHandlerParameter = parameter;
}

void
MmsEndpoint_installConnectionHandler(MmsEndpoint self, MmsConnectionHandler handler,
        void* parameter)
{
    self->connectionHandler = handler;
    self->connectionHandlerParameter = parameter;
}

#if (MMS_FILE_SERVICE == 1)
void
MmsEndpoint_installFileAccessHandler(MmsEndpoint self, MmsFileAccessHandler handler, void* parameter)
{
    self->fileAccessHandler = handler;
    self->fileAccessHandlerParameter = parameter;
}
#endif /* (MMS_FILE_SERVICE == 1) */

#if (MMS_OBTAIN_FILE_SERVICE == 1)
void
MmsEndpoint_installObtainFileHandler(MmsEndpoint self, MmsObtainFileHandler handler, void* parameter)
{
    self->obtainFileHandler = handler;
    self->obtainFileHandlerParameter = parameter;
}

void
MmsEndpoint_installGetFileCompleteHandler(MmsEndpoint self, MmsGetFileCompleteHandler handler, void* parameter)
{
    self->getFileCompleteHandler = handler;
    self->getFileCompleteHandlerParameter = parameter;
}
#endif /* (MMS_OBTAIN_FILE_SERVICE == 1) */

void
MmsEndpoint_setFilestoreBasepath(MmsEndpoint self, const char* basepath)
{
#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    if (self->filestoreBasepath != NULL) {
        GLOBAL_FREEMEM(self->filestoreBasepath);
        self->filestoreBasepath = NULL;
    }

    if (basepath != NULL)
        self->filestoreBasepath = StringUtils_copyString(basepath);
#endif
}

const char*
MmsEndpoint_getFilestoreBasepath(MmsEndpoint self)
{
#if (CONFIG_SET_FILESTORE_BASEPATH_AT_RUNTIME == 1)
    if (self->filestoreBasepath != NULL)
        return self->filestoreBasepath;
    else
        return CONFIG_VIRTUAL_FILESTORE_BASEPATH;
#else
    return CONFIG_VIRTUAL_FILESTORE_BASEPATH;
#endif
}

static void /* will be called by ISO server stack */
isoServerConnectionIndicationHandler(IsoConnectionIndication indication,
        void* parameter, IsoConnection connection)
{
    MmsEndpoint self = (MmsEndpoint) parameter;

    if (indication == ISO_CONNECTION_OPENED) {
        MmsEndpointConnection mmsCon = MmsEndpointConnection_create(self);

        if (mmsCon) {

            MmsEndpointConnection_initializeAsTcpServer(mmsCon, connection);

            if (DEBUG_MMS_ENDPOINT)
                libmms_log(MMS_LOG_INFO, mmsCon, "MMS_ENDPOINT: new connection");

            bool success = false;

            Semaphore_wait(self->openConnectionsLock);
            success = LinkedList_addEx(self->openConnections, mmsCon);
            Semaphore_post(self->openConnectionsLock);

            if (success == false) {
                libmms_log(MMS_LOG_ERROR, NULL, "MMS_ENDPOINT: out of memory");
            }
        }
    }
    else if (indication == ISO_CONNECTION_READY) {
        MmsEndpointConnection mmsCon = getMatchingOpenTcpServerConnection(self, connection);

        /* call user application */
        if (self->connectionHandler != NULL)
            self->connectionHandler(self->connectionHandlerParameter,
                    mmsCon, MMS_ENDPOINT_EVENT_NEW_CONNECTION);
    }
    else if (indication == ISO_CONNECTION_CLOSED) {
        MmsEndpointConnection mmsCon = getMatchingOpenTcpServerConnection(self, connection);

        if (mmsCon) {

            /* call user application */
            if (self->connectionHandler != NULL)
                self->connectionHandler(self->connectionHandlerParameter,
                        mmsCon, MMS_ENDPOINT_EVENT_CONNECTION_CLOSED);

            Semaphore_wait(self->openConnectionsLock);
            LinkedList_remove(self->openConnections, mmsCon);
            Semaphore_post(self->openConnectionsLock);

            MmsEndpointConnection_destroy(mmsCon);
        }
    }
}

void
MmsEndpoint_connect(MmsEndpoint self)
{
    if (self->passiveMode) {
        IsoServer_setConnectionHandler(self->isoServer, isoServerConnectionIndicationHandler, (void*) self);
        IsoServer_startListening(self->isoServer);

        if (IsoServer_getState(self->isoServer) == ISO_SVR_STATE_RUNNING) {

            setEndpointState(self, MMS_ENDPOINT_STATE_LISTENING);

            Semaphore_wait(self->appTickLock);
            self->appTickEnabled = true;
            Semaphore_post(self->appTickLock);
        }
        else {
            setEndpointState(self, MMS_ENDPOINT_STATE_ERROR);
        }

    }
    else {

        bool isAlreadyConnectedOrConnecting = false;

        Semaphore_wait(self->openConnectionsLock);
        if (LinkedList_size(self->openConnections) > 0)
            isAlreadyConnectedOrConnecting = true;
        Semaphore_post(self->openConnectionsLock);

        if (isAlreadyConnectedOrConnecting) {
            if (DEBUG_MMS_ENDPOINT)
                libmms_log2(MMS_LOG_WARNING, self, "MMS_ENDPOINT: already connecting or connected (state=%i)", getEndpointState(self));
        }
        else {
            MmsEndpointConnection mmsCon = MmsEndpointConnection_create(self);
            mmsCon->isoClient = self->isoClient;

            Semaphore_wait(self->openConnectionsLock);
            LinkedList_add(self->openConnections, mmsCon);
            Semaphore_post(self->openConnectionsLock);

            ByteBuffer* payload = IsoClientConnection_allocateTransmitBuffer(self->isoClient);

            mmsClient_createInitiateRequest(mmsCon, payload);

            setEndpointState(self, MMS_ENDPOINT_STATE_CONNECTING);

            if (IsoClientConnection_associateAsync(self->isoClient, self->defaultRemoteHostname, self->defaultRemoteTcpPort,
                    self->connectTimeout, self->requestTimeout))
            {
                if (DEBUG_MMS_ENDPOINT)
                    libmms_log2(MMS_LOG_INFO, self, "MMS_ENDPOINT: new connection started");

                Semaphore_wait(self->appTickLock);
                self->appTickEnabled = true;
                Semaphore_post(self->appTickLock);
            }
            else {
                if (DEBUG_MMS_ENDPOINT)
                    libmms_log2(MMS_LOG_ERROR, self, "MMS_ENDPOINT: connection startup failed!");

                setEndpointState(self, MMS_ENDPOINT_STATE_ERROR);
            }

            if (self->createThread) {
                if (self->connectionHandlingThread == NULL) {
                    self->connectionHandlingThread = Thread_create(isoClientConnectionHandlingThread, self, false);
                    self->connectionThreadRunning = true;
                    Thread_start(self->connectionHandlingThread);
                }
            }
        }
    }
}

void
MmsEndpoint_disconnect(MmsEndpoint self, int disconnectMode)
{
    if (self->passiveMode) {
        /* TODO implement different modes!!! */
        IsoServer_stopListening(self->isoServer);

        setEndpointState(self, MMS_ENDPOINT_STATE_IDLE);

        timeoutAllWaitHandles(self);

        internalAppTickHandler(self);

        Semaphore_wait(self->appTickLock);
        self->appTickEnabled = false;
        Semaphore_post(self->appTickLock);
    }
    else {
        if (disconnectMode == MMS_ENDPOINT_DISCONNECT_CLOSE) {
            IsoClientConnection_close(self->isoClient);
        }
        else if (disconnectMode == MMS_ENDPOINT_DISCONNECT_ABORT) {
            IsoClientConnection_abortAsync(self->isoClient);
        }
        else if (disconnectMode == MMS_ENDPOINT_DISCONNECT_RELEASE) {
            IsoClientConnection_release(self->isoClient);
        }
    }
}

void
MmsEndpoint_disconnectPeer(MmsEndpoint self, const char* ipAddr)
{
    if (self->passiveMode) {

        Semaphore_wait(self->openConnectionsLock);

        LinkedList conElem = LinkedList_getNext(self->openConnections);

        while (conElem) {

            MmsEndpointConnection con = (MmsEndpointConnection) LinkedList_getData(conElem);

            const char* conIpAddr = IsoConnection_getPeerAddress(con->isoConnection);

            const char* seperator = strchr(ipAddr, ':');

            if (seperator) {

                if (!strcmp(ipAddr, conIpAddr)) {
                    IsoConnection_close(con->isoConnection);
                    break;
                }
            }
            else {
                if (!strncmp(ipAddr, conIpAddr, strlen(ipAddr))) {
                    IsoConnection_close(con->isoConnection);
                    break;
                }
            }

            conElem = LinkedList_getNext(conElem);
        }

        Semaphore_post(self->openConnectionsLock);
    }
}

void
MmsEndpoint_disconnectPeersForServer(MmsEndpoint self, MmsServer server)
{
    if (self->passiveMode) {

        Semaphore_wait(self->openConnectionsLock);

        LinkedList conElem = LinkedList_getNext(self->openConnections);

        while (conElem) {

            MmsEndpointConnection con = (MmsEndpointConnection) LinkedList_getData(conElem);

            if (con->serverConnection) {
                if (con->serverConnection->server == server) {
                    IsoConnection_close(con->isoConnection);
                }
            }

            conElem = LinkedList_getNext(conElem);
        }

        Semaphore_post(self->openConnectionsLock);
    }
}

bool
MmsEndpoint_isRunning(MmsEndpoint self)
{
    if (self->passiveMode) {
        if (self->isoServer) {
            if (IsoServer_getState(self->isoServer) == ISO_SVR_STATE_RUNNING)
                return true;
        }
    }
    else {
        if (self->isoClient) {
            return IsoClientConnection_isRunning(self->isoClient);
        }
    }

    return false;
}

bool
MmsEndpoint_isPassive(MmsEndpoint self)
{
    return self->passiveMode;
}

void
MmsEndpoint_installInformationReportHandler(MmsEndpoint self, MmsInformationReportHandler handler, void* parameter)
{
    self->reportHandler = handler;
    self->reportHandlerParameter = parameter;
}

uint64_t
MmsEndpoint_getStatistics(MmsEndpoint self, int statisticValueId)
{
#if (CONFIG_MMS_COLLECT_STATISTICS == 1)

    switch (statisticValueId)
    {
    case MMS_STAT_ID_APL_MESSAGES_SENT:
        return self->statAplMessagesSent;
        break;

    case MMS_STAT_ID_APL_MESSAGES_RCVD:
        return self->statAplMessagesRcvd;
        break;

    case MMS_STAT_ID_CONF_ERROR_RCVD:
        return self->statConfErrorPduRcvd;
        break;

    case MMS_STAT_ID_CONF_ERROR_SENT:
        return self->statConfErrorPduSend;
        break;

    case MMS_STAT_ID_REJECT_RCVD:
        return self->statRejectPduRcvd;
        break;

    case MMS_STAT_ID_REJECT_SENT:
        return self->statRejectPduSent;
        break;

    case MMS_STAT_ID_REQUEST_TIMEOUTS:
        return self->statRequestTimeouts;
        break;

    default:
        return 0;
    }

#else
    return 0;
#endif
}

void
MmsEndpoint_setIdentity(MmsEndpoint self, char* vendorName, char* modelName, char* revision)
{
    self->vendorName = vendorName;
    self->modelName = modelName;
    self->revision = revision;
}

const char*
MmsEndpoint_getVendorName(MmsEndpoint self)
{
    if (self->vendorName != NULL)
        return self->vendorName;
    else
        return CONFIG_DEFAULT_MMS_VENDOR_NAME;
}

const char*
MmsEndpoint_getModelName(MmsEndpoint self)
{
    if (self->modelName != NULL)
        return self->modelName;
    else
        return CONFIG_DEFAULT_MMS_MODEL_NAME;
}

const char*
MmsEndpoint_getRevision(MmsEndpoint self)
{
    if (self->revision != NULL)
        return self->revision;
    else
        return CONFIG_DEFAULT_MMS_REVISION;
}

void
MmsEndpoint_setClientAuthenticator(MmsEndpoint self, AcseAuthenticator authenticator, void* authenticatorParameter)
{
    if (self->passiveMode) {
        IsoServer_setAuthenticator(self->isoServer, authenticator, authenticatorParameter);
    }
    else
    {
        IsoClientConnection_setAuthenticator(self->isoClient, authenticator, authenticatorParameter);
    }
}

#if (MMS_DYNAMIC_DATA_SETS == 1)

LinkedList
MmsEndpointServerConnection_getNamedVariableLists(MmsEndpointServerConnection self)
{
    return self->namedVariableLists;
}

bool
MmsEndpointServerConnection_addNamedVariableList(MmsEndpointServerConnection self, MmsNamedVariableList variableList)
{
    /* TODO check if operation is allowed! */

    LinkedList_add(self->namedVariableLists, variableList);

    return true;
}

void
MmsEndpointServerConnection_deleteNamedVariableList(MmsEndpointServerConnection self, char* listName)
{
    mmsServer_deleteVariableList(self->namedVariableLists, listName);
}

MmsNamedVariableList
MmsEndpointServerConnection_getNamedVariableList(MmsEndpointServerConnection self, const char* variableListName)
{
    return mmsServer_getNamedVariableListWithName(self->namedVariableLists, variableListName);
}

#endif /* (MMS_DYNAMIC_DATA_SETS == 1) */

uint32_t
MmsEndpointConnection_getNextInvokeId(MmsEndpointConnection self)
{
    uint32_t nextInvokeId;

    Semaphore_wait(self->lastRequestInvokeIdLock);
    self->lastRequestInvokeId++;
    nextInvokeId = self->lastRequestInvokeId;
    Semaphore_post(self->lastRequestInvokeIdLock);

    return nextInvokeId;
}

MmsError
MmsEndpointConnection_sendAsyncRequest(MmsEndpointConnection self, uint32_t invokeId, ByteBuffer* message, eMmsOutstandingCallType type,
        void* userCallback, void* userParameter, void* internalParameter)
{
    if (addToOutstandingCalls(self, invokeId, type, userCallback, userParameter, internalParameter) == false) {

        /* message cannot be sent - release resources */
        MmsEndpoint_releaseTransmitBuffer(self->endpoint);

        return MMS_ERROR_OUTSTANDING_CALL_LIMIT;
    }
    else {
        MmsEndpointConnection_sendMessage(self, message);

        MmsEndpoint_releaseTransmitBuffer(self->endpoint);

        return MMS_ERROR_NONE;
    }
}

void
MmsEndpointConnection_lockConnection(MmsEndpointConnection self)
{
    if (self->isoConnection) {
        IsoConnection_lockConnection(self->isoConnection);
    }
}

void
MmsEndpointConnection_unlockConnection(MmsEndpointConnection self)
{
    if (self->isoConnection) {
        IsoConnection_unlockConnection(self->isoConnection);
    }
}

MmsError
MmsEndpointConnection_sendAsyncRequestInHandlerMode(MmsEndpointConnection self, uint32_t invokeId, ByteBuffer* message, eMmsOutstandingCallType type,
        void* userCallback, void* userParameter, void* internalParameter)
{
    if (addToOutstandingCalls(self, invokeId, type, userCallback, userParameter, internalParameter) == false) {

        /* message cannot be sent - release resources */
        MmsEndpoint_releaseTransmitBuffer(self->endpoint);

        return MMS_ERROR_OUTSTANDING_CALL_LIMIT;
    }
    else {
        MmsEndpointConnection_sendMessage(self, message);

        MmsEndpoint_releaseTransmitBuffer(self->endpoint);

        return MMS_ERROR_NONE;
    }
}

#if (MMS_OBTAIN_FILE_SERVICE == 1)

MmsObtainFileTask
MmsEndpoint_getObtainFileTask(MmsEndpoint self)
{
    int i;

    for (i = 0; i < CONFIG_MMS_SERVER_MAX_GET_FILE_TASKS; i++) {

        if (self->fileUploadTasks[i].state == 0) {
            self->fileUploadTasks[i].state = 1;
            return &(self->fileUploadTasks[i]);
        }

    }

    return NULL;
}

#endif /* (MMS_OBTAIN_FILE_SERVICE == 1) */

