/*
 *  mms_get_namelist_service.c
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
#include "ber_encoder.h"
#include "ber_decode.h"

#if (MMS_GET_NAME_LIST == 1)

/**********************************************************************************************
 * MMS GetNameList Service
 *********************************************************************************************/

#ifndef CONFIG_MMS_SORT_NAME_LIST
#define CONFIG_MMS_SORT_NAME_LIST 1
#endif

#define OBJECT_CLASS_NAMED_VARIABLE 0
#define OBJECT_CLASS_NAMED_VARIABLE_LIST 2
#define OBJECT_CLASS_JOURNAL 8
#define OBJECT_CLASS_DOMAIN 9

#define OBJECT_SCOPE_VMD 0
#define OBJECT_SCOPE_DOMAIN 1
#define OBJECT_SCOPE_ASSOCIATION 2

static LinkedList
getDomainNames(MmsEndpointConnection connection)
{
	MmsDevice* device = MmsServer_getDevice(connection->serverConnection->server);

	LinkedList list = LinkedList_create();

	int i;

	for (i = 0; i < device->domainCount; i++) {
		LinkedList_add(list, device->domains[i]->domainName);
	}

	return list;
}

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
static void
sortIndex(int* index, int size, MmsVariableSpecification** namedVariables)
{
    int n;
    int i;

    for (n = size; n > 1; n = n - 1) {
        for (i = 0; i < n - 1; i = i + 1) {
            if (StringUtils_compareStrings(namedVariables[index[i]]->name, namedVariables[index[i + 1]]->name) > 0) {
                int storedIndex = index[i];
                index[i] = index[i + 1];
                index[i + 1] = storedIndex;
            }
        }
    }
}
#endif /* (CONFIG_MMS_SORT_NAME_LIST == 1) */

#if (CONFIG_MMS_SUPPORT_FLATTED_NAME_SPACE == 1)
static char*
appendMmsSubVariable(char* name, char* child)
{
    int nameLen = strlen(name);
    int childLen = strlen(child);

    int newSize = nameLen + childLen + 2;

    char* newName = (char*) GLOBAL_MALLOC(newSize);

    int bufPos = 0;
    int i;
    for (i = 0; i < nameLen; i++) {
        newName[bufPos++] = name[i];
    }
    newName[bufPos++] = CONFIG_MMS_SEPARATOR_CHAR;

    for (i = 0; i < childLen; i++) {
        newName[bufPos++] = child[i];
    }

    newName[bufPos] = 0;

    return newName;
}

static LinkedList
addSubNamedVaribleNamesToList(LinkedList nameList, char* prefix, MmsVariableSpecification* variable)
{
    LinkedList listElement = nameList;

    if (variable->type == MMS_STRUCTURE) {

        int i;

        MmsVariableSpecification** variables = variable->typeSpec.structure.elements;

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
        int* index = (int*) GLOBAL_MALLOC(sizeof(int) * variable->typeSpec.structure.elementCount);

        for (i = 0; i < variable->typeSpec.structure.elementCount; i++)
            index[i] = i;

        sortIndex(index, variable->typeSpec.structure.elementCount, variables);
#endif /* (CONFIG_MMS_SORT_NAME_LIST == 1) */

        for (i = 0; i < variable->typeSpec.structure.elementCount; i++) {
#if (CONFIG_MMS_SORT_NAME_LIST == 1)
            char* variableName = appendMmsSubVariable(prefix, variables[index[i]]->name);
#else
            char* variableName = appendMmsSubVariable(prefix, variables[i]->name);
#endif /* (CONFIG_MMS_SORT_NAME_LIST == 1) */

            listElement = LinkedList_insertAfter(listElement, variableName);

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
            listElement = addSubNamedVaribleNamesToList(listElement, variableName, variables[index[i]]);
#else
            listElement = addSubNamedVaribleNamesToList(listElement, variableName, variables[i]);
#endif /* (CONFIG_MMS_SORT_NAME_LIST == 1) */
        }

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
        GLOBAL_FREEMEM(index);
#endif
    }

    return listElement;
}

#endif /* (CONFIG_MMS_SUPPORT_FLATTED_NAME_SPACE == 1) */

static LinkedList
getJournalListDomainSpecific(MmsEndpointConnection connection, char* domainName)
{
    MmsDevice* device = MmsServer_getDevice(connection->serverConnection->server);

    LinkedList nameList = NULL;

    MmsDomain* domain = MmsDevice_getDomain(device, domainName);

    if (domain != NULL) {
        nameList = LinkedList_create();

        if (domain->journals != NULL) {

            LinkedList journalList = domain->journals;

            while ((journalList = LinkedList_getNext(journalList)) != NULL) {

                MmsJournal journal = (MmsJournal) LinkedList_getData(journalList);

                LinkedList_add(nameList, (void*) journal->name);
            }

        }
    }

    return nameList;
}

#if (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1)
static LinkedList
getNameListVMDSpecific(MmsEndpointConnection connection)
{
    MmsDevice* device = MmsServer_getDevice(connection->serverConnection->server);

    LinkedList list = LinkedList_create();

    MmsVariableSpecification** variables = device->namedVariables;

    int i;

    LinkedList element = list;

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
    int* index = (int*) GLOBAL_MALLOC(sizeof(int) * device->namedVariablesCount);

    for (i = 0; i < device->namedVariablesCount; i++)
        index[i] = i;

    sortIndex(index, device->namedVariablesCount, device->namedVariables);
#endif /* (CONFIG_MMS_SORT_NAME_LIST == 1) */

    for (i = 0; i < device->namedVariablesCount; i++) {

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
        element = LinkedList_insertAfter(element, StringUtils_copyString(variables[index[i]]->name));
#else
        element = LinkedList_insertAfter(element, StringUtils_copyString(variables[i]->name));
#endif

    }

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
    GLOBAL_FREEMEM(index);
#endif

    return list;
}
#endif /* (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1) */

static LinkedList
getNameListDomainSpecific(MmsEndpointConnection connection, char* domainName)
{
    MmsDevice* device = MmsServer_getDevice(connection->serverConnection->server);

    LinkedList nameList = NULL;

    MmsDomain* domain = MmsDevice_getDomain(device, domainName);

    if (domain != NULL) {
        nameList = LinkedList_create();
        MmsVariableSpecification** variables = domain->namedVariables;

        int i;

        LinkedList element = nameList;

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
        int* index = (int*) GLOBAL_MALLOC(sizeof(int) * domain->namedVariablesCount);

        for (i = 0; i < domain->namedVariablesCount; i++)
            index[i] = i;

        sortIndex(index, domain->namedVariablesCount, domain->namedVariables);
#endif /* (CONFIG_MMS_SORT_NAME_LIST == 1) */

        for (i = 0; i < domain->namedVariablesCount; i++) {

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
            element = LinkedList_insertAfter(element, StringUtils_copyString(variables[index[i]]->name));
#else
            element = LinkedList_insertAfter(element, StringUtils_copyString(variables[i]->name));
#endif

#if (CONFIG_MMS_SUPPORT_FLATTED_NAME_SPACE == 1)
#if (CONFIG_MMS_SORT_NAME_LIST == 1)
            char* prefix = variables[index[i]]->name;
            element = addSubNamedVaribleNamesToList(element, prefix, variables[index[i]]);
#else
            char* prefix = variables[i]->name;
            element = addSubNamedVaribleNamesToList(element, prefix, variables[i]);
#endif /* (CONFIG_MMS_SORT_NAME_LIST == 1) */
#endif /* (CONFIG_MMS_SUPPORT_FLATTED_NAME_SPACE == 1) */

        }

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
        GLOBAL_FREEMEM(index);
#endif
    }

    return nameList;
}


#if (MMS_DATA_SET_SERVICE == 1)

static LinkedList
createStringsFromNamedVariableList(LinkedList nameList, LinkedList variableLists)
{
    nameList = LinkedList_create();
    LinkedList variableListsElement = LinkedList_getNext(variableLists);
    while (variableListsElement != NULL) {
        MmsNamedVariableList variableList =
                (MmsNamedVariableList) variableListsElement->data;

        LinkedList_add(nameList,
                StringUtils_copyString(MmsNamedVariableList_getName(variableList)));

        variableListsElement = LinkedList_getNext(variableListsElement);
    }
    return nameList;
}

static LinkedList
getNamedVariableListsDomainSpecific(MmsEndpointConnection connection, char* domainName)
{
    MmsDevice* device = MmsServer_getDevice(connection->serverConnection->server);

    LinkedList nameList = NULL;

    MmsDomain* domain = MmsDevice_getDomain(device, domainName);

    if (domain != NULL) {
        LinkedList variableLists = MmsDomain_getNamedVariableLists(domain);

        nameList = createStringsFromNamedVariableList(nameList, variableLists);
    }

    return nameList;
}

static LinkedList
getNamedVariableListsVMDSpecific(MmsEndpointConnection connection)
{
    MmsDevice* device = MmsServer_getDevice(connection->serverConnection->server);

    LinkedList nameList = NULL;

    LinkedList variableLists = MmsDevice_getNamedVariableLists(device);

    nameList = createStringsFromNamedVariableList(nameList, variableLists);

    return nameList;
}

#if (MMS_DYNAMIC_DATA_SETS == 1)
static LinkedList
getNamedVariableListAssociationSpecific(MmsEndpointConnection connection)
{
	LinkedList nameList = NULL;

	LinkedList variableLists = MmsEndpointServerConnection_getNamedVariableLists(connection->serverConnection);

	nameList = createStringsFromNamedVariableList(nameList, variableLists);

	return nameList;
}
#endif /* (MMS_DYNAMIC_DATA_SETS == 1) */

#endif /* (MMS_DATA_SET_SERVICE == 1) */

static void
createNameListResponse(
        MmsEndpointConnection connection,
        uint32_t invokeId,
        LinkedList nameList,
        ByteBuffer* response,
        char* continueAfter)
{
    LinkedList startElement = NULL;

    if (continueAfter != NULL) {
        LinkedList element = nameList;

        while ((element = LinkedList_getNext(element)) != NULL) {
            if (strcmp((char*) (element->data), continueAfter) == 0) {
                startElement = element;
                break;
            }
        }

        if (startElement == NULL) {
            mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);
            return;
        }
    }

    /* determine number of identifiers to include in response */
    if (startElement == NULL)
        startElement = nameList;

    int nameCount = 0;
    int estimatedMmsPduLength = 27; /* estimated overhead size of PDU encoding */
    int maxPduSize = connection->maxPduSize;

    bool moreFollows = false;

    LinkedList element = startElement;

    uint32_t identifierListSize = 0;

    while ((element = LinkedList_getNext(element)) != NULL) {
        int elementLength;

        elementLength = BerEncoder_determineEncodedStringSize((char*) element->data);

        if ((estimatedMmsPduLength + elementLength) > maxPduSize) {
            moreFollows = true;
            break;
        }
        else {
            estimatedMmsPduLength += elementLength;
            identifierListSize += elementLength;
            nameCount++;
        }

    }

    uint32_t listOfIdentifierSize = 1 + BerEncoder_determineLengthSize(identifierListSize) + identifierListSize;

    uint32_t getNameListSize = listOfIdentifierSize;

    if (moreFollows == false)
    	getNameListSize += 3;

    uint32_t confirmedServiceResponseSize = 1 + BerEncoder_determineLengthSize(getNameListSize) + getNameListSize;

    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize((uint32_t) invokeId) + 2;

    uint32_t confirmedResponsePDUSize = confirmedServiceResponseSize + invokeIdSize;

    /* encode response */
    element = startElement;

    uint8_t* buffer = response->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0xa1, confirmedResponsePDUSize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0xa1, getNameListSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0xa0, identifierListSize, buffer, bufPos);

    int i = 0;

    while ((element = LinkedList_getNext(element)) != NULL) {
        bufPos = BerEncoder_encodeStringWithTag(0x1a, (char*) element->data, buffer, bufPos);

        i++;

        if (i == nameCount)
            break;
    }

    if (moreFollows == false)
    	bufPos = BerEncoder_encodeBoolean(0x81, moreFollows, buffer, bufPos);

    response->size = bufPos;

    if (DEBUG_MMS_SERVER)
        libmms_log(MMS_LOG_DEBUG, connection, "MMS_SERVER: get name list (invoke-id: %u) --> response (size: %i)", invokeId, response->size);
}

void
mmsServer_handleGetNameListRequest(
        MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId)
{
	int objectClass = -1;

	int objectScope = -1;

	char* domainId = NULL;
	int domainIdLength;

	char* continueAfter = NULL;
	int continueAfterLength;

	while (bufPos < maxBufPos) {
		uint8_t tag = buffer[bufPos++];
		int length;

		bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

		if (bufPos < 0)  {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get name list (invoke-id: %u) - invalid PDU", invokeId);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_INVALID_PDU, response);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

			return;
		}

		switch (tag) {

		case 0xa0: /* objectClass */
			bufPos++;
			length = buffer[bufPos++];
			objectClass = BerDecoder_decodeUint32(buffer, length, bufPos);
			break;

		case 0xa1: /* objectScope */
			{
				uint8_t objectScopeTag = buffer[bufPos++];
				bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);

				switch (objectScopeTag) {
				case 0x80: /* vmd-specific */
					objectScope = OBJECT_SCOPE_VMD;
					break;
				case 0x81: /* domain-specific */
				    domainIdLength = length;
					domainId = (char*) (buffer + bufPos);
					objectScope = OBJECT_SCOPE_DOMAIN;
					break;
				case 0x82: /* association-specific */
					objectScope = OBJECT_SCOPE_ASSOCIATION;
					break;
			    case 0x00: /* indefinite length end tag -> ignore */
			        break;

				default:

		            if (DEBUG_MMS_SERVER)
		                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get name list (invoke-id: %u) with unrecognized object scope %02x", invokeId, objectScopeTag);

		            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

		            if (response) {
	                    mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_UNRECOGNIZED_MODIFIER, response);

		                MmsEndpointConnection_sendMessage(connection, response);

		                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
		            }

					return;
				}
			}
			break;

		case 0x82: /* continueAfter */
			continueAfter = (char*) (buffer + bufPos);
			continueAfterLength = length;
			break;

	    case 0x00: /* indefinite length end tag -> ignore */
	        break;

		default:
			/* ignore unknown tag */

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_WARNING, connection, "MMS_SERVER: get name list (invoke-id: %u) with unknown tag %02x", invokeId, tag);

			break;
		}

		bufPos += length;
	}

	char continueAfterIdMemory[130];
	char* continueAfterId = NULL;

	if (continueAfter != NULL) {
		continueAfterId = continueAfterIdMemory;
		memcpy(continueAfterId, continueAfter, continueAfterLength);
		continueAfterId[continueAfterLength] = 0;

		if (DEBUG_MMS_SERVER)
		    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get name list (invoke-id: %u) - continue after: (%s)", invokeId, continueAfterId);
	}

	if (objectScope == OBJECT_SCOPE_DOMAIN) {
		char domainSpecificName[130];

		memcpy(domainSpecificName, domainId, domainIdLength);
		domainSpecificName[domainIdLength] = 0;

		if (objectClass == OBJECT_CLASS_NAMED_VARIABLE) {
		    if (DEBUG_MMS_SERVER)
		        libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get name list (invoke-id: %u) for named variables in domain (%s)", invokeId, domainSpecificName);

			LinkedList nameList = getNameListDomainSpecific(connection, domainSpecificName);

			if (nameList == NULL) {

			    if (DEBUG_MMS_SERVER)
			        libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get name list (invoke-id: %u) for named variables in domain (%s) -> object-non-existent", invokeId, domainSpecificName);

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }
			}
			else {
                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    createNameListResponse(connection, invokeId, nameList, response, continueAfterId);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }

				LinkedList_destroy(nameList);
			}
		}
		else if (objectClass == OBJECT_CLASS_JOURNAL) {
		    LinkedList nameList = getJournalListDomainSpecific(connection, domainSpecificName);

            if (nameList == NULL) {

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get name list (invoke-id: %u) for journals in domain (%s) -> object-non-existent", invokeId, domainSpecificName);

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }
            }
            else {
#if (CONFIG_MMS_SORT_NAME_LIST == 1)
                StringUtils_sortList(nameList);
#endif

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    createNameListResponse(connection, invokeId, nameList, response, continueAfterId);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }

                LinkedList_destroyStatic(nameList);
            }
		}
#if (MMS_DATA_SET_SERVICE == 1)
		else if (objectClass == OBJECT_CLASS_NAMED_VARIABLE_LIST) {
			LinkedList nameList = getNamedVariableListsDomainSpecific(connection, domainSpecificName);

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get name list (invoke-id: %u) for named variable lists in domain (%s)", invokeId, domainSpecificName);

			if (nameList == NULL) {

			    if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: get name list (invoke-id: %u) for named variable lists in domain (%s) -> object-non-existent", invokeId, domainSpecificName);

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }
			}
			else {

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
				StringUtils_sortList(nameList);
#endif

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    createNameListResponse(connection, invokeId, nameList, response, continueAfterId);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }

                LinkedList_destroy(nameList);
			}
		}
#endif /* (MMS_DATA_SET_SERVICE == 1) */

		else {
			if (DEBUG_MMS_SERVER)
			    libmms_log(MMS_LOG_WARNING, connection, "MMS_SERVER: get name list (invoke-id: %u) for domain specific objectClass %i not supported!", invokeId, objectClass);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
		}
	}

	else if (objectScope == OBJECT_SCOPE_VMD) { /* vmd-specific */

	    if (objectClass == OBJECT_CLASS_DOMAIN) {

	         if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get name list (invoke-id: %u) for domain names", invokeId);

	        LinkedList nameList = getDomainNames(connection);

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
	        StringUtils_sortList(nameList);
#endif

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                createNameListResponse(connection, invokeId, nameList, response, continueAfterId);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

	        LinkedList_destroyStatic(nameList);
	    }

#if (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1)
	    else if (objectClass == OBJECT_CLASS_NAMED_VARIABLE) {

            if (DEBUG_MMS_SERVER)
               libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get name list (invoke-id: %u) for VMD specific named variables", invokeId);

            LinkedList nameList = getNameListVMDSpecific(connection);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                createNameListResponse(connection, invokeId, nameList, response, continueAfterId);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

            LinkedList_destroy(nameList);
	    }
#endif /* (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1) */

#if (MMS_DATA_SET_SERVICE == 1)
	    else if (objectClass == OBJECT_CLASS_NAMED_VARIABLE_LIST) {

            if (DEBUG_MMS_SERVER)
               libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get name list (invoke-id: %u) for VMD specific named variable lists", invokeId);

	        LinkedList nameList = getNamedVariableListsVMDSpecific(connection);

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
	        StringUtils_sortList(nameList);
#endif

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                createNameListResponse(connection, invokeId, nameList, response, continueAfterId);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

	        LinkedList_destroy(nameList);
	    }
#endif /* (MMS_DATA_SET_SERVICE == 1) */

	    else if (objectClass == OBJECT_CLASS_JOURNAL) {

            if (DEBUG_MMS_SERVER)
               libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get name list (invoke-id: %u) for VMD specific journals", invokeId);

	        LinkedList nameList = LinkedList_create();

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
	        StringUtils_sortList(nameList);
#endif

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                createNameListResponse(connection, invokeId, nameList, response, continueAfterId);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

	        LinkedList_destroy(nameList);
	    }

        else {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_WARNING, connection, "MMS_SERVER: get name list (invoke-id: %u) for VMD specific objectClass %i not supported!", invokeId, objectClass);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
        }
	}

#if (MMS_DATA_SET_SERVICE == 1)
#if (MMS_DYNAMIC_DATA_SETS == 1)
	else if (objectScope == OBJECT_SCOPE_ASSOCIATION) { /* association-specific */

		if (objectClass == OBJECT_CLASS_NAMED_VARIABLE_LIST) {

            if (DEBUG_MMS_SERVER)
               libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: get name list (invoke-id: %u) for association specific named variable lists", invokeId);

			LinkedList nameList = getNamedVariableListAssociationSpecific(connection);

#if (CONFIG_MMS_SORT_NAME_LIST == 1)
			StringUtils_sortList(nameList);
#endif

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                createNameListResponse(connection, invokeId, nameList, response, continueAfterId);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

			LinkedList_destroy(nameList);
		}
		else {

		    if (DEBUG_MMS_SERVER)
		        libmms_log(MMS_LOG_WARNING, connection, "MMS_SERVER: get name list (invoke-id: %u) for association specific object-class %i not supported -> object-access-unsupported", invokeId, objectClass);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
		}
	}
#endif /* (MMS_DYNAMIC_DATA_SETS == 1) */
#endif /* (MMS_DATA_SET_SERVICE == 1) */

	else {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_WARNING, connection, "MMS_SERVER: get name list (invoke-id: %u) for object-scope %i not supported -> object-access-unsupported", invokeId, objectScope);

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }
	}

}


#endif /* (MMS_GET_NAME_LIST == 1) */
