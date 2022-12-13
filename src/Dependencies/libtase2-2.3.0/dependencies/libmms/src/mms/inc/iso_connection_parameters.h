/*
 *  iso_connection_parameters.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef ISO_CONNECTION_PARAMETERS_H_
#define ISO_CONNECTION_PARAMETERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tls_config.h"
#include "hal_socket.h"
#include "libmms_common_api.h"
#include <stdint.h>

/**
 * \addtogroup mms_client_api_group
 */
/**@{*/

/**
 * \brief ITU (International Telecommunication Union) object identifier (OID)
 */
typedef struct {
    uint16_t arc[10];
    int arcCount;
} ItuObjectIdentifier;

LIBMMS_API void
ItuObjectIdentifier_setFromString(ItuObjectIdentifier* self, const char* str);

/**
 * \brief Convert OID to string
 *
 * \return the OID as dot separated string (has to be free'd by the caller)
 */
LIBMMS_API char*
ItuObjectIdentifier_toString(ItuObjectIdentifier* self);

/**
 * \brief ISO application reference (specifies an ISO application endpoint)
 */
typedef struct {
    ItuObjectIdentifier apTitle;
    int aeQualifier;
} IsoApplicationReference;

/**
 * \brief authentication mechanism used by AcseAuthenticator
 */
typedef enum
{
    /** Neither ACSE nor TLS authentication used */
    ACSE_AUTH_NONE = 0,

    /** Use ACSE password for peer authentication */
    ACSE_AUTH_PASSWORD = 1,

    /** Use ACSE certificate for peer authentication */
    ACSE_AUTH_CERTIFICATE = 2,

    /** Use TLS certificate for peer authentication */
    ACSE_AUTH_TLS = 3,

    /** Unknown mechanism for peer authentication */
    ACSE_AUTH_MECH_UNKNOWN = 99
} AcseAuthenticationMechanism;

/**
 * \brief Diagnostic information for ACSE service user (authentication) result
 */
typedef enum {
    ACSE_USER_OK = 0,
    ACSE_USER_NO_REASON_GIVEN = 1,
    ACSE_USER_APP_CONTEXT_NAME_NOT_SUPPORTED = 2,
    ACSE_USER_CALLING_AP_TITLE_NOT_RECOGNIZED = 3,
    ACSE_USER_CALLING_AP_INVOCATION_ID_NOT_RECOGNIZED = 4,
    ACSE_USER_CALLING_AE_QUALIFIER_NOT_RECOGNIZED = 5,
    ACSE_USER_CALLING_AE_INVOCATION_ID_NOT_RECOGNIZED = 6,
    ACSE_USER_CALLED_AP_TITLE_NOT_RECOGNIZED = 7,
    ACSE_USER_CALLED_AP_INVOCATION_ID_NOT_RECOGNIZED = 8,
    ACSE_USER_CALLED_AE_QUALIFIER_NOT_RECOGNIZED = 9,
    ACSE_USER_CALLED_AE_INVOCATION_ID_NOT_RECOGNIZED = 10,
    ACSE_USER_AUTH_MECH_NAME_NOT_RECOGNIZED = 11,
    ACSE_USER_AUTH_MECH_NAME_REQUIRED = 12,
    ACSE_USER_AUTH_FAILURE = 13,
    ACSE_USER_AUTH_REQUIRED = 14,
} AcseServiceUserDiagnostics;

typedef struct sAcseAuthenticationParameter* AcseAuthenticationParameter;

struct sAcseAuthenticationParameter
{
    AcseAuthenticationMechanism mechanism;

    union
    {
        struct
        {
            uint8_t* octetString;
            int passwordLength;
        } password; /* for mechanism = ACSE_AUTH_PASSWORD */

        struct
        {
            uint8_t* buf;
            int length;
        } certificate; /* for mechanism = ACSE_AUTH_CERTIFICATE or ACSE_AUTH_TLS */

    } value;
};

AcseAuthenticationParameter
AcseAuthenticationParameter_create(void);

void
AcseAuthenticationParameter_destroy(AcseAuthenticationParameter self);

void
AcseAuthenticationParameter_setAuthMechanism(AcseAuthenticationParameter self, AcseAuthenticationMechanism mechanism);

void
AcseAuthenticationParameter_setPassword(AcseAuthenticationParameter self, char* password);


/**
 * \brief COTP T selector
 *
 * To not use T SEL set size to 0.
 */
typedef struct {
    uint8_t size; /** 0 .. 16 - 0 means T-selector is not present */
    uint8_t value[16]; /** T-selector value */
} TSelector;

typedef struct {
    uint8_t size; /** 0 .. 16 - 0 means S-selector is not present */
    uint8_t value[16]; /** S-selector value */
} SSelector;

typedef struct {
    uint8_t size; /** 0 .. 16 - 0 means P-selector is not present */
    uint8_t value[16]; /** P-selector value */
} PSelector;

typedef struct sIsoApplicationAddress* IsoApplicationAddress;

struct sIsoApplicationAddress
{
    unsigned int hasAeQualifier:1;
    unsigned int hasAeInvocationId:1;
    unsigned int hasApInvocationId:1;

    ItuObjectIdentifier apTitle;
    int aeQualifier;
    int aeInvocationId;
    int apInvocationId;
    PSelector pSelector;
    SSelector sSelector;
    TSelector tSelector;
};

/**
 * \brief Callback function to authenticate a peer
 *
 * \param parameter user provided parameter - set when user registers the authenticator
 * \param authParameter the authentication parameters provided by the client
 * \param securityToken pointer where to store an application specific security token - can be ignored if not used.
 * \param remoteAppAddr ISO application address of the peer (ap-title + ae-qualifier, ...)
 *
 * \return ACSE_USER_OK if client connection is accepted, error code otherwise
 */
typedef AcseServiceUserDiagnostics
(*AcseAuthenticator)(void* parameter, AcseAuthenticationParameter authParameter, void** securityToken, IsoApplicationAddress localAppAddr, IsoApplicationAddress remoteAppAddr, Socket socket);

/**
 * \brief set the AP-Title
 *
 * If apTitle is NULL the parameter the AP-Title and AE-Qualifier will not be transmitted.
 * This seems to be required by some server devices.
 *
 * \param self the IsoConnectionParameters instance
 * \param apTitle the AP-Title OID as string.
 */
LIBMMS_API void
IsoApplicationAddress_setApTitle(IsoApplicationAddress self, const char* apTitle);

/**
 * \brief set the AE-Qualifier
 *
 * \param aeQualifier the AP-qualifier
 */
LIBMMS_API void
IsoApplicationAddress_setAeQualifier(IsoApplicationAddress self, int aeQualifier);

/**
 * \brief set the AE-invocation-identifier
 *
 * \param aeInvocationId the AE-invocation-identifier
 */
LIBMMS_API void
IsoApplicationAddress_setAeInvocationId(IsoApplicationAddress self, int aeInvocationId);

/**
 * \brief set the AP-invocation-identifier
 *
 * \param aeInvocationId the AP-invocation-identifier
 */
LIBMMS_API void
IsoApplicationAddress_setApInvocationId(IsoApplicationAddress self, int apInvocationId);

/**
 * \brief set addresses for the lower layers
 *
 * This function can be used to set the addresses for the lower layer protocols (presentation, session, and transport
 * layer). Calling this function is optional and not recommended. If not called the default
 * parameters are used.
 *
 *  \param self the IsoConnectionParameters instance
 *  \param pSelector the P-Selector (presentation layer address)
 *  \param sSelector the S-Selector (session layer address)
 *  \param tSelector the T-Selector (ISO transport layer address)
 */
LIBMMS_API void
IsoApplicationAddress_setLowerLayerAddresses(IsoApplicationAddress self, PSelector pSelector, SSelector sSelector, TSelector tSelector);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ISO_CONNECTION_PARAMETERS_H_ */
