/*
 *  mms_client_get_namelist.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include <MmsPdu.h>
#include "mms_client.h"
#include "libmms_platform_includes.h"
#include "stack_config.h"
#include "mms_common.h"
#include "byte_buffer.h"
#include "mms_client_internal.h"

#include "ber_decode.h"

int
mmsClient_createMmsGetNameListRequestVMDspecific(long invokeId, ByteBuffer* writeBuffer,
		const char* continueAfter)
{
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(invokeId);

	mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.present =
				ConfirmedServiceRequest_PR_getNameList;

	GetNameListRequest_t* request;

	request = &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.getNameList);

	if (continueAfter != NULL) {
		request->continueAfter = (Identifier_t*) GLOBAL_CALLOC(1, sizeof(Identifier_t));
		request->continueAfter->buf = (uint8_t*) StringUtils_copyString(continueAfter);
		request->continueAfter->size = strlen(continueAfter);
	}
	else
		request->continueAfter = NULL;

	request->objectScope.present = GetNameListRequest__objectScope_PR_vmdSpecific;
	request->objectClass.present = ObjectClass_PR_basicObjectClass;

	asn_long2INTEGER(&request->objectClass.choice.basicObjectClass,
			ObjectClass__basicObjectClass_domain);

	asn_enc_rval_t rval;

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
		(asn_app_consume_bytes_f*) mmsClient_write_out, (void*) writeBuffer);

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}

int
mmsClient_createMmsGetNameListRequestAssociationSpecific(long invokeId, ByteBuffer* writeBuffer,
		const char* continueAfter)
{
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(invokeId);

	mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.present =
				ConfirmedServiceRequest_PR_getNameList;

	GetNameListRequest_t* request;

	request = &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.getNameList);


	if (continueAfter != NULL) {
		request->continueAfter = (Identifier_t*) GLOBAL_CALLOC(1, sizeof(Identifier_t));
		request->continueAfter->buf = (uint8_t*) StringUtils_copyString(continueAfter);
		request->continueAfter->size = strlen(continueAfter);
	}
	else
		request->continueAfter = NULL;


	request->objectScope.present = GetNameListRequest__objectScope_PR_aaSpecific;
	request->objectClass.present = ObjectClass_PR_basicObjectClass;

	asn_long2INTEGER(&request->objectClass.choice.basicObjectClass,
				ObjectClass__basicObjectClass_namedVariableList);

	asn_enc_rval_t rval;

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
		(asn_app_consume_bytes_f*) mmsClient_write_out, (void*) writeBuffer);

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}

bool
mmsClient_parseGetNameListResponse(MmsEndpointConnection self, LinkedList* nameList, ByteBuffer* message)
{
    /* TODO only parse get name list specific part here */

    bool moreFollows = true;

    uint8_t* buffer = message->buffer;
    int maxBufPos = message->size;
    int bufPos = 0;
    int length;
    int listEndPos;
    LinkedList element;

    uint8_t tag = buffer[bufPos++];
    if (tag == 0xa2) {
        /* TODO parse confirmed error PDU */
        goto exit_error;
    }
    if (tag != 0xa1) goto exit_error;

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
    if (bufPos < 0) goto exit_error;

    /* get invokeId */
    tag = buffer[bufPos++];
    if (tag != 0x02) goto exit_error;

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
    if (bufPos < 0) goto exit_error;

    bufPos += length;

    tag = buffer[bufPos++];
    if (tag != 0xa1) goto exit_error;

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
    if (bufPos < 0) goto exit_error;

    tag = buffer[bufPos++];
    if (tag != 0xa0) goto exit_error;

    bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
    if (bufPos < 0) goto exit_error;

    listEndPos = bufPos + length;
    if (listEndPos > maxBufPos) goto exit_error;

    if (*nameList == NULL)
        *nameList = LinkedList_create();

    element = LinkedList_getLastElement(*nameList);

    while (bufPos < listEndPos) {
        tag = buffer[bufPos++];
        if (tag != 0x1a) goto exit_error;

        bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
        if (bufPos < 0) goto exit_error;

        char* variableName = StringUtils_createStringFromBuffer(buffer + bufPos, length);

        element = LinkedList_insertAfter(element, variableName);

        bufPos += length;
    }

    if (bufPos < maxBufPos) {
		tag = buffer[bufPos++];
		if (tag != 0x81) goto exit_error;
		bufPos = BerDecoder_decodeLength(buffer, &length, bufPos, maxBufPos);
		if (bufPos < 0) goto exit_error;
		if (length != 1) goto exit_error;
		if (buffer[bufPos++] > 0)
			moreFollows = true;
		else
			moreFollows = false;
    }

	return moreFollows;

exit_error:
    if (*nameList != NULL) {
        LinkedList_destroy(*nameList);
        *nameList = NULL;
    }

    if (DEBUG_MMS_CLIENT)
        libmms_log(MMS_LOG_ERROR, self, "MMS_CLIENT: get-name-list-response: error parsing message");

    return false;
}

int
mmsClient_createGetNameListRequestDomainOrVMDSpecific(long invokeId, const char* domainName,
		ByteBuffer* writeBuffer, MmsObjectClass objectClass, const char* continueAfter)
{
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(invokeId);

	mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.present =
				ConfirmedServiceRequest_PR_getNameList;

	GetNameListRequest_t* request;

	request = &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.getNameList);

	if (continueAfter != NULL) {
		request->continueAfter = (Identifier_t*) GLOBAL_CALLOC(1, sizeof(Identifier_t));
		request->continueAfter->buf = (uint8_t*) StringUtils_copyString(continueAfter);
		request->continueAfter->size = strlen(continueAfter);
	}
	else
		request->continueAfter = NULL;


	if (domainName != NULL) {
        request->objectScope.present = GetNameListRequest__objectScope_PR_domainSpecific;
        request->objectScope.choice.domainSpecific.buf = (uint8_t*) domainName;
        request->objectScope.choice.domainSpecific.size = strlen(domainName);
	}
	else {
	    request->objectScope.present = GetNameListRequest__objectScope_PR_vmdSpecific;
	}

	request->objectClass.present = ObjectClass_PR_basicObjectClass;

	asn_long2INTEGER(&request->objectClass.choice.basicObjectClass, objectClass);

	asn_enc_rval_t rval;

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
		(asn_app_consume_bytes_f*) mmsClient_write_out, (void*) writeBuffer);

	request->objectScope.choice.domainSpecific.buf = 0;
	request->objectScope.choice.domainSpecific.size = 0;

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}
