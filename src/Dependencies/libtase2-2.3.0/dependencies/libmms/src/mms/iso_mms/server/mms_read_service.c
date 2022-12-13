/*
 *  mms_read_service.c
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
#include "mms_common_internal.h"
#include "mms_value_internal.h"

#include "linked_list.h"

#include "ber_encoder.h"

/**********************************************************************************************
 * MMS Read Service
 *********************************************************************************************/

typedef struct sVarAccessSpec* VarAccessSpec;

struct sVarAccessSpec {
	int specific; /* 0 - vmd, 1 - domain, 2 - association */

	char* itemId;
	char* domainId;
	char* component;

	VarAccessSpec next;
};

static VarAccessSpec
VarAccessSpec_create(VarAccessSpec self)
{
    if (self == NULL)
        self = (VarAccessSpec) GLOBAL_MALLOC(sizeof(struct sVarAccessSpec));

    if (self) {
        self->itemId = NULL;
        self->domainId = NULL;
        self->component = NULL;
        self->next = NULL;
    }

    return self;
}

static void
VarAccessSpec_destroyList(VarAccessSpec self)
{
    VarAccessSpec currentAccessSpec = self;

    while (currentAccessSpec) {

        if (currentAccessSpec->itemId != NULL)
            GLOBAL_FREEMEM(currentAccessSpec->itemId);

        if (currentAccessSpec->domainId != NULL)
            GLOBAL_FREEMEM(currentAccessSpec->domainId);

        if (currentAccessSpec->component != NULL)
            GLOBAL_FREEMEM(currentAccessSpec->component);

        VarAccessSpec toDelete = currentAccessSpec;

        currentAccessSpec = currentAccessSpec->next;

        GLOBAL_FREEMEM(toDelete);
    }
}

static MmsValue*
addNamedVariableValue(MmsVariableSpecification* namedVariable, MmsEndpointConnection connection,
        MmsDomain* domain, char* itemId)
{
    MmsValue* value = NULL;

    if (namedVariable->type == MMS_STRUCTURE) {

        value = mmsServer_getValue(connection->serverConnection->server, domain, itemId, connection);

        if (value != NULL)
            goto exit_function;
        else {

            int componentCount = namedVariable->typeSpec.structure.elementCount;

            value = MmsValue_createEmptyStructure(componentCount);

            value->deleteValue = 1;

            int i;

            for (i = 0; i < componentCount; i++) {
                char newNameIdStr[65];

                StringUtils_createStringInBuffer(newNameIdStr, 3, itemId, CONFIG_MMS_SEPARATOR_CHAR,
                        namedVariable->typeSpec.structure.elements[i]->name);

                MmsValue* element =
                        addNamedVariableValue(namedVariable->typeSpec.structure.elements[i],
                                connection, domain, newNameIdStr);

                if (element == NULL) {
                    MmsValue_delete(value);
                    value = NULL;
                    break;
                }

                MmsValue_setElement(value, i, element);
            }
        }
    }
    else {
        value = mmsServer_getValue(connection->serverConnection->server, domain, itemId, connection);
    }

exit_function:
    return value;
}

static void
addComplexValueToResultList(MmsVariableSpecification* namedVariable,
                                LinkedList typedValues, MmsEndpointConnection connection,
                                MmsDomain* domain, char* nameIdStr)
{

    MmsValue* value = addNamedVariableValue(namedVariable, connection, domain, nameIdStr);

    if (value != NULL)
        LinkedList_add(typedValues, value);
}


static void
appendValueToResultList(MmsValue* value, LinkedList values)
{

	if (value != NULL )
		LinkedList_add(values, value);
}

static void
appendErrorToResultList(LinkedList values, MmsDataAccessError errorType) {
    MmsValue* value = MmsValue_newDataAccessError(errorType);
    MmsValue_setDeletable(value);
    appendValueToResultList(value, values);
}

static void
deleteValueList(LinkedList values)
{
    LinkedList value = LinkedList_getNext(values);

	while (value != NULL ) {
	    MmsValue* typedValue = (MmsValue*) (value->data);

		MmsValue_deleteConditional(typedValue);

		value = LinkedList_getNext(value);
	}

	LinkedList_destroyStatic(values);
}

static bool
isAccessToArrayComponent(AlternateAccess_t* alternateAccess)
{
    if (alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.
                alternateAccess != NULL)
        return true;
    else
        return false;
}

static MmsValue*
getComponent(AlternateAccess_t* alternateAccess, MmsVariableSpecification* namedVariable, MmsValue* variableValue)
{
    MmsValue* retValue = NULL;

    if (mmsServer_isComponentAccess(alternateAccess)) {
        Identifier_t component =
                alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.component;

        if (component.size > 129)
            goto exit_function;

        if (namedVariable->type == MMS_STRUCTURE) {

            int i;

            for (i = 0; i < namedVariable->typeSpec.structure.elementCount; i++) {

                if (strlen(namedVariable->typeSpec.structure.elements[i]->name) == component.size) {
                    if (strncmp(namedVariable->typeSpec.structure.elements[i]->name,
                            (char*) component.buf, component.size) == 0) {
                        MmsValue* value = MmsValue_getElement(variableValue, i);

                        if (alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.alternateAccess != NULL) {
                            retValue = getComponent(
                                            alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.alternateAccess,
                                            namedVariable->typeSpec.structure.elements[i],
                                            value);
                        }
                        else
                            retValue = value;
                    }
                }
            }
        }
    }

    exit_function: return retValue;
}

static MmsValue*
getComponentOfArrayElement(AlternateAccess_t* alternateAccess, MmsVariableSpecification* namedVariable,
        MmsValue* structuredValue)
{
    MmsValue* retValue = NULL;

    if (isAccessToArrayComponent(alternateAccess))
    {
        Identifier_t component = alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.alternateAccess
                ->list.array[0]->choice.unnamed->choice.selectAccess.choice.component;

        if (component.size > 129)
            goto exit_function;

        MmsVariableSpecification* structSpec;

        if (structuredValue->type == MMS_ARRAY)
            structSpec = namedVariable->typeSpec.array.elementTypeSpec;
        else if (namedVariable->type == MMS_STRUCTURE)
            structSpec = namedVariable;
        else
            goto exit_function;

        int elementCount = namedVariable->typeSpec.structure.elementCount;

        int i;
        for (i = 0; i < elementCount; i++) {
            if ((int) strlen(structSpec->typeSpec.structure.elements[i]->name) == component.size) {
                if (strncmp (structSpec->typeSpec.structure.elements[i]->name, (char*) component.buf,
                        component.size) == 0)
                {
                    MmsValue* value = MmsValue_getElement(structuredValue, i);

                    retValue = value;

                    goto exit_function;
                }
            }
        }
    }

exit_function:
    return retValue;
}

static void
alternateArrayAccess(MmsEndpointConnection connection,
		AlternateAccess_t* alternateAccess, MmsDomain* domain,
		char* itemId, LinkedList values,
		MmsVariableSpecification* namedVariable)
{
	if (mmsServer_isIndexAccess(alternateAccess))
	{
		int lowIndex = mmsServer_getLowIndex(alternateAccess);
		int numberOfElements = mmsServer_getNumberOfElements(alternateAccess);

		if (DEBUG_MMS_SERVER)
		    libmms_log(MMS_LOG_DEBUG, connection, "MMS_SERVER: Alternate access index: %i elements %i", lowIndex, numberOfElements);

		int index = lowIndex;

		MmsValue* arrayValue = mmsServer_getValue(connection->serverConnection->server, domain, itemId, connection);

		if (arrayValue != NULL) {

	        MmsValue* value = NULL;

			if (numberOfElements == 0)
			    if (isAccessToArrayComponent(alternateAccess)) {
			        if (namedVariable->typeSpec.array.elementTypeSpec->type == MMS_STRUCTURE) {
			            MmsValue* structValue = MmsValue_getElement(arrayValue, index);

			            if (structValue != NULL)
			                value = getComponentOfArrayElement(alternateAccess,
			                        namedVariable, structValue);
			        }
			    }
			    else {
			        value = MmsValue_getElement(arrayValue, index);
			    }
			else {
				value = MmsValue_createEmptyArray(numberOfElements);

				MmsValue_setDeletable(value);

				int resultIndex = 0;
				while (index < lowIndex + numberOfElements) {
					MmsValue* elementValue = NULL;

					elementValue = MmsValue_getElement(arrayValue, index);

					if (!MmsValue_isDeletable(elementValue)) {
						elementValue = MmsValue_clone(elementValue);
						elementValue->deleteValue = 1;
					}

					MmsValue_setElement(value, resultIndex, elementValue);

					index++;
					resultIndex++;
				}
			}

			appendValueToResultList(value, values);

		}
		else  /* access error */
			appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);

	}
	else { /* invalid access */
	    if (DEBUG_MMS_SERVER)
	        libmms_log(MMS_LOG_DEBUG, connection, "MMS_SERVER: Invalid alternate access");

        appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);
	}
}

static void
addNamedVariableToResultList(MmsVariableSpecification* namedVariable, MmsDomain* domain, char* nameIdStr,
		LinkedList /*<MmsValue>*/ values, MmsEndpointConnection connection, AlternateAccess_t* alternateAccess)
{
	if (namedVariable != NULL) {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_DEBUG, connection, "MMS_SERVER: read access - found named variable %s:%s",
                    domain != NULL ? MmsDomain_getName(domain) : "",
                    namedVariable->name);

        if (namedVariable->type == MMS_STRUCTURE) {

            MmsValue* value = mmsServer_getValue(connection->serverConnection->server, domain, nameIdStr, connection);

            if (alternateAccess != NULL) {
                value = getComponent(alternateAccess, namedVariable, value);

                if (value != NULL) {
                    appendValueToResultList(value, values);
                }
                else {
                    appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);
                }
            }
            else {
                if (value != NULL) {
                    appendValueToResultList(value, values);
                }
                else {
                    addComplexValueToResultList(namedVariable,
                            values, connection, domain, nameIdStr);
                }
            }
        }
        else if (namedVariable->type == MMS_ARRAY) {

            if (alternateAccess != NULL) {
                alternateArrayAccess(connection, alternateAccess, domain,
                        nameIdStr, values, namedVariable);
            }
            else { /* return complete array */
                MmsValue* value = mmsServer_getValue(connection->serverConnection->server, domain, nameIdStr, connection);
                appendValueToResultList(value, values);
            }
        }
        else {
            if (alternateAccess != NULL) {
                appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);
            }
            else {
                MmsValue* value = mmsServer_getValue(connection->serverConnection->server, domain, nameIdStr, connection);

                if (value == NULL) {
                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_DEBUG, connection, "MMS_SERVER: value of known variable is not found. Maybe illegal access to array element!");

                    appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);
                }
                else
                    appendValueToResultList(value, values);
            }
        }
    }
    else
        appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);
}


static bool
isSpecWithResult(ReadRequest_t* read)
{
	if (read->specificationWithResult != NULL)
		if (*(read->specificationWithResult) != false)
			return true;

	return false;
}

static int
encodeVariableAccessSpecificationNamedVariableList(VarAccessSpec accessSpec, uint8_t* buffer, int bufPos, bool encode)
{
	/* determine size */
    uint32_t varAccessSpecSize = 0;

	uint32_t itemIdLen = strlen(accessSpec->itemId);

	varAccessSpecSize += itemIdLen + BerEncoder_determineLengthSize(itemIdLen) + 1;

	if (accessSpec->domainId != NULL) {
	    uint32_t domainIdLen = strlen(accessSpec->domainId);

		varAccessSpecSize += domainIdLen + BerEncoder_determineLengthSize(domainIdLen) + 1;
	}

	uint32_t specificityLength = varAccessSpecSize;

	varAccessSpecSize += 1 + BerEncoder_determineLengthSize(specificityLength);

	uint32_t variableListNameLength = varAccessSpecSize;

	varAccessSpecSize += 1 + BerEncoder_determineLengthSize(variableListNameLength);

	uint32_t varAccessSpecLength = varAccessSpecSize;

	varAccessSpecSize += 1 + BerEncoder_determineLengthSize(varAccessSpecLength);

	if (encode == false) {
	    bufPos = varAccessSpecSize;
	    goto exit_function;
	}

	/* encode to buffer */
	bufPos = BerEncoder_encodeTL(0xa0, varAccessSpecLength, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0xa1, variableListNameLength, buffer, bufPos);

    if (accessSpec->specific == 0) { /* vmd-specific */
        bufPos = BerEncoder_encodeTL(0xa0, specificityLength, buffer, bufPos);
    }
    else if (accessSpec->specific == 1) { /* domain-specific */
        bufPos = BerEncoder_encodeTL(0xa1, specificityLength, buffer, bufPos);

    }
    else { /* association-specific */
        bufPos = BerEncoder_encodeTL(0xa2, specificityLength, buffer, bufPos);
    }


    if (accessSpec->domainId != NULL)
        bufPos = BerEncoder_encodeStringWithTag(0x1a, accessSpec->domainId, buffer, bufPos);

    bufPos = BerEncoder_encodeStringWithTag(0x1a, accessSpec->itemId, buffer, bufPos);

exit_function:
	return bufPos;
}

static int
encodeVariableAccessSpecificationListOfVariables(VarAccessSpec accessSpecs, uint8_t* buffer, int bufPos, bool encode)
{
    /* determine size */
    uint32_t varAccessSpecSize = 0;

    uint32_t listOfVariableSize = 0;

    uint32_t listElementsSize = 0;

    VarAccessSpec spec = accessSpecs;

    while (spec) {

        uint32_t elementSize = 0;

        if (spec->domainId) { /* domain specific */
            uint32_t itemIdSize = strlen(spec->itemId);

            elementSize += (1 + BerEncoder_determineLengthSize(itemIdSize) + itemIdSize);

            uint32_t domainIdSize = strlen(spec->domainId);

            elementSize += (1 + BerEncoder_determineLengthSize(domainIdSize) + domainIdSize);

            elementSize = (1 + BerEncoder_determineLengthSize(elementSize) + elementSize);
            elementSize = (1 + BerEncoder_determineLengthSize(elementSize) + elementSize);
        }
        else { /* VMD specific */
            uint32_t itemIdSize = strlen(spec->itemId);
            elementSize += (1 + BerEncoder_determineLengthSize(itemIdSize) + itemIdSize);

           elementSize = (1 + BerEncoder_determineLengthSize(elementSize) + elementSize);
        }

        elementSize = (1 + BerEncoder_determineLengthSize(elementSize) + elementSize); /* sequence */

        listElementsSize += elementSize;

        spec = spec->next;
    }

    listOfVariableSize = 1 + BerEncoder_determineLengthSize(listElementsSize) + listElementsSize;

    varAccessSpecSize = 1 + BerEncoder_determineLengthSize(listOfVariableSize) + listOfVariableSize;

    if (encode == false) {
        bufPos = varAccessSpecSize;
        goto exit_function;
    }

    /* encode to buffer */
    bufPos = BerEncoder_encodeTL(0xa0, listOfVariableSize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0xa0, listElementsSize, buffer, bufPos);

    spec = accessSpecs;

    while (spec) {


        if (spec->domainId) { /* domain specific */
            uint32_t itemIdSize = strlen(spec->itemId);
            uint32_t domainIdSize = strlen(spec->domainId);

            uint32_t domainSpecificSize = (1 + BerEncoder_determineLengthSize(itemIdSize) + itemIdSize);

            domainSpecificSize += (1 + BerEncoder_determineLengthSize(domainIdSize) + domainIdSize);

            uint32_t objectNameSize = (1 + BerEncoder_determineLengthSize(domainSpecificSize) + domainSpecificSize);

            uint32_t listElementSize = 1 + BerEncoder_determineLengthSize(objectNameSize) + objectNameSize;

            bufPos = BerEncoder_encodeTL(0x30, listElementSize, buffer, bufPos);
            bufPos = BerEncoder_encodeTL(0xa0, objectNameSize, buffer, bufPos);
            bufPos = BerEncoder_encodeTL(0xa1, domainSpecificSize, buffer, bufPos);

            bufPos = BerEncoder_encodeStringWithTag(0x1a, spec->domainId, buffer, bufPos);
            bufPos = BerEncoder_encodeStringWithTag(0x1a, spec->itemId, buffer, bufPos);
        }
        else { /* VMD specific */
            uint32_t itemIdSize = strlen(spec->itemId);
            uint32_t vmdSpecificSize = 1 + BerEncoder_determineLengthSize(itemIdSize) + itemIdSize;

            uint32_t listElementSize = 1 + BerEncoder_determineLengthSize(vmdSpecificSize) + vmdSpecificSize;

            bufPos = BerEncoder_encodeTL(0x30, listElementSize, buffer, bufPos);
            bufPos = BerEncoder_encodeTL(0xa0, vmdSpecificSize, buffer, bufPos);
            bufPos = BerEncoder_encodeStringWithTag(0x80, spec->itemId, buffer, bufPos);

        }

        spec = spec->next;
    }


exit_function:
    return bufPos;
}

static void
encodeReadResponse(MmsEndpointConnection connection,
        uint32_t invokeId, ByteBuffer* response, LinkedList values,
        VarAccessSpec accessSpec, bool isNamedVariableList)
{
    int i;
    uint8_t* buffer;
    int bufPos;

    int variableCount = LinkedList_size(values);

    uint32_t varAccessSpecSize = 0;

    if (accessSpec != NULL) {
        if (isNamedVariableList)
            varAccessSpecSize = encodeVariableAccessSpecificationNamedVariableList(accessSpec, NULL, 0, false);
        else
            varAccessSpecSize = encodeVariableAccessSpecificationListOfVariables(accessSpec, NULL, 0, false);
    }

    /* determine BER encoded message sizes */
    uint32_t accessResultSize = 0;

    /* iterate values list to determine encoded size  */
    LinkedList value = LinkedList_getNext(values);

    for (i = 0; i < variableCount; i++) {

        MmsValue* data = (MmsValue*) value->data;

        accessResultSize += MmsValue_encodeMmsData(data, NULL, 0, false);

        value = LinkedList_getNext(value);
    }

    uint32_t listOfAccessResultsLength = 1 +
            BerEncoder_determineLengthSize(accessResultSize) +
            accessResultSize;

    uint32_t confirmedServiceResponseContentLength = listOfAccessResultsLength + varAccessSpecSize;

    uint32_t confirmedServiceResponseLength = 1 +
            BerEncoder_determineLengthSize(confirmedServiceResponseContentLength) +
            confirmedServiceResponseContentLength;

    uint32_t invokeIdSize = BerEncoder_UInt32determineEncodedSize(invokeId) + 2;

    uint32_t confirmedResponseContentSize = confirmedServiceResponseLength + invokeIdSize;

    uint32_t mmsPduSize = 1 + BerEncoder_determineLengthSize(confirmedResponseContentSize) +
            confirmedResponseContentSize;

    /* Check if message would fit in the MMS PDU */
    if (mmsPduSize > connection->maxPduSize) {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER:  message to large! send error PDU!");

        mmsMsg_createServiceErrorPdu(invokeId, response,
                MMS_ERROR_SERVICE_OTHER);

        goto exit_function;
    }

    /* encode message */

    buffer = response->buffer;
    bufPos = 0;

    /* confirmed response PDU */
    bufPos = BerEncoder_encodeTL(0xa1, confirmedResponseContentSize, buffer, bufPos);

    /* invoke id */
    bufPos = BerEncoder_encodeTL(0x02, invokeIdSize - 2, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(invokeId, buffer, bufPos);

    /* confirmed-service-response read */
    bufPos = BerEncoder_encodeTL(0xa4, confirmedServiceResponseContentLength, buffer, bufPos);

    /* encode variable access specification */
    if (accessSpec != NULL) {
        if (isNamedVariableList)
            bufPos = encodeVariableAccessSpecificationNamedVariableList(accessSpec, buffer, bufPos, true);
        else
            bufPos = encodeVariableAccessSpecificationListOfVariables(accessSpec, buffer, bufPos, true);
    }

    /* encode list of access results */
    bufPos = BerEncoder_encodeTL(0xa1, accessResultSize, buffer, bufPos);

    /* encode access results */
    value = LinkedList_getNext(values);

    for (i = 0; i < variableCount; i++) {
        MmsValue* data = (MmsValue*) value->data;

        bufPos = MmsValue_encodeMmsData(data, buffer, bufPos, true);

        value = LinkedList_getNext(value);
    }

    response->size = bufPos;

    if (DEBUG_MMS_SERVER)
        libmms_log(MMS_LOG_DEBUG, connection, "MMS_SERVER: read response (invoke-id: %u) sent (size: %i)", invokeId, bufPos);

exit_function:
    return;
}

/**
 * \brief implements access to list of variables (multiple MMS variables)
 *
 * \param connection the client connection that received the request
 * \param read read request information
 * \param invokeId the invoke ID of the confirmed request PDU
 * \param response byte buffer to encode the response
 */
static void
handleReadListOfVariablesRequest(
        MmsEndpointConnection connection,
		ReadRequest_t* read,
		uint32_t invokeId)
{
	int variableCount = read->variableAccessSpecification.choice.listOfVariable.list.count;

	LinkedList /*<MmsValue>*/ values = LinkedList_create();

	struct sVarAccessSpec _accessSpec;

	VarAccessSpec accessSpec = NULL;
	VarAccessSpec currentAccessSpec = NULL;

	if (isSpecWithResult(read)) { /* add specification to result */
		/* not required for IEC 61850 but mandatory for TASE.2 */
	    accessSpec = VarAccessSpec_create(&_accessSpec);
	    currentAccessSpec = accessSpec;
	}

	int i;

	for (i = 0; i < variableCount; i++) {
		VariableSpecification_t varSpec =
			read->variableAccessSpecification.choice.listOfVariable.list.array[i]->variableSpecification;

		AlternateAccess_t* alternateAccess =
			read->variableAccessSpecification.choice.listOfVariable.list.array[i]->alternateAccess;

		if (varSpec.present == VariableSpecification_PR_name) {

			if (varSpec.choice.name.present == ObjectName_PR_domainspecific) {
				char domainIdStr[65];
				char nameIdStr[65];

				mmsMsg_copyAsn1IdentifierToStringBuffer(varSpec.choice.name.choice.domainspecific.domainId,
				        domainIdStr, 65);

				mmsMsg_copyAsn1IdentifierToStringBuffer(varSpec.choice.name.choice.domainspecific.itemId,
				        nameIdStr, 65);

				if (currentAccessSpec) {
				    currentAccessSpec->domainId = StringUtils_copyString(domainIdStr);
				    currentAccessSpec->itemId = StringUtils_copyString(nameIdStr);
				}

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: read (invoke-id: %u) access to domain variable %s:%s", invokeId, domainIdStr, nameIdStr);

				MmsDomain* domain = MmsDevice_getDomain(MmsServer_getDevice(connection->serverConnection->server), domainIdStr);

				if (domain == NULL) {
				    if (DEBUG_MMS_SERVER)
				        libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: domain %s not found -> object-none-existent", domainIdStr);

					appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);
				}
				else {
                    MmsVariableSpecification* namedVariable = MmsDomain_getNamedVariable(domain, nameIdStr);

                    if (namedVariable == NULL) {

                        if (DEBUG_MMS_SERVER)
                            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: variable %s not found -> object-none-existent", nameIdStr);

                        appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);
                    }
                    else
                        addNamedVariableToResultList(namedVariable, domain, nameIdStr,
                            values, connection, alternateAccess);
				}
			}
#if (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1)

			else if (varSpec.choice.name.present == ObjectName_PR_vmdspecific) {
			    char nameIdStr[65];

			    mmsMsg_copyAsn1IdentifierToStringBuffer(varSpec.choice.name.choice.vmdspecific, nameIdStr, 65);

                if (currentAccessSpec) {
                    currentAccessSpec->itemId = StringUtils_copyString(nameIdStr);
                }

			    if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: read (invoke-id: %u) access to vmd variable %s", invokeId, nameIdStr);

			    MmsVariableSpecification* namedVariable = MmsDevice_getNamedVariable(MmsServer_getDevice(connection->serverConnection->server), nameIdStr);

                if (namedVariable == NULL) {

                    if (DEBUG_MMS_SERVER)
                        libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: variable %s not found -> object-none-existent", nameIdStr);

                    appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);
                }
                else
                    addNamedVariableToResultList(namedVariable, (MmsDomain*) MmsServer_getDevice(connection->serverConnection->server), nameIdStr,
                            values, connection, alternateAccess);

			}
#endif /* (CONFIG_MMS_SUPPORT_VMD_SCOPE_NAMED_VARIABLES == 1) */

			else {
                appendErrorToResultList(values, DATA_ACCESS_ERROR_OBJECT_NONE_EXISTENT);

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: read (invoke-id: %u) access - object name type not supported -> object-none-existent", invokeId);
			}

			if (currentAccessSpec) {
			    if (i != (variableCount - 1)) {
			        currentAccessSpec->next = VarAccessSpec_create(NULL);
			        currentAccessSpec = currentAccessSpec->next;
			    }
			}
		}
		else {

		    if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: read (invoke-id: %u) access - varspec type not supported -> reject: invalid-argument", invokeId);

		    ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

		    if (response) {
	            mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT, response);

		        MmsEndpointConnection_sendMessage(connection, response);

		        MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
		    }

			goto exit;
		}
	}

	ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

	if (response) {
	    encodeReadResponse(connection, invokeId, response, values, accessSpec, false);

	    MmsEndpointConnection_sendMessage(connection, response);

	    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
	}

	if (accessSpec) {

	    if (accessSpec->domainId)
	        GLOBAL_FREEMEM(accessSpec->domainId);

        if (accessSpec->itemId)
            GLOBAL_FREEMEM(accessSpec->itemId);

        if (accessSpec->component)
            GLOBAL_FREEMEM(accessSpec->component);

	    VarAccessSpec_destroyList(accessSpec->next);
	}

exit:

    deleteValueList(values);
}

#if (MMS_DATA_SET_SERVICE == 1)

static void
createNamedVariableListResponse(MmsEndpointConnection connection, MmsNamedVariableList namedList,
		int invokeId, ReadRequest_t* read, VarAccessSpec accessSpec)
{
	LinkedList /*<MmsValue>*/ values = LinkedList_create();
	LinkedList variables = MmsNamedVariableList_getVariableList(namedList);

	int variableCount = LinkedList_size(variables);

	int i;

	LinkedList variable = LinkedList_getNext(variables);

    for (i = 0; i < variableCount; i++) {

        MmsNamedVariableListEntry variableListEntry = (MmsNamedVariableListEntry) variable->data;

        MmsDomain* variableDomain = MmsNamedVariableListEntry_getDomain(variableListEntry);
        char* variableName = MmsNamedVariableListEntry_getVariableName(variableListEntry);

        MmsVariableSpecification* namedVariable;

        if (variableDomain == NULL)
            namedVariable = MmsDevice_getNamedVariable(connection->serverConnection->server->device, variableName);
        else
            namedVariable = MmsDomain_getNamedVariable(variableDomain, variableName);

        addNamedVariableToResultList(namedVariable, variableDomain, variableName,
                values, connection, NULL);

        variable = LinkedList_getNext(variable);
    }

    ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

    if (response) {
        if (isSpecWithResult(read)) /* add specification to result */
            encodeReadResponse(connection, invokeId, response, values, accessSpec, true);
        else
            encodeReadResponse(connection, invokeId, response, values, NULL, true);

        MmsEndpointConnection_sendMessage(connection, response);

        MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
    }

	deleteValueList(values);
}

/**
 * \brief implements access to named variable lists (data sets)
 *
 * \param connection the client connection that received the request
 * \param read read request information
 * \param invokeId the invoke ID of the confirmed request PDU
 */
static void
handleReadNamedVariableListRequest(
		MmsEndpointConnection connection,
		ReadRequest_t* read,
		int invokeId)
{
    if (read->variableAccessSpecification.choice.variableListName.present ==
            ObjectName_PR_domainspecific)
    {
        char domainIdStr[65];
        char nameIdStr[65];

        mmsMsg_copyAsn1IdentifierToStringBuffer(read->variableAccessSpecification.choice.variableListName.choice.domainspecific.domainId,
                domainIdStr, 65);

        mmsMsg_copyAsn1IdentifierToStringBuffer(read->variableAccessSpecification.choice.variableListName.choice.domainspecific.itemId,
                nameIdStr, 65);

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: read (invoke-id: %u) access to domain variable list - %s:%s", invokeId, domainIdStr, nameIdStr);

        struct sVarAccessSpec accessSpec;

        accessSpec.specific = 1;
        accessSpec.domainId = domainIdStr;
        accessSpec.itemId = nameIdStr;
        accessSpec.component = NULL;
        accessSpec.next = NULL;

        MmsDomain* domain = MmsDevice_getDomain(MmsServer_getDevice(connection->serverConnection->server), domainIdStr);

        if (domain == NULL) {

            if (DEBUG_MMS_SERVER)
                libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: read (invoke-id: %u) access to variable list - domain %s not found -> object-non-existent", invokeId, domainIdStr);

            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }

        }
        else {
            MmsNamedVariableList namedList = MmsDomain_getNamedVariableList(domain, nameIdStr);

            if (namedList != NULL) {
                createNamedVariableListResponse(connection, namedList, invokeId, read,
                        &accessSpec);
            }
            else {

                if (DEBUG_MMS_SERVER)
                    libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: read (invoke-id: %u) access - named variable list %s not found -> object-non-existent", invokeId, nameIdStr);

                ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

                if (response) {
                    mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                    MmsEndpointConnection_sendMessage(connection, response);

                    MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
                }
            }
        }
    }
	else if (read->variableAccessSpecification.choice.variableListName.present == ObjectName_PR_vmdspecific)
	{
	    char listName[65];

        mmsMsg_copyAsn1IdentifierToStringBuffer(read->variableAccessSpecification.choice.variableListName.choice.vmdspecific,
                listName, 65);

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: read (invoke-id: %u) access to VMD variable list - %s", invokeId, listName);

        MmsNamedVariableList namedList = mmsServer_getNamedVariableListWithName(connection->serverConnection->server->device->namedVariableLists, listName);

        if (namedList == NULL) {
            ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

            if (response) {
                mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

                MmsEndpointConnection_sendMessage(connection, response);

                MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
            }
        }
        else {

            struct sVarAccessSpec accessSpec;

            accessSpec.specific = 0;
            accessSpec.domainId = NULL;
            accessSpec.itemId = listName;
            accessSpec.component = NULL;
            accessSpec.next = NULL;

            createNamedVariableListResponse(connection, namedList, invokeId, read, &accessSpec);
        }
	}
#if (MMS_DYNAMIC_DATA_SETS == 1)
	else if (read->variableAccessSpecification.choice.variableListName.present ==
				ObjectName_PR_aaspecific)
	{
        char listName[65];

        mmsMsg_copyAsn1IdentifierToStringBuffer(read->variableAccessSpecification.choice.variableListName.choice.aaspecific,
                listName, 65);

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_INFO, connection, "MMS_SERVER: read (invoke-id: %u) access to association specific variable list - %s", invokeId, listName);

		MmsNamedVariableList namedList = MmsEndpointServerConnection_getNamedVariableList(connection->serverConnection, listName);

		if (namedList == NULL) {
	        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

	        if (response) {
	            mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT);

	            MmsEndpointConnection_sendMessage(connection, response);

	            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
	        }
		}
		else {
            struct sVarAccessSpec accessSpec;

            accessSpec.specific = 2;
            accessSpec.domainId = NULL;
            accessSpec.itemId = listName;
            accessSpec.component = NULL;
            accessSpec.next = NULL;

			createNamedVariableListResponse(connection, namedList, invokeId, read, &accessSpec);
		}
	}
#endif /* (MMS_DYNAMIC_DATA_SETS == 1) */
	else {
        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }
	}

}

#endif /* MMS_DATA_SET_SERVICE == 1 */

void
mmsServer_handleReadRequest(
		MmsEndpointConnection connection,
		uint8_t* buffer, int bufPos, int maxBufPos,
		uint32_t invokeId)
{
	ReadRequest_t* request = 0; /* allow asn1c to allocate structure */

	MmsPdu_t* mmsPdu = 0;

	asn_dec_rval_t rval = ber_decode(NULL, &asn_DEF_MmsPdu, (void**) &mmsPdu, buffer, CONFIG_MMS_MAXIMUM_PDU_SIZE);

	if (rval.code != RC_OK) {

        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: read (invoke-id: %u) access - invalid PDU -> reject", invokeId);

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createMmsRejectPdu(&invokeId, MMS_ERROR_REJECT_INVALID_PDU, response);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }

	    goto exit_function;
	}

	request = &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.read);

	if (request->variableAccessSpecification.present == VariableAccessSpecification_PR_listOfVariable) {
		handleReadListOfVariablesRequest(connection, request, invokeId);
	}
#if (MMS_DATA_SET_SERVICE == 1)
	else if (request->variableAccessSpecification.present == VariableAccessSpecification_PR_variableListName) {
		handleReadNamedVariableListRequest(connection, request, invokeId);
	}
#endif
	else {
        if (DEBUG_MMS_SERVER)
            libmms_log(MMS_LOG_ERROR, connection, "MMS_SERVER: read (invoke-id: %u) access - service error -> object-access-unsupported", invokeId);

        ByteBuffer* response = MmsEndpoint_reserveTransmitBuffer(connection->endpoint);

        if (response) {
            mmsMsg_createServiceErrorPdu(invokeId, response, MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED);

            MmsEndpointConnection_sendMessage(connection, response);

            MmsEndpoint_releaseTransmitBuffer(connection->endpoint);
        }
	}

exit_function:	
	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

