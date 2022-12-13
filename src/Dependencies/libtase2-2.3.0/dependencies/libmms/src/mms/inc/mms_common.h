/*
 *  mms_common.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_common_api.h"

#ifndef MMS_COMMON_H_
#define MMS_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup common_api_group
 */
/**@{*/


#define MMS_STAT_ID_APL_MESSAGES_SENT 0
#define MMS_STAT_ID_APL_MESSAGES_RCVD 1
#define MMS_STAT_ID_CONF_ERROR_RCVD   2
#define MMS_STAT_ID_CONF_ERROR_SENT   3
#define MMS_STAT_ID_REJECT_RCVD       4
#define MMS_STAT_ID_REJECT_SENT       6
#define MMS_STAT_ID_REQUEST_TIMEOUTS  7

/**
 * \brief Specify the log level for the library internal logging
 */
typedef enum {
    /**
     * log level DEBUG - shows the most information (useful for debugging applications)
     */
    MMS_LOG_DEBUG = 1,

    /**
     * log level INFO - show informational messages (useful to trace communication problems)
     */
    MMS_LOG_INFO = 2,

    /**
     * log level WARNING - show only errors and warning message that indicate wrong configuration
     */
    MMS_LOG_WARNING = 3,

    /**
     * log level ERROR - show critical problems and communication errors
     */
    MMS_LOG_ERROR = 4,

    /**
     * log level NONE - don't show any log messages
     */
    MMS_LOG_NONE = 5
} MmsLogLevel;

typedef enum
{
    /* generic error codes */
    MMS_ERROR_NONE = 0,
    MMS_ERROR_CONNECTION_REJECTED = 1,
    MMS_ERROR_CONNECTION_LOST = 2,
    MMS_ERROR_SERVICE_TIMEOUT = 3,
    MMS_ERROR_PARSING_RESPONSE = 4,
    MMS_ERROR_HARDWARE_FAULT = 5,
    MMS_ERROR_CONCLUDE_REJECTED = 6,
    MMS_ERROR_INVALID_ARGUMENTS = 7,
    MMS_ERROR_OUTSTANDING_CALL_LIMIT = 8,

    MMS_ERROR_OTHER = 9,

    /* confirmed error PDU codes */
    MMS_ERROR_VMDSTATE_OTHER = 10,

    MMS_ERROR_APPLICATION_REFERENCE_OTHER = 20,

    MMS_ERROR_DEFINITION_OTHER = 30,
    MMS_ERROR_DEFINITION_INVALID_ADDRESS = 31,
    MMS_ERROR_DEFINITION_TYPE_UNSUPPORTED = 32,
    MMS_ERROR_DEFINITION_TYPE_INCONSISTENT = 33,
    MMS_ERROR_DEFINITION_OBJECT_UNDEFINED = 34,
    MMS_ERROR_DEFINITION_OBJECT_EXISTS = 35,
    MMS_ERROR_DEFINITION_OBJECT_ATTRIBUTE_INCONSISTENT = 36,

    MMS_ERROR_RESOURCE_OTHER = 40,
    MMS_ERROR_RESOURCE_CAPABILITY_UNAVAILABLE = 41,

    MMS_ERROR_SERVICE_OTHER = 50,
    MMS_ERROR_SERVICE_OBJECT_CONSTRAINT_CONFLICT = 55,

    MMS_ERROR_SERVICE_PREEMPT_OTHER = 60,

    MMS_ERROR_TIME_RESOLUTION_OTHER = 70,

    MMS_ERROR_ACCESS_OTHER = 80,
    MMS_ERROR_ACCESS_OBJECT_NON_EXISTENT = 81,
    MMS_ERROR_ACCESS_OBJECT_ACCESS_UNSUPPORTED = 82,
    MMS_ERROR_ACCESS_OBJECT_ACCESS_DENIED = 83,
    MMS_ERROR_ACCESS_OBJECT_INVALIDATED = 84,
    MMS_ERROR_ACCESS_OBJECT_VALUE_INVALID = 85, /* for DataAccessError 11 */
    MMS_ERROR_ACCESS_TEMPORARILY_UNAVAILABLE = 86, /* for DataAccessError 2 */

    MMS_ERROR_FILE_OTHER = 90,
    MMS_ERROR_FILE_FILENAME_AMBIGUOUS = 91,
    MMS_ERROR_FILE_FILE_BUSY = 92,
    MMS_ERROR_FILE_FILENAME_SYNTAX_ERROR = 93,
    MMS_ERROR_FILE_CONTENT_TYPE_INVALID = 94,
    MMS_ERROR_FILE_POSITION_INVALID = 95,
    MMS_ERROR_FILE_FILE_ACCESS_DENIED = 96,
    MMS_ERROR_FILE_FILE_NON_EXISTENT = 97,
    MMS_ERROR_FILE_DUPLICATE_FILENAME = 98,
    MMS_ERROR_FILE_INSUFFICIENT_SPACE_IN_FILESTORE = 99,

    /* reject codes */
    MMS_ERROR_REJECT_OTHER = 100,
    MMS_ERROR_REJECT_UNKNOWN_PDU_TYPE = 101,
    MMS_ERROR_REJECT_INVALID_PDU = 102,
    MMS_ERROR_REJECT_UNRECOGNIZED_SERVICE = 103,
    MMS_ERROR_REJECT_UNRECOGNIZED_MODIFIER = 104,
    MMS_ERROR_REJECT_REQUEST_INVALID_ARGUMENT = 105,
    MMS_ERROR_REJECT_MAX_OUTSTANDING_SERVICES_EXCEEDED = 106

} MmsError;

typedef enum
{
    /*! this represents all MMS array types (arrays contain uniform elements) */
    MMS_ARRAY = 0,
    /*! this represents all complex MMS types (structures) */
    MMS_STRUCTURE = 1,
    /*! boolean value */
    MMS_BOOLEAN = 2,
    /*! bit string */
    MMS_BIT_STRING = 3,
    /*! represents all signed integer types */
    MMS_INTEGER = 4,
    /*! represents all unsigned integer types */
    MMS_UNSIGNED = 5,
    /*! represents all float type (32 and 64 bit) */
    MMS_FLOAT = 6,
    /*! octet string (unstructured bytes) */
    MMS_OCTET_STRING = 7,
    /*! MMS visible string */
    MMS_VISIBLE_STRING = 8,
    MMS_GENERALIZED_TIME = 9,
    MMS_BINARY_TIME = 10,
    MMS_BCD = 11,
    MMS_OBJ_ID = 12,
    /*! MMS unicode string */
    MMS_STRING = 13,
    /*! MMS UTC time type */
    MMS_UTC_TIME = 14,
    /*! This represents an error code as returned by MMS read services */
    MMS_DATA_ACCESS_ERROR = 15
} MmsType;

typedef struct sMmsDomain MmsDomain;

typedef struct sMmsAccessSpecifier
{
    MmsDomain* domain;
    char* variableName;
    int arrayIndex; /* -1 --> no index present / ignore index */
    char* componentName;
} MmsAccessSpecifier;

typedef struct
{
    const char* domainId;
    const char* itemId;
    int32_t arrayIndex; /* -1 --> no index present / ignore index */
    const char* componentName;
} MmsVariableAccessSpecification;

typedef struct sMmsNamedVariableList* MmsNamedVariableList;
typedef struct sMmsAccessSpecifier* MmsNamedVariableListEntry;

/**
 * \brief Create a new MmsVariableSpecification that can be used to define named variable lists.
 *
 * The created object can be deleted with free(). If the parameter strings were dynamically
 * allocated the deallocation is in the responsibility of the user.
 *
 * \param domainId the MMS domain name of the variable
 * \param itemId the name for the MMS variable
 *
 * \return reference to the new MmsVariableSpecfication object
 */
LIBMMS_API MmsVariableAccessSpecification*
MmsVariableAccessSpecification_create(const char* domainId, const char* itemId);

/**
 * \brief Create a new MmsVariableSpecification that can be used to define named variable lists.
 *
 * The created object can be deleted with free(). If the parameter strings were dynamically
 * allocated the deallocation is in the responsibility of the user. This function should be
 * used for named variable list entries that are array elements or components of array elements
 * in the case when the array element is of complex (structured) type.
 *
 * \param domainId the MMS domain name of the variable
 * \param itemId the name for the MMS variable
 * \param index the array index to describe an array element
 * \param componentName the name of the component of the array element. Should be set to NULL
 *        if the array element is of simple type or the whole array element is required.
 *
 * \return reference to the new MmsVariableSpecfication object
 */
LIBMMS_API MmsVariableAccessSpecification*
MmsVariableAccessSpecification_createAlternateAccess(char* domainId, char* itemId, int32_t index,
        char* componentName);

LIBMMS_API void
MmsVariableAccessSpecification_setDomainId(MmsVariableAccessSpecification* self, const char* domainId);

LIBMMS_API void
MmsVariableAccessSpecification_setItemId(MmsVariableAccessSpecification* self, const char* itemId);

LIBMMS_API void
MmsVariableAccessSpecification_setComponentName(MmsVariableAccessSpecification* self, const char* componentName);

/**
 * \brief Delete the MmsVariableAccessSpecification data structure
 *
 * \param self the instance to delete
 */
LIBMMS_API void
MmsVariableAccessSpecification_destroy(MmsVariableAccessSpecification* self);

/**@}*/


#ifdef __cplusplus
}
#endif

#endif /* MMS_COMMON_H_ */
