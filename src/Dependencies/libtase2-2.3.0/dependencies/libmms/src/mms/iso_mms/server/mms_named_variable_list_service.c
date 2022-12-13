/*
 *  mms_named_variable_list_service.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_server_internal.h"
#include "mms_common.h"
#include "mms_endpoint_internal.h"
#include "mms_named_variable_list.h"

#include "ber_encoder.h"

#if (MMS_DATA_SET_SERVICE == 1)

#if (MMS_DYNAMIC_DATA_SETS == 1)

#ifndef CONFIG_MMS_MAX_NUMBER_OF_DOMAIN_SPECIFIC_DATA_SETS
#define CONFIG_MMS_MAX_NUMBER_OF_DOMAIN_SPECIFIC_DATA_SETS 10
#endif

#ifndef CONFIG_MMS_MAX_NUMBER_OF_ASSOCIATION_SPECIFIC_DATA_SETS
#define CONFIG_MMS_MAX_NUMBER_OF_ASSOCIATION_SPECIFIC_DATA_SETS 10
#endif

#ifndef CONFIG_MMS_MAX_NUMBER_OF_VMD_SPECIFIC_DATA_SETS
#define CONFIG_MMS_MAX_NUMBER_OF_VMD_SPECIFIC_DATA_SETS 10
#endif

#ifndef CONFIG_MMS_MAX_NUMBER_OF_DATA_SET_MEMBERS
#define CONFIG_MMS_MAX_NUMBER_OF_DATA_SET_MEMBERS 50
#endif

MmsError
mmsServer_callVariableListChangedHandler(bool create, MmsObjectScope objectScope, MmsNamedVariableList namedVariableList, MmsEndpointConnection connection)
{
    MmsServer self = connection->serverConnection->server;

    if (self->variableListChangedHandler != NULL) {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_DEBUG, connection, "MMS_SERVER: call MmsNamedVariableListChangedHandler for new list %s\n", namedVariableList->name);

        return self->variableListChangedHandler(self->variableListChangedHandlerParameter,
                create, objectScope, namedVariableList, connection);
    }
    else
        return MMS_ERROR_NONE;
}

static void
createDeleteNamedVariableListResponse(uint32_t invokeId, ByteBuffer* response,
        uint32_t numberMatched, uint32_t numberDeleted)
{
    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize(invokeId) + 2;

    uint32_t numberMatchedSize =
            2 + BerEncoder_UInt32determineEncodedSize(numberMatched);

    uint32_t numberDeletedSize =
            2 + BerEncoder_UInt32determineEncodedSize(numberDeleted);

    uint32_t deleteNVLSize = 2 + numberMatchedSize + numberDeletedSize;

    uint32_t confirmedResponsePDUSize = invokeIdSize + deleteNVLSize;

    int bufPos = 0;
    uint8_t* buffer = response->buffer;

    bufPos = BerEncoder_encodeTL(0xa1, confirmedResponsePDUSize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0xad, numberMatchedSize + numberDeletedSize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x80, numberMatchedSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(numberMatched, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x81, numberDeletedSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(numberDeleted, buffer, bufPos);

    response->size = bufPos;
}

static void /* Confirmed service error (ServiceError) */
createServiceErrorDeleteVariableLists(uint32_t invokeId, ByteBuffer* response,
        MmsError errorType, uint32_t numberDeleted)
{
    uint8_t buffer[8];

    int size = BerEncoder_encodeUInt32WithTL(0x86, numberDeleted, buffer, 0);

    mmsServer_createServiceErrorPduWithServiceSpecificInfo(invokeId, response, errorType,
            buffer, size);
}

void
mmsServer_handleDeleteNamedVariableListRequest(MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId)
{
	DeleteNamedVariableListRequest_t* request = 0;
	long scopeOfDelete;
	MmsDevice* device;
    MmsPdu_t* mmsPdu = 0;

    asn_dec_rval_t rval = ber_decode(NULL, &asn_DEF_MmsPdu, (void**) &mmsPdu, buffer, maxBufPos);

    if (rval.code != RC_OK) {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: delete-named-variable-list (invoke-id: %u) - invalid PDU -> reject", invokeId);

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_INVALID_PDU, response);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

        goto exit_function;
    }

    request = &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.deleteNamedVariableList);

	scopeOfDelete = DeleteNamedVariableListRequest__scopeOfDelete_specific;

	if (request->scopeOfDelete)
	    asn_INTEGER2long(request->scopeOfDelete, &scopeOfDelete);

	device = MmsServer_getDevice(connection->serverConnection->server);

	if (scopeOfDelete == DeleteNamedVariableListRequest__scopeOfDelete_specific) {
	    MmsError serviceError = MMS_ERROR_NONE;

		int numberMatched = 0;
		int numberDeleted = 0;

		int numberItems = request->listOfVariableListName->list.count;

		int i;

		for (i = 0; i < numberItems; i++) {
			if (request->listOfVariableListName->list.array[i]->present == ObjectName_PR_domainspecific) {
		        char domainName[65];
		        char listName[65];

		        mmsMsg_copyAsn1IdentifierToStringBuffer(request->listOfVariableListName->list.array[i]->choice.domainspecific.domainId,
		                domainName, 65);

		        mmsMsg_copyAsn1IdentifierToStringBuffer(request->listOfVariableListName->list.array[i]->choice.domainspecific.itemId,
		                listName, 65);

		        if (DEBUG_MMS_SERVER)
		            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: delete-named-variable-list (invoke-id: %u) - domain specific %s:%s", invokeId, domainName, listName);

		        MmsDomain* domain = MmsDevice_getDomain(device, domainName);

		        if (domain != NULL) {

                    MmsNamedVariableList variableList = MmsDomain_getNamedVariableList(domain, listName);

                    if (variableList != NULL) {
                        numberMatched++;

                        if (MmsNamedVariableList_isDeletable(variableList)) {

                            MmsError deleteError = mmsServer_callVariableListChangedHandler(false, MMS_DOMAIN_SPECIFIC, variableList, connection);

                            if (deleteError == MMS_ERROR_NONE) {
                                MmsDomain_deleteNamedVariableList(domain, listName);
                                numberDeleted++;
                            }
                            else
                                serviceError = deleteError;
                        }
                    }
		        }
			}
			else if (request->listOfVariableListName->list.array[i]->present == ObjectName_PR_aaspecific) {
			    char listName[65];

			    mmsMsg_copyAsn1IdentifierToStringBuffer(request->listOfVariableListName->list.array[i]->choice.aaspecific,
			            listName, 65);

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: delete-named-variable-list (invoke-id: %u) - association specific %s", invokeId, listName);

				MmsNamedVariableList variableList = MmsEndpointServerConnection_getNamedVariableList(connection->serverConnection, listName);

				if (variableList != NULL) {
					numberMatched++;

					MmsError deleteError = mmsServer_callVariableListChangedHandler(false, MMS_ASSOCIATION_SPECIFIC, variableList, connection);

					if (deleteError == MMS_ERROR_NONE) {
					    numberDeleted++;
					    MmsEndpointServerConnection_deleteNamedVariableList(connection->serverConnection, listName);
					}
					else
					    serviceError = deleteError;
				}
			}
			else if (request->listOfVariableListName->list.array[i]->present == ObjectName_PR_vmdspecific) {
			    char listName[65];

                mmsMsg_copyAsn1IdentifierToStringBuffer(request->listOfVariableListName->list.array[i]->choice.vmdspecific,
                        listName, 65);

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: delete-named-variable-list (invoke-id: %u) - vmd specific %s", invokeId, listName);

                MmsNamedVariableList variableList = mmsServer_getNamedVariableListWithName(device->namedVariableLists, listName);

                if (variableList != NULL) {
                    numberMatched++;

                    MmsError deleteError = mmsServer_callVariableListChangedHandler(false, MMS_VMD_SPECIFIC, variableList, connection);

                    if (deleteError == MMS_ERROR_NONE) {
                        numberDeleted++;
                        mmsServer_deleteVariableList(device->namedVariableLists, listName);
                    }
                    else
                        serviceError = deleteError;
                }
			}
		}

		if (serviceError ==  MMS_ERROR_NONE) {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: delete-named-variable-list (invoke-id: %u) - matched: %i (deleted: %i)", invokeId, numberMatched, numberDeleted);

	        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

	        if (response) {
	            createDeleteNamedVariableListResponse(invokeId, response, numberMatched, numberDeleted);

	            MmsEndpointConnection_sendMessage(connection, response);

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
                connection->endpoint->statAplMessagesSent++;
#endif

	            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
	        }

		}
		else {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: delete-named-variable-list (invoke-id: %u) --> service error %i (deleted: %i)", invokeId, (int) serviceError, numberDeleted);

	        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

	        if (response) {
	            createServiceErrorDeleteVariableLists(invokeId, response, serviceError, numberDeleted);

	            MmsEndpointConnection_sendMessage(connection, response);

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
                connection->endpoint->statAplMessagesSent++;
#endif

	            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
	        }

		}
	}
	else {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: delete-named-variable-list (invoke-id: %u) -> object-access-unsupported", invokeId);

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

	}

exit_function:

    asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

    return;
}

static void
createDefineNamedVariableListResponse(uint32_t invokeId, ByteBuffer* response)
{
    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize((uint32_t) invokeId) + 2;

    uint32_t confirmedResponsePDUSize = 2 + invokeIdSize;

    int bufPos = 0;
    uint8_t* buffer = response->buffer;

    bufPos = BerEncoder_encodeTL(0xa1, confirmedResponsePDUSize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x8b, 0, buffer, bufPos);

    response->size = bufPos;
}

static bool
checkIfVariableExists(MmsDevice* device, MmsVariableAccessSpecification* accessSpecifier)
{
    MmsVariableSpecification* variableSpec = NULL;

    if (accessSpecifier->domainId) {
        MmsDomain* itemDomain = MmsDevice_getDomain(device, accessSpecifier->domainId);

        if (itemDomain) {
            variableSpec = MmsDomain_getNamedVariable(itemDomain, accessSpecifier->itemId);
        }
    }
    else {
        variableSpec = MmsDevice_getNamedVariable(device, (char*) accessSpecifier->itemId);
    }

    if (variableSpec == NULL)
        return false;

    if (accessSpecifier->arrayIndex != -1) {
        if (variableSpec->type != MMS_ARRAY)
            return false;

        if (accessSpecifier->arrayIndex >= variableSpec->typeSpec.array.elementCount)
            return false;

        if (accessSpecifier->componentName != NULL) {
            if (MmsVariableSpecification_getNamedVariableRecursive(variableSpec, accessSpecifier->componentName) == NULL)
                return false;
        }
    }

    return true;
}

static MmsNamedVariableList
createNamedVariableList(MmsDomain* domain, MmsDevice* device,
		LinkedList itemList,
		char* variableListName, MmsError* mmsError)
{
    MmsNamedVariableList namedVariableList = NULL;

	int variableCount = LinkedList_size(itemList);

	if ((variableCount == 0 ) || (variableCount > CONFIG_MMS_MAX_NUMBER_OF_DATA_SET_MEMBERS)) {
	    *mmsError = MMS_ERROR_DEFINITION_OTHER;
	    goto exit_function;
	}

	/* check if all variables exist and are accessible */

	LinkedList itemElem = LinkedList_getNext(itemList);

	while (itemElem) {
	    MmsVariableAccessSpecification* accessSpec = (MmsVariableAccessSpecification*) LinkedList_getData(itemElem);

	    if (checkIfVariableExists(device, accessSpec) == false) {
	        *mmsError = MMS_ERROR_DEFINITION_OBJECT_UNDEFINED;
	        goto exit_function;
	    }

	    itemElem = LinkedList_getNext(itemElem);
	}

	/* all variables exit -> now create the new list */

	namedVariableList = MmsNamedVariableList_create(domain, variableListName, true);

	itemElem = LinkedList_getNext(itemList);

    while (itemElem) {
        MmsVariableAccessSpecification* accessSpec = (MmsVariableAccessSpecification*) LinkedList_getData(itemElem);

        MmsAccessSpecifier accessSpecifier;

        if (accessSpec->domainId) {
            accessSpecifier.domain = MmsDevice_getDomain(device, accessSpec->domainId);
        }
        else {
            accessSpecifier.domain = NULL;
        }

        accessSpecifier.variableName = (char*) accessSpec->itemId;
        accessSpecifier.componentName = (char*) accessSpec->componentName;
        accessSpecifier.arrayIndex = accessSpec->arrayIndex;

        MmsNamedVariableListEntry variable =
                MmsNamedVariableListEntry_create(accessSpecifier);

        MmsNamedVariableList_addVariable(namedVariableList, variable);

        itemElem = LinkedList_getNext(itemElem);
    }

exit_function:
	return namedVariableList;
}

static bool parseStringWithMaxLength(MmsEndpointConnection connection,
        char *filename, int maxLength, uint8_t *buffer, int *bufPos,
        int maxBufPos, uint32_t invokeId)
{
    uint8_t tag = buffer[(*bufPos)++];
    int length;

    if (tag != 0x1a) {
        MmsEndpointConnection_sendRejectPdu(connection, &invokeId,
                MMS_ERROR_REJECT_INVALID_PDU);
        return false;
    }

    *bufPos = BerDecoder_decodeLength(buffer, &length, *bufPos, maxBufPos);

    if (*bufPos < 0) {
        MmsEndpointConnection_sendRejectPdu(connection, &invokeId,
                MMS_ERROR_REJECT_INVALID_PDU);
        return false;
    }

    if (length > maxLength) {
        MmsEndpointConnection_sendRejectPdu(connection, &invokeId,
                MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT);
        return false;
    }

    memcpy(filename, buffer + *bufPos, length);
    filename[length] = 0;
    *bufPos += length;

    return true;
}

static bool
parseAlternateAccess(MmsEndpointConnection connection,
        MmsVariableAccessSpecification* variableSpecifier,
        uint32_t invokeId,
        uint8_t* buffer, int bufPos, int maxBufPos)
{
    char component[256] = "";
    uint32_t index = -1;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if ((bufPos < 0) || (bufPos + length > maxBufPos)) {
            MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
            return false;;
        }

        switch(tag) {
        case 0x81: /* component */

            StringUtils_createStringFromBufferInBuffer(component, buffer + bufPos, length);

            MmsVariableAccessSpecification_setComponentName(variableSpecifier, component);

            bufPos += length;

            break;

        case 0x82: /* index */

            index = BerDecoder_decodeUint32(buffer, length, bufPos);

            variableSpecifier->arrayIndex = index;

            bufPos += length;

            break;

        case 0xa3: /* index-range */

            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: reject access by index-range");

            bufPos += length;

            return false;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* ignore unknown tag */
            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: unknown tag in alternate access specification: (%02x)\n", tag);

            bufPos += length;
            break;
        }
    }

    return true;
}

static bool
parseVariableSpecification(MmsEndpointConnection connection,
        MmsVariableAccessSpecification* variableSpecifier,
        uint32_t invokeId,
        uint8_t* buffer, int bufPos, int maxBufPos)
{
    int variableType = -1; /* 0 = vmd-specific, 1 = domain-specific, 2 = aa-specific */
    char variableName[256] = "";
    char variableDomain[256] = "";

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if ((bufPos < 0) || (bufPos + length > maxBufPos)) {
            MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
            return false;;
        }

        switch(tag) {
        case 0x80: /* variable-list-name vmd-specific */

            StringUtils_createStringFromBufferInBuffer(variableName, buffer + bufPos, length);

            variableType = 0;

            bufPos += length;

            break;

        case 0xa1: /* variable-list-name domain-specific */

            if (!parseStringWithMaxLength(connection, variableDomain, 64, buffer, &bufPos, bufPos + length, invokeId)) {
                return false;
            }

            if (!parseStringWithMaxLength(connection, variableName, 64, buffer, &bufPos, bufPos + length, invokeId)) {
                return false;
            }

            variableType = 1;

            break;

        case 0x82: /* variable-list-name aa-specific */

            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: aa-specific named variables not supported\n");

            return false;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* ignore unknown tag */
            if (DEBUG_MMS_SERVER)
                printf("MMS_SERVER: unknown tag in variable specification: (%02x)\n", tag);

            bufPos += length;
            break;
        }
    }

	MmsVariableAccessSpecification_setItemId(variableSpecifier, variableName);
	MmsVariableAccessSpecification_setDomainId(variableSpecifier, variableDomain);

    return true;
}

static bool
parseListOfVariableSpecifications(MmsEndpointConnection connection,
        LinkedList listOfVariables,
        uint32_t invokeId,
        uint8_t* buffer, int bufPos, int maxBufPos)
{
    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if ((bufPos < 0) || (bufPos + length > maxBufPos)) {
            MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
            return false;
        }

        switch(tag) {
        case 0x30: /* listOfVariable item */
            {
                int subElementMaxBufPos = bufPos + length;

                MmsVariableAccessSpecification* variableSpecifier = NULL;

                while (bufPos < subElementMaxBufPos) {

                    uint8_t subTag = buffer[bufPos++];
                    int subLength;

                    bufPos = BerDecoder_decodeLength(buffer, &subLength, bufPos, bufPos + length);

                    if ((bufPos < 0) || (bufPos + subLength > bufPos + length)) {
                        MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
                        return false;
                    }

                    switch (subTag) {

                    case 0xa0: /* variableSpecification */

                        if (variableSpecifier == NULL)
							variableSpecifier = MmsVariableAccessSpecification_create(NULL, NULL);

                        if (!parseVariableSpecification(connection, variableSpecifier, invokeId, buffer, bufPos, bufPos + subLength)) {
                            MmsVariableAccessSpecification_destroy(variableSpecifier);
                            return false;
                        }

                        bufPos += subLength;

                        break;

                    case 0xa5: /* alternateAccess */

                        if (!parseAlternateAccess(connection, variableSpecifier, invokeId, buffer, bufPos, bufPos + subLength)) {
                            MmsVariableAccessSpecification_destroy(variableSpecifier);
                            return false;
                        }

                        bufPos += subLength;

                        break;

                    case 0x00: /* indefinite length end tag -> ignore */
                        break;
                    default: /* ignore unknown tag */
                        if (DEBUG_MMS_SERVER)
                            printf("MMS_SERVER: unknown sub element tag: (%02x)\n", tag);

                        bufPos += subLength;
                        break;
                    }
                }

                if (variableSpecifier)
                    LinkedList_add(listOfVariables, variableSpecifier);
            }
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

    return true;
}

bool
mmsServer_parseDefineNamedVariableListRequest(MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId,
        char* variableListName,
        char* variableListDomain,
        int* variableListType,
        LinkedList* listOfVariables)
{
    *listOfVariables = LinkedList_create();
    *variableListType = -1;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int length;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

        if ((bufPos < 0) || (bufPos + length > maxBufPos)) {
            MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
            goto exit_error;
        }

        switch(tag) {
        case 0x80: /* variable-list-name vmd-specific */
            StringUtils_createStringFromBufferInBuffer(variableListName, buffer + bufPos, length);

            *variableListType = 0;

            bufPos += length;

            break;

        case 0xa1: /* variable-list-name domain-specific */
            if (!parseStringWithMaxLength(connection, variableListDomain, 64, buffer, &bufPos, bufPos + length, invokeId)) {
                goto exit_error;
            }

            if (!parseStringWithMaxLength(connection, variableListName, 64, buffer, &bufPos, bufPos + length, invokeId)) {
                goto exit_error;
            }

            *variableListType = 1;

            break;

        case 0x82: /* variable-list-name aa-specific */
            StringUtils_createStringFromBufferInBuffer(variableListName, buffer + bufPos, length);

            *variableListType = 2;

            bufPos += length;

            break;

        case 0xa0: /* list-of-variable */
            if (!parseListOfVariableSpecifications(connection, *listOfVariables, invokeId, buffer, bufPos, bufPos + length)) {
                MmsEndpointConnection_sendRejectPdu(connection, &invokeId, MMS_ERROR_REJECT_INVALID_PDU);
                goto exit_error;
            }

            bufPos += length;

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

exit_function:
    return true;

exit_error:

    LinkedList_destroyDeep(*listOfVariables, (LinkedListValueDeleteFunction) MmsVariableAccessSpecification_destroy);
    *listOfVariables = NULL;

    return false;
}

void
mmsServer_handleDefineNamedVariableListRequest(
        MmsEndpointConnection connection,
        uint8_t* buffer, int bufPos, int maxBufPos,
        uint32_t invokeId)
{
    int variableListType = -1; /* 0 = vmd-specific, 1 = domain-specific, 2 = aa-specific */
    char variableListName[256] = "";
    char variableListDomain[256] = "";
    LinkedList listOfVariables = NULL;

    if (mmsServer_parseDefineNamedVariableListRequest(connection,
            buffer, bufPos, maxBufPos, invokeId,
            variableListName, variableListDomain, &variableListType, &listOfVariables))
    {
        /* message parsing successful -> now we try to create the new named variable list */

        MmsDevice* device = MmsServer_getDevice(connection->serverConnection->server);

        if (variableListType == 0 /* vmd-specific */) {
#if (CONFIG_MMS_ALLOW_DEFINITION_OF_VMD_SPECIFIC_DATA_SETS == 1)
            LinkedList vmdScopeNVLs = MmsDevice_getNamedVariableLists(connection->serverConnection->server->device);

            if (LinkedList_size(vmdScopeNVLs) < CONFIG_MMS_MAX_NUMBER_OF_VMD_SPECIFIC_DATA_SETS) {

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) - vmd specific %s", invokeId, variableListName);

                if (mmsServer_getNamedVariableListWithName(MmsDevice_getNamedVariableLists(connection->serverConnection->server->device), variableListName) != NULL) {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) object already exists --> service-error: definition-object-exists", invokeId);

                    MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_DEFINITION_OBJECT_EXISTS);

                    goto exit_function;
                }
                else {
                    MmsError mmsError;

                    MmsNamedVariableList namedVariableList = createNamedVariableList(NULL, device,
                            listOfVariables, variableListName, &mmsError);

                    if (namedVariableList != NULL) {
                        if (mmsServer_callVariableListChangedHandler(true, MMS_VMD_SPECIFIC, NULL, variableListName, connection)
                                == MMS_ERROR_NONE) {
                            LinkedList_add(vmdScopeNVLs, (void*) namedVariableList);

                            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                            if (response) {
                                createDefineNamedVariableListResponse(invokeId, response);

                                MmsEndpointConnection_sendMessage(connection, response);

                                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                            }
                        }
                        else {
                            MmsNamedVariableList_destroy(namedVariableList);

                            if (DEBUG_MMS_SERVER)
                                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) access denied --> service-error: object-access-denied", invokeId);

                            MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED);

                            goto exit_function;
                        }
                    }
                }
            }
            else {
                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) too much data sets --> service-error: resource-capability-unavailable", invokeId);

                MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_RESOURCE_CAPABILITY_UNAVAILABLE);

                goto exit_function;
            }
#else
            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) VMD specific data set not allowed --> service-error: definition-type-unsupported", invokeId);

            MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_DEFINITION_TYPE_UNSUPPORTED);

            goto exit_function;
#endif /* (CONFIG_MMS_ALLOW_DEFINITION_OF_VMD_SPECIFIC_DATA_SETS == 1) */
        }
        else if (variableListType == 1 /* domain-specific */) {

            MmsDomain* domain = MmsDevice_getDomain(device, variableListDomain);

            if (domain == NULL) {
                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) domain not found --> service-error: object-non-existent", invokeId);

                MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                goto exit_function;
            }

            if (LinkedList_size(domain->namedVariableLists) < CONFIG_MMS_MAX_NUMBER_OF_DOMAIN_SPECIFIC_DATA_SETS) {

                if (MmsDomain_getNamedVariableList(domain, variableListName) != NULL) {

                    if (DEBUG_MMS_SERVER)
                         libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) named variable list already exists --> service-error: object-exists", invokeId);

                    MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_DEFINITION_OBJECT_EXISTS);

                    goto exit_function;
                }

                MmsError mmsError;

                MmsNamedVariableList namedVariableList = createNamedVariableList(domain, device,
                        listOfVariables, variableListName, &mmsError);

                if (namedVariableList != NULL) {

                    mmsError = mmsServer_callVariableListChangedHandler(true, MMS_DOMAIN_SPECIFIC, namedVariableList, connection);

                    if (mmsError == MMS_ERROR_NONE) {
                        MmsDomain_addNamedVariableList(domain, namedVariableList);

                        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                        if (response) {
                            createDefineNamedVariableListResponse(invokeId, response);

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
                            connection->endpoint->statAplMessagesSent++;
#endif

                            MmsEndpointConnection_sendMessage(connection, response);

                            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                        }
                    }
                    else {
                        MmsNamedVariableList_destroy(namedVariableList);

                        if (DEBUG_MMS_SERVER)
                            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) failed to create data set --> service-error: %i", invokeId, (int) mmsError);

                        MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, mmsError);

                        goto exit_function;
                    }
                }
                else {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) failed to create data set --> service-error: %i", invokeId, (int) mmsError);

                    MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, mmsError);

                    goto exit_function;
                }
            }
            else {
                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) too much data sets --> service-error: resource-capability-unavailable", invokeId);

                MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_RESOURCE_CAPABILITY_UNAVAILABLE);

                goto exit_function;
            }
        }
        else if (variableListType == 2 /* aa-specific */) {
            if (LinkedList_size(connection->serverConnection->namedVariableLists) < CONFIG_MMS_MAX_NUMBER_OF_ASSOCIATION_SPECIFIC_DATA_SETS) {

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) - association specific %s", invokeId, variableListName);

                if (MmsEndpointServerConnection_getNamedVariableList(connection->serverConnection, variableListName) != NULL) {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) object already exists --> service-error: definition-object-exists", invokeId);

                    MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_DEFINITION_OBJECT_EXISTS);

                    goto exit_function;
                }
                else {
                    MmsError mmsError;

                    MmsNamedVariableList namedVariableList = createNamedVariableList(NULL, device,
                            listOfVariables, variableListName, &mmsError);

                    if (namedVariableList != NULL) {

                        if (mmsServer_callVariableListChangedHandler(true, MMS_ASSOCIATION_SPECIFIC, namedVariableList, connection) == MMS_ERROR_NONE) {
                            MmsEndpointServerConnection_addNamedVariableList(connection->serverConnection, namedVariableList);

                            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                            if (response) {
                                createDefineNamedVariableListResponse(invokeId, response);

                                MmsEndpointConnection_sendMessage(connection, response);

                                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                            }
                        }
                        else {
                            MmsNamedVariableList_destroy(namedVariableList);

                            if (DEBUG_MMS_SERVER)
                                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) access denied--> service-error: object-access-denied", invokeId);

                            MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED);

                            goto exit_function;
                        }
                    }
                    else {

                        if (DEBUG_MMS_SERVER)
                            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) failed to create data set --> service-error: %i", invokeId, (int) mmsError);

                        MmsEndpointConnection_sendServiceErrorPdu(connection, invokeId, mmsError);

                        goto exit_function;
                    }
                }
            }
            else {

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: define-named-variable-list (invoke-id: %u) too much data sets --> service-error: resource-capability-unavailable", invokeId);

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_RESOURCE_CAPABILITY_UNAVAILABLE);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }
            }
        }
    }

exit_function:

    if (listOfVariables)
        LinkedList_destroyDeep(listOfVariables, (LinkedListValueDeleteFunction) MmsVariableAccessSpecification_destroy);

    return;
}

#endif /* (MMS_DYNAMIC_DATA_SETS == 1) */

#if (MMS_GET_DATA_SET_ATTRIBUTES == 1)

static void
createGetNamedVariableListAttributesResponse(MmsEndpointConnection connection, int invokeId, ByteBuffer* response, MmsNamedVariableList variableList)
{
    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize(invokeId) + 2;

    uint32_t confirmedResponsePDUContentSize = invokeIdSize /* + deleteNVLSize */;

    uint32_t isDeletableSize = 3;

    uint32_t listOfVariableContentSize = 0;

    LinkedList variableElem = LinkedList_getNext(MmsNamedVariableList_getVariableList(variableList));

    while (variableElem) {
        MmsNamedVariableListEntry variableEntry = (MmsNamedVariableListEntry) LinkedList_getData(variableElem);

        uint32_t varibaleSpecificationNameSize = 0;

        if (variableEntry->domain) { /* domain specific variable */
            int itemIdSize = strlen(variableEntry->variableName);

            uint32_t nameLengthSize = BerEncoder_determineLengthSize(itemIdSize);

            uint32_t nameSize = 1 + nameLengthSize + itemIdSize;

            int domainIdSize = strlen(variableEntry->domain->domainName);

            uint32_t domainLengthSize = BerEncoder_determineLengthSize(domainIdSize);

            uint32_t domainSize = 1 + domainLengthSize + domainIdSize;

            varibaleSpecificationNameSize = 1 +  BerEncoder_determineLengthSize(nameSize + domainSize) + nameSize + domainSize;
        }
        else { /* VMD specific variable */
            int itemIdSize = strlen(variableEntry->variableName);

            uint32_t nameLengthSize = BerEncoder_determineLengthSize(itemIdSize);

            uint32_t nameSize = 1 + nameLengthSize + itemIdSize;

            varibaleSpecificationNameSize = nameSize;
        }

        varibaleSpecificationNameSize = 1 + BerEncoder_determineLengthSize(varibaleSpecificationNameSize) + varibaleSpecificationNameSize;

        uint32_t listOfVariableItemSize = 1 + BerEncoder_determineLengthSize(varibaleSpecificationNameSize) + varibaleSpecificationNameSize;

        /* TODO add component and index */

        listOfVariableContentSize += listOfVariableItemSize;

        variableElem = LinkedList_getNext(variableElem);
    }

    uint32_t listOfVariabletSize = 1 + BerEncoder_determineLengthSize(listOfVariableContentSize) + listOfVariableContentSize;

    uint32_t confirmedResponseContentSize = isDeletableSize + listOfVariabletSize;

    uint32_t confirmedResponseSize = 1 + BerEncoder_determineLengthSize(confirmedResponseContentSize) + confirmedResponseContentSize;

    confirmedResponsePDUContentSize += confirmedResponseSize;

    uint32_t confirmedResponsePDUSize = 1 + BerEncoder_determineLengthSize(confirmedResponsePDUContentSize) + confirmedResponsePDUContentSize;

    /* check if message fits in MMS PDU */
    if (confirmedResponsePDUSize > connection->maxPduSize) {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) response does not fit in MMS PDU --> service-error: resource-capability-unavailable", invokeId);

        mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_RESOURCE_CAPABILITY_UNAVAILABLE);
    }
    else {
        int bufPos = 0;
        uint8_t* buffer = response->buffer;

        bufPos = BerEncoder_encodeTL(0xa1, confirmedResponsePDUContentSize, buffer, bufPos);

        bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
        bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

        bufPos = BerEncoder_encodeTL(0xac, confirmedResponseContentSize, buffer, bufPos);

        bufPos = BerEncoder_encodeBoolean(0x80, MmsNamedVariableList_isDeletable(variableList), buffer, bufPos);

        bufPos = BerEncoder_encodeTL(0xa1, listOfVariableContentSize, buffer, bufPos);

        variableElem = LinkedList_getNext(MmsNamedVariableList_getVariableList(variableList));

        while (variableElem) {
            MmsNamedVariableListEntry variableEntry = (MmsNamedVariableListEntry) LinkedList_getData(variableElem);

            uint32_t varibaleSpecificationNameSize = 0;
            uint32_t variableSpecificationContentSize = 0;

            int itemIdSize = 0;
            int domainIdSize = 0;

            if (variableEntry->domain) { /* domain specific variable */
                itemIdSize = strlen(variableEntry->variableName);

                uint32_t nameLengthSize = BerEncoder_determineLengthSize(itemIdSize);

                uint32_t nameSize = 1 + nameLengthSize + itemIdSize;

                domainIdSize = strlen(variableEntry->domain->domainName);

                uint32_t domainLengthSize = BerEncoder_determineLengthSize(domainIdSize);

                uint32_t domainSize = 1 + domainLengthSize + domainIdSize;

                variableSpecificationContentSize = nameSize + domainSize;

                varibaleSpecificationNameSize = 1 +  BerEncoder_determineLengthSize(nameSize + domainSize) + nameSize + domainSize;
            }
            else { /* VMD specific variable */
                itemIdSize = strlen(variableEntry->variableName);

                uint32_t nameLengthSize = BerEncoder_determineLengthSize(itemIdSize);

                uint32_t nameSize = 1 + nameLengthSize + itemIdSize;

                varibaleSpecificationNameSize = nameSize;

                variableSpecificationContentSize = itemIdSize;
            }

            uint32_t listOfVariableItemContentSize = 1 + BerEncoder_determineLengthSize(varibaleSpecificationNameSize) + varibaleSpecificationNameSize;

            /* TODO add component and index */

            bufPos = BerEncoder_encodeTL(0x30, listOfVariableItemContentSize, buffer, bufPos);
            bufPos = BerEncoder_encodeTL(0xa0, varibaleSpecificationNameSize, buffer, bufPos);

            if (variableEntry->domain) {
                bufPos = BerEncoder_encodeTL(0xa1, variableSpecificationContentSize, buffer, bufPos);
                bufPos = BerEncoder_encodeStringWithTag(0x1a, variableEntry->domain->domainName, buffer, bufPos);
                bufPos = BerEncoder_encodeStringWithTag(0x1a, variableEntry->variableName, buffer, bufPos);
            }
            else {
                bufPos = BerEncoder_encodeStringWithTag(0x80, variableEntry->variableName, buffer, bufPos);
            }

            variableElem = LinkedList_getNext(variableElem);
        }

        response->size = bufPos;
    }
}

void
mmsServer_handleGetNamedVariableListAttributesRequest(
		MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId)
{
	GetNamedVariableListAttributesRequest_t* request = 0;

	asn_dec_rval_t rval = ber_decode(NULL, &asn_DEF_GetNamedVariableListAttributesRequest,
				(void**) &request, buffer + bufPos, maxBufPos - bufPos);

	if (rval.code != RC_OK) {
        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_INVALID_PDU, response);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

	    goto exit_function;
	}

	if (request->present == ObjectName_PR_domainspecific) {

	    char domainName[65];
	    char itemName[65];

	    if ((request->choice.domainspecific.domainId.size > 64) ||
	        (request->choice.domainspecific.itemId.size > 64)) {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) item name too long --> service-error: object-attribute-inconsistent", invokeId);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_DEFINITION_OBJECT_ATTRIBUTE_INCONSISTENT);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

	        goto exit_function;
	    }

	    StringUtils_createStringFromBufferInBuffer(domainName, request->choice.domainspecific.domainId.buf,
	            request->choice.domainspecific.domainId.size);

	    StringUtils_createStringFromBufferInBuffer(itemName, request->choice.domainspecific.itemId.buf,
                request->choice.domainspecific.itemId.size);

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) domain specific %s:%s",
                    invokeId, domainName, itemName);

		MmsDevice* mmsDevice = MmsServer_getDevice(connection->serverConnection->server);

		MmsDomain* domain = MmsDevice_getDomain(mmsDevice, domainName);

		MmsNamedVariableList variableList = NULL;

		if (domain)
		    variableList = MmsDomain_getNamedVariableList(domain, itemName);

		if (domain && variableList) {
            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                createGetNamedVariableListAttributesResponse(connection, invokeId, response, variableList);

                MmsEndpointConnection_sendMessage(connection, response);

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
                connection->endpoint->statAplMessagesSent++;
#endif

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
		}
		else {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) item not found --> service-error: object-non-existent", invokeId);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
		}

	}
#if (MMS_DYNAMIC_DATA_SETS == 1)
	else if (request->present == ObjectName_PR_aaspecific) {

	    char listName[65];

        if (request->choice.aaspecific.size > 64) {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) item name too long --> service-error: object-attribute-inconsistent", invokeId);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OTHER);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

            goto exit_function;
        }

	    StringUtils_createStringFromBufferInBuffer(listName, request->choice.aaspecific.buf,
	            request->choice.aaspecific.size);

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) association specific %s",
                    invokeId, listName);

	    MmsNamedVariableList varList = MmsEndpointServerConnection_getNamedVariableList(connection->serverConnection, listName);

	    if (varList != NULL) {
            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                createGetNamedVariableListAttributesResponse(connection, invokeId, response, varList);

                MmsEndpointConnection_sendMessage(connection, response);

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
                connection->endpoint->statAplMessagesSent++;
#endif

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
	    }
	    else {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) item not found --> service-error: object-non-existent", invokeId);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
	    }
	}
#endif /* (MMS_DYNAMIC_DATA_SETS == 1) */
	else if (request->present == ObjectName_PR_vmdspecific) {
	    char listName[65];

	    if (request->choice.vmdspecific.size > 64) {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) item name too long --> service-error: object-attribute-inconsistent", invokeId);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OTHER);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

            goto exit_function;
	    }

	    StringUtils_createStringFromBufferInBuffer(listName, request->choice.vmdspecific.buf,
                request->choice.vmdspecific.size);

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) vmd specific %s",
                    invokeId, listName);

	    MmsDevice* mmsDevice = MmsServer_getDevice(connection->serverConnection->server);

	    MmsNamedVariableList varList = mmsServer_getNamedVariableListWithName(mmsDevice->namedVariableLists, listName);

        if (varList != NULL) {
            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                createGetNamedVariableListAttributesResponse(connection, invokeId, response, varList);

                MmsEndpointConnection_sendMessage(connection, response);

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
                connection->endpoint->statAplMessagesSent++;
#endif

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
        }
        else {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) item not found --> service-error: object-non-existent", invokeId);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
        }
	}
	else {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get-named-variable-list-attributes (invoke-id: %u) --> service-error: object-access-unsupported", invokeId);

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }
	}

exit_function:

	asn_DEF_GetVariableAccessAttributesRequest.free_struct(&asn_DEF_GetNamedVariableListAttributesRequest,
			request, 0);
}

#endif /* (MMS_GET_DATA_SET_ATTRIBUTES == 1) */

#endif /* (MMS_DATA_SET_SERVICE == 1) */
