/*
 *  iso_connection_parameters.c
 *
 *  IsoConnectionParameters abstract data type to represent the configurable parameters of the ISO protocol stack.
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "stack_config.h"

#include "iso_connection_parameters.h"

#include "libmms_platform_includes.h"
#include "ber_encoder.h"

AcseAuthenticationParameter
AcseAuthenticationParameter_create()
{
    AcseAuthenticationParameter self = (AcseAuthenticationParameter)
        GLOBAL_CALLOC(1, sizeof(struct sAcseAuthenticationParameter));

    return self;
}

void
AcseAuthenticationParameter_destroy(AcseAuthenticationParameter self)
{
    if (self->mechanism == ACSE_AUTH_PASSWORD)
        if (self->value.password.octetString != NULL)
            GLOBAL_FREEMEM(self->value.password.octetString);

    GLOBAL_FREEMEM(self);
}

void
AcseAuthenticationParameter_setPassword(AcseAuthenticationParameter self, char* password)
{
    self->value.password.octetString = (uint8_t*) StringUtils_copyString(password);
    self->value.password.passwordLength = strlen(password);
}

void
AcseAuthenticationParameter_setAuthMechanism(AcseAuthenticationParameter self, AcseAuthenticationMechanism mechanism)
{
    self->mechanism = mechanism;
}

static void
convertApTitleString(const char* apTitle, ItuObjectIdentifier* oid)
{
    /* automatically select separator character - "." or "," */
   char seperatorChar = '.';

   if (strchr((char*) apTitle, ','))
       seperatorChar = ',';

   int arcCount = 1;

   oid->arc[0] = atoi(apTitle);

   char* separator = strchr((char*)apTitle, seperatorChar);

   while (separator) {
       oid->arc[arcCount++] = atoi(separator + 1);

       separator = strchr(separator + 1, seperatorChar);
   }

   oid->arcCount = arcCount;
}

void
ItuObjectIdentifier_setFromString(ItuObjectIdentifier* self, const char* str)
{
    convertApTitleString(str, self);
}

char*
ItuObjectIdentifier_toString(ItuObjectIdentifier* self)
{
    int i;
    char strbuf[100];
    strbuf[0] = 0;

    int startPos = 0;

    for (i = 0; i < self->arcCount; i++) {
        startPos += sprintf(strbuf + startPos, "%hu", self->arc[i]);

        if ((i + 1) != self->arcCount) {
            strbuf[startPos++] = '.';
        }
    }

    return StringUtils_copyString(strbuf);
}

void
IsoApplicationAddress_setApTitle(IsoApplicationAddress self, const char* apTitle)
{
    if (apTitle == NULL)
        self->apTitle.arcCount = 0;
    else {
        convertApTitleString(apTitle, &(self->apTitle));
    }
}

void
IsoApplicationAddress_setAeQualifier(IsoApplicationAddress self, int aeQualifier)
{
    self->hasAeQualifier = 1;
    self->aeQualifier = aeQualifier;
}

void
IsoApplicationAddress_setAeInvocationId(IsoApplicationAddress self, int aeInvocationId)
{
    self->hasAeInvocationId = 1;
    self->aeInvocationId = aeInvocationId;
}
void
IsoApplicationAddress_setApInvocationId(IsoApplicationAddress self, int apInvocationId)
{
    self->hasApInvocationId = 1;
    self->apInvocationId = apInvocationId;
}

void
IsoApplicationAddress_setLowerLayerAddresses(IsoApplicationAddress self, PSelector pSelector, SSelector sSelector, TSelector tSelector)
{
    self->pSelector = pSelector;
    self->sSelector = sSelector;
    self->tSelector = tSelector;
}

