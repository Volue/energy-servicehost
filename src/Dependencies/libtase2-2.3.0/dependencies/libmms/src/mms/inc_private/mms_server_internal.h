/*
 *  mms_server_internal.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_SERVER_INTERNAL_H_
#define MMS_SERVER_INTERNAL_H_

#include "libmms_platform_includes.h"
#include "MmsPdu.h"
#include "mms_common.h"
#include "mms_device_model.h"
#include "mms_common_internal.h"
#include "stack_config.h"
#include "mms_server.h"


#include "byte_buffer.h"
#include "string_utilities.h"
#include "hal_thread.h"
#include "avl_tree.h"

#include "ber_encoder.h"
#include "ber_decode.h"

#if 0
#if (MMS_OBTAIN_FILE_SERVICE == 1)
#include "hal_filesystem.h"
#endif


#ifndef DEBUG_MMS_SERVER
#define DEBUG_MMS_SERVER 0
#endif

#ifndef MMS_STATUS_SERVICE
#define MMS_STATUS_SERVICE 1
#endif

#ifndef MMS_IDENTIFY_SERVICE
#define MMS_IDENTIFY_SERVICE 1
#endif

#ifndef MMS_FILE_SERVICE
#define MMS_FILE_SERVICE 1
#endif

#ifndef CONFIG_MMS_SERVER_MAX_GET_FILE_TASKS
#define CONFIG_MMS_SERVER_MAX_GET_FILE_TASKS 5
#endif


#if (MMS_OBTAIN_FILE_SERVICE == 1)

#define MMS_FILE_UPLOAD_STATE_NOT_USED 0
#define MMS_FILE_UPLOAD_STATE_READY 1
#define MMS_FILE_UPLOAD_STATE_FILE_OPEN_SENT 2

#define MMS_FILE_UPLOAD_STATE_SEND_FILE_READ 3
#define MMS_FILE_UPLOAD_STATE_FILE_READ_SENT 4

#define MMS_FILE_UPLOAD_STATE_SEND_FILE_CLOSE 5
#define MMS_FILE_UPLOAD_STATE_FILE_CLOSE_SENT 6

#define MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_SOURCE 8
#define MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_ERROR_DESTINATION 9
#define MMS_FILE_UPLOAD_STATE_SEND_OBTAIN_FILE_RESPONSE 10

typedef struct sMmsObtainFileTask* MmsObtainFileTask;

struct sMmsObtainFileTask {
    MmsServerConnection connection;
    uint32_t lastRequestInvokeId;
    uint32_t obtainFileRequestInvokeId;
    FileHandle fileHandle;
    char destinationFilename[256];
    uint64_t nextTimeout;
    int32_t frmsId;
    int state;
};

#endif /* (MMS_OBTAIN_FILE_SERVICE == 1) */
#endif

struct sMmsServer {
    MmsEndpoint endpoint;

    MmsDevice* device;

    MmsReadVariableHandler readHandler;
    void* readHandlerParameter;

    MmsReadAccessHandler readAccessHandler;
    void* readAccessHandlerParameter;

    MmsWriteVariableHandler writeHandler;
    void* writeHandlerParameter;

    MmsVarSpecHandler aaVarSpecHandler;
    void* aaVarSpecHandlerParameter;

    MmsInformationReportHandler reportHandler;
    void* reportHandlerParameter;

    MmsNamedVariableListChangedHandler variableListChangedHandler; //TODO this is only required if dynamic data sets are supported!
    void* variableListChangedHandlerParameter;

    struct sIsoApplicationAddress localApAddress;

    AvlTree valueCaches;
    bool isLocked;

#if (CONFIG_MMS_THREADLESS_STACK != 1)
    Semaphore modelMutex;
#endif

#if (MMS_STATUS_SERVICE == 1)
    int vmdLogicalStatus;
    int vmdPhysicalStatus;
    MmsStatusRequestListener statusRequestListener;
    void* statusRequestListenerParameter;
#endif /* MMS_STATUS_SERVICE == 1 */

};

/* write_out function required for ASN.1 encoding */
LIBMMS_INTERNAL int
mmsServer_write_out(const void *buffer, size_t size, void *app_key);

LIBMMS_INTERNAL void
mmsServer_handleDeleteNamedVariableListRequest(MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
mmsServer_handleGetNamedVariableListAttributesRequest(
        MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId);

LIBMMS_INTERNAL void
mmsServer_handleReadRequest(
        MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId);

LIBMMS_INTERNAL MmsPdu_t*
mmsServer_createConfirmedResponse(uint32_t invokeId);

LIBMMS_INTERNAL void
mmsMsg_createServiceErrorPdu(uint32_t invokeId, ByteBuffer* response, MmsError errorType);

LIBMMS_INTERNAL void
mmsServer_createServiceErrorPduWithServiceSpecificInfo(uint32_t invokeId, ByteBuffer* response,
        MmsError errorType, uint8_t* serviceSpecificInfo, int serviceSpecficInfoLength);

LIBMMS_INTERNAL void
mmsEndpointConnection_writeConcludeResponsePdu(ByteBuffer* response);

LIBMMS_INTERNAL void
mmsEndpointConnection_handleInitiateRequest (
        MmsEndpointConnection self,
        uint8_t* buffer, int bufPos, int maxBufPos,
        ByteBuffer* response);

LIBMMS_INTERNAL int
mmsServer_handleGetVariableAccessAttributesRequest(
        MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId);

LIBMMS_INTERNAL void
mmsServer_handleDefineNamedVariableListRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
mmsServer_handleGetNameListRequest(
        MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId);

LIBMMS_INTERNAL void
mmsServer_handleWriteRequest(
        MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId);

LIBMMS_INTERNAL void
mmsServer_handleIdentifyRequest(
        uint32_t invokeId,
        ByteBuffer* response,
        const char* vendorName,
        const char* modelName,
        const char* revision);

LIBMMS_INTERNAL void
mmsServer_handleStatusRequest(
        MmsEndpointConnection connection,
        uint8_t* requestBuffer,
        int bufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL void
mmsServer_handleReadJournalRequest(
        MmsEndpointConnection connection,
        uint8_t* requestBuffer,
        int bufPos, int maxBufPos,
        uint32_t invokeId);

LIBMMS_INTERNAL bool
mmsServer_isComponentAccess(AlternateAccess_t* alternateAccess);

LIBMMS_INTERNAL char*
mmsServer_getComponentName(AlternateAccess_t* alternateAccess, char* buffer);

LIBMMS_INTERNAL bool
mmsServer_isIndexAccess(AlternateAccess_t* alternateAccess);

LIBMMS_INTERNAL int
mmsServer_getLowIndex(AlternateAccess_t* alternateAccess);

LIBMMS_INTERNAL int
mmsServer_getNumberOfElements(AlternateAccess_t* alternateAccess);

LIBMMS_INTERNAL MmsNamedVariableList
mmsServer_getNamedVariableListWithName(LinkedList namedVariableLists, const char* variableListName);

LIBMMS_INTERNAL void
mmsServer_deleteVariableList(LinkedList namedVariableLists, char* variableListName);

LIBMMS_INTERNAL MmsDataAccessError
mmsServer_setComponentValue(MmsServer self, MmsObjectScope objectScope, MmsDomain* domain, const char* itemId, const char* componentName, int componentIndex,
        MmsValue* value, MmsEndpointConnection connection);

LIBMMS_INTERNAL MmsDataAccessError
mmsServer_setValue(MmsServer self, MmsObjectScope objectScope, MmsDomain* domain, const char* itemId, MmsValue* value,
        MmsEndpointConnection connection);

LIBMMS_INTERNAL MmsValue*
mmsServer_getValue(MmsServer self, MmsDomain* domain, char* itemId, MmsEndpointConnection connection);

LIBMMS_INTERNAL void
mmsServer_createMmsWriteResponse(MmsEndpointConnection connection,
        uint32_t invokeId, ByteBuffer* response, int numberOfItems, MmsDataAccessError* accessResults);

LIBMMS_INTERNAL void
mmsMsg_createMmsRejectPdu(uint32_t* invokeId, int reason, ByteBuffer* response);

LIBMMS_INTERNAL MmsError
mmsServer_callVariableListChangedHandler(bool create, MmsObjectScope objectScope, MmsNamedVariableList variableList, MmsEndpointConnection connection);

#endif /* MMS_SERVER_INTERNAL_H_ */
