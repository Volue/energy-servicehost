/*
 *  tase2_common.h
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef SRC_TASE2_COMMON_TASE2_COMMON_H_
#define SRC_TASE2_COMMON_TASE2_COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include "tase2_api_exports.h"
#include "tase2_endpoint.h"
#include "linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup tase2_common Common API parts
 * @{
 */

/**
 * \brief Specify the log level for the library internal logging
 */
typedef enum {
    /**
     * log level DEBUG - shows the most information (useful for debugging applications)
     */
    TASE2_LOG_DEBUG = 1,

    /**
     * log level INFO - show informational messages (useful to trace communication problems)
     */
    TASE2_LOG_INFO = 2,

    /**
     * log level WARNING - show only errors and warning message that indicate wrong configuration
     */
    TASE2_LOG_WARNING = 3,

    /**
     * log level ERROR - show critical problems and communication errors
     */
    TASE2_LOG_ERROR = 4,

    /**
     * log level NONE - don't show any log messages
     */
    TASE2_LOG_NONE = 5
} Tase2_LogLevel;

typedef enum {
    TASE2_LOG_SOURCE_ICCP  = 1,
    TASE2_LOG_SOURCE_MMS = 2,
    TASE2_LOG_SOURCE_TCP = 3,
    TASE2_LOG_SOURCE_TLS = 4,
    TASE2_LOG_SOURCE_ISO_LAYERS = 5
} Tase2_LogSource;

/**
 * \brief User provided log message handler (optional)
 */
typedef void
(*Tase2_Log_Function) (int logLevel, const char* message);

typedef void
(*Tase2_Log_FunctionEx) (Tase2_LogLevel logLevel, Tase2_LogSource source, Tase2_Endpoint endpoint, Tase2_Endpoint_Connection peer, const char* message);

/**
 * \brief Set the library log level.
 *
 * NOTE: Default log level is TASE2_LOG_INFO
 *
 * \param level the log level to apply
 */
TASE2_API void
Tase2_Library_setLogLevel(Tase2_LogLevel level);

/**
 * \brief Set a user provided log message handler.
 *
 * Note: If not set, or set to NULL the log messages will be
 * printed to stdout. Otherwise library internal log messages
 * will be sent to the application by calling the provided
 * function once for each log message.
 *
 * \param logFunction user provided log handler or NULL
 */
TASE2_API void
Tase2_Library_setLogFunction(Tase2_Log_Function logFunction);

TASE2_API void
Tase2_Library_setLogFunctionEx(Tase2_Log_FunctionEx logFunction);

/**
 * \brief Get the version string of the library (e.g. "1.2.3")
 *
 * The format is MAJOR.MINOR.PATCH
 */
TASE2_API const char*
Tase2_Library_getVersionString();

/**
 * \brief Get the major version of the library
 */
TASE2_API int
Tase2_Library_getMajorVersion();

/**
 * \brief Get the minor version of the library
 */
TASE2_API int
Tase2_Library_getMinorVersion();

/**
 * \brief Get the patch version of the library
 */
TASE2_API int
Tase2_Library_getPatchVersion();

/**
 * \brief Some stack statistics
 */
typedef struct {
    uint64_t messagesSent;  /** application layer messages sent to peer */
    uint64_t messagesRcvd;  /** application layer messages received from peer */
    uint64_t errors;        /** request timeouts, unconfirmed critical reports, ... */
} Tase2_Statistics;

typedef enum {
    TASE2_IND_POINT_TYPE_REAL = 0,
    TASE2_IND_POINT_TYPE_STATE,
    TASE2_IND_POINT_TYPE_DISCRETE,
    TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL
} Tase2_IndicationPointType;

typedef enum {
    TASE2_NO_QUALITY = 0,
    TASE2_QUALITY
} Tase2_QualityClass;

typedef enum {
    /**
     * The indication point or type has no time stamp
     */
    TASE2_NO_TIMESTAMP = 0,

    /**
     * The indication point or type has a time stamp with seconds precision
     */
    TASE2_TIMESTAMP,

    /**
     * The indication point or type has a time stamp with milliseconds precision
     */
    TASE2_TIMESTAMP_EXTENDED
} Tase2_TimeStampClass;

/**
 * \brief Client side error code
 */
typedef enum {

    /**
     * No error occurred (request has been successful)
     */
    TASE2_CLIENT_ERROR_OK = 0,

    /**
     * Request failed
     */
    TASE2_CLIENT_ERROR_FAILED,

    /**
     * Request timeout
     */
    TASE2_CLIENT_ERROR_TIMEOUT,

    /**
     * Access denied by server
     */
    TASE2_CLIENT_ERROR_ACCESS_DENIED,

    /**
     * Server resource is temporarily unavailable
     */
    TASE2_CLIENT_ERROR_TEMPORARILY_UNAVAILABLE,

    /**
     * invalid argument
     */
    TASE2_CLIENT_ERROR_INVALID_ARGUMENT,

    /**
     * server return value invalid
     */
    TASE2_CLIENT_ERROR_INVALID_RETURN_VALUE,

    /**
     * other error
     */
    TASE2_CLIENT_ERROR_OTHER,

    /**
     * object does not exist
     */
    TASE2_CLIENT_ERROR_OBJECT_NOT_EXISTING,

    /**
     * object already exists
     */
    TASE2_CLIENT_ERROR_OBJECT_EXISTS
} Tase2_ClientError;

/**
 * \addtogroup command_values Command values (device model)
 *
 * NOTE: Command values with different names can result in the same command
 *
 * @{
 */

/**
 * A trip command (open a circuit)
 */
#define TASE2_COMMAND_TRIP 0

/**
 * A close command (close a circuit)
 */
#define TASE2_COMMAND_CLOSE 1
#define TASE2_COMMAND_OPEN 0
#define TASE2_COMMAND_OFF 0
#define TASE2_COMMAND_ON 1
#define TASE2_COMMAND_LOWER 0
#define TASE2_COMMAND_RAISE 1

/** @} */


/**
 * \addtogroup supported_cbs Supported conformance blocks (for "Supported_Features")
 * @{
 */

#define TASE2_SUPPORTED_FB_1 1
#define TASE2_SUPPORTED_FB_2 2
#define TASE2_SUPPORTED_FB_3 4
#define TASE2_SUPPORTED_FB_4 8
#define TASE2_SUPPORTED_FB_5 16

/** @} */

/**
 * \addtogroup ds_conditions DS conditions (DS transfer sets)
 * @{
 */

/**
 * Enable periodic transmission of reports
 */
#define TASE2_DS_CONDITION_INTERVAL 1

/**
 * Enable sending integrity reports
 */
#define TASE2_DS_CONDITION_INTEGRITY 2

/**
 * Enable sending reports when a variable changes
 */
#define TASE2_DS_CONDITION_CHANGE 4

/**
 * Enable sending reports when requested by operator
 */
#define TASE2_DS_CONDITION_OPERATOR_REQUESTED 8

/**
 * Enable sending reports triggered by external events
 */
#define TASE2_DS_CONDITION_EXTERNAL_EVENT 16

/** @} */

typedef enum {
    /** No tag present */
    TASE2_TAG_NO_TAG = 0,

    /** The tag prohibits to open or close the device */
    TASE2_TAG_OPEN_AND_CLOSE_INHIBIT = 1,

    /** Tag tag allow to open but inhibits to close the device */
    TASE2_TAG_CLOSE_ONLY_INHIBIT = 2,

    /** The device has an invalid tag */
    TASE2_TAG_INVALID = 3
} Tase2_TagValue;

typedef enum
{
    TASE2_REPORT_REASON_INTERVAL_TIMEOUT = 1,
    TASE2_REPORT_REASON_INTEGRITY_TIMEOUT = 2,
    TASE2_REPORT_REASON_OBJECT_CHANGE = 4,
    TASE2_REPORT_REASON_OPERATOR_REQUESTED = 8,
    TASE2_REPORT_REASON_OTHER_EXTERNAL_EVENT = 16
} Tase2_ReportReason;

typedef enum {
    /**
     * Point value is of State type
     */
    TASE2_VALUE_TYPE_STATE = 0,

    /**
     * Point value is of StateSupplemental type
     */
    TASE2_VALUE_TYPE_STATE_SUPPLEMENTAL,

    /**
     * Point value is of Discrete type
     */
    TASE2_VALUE_TYPE_DISCRETE,

    /**
     * Point value is of Real type
     */
    TASE2_VALUE_TYPE_REAL,

    /**
     * Point value is a Single event
     */
    TASE2_VALUE_TYPE_SINGLE_EVENT,

    /**
     * Point value is a Packed Event
     */
    TASE2_VALUE_TYPE_PACKED_EVENT,

    /**
     * Point value instance represents an error (no point value received)
     */
    TASE2_VALUE_TYPE_ERROR = 99,

    /**
     * Point value type cannot be determined
     */
    TASE2_VALUE_TYPE_UNKNOWN = 100,

    /**
     * Point value is either State or StateSupplemental (ambiguous type)
     */
    TASE2_VALUE_TYPE_STATE_OR_STATE_SUPPLEMENTAL = 101,

    /**
     * Point value is either State or Discrete (ambiguous type)
     */
    TASE2_VALUE_TYPE_STATE_OR_DISCRETE = 102,
} Tase2_PointValueType;


typedef struct sTase2_PointName* Tase2_PointName;

struct sTase2_PointName {
    char domainName[33];
    char pointName[33];
};

/**
 * \defgroup tase2_point_value Point value type
 * @{
 */

/**
 * \brief A TASE.2 point value with type information
 *
 * A point value can have the following types defined here: \ref Tase2_PointValueType
 *
 * NOTE: type information may be inaccurate depending on context
 */
typedef struct sTase2_PointValue* Tase2_PointValue;

/**
 * \brief Get the (estimated) type of the point value
 *
 * \param self the PointValue instance
 *
 * \return type information
 */
TASE2_API Tase2_PointValueType
Tase2_PointValue_getType(Tase2_PointValue self);

/**
 * \brief Get the error code (when point value type is TASE2_VALUE_TYPE_ERROR)
 *
 * \param self the PointValue instance
 *
 * \return the error code
 */
TASE2_API Tase2_ClientError
Tase2_PointValue_getError(Tase2_PointValue self);

/**
 * \brief Set the type of a point value.
 *
 * The elements of a point value can only be interpreted correctly when the TASE.2 type is known.
 * In most cases the TASE.2 type can be derived from the MMS type of the MMS value that has been received.
 * However, in some cases this is not possible because the same MMS type is used for different TASE.2 types.
 * In this case it is required to tell the library what TASE.2 type has to be assumed to interpret the value elements.
 * When you get a type of TASE2_VALUE_TYPE_UNKNOWN, TASE2_VALUE_TYPE_STATE_OR_STATE_SUPPLEMENTAL, or
 * TASE2_VALUE_TYPE_STATE_OR_DISCRETE you have to use this function to advise the correct type.
 *
 * \param self the PointValue instance
 * \param type the TASE.2 type to be used for value interpretation.
 */
TASE2_API void
Tase2_PointValue_setType(Tase2_PointValue self, Tase2_PointValueType type);

/**
 * \brief Release all resource related to the point value instance
 *
 * NOTE: Do not use the instance after this call!
 *
 * \param self the PointValue instance
 */
TASE2_API void
Tase2_PointValue_destroy(Tase2_PointValue self);

/********************************************************
 * Types and functions for indication point types
 *********************************************************/

typedef uint8_t Tase2_DataFlags;

#define TASE2_DATA_FLAGS_UNUSED_0 1
#define TASE2_DATA_FLAGS_UNUSED_1 2
#define TASE2_DATA_FLAGS_VALIDITY_HI 4
#define TASE2_DATA_FLAGS_VALIDITY_LO 8
#define TASE2_DATA_FLAGS_CURRENT_SOURCE_HI 16
#define TASE2_DATA_FLAGS_CURRENT_SOURCE_LO 32
#define TASE2_DATA_FLAGS_NORMAL_VALUE 64
#define TASE2_DATA_FLAGS_TIME_STAMP_QUALITY 128

typedef uint8_t Tase2_DataState;

#define TASE2_DATA_STATE_HI 1
#define TASE2_DATA_STATE_LO 2
#define TASE2_DATA_STATE_VALIDITY_HI 4
#define TASE2_DATA_STATE_VALIDITY_LO 8
#define TASE2_DATA_STATE_CURRENT_SOURCE_HI 16
#define TASE2_DATA_STATE_CURRENT_SOURCE_LO 32
#define TASE2_DATA_STATE_NORMAL_VALUE 64
#define TASE2_DATA_STATE_TIME_STAMP_QUALITY 128

#define TASE2_DATA_STATE_BETWEEN 0
#define TASE2_DATA_STATE_TRIPPED 2
#define TASE2_DATA_STATE_OFF 2
#define TASE2_DATA_STATE_CLOSED 1
#define TASE2_DATA_STATE_ON 1
#define TASE2_DATA_STATE_INVALID 3

#define TASE2_DATA_FLAGS_VALIDITY_VALID 0
#define TASE2_DATA_FLAGS_VALIDITY_HELD 8
#define TASE2_DATA_FLAGS_VALIDITY_SUSPECT 4
#define TASE2_DATA_FLAGS_VALIDITY_NOTVALID 12

#define TASE2_DATA_FLAGS_CURRENT_SOURCE_TELEMETERED 0
#define TASE2_DATA_FLAGS_CURRENT_SOURCE_CALCULATED 32
#define TASE2_DATA_FLAGS_CURRENT_SOURCE_ENTERED 16
#define TASE2_DATA_FLAGS_CURRENT_SOURCE_ESTIMATED 48

typedef uint8_t Tase2_DataStateSupplemental;

#define TASE2_DATA_SUPPL_STATE_HI 1
#define TASE2_DATA_SUPPL_STATE_LO 2
#define TASE2_DATA_SUPPL_STATE_TAG_HI 4
#define TASE2_DATA_SUPPL_STATE_TAG_LO 8
#define TASE2_DATA_SUPPL_STATE_EXPECTED_HI 16
#define TASE2_DATA_SUPPL_STATE_EXPECTED_LO 32

typedef enum {
    TASE2_POINT_VALUE_ERROR_OK = 0,
    TASE2_POINT_VALUE_ERROR_WRONG_TYPE = 1
} Tase2_PointValueError;

/**
 * \brief Create a copy (clone) of the PointValue instances
 *
 * \param self the PointValue instance to clone
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_getCopy(Tase2_PointValue self);

/**
 * \brief Create a new PointValue instance of type "real"
 *
 * \param value the value of the new instance
 *
 * \return the new PointValue instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createReal(float value);

/**
 * \brief Create a new PointValue instance of type "discrete"
 *
 * \param value the value of the new instance
 *
 * \return the new PointValue instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createDiscrete(int32_t value);

/**
 * \brief Create a new PointValue instance of type "state"
 *
 * \param value the value of the new instance
 *
 * \return the new PointValue instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createState(Tase2_DataState value);

/**
 * \brief Create a new PointValue instance of type "state-supplemental"
 *
 * \param value the value of the new instance
 *
 * \return the new PointValue instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createStateSupplemental(Tase2_DataStateSupplemental value);

/**
 * \brief Create a new point value instance of Type RealQ
 *
 * \param value the real (floating point) value
 * \param flags the quality flags of the value
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createRealQ(float value, Tase2_DataFlags flags);

/**
 * \brief Create a new point value instance of Type DiscreteQ
 *
 * \param value the discrete value
 * \param flags the quality flags of the value
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createDiscreteQ(int32_t value, Tase2_DataFlags flags);

/**
 * \brief Create a new PointValue instance of type StateQ
 *
 * \param value the state value
 * \param flags the quality flags of the value
 *
 * \return the new PointValue instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createStateQ(Tase2_DataState value, Tase2_DataFlags flags);

/**
 * \brief Create a new PointValue instance of type StateSupplementalQ
 *
 * \param value the state supplemental value
 * \param flags the quality flags of the value
 *
 * \return the new PointValue instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createStateSupplementalQ(Tase2_DataStateSupplemental value, Tase2_DataFlags flags);

/**
 * \brief Create a new point value instance of Type RealQTimeTag
 *
 * \param value the real (floating point) value
 * \param flags the quality flags of the value
 * \param timeTag the time tag in seconds since Epoch (Unix time)
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createRealQTimeTag(float value, Tase2_DataFlags flags, int32_t timeTag);

/**
 * \brief Create a new point value instance of Type StateQTimeTag
 *
 * \param valueAndFlags the combined value and flags value
 * \param timeTag the time tag in seconds since Epoch (Unix time)
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createStateQTimeTag(Tase2_DataFlags valueAndFlags, int32_t timeTag);

/**
 * \brief Create a new point value instance of Type DiscreteQTimeTag
 *
 * \param value the discrete value
 * \param flags the quality flags of the value
 * \param timeTag the time tag in seconds since Epoch (Unix time)
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createDiscreteQTimeTag(int32_t value, Tase2_DataFlags flags, int32_t timeTag);

/**
 * \brief Create a new PointValue instance of type StateSupplementalQTimeTag
 *
 * \param value the state supplemental value
 * \param flags the quality flags of the value
 * \param timeTag the time tag in seconds since Epoch (Unix time)
 *
 * \return the new PointValue instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createStateSupplementalQTimeTag(Tase2_DataStateSupplemental value, Tase2_DataFlags flags, int32_t timeTag);

/**
 * \brief Create a new point value instance of Type RealExtended
 *
 * \param value the real (floating point) value
 * \param flags the quality flags of the value
 * \param timeTag the time tag in seconds since Epoch (Unix time)
 * \param cov the counter of events of the value
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createRealExtended(float value, Tase2_DataFlags flags, int32_t timeTag, uint16_t cov);

/**
 * \brief Create a new point value instance of Type StateExtended
 *
 * \param valueAndFlags the combined value and flags value
 * \param timeTag the time tag in seconds since Epoch (Unix time)
 * \param cov the counter of events of the value
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createStateExtended(Tase2_DataFlags valueAndFlags, int32_t timeTag, uint16_t cov);

/**
 * \brief Create a new point value instance of Type DiscreteExtended
 *
 * \param value the discrete value
 * \param flags the quality flags of the value
 * \param timeTag the time tag in seconds since Epoch (Unix time)
 * \param cov the counter of events of the value
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createDiscreteExtended(int32_t value, Tase2_DataFlags flags, int32_t timeTag, uint16_t cov);

/**
 * \brief Create a new PointValue instance of type StateSupplementalExtended
 *
 * \param value the state supplemental value
 * \param flags the quality flags of the value
 * \param timeTag the time tag in seconds since Epoch (Unix time)
 * \param cov the counter of events of the value
 *
 * \return the new PointValue instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createStateSupplementalExtended(Tase2_DataStateSupplemental value, Tase2_DataFlags flags, int32_t timeTag, uint16_t cov);

/**
 * \brief Create a new point value instance of Type RealQTimeTagExtended
 *
 * \param value the real (floating point) value
 * \param flags the quality flags of the value
 * \param timeTag the extended time tag in milliseconds since Epoch
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createRealQTimeTagExtended(float value, Tase2_DataFlags flags, uint64_t timeTagExtended);

/**
 * \brief Create a new point value instance of Type StateQTimeTagExtended
 *
 * \param value the discrete value
 * \param flags the quality flags of the value
 * \param timeTag the extended time tag in milliseconds since Epoch
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createStateQTimeTagExtended(Tase2_DataFlags valueAndFlags, uint64_t timeTagExtended);

/**
 * \brief Create a new point value instance of Type DiscreteQTimeTagExtended
 *
 * \param value the discrete value
 * \param flags the quality flags of the value
 * \param timeTag the extended time tag in milliseconds since Epoch
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createDiscreteQTimeTagExtended(int32_t value, Tase2_DataFlags flags, uint64_t timeTagExtended);

/**
 * \brief Create a new PointValue instance of type StateSupplementalQTimeTagExtended
 *
 * \param value the state supplemental value
 * \param flags the quality flags of the value
 * \param timeTag the extended time tag in milliseconds since Epoch
 *
 * \return the new PointValue instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createStateSupplementalQTimeTagExtended(Tase2_DataStateSupplemental value, Tase2_DataFlags flags, uint64_t timeTagExtended);

/**
 * \brief Get the real value
 *
 * \param the point value instance
 *
 * \return the real value or NAN when the Tase2_PointValue object has the wrong type
 */
TASE2_API float
Tase2_PointValue_getValueReal(Tase2_PointValue self);

/**
 * \brief Get the real value
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the real value or NAN when the Tase2_PointValue object has the wrong type
 */
TASE2_API float
Tase2_PointValue_getValueRealEx(Tase2_PointValue self, Tase2_PointValueError* err);

/**
 * \brief Set the real value
 *
 * \param the point value instance
 * \param value the new value
 */
TASE2_API void
Tase2_PointValue_setValueReal(Tase2_PointValue self, float value);

/**
 * \brief Set the real value
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param value the new value
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setValueRealEx(Tase2_PointValue self, float value);

/**
 * \brief Get the value of an indication point of type DISCRETE
 *
 * \param the point value instance
 *
 * \return the discrete value or 0 when the Tase2_PointValue object has the wrong type
 */
TASE2_API int32_t
Tase2_PointValue_getValueDiscrete(Tase2_PointValue self);

/**
 * \brief Get the value of an indication point of type DISCRETE
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the discrete value or 0 when the Tase2_PointValue object has the wrong type
 */
TASE2_API int32_t
Tase2_PointValue_getValueDiscreteEx(Tase2_PointValue self, Tase2_PointValueError* err);

/**
 * \brief Set the value of an indication point of type DISCRETE
 *
 * \param the point value instance
 * \param value the new value
 */
TASE2_API void
Tase2_PointValue_setValueDiscrete(Tase2_PointValue self, int32_t value);

/**
 * \brief Set the value of an indication point of type DISCRETE
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param value the new value
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setValueDiscreteEx(Tase2_PointValue self, int32_t value);

/**
 * \brief Get the value of an indication point of type STATE
 *
 * \param the point value instance
 * \return the state value
 */
TASE2_API Tase2_DataState
Tase2_PointValue_getValueState(Tase2_PointValue self);

/**
 * \brief Get the value of an indication point of type STATE
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the state value
 */
TASE2_API Tase2_DataState
Tase2_PointValue_getValueStateEx(Tase2_PointValue self, Tase2_PointValueError* err);

/**
 * \brief Set the value of an indication point of type STATE
 *
 * \param the point value instance
 * \param value the state value
 */
TASE2_API void
Tase2_PointValue_setValueState(Tase2_PointValue self, Tase2_DataState value);

/**
 * \brief Set the value of an indication point of type STATE
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param value the state value
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setValueStateEx(Tase2_PointValue self, Tase2_DataState value);

/**
 * \brief Get the value of an indication point of type STATE_SUPPLEMENTAL
 *
 * \param the point value instance
 *
 * \return the state value
 */
TASE2_API Tase2_DataStateSupplemental
Tase2_PointValue_getValueStateSupplemental(Tase2_PointValue self);

/**
 * \brief Get the value of an indication point of type STATE_SUPPLEMENTAL
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the state value
 */
TASE2_API Tase2_DataStateSupplemental
Tase2_PointValue_getValueStateSupplementalEx(Tase2_PointValue self, Tase2_PointValueError* err);

/**
 * \brief Set the value of an indication point of type STATE_SUPPLEMENTAL
 *
 * \param the point value instance
 * \param value the state value
 */
TASE2_API void
Tase2_PointValue_setValueStateSupplemental(Tase2_PointValue self, Tase2_DataStateSupplemental value);

/**
 * \brief Set the value of an indication point of type STATE_SUPPLEMENTAL
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param value the state value
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setValueStateSupplementalEx(Tase2_PointValue self, Tase2_DataStateSupplemental value);

/**
 * \brief Try to determine if the value has a timestamp
 *
 * NOTE: When the \ref Tase2_PointValue_getType function returns TASE2_VALUE_TYPE_STATE_OR_DISCRETE
 * the function cannot determine if there is a timestamp or not. In this case the function returns false
 *
 * \return true when the value contains a timestamp, false otherwise
 */
TASE2_API bool
Tase2_PointValue_hasTimeStamp(Tase2_PointValue self);

/**
 * \brief Get the timestamp class of the value
 *
 * NOTE: When the \ref Tase2_PointValue_getType function returns TASE2_VALUE_TYPE_STATE_OR_DISCRETE
 * the function cannot determine if there is a timestamp or not. In this case the function returns TASE2_NO_TIMESTAMP
 *
 * \returns the timestamp class of the point value
 */
TASE2_API Tase2_TimeStampClass
Tase2_PointValue_getTimeStampClass(Tase2_PointValue self);

/**
 * \brief Get the time stamp of the last data point change or update
 *
 * NOTE: the result is only valid when the data point type includes a time stamp
 *
 * \param the point value instance
 *
 * \return the time stamp in ms since epoch
 */
TASE2_API uint64_t
Tase2_PointValue_getTimeStamp(Tase2_PointValue self);

/**
 * \brief Get the time stamp of the last data point change or update
 *
 * New in version 2.1.0
 *
 * NOTE: the result is only valid when the data point type includes a time stamp
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the time stamp in ms since epoch
 */
TASE2_API uint64_t
Tase2_PointValue_getTimeStampEx(Tase2_PointValue self, Tase2_PointValueError *err);

/**
 * \brief Set the time stamp of the last data point change or update
 *
 * NOTE: has no effect when the data point type doesn't include a time stamp
 *
 * \param timestamp the time stamp in ms since epoch
 */
TASE2_API void
Tase2_PointValue_setTimeStamp(Tase2_PointValue self, uint64_t timestamp);

/**
 * \brief Set the time stamp of the last data point change or update
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param timestamp the time stamp in ms since epoch
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setTimeStampEx(Tase2_PointValue self, uint64_t timestamp);

/**
 * \brief Get the quality flags of a data point
 *
 * NOTE: the result is only valid when the data point type includes quality
 *
 * \param the point value instance
 *
 * \return the quality flags of a data point (or 0xff when the point value type has no flags)
 */
TASE2_API Tase2_DataFlags
Tase2_PointValue_getFlags(Tase2_PointValue self);

/**
 * \brief Get the quality flags of a data point
 *
 * New in version 2.1.0
 *
 * NOTE: the result is only valid when the data point type includes quality
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the quality flags of a data point (or 0xff when the point value type has no flags)
 */
TASE2_API Tase2_DataFlags
Tase2_PointValue_getFlagsEx(Tase2_PointValue self, Tase2_PointValueError *err);

/**
 * \brief Set the quality flags of a data point
 *
 * NOTE: has no effect when the data point type doesn't include a quality
 *
 * \param the point value instance
 *
 * \param flags the quality flags of a data point
 */
TASE2_API void
Tase2_PointValue_setFlags(Tase2_PointValue self, Tase2_DataFlags flags);
/**
 * \brief Set the quality flags of a data point
 *
 * New in version 2.1.0
 *
 * NOTE: has no effect when the data point type doesn't include a quality
 *
 * \param the point value instance
 * \param flags the quality flags of a data point
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setFlagsEx(Tase2_PointValue self, Tase2_DataFlags flags);

/**
 * \brief Get the COV (counter of value changes) of a data point
 *
 * NOTE: the result is only valid when the data point type includes COV
 *
 * \param the point value instance
 *
 * \return the COV value
 */
TASE2_API uint16_t
Tase2_PointValue_getCOV(Tase2_PointValue self);

/**
 * \brief Get the COV (counter of value changes) of a data point
 *
 * New in version 2.1.0
 *
 * NOTE: the result is only valid when the data point type includes COV
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the COV value
 */
TASE2_API uint16_t
Tase2_PointValue_getCOVEx(Tase2_PointValue self, Tase2_PointValueError *err);

/**
 * \brief Set the COV (counter of value changes) of a data point
 *
 * NOTE: has no effect when the data point type doesn't include a COV
 *
 * \param the point value instance
 * \param cov the COV value
 */
TASE2_API void
Tase2_PointValue_setCOV(Tase2_PointValue self, uint16_t cov);

/**
 * \brief Set the COV (counter of value changes) of a data point
 *
 * New in version 2.1.0
 *
 * NOTE: has no effect when the data point type doesn't include a COV
 *
 * \param the point value instance
 * \param cov the COV value
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setCOVEx(Tase2_PointValue self, uint16_t cov);

/********************************************************
 * Types and functions for protection event types
 *********************************************************/

typedef uint8_t Tase2_SingleEventFlags;

#define TASE2_SINGLE_FLAGS_ELAPSED_TIME_VALIDITY 1
#define TASE2_SINGLE_FLAGS_BLOCKED               2
#define TASE2_SINGLE_FLAGS_SUBSTITUTED           4
#define TASE2_SINGLE_FLAGS_TOPICAL               8
#define TASE2_SINGLE_FLAGS_EVENT_VALIDITY        16
#define TASE2_SINGLE_FLAGS_UNUSED                32
#define TASE2_SINGLE_FLAGS_EVENT_STATE_HI        64
#define TASE2_SINGLE_FLAGS_EVENT_STATE_LO        128

typedef uint8_t Tase2_EventFlags;

#define TASE2_EVENT_FLAGS_GENERAL  1
#define TASE2_EVENT_FLAGS_PHASE_1  2
#define TASE2_EVENT_FLAGS_PHASE_2  4
#define TASE2_EVENT_FLAGS_PHASE_3  8
#define TASE2_EVENT_FLAGS_EARTH    16
#define TASE2_EVENT_FLAGS_REVERSE  32

typedef uint8_t Tase2_PackedEventFlags;

#define TASE2_PACKED_FLAGS_ELAPSED_TIME_VALIDITY 1
#define TASE2_PACKED_FLAGS_BLOCKED               2
#define TASE2_PACKED_FLAGS_SUBSTITUTED           4
#define TASE2_PACKED_FLAGS_TOPICAL               8
#define TASE2_PACKED_FLAGS_EVENT_VALIDITY        16

/**
 * \brief Create a new point value instance of type single event (\ref TASE2_VALUE_TYPE_SINGLE_EVENT)
 *
 * \param eventFlags the single event flags
 * \param operatingTime duration of the event in milliseconds
 * \param eventTime the event time in milliseconds since epoch
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createSingleProtectionEvent(Tase2_SingleEventFlags eventFlags, int32_t operatingTime, uint64_t eventTime);

/**
 * \brief Create a new point value instance of type packed event (\ref TASE2_VALUE_TYPE_PACKED_EVENT)
 *
 * \param packedEvend the packed event values
 * \param packedEventFlags the packed event flags
 * \param operatingTime duration of the event in milliseconds
 * \param eventTime the event time in milliseconds since epoch
 *
 * \return the new point value instance
 */
TASE2_API Tase2_PointValue
Tase2_PointValue_createPackedProtectionEvent(Tase2_EventFlags packedEvent, Tase2_PackedEventFlags packedEventFlags, int32_t operatingTime, uint64_t eventTime);

/**
 * \brief Get the single event flags value
 *
 * NOTE: the result is only valid when data point is of type \ref TASE2_VALUE_TYPE_SINGLE_EVENT
 *
 * \param the point value instance
 *
 * \return the single event flags value
 */
TASE2_API Tase2_SingleEventFlags
Tase2_PointValue_getSingleEventFlags(Tase2_PointValue self);

/**
 * \brief  Get the single event flags value
 *
 * New in version 2.1.0
 *
 * NOTE: the result is only valid when data point is of type \ref TASE2_VALUE_TYPE_SINGLE_EVENT
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the single event flags value
 */
TASE2_API Tase2_SingleEventFlags
Tase2_PointValue_getSingleEventFlagsEx(Tase2_PointValue self, Tase2_PointValueError *err);

/**
 * \brief Set the single event flags value
 *
 * NOTE: the function has only an effect when point value is of type \ref TASE2_VALUE_TYPE_SINGLE_EVENT
 *
 * \param the point value instance
 * \param eventFlags  the single event flags
 */
TASE2_API void
Tase2_PointValue_setSingleEventFlags(Tase2_PointValue self, Tase2_SingleEventFlags eventFlags);

/**
 * \brief Set the single event flags value
 *
 * New in version 2.1.0
 *
 * NOTE: the function has only an effect when point value is of type \ref TASE2_VALUE_TYPE_SINGLE_EVENT
 *
 * \param the point value instance
 * \param eventFlags  the single event flags
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setSingleEventFlagsEx(Tase2_PointValue self, Tase2_SingleEventFlags eventFlags);

/**
 * \brief Get the packed event flags values
 *
 * NOTE: the result is only valid when data point is of type \ref TASE2_VALUE_TYPE_PACKED_EVENT
 *
 * \param the point value instance
 *
 * \return the packed event flags value
 */
TASE2_API Tase2_EventFlags
Tase2_PointValue_getPackedEvent(Tase2_PointValue self);

/**
 * \brief Get the packed event values
 *
 * New in version 2.1.0
 *
 * NOTE: the result is only valid when data point is of type \ref TASE2_VALUE_TYPE_PACKED_EVENT
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the packed event value
 */
TASE2_API Tase2_EventFlags
Tase2_PointValue_getPackedEventEx(Tase2_PointValue self, Tase2_PointValueError *err);

/**
 * \brief Set the packed event values
 *
 * NOTE: the function has only an effect when point value is of type \ref TASE2_VALUE_TYPE_PACKED_EVENT
 *
 * \param the point value instance
 * \param packedEvent the packed event values
 */
TASE2_API void
Tase2_PointValue_setPackedEvent(Tase2_PointValue self, Tase2_EventFlags packedEvent);

/**
 * \brief Set the packed event values
 *
 * New in version 2.1.0
 *
 * NOTE: the function has only an effect when point value is of type \ref TASE2_VALUE_TYPE_PACKED_EVENT
 *
 * \param the point value instance
 * \param packedEvent the packed event values
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
Tase2_PointValueError
Tase2_PointValue_setPackedEventEx(Tase2_PointValue self, Tase2_EventFlags packedEvent);

/**
 * \brief Get the packed event flags value
 *
 * NOTE: the result is only valid when data point is of type \ref TASE2_VALUE_TYPE_PACKED_EVENT
 *
 * \param the point value instance
 *
 * \return the packed event flags values
 */
TASE2_API Tase2_PackedEventFlags
Tase2_PointValue_getPackedEventFlags(Tase2_PointValue self);

/**
 * \brief Get the packed event flags value
 *
 * New in version 2.1.0
 *
 * NOTE: the result is only valid when data point is of type \ref TASE2_VALUE_TYPE_PACKED_EVENT
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the packed event flags values
 */
TASE2_API Tase2_PackedEventFlags
Tase2_PointValue_getPackedEventFlagsEx(Tase2_PointValue self, Tase2_PointValueError *err);

/**
 * \brief Set the packed event flags
 *
 * NOTE: the function has only an effect when point value is of type \ref TASE2_VALUE_TYPE_PACKED_EVENT
 *
 * \param the point value instance
 * \param packedEvent the packed event flags
 */
TASE2_API void
Tase2_PointValue_setPackedEventFlags(Tase2_PointValue self, Tase2_PackedEventFlags flags);

/**
 * \brief Set the packed event flags
 *
 * New in version 2.1.0
 *
 * NOTE: the function has only an effect when point value is of type \ref TASE2_VALUE_TYPE_PACKED_EVENT
 *
 * \param the point value instance
 * \param packedEvent the packed event flags
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setPackedEventFlagsEx(Tase2_PointValue self, Tase2_PackedEventFlags flags);

/**
 * \brief Get the operation or event duration.
 *
 * \param the point value instance
 *
 * \return the operation or event duration in milliseconds.
 */
TASE2_API int32_t
Tase2_PointValue_getOperatingTime(Tase2_PointValue self);

/**
 * \brief Get the operation or event duration.
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the operation or event duration in milliseconds.
 */
TASE2_API int32_t
Tase2_PointValue_getOperatingTimeEx(Tase2_PointValue self, Tase2_PointValueError *err);

/**
 * \brief Set the operation or event duration.
 *
 * \param the point value instance
 * \param operatingTime the operation or event duration in milliseconds.
 */
TASE2_API void
Tase2_PointValue_setOperatingTime(Tase2_PointValue self, int32_t operatingTime);

/**
 * \brief Set the operation or event duration.
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param operatingTime the operation or event duration in milliseconds.
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setOperatingTimeEx(Tase2_PointValue self, int32_t operatingTime);

/**
 * \brief Get the event/operation start time
 *
 * \param the point value instance
 *
 * \return the operation or event start time in milliseconds since epoch
 */
TASE2_API uint64_t
Tase2_PointValue_getEventTime(Tase2_PointValue self);

/**
 * \brief Get the event/operation start time
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param[out] err \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 *
 * \return the operation or event start time in milliseconds since epoch
 */
TASE2_API uint64_t
Tase2_PointValue_getEventTimeEx(Tase2_PointValue self, Tase2_PointValueError *err);

/**
 * \brief Set the event/operation start time
 *
 * \param the point value instance
 * \param eventTime the operation or event start time in milliseconds since epoch
 */
TASE2_API void
Tase2_PointValue_setEventTime(Tase2_PointValue self, uint64_t eventTime);

/**
 * \brief Set the event/operation start time
 *
 * New in version 2.1.0
 *
 * \param the point value instance
 * \param eventTime the operation or event start time in milliseconds since epoch
 *
 * \return \ref TASE2_POINT_VALUE_ERROR_OK or \ref TASE2_POINT_VALUE_ERROR_WRONG_TYPE
 */
TASE2_API Tase2_PointValueError
Tase2_PointValue_setEventTimeEx(Tase2_PointValue self, uint64_t eventTime);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SRC_TASE2_COMMON_TASE2_COMMON_H_ */
