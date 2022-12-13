/*
 *  im_transfer_set.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2018-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */


#include "tase2_server_internal.h"
#include "tase2_common_internal.h"

#include "lib_memory.h"

static bool
sendInformationMessage(Tase2_Server self, Tase2_Domain domain, int32_t infoRef, int32_t localRef, int32_t msgId, int32_t size, Tase2_InformationMessage infoMsg, uint8_t* buffer)
{
    if (size <= infoMsg->maxSize) {

        char* domainName;

        if (domain)
            domainName = domain->name;
        else
            domainName = NULL;

        MmsValue* infoHeader = MmsValue_createEmptyStructure(4);
        MmsValue_setElement(infoHeader, 0, MmsValue_newIntegerFromInt32(infoRef));
        MmsValue_setElement(infoHeader, 1, MmsValue_newIntegerFromInt32(localRef));
        MmsValue_setElement(infoHeader, 2, MmsValue_newIntegerFromInt32(msgId));
        MmsValue_setElement(infoHeader, 3, MmsValue_newIntegerFromInt32(size));

        LinkedList varAccessSpecs = LinkedList_create();
        LinkedList values = LinkedList_create();

        MmsVariableAccessSpecification* varAccessSpec = MmsVariableAccessSpecification_create(domainName, "Info_Mess_Header");

        LinkedList_add(varAccessSpecs, varAccessSpec);
        LinkedList_add(values, infoHeader);

        char infoBufferName[65];

        sprintf(infoBufferName, "Info_Buff_%d", infoMsg->maxSize);

        varAccessSpec = MmsVariableAccessSpecification_create(domainName, infoBufferName);

        LinkedList_add(varAccessSpecs, varAccessSpec);

        uint8_t* infoBufBuf = MmsValue_getOctetStringBuffer((MmsValue*) (infoMsg->infoBuf));

        memset(infoBufBuf, 0, infoMsg->maxSize);
        memcpy(infoBufBuf, buffer, size);

        LinkedList_add(values, infoMsg->infoBuf);

        /* send report to listening clients */

        Semaphore_wait(self->imEnabledConnectionsLock);

        LinkedList element = LinkedList_getNext(self->imEnabledConnections);

        while (element) {

            MmsEndpointConnection connection = (MmsEndpointConnection) LinkedList_getData(element);

            /* check if client matches BLT rules */

            Tase2_BilateralTable blt = (Tase2_BilateralTable) MmsEndpointConnection_getSecurityToken(connection);

            bool sendReport = false;

            if (blt) {
                if (BilateralTable_checkInformationMessageAccess(blt, domain, infoRef, localRef)) {
                    sendReport = true;
                }
            }
            else
                sendReport = true;

            if (sendReport) {
                MmsEndpointConnection_lockConnection(connection);
                MmsEndpointConnection_sendInformationReportListOfVariables(connection, varAccessSpecs, values);
                MmsEndpointConnection_unlockConnection(connection);

                Tase2_Endpoint_Connection peer = Tase2_Endpoint_getLockedEndpointConnection(self->endpoint, connection);

                tase2_log(TASE2_LOG_DEBUG, TASE2_LOG_SOURCE_ICCP, self->endpoint, peer, "sent information message info-ref: %i local-ref %i msg-id: %i", infoRef, localRef, msgId);

                Tase2_Endpoint_unlockEndpointConnection(self->endpoint, peer);
            }

            element = LinkedList_getNext(element);
        }

        Semaphore_post(self->imEnabledConnectionsLock);

        LinkedList_destroy(varAccessSpecs);
        LinkedList_destroyStatic(values);

        MmsValue_delete(infoHeader);

        return true;
    }
    else {
        tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "information message too large for buffer");
        return false;
    }
}

int
Tase2_Server_sendInformationMessage(Tase2_Server self, Tase2_Domain domain, int32_t infoRef, int32_t localRef, int32_t msgId, int32_t size, uint8_t* buffer)
{
    int maxSize = 0;

    Tase2_Domain searchDomain;
    Tase2_InformationMessage infoBuf = NULL;

    if (domain == NULL)
        searchDomain = self->dataModel->vcc;
    else
        searchDomain = domain;

    if (searchDomain->informationMessages == NULL) {
        tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "no information messages defined for domain %s", searchDomain->name);
        return 1;
    }
    else {
        Tase2_InformationMessage im = searchDomain->informationMessages;

        if (size <= im->maxSize) {
            maxSize = im->maxSize;
            infoBuf = im;
        }

        while (im->sibling) {

            im = im->sibling;

            if (size <= im->maxSize) {
                if (infoBuf) {
                    if ((im->maxSize - size) < (maxSize - size)) {
                        maxSize = im->maxSize;
                        infoBuf = im;
                    }
                }
                else {
                    maxSize = im->maxSize;
                    infoBuf = im;
                }
            }
        }
    }

    if (infoBuf) {
        if (sendInformationMessage(self, domain, infoRef, localRef, msgId, size, infoBuf, buffer))
            return 0;
        else
            return 2;
    }
    else {
        tase2_log(TASE2_LOG_WARNING, TASE2_LOG_SOURCE_ICCP, self->endpoint, NULL, "no buffer found to send information message");

        return 2;
    }
}
