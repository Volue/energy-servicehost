/*
 *  mms_information_report.c
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

#include "ber_encoder.h"

void
MmsEndpointConnection_sendInformationReportNamedVariableList(MmsEndpointConnection self,
        const char* domainId, const char* itemId, LinkedList values)
{
    ByteBuffer* reportBuffer;
    uint8_t* buffer;
    int bufPos = 0;

    uint32_t variableAccessSpecSize = 0;
    uint32_t objectNameSize = 0;
    uint32_t domainSpecificSize = 0;

    uint32_t accessResultSize = 0;
    uint32_t listOfAccessResultSize;
    uint32_t informationReportSize;

    if (domainId != NULL) {
        uint32_t nameElementsSize = BerEncoder_determineEncodedStringSize(itemId);

        nameElementsSize += BerEncoder_determineEncodedStringSize(domainId);

        domainSpecificSize = nameElementsSize;

        objectNameSize = 1 + BerEncoder_determineLengthSize(domainSpecificSize) + domainSpecificSize;
    }
    else {
        objectNameSize = BerEncoder_determineEncodedStringSize(itemId);
    }

    variableAccessSpecSize += objectNameSize;
    variableAccessSpecSize += BerEncoder_determineLengthSize(objectNameSize);
    variableAccessSpecSize += 1; /* space for tag (a1) */

    /* iterate values list and add values to the accessResultList */
    LinkedList value = LinkedList_getNext(values);

    while (value != NULL) {

        MmsValue* data = (MmsValue*) value->data;

        accessResultSize += MmsValue_encodeMmsData(data, NULL, 0, false);

        value = LinkedList_getNext(value);
    }

    listOfAccessResultSize = accessResultSize +
            BerEncoder_determineLengthSize(accessResultSize) + 1;

    uint32_t informationReportContentSize = variableAccessSpecSize + listOfAccessResultSize;

    informationReportSize = 1 +  informationReportContentSize +
            BerEncoder_determineLengthSize(informationReportContentSize);

    uint32_t completeMessageSize = 1 + informationReportSize + BerEncoder_determineLengthSize(informationReportSize);

    if (completeMessageSize > self->maxPduSize) {

        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: report message too large %i (max = %i) -> skip message!", completeMessageSize, self->maxPduSize);

        goto exit_function;
    }

    if (DEBUG_MMS_ENDPOINT) libmms_log(MMS_LOG_DEBUG, self, "MMS_ENDPOINT: send report (named variable list)");

    reportBuffer = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

    buffer = reportBuffer->buffer;

    /* encode */
    bufPos = BerEncoder_encodeTL(0xa3, informationReportSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0xa0, informationReportContentSize, buffer, bufPos);

    bufPos = BerEncoder_encodeTL(0xa1, objectNameSize, buffer, bufPos);

    if (domainId != NULL) {
        bufPos = BerEncoder_encodeTL(0xa1, domainSpecificSize, buffer, bufPos);
        bufPos = BerEncoder_encodeStringWithTag(0x1a, domainId, buffer, bufPos);
        bufPos = BerEncoder_encodeStringWithTag(0x1a, itemId, buffer, bufPos);
    }
    else {
        bufPos = BerEncoder_encodeStringWithTag(0x80, itemId, buffer, bufPos);
    }

    bufPos = BerEncoder_encodeTL(0xa0, accessResultSize, buffer, bufPos);

    value = LinkedList_getNext(values);

    while (value != NULL) {

        MmsValue* data = (MmsValue*) value->data;

        bufPos = MmsValue_encodeMmsData(data, buffer, bufPos, true);

        value = LinkedList_getNext(value);
    }

    reportBuffer->size = bufPos;

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
    self->endpoint->statAplMessagesSent++;
#endif

    if (self->endpoint->passiveMode) {
        IsoConnection_sendMessage(self->isoConnection, reportBuffer);
    }
    else {
        IsoClientConnection_sendMessage(self->isoClient, reportBuffer);
    }

    MmsEndpoint_releaseTransmitBuffer(self->endpoint);

exit_function:
    return;
}

void
MmsEndpointConnection_sendInformationReportSingleVariableVMDSpecific(MmsEndpointConnection self,
        char* itemId, MmsValue* value)
{
    ByteBuffer* reportBuffer;
    uint8_t* buffer;
    int bufPos = 0;

    uint32_t itemIdSize = strlen(itemId);
    uint32_t varSpecSize = 1 + BerEncoder_determineLengthSize(itemIdSize) + itemIdSize;
    uint32_t sequenceSize = 1 + BerEncoder_determineLengthSize(varSpecSize) + varSpecSize;
    uint32_t listOfVariableSize = 1 + BerEncoder_determineLengthSize(sequenceSize) + sequenceSize;

    uint32_t accessResultSize = MmsValue_encodeMmsData(value, NULL, 0, false);

    uint32_t listOfAccessResultSize = 1 + BerEncoder_determineLengthSize(accessResultSize) + accessResultSize;

    uint32_t variableSpecSize = 1 + BerEncoder_determineLengthSize(listOfVariableSize) + listOfVariableSize;

    uint32_t informationReportContentSize = variableSpecSize + listOfAccessResultSize;

    uint32_t informationReportSize = 1 + BerEncoder_determineLengthSize(informationReportContentSize) +
            informationReportContentSize;

    uint32_t completeMessageSize = 1 + informationReportSize + BerEncoder_determineLengthSize(informationReportSize);

    if (completeMessageSize > self->maxPduSize) {

        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: report message too large %i (max = %i) -> skip message!", completeMessageSize, self->maxPduSize);

        goto exit_function;
    }

    if (DEBUG_MMS_ENDPOINT) libmms_log(MMS_LOG_DEBUG, self, "MMS_ENDPOINT: send report (single variable)");

    reportBuffer = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

    buffer = reportBuffer->buffer;

    /* encode information report header */
    bufPos = BerEncoder_encodeTL(0xa3, informationReportSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0xa0, informationReportContentSize, buffer, bufPos);

    /* encode list of variable access specifications */
    bufPos = BerEncoder_encodeTL(0xa0, listOfVariableSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x30, sequenceSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0xa0, varSpecSize, buffer, bufPos);
    bufPos = BerEncoder_encodeStringWithTag(0x80, itemId, buffer, bufPos);

    /* encode access result (variable value) */
    bufPos = BerEncoder_encodeTL(0xa0, accessResultSize, buffer, bufPos);
    bufPos = MmsValue_encodeMmsData(value, buffer, bufPos, true);

    reportBuffer->size = bufPos;

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
    self->endpoint->statAplMessagesSent++;
#endif

    if (self->endpoint->passiveMode) {
        IsoConnection_sendMessage(self->isoConnection, reportBuffer);
    }
    else {
        IsoClientConnection_sendMessage(self->isoClient, reportBuffer);
    }

    MmsEndpoint_releaseTransmitBuffer(self->endpoint);

exit_function:
    return;
}

void
MmsEndpointConnection_sendInformationReportListOfVariables(MmsEndpointConnection self,
        LinkedList /* MmsVariableAccessSpecification */ variableAccessDeclarations,
        LinkedList /* MmsValue */ values)
{
    ByteBuffer* reportBuffer;
    uint8_t* buffer;
    int bufPos = 0;

    int numberOfVariables = 0;

    /* determine message size */
    uint32_t listOfVariableSize = 0;

    LinkedList specElement = LinkedList_getNext(variableAccessDeclarations);

    while (specElement != NULL) {
        MmsVariableAccessSpecification* spec = (MmsVariableAccessSpecification*) specElement->data;

        uint32_t nameSpecifierSize;

        if (spec->domainId != NULL) {

            uint32_t nameElementsSize = BerEncoder_determineEncodedStringSize(spec->itemId);

            nameElementsSize += BerEncoder_determineEncodedStringSize(spec->domainId);

            nameSpecifierSize = (nameElementsSize + 1 + BerEncoder_determineLengthSize(nameElementsSize));
        }
        else {
            nameSpecifierSize = BerEncoder_determineEncodedStringSize(spec->itemId);
        }

        uint32_t varSpecNameSize = (1 + BerEncoder_determineLengthSize(nameSpecifierSize) + nameSpecifierSize);

        listOfVariableSize += (1 + BerEncoder_determineLengthSize(varSpecNameSize) + varSpecNameSize);

        numberOfVariables++;

        specElement = LinkedList_getNext(specElement);
    }

    uint32_t accessResultSize = 0;

    LinkedList valueElement = LinkedList_getNext(values);

    while (valueElement != NULL) {
        MmsValue* value = (MmsValue*) valueElement->data;

        accessResultSize += MmsValue_encodeMmsData(value, NULL, 0, false);

        valueElement = LinkedList_getNext(valueElement);
    }

    uint32_t listOfAccessResultSize = 1 + BerEncoder_determineLengthSize(accessResultSize) + accessResultSize;

    uint32_t variableSpecSize = 1 + BerEncoder_determineLengthSize(listOfVariableSize) + listOfVariableSize;

    uint32_t informationReportContentSize = variableSpecSize + listOfAccessResultSize;

    uint32_t informationReportSize = 1 + BerEncoder_determineLengthSize(informationReportContentSize) +
            informationReportContentSize;

    uint32_t completeMessageSize = 1 + informationReportSize + BerEncoder_determineLengthSize(informationReportSize);

    if (completeMessageSize > self->maxPduSize) {
        if (DEBUG_MMS_ENDPOINT)
            libmms_log(MMS_LOG_ERROR, self, "MMS_ENDPOINT: report message too large %i (max = %i) -> skip message!", completeMessageSize, self->maxPduSize);

        goto exit_function;
    }

    if (DEBUG_MMS_ENDPOINT) libmms_log(MMS_LOG_DEBUG, self, "MMS_ENDPOINT: send report (%i variables)", numberOfVariables);

    /* encode message */
    reportBuffer = MmsEndpoint_reserveTransmitBuffer(self->endpoint);

    buffer = reportBuffer->buffer;

    /* encode information report header */
    bufPos = BerEncoder_encodeTL(0xa3, informationReportSize, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0xa0, informationReportContentSize, buffer, bufPos);

    /* encode list of variable access specifications */
    bufPos = BerEncoder_encodeTL(0xa0, listOfVariableSize, buffer, bufPos);

    specElement = LinkedList_getNext(variableAccessDeclarations);

    while (specElement != NULL) {
        MmsVariableAccessSpecification* spec = (MmsVariableAccessSpecification*) specElement->data;

        uint32_t varSpecSize = BerEncoder_determineEncodedStringSize(spec->itemId);

        if (spec->domainId != NULL) {

            varSpecSize += BerEncoder_determineEncodedStringSize(spec->domainId);
            uint32_t varSpecSizeComplete =   varSpecSize + BerEncoder_determineLengthSize(varSpecSize) + 1;
            uint32_t sequenceSize = varSpecSizeComplete + BerEncoder_determineLengthSize(varSpecSizeComplete) + 1;

            bufPos = BerEncoder_encodeTL(0x30, sequenceSize, buffer, bufPos);
            bufPos = BerEncoder_encodeTL(0xa0, varSpecSizeComplete, buffer, bufPos); /* domain-specific */
            bufPos = BerEncoder_encodeTL(0xa1, varSpecSize, buffer, bufPos);
            bufPos = BerEncoder_encodeStringWithTag(0x1a, spec->domainId, buffer, bufPos);
            bufPos = BerEncoder_encodeStringWithTag(0x1a, spec->itemId, buffer, bufPos);
        }
        else {
            uint32_t sequenceSize = varSpecSize + BerEncoder_determineLengthSize(varSpecSize) + 1;
            bufPos = BerEncoder_encodeTL(0x30, sequenceSize, buffer, bufPos);
            bufPos = BerEncoder_encodeTL(0xa0, varSpecSize, buffer, bufPos); /* vmd-specific */
            bufPos = BerEncoder_encodeStringWithTag(0x80, spec->itemId, buffer, bufPos);
        }

        specElement = LinkedList_getNext(specElement);
    }

    /* encode list of access results (variable values) */
    bufPos = BerEncoder_encodeTL(0xa0, accessResultSize, buffer, bufPos);

    valueElement = LinkedList_getNext(values);

    while (valueElement != NULL) {
        MmsValue* value = (MmsValue*) valueElement->data;

        bufPos = MmsValue_encodeMmsData(value, buffer, bufPos, true);

        valueElement = LinkedList_getNext(valueElement);
    }

    reportBuffer->size = bufPos;

#if (CONFIG_MMS_COLLECT_STATISTICS == 1)
    self->endpoint->statAplMessagesSent++;
#endif

    if (self->endpoint->passiveMode) {
        MmsEndpointConnection_sendMessage(self, reportBuffer);
    }
    else {
        IsoClientConnection_sendMessage(self->isoClient, reportBuffer);
    }

    MmsEndpoint_releaseTransmitBuffer(self->endpoint);

exit_function:
    return;
}

