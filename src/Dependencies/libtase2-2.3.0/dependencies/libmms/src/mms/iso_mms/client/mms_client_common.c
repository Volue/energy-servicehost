/*
 *  mms_client_common.c
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
#include "mms_common.h"
#include "byte_buffer.h"

#include "mms_client_internal.h"

int
mmsClient_write_out(void *buffer, size_t size, void *app_key)
{
    ByteBuffer* writeBuffer = (ByteBuffer*) app_key;
    return ByteBuffer_append(writeBuffer, (uint8_t*) buffer, size);
}


uint32_t
mmsClient_getInvokeId(ConfirmedResponsePdu_t* confirmedResponse)
{
	long invokeId;

	asn_INTEGER2long(&confirmedResponse->invokeID, &invokeId);

	return (uint32_t) invokeId;
}


MmsPdu_t*
mmsClient_createConfirmedRequestPdu(uint32_t invokeId)
{
	MmsPdu_t* mmsPdu = (MmsPdu_t*) GLOBAL_CALLOC(1, sizeof(MmsPdu_t));
	mmsPdu->present = MmsPdu_PR_confirmedRequestPdu;

	asn_long2INTEGER(&(mmsPdu->choice.confirmedRequestPdu.invokeID), invokeId);

	return mmsPdu;
}

AlternateAccess_t*
mmsClient_createAlternateAccess(uint32_t index, uint32_t elementCount)
{
    AlternateAccess_t* alternateAccess = (AlternateAccess_t*) GLOBAL_CALLOC(1, sizeof(AlternateAccess_t));
    alternateAccess->list.count = 1;
    alternateAccess->list.array = (struct AlternateAccess__Member**) GLOBAL_CALLOC(1, sizeof(struct AlternateAccess__Member*));
    alternateAccess->list.array[0] = (struct AlternateAccess__Member*) GLOBAL_CALLOC(1, sizeof(struct AlternateAccess__Member));
    alternateAccess->list.array[0]->present = AlternateAccess__Member_PR_unnamed;

    alternateAccess->list.array[0]->choice.unnamed = (AlternateAccessSelection_t*) GLOBAL_CALLOC(1, sizeof(AlternateAccessSelection_t));

    alternateAccess->list.array[0]->choice.unnamed->present = AlternateAccessSelection_PR_selectAccess;

    if (elementCount > 0) {
        alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.present =
                AlternateAccessSelection__selectAccess_PR_indexRange;

        INTEGER_t* asnIndex =
            &(alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.indexRange.lowIndex);

        asn_long2INTEGER(asnIndex, index);

        asnIndex =
            &(alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.indexRange.numberOfElements);

        asn_long2INTEGER(asnIndex, elementCount);
    }
    else {
        alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.present =
                AlternateAccessSelection__selectAccess_PR_index;

        INTEGER_t* asnIndex =
            &(alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.index);

        asn_long2INTEGER(asnIndex, index);
    }

    return alternateAccess;
}

void
mmsClient_deleteAlternateAccess(AlternateAccess_t* alternateAccess)
{
    if (alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.indexRange.lowIndex.buf != NULL) {
         GLOBAL_FREEMEM(alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.indexRange.lowIndex.buf);
         alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.indexRange.lowIndex.buf = NULL;
    }

    if (alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.indexRange.numberOfElements.buf != NULL) {
        GLOBAL_FREEMEM(alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.indexRange.numberOfElements.buf);
        alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.indexRange.numberOfElements.buf = NULL;
    }

    if (alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.index.buf != NULL) {
        GLOBAL_FREEMEM(alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.index.buf);
        alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.index.buf = NULL;
    }

    if (alternateAccess->list.array[0]->choice.unnamed->present == AlternateAccessSelection_PR_selectAlternateAccess) {
        if (alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.alternateAccess != NULL) {
            mmsClient_deleteAlternateAccess(alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.alternateAccess);
        }
    }

    GLOBAL_FREEMEM(alternateAccess->list.array[0]->choice.unnamed);
    GLOBAL_FREEMEM(alternateAccess->list.array[0]);
    GLOBAL_FREEMEM(alternateAccess->list.array);
    GLOBAL_FREEMEM(alternateAccess);
}

static AlternateAccess_t*
createAlternateAccessComponent(const char* componentName)
{
    AlternateAccess_t* alternateAccess = (AlternateAccess_t*) GLOBAL_CALLOC(1, sizeof(AlternateAccess_t));
    alternateAccess->list.count = 1;
    alternateAccess->list.array = (struct AlternateAccess__Member**) GLOBAL_CALLOC(1, sizeof(struct AlternateAccess__Member*));
    alternateAccess->list.array[0] = (struct AlternateAccess__Member*) GLOBAL_CALLOC(1, sizeof(struct AlternateAccess__Member));
    alternateAccess->list.array[0]->present = AlternateAccess__Member_PR_unnamed;

    alternateAccess->list.array[0]->choice.unnamed = (AlternateAccessSelection_t*) GLOBAL_CALLOC(1, sizeof(AlternateAccessSelection_t));

    const char* separator = strchr(componentName, CONFIG_MMS_SEPARATOR_CHAR);

    if (separator) {
        int size = separator - componentName;

        alternateAccess->list.array[0]->choice.unnamed->present = AlternateAccessSelection_PR_selectAlternateAccess;
        alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.accessSelection.present =
                AlternateAccessSelection__selectAlternateAccess__accessSelection_PR_component;

        alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.accessSelection.choice.component.buf =
            (uint8_t*) StringUtils_copySubString((char*) componentName, (char*) separator);
        alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.accessSelection.choice.component.size = size;

        alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.alternateAccess = createAlternateAccessComponent(separator + 1);
    }
    else {
        int size = strlen(componentName);

        alternateAccess->list.array[0]->choice.unnamed->present = AlternateAccessSelection_PR_selectAccess;

        alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.present =
                AlternateAccessSelection__selectAccess_PR_component;

        alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.component.buf =
            (uint8_t*) StringUtils_copyString(componentName);
        alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.component.size = size;
    }

    return alternateAccess;
}

AlternateAccess_t*
mmsClient_createAlternateAccessIndexComponent(uint32_t index, const char* componentName)
{
    AlternateAccess_t* alternateAccess = (AlternateAccess_t*) GLOBAL_CALLOC(1, sizeof(AlternateAccess_t));
    alternateAccess->list.count = 1;
    alternateAccess->list.array = (struct AlternateAccess__Member**) GLOBAL_CALLOC(1, sizeof(struct AlternateAccess__Member*));
    alternateAccess->list.array[0] = (struct AlternateAccess__Member*) GLOBAL_CALLOC(1, sizeof(struct AlternateAccess__Member));
    alternateAccess->list.array[0]->present = AlternateAccess__Member_PR_unnamed;

    alternateAccess->list.array[0]->choice.unnamed = (AlternateAccessSelection_t*) GLOBAL_CALLOC(1, sizeof(AlternateAccessSelection_t));

    if (componentName) {
        alternateAccess->list.array[0]->choice.unnamed->present = AlternateAccessSelection_PR_selectAlternateAccess;

        alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.accessSelection.present =
                AlternateAccessSelection__selectAlternateAccess__accessSelection_PR_index;

        INTEGER_t* asnIndex =
                &(alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.index);

        asn_long2INTEGER(asnIndex, index);

        alternateAccess->list.array[0]->choice.unnamed->choice.selectAlternateAccess.alternateAccess = createAlternateAccessComponent(componentName);
    }
    else {
        alternateAccess->list.array[0]->choice.unnamed->present = AlternateAccessSelection_PR_selectAccess;

        alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.present =
                        AlternateAccessSelection__selectAccess_PR_index;

        INTEGER_t* asnIndex =
                &(alternateAccess->list.array[0]->choice.unnamed->choice.selectAccess.choice.index);

        asn_long2INTEGER(asnIndex, index);
    }

    return alternateAccess;
}
