/*
 *  mms_write_service.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_server_internal.h"
#include "mms_common_internal.h"
#include "mms_endpoint_internal.h"
#include "mms_types.h"

#if (MMS_WRITE_SERVICE == 1)

#define CONFIG_MMS_WRITE_SERVICE_MAX_NUMBER_OF_WRITE_ITEMS 100

void
mmsServer_createMmsWriteResponse(MmsEndpointConnection connection,
        uint32_t invokeId, ByteBuffer* response, int numberOfItems, MmsDataAccessError* accessResults)
{
    int bufPos = 0;
    uint8_t* buffer = response->buffer;

    /* Determine length fields */

    uint32_t invokeIdLength = BerEncoder_UInt32determineEncodedSize(invokeId);

    uint32_t accessResultsLength = 0;

    int i;
    for (i = 0; i < numberOfItems; i++) {
        if (accessResults[i] < 0)
            accessResultsLength += 2;
        else
            accessResultsLength += 3;
    }

    uint32_t writeResponseLength = 2 + invokeIdLength
                                 + 1 + BerEncoder_determineLengthSize(accessResultsLength)
                                 + accessResultsLength;

    /* Encode write response */

    bufPos = BerEncoder_encodeTL(0xa1, writeResponseLength, buffer, bufPos);

    /* invokeId */
    bufPos = BerEncoder_encodeTL(0x02, invokeIdLength, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0xa5, accessResultsLength, buffer, bufPos);

    for (i = 0; i < numberOfItems; i++) {
        if (accessResults[i] < 0) {
            buffer[bufPos++] = 0x81;
            buffer[bufPos++] = 0x00;
        }
        else {
            buffer[bufPos++] = 0x80;
            buffer[bufPos++] = 0x01;
            buffer[bufPos++] = (uint8_t) accessResults[i];
        }
    }

    response->size = bufPos;
}

static void
createWriteNamedVariableListResponse(
        MmsEndpointConnection connection,
        WriteRequest_t* writeRequest,
        uint32_t invokeId,
        MmsNamedVariableList namedList)
{
    bool sendResponse = true;

    LinkedList variables = MmsNamedVariableList_getVariableList(namedList);

    int numberOfWriteItems = LinkedList_size(variables);

    if (numberOfWriteItems > CONFIG_MMS_WRITE_SERVICE_MAX_NUMBER_OF_WRITE_ITEMS) {

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_OTHER, response);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

        return;
    }

    /* write variables and send response */

    MmsDataAccessError accessResults[CONFIG_MMS_WRITE_SERVICE_MAX_NUMBER_OF_WRITE_ITEMS * sizeof(MmsDataAccessError)];

    LinkedList element;

    int i = 0;

    for (element = LinkedList_getNext(variables); element != NULL; element = LinkedList_getNext(element)) {
        MmsNamedVariableListEntry variableListEntry = (MmsNamedVariableListEntry) LinkedList_getData(element);

        MmsDomain* variableDomain = MmsNamedVariableListEntry_getDomain(variableListEntry);
        char* variableName = MmsNamedVariableListEntry_getVariableName(variableListEntry);

        MmsValue* oldValue = mmsServer_getValue(connection->serverConnection->server, variableDomain, variableName, connection);

        Data_t* dataElement = writeRequest->listOfData.list.array[i];

        MmsValue* newValue = mmsMsg_parseDataElement(dataElement);

        if (newValue == NULL) {
            accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
        }
        else if (MmsValue_equalTypes(oldValue, newValue) == false) {
            MmsValue_delete(newValue);
            accessResults[i] = DATA_ACCESS_ERROR_TYPE_INCONSISTENT;
        }
        else {

            MmsObjectScope objectScope;

            if (variableDomain != NULL)
                objectScope = MMS_DOMAIN_SPECIFIC;
            else
                objectScope = MMS_VMD_SPECIFIC;

            MmsDataAccessError valueIndication =
                    mmsServer_setValue(connection->serverConnection->server, objectScope, variableDomain, variableName, newValue, connection);

            accessResults[i] = valueIndication;

            if (valueIndication == DATA_ACCESS_ERROR_NO_RESPONSE)
                sendResponse = false;

            MmsValue_delete(newValue);
        }

        i++;
    }

    if (sendResponse) {
        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsServer_createMmsWriteResponse(connection, invokeId, response, numberOfWriteItems, accessResults);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }
    }

}

static void
handleWriteNamedVariableListRequest(
        MmsEndpointConnection connection,
        WriteRequest_t* writeRequest,
        uint32_t invokeId)
{
    if (writeRequest->variableAccessSpecification.choice.variableListName.present == ObjectName_PR_domainspecific)
    {
        char domainIdStr[65];
        char nameIdStr[65];

        mmsMsg_copyAsn1IdentifierToStringBuffer(writeRequest->variableAccessSpecification.choice.variableListName.choice.domainspecific.domainId,
                domainIdStr, 65);

        mmsMsg_copyAsn1IdentifierToStringBuffer(writeRequest->variableAccessSpecification.choice.variableListName.choice.domainspecific.itemId,
                nameIdStr, 65);

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: write (invoke-id: %u) named variable list domain specific %s:%s",
                    invokeId, domainIdStr, nameIdStr);

        MmsDomain* domain = MmsDevice_getDomain(MmsServer_getDevice(connection->serverConnection->server), domainIdStr);

        if (domain == NULL) {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: write (invoke-id: %u) domain %s not found -> service-error: object-non-existent",
                        invokeId, domainIdStr);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

            return;
        }
        else {
            MmsNamedVariableList namedList = MmsDomain_getNamedVariableList(domain, nameIdStr);

            if (namedList != NULL) {
                createWriteNamedVariableListResponse(connection, writeRequest, invokeId, namedList);
            }
            else {

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: write (invoke-id: %u) named variable list %s not found -> service-error: object-non-existent",
                            invokeId, nameIdStr);

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }

            }
        }

    }
    else if (writeRequest->variableAccessSpecification.choice.variableListName.present == ObjectName_PR_vmdspecific) {
        char listName[65];

        mmsMsg_copyAsn1IdentifierToStringBuffer(writeRequest->variableAccessSpecification.choice.variableListName.choice.vmdspecific,
                listName, 65);

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: write (invoke-id: %u) named variable list VMD specific %s",
                    invokeId, listName);

        MmsNamedVariableList namedList = mmsServer_getNamedVariableListWithName(connection->serverConnection->server->device->namedVariableLists, listName);

        if (namedList != NULL) {
            createWriteNamedVariableListResponse(connection, writeRequest, invokeId, namedList);
        }
        else {
            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: write (invoke-id: %u) variable list %s not found -> service-error: object-non-existent",
                        invokeId, listName);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

        }
    }
#if (MMS_DYNAMIC_DATA_SETS == 1)
    else if (writeRequest->variableAccessSpecification.choice.variableListName.present == ObjectName_PR_aaspecific) {
        char listName[65];

        mmsMsg_copyAsn1IdentifierToStringBuffer(writeRequest->variableAccessSpecification.choice.variableListName.choice.aaspecific,
                listName, 65);

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: write (invoke-id: %u) named variable list association specific %s",
                    invokeId, listName);

        MmsNamedVariableList namedList = MmsEndpointServerConnection_getNamedVariableList(connection->serverConnection, listName);

        if (namedList != NULL) {
            createWriteNamedVariableListResponse(connection, writeRequest, invokeId, namedList);
        }
        else {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: write (invoke-id: %u) variable list %s not found -> service-error: object-non-existent",
                        invokeId, listName);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

        }
    }
#endif /* (MMS_DYNAMIC_DATA_SETS == 1) */
    else {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: write (invoke-id: %u) -> service-error: object-access-unsupported",
                    invokeId);

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

    }

}


void
mmsServer_handleWriteRequest(
		MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId)
{
	MmsPdu_t* mmsPdu = 0;
	WriteRequest_t* writeRequest;
	asn_dec_rval_t rval; /* Decoder return value  */

	rval = ber_decode(NULL, &asn_DEF_MmsPdu, (void**) &mmsPdu, buffer, CONFIG_MMS_MAXIMUM_PDU_SIZE);

	if (rval.code != RC_OK) {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: write (invoke-id: %u) reject --> invalid-pdu", invokeId);

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_INVALID_PDU, response);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

	    goto exit_function;
	}

	writeRequest = &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.write);

	if (writeRequest->variableAccessSpecification.present == VariableAccessSpecification_PR_variableListName) {
	    handleWriteNamedVariableListRequest(connection, writeRequest, invokeId);
	    goto exit_function;
	}
	else if (writeRequest->variableAccessSpecification.present == VariableAccessSpecification_PR_listOfVariable) {

        int numberOfWriteItems = writeRequest->variableAccessSpecification.choice.listOfVariable.list.count;

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: write (invoke-id: %u) named variables (count: %i)",
                    invokeId, numberOfWriteItems);

        if (numberOfWriteItems < 1) {

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT, response);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

            goto exit_function;
        }

        if (numberOfWriteItems > CONFIG_MMS_WRITE_SERVICE_MAX_NUMBER_OF_WRITE_ITEMS) {

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_OTHER, response);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

            goto exit_function;
        }

        if (writeRequest->listOfData.list.count != numberOfWriteItems) {

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT, response);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

            goto exit_function;
        }

        MmsDataAccessError accessResults[CONFIG_MMS_WRITE_SERVICE_MAX_NUMBER_OF_WRITE_ITEMS * sizeof(MmsDataAccessError)];

        bool sendResponse = true;

        int i;

        for (i = 0; i < numberOfWriteItems; i++) {

            MmsObjectScope objectScope;
            MmsDataAccessError valueIndication;

            ListOfVariableSeq_t* varSpec =
                    writeRequest->variableAccessSpecification.choice.listOfVariable.list.array[i];

            if (varSpec->variableSpecification.present != VariableSpecification_PR_name) {
                accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ACCESS_UNSUPPORTED;
                continue;
            }

            MmsVariableSpecification* variable;

            MmsDevice* device = MmsServer_getDevice(connection->serverConnection->server);

            MmsDomain* domain = NULL;

            char nameIdStr[65];

            if (varSpec->variableSpecification.choice.name.present == ObjectName_PR_domainspecific) {
                Identifier_t domainId = varSpec->variableSpecification.choice.name.choice.domainspecific.domainId;

                objectScope = MMS_DOMAIN_SPECIFIC;

                char domainIdStr[65];

                mmsMsg_copyAsn1IdentifierToStringBuffer(domainId, domainIdStr, 65);

                Identifier_t nameId = varSpec->variableSpecification.choice.name.choice.domainspecific.itemId;

                mmsMsg_copyAsn1IdentifierToStringBuffer(nameId, nameIdStr, 65);

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: write (invoke-id: %u) named variable domain specific %s:%s",
                            invokeId, domainIdStr, nameIdStr);

                domain = MmsDevice_getDomain(device, domainIdStr);

                if (domain == NULL) {
                    accessResults[i] = DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT;
                    continue;
                }

                variable = MmsDomain_getNamedVariable(domain, nameIdStr);
            }

    #if (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1)
            else if (varSpec->variableSpecification.choice.name.present == ObjectName_PR_vmdspecific) {
                Identifier_t nameId = varSpec->variableSpecification.choice.name.choice.vmdspecific;

                objectScope = MMS_VMD_SPECIFIC;

                mmsMsg_copyAsn1IdentifierToStringBuffer(nameId, nameIdStr, 65);

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: write (invoke-id: %u) named variable vmd specific %s",
                            invokeId, nameIdStr);

                variable = MmsDevice_getNamedVariable(device, nameIdStr);
            }
    #endif /* (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1) */
            else if (varSpec->variableSpecification.choice.name.present == ObjectName_PR_aaspecific) {
                Identifier_t nameId = varSpec->variableSpecification.choice.name.choice.aaspecific;

                objectScope = MMS_ASSOCIATION_SPECIFIC;

                mmsMsg_copyAsn1IdentifierToStringBuffer(nameId, nameIdStr, 65);

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: write (invoke-id: %u) named variable association specific %s",
                            invokeId, nameIdStr);

                /* call user provided handler to get type */
                if (connection->serverConnection->server->aaVarSpecHandler != NULL) {
                    variable = connection->serverConnection->server->aaVarSpecHandler(
                            connection->serverConnection->server->aaVarSpecHandlerParameter, nameIdStr, connection);
                }
                else
                    variable = NULL;
            }
            else {

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: write (invoke-id: %u) - object-access-unsupported",
                            invokeId);

                accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ACCESS_UNSUPPORTED;
                continue;
            }

            if (variable == NULL) {

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: write (invoke-id: %u) - object-none-existent",
                            invokeId);

                accessResults[i] = DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT;
                continue;
            }

            AlternateAccess_t* alternateAccess = varSpec->alternateAccess;

            if (alternateAccess != NULL) {

                if (mmsServer_isComponentAccess(alternateAccess)) {
                    if (variable->type != MMS_STRUCTURE) {
                        accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
                        continue;
                    }
                }
                else if (mmsServer_isIndexAccess(alternateAccess)) {
                    if (variable->type != MMS_ARRAY) {
                        accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
                        continue;
                    }
                }
                else {
                    accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ACCESS_UNSUPPORTED;
                    continue;
                }
            }

            Data_t* dataElement = writeRequest->listOfData.list.array[i];

            MmsValue* value = mmsMsg_parseDataElement(dataElement);

            if (value == NULL) {
                accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
                continue;
            }

            if (alternateAccess != NULL) {

                if (domain == NULL)
                    domain = (MmsDomain*) device;

                if (mmsServer_isIndexAccess(alternateAccess)) { /* array index access */

                    MmsValue* cachedArray = MmsServer_getValueFromCache(connection->serverConnection->server, domain, nameIdStr);

                    if (cachedArray == NULL) {
                        accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
                        goto end_of_main_loop;
                    }

                    int index = mmsServer_getLowIndex(alternateAccess);
                    int numberOfElements = mmsServer_getNumberOfElements(alternateAccess);

                    if (numberOfElements == 0) { /* select single array element with index */

                        MmsValue* elementValue = MmsValue_getElement(cachedArray, index);

                        if (elementValue == NULL) {
                            accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
                            goto end_of_main_loop;
                        }

                        if (MmsValue_update(elementValue, value) == false) {
                            accessResults[i] = DATA_ACCESS_ERROR_TYPE_INCONSISTENT;
                            goto end_of_main_loop;
                        }
                    }
                    else { /* select sub-array with start-index and number-of-elements */

                        if (MmsValue_getType(value) != MMS_ARRAY) {
                            accessResults[i] = DATA_ACCESS_ERROR_TYPE_INCONSISTENT;
                            goto end_of_main_loop;
                        }

                        int elementNo;

                        for (elementNo = 0; elementNo < numberOfElements; elementNo++) {
                            MmsValue* newElement = MmsValue_getElement(value, elementNo);
                            MmsValue* elementValue = MmsValue_getElement(cachedArray, index++);

                            if ((elementValue == NULL) || (newElement == NULL) ) {
                                accessResults[i] = DATA_ACCESS_ERROR_TYPE_INCONSISTENT;
                                goto end_of_main_loop;
                            }

                            if (MmsValue_update(elementValue, newElement) == false) {
                                accessResults[i] = DATA_ACCESS_ERROR_TYPE_INCONSISTENT;
                                goto end_of_main_loop;
                            }

                        }
                    }

                    accessResults[i] = DATA_ACCESS_ERROR_SUCCESS;

                    goto end_of_main_loop;
                }
                else { /* structure component access */

                    char componentName[65];

                    if (mmsServer_getComponentName(alternateAccess, componentName) != NULL) {

                        int componentIndex;

                        MmsVariableSpecification* componentSpec =
                                MmsVariableSpecification_getChildSpecificationByName(variable, componentName, &componentIndex);

                        if (componentSpec != NULL) {

                            if (DEBUG_MMS_SERVER)
                                libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: write (invoke-id: %u) access to variable component %s:%s:%s", invokeId, domain->domainName, variable->name, componentName);

                            /* Check for correct type */
                            if (MmsValue_getType(value) != MmsVariableSpecification_getType(componentSpec)) {
                                accessResults[i] = DATA_ACCESS_ERROR_TYPE_INCONSISTENT;
                                goto end_of_main_loop;
                            }

                            MmsDataAccessError valueIndication =
                                    mmsServer_setComponentValue(connection->serverConnection->server, objectScope, domain, nameIdStr, componentName, componentIndex, value, connection);

                            if (valueIndication == DATA_ACCESS_ERROR_NO_RESPONSE)
                                sendResponse = false;

                            accessResults[i] = valueIndication;

                            goto end_of_main_loop;
                        }
                        else {
                            accessResults[i] = DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT;
                            goto end_of_main_loop;
                        }


                    }
                    else {
                        accessResults[i] = DATA_ACCESS_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT;
                        goto end_of_main_loop;
                    }

                }

            }

            /* Check for correct type */
            if (MmsValue_getType(value) != MmsVariableSpecification_getType(variable)) {
                accessResults[i] = DATA_ACCESS_ERROR_TYPE_INCONSISTENT;
                goto end_of_main_loop;
            }

            valueIndication =
                    mmsServer_setValue(connection->serverConnection->server, objectScope, domain, nameIdStr, value, connection);

            if (valueIndication == DATA_ACCESS_ERROR_NO_RESPONSE)
                sendResponse = false;

            accessResults[i] = valueIndication;

end_of_main_loop:

            MmsValue_delete(value);
        }

        if (sendResponse) {
            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsServer_createMmsWriteResponse(connection, invokeId, response, numberOfWriteItems, accessResults);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
        }

    }
	else { /* unknown request type */

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: write (invoke-id: %u) reject --> invalid-argument", invokeId);

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT, response);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

        goto exit_function;
	}

exit_function:
	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

#endif /* (MMS_WRITE_SERVICE == 1) */
