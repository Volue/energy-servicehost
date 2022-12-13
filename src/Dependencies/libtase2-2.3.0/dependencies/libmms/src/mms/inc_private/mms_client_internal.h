/*
 *  mms_msg_internal.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_MSG_INTERNAL_H_
#define MMS_MSG_INTERNAL_H_

#include "mms_client.h"
#include "MmsPdu.h"
#include "linked_list.h"
#include "iso_client_connection.h"
#include "ber_decode.h"

#include "hal_thread.h"

#include "mms_common_internal.h"

#ifndef CONFIG_MMS_RAW_MESSAGE_LOGGING
#define CONFIG_MMS_RAW_MESSAGE_LOGGING 0
#endif

#ifndef DEBUG_MMS_CLIENT
#define DEBUG_MMS_CLIENT 0
#endif

#define CONCLUDE_STATE_CONNECTION_ACTIVE 0
#define CONCLUDE_STATE_REQUESTED 1
#define CONCLUDE_STATE_REJECTED 2
#define CONCLUDE_STATE_ACCEPTED 3

typedef enum {
    MMS_CALL_TYPE_NONE,
    MMS_CALL_TYPE_READ_VARIABLE,
    MMS_CALL_TYPE_READ_MULTIPLE_VARIABLES,
    MMS_CALL_TYPE_WRITE_VARIABLE,
    MMS_CALL_TYPE_WRITE_MULTIPLE_VARIABLES,
    MMS_CALL_TYPE_READ_NVL_DIRECTORY,
    MMS_CALL_TYPE_DEFINE_NVL,
    MMS_CALL_TYPE_DELETE_NVL,
    MMS_CALL_TYPE_GET_VAR_ACCESS_ATTR,
    MMS_CALL_TYPE_GET_SERVER_STATUS,
    MMS_CALL_TYPE_IDENTIFY,
    MMS_CALL_TYPE_READ_JOURNAL,
    MMS_CALL_TYPE_GET_NAME_LIST,
    MMS_CALL_TYPE_FILE_OPEN,
    MMS_CALL_TYPE_FILE_READ,
    MMS_CALL_TYPE_FILE_CLOSE,
    MMS_CALL_TYPE_FILE_DELETE,
    MMS_CALL_TYPE_FILE_RENAME,
    MMS_CALL_TYPE_OBTAIN_FILE,
    MMS_CALL_TYPE_GET_FILE_DIR
} eMmsOutstandingCallType;

struct sMmsOutstandingCall
{
    bool isUsed;
    uint32_t invokeId;
    eMmsOutstandingCallType type;
    void* userCallback;
    void* userParameter;
    void* internalParameter;
    uint64_t timeout;
};

typedef struct sMmsOutstandingCall* MmsOutstandingCall;

/**
 * MMS Object class enumeration type
 */
typedef enum {
	MMS_OBJECT_CLASS_NAMED_VARIABLE = 0,
	MMS_OBJECT_CLASS_NAMED_VARIABLE_LIST = 2,
	MMS_OBJECT_CLASS_JOURNAL = 8,
	MMS_OBJECT_CLASS_DOMAIN = 9
} MmsObjectClass;

LIBMMS_INTERNAL uint32_t
mmsClient_getInvokeId(ConfirmedResponsePdu_t* confirmedResponse);

LIBMMS_INTERNAL int
mmsClient_write_out(void *buffer, size_t size, void *app_key);

LIBMMS_INTERNAL void
mmsClient_createInitiateRequest(MmsEndpointConnection self, ByteBuffer* writeBuffer);

LIBMMS_INTERNAL MmsPdu_t*
mmsClient_createConfirmedRequestPdu(uint32_t invokeId);

LIBMMS_INTERNAL int
mmsClient_createMmsGetNameListRequestVMDspecific(long invokeId, ByteBuffer* writeBuffer, const char* continueAfter);

LIBMMS_INTERNAL bool
mmsClient_parseGetNameListResponse(MmsEndpointConnection self, LinkedList* nameList, ByteBuffer* message);

LIBMMS_INTERNAL int
mmsClient_createGetNameListRequestDomainOrVMDSpecific(long invokeId, const char* domainName,
		ByteBuffer* writeBuffer, MmsObjectClass objectClass, const char* continueAfter);

LIBMMS_INTERNAL MmsValue*
mmsClient_parseReadResponse(MmsEndpointConnection self, ByteBuffer* message, uint32_t* invokeId, bool createArray);

LIBMMS_INTERNAL int
mmsClient_createReadRequest(uint32_t invokeId, const char* domainId, const char* itemId, bool specWithResult, ByteBuffer* writeBuffer);

LIBMMS_INTERNAL int
mmsClient_createReadRequestAlternateAccessIndex(uint32_t invokeId, const char* domainId, const char* itemId,
		uint32_t index, uint32_t elementCount, ByteBuffer* writeBuffer);

LIBMMS_INTERNAL int
mmsClient_createReadRequestAlternateAccessSingleIndexComponent(uint32_t invokeId, const char* domainId, const char* itemId,
        uint32_t index, const char* component, ByteBuffer* writeBuffer);

LIBMMS_INTERNAL int
mmsClient_createReadRequestMultipleValues(uint32_t invokeId, const char* domainId, LinkedList /*<char*>*/ items, bool specificationWithResult,
		ByteBuffer* writeBuffer);

LIBMMS_INTERNAL int
mmsClient_createReadNamedVariableListRequest(uint32_t invokeId, const char* domainId, const char* itemId,
		ByteBuffer* writeBuffer, bool specWithResult);

LIBMMS_INTERNAL int
mmsClient_createReadAssociationSpecificNamedVariableListRequest(
		uint32_t invokeId,
		const char* itemId,
		ByteBuffer* writeBuffer,
		bool specWithResult);

LIBMMS_INTERNAL void
mmsClient_createGetNamedVariableListAttributesRequest(uint32_t invokeId, ByteBuffer* writeBuffer,
		const char* domainId, const char* listNameId);

LIBMMS_INTERNAL void
mmsClient_createGetNamedVariableListAttributesRequestAssociationSpecific(uint32_t invokeId,
        ByteBuffer* writeBuffer, const char* listNameId);

LIBMMS_INTERNAL LinkedList
mmsClient_parseGetNamedVariableListAttributesResponse(ByteBuffer* message, bool* /*OUT*/ deletable);

LIBMMS_INTERNAL int
mmsClient_createGetVariableAccessAttributesRequest(
        uint32_t invokeId,
        const char* domainId, const char* itemId,
		ByteBuffer* writeBuffer);

LIBMMS_INTERNAL MmsVariableSpecification*
mmsClient_parseGetVariableAccessAttributesResponse(ByteBuffer* message, uint32_t* invokeId);

LIBMMS_INTERNAL MmsDataAccessError
mmsClient_parseWriteResponse(ByteBuffer* message, int32_t bufPos, MmsError* mmsError);

LIBMMS_INTERNAL void
mmsClient_parseWriteMultipleItemsResponse(ByteBuffer* message, int32_t bufPos, MmsError* mmsError,
        int itemCount, LinkedList* accessResults);

LIBMMS_INTERNAL int
mmsClient_createWriteRequest(uint32_t invokeId, const char* domainId, const char* itemId, MmsValue* value,
        ByteBuffer* writeBuffer, bool aaspecific);

LIBMMS_INTERNAL int
mmsClient_createWriteComponenteRequest(uint32_t invokeId, const char* domainId, const char* itemId, const char* componentName,
        MmsValue* value, ByteBuffer* writeBuffer);

int
mmsClient_createWriteMultipleItemsRequest(uint32_t invokeId, const char* domainId, LinkedList itemIds, LinkedList values,
        ByteBuffer* writeBuffer);

LIBMMS_INTERNAL int
mmsClient_createWriteRequestNamedVariableList(uint32_t invokeId, bool isAssociationSpecific, const char* domainId, const char* itemId,
        LinkedList values, ByteBuffer* writeBuffer);

LIBMMS_INTERNAL int
mmsClient_createWriteRequestArray(uint32_t invokeId, const char* domainId, const char* itemId,
        int startIndex, int elementCount,
        MmsValue* value, ByteBuffer* writeBuffer);

LIBMMS_INTERNAL int
mmsClient_createWriteRequestAlternateAccessSingleIndexComponent(uint32_t invokeId, const char* domainId, const char* itemId,
        uint32_t arrayIndex, const char* component,
        MmsValue* value,
        ByteBuffer* writeBuffer);

LIBMMS_INTERNAL void
mmsClient_createDefineNamedVariableListRequest(uint32_t invokeId, ByteBuffer* writeBuffer,
		const char* domainId, const char* listNameId, LinkedList /*<char*>*/ listOfVariables,
		bool associationSpecific);

LIBMMS_INTERNAL bool
mmsClient_parseDefineNamedVariableResponse(ByteBuffer* message, uint32_t* invokeId);

LIBMMS_INTERNAL void
mmsClient_createDeleteNamedVariableListRequest(long invokeId, ByteBuffer* writeBuffer,
        const char* domainId, const char* listNameId);

LIBMMS_INTERNAL bool
mmsClient_parseDeleteNamedVariableListResponse(ByteBuffer* message, uint32_t* invokeId);

LIBMMS_INTERNAL void
mmsClient_createDeleteAssociationSpecificNamedVariableListRequest(
		long invokeId,
		ByteBuffer* writeBuffer,
		const char* listNameId);

LIBMMS_INTERNAL void
mmsClient_createIdentifyRequest(uint32_t invokeId, ByteBuffer* request);

LIBMMS_INTERNAL bool
mmsClient_parseIdentifyResponse(MmsEndpointConnection self, ByteBuffer* response, uint32_t bufPos, uint32_t invokeId, MmsConnection_IdentifyHandler handler, void* parameter);

LIBMMS_INTERNAL void
mmsClient_createStatusRequest(uint32_t invokeId, ByteBuffer* request, bool extendedDerivation);

LIBMMS_INTERNAL bool
mmsClient_parseStatusResponse(MmsEndpointConnection self, ByteBuffer* response, int bufPos, int* vmdLogicalStatus, int* vmdPhysicalStatus);

LIBMMS_INTERNAL void
mmsClient_createFileOpenRequest(uint32_t invokeId, ByteBuffer* request, const char* fileName, uint32_t initialPosition);

LIBMMS_INTERNAL void
mmsClient_createFileReadRequest(uint32_t invokeId, ByteBuffer* request, int32_t frsmId);

LIBMMS_INTERNAL void
mmsClient_createFileCloseRequest(uint32_t invokeId, ByteBuffer* request, int32_t frsmId);

LIBMMS_INTERNAL void
mmsClient_createFileRenameRequest(uint32_t invokeId, ByteBuffer* request, const char* currentFileName, const char* newFileName);

LIBMMS_INTERNAL void
mmsClient_createObtainFileRequest(uint32_t invokeId, ByteBuffer* request, const char* sourceFile, const char* destinationFile);

LIBMMS_INTERNAL void
mmsClient_createFileDeleteRequest(uint32_t invokeId, ByteBuffer* request, const char* fileName);

LIBMMS_INTERNAL void
mmsClient_createFileDirectoryRequest(uint32_t invokeId, ByteBuffer* request, const char* fileSpecification, const char* continueAfter);

LIBMMS_INTERNAL bool
mmsClient_parseFileDirectoryResponse(ByteBuffer* response, int bufPos, uint32_t invokeId, MmsConnection_FileDirectoryHandler handler, void* parameter);

LIBMMS_INTERNAL bool
mmsClient_parseInitiateResponse(MmsEndpointConnection self, ByteBuffer* response);

LIBMMS_INTERNAL int
mmsClient_createConcludeRequest(MmsConnection self, ByteBuffer* message);

LIBMMS_INTERNAL int
mmsClient_createMmsGetNameListRequestAssociationSpecific(long invokeId, ByteBuffer* writeBuffer,
		const char* continueAfter);

LIBMMS_INTERNAL void
mmsClient_createReadJournalRequestWithTimeRange(uint32_t invokeId, ByteBuffer* request, const char* domainId, const char* itemId,
        MmsValue* startingTime, MmsValue* endingTime);

LIBMMS_INTERNAL void
mmsClient_createReadJournalRequestStartAfter(uint32_t invokeId, ByteBuffer* request, const char* domainId, const char* itemId,
        MmsValue* timeSpecification, MmsValue* entrySpecification);

LIBMMS_INTERNAL bool
mmsClient_parseReadJournalResponse(ByteBuffer* response, int respBufPos, bool* moreFollows, LinkedList* result);

LIBMMS_INTERNAL void
mmsClient_handleFileOpenRequest(MmsClient connection,
    uint8_t* buffer, int bufPos, int maxBufPos,
    uint32_t invokeId, ByteBuffer* response);

LIBMMS_INTERNAL void
mmsClient_handleFileReadRequest(
    MmsClient connection,
    uint8_t* buffer, int bufPos, int maxBufPos,
    uint32_t invokeId,
    ByteBuffer* response);

LIBMMS_INTERNAL void
mmsClient_handleFileCloseRequest(
    MmsClient connection,
    uint8_t* buffer, int bufPos, int maxBufPos,
    uint32_t invokeId,
    ByteBuffer* response);

LIBMMS_INTERNAL bool
MmsClient_lockConnection(MmsClient self);

LIBMMS_INTERNAL void
MmsClient_unlockConnection(MmsClient self);


#endif /* MMS_MSG_INTERNAL_H_ */
