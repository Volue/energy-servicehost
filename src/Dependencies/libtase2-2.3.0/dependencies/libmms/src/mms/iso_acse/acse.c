/*
 *  acse.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "acse.h"

#include "libmms_platform_includes.h"
#include "mms_common_internal.h"
#include "ber_encoder.h"
#include "ber_decode.h"

#if ((DEBUG_ISO_CLIENT == 1) || (DEBUG_ISO_SERVER == 1))
#define DEBUG_ACSE 1
#else
#define DEBUG_ACSE 0
#endif

static uint8_t appContextNameMms[] = { 0x28, 0xca, 0x22, 0x02, 0x03 };

static uint8_t auth_mech_password_oid[] = { 0x52, 0x03, 0x01 };

static uint8_t requirements_authentication[] = { 0x80 };

#define ACSE_ASSOCIATE_RESULT_ACCEPTED 0
#define ACSE_ASSOCIATE_RESULT_REJECTED_PERMANENT 1
#define ACSE_ASSOCIATE_RESULT_REJECTED_TRANSIENT 2

static AcseAuthenticationMechanism
checkAuthMechanismName(uint8_t* authMechanism, int authMechLen)
{
    AcseAuthenticationMechanism authenticationMechanism = ACSE_AUTH_NONE;

    if (authMechanism != NULL) {

        authenticationMechanism = ACSE_AUTH_MECH_UNKNOWN;

        if (authMechLen == 3) {
            if (memcmp(auth_mech_password_oid, authMechanism, 3) == 0) {
                authenticationMechanism = ACSE_AUTH_PASSWORD;
            }
        }
    }

    return authenticationMechanism;
}

static AcseServiceUserDiagnostics
authenticatePeer(AcseConnection* self, AcseAuthenticationMechanism mechanism, uint8_t* authValue, int authValueLen, Socket socket)
{
    struct sAcseAuthenticationParameter authParamStruct;

    AcseAuthenticationParameter authParameter = &authParamStruct;

    authParameter->mechanism = mechanism;

    if (mechanism == ACSE_AUTH_PASSWORD) {
        authParameter->value.password.octetString = authValue;
        authParameter->value.password.passwordLength = authValueLen;
    }
    else if (mechanism == ACSE_AUTH_TLS) {
        authParameter->value.certificate.buf = authValue;
        authParameter->value.certificate.length = authValueLen;
    }

    return self->authenticator(self->authenticatorParameter, authParameter, &(self->securityToken), self->localAppAddr, self->remoteAppAddr, socket);
}

static AcseServiceUserDiagnostics
checkAuthentication(AcseConnection* self, uint8_t* authMechanism, int authMechLen, uint8_t* authValue, int authValueLen, Socket socket)
{
    self->securityToken = NULL;

    if (self->authenticator != NULL) {

        AcseAuthenticationMechanism mechanism = checkAuthMechanismName(authMechanism, authMechLen);

        if (mechanism == ACSE_AUTH_NONE) {

#if (CONFIG_MMS_SUPPORT_TLS == 1)
            if (self->tlsSocket) {

                int certLen;

                uint8_t* certBuf = TLSSocket_getPeerCertificate(self->tlsSocket, &certLen);

                if (certBuf) {
                    mechanism = ACSE_AUTH_TLS;
                    authValue = certBuf;
                    authValueLen = certLen;
                }

            }
#endif /* (CONFIG_MMS_SUPPORT_TLS == 1) */

        }
        else if (mechanism == ACSE_AUTH_MECH_UNKNOWN) {
            return ACSE_USER_AUTH_MECH_NAME_NOT_RECOGNIZED;
        }

        return authenticatePeer(self, mechanism, authValue, authValueLen, socket);
    }
    else
        return ACSE_USER_OK;
}

static int
parseUserInformation(AcseConnection* self, uint8_t* buffer, int bufPos, int maxBufPos, bool* userInfoValid)
{
    bool hasindirectReference = false;
    bool isDataValid = false;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int len;

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

        if (bufPos < 0) {
            *userInfoValid = false;
            return -1;
        }

        switch (tag)
        {

        case 0x02: /* indirect-reference */
            self->nextReference = BerDecoder_decodeUint32(buffer, len, bufPos);
            bufPos += len;
            hasindirectReference = true;
            break;

        case 0xa0: /* encoding */
            isDataValid = true;

            self->userDataBufferSize = len;
            self->userDataBuffer = buffer + bufPos;

            bufPos += len;

            break;

        default: /* ignore unknown tag */
            bufPos += len;
        }
    }

    if (DEBUG_ACSE) {
        if (!hasindirectReference)
            libmms_iso_log(MMS_LOG_WARNING, "ACSE: User data has no indirect reference!");

        if (!isDataValid)
            libmms_iso_log(MMS_LOG_WARNING, "ACSE: No valid user data");
    }

    if (hasindirectReference && isDataValid)
        *userInfoValid = true;
    else
        *userInfoValid = false;

    return bufPos;
}

static AcseIndication
parseAarePdu(AcseConnection* self, uint8_t* buffer, int bufPos, int maxBufPos, Socket socket)
{
    if (DEBUG_ACSE)
        libmms_iso_log(MMS_LOG_DEBUG, "ACSE: parse AARE PDU");

    bool userInfoValid = false;

    /* authentication support */
    uint8_t* authValue = NULL;
    int authValueLen = 0;
    uint8_t* authMechanism = NULL;
    int authMechLen = 0;

    uint32_t result = 99;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int len;

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);
        if (bufPos < 0)
            return ACSE_ERROR;

        switch (tag)
        {
        case 0xa1: /* application context name */
            bufPos += len;
            break;

        case 0xa2: /* result */
            bufPos++;

            bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);
            if (bufPos < 0)
                return ACSE_ERROR;

            result = BerDecoder_decodeUint32(buffer, len, bufPos);

            bufPos += len;
            break;

        case 0xa3: /* result source diagnostic */
            bufPos += len;
            break;

        case 0xa4: /* responding AP-title */
            if (self->remoteAppAddr) {
                if (buffer[bufPos] == 0x06) { /* ap-title-form2 */

                    int innerLength = buffer[bufPos + 1];

                    if (innerLength == len - 2)
                        BerDecoder_decodeOID(buffer, bufPos + 2, innerLength, &(self->remoteAppAddr->apTitle));
                }
            }

            bufPos += len;
            break;

        case 0xa5: /* responding AE-qualifier */
            if (self->remoteAppAddr) {
                if (buffer[bufPos] == 0x02) { /* ae-qualifier-form2 */

                    int innerLength = buffer[bufPos + 1];

                    if (innerLength == len - 2)
                        self->remoteAppAddr->aeQualifier = BerDecoder_decodeInt32(buffer + 2, buffer[bufPos + 1], bufPos);
                }
            }
            bufPos += len;
            break;

        case 0xa9: /* mechanism-name (for authentication functional unit) */
            authMechLen = len;
            authMechanism = buffer + bufPos;
            bufPos += len;
            break;

        case 0xaa: /* authentication value */
            bufPos++;

            bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);
            if (bufPos < 0) {
                if (DEBUG_ACSE)
                    libmms_iso_log(MMS_LOG_ERROR, "ACSE: Invalid AARE PDU!");
                return ACSE_ASSOCIATE_FAILED;
            }

            authValueLen = len;
            authValue = buffer + bufPos;
            bufPos += len;
            break;

        case 0xbe: /* user information */
            if (buffer[bufPos] != 0x28) {
                if (DEBUG_ACSE)
                    libmms_iso_log(MMS_LOG_ERROR, "ACSE: AARE PDU has invalid user info");
                bufPos += len;
            }
            else {
                bufPos++;

                bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);
                if (bufPos < 0)
                    return ACSE_ERROR;

                bufPos = parseUserInformation(self, buffer, bufPos, bufPos + len, &userInfoValid);
                if (bufPos < 0)
                    return ACSE_ERROR;
            }
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* ignore unknown tag */
            if (DEBUG_ACSE)
                libmms_iso_log(MMS_LOG_WARNING, "ACSE: AARE PDU has unknown tag %02x", tag);

            bufPos += len;
            break;
        }
    }

    AcseServiceUserDiagnostics authResult = checkAuthentication(self, authMechanism, authMechLen, authValue, authValueLen, socket);

    if (authResult != ACSE_USER_OK) {
        if (DEBUG_ACSE)
            libmms_iso_log(MMS_LOG_ERROR, "ACSE: AARE - check authentication failed (code: %i)!", authResult);

        return ACSE_ASSOCIATE_FAILED;
    }

    if (!userInfoValid)
        return ACSE_ERROR;

    if (result != ACSE_ASSOCIATE_RESULT_ACCEPTED) {

        if (DEBUG_ACSE)
            libmms_iso_log(MMS_LOG_ERROR, "ACSE: peer rejected connection %s", result == 1 ? "permanent" : "transient");

        return ACSE_ASSOCIATE_FAILED;
    }

    return ACSE_ASSOCIATE;
}

static AcseIndication
parseAbrtPdu(AcseConnection* self, uint8_t* buffer, int bufPos, int maxBufPos, Socket socket)
{
	int source;
	int diagnostic;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];

        int len;
        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

        if (bufPos < 0) {
            if (DEBUG_ACSE)
                libmms_iso_log(MMS_LOG_ERROR, "ACSE: Invalid ABRT PDU!");
            return ACSE_ERROR;
        }

        switch(tag)
        {
        case 0x80: /* abort-source */
            if (len != 1) {
                if (DEBUG_ACSE)
                    libmms_iso_log(MMS_LOG_WARNING, "ACSE: Invalid abort-source in ABORT PDU!");

                return ACSE_ERROR;
            }

            source = buffer[bufPos++];

            if (DEBUG_ACSE) {
                char* sourceStr = "invalid";

                if (source == 0)
                    sourceStr = "service-user";
                else if (source == 1)
                    sourceStr = "service-provider";

                libmms_iso_log(MMS_LOG_INFO, "ACSE: ABORT source %s", sourceStr);
            }

            break;

        case 0x81: /* abort-diagnostic */
            if (len != 1) {
                if (DEBUG_ACSE)
                    libmms_iso_log(MMS_LOG_ERROR, "ACSE: Invalid abort-diagnostic in ABORT PDU!");

                return ACSE_ERROR;
            }

            diagnostic = buffer[bufPos++];

            if (DEBUG_ACSE) {
                char* diagnosticStr = "invalid";

                if (diagnostic == 1)
                    diagnosticStr = "no-reason-given";
                else if (diagnostic == 2)
                    diagnosticStr = "protocol-error";
                else if (diagnostic == 3)
                    diagnosticStr = "authentication-mechanism-name-not-recognized";
                else if (diagnostic == 4)
                    diagnosticStr = "authentication-mechanism-name-required";
                else if (diagnostic == 5)
                    diagnosticStr = "authentication-failure";
                else if (diagnostic == 6)
                    diagnosticStr = "authentication-required";

                libmms_iso_log(MMS_LOG_INFO, "ACSE: ABORT diagnostic %s", diagnosticStr);
            }

            break;

        case 0xb0: /* user information */
            bufPos += len;
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default:
            if (DEBUG_ACSE)
                libmms_iso_log(MMS_LOG_ERROR, "ACSE: ABORT message with unknown tag %02x", tag);

            return ACSE_ERROR;

            break;

        }
    }

    return ACSE_ABORT;
}

static AcseIndication
parseAarqPdu(AcseConnection* self, uint8_t* buffer, int bufPos, int maxBufPos, Socket socket)
{
    if (DEBUG_ACSE)
        libmms_iso_log(MMS_LOG_DEBUG, "ACSE: AARQ PDU");

    uint8_t* authValue = NULL;
    int authValueLen = 0;
    uint8_t* authMechanism = NULL;
    int authMechLen = 0;
    bool userInfoValid = false;

    while (bufPos < maxBufPos) {
        uint8_t tag = buffer[bufPos++];
        int len;

        bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

        if (bufPos < 0) {
            if (DEBUG_ACSE)
                libmms_iso_log(MMS_LOG_ERROR, "ACSE: Invalid AARQ PDU!");
            return ACSE_ASSOCIATE_FAILED;
        }

        switch (tag)
        {
        case 0xa1: /* application context name */
            bufPos += len;
            break;

        case 0xa2: /* called AP title */
            if (self->localAppAddr) {
                if (buffer[bufPos] == 0x06) { /* ap-title-form2 */

                    int innerLength = buffer[bufPos + 1];

                    if (innerLength == len - 2)
                        BerDecoder_decodeOID(buffer, bufPos + 2, innerLength, &(self->localAppAddr->apTitle));
                }
            }
            bufPos += len;
            break;

        case 0xa3: /* called AE qualifier */
            if (self->localAppAddr) {
                if (buffer[bufPos] == 0x02) { /* ae-qualifier-form2 */

                    int innerLength = buffer[bufPos + 1];

                    if (innerLength == len - 2)
                        self->localAppAddr->aeQualifier = BerDecoder_decodeInt32(buffer + 2, buffer[bufPos + 1], bufPos);
                }
            }
            bufPos += len;
            break;

        case 0xa6: /* calling AP title */
            if (self->remoteAppAddr) {
                if (buffer[bufPos] == 0x06) { /* ap-title-form2 */

                    int innerLength = buffer[bufPos + 1];

                    if (innerLength == len - 2)
                        BerDecoder_decodeOID(buffer, bufPos + 2, innerLength, &(self->remoteAppAddr->apTitle));
                }
            }
            bufPos += len;
            break;

        case 0xa7: /* calling AE qualifier */
            if (self->remoteAppAddr) {
                if (buffer[bufPos] == 0x02) { /* ae-qualifier-form2 */

                    int innerLength = buffer[bufPos + 1];

                    if (innerLength == len - 2)
                        self->remoteAppAddr->aeQualifier = BerDecoder_decodeInt32(buffer + 2, buffer[bufPos + 1], bufPos);
                }
            }
            bufPos += len;
            break;

        case 0x8a: /* sender ACSE requirements */
            bufPos += len;
            break;

        case 0x8b: /* (authentication) mechanism name */
            authMechLen = len;
            authMechanism = buffer + bufPos;
            bufPos += len;
            break;

        case 0xac: /* authentication value */
            bufPos++;

            bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);
            if (bufPos < 0) {
                if (DEBUG_ACSE)
                    libmms_iso_log(MMS_LOG_ERROR, "ACSE: Invalid AARQ PDU!");
                return ACSE_ASSOCIATE_FAILED;
            }

            authValueLen = len;
            authValue = buffer + bufPos;
            bufPos += len;
            break;

        case 0xbe: /* user information */
            if (buffer[bufPos] != 0x28) {
                if (DEBUG_ACSE)
                    libmms_iso_log(MMS_LOG_WARNING, "ACSE: invalid user info in AARQ");
                bufPos += len;
            }
            else {
                bufPos++;

                bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, maxBufPos);

                if (bufPos < 0) {
                    if (DEBUG_ACSE)
                        libmms_iso_log(MMS_LOG_ERROR, "ACSE: Invalid AARQ PDU!");
                    return ACSE_ASSOCIATE_FAILED;
                }

                bufPos = parseUserInformation(self, buffer, bufPos, bufPos + len, &userInfoValid);

                if (bufPos < 0) {
                    if (DEBUG_ACSE)
                        libmms_iso_log(MMS_LOG_ERROR, "ACSE: Invalid AARQ PDU!");
                    return ACSE_ASSOCIATE_FAILED;
                }
            }
            break;

        case 0x00: /* indefinite length end tag -> ignore */
            break;

        default: /* ignore unknown tag */
            if (DEBUG_ACSE)
                libmms_iso_log(MMS_LOG_ERROR, "ACSE: unknown tag %02x in AARQ PDU", tag);

            bufPos += len;
            break;
        }
    }

    AcseServiceUserDiagnostics authResult = checkAuthentication(self, authMechanism, authMechLen, authValue, authValueLen, socket);

    if (authResult != ACSE_USER_OK) {
        if (DEBUG_ACSE)
            libmms_iso_log(MMS_LOG_ERROR, "ACSE: AARQ - check authentication failed (code: %i)!", authResult);

        return ACSE_ASSOCIATE_FAILED;
    }

    if (userInfoValid == false) {
        if (DEBUG_ACSE)
            libmms_iso_log(MMS_LOG_ERROR, "ACSE: AARQ - user info invalid!");

        return ACSE_ASSOCIATE_FAILED;
    }

    return ACSE_ASSOCIATE;
}

void
AcseConnection_init(AcseConnection* self, IsoApplicationAddress remoteAppAddr, IsoApplicationAddress localAppAddr, AcseAuthenticator authenticator, void* parameter, TLSSocket tlsSocket)
{
    self->state = idle;
    self->nextReference = 0;
    self->userDataBuffer = NULL;
    self->userDataBufferSize = 0;
    self->authenticator = authenticator;
    self->authenticatorParameter = parameter;

#if (CONFIG_MMS_SUPPORT_TLS == 1)
    self->tlsSocket = tlsSocket;
#endif

    self->remoteAppAddr = remoteAppAddr;
    self->localAppAddr = localAppAddr;
}

void
AcseConnection_destroy(AcseConnection* connection)
{
}

AcseIndication
AcseConnection_parseMessage(AcseConnection* self, ByteBuffer* message, Socket socket)
{
    AcseIndication indication;

    uint8_t* buffer = message->buffer;

    int messageSize = message->size;

    int bufPos = 0;

    uint8_t messageType = buffer[bufPos++];

    int len;

    bufPos = BerDecoder_decodeLength(buffer, &len, bufPos, messageSize);

    if (bufPos < 0)
    {
        if (DEBUG_ACSE)
            libmms_iso_log(MMS_LOG_ERROR, "ACSE: invalid ACSE PDU!");

        return ACSE_ERROR;
    }

    switch (messageType)
    {
    case 0x60:
        indication = parseAarqPdu(self, buffer, bufPos, messageSize, socket);
        break;
    case 0x61:
        indication = parseAarePdu(self, buffer, bufPos, messageSize, socket);
        break;
    case 0x62: /* A_RELEASE.request RLRQ-apdu */
        indication = ACSE_RELEASE_REQUEST;
        break;
    case 0x63: /* A_RELEASE.response RLRE-apdu */
        indication = ACSE_RELEASE_RESPONSE;
        break;
    case 0x64: /* A_ABORT */
        indication = parseAbrtPdu(self, buffer, bufPos, messageSize, socket);
        break;
    case 0x00: /* indefinite length end tag -> ignore */
        break;
    default:
        if (DEBUG_ACSE)
            libmms_iso_log(MMS_LOG_ERROR, "ACSE: Unknown ACSE PDU");
        indication = ACSE_ERROR;
        break;
    }

    return indication;
}

void
AcseConnection_createAssociateFailedMessage(AcseConnection* self, BufferChain writeBuffer)
{
    AcseConnection_createAssociateResponseMessage(self, NULL, ACSE_RESULT_REJECT_PERMANENT, writeBuffer, NULL);
}

void
AcseConnection_createAssociateResponseMessage(AcseConnection* self,
        IsoApplicationAddress localAppAddr,
        uint8_t acseResult,
        BufferChain writeBuffer,
        BufferChain payload
        )
{
    assert(self != NULL);
    assert(writeBuffer != NULL);
    assert(payload != NULL);

    int appContextLength = 9;
    int resultLength = 5;
    int resultDiagnosticLength = 5;

    int headerContentLen = appContextLength + resultLength
            + resultDiagnosticLength;

    int userInfoContentLen = 0;

    int assocDataLength;
    int userInfoLength;
    int nextRefLength;

    int payloadLength = payload->length;


    int localAEQualifierLength = 0;
    int localAEInvokeIdLength = 0;
    int localAPInvokeIdLength = 0;

    uint8_t localApTitle[10];
    int localApTitleLen = 0;

    if (localAppAddr) {
        localApTitleLen = BerEncoder_encodeItuObjectIdentifierToBuffer(&(localAppAddr->apTitle), localApTitle, 10);

        if (localApTitleLen > 0)
        {
            /* responding AP title */
            headerContentLen += (4 + localApTitleLen);
        }

        if (localAppAddr->hasAeQualifier) {
            /* responding AE qualifier */

            localAEQualifierLength = BerEncoder_Int32determineEncodedSize(localAppAddr->aeQualifier);

            headerContentLen += (4 + localAEQualifierLength);
        }

        if (localAppAddr->hasAeInvocationId) {
            /* responding AE Invocation Identifier */
            localAEInvokeIdLength = BerEncoder_Int32determineEncodedSize(localAppAddr->aeInvocationId);

            headerContentLen += (4 + localAEInvokeIdLength);
        }

        if (localAppAddr->hasApInvocationId) {
            /* responding AP Invocation Identifier */
            localAPInvokeIdLength = BerEncoder_Int32determineEncodedSize(localAppAddr->apInvocationId);

            headerContentLen += (4 + localAPInvokeIdLength);
        }

    }

    /* single ASN1 type tag */
    userInfoContentLen += payloadLength;
    userInfoContentLen += 1;
    userInfoContentLen += BerEncoder_determineLengthSize(payloadLength);

    /* indirect reference */
    nextRefLength = BerEncoder_UInt32determineEncodedSize(self->nextReference);
    userInfoContentLen += nextRefLength;
    userInfoContentLen += 2;

    /* association data */
    assocDataLength = userInfoContentLen;
    userInfoContentLen += BerEncoder_determineLengthSize(assocDataLength);
    userInfoContentLen += 1;

    /* user information */
    userInfoLength = userInfoContentLen;
    userInfoContentLen += BerEncoder_determineLengthSize(userInfoLength);
    userInfoContentLen += 1;

    userInfoContentLen += 2;

    int contentLength = headerContentLen + userInfoContentLen;

    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0x61, contentLength, buffer, bufPos);

    /* application context name */
    bufPos = BerEncoder_encodeTL(0xa1, 7, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x06, 5, buffer, bufPos);
    memcpy(buffer + bufPos, appContextNameMms, 5);
    bufPos += 5;

    /* result */
    bufPos = BerEncoder_encodeTL(0xa2, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
    buffer[bufPos++] = acseResult;

    /* result source diagnostics */
    bufPos = BerEncoder_encodeTL(0xa3, 5, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0xa1, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
    buffer[bufPos++] = 0;

    if (localAppAddr) {

        if (localApTitleLen > 0)
        {
            /* responding AP title */
            bufPos = BerEncoder_encodeTL(0xa4, localApTitleLen + 2, buffer, bufPos);
            bufPos = BerEncoder_encodeTL(0x06, localApTitleLen, buffer, bufPos);

            memcpy(buffer + bufPos, localApTitle, localApTitleLen);
            bufPos += localApTitleLen;
        }

        if (localAppAddr->hasAeQualifier) {
            /* responding AE qualifier */
            bufPos = BerEncoder_encodeTL(0xa5, localAEQualifierLength + 2, buffer,
                    bufPos);
            bufPos = BerEncoder_encodeTL(0x02, localAEQualifierLength, buffer,
                    bufPos);
            bufPos = BerEncoder_encodeInt32(localAppAddr->aeQualifier,
                    buffer, bufPos);
        }

        if (localAppAddr->hasApInvocationId)
        {
            /* responding AP invocation identifier */
            bufPos = BerEncoder_encodeTL(0xa6, localAPInvokeIdLength + 2, buffer,
                    bufPos);
            bufPos = BerEncoder_encodeTL(0x02, localAPInvokeIdLength, buffer,
                    bufPos);
            bufPos = BerEncoder_encodeInt32(localAppAddr->apInvocationId,
                    buffer, bufPos);
        }

        if (localAppAddr->hasAeInvocationId)
        {
            /* responding AE invocation identifier */
            bufPos = BerEncoder_encodeTL(0xa7, localAEInvokeIdLength + 2, buffer,
                    bufPos);
            bufPos = BerEncoder_encodeTL(0x02, localAEInvokeIdLength, buffer,
                    bufPos);
            bufPos = BerEncoder_encodeInt32(localAppAddr->aeInvocationId,
                    buffer, bufPos);
        }
    }

    /* user information */
    bufPos = BerEncoder_encodeTL(0xbe, userInfoLength, buffer, bufPos);

    /* association data */
    bufPos = BerEncoder_encodeTL(0x28, assocDataLength, buffer, bufPos);

    /* indirect-reference */
    bufPos = BerEncoder_encodeTL(0x02, nextRefLength, buffer, bufPos);
    bufPos = BerEncoder_encodeUInt32(self->nextReference, buffer, bufPos);

    /* single ASN1 type */
    bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payloadLength;
    writeBuffer->nextPart = payload;
}

void
AcseConnection_createAssociateRequestMessage(AcseConnection* self,
        IsoApplicationAddress remoteAddr,
        IsoApplicationAddress localAddr,
        BufferChain writeBuffer,
        BufferChain payload,
        AcseAuthenticationParameter authParameter)
{
    assert(self != NULL);
    assert(writeBuffer != NULL);
    assert(payload != NULL);

    int payloadLength = payload->length;
    int authValueLength;
    int authValueStringLength = 0;

    int passwordLength = 0;

    int contentLength = 0;

    /* application context name */
    contentLength += 9;

    int calledAEQualifierLength = 0;
    int calledAEInvokeIdLength = 0;
    int calledAPInvokeIdLength = 0;

    int callingAEQualifierLength = 0;
    int callingAEInvokeIdLength = 0;
    int callingAPInvokeIdLength = 0;

    uint8_t remoteApTitle[10];
    int remoteApTitleLen;

    remoteApTitleLen = BerEncoder_encodeItuObjectIdentifierToBuffer(&(remoteAddr->apTitle), remoteApTitle, 10);

    uint8_t localApTitle[10];
    int localApTitleLen;

    localApTitleLen = BerEncoder_encodeItuObjectIdentifierToBuffer(&(localAddr->apTitle), localApTitle, 10);


    if (remoteApTitleLen > 0)
    {
        /* called AP title */
        contentLength += (4 + remoteApTitleLen);
    }

    if (remoteAddr->hasAeQualifier) {
        /* called AE qualifier */
        calledAEQualifierLength = BerEncoder_Int32determineEncodedSize(remoteAddr->aeQualifier);

        contentLength += (4 + calledAEQualifierLength);
    }

    if (remoteAddr->hasAeInvocationId) {
        /* called AE Invocation Identifier */
        calledAEInvokeIdLength = BerEncoder_Int32determineEncodedSize(remoteAddr->aeInvocationId);

        contentLength += (4 + calledAEInvokeIdLength);
    }

    if (remoteAddr->hasApInvocationId) {
        /* called AP Invocation Identifier */
        calledAPInvokeIdLength = BerEncoder_Int32determineEncodedSize(remoteAddr->apInvocationId);

        contentLength += (4 + calledAPInvokeIdLength);
    }

    if (localApTitleLen > 0)
    {
        /* calling AP title */
        contentLength += (4 + localApTitleLen);
    }

    if (localAddr->hasAeQualifier) {
        /* calling AE qualifier */

        callingAEQualifierLength = BerEncoder_Int32determineEncodedSize(localAddr->aeQualifier);

        contentLength += (4 + callingAEQualifierLength);
    }

    if (localAddr->hasAeInvocationId) {
        /* calling AE Invocation Identifier */
        callingAEInvokeIdLength = BerEncoder_Int32determineEncodedSize(localAddr->aeInvocationId);

        contentLength += (4 + callingAEInvokeIdLength);
    }

    if (localAddr->hasApInvocationId) {
        /* calling AP Invocation Identifier */
        callingAPInvokeIdLength = BerEncoder_Int32determineEncodedSize(localAddr->apInvocationId);

        contentLength += (4 + callingAPInvokeIdLength);
    }

    if (authParameter != NULL)
    {
        /* sender ACSE requirements */
        contentLength += 4;

        /* mechanism name */
        contentLength += 5;

        /* authentication value */
        if (authParameter->mechanism == ACSE_AUTH_PASSWORD)
        {
            contentLength += 2;

            passwordLength = authParameter->value.password.passwordLength;

            authValueStringLength = BerEncoder_determineLengthSize(
                    passwordLength);

            contentLength += passwordLength + authValueStringLength;

            authValueLength = BerEncoder_determineLengthSize(
                    passwordLength + authValueStringLength + 1);

            contentLength += authValueLength;
        }
        else
        {
            contentLength += 2;
        }
    }

    /* user information */
    int userInfoLength = 0;

    /* single ASN1 type tag */
    userInfoLength += payloadLength;
    userInfoLength += 1;
    userInfoLength += BerEncoder_determineLengthSize(payloadLength);

    /* indirect reference */
    userInfoLength += 1;
    userInfoLength += 2;

    /* association data */
    int assocDataLength = userInfoLength;
    userInfoLength += BerEncoder_determineLengthSize(assocDataLength);
    userInfoLength += 1;

    /* user information */
    int userInfoLen = userInfoLength;
    userInfoLength += BerEncoder_determineLengthSize(userInfoLength);
    userInfoLength += 1;

    contentLength += userInfoLength;

    uint8_t* buffer = writeBuffer->buffer;
    int bufPos = 0;

    bufPos = BerEncoder_encodeTL(0x60, contentLength, buffer, bufPos);

    /* application context name */
    bufPos = BerEncoder_encodeTL(0xa1, 7, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x06, 5, buffer, bufPos);
    memcpy(buffer + bufPos, appContextNameMms, 5);
    bufPos += 5;

    if (remoteApTitleLen > 0)
    {
        /* called AP title */
        bufPos = BerEncoder_encodeTL(0xa2, remoteApTitleLen + 2, buffer, bufPos);
        bufPos = BerEncoder_encodeTL(0x06, remoteApTitleLen, buffer, bufPos);

        memcpy(buffer + bufPos, remoteApTitle, remoteApTitleLen);
        bufPos += remoteApTitleLen;
    }

    if (remoteAddr->hasAeQualifier)
    {
        /* called AE qualifier */
        bufPos = BerEncoder_encodeTL(0xa3, calledAEQualifierLength + 2, buffer,
                bufPos);
        bufPos = BerEncoder_encodeTL(0x02, calledAEQualifierLength, buffer,
                bufPos);
        bufPos = BerEncoder_encodeInt32(remoteAddr->aeQualifier,
                buffer, bufPos);
    }

    if (remoteAddr->hasApInvocationId)
    {
        /* called AP invocation identifier */
        bufPos = BerEncoder_encodeTL(0xa4, calledAPInvokeIdLength + 2, buffer,
                bufPos);
        bufPos = BerEncoder_encodeTL(0x02, calledAPInvokeIdLength, buffer,
                bufPos);
        bufPos = BerEncoder_encodeInt32(remoteAddr->apInvocationId,
                buffer, bufPos);
    }

    if (remoteAddr->hasAeInvocationId)
    {
        /* called AE invocation identifier */
        bufPos = BerEncoder_encodeTL(0xa5, calledAEInvokeIdLength + 2, buffer,
                bufPos);
        bufPos = BerEncoder_encodeTL(0x02, calledAEInvokeIdLength, buffer,
                bufPos);
        bufPos = BerEncoder_encodeInt32(remoteAddr->aeInvocationId,
                buffer, bufPos);
    }

    if (localApTitleLen > 0)
    {
        /* calling AP title */
        bufPos = BerEncoder_encodeTL(0xa6, localApTitleLen + 2, buffer, bufPos);
        bufPos = BerEncoder_encodeTL(0x06, localApTitleLen, buffer, bufPos);

        memcpy(buffer + bufPos, localApTitle, localApTitleLen);
        bufPos += localApTitleLen;
    }

    if (localAddr->hasAeQualifier) {
        /* calling AE qualifier */
        bufPos = BerEncoder_encodeTL(0xa7, callingAEQualifierLength + 2, buffer,
                bufPos);
        bufPos = BerEncoder_encodeTL(0x02, callingAEQualifierLength, buffer,
                bufPos);
        bufPos = BerEncoder_encodeInt32(localAddr->aeQualifier,
                buffer, bufPos);
    }

    if (localAddr->hasApInvocationId)
    {
        /* calling AP invocation identifier */
        bufPos = BerEncoder_encodeTL(0xa8, callingAPInvokeIdLength + 2, buffer,
                bufPos);
        bufPos = BerEncoder_encodeTL(0x02, callingAPInvokeIdLength, buffer,
                bufPos);
        bufPos = BerEncoder_encodeInt32(localAddr->apInvocationId,
                buffer, bufPos);
    }

    if (localAddr->hasAeInvocationId)
    {
        /* calling AE invocation identifier */
        bufPos = BerEncoder_encodeTL(0xa9, callingAEInvokeIdLength + 2, buffer,
                bufPos);
        bufPos = BerEncoder_encodeTL(0x02, callingAEInvokeIdLength, buffer,
                bufPos);
        bufPos = BerEncoder_encodeInt32(localAddr->aeInvocationId,
                buffer, bufPos);
    }

    if (authParameter != NULL)
    {
        /* sender requirements */
        bufPos = BerEncoder_encodeTL(0x8a, 2, buffer, bufPos);
        buffer[bufPos++] = 0x04;

        if (authParameter->mechanism == ACSE_AUTH_PASSWORD)
        {
            buffer[bufPos++] = requirements_authentication[0];

            bufPos = BerEncoder_encodeTL(0x8b, 3, buffer, bufPos);
            memcpy(buffer + bufPos, auth_mech_password_oid, 3);
            bufPos += 3;

            /* authentication value */
            bufPos = BerEncoder_encodeTL(0xac,
                    authValueStringLength + passwordLength + 1, buffer, bufPos);
            bufPos = BerEncoder_encodeTL(0x80, passwordLength, buffer, bufPos);
            memcpy(buffer + bufPos, authParameter->value.password.octetString,
                    passwordLength);
            bufPos += passwordLength;
        }
        else
        { /* AUTH_NONE */
            buffer[bufPos++] = 0;
        }
    }

    /* user information */
    bufPos = BerEncoder_encodeTL(0xbe, userInfoLen, buffer, bufPos);

    /* association data */
    bufPos = BerEncoder_encodeTL(0x28, assocDataLength, buffer, bufPos);

    /* indirect-reference */
    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos);
    buffer[bufPos++] = 3;

    /* single ASN1 type */
    bufPos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, bufPos);

    writeBuffer->partLength = bufPos;
    writeBuffer->length = bufPos + payload->length;
    writeBuffer->nextPart = payload;
}

/**
 * \param isProvider specifies abort source (false = user/client; true = provider/server)
 */
void
AcseConnection_createAbortMessage(AcseConnection* self, BufferChain writeBuffer, bool isProvider)
{
    uint8_t* buffer = writeBuffer->buffer;

    buffer[0] = 0x64; /* [APPLICATION 4] */
    buffer[1] = 3;
    buffer[2] = 0x80;
    buffer[3] = 1;

    if (isProvider)
        buffer[4] = 1;
    else
        buffer[4] = 0;

    writeBuffer->partLength = 5;
    writeBuffer->length = 5;
    writeBuffer->nextPart = NULL;
}

void
AcseConnection_createReleaseRequestMessage(AcseConnection* self, BufferChain writeBuffer)
{
    uint8_t* buffer = writeBuffer->buffer;

    buffer[0] = 0x62;
    buffer[1] = 3;
    buffer[2] = 0x80;
    buffer[3] = 1;
    buffer[4] = 0;

    writeBuffer->partLength = 5;
    writeBuffer->length = 5;
    writeBuffer->nextPart = NULL;
}

void
AcseConnection_createReleaseResponseMessage(AcseConnection* self, BufferChain writeBuffer)
{
    uint8_t* buffer = writeBuffer->buffer;

    buffer[0] = 0x63;
    buffer[1] = 0;

    writeBuffer->partLength = 2;
    writeBuffer->length = 2;
    writeBuffer->nextPart = NULL;
}

