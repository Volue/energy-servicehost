/*
 *  tase2_server.h
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2021 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef TASE2_SERVER_H_
#define TASE2_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tase2_common.h"
#include "tase2_model.h"
#include "tase2_endpoint.h"
#include "tls_config.h"

/**
 * \defgroup tase2_server Server side API parts
 * @{
 */

/**
 * \brief Type for the result of callback handlers for operations
 */
typedef enum
{
    /** Operation has been successful */
    TASE2_RESULT_SUCCESS = -1,

    /** Operation cannot be executed because of an hardware fault */
    TASE2_RESULT_HARDWARE_FAULT = 1,

    /** The requested resource is temporarily unavailable */
    TASE2_RESULT_TEMPORARILY_UNAVAILABLE = 2,

    /** The access to the resource is denied because of insufficient user privileges */
    TASE2_RESULT_OBJECT_ACCESS_DENIED = 3,

    /** Operation is rejected because of an invalid parameter value */
    TASE2_RESULT_OBJECT_VALUE_INVALID = 11
} Tase2_HandlerResult;

/**
 * \brief Value for the operate action (supports the different types for command and set points)
 */
typedef struct {
    union {
        int16_t commandValue; /** value for command */
        int16_t discreteValue; /** value for discrete set point */
        float realValue; /** value for real (float) set point */
    };
} Tase2_OperateValue;

typedef struct sTase2_SentPointValue* Tase2_SentPointValue;

TASE2_API Tase2_DataPoint
Tase2_SentPointValue_getDataPoint(Tase2_SentPointValue self);

TASE2_API Tase2_PointValue
Tase2_SentPointValue_getPointValue(Tase2_SentPointValue self);

/**
 * \brief Callback handler for accepted client connections
 *
 * This callback is called when a new connection is accepted or an already accepted connection is closed.
 *
 * \param parameter user provided parameter
 * \param clientAddress client IP address and port number (only valid in the context of the callback function)
 * \param clientBlt the BLT associated with the client
 * \param connect If true, client is connecting, otherwise disconnecting
 */
typedef void (*Tase2_ClientConnectionHandler) (void* parameter, const char* clientAddress, Tase2_BilateralTable clientBlt, bool connect);

/**
 * \brief Callback handler for the select operation
 *
 * \param parameter user provided parameter
 * \param controlPoint target control point (device) of the select operation
 *
 * \return TASE2_RESULT_SUCCESS when the operation has been executed, or another value indicating why the operation is not accepted
 */
typedef Tase2_HandlerResult (*Tase2_SelectHandler) (void* parameter, Tase2_ControlPoint controlPoint);

/**
 * \brief Callback handler for the operate operation
 *
 * \param parameter user provided parameter
 * \param controlPoint target control point (device) of the operate request
 * \param value the command/set point value
 *
 * \return TASE2_RESULT_SUCCESS when the command is accepted, error code otherwise
 */
typedef Tase2_HandlerResult (*Tase2_OperateHandler) (void* parameter, Tase2_ControlPoint controlPoint, Tase2_OperateValue value);

typedef Tase2_HandlerResult (*Tase2_OperateHandlerEx) (void* parameter, Tase2_ControlPoint controlPoint, int16_t cmdDiscreteValue, float realValue);

/**
 * \brief Callback handler for the set tag operation
 *
 * \param parameter user provided parameter
 * \param controlPoint target control point (device) of the set tag request
 * \param value the tag value
 * \param reason a text message from the client to indicate the reason of the tag
 *
 * \return TASE2_RESULT_SUCCESS when the tag is accepted, error code otherwise
 */
typedef Tase2_HandlerResult (*Tase2_SetTagHandler) (void* parameter, Tase2_ControlPoint controlPoint, Tase2_TagValue value, const char* reason);

/**
 * \brief Callback handler for write operations to indication points (and protection event data points)
 *
 * \param parameter user provided context parameter
 * \param domain domain of the data point to be written
 * \param dataPoint data point to be written
 * \param value the new value of the data point
 *
 * \return TASE2_RESULT_SUCCESS when the new value accepted, or another value indicating why the new value is not accepted
 */
typedef Tase2_HandlerResult (*Tase2_WriteDataHandler) (void* parameter, Tase2_Domain domain, Tase2_DataPoint dataPoint, Tase2_PointValue value);

/**
 * \brief Callback handler for critical report timeout or negative acknowledge
 *
 * \param parameter used provided context parameter
 * \param transferSet the transfer set that caused the report
 * \param timeSent time when the report has been sent (milliseconds since UTC epoch)
 * \param nack true if a NACK message caused the callback, false if the callback was caused by a timeout
 */
typedef void (*Tase2_TSCriticalNotConfirmedHandler) (void* parameter, Tase2_TransferSet transferSet, uint64_t timeSent, bool nack);

/**
 * \brief Callback handler for DSTS state changes (enabled/disabled)
 *
 * \param parameter user provided context parameter
 * \param transferSet the transfer set that changed its state
 * \param enabled true when the new state is enabled, false otherwise
 */
typedef void (*Tase2_DSTransferSetStateChangedHandler) (void* parameter, Tase2_DSTransferSet transferSet, bool enabled);

/**
 * \brief Callback handler for DSTS updates (enabled/disabled)
 *
 * \param parameter user provided context parameter
 * \param peer the client that caused the update
 * \param clientBlt the BLT associated with the client
 * \param transferSet the transfer set that is updated
 * \param isEnabled true when the state is enabled, false otherwise
 */
typedef void (*Tase2_DSTransferSetUpdateHandler) (void* parameter, Tase2_Endpoint_Connection peer, Tase2_BilateralTable clientBlt, Tase2_DSTransferSet transferSet, bool isEnabled);

/**
 * \brief Callback handler that is called when a data set is created or deleted
 *
 * \param parameter user provided context parameter
 * \param create when true, the data set is created, when false it is deleted
 * \param peer the client that caused the update
 * \param clientBlt the BLT associated with the client
 * \param dataSetDomain the domain of the data set
 * \param dataSetName the name of the created or deleted data set
 * \param dataPoints the data points of the created or deleted data set (list elements are of type \ref Tase2_DataPoint)
 */
typedef void (*Tase2_DataSetEventHandler) (void* parameter, bool create, Tase2_Endpoint_Connection peer, Tase2_BilateralTable clientBlt, Tase2_Domain dataSetDomain, char* dataSetName, LinkedList dataPoints);

/**
 * \brief Callback handler for IM transfer set events (enabled/disabled)
 *
 * \param parameter user provided context parameter
 * \param peer the client that caused the update
 * \param clientBlt the BLT associated with the client
 * \param isEnabled true when the state is enabled, false otherwise
 */
typedef void (*Tase2_IMTransferSetEventHandler) (void* parameter, Tase2_Endpoint_Connection peer, Tase2_BilateralTable clientBlt, bool enabled);

/**
 * \brief Callback handler for sent DSTS reports
 *
 * \param parameter user provided context parameter
 * \param peer the client that caused the update
 * \param clientBlt the BLT associated with the client
 * \param transferSet the transfer set that sending the report
 * \param sentDataPoints list of sent data points (list elements of type \ref Tase2_SentPointValue)
 * \param reason reason of the transfer set report
 */
typedef void (*Tase2_DSTransferSetReportSentHandler) (void* parameter, Tase2_Endpoint_Connection peer, Tase2_BilateralTable clientBlt, Tase2_DSTransferSet transferSet, LinkedList sentDataPoints, Tase2_ReportReason reason);

/**
 * \brief Create a new TASE.2 server instance
 *
 * NOTE: This function creates a new passive endpoint (see \ref Tase2_Endpoint) instance (TCP server).
 *
 * \param dataModel the data model to use
 * \param tlsConfiguration the TLS configuration object or NULL when no TLS is used.
 *
 * \return the new server instance
 */
TASE2_API Tase2_Server
Tase2_Server_create(Tase2_DataModel dataModel, TLSConfiguration tlsConfiguration);

/**
 * \brief Create a new TASE.2 server instance using an existing endpoint instance
 *
 * \param dataModel the data model to use
 * \param endpoint the endpoint instance to use
 *
 * \return the new server instance
 */
TASE2_API Tase2_Server
Tase2_Server_createEx(Tase2_DataModel dataModel, Tase2_Endpoint endpoint);

/**
 * \brief Get the endpoint instance used by the server
 *
 * \return the endpoint instance
 */
TASE2_API Tase2_Endpoint
Tase2_Server_getEndpoint(Tase2_Server self);

/**
 * \brief Set the local IP address.
 *
 * NOTE: Calls \ref Tase2_Endpoint_setLocalIpAddress
 *
 * \param self the Tase2_Server instance
 * \param localIpAddress the local IP address as C string (an internal copy will be created)
 */
TASE2_API void
Tase2_Server_setLocalIpAddress(Tase2_Server self, const char* localIpAddress);

/**
 * \brief Set the local TCP port.
 *
 * NOTE: Calls \ref Tase2_Endpoint_setLocalTcpPort
 *
 * \param self Tase2_Server instance
 * \param tcpPort TCP port number or -1 to use the default TCP port numbers 102 (MMS) or 3782 (MMS/TLS)
 */
TASE2_API void
Tase2_Server_setTcpPort(Tase2_Server self, int tcpPort);

/**
 * \brief Set application endpoint identifier of the local endpoint
 *
 * NOTE: It is important to set this parameter correctly to match the remotely configured parameter.
 *
 * \param apTitle AP-title part of the local application endpoint identifier
 * \param aeQualifier AE-qualifier part of the local application endpoint identifier
 */
TASE2_API void
Tase2_Server_setLocalApTitle(Tase2_Server self, const char* apTitle, int aeQualifier);

/**
 * \brief set local addresses for the lower layers
 *
 * This function can be used to set the addresses for the lower layer protocols (presentation, session, and transport
 * layer). Calling this function is optional. It might be required when the peer expects specific
 * values for the parameters.
 *
 *  \param pSelector the P-Selector (presentation layer address)
 *  \param sSelector the S-Selector (session layer address)
 *  \param tSelector the T-Selector (ISO transport layer address)
 */
TASE2_API void
Tase2_Server_setLocalAddresses(Tase2_Server self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector);

TASE2_API IsoApplicationAddress
Tase2_Server_getLocalAddresses(Tase2_Server self);

/**
 * \brief Set the timeout for select operations (device model)
 *
 * \param self Tase2_Server instance
 * \param timeoutInMs select timeout in milliseconds
 */
TASE2_API void
Tase2_Server_setSelectTimeout(Tase2_Server self, int timeoutInMs);


/**
 * \brief Set the timeout for critical report confirmations
 *
 * \param self Tase2_Server instance
 * \param timeoutInMs timeout in milliseconds
 */
TASE2_API void
Tase2_Server_setCriticalReportTimeout(Tase2_Server self, int timeoutInMs);

/**
 * \brief Set the maximum event queue size for DS transfer sets
 *
 * Use this function to limit the memory required to buffer events.
 *
 * \param self Tase2_Server instance
 * \param maxSize a positive number to limit the event queue size or -1 (unlimited size)
 */
TASE2_API void
Tase2_Server_setMaxDSTSEventQueueSize(Tase2_Server self, int maxSize);

/**
 * \brief Load bilateral table (BLT) into the server (part of the server configuration phase)
 *
 * \param self Tase2_Server instance
 * \param blt a BLT instance
 */
TASE2_API void
Tase2_Server_addBilateralTable(Tase2_Server self, Tase2_BilateralTable blt);

/**
 * \brief Set own identity for the identity service
 *
 * \param self Tase2_Server instance
 *
 * \param vendor device/software vendor name
 * \param model device/software model name
 * \param revision device/software revision name
 */
TASE2_API void
Tase2_Server_setIdentity(Tase2_Server self, char* vendor, char* model, char* revision);

/**
 * \brief Change the default value of the "Supported_Features" variable
 *
 * NOTE: Default is conformance blocks 1, 2, 4, and 5
 *
 * \param self Tase2_Server instance
 * \param value addition of \ref supported_cbs
 */
TASE2_API void
Tase2_Server_setSupportedFeatures(Tase2_Server self, int value);

/**
 * \brief Set the client connection handler that is called when a client connects or disconnects
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setClientConnectionHandler(Tase2_Server self, Tase2_ClientConnectionHandler handler, void* parameter);

/**
 * \brief Set the select operation callback handler and parameter
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setSelectHandler(Tase2_Server self, Tase2_SelectHandler handler, void* parameter);

/**
 * \brief Set the operate operation callback handler and parameter
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setOperateHandler(Tase2_Server self, Tase2_OperateHandler handler, void* parameter);


/**
 * \brief Set the alternative operate handler (used to simplify programming language bindings)
 */
TASE2_API void
Tase2_Server_setOperateHandlerEx(Tase2_Server self, Tase2_OperateHandlerEx handler, void* parameter);

/**
 * \brief Set the set tag operation callback handler and parameter
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setSetTagHandler(Tase2_Server self, Tase2_SetTagHandler handler, void* parameter);

/**
 * \brief Set the Tase2_WriteDataHandler callback handler that is called when a data point is written
 *
 * A data point can be either a Tase2_IndicationPoint or a Tase2_ProtectionEquipment instance
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setWriteDataHandler(Tase2_Server self, Tase2_WriteDataHandler handler, void* parameter);

/**
 * \brief Install a callback handler that is called when a transfer set report (marked as critical) is not confirmed by the client or confirmed negative
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setTSCriticalNotConfirmedHandler(Tase2_Server self, Tase2_TSCriticalNotConfirmedHandler handler, void* parameter);

/**
 * \brief Install a callback handler that is called whenever the state of a DS transfer set changes
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setDSTransferSetStateChangedHandler(Tase2_Server self, Tase2_DSTransferSetStateChangedHandler handler, void* parameter);

/**
 * \brief Install a callback handler that is called whenever a dynamic data set is created or deleted by a client
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setDataSetEventHandler(Tase2_Server self, Tase2_DataSetEventHandler handler, void* parameter);

/**
 * \brief Install a callback handler that is called whenever the DS transfer set is updated (enabled/disabled)
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setDSTransferSetUpdateHandler(Tase2_Server self, Tase2_DSTransferSetUpdateHandler handler, void* parameter);

/**
 * \brief Install a callback handler that is called whenever the IM Transfer set is enabled or disabled
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setIMTransferSetEventHandler(Tase2_Server self, Tase2_IMTransferSetEventHandler handler, void* parameter);

/**
 * \brief Install a callback handler that is called whenever a transfer set report is sent
 *
 * \param self Tase2_Server instance
 * \param handler the callback handler function
 * \param parameter user provided parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Server_setDSTransferSetReportSentHandler(Tase2_Server self, Tase2_DSTransferSetReportSentHandler handler, void* parameter);

/**
 * \brief Manually trigger a DSTS report transmission of the given transfer set
 *
 * This function can be used to trigger data set transfer set reports with the reasons "operator-requested" and
 * "other-external-event". The reports will only be sent when the transfer set it enabled.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Server instance
 * \param transferSet the transfer set to report/transmit
 * \param reason the reason for the report (should be TASE2_REPORT_REASON_OPERATOR_REQUESTED or TASE2_REPORT_REASON_OTHER_EXTERNAL_EVENT)
 * \param eventCode the event code in case the reason is TASE2_REPORT_REASON_OTHER_EXTERNAL_EVENT
 *
 * \return true, when report has been sent, false otherwise (e.g. when client is not connected and/or TS not enabled)
 */
TASE2_API bool
Tase2_Server_requestReport(Tase2_Server self, Tase2_DSTransferSet transferSet, Tase2_ReportReason reason, int16_t eventCode);

/**
 * \brief Send information message to all clients that have the IM transfer set enabled and BLT matches the information object
 *
 * NOTE: when no BLT is defined the messages will be sent to all clients.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Server instance
 * \param domain use domain to send information object, NULL for VCC scope information message
 *
 * \return error code: 0 = no error, 1 = domain has no info buffer, 2 - message too large for info buffer
 */
TASE2_API int
Tase2_Server_sendInformationMessage(Tase2_Server self, Tase2_Domain domain, int32_t infoRef, int32_t localRef, int32_t msgId, int32_t size, uint8_t* buffer);

/**
 * \brief Starts the server (will start a background thread handling incoming connections)
 *
 * NOTE: This function calls \ref Tase2_Endpoint_connect
 *
 * \param self Tase2_Server instance
 */
TASE2_API void
Tase2_Server_start(Tase2_Server self);

/**
 * \brief Stops the server
 *
 * NOTE: This function calls \ref Tase2_Endpoint_disconnect
 *
 * \param self Tase2_Server instance
 */
TASE2_API void
Tase2_Server_stop(Tase2_Server self);

/**
 * \brief Get some communication statistics
 *
 * \param self Tase2_Server instance
 */
TASE2_API Tase2_Statistics
Tase2_Server_getStatistics(Tase2_Server self);

/**
 * \brief Checks if the server is running
 *
 * Can be used to check if the server is listening for incoming connections (passive mode) or is connected (active and passive mode)
 *
 * \param self Tase2_Server instance
 *
 * \return true, if server is running, otherwise false
 */
TASE2_API bool
Tase2_Server_isRunning(Tase2_Server self);

/**
 * \brief Update the online available value with the local value of the data point
 *
 * NOTE: This function may trigger server actions (like sending DS transfer set reports)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Server instance
 * \param dataPoint the data point instance to update
 */
TASE2_API void
Tase2_Server_updateOnlineValue(Tase2_Server self, Tase2_DataPoint dataPoint);

/**
 * \brief Update the local value by the online available value of the data point
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Server instance
 * \param dataPoint the data point instance to update
 */
TASE2_API void
Tase2_Server_updateByOnlineValue(Tase2_Server self, Tase2_DataPoint dataPoint);

/**
 * \brief Update the online available value with the local value of the data point (don't trigger transfer sets)
 *
 * This function can be used to implement application specific limit or deadband handling for analogue indication points.
 * Call this function when analogue value change doesn't violate a limit or deadband condition. In the case
 * the new analogue value violates such a condition, then call \ref Tase2_Server_updateOnlineValue instead.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Server instance
 * \param dataPoint the data point instance to update
 */
TASE2_API void
Tase2_Server_updateOnlineValueNoTrigger(Tase2_Server self, Tase2_DataPoint dataPoint);

/**
 * \brief Destroy the server instance (release all allocated resources)
 *
 * Should be called when the server is no longer needed.
 * NOTE: The server instance can no longer be used after calling this function.
 *
 * \param self Tase2_Server instance
 */
TASE2_API void
Tase2_Server_destroy(Tase2_Server self);

/*************************************************************************
 * Functions for bilateral table (BLT) management
 *************************************************************************/

/**
 * \defgroup tase2_server_blt Functions for bilateral table (BLT) management
 *
 * @{
 */

/**
 * \brief Create a new bilateral table (\ref Tase2_BilateralTable) instance
 *
 * NOTE: the bilateral table is associated with a client by the application reference (ap-title + ae-qualifier)
 *
 * \param bltId The bilateral table ID (should be unique). Can be accessed from the client by the "Bilateral_Table_ID" variable
 * \param domain The \ref Tase2_Domain that is associated with the client (only the associated client can access this domain)
 * \param apTitle the ap-title as string (point separated values e.g. "1.1.1.999")
 * \param aeQualifier the ae-qualifier value
 *
 * \return the new bilateral table instance
 */
TASE2_API Tase2_BilateralTable
Tase2_BilateralTable_create(const char* bltId, Tase2_Domain domain, const char* apTitle, int aeQualifier);

/**
 * \brief Set the ISO protocol layer addresses to associate the endpoint to this BLT
 *
 * NOTE: By default only AP-title and AE-qualifier are used to associate an endpoint
 * with the BLT. You can change this behavior with the \ref Tase2_Endpoint_setAddressMatchingRules
 * function. When using the ISO layer addresses you have to set them with this function.
 *
 * \param self The Tase2_BilateralTable instance
 *
 * \param pSelector presentation layer address
 * \param sSelector session layer address
 * \param tSelector ISO COTP layer address
 */
TASE2_API void
Tase2_BilateralTable_setIsoParameters(Tase2_BilateralTable self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector);

/**
 * \brief Add an indication point or protection equipment object to the BLT
 *
 * \param self The Tase2_BilateralTable instance
 * \param dataPoint the indication point or protection equipment object
 * \param allowRead allow read operation on this data point
 * \param allowWrite allow write operation on this data point
 */
TASE2_API void
Tase2_BilateralTable_addDataPoint(Tase2_BilateralTable self, Tase2_DataPoint dataPoint, bool allowRead, bool allowWrite);

/**
 * \brief Add a control point object to the BLT
 *
 * \param self The Tase2_BilateralTable instance
 * \param controlPoint the control point instance
 * \param checkBackId the check back ID used as SBO select return value
 * \param allowSetTag allow set tag operation on this control point
 * \param allowGetTag allow get tag operation on this control point
 * \param allowOperate allow operate operation on this control point
 * \param allowRead allow read operation on this control point
 */
TASE2_API void
Tase2_BilateralTable_addControlPoint(Tase2_BilateralTable self, Tase2_ControlPoint controlPoint, int16_t checkBackId, bool allowSetTag, bool allowGetTag, bool allowOperate, bool allowRead);

/**
 * \brief Add an information message object to the BLT
 *
 * \param self The Tase2_BilateralTable instance
 * \param infoRef information reference parameter (specifies application)
 * \param localRef local reference parameter
 * \param vccScope indicates if object has VCC scope (true) or ICC scope (false)
 */
TASE2_API void
Tase2_BilateralTable_addInformationMessage(Tase2_BilateralTable self, int32_t infoRef, int32_t localRef, bool vccScope);

/**
 * \brief Get the BLT ID
 *
 * \param self The Tase2_BilateralTable instance
 *
 * \return the BLT ID
 */
TASE2_API const char*
Tase2_BilateralTable_getID(Tase2_BilateralTable self);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif


#endif /* TASE2_SERVER_H_ */
