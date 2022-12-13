/*
 *  tase2_client.h
 *
 *  This file is part of libtase2
 *
 *  Copyright 2017-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef SRC_TASE2_CLIENT_TASE2_CLIENT_H_
#define SRC_TASE2_CLIENT_TASE2_CLIENT_H_

#include "tase2_common.h"
#include "tase2_endpoint.h"
#include "tls_config.h"
#include "linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup tase2_client Client side related API parts
 * @{
 */

typedef struct {
    int32_t major;
    int32_t minor;
} Tase2_Version;

/**
 * \brief Client side representation of a DS transfer set
 */
typedef struct sTase2_ClientDSTransferSet* Tase2_ClientDSTransferSet;

/** Connection state of the IedConnection instance (either idle, connected or closed) */
typedef enum
{
    TASE2_CLIENT_STATE_IDLE,
    TASE2_CLIENT_STATE_CONNECTED,
    TASE2_CLIENT_STATE_CLOSED
} Tase2_ClientState;

/**
 * \brief Client side representation of a data set
 */
typedef struct sTase2_ClientDataSet* Tase2_ClientDataSet;

/**
 * \brief Callback handler that is called for each data point contained in a received DS transfer set
 *
 * \param parameter a user provided parameter
 * \param transferSet the related transfer set object or NULL if the transfer set cannot be identified
 * \param domainName the domain name of the data point
 * \param pointName the variable name of the data point
 * \param pointValue the value of the data point
 */
typedef void
(*Tase2_DSTransferSetValueHandler) (void* parameter, Tase2_ClientDSTransferSet transferSet, const char* domainName, const char* pointName, Tase2_PointValue pointValue);

/**
 * \brief Callback handler that is called whenever a DS transfer set is received
 *
 * When used, this callback is called twice for each received DS transfer set report.
 * After the report is received the callback is initially called with the finished flag set to false.
 * Then each data point of the report is reported with a separate call to the \ref Tase2_DSTransferSetValueHandler
 * callback. When all data points have been reported the callback is invoked a second time with the
 * finished flag set to true.
 *
 * \param parameter a user provided parameter
 * \param finished false for the first invocation after a report is received, true when all data points are reported.
 * \param seq is a report sequence number that will be increased by one after each report received.
 * \param transferSet the related transfer set object
 *
 */
typedef void
(*Tase2_DSTransferSetReportHandler) (void* parameter, bool finished, uint32_t seq, Tase2_ClientDSTransferSet transferSet);

/**
 * \brief Callback handler for information messages (IM transfer set)
 *
 * \param parameter a user provided parameter
 * \param msgId the information message instance
 * \param size the size in bytes of the information message
 * \param message buffer where the information message is stored
 */
typedef void
(*Tase2_InformationMessageHandler) (void* parameter, int32_t infoRef, int32_t localRef, int32_t msgId, int32_t size, uint8_t* messageBuffer);


/**
 * \brief Callback handler that is called when the server closes the connection
 *
 * \param parameter a user provided parameter
 * \param connection the related client instance
 */
typedef void
(*Tase2_ConnectionClosedHandler) (void* parameter, Tase2_Client connection);

/**
 * \brief Callback handler that is called whenever the client state changes
 *
 * \param parameter a user provided parameter
 * \param client the related client instance
 * \param newState the new state
 */
typedef void
(*Tase2_Client_StateChangedHandler) (void* parameter, Tase2_Client client, Tase2_ClientState newState);


/**
 * \brief Read the data set directory from the server and create a new Tase2_ClientDataSet instance
 *
 * NOTE: A Tase2_ClientDataSet instance will only be created when the data set
 * was read successfully.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self
 * \param[out] err the error return code
 * \param domainName domain name of the data set, or NULL for VCC scope data sets
 * \param dataSetName the name of the data set
 *
 * \return new instance or NULL when the data set directory cannot be read
 */
TASE2_API Tase2_ClientDataSet
Tase2_Client_getDataSet(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* dataSetName);

/**
 * \brief  Request the server to create a new data set
 *
 * The data set element specifications are string of the format
 * "<domain name>/<data set name>"
 *
 * For VCC scope variable the domain name part and the '/' character has to be omitted.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self
 * \param[out] err the error return code
 * \param domainName domain name of the data set of NULL for a VCC scope data set
 * \param dataSetName name of the data set
 * \param dataSetElemens a list (see \ref LinkedList) with the data elements of the new data set
 *
 * \return true if data set has been created successfully, false otherwise
 */
TASE2_API bool
Tase2_Client_createDataSet(Tase2_Client self, Tase2_ClientError* err,
        const char* domainName, const char* dataSetName,
        LinkedList /* <char*> */dataSetElements);

/**
 * \brief Delete a data set
 *
 * This function requests the server to delete the specified data set.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self
 * \param[out] err the error return code
 * \param domainName domain name of the data set or NULL for a VCC scope data set
 * \param dataSetName name of the data set
 *
 * \return true if data set has been deleted, false otherwise
 */
TASE2_API bool
Tase2_Client_deleteDataSet(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* dataSetName);

/**
 * \brief Get the domain name of the data set
 *
 * \param self
 *
 * \return the domain name or NULL, for VCC scope data sets
 */
TASE2_API const char*
Tase2_ClientDataSet_getDomainName(Tase2_ClientDataSet self);

/**
 * \brief Get the name of the data set
 *
 * \return the data set name (only valid as long as the \ref Tase2_ClientDataSet instance exists)
 */
TASE2_API const char*
Tase2_ClientDataSet_getDataSetName(Tase2_ClientDataSet self);

/**
 * \brief Check if the data set can be deleted by the client
 *
 * \return true when data set can be deleted by the client, false otherwise
 */
TASE2_API bool
Tase2_ClientDataSet_isDeletable(Tase2_ClientDataSet self);

/**
 * \brief Get the size (number of entries) of the data set
 *
 * \return number of data set entries
 */
TASE2_API int
Tase2_ClientDataSet_getSize(Tase2_ClientDataSet self);

/**
 * \brief Get the \ref Tase2_PointName of the data set entry at the given index
 *
 * \param self the Tase2_ClientDataSet instance
 * \param index the index of the data set entry (starting with 0)
 *
 * \return the point name of the data set entry
 */
TASE2_API Tase2_PointName
Tase2_ClientDataSet_getPointName(Tase2_ClientDataSet self, int index);

/**
 * \brief Get the domain name of the data set entry at the given index
 *
 * \param self the Tase2_ClientDataSet instance
 * \param index the index of the data set entry (starting with 0)
 *
 * \return the domain name of the data set entry
 */
TASE2_API const char*
Tase2_ClientDataSet_getPointDomainName(Tase2_ClientDataSet self, int index);

/**
 * \brief Get the variable name of the data set entry at the given index
 *
 * \param self the Tase2_ClientDataSet instance
 * \param index the index of the data set entry (starting with 0)
 *
 * \return the variable name of the data set entry
 */
TASE2_API const char*
Tase2_ClientDataSet_getPointVariableName(Tase2_ClientDataSet self, int index);


/**
 * \brief Return a copy of the point value for the data set entry with the given index
 *
 * \param self the Tase2_ClientDataSet instance
 * \param index the index position of the data set entry (starting with 0)
 *
 * \return Return a copy of the data point value. Releasing the allocated memory is in the responsibility of the caller.
 */
TASE2_API Tase2_PointValue
Tase2_ClientDataSet_getPointValue(Tase2_ClientDataSet self, int index);

/**
 * \brief read the data set values from the server
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDataSet instance
 * \param client the connection to be used
 *
 * \return TASE2_CLIENT_ERROR_OK in case of success, or error code
 */
TASE2_API Tase2_ClientError
Tase2_ClientDataSet_read(Tase2_ClientDataSet self, Tase2_Client client);

/**
 * \brief Destroy the client data set instance and release all resources
 *
 * \param self the \ref Tase2_ClientDataSet instance
 */
TASE2_API void
Tase2_ClientDataSet_destroy(Tase2_ClientDataSet self);

/**
 * \brief Get the next available DS transfer set (GetNextDSTransferSet operation)
 *
 * \param self the \ref Tase2_Client instance
 * \param domainName the domain name of associated TASE.2 domain of the requesting client
 * \param[out] err pointer to Tase2_ClientError variable to store the error code of the service call
 */
TASE2_API Tase2_ClientDSTransferSet
Tase2_Client_getNextDSTransferSet(Tase2_Client self, const char* domainName, Tase2_ClientError* err);

/**
 * \brief Read the values of the DSTransferSet from the server
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the \ref Tase2_Client connection to use
 *
 * \return error code of the read service
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_readValues(Tase2_ClientDSTransferSet self, Tase2_Client client);

/**
 * \brief Write the buffered values of the DSTransferSet to the server
 *
 * NOTE: This function can only be used after calling the \ref Tase2_ClientDSTransferSet_readValues
 * function to get the current values of the transfer set in the server.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the \ref Tase2_Client connection to use
 *
 * \return error code of the write service
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeValues(Tase2_ClientDSTransferSet self, Tase2_Client client);

/**
 * \brief Get the domain name of the DSTransferSet
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 */
TASE2_API const char*
Tase2_ClientDSTransferSet_getDomain(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the name of the DSTransferSet
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 */
TASE2_API const char*
Tase2_ClientDSTransferSet_getName(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the domain name of the data set
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return the data set domain name string
 */
TASE2_API const char*
Tase2_ClientDSTransferSet_getDataSetDomain(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the name of the data set
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return the data set name string
 */
TASE2_API const char*
Tase2_ClientDSTransferSet_getDataSetName(Tase2_ClientDSTransferSet self);

/**
 * \brief Get \ref Tase2_ClientDataSet instance of the transfer set data set
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return the \ref Tase2_ClientDataSet instance of the transfer set
 */
TASE2_API Tase2_ClientDataSet
Tase2_ClientDSTransferSet_getDataSet(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the start time of report transmission
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return the transmission start time value in seconds since 01.01.1970 UTC
 */
TASE2_API int32_t
Tase2_ClientDSTransferSet_getStartTime(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the transmission interval of the report
 *
 * When the report time is set, then events are collected and reported
 * after the interval timeout.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return transmission interval for periodic transmission in seconds
 */
TASE2_API int32_t
Tase2_ClientDSTransferSet_getInterval(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the TLE (Time Limit for Execution) value
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return TLE value in seconds
 */
TASE2_API int32_t
Tase2_ClientDSTransferSet_getTLE(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the buffer time value
 *
 * The buffer time is the time between an object change and the
 * transmission of the report. During the buffer time events are
 * collected and can be transmitted with the same report. If the buffer
 * time is 0 then every event causes a report.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return buffer time value in seconds
 */
TASE2_API int32_t
Tase2_ClientDSTransferSet_getBufferTime(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the integrity check interval time
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return the integrity check interval time value in seconds
 */
TASE2_API int32_t
Tase2_ClientDSTransferSet_getIntegrityCheck(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the DSConditions to trigger a report transmission
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return the DSConditions value (\ref ds_conditions)
 */
TASE2_API int
Tase2_ClientDSTransferSet_getDSConditionsRequested(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the information if blocked data transfer is used
 *
 * NOTE: blocked data transfer (CB3) is not supported!
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return true when blocked data transfer is used, false otherwise
 */
TASE2_API bool
Tase2_ClientDSTransferSet_getBlockData(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the value of the critical flag
 *
 * The critical flag indicates that the client has to confirm the reception
 * of the critical report.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return true if critical is set, false otherwise
 */
TASE2_API bool
Tase2_ClientDSTransferSet_getCritical(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the value of the RBE (report by exception) flag
 *
 * When RBE is activated only the changed objects are reported. Otherwise all
 * objects in the transfer set data set are reported.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return true if RBE is enabled, false otherwise
 */
TASE2_API bool
Tase2_ClientDSTransferSet_getRBE(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the value of the AllChangesReported attribute
 *
 * This attributes indicates if all changes of an element during the buffer time are
 * reported (true), or only the last change (false).
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return true if all changes reported is enabled, false otherwise
 */
TASE2_API bool
Tase2_ClientDSTransferSet_getAllChangesReported(Tase2_ClientDSTransferSet self);

/**
 * \brief Get status of the transfer set
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return true when the transfer set (reporting) is enabled, false otherwise
 */
TASE2_API bool
Tase2_ClientDSTransferSet_getStatus(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the external event code that caused the transmission
 *
 * \return the external event code
 */
TASE2_API int16_t
Tase2_ClientDSTransferSet_getEventCodeRequested(Tase2_ClientDSTransferSet self);

/**
 * \brief Sets the name of the transfer set data set
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \param domainId domain name of the data set
 * \param dataSetid name of the data set
 */
TASE2_API void
Tase2_ClientDSTransferSet_setDataSetName(Tase2_ClientDSTransferSet self, const char* domainId, const char* dataSetId);

/**
 * \brief Associate a data set instance with the transfer set
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param dataSet the data set instance to use
 */
TASE2_API void
Tase2_ClientDSTransferSet_setDataSet(Tase2_ClientDSTransferSet self, Tase2_ClientDataSet dataSet);

/**
 * \brief Sets the start time of transfer set monitoring
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param startTime Start time as seconds since epoch.
 */
TASE2_API void
Tase2_ClientDSTransferSet_setStartTime(Tase2_ClientDSTransferSet self, int32_t startTime);

/**
 * \brief Sets the interval time for transfer set reports
 *
 * The interval time determines the time period for cyclic report transmission.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param interval the interval time in seconds.
 */
TASE2_API void
Tase2_ClientDSTransferSet_setInterval(Tase2_ClientDSTransferSet self, int32_t interval);

/**
 * \brief Sets the TLE (Time Limit for Execution) value.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param tle TLE value in seconds
 */
TASE2_API void
Tase2_ClientDSTransferSet_setTLE(Tase2_ClientDSTransferSet self, int32_t tle);

/**
 * \brief Sets the buffer time (wait time until report is sent)
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param bufferTime the buffer time value in seconds
 */
TASE2_API void
Tase2_ClientDSTransferSet_setBufferTime(Tase2_ClientDSTransferSet self, int32_t bufferTime);

/**
 * \brief Sets the integrity check interval time
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param integrityCheck the integrity check value in seconds.
 */
TASE2_API void
Tase2_ClientDSTransferSet_setIntegrityCheck(Tase2_ClientDSTransferSet self, int32_t integrityCheck);

/**
 * \brief Sets the trigger conditions for reports (\ref ds_conditions)
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param dsConditionRequested the trigger conditions (bitmask/sum of \ref ds_conditions)
 */
TASE2_API void
Tase2_ClientDSTransferSet_setDSConditionsRequested(Tase2_ClientDSTransferSet self, int dsConditionsRequested);

/**
 * \brief Enables/disables block data transfer
 *
 * The block data mode provides a compressed data format for transmission to save bandwidth.
 * NOTE: Should not be set to true! Block data transfer (CB3 is not supported by the library)
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param blockData true to enable block data transfer, false otherwise
 */
TASE2_API void
Tase2_ClientDSTransferSet_setBlockData(Tase2_ClientDSTransferSet self, bool blockData);

/**
 *  \brief Sets the critical flag.
 *
 *  NOTE: critical reports have to be acknowledged by the client.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param critical true, enable critical transfer report, false, otherwise
 */
TASE2_API void
Tase2_ClientDSTransferSet_setCritical(Tase2_ClientDSTransferSet self, bool critical);

/**
 * \brief Set the value of the RBE (report by exception) flag
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param rbe true, enable RBE, false, otherwise
 */
TASE2_API void
Tase2_ClientDSTransferSet_setRBE(Tase2_ClientDSTransferSet self, bool rbe);

/**
 * \brief Enable/disable transmission of all changes
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param allChangesReported when true, all changes of an object are reported, when false, changes that happen in the buffer time may not be reported
 */
TASE2_API void
Tase2_ClientDSTransferSet_setAllChangesReported(Tase2_ClientDSTransferSet self, bool allChangesReported);

/**
 * \brief Enable/disable the status of condition monitoring (and report transmission)
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param status true, enable reporting, false otherwise
 */
TASE2_API void
Tase2_ClientDSTransferSet_setStatus(Tase2_ClientDSTransferSet self, bool status);

/**
 * \brief Write the name of the transfer set data set (write immediately to server)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param domainId domain name of the data set
 * \param dataSetid name of the data set
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeDataSetName(Tase2_ClientDSTransferSet self, Tase2_Client client, const char* domainId, const char* dataSetId);

/**
 * \brief Sets the start time of transfer set monitoring (write immediately to server)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param startTime Start time as seconds since epoch.
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeStartTime(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t startTime);

/**
 * \brief Sets the interval time for transfer set reports  (write immediately to server)
 *
 * The interval time determines the time period for cyclic report transmission.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param interval the interval time in seconds.
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeInterval(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t interval);

/**
 * \brief Sets the TLE (Time Limit for Execution) value (write immediately to server).
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param tle TLE value in seconds
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeTLE(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t tle);

/**
 * \brief Sets the buffer time (wait time until report is sent) (write immediately to server)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param bufferTime the buffer time value in seconds
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeBufferTime(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t bufferTime);

/**
 * \brief Sets the integrity check interval time (write immediately to server)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param integrityCheck the integrity check value in seconds.
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeIntegrityCheck(Tase2_ClientDSTransferSet self, Tase2_Client client, int32_t integrityCheck);

/**
 * \brief Sets the trigger conditions for reports (\ref ds_conditions)  (write immediately to server)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param dsConditionRequested the trigger conditions (bitmask/sum of \ref ds_conditions)
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeDSConditionsRequested(Tase2_ClientDSTransferSet self, Tase2_Client client, int dsConditionsRequested);

/**
 * \brief Enables/disables block data transfer (write immediately to server)
 *
 * The block data mode provides a compressed data format for transmission to save bandwidth.
 * NOTE: Should not be set to true! Block data transfer (CB3 is not supported by the library)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param blockData true to enable block data transfer, false otherwise
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeBlockData(Tase2_ClientDSTransferSet self, Tase2_Client client, bool blockData);

/**
 * \brief Set the value of the critical flag (write immediately to server)
 *
 * The critical flag indicates that the client has to confirm the reception
 * of the critical report.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param critical true to set the critical flag, false otherwise
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeCritical(Tase2_ClientDSTransferSet self, Tase2_Client client, bool critical);

/**
 * \brief Write the value of the RBE (report by exception) flag (write immediately to server)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param rbe true, send reports when a value changes
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeRBE(Tase2_ClientDSTransferSet self, Tase2_Client client, bool rbe);

/**
 * \brief Enable/disable transmission of all changes (write immediately to server)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param allChangesReported when true, all changes are reported, when false, changes that happen in the buffer time may not be reported
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeAllChangesReported(Tase2_ClientDSTransferSet self, Tase2_Client client, bool allChangesReported);

/**
 * \brief Enable/disable the status of condition monitoring and report transmission (write immediately to server)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 * \param client the related client instance used to send the write message to the server
 * \param status true, enable reporting, false otherwise
 *
 * \return error code (TASE2_CLIENT_ERROR_OK in case of success)
 */
TASE2_API Tase2_ClientError
Tase2_ClientDSTransferSet_writeStatus(Tase2_ClientDSTransferSet self, Tase2_Client client, bool status);

/**
 * \brief Get the DSConditions Detected value from the last received report
 *
 * This represents an optional special data set variable. When part of the
 * transfer set data set this will be sent with reports.
 *
 * At the time of reporting, this attribute uniquely identifies the conditions that
 * caused the generation of the report.
 *
 * NOTE: The value may not be present in a report.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return DSConditions Detected value (reason code for transfer report)
 */
TASE2_API uint8_t
Tase2_ClientDSTransferSet_getDSConditionsDetected(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the event code detected value from the last received report
 *
 * This represents an optional special data set variable. When part of the
 * transfer set data set this will be sent with reports.
 *
 * \param self the \ref Tase2_ClientDSTransferSet instance
 *
 * \return the event code
 */
TASE2_API int16_t
Tase2_ClientDSTransferSet_getEventCodeDetected(Tase2_ClientDSTransferSet self);

/**
 * \brief Get the transfer set timestamp value from the last received report
 *
 * This represents an optional special data set variable. When part of the
 * transfer set data set this will be sent with reports.
 *
 * This attribute specifies the time the transfer report was generated at the
 * server.
 *
 * \return transfer set timestamp value in seconds since 01.01.1970 UTC
 */
TASE2_API int32_t
Tase2_ClientDSTransferSet_getTransferSetTimestamp(Tase2_ClientDSTransferSet self);

/**
 * \brief Destroy transfer set.
 *
 * NOTE: Do not call this function until the \ref Tase2_Client instance is either
 * destroyed or the client state (see \ref Tase2_Client_getState) changes to
 * TASE2_CLIENT_STATE_IDLE or TASE2_CLIENT_STATE_CLOSED. Otherwise the instance might
 * be called internally by \ref Tase2_Client and cause memory corruption!
 *
 * \param self the Tase2_ClientDSTransferSet instance
 */
TASE2_API void
Tase2_ClientDSTransferSet_destroy(Tase2_ClientDSTransferSet self);

/**
 * \brief Create a new TASE.2 client instance
 *
 * NOTE: The function will also create a new \ref Tase2_Endpoint instance
 *
 * \param tlsConfig TLS configuration object or NULL if TLS is not used
 *
 * \return the new TASE.2 client instances
 */
TASE2_API Tase2_Client
Tase2_Client_create(TLSConfiguration tlsConfig);

/**
 * \brief Create a new Tase2_Client instance using an existing endpoint
 *
 * \param endpoint the endpoint instance to be used by the client
 *
 * \return the new TASE.2 client instances
 */
TASE2_API Tase2_Client
Tase2_Client_createEx(Tase2_Endpoint endpoint);

/**
 * \brief Get the endpoint instance this client is assigned
 *
 * \param self Tase2_Client instance
 *
 * \return the assigned endpoint instance
 */
TASE2_API Tase2_Endpoint
Tase2_Client_getEndpoint(Tase2_Client self);

/**
 * \brief Destroy the client instance. Releases all resources.
 *
 * NOTE: do not use any of the Tase2_Client_ functions for this instance after this call.
 *
 * \param self Tase2_Client instance
 */
TASE2_API void
Tase2_Client_destroy(Tase2_Client self);

/**
 * \brief Access some statistical information for the client connection
 *
 * Statistical information comprises the number of sent and received messages and
 * the number of communication errors occurred.
 *
 * \param self Tase2_Client instance
 *
 * \return the statistical information
 */
TASE2_API Tase2_Statistics
Tase2_Client_getStatistics(Tase2_Client self);

/**
 * \brief Set own identity for the identity service
 *
 * \param self Tase2_Client instance
 * \param vendor the vendor name of the TASE.2 application
 * \param model the model name of the TASE.2 application
 * \param revision the revision string of the TASE.2 application
 */
TASE2_API void
Tase2_Client_setIdentity(Tase2_Client self, const char* vendor, const char* model, const char* revision);

/**
 * \brief Install the callback handler to get data points from received transfer set reports
 *
 * \param self Tase2_Client instance
 * \param handler the callback function
 * \param parameter a user provided context parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Client_installDSTransferSetValueHandler(Tase2_Client self, Tase2_DSTransferSetValueHandler handler, void* parameter);

/**
 * \brief Install the callback handler for received transfer set reports
 *
 * NOTE: The callback handler is invoked twice for each received report. Once directly after the report is received and
 * a second time after all values have been pushed to the application using the \ref Tase2_DSTransferSetValueHandler.
 *
 * \param self Tase2_Client instance
 * \param handler the callback function
 * \param parameter a user provided context parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Client_installDSTransferSetReportHandler(Tase2_Client self, Tase2_DSTransferSetReportHandler handler, void* parameter);

/**
 * \brief Install a handler function that will be called when the connection is lost.
 *
 * \param self Tase2_Client instance
 * \param handler the callback function
 * \param parameter a user provided context parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Client_installConnectionClosedHandler(Tase2_Client self, Tase2_ConnectionClosedHandler handler, void* parameter);

/**
 * \brief Install a handler that is called whenever the client state changes (e.g. client gets connected or disconnected)
 *
 * \param self Tase2_Client instance
 * \param handler the callback function
 * \param parameter a user provided context parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Client_installStateChangedHandler(Tase2_Client self, Tase2_Client_StateChangedHandler handler, void* parameter);

/**
 * \brief Change default remote TCP port used by this client
 *
 * \param self Tase2_Client instance
 * \param tcpPort the TCP port number of the remote endpoint
 */
TASE2_API void
Tase2_Client_setTcpPort(Tase2_Client self, int tcpPort);

/**
 * \brief Sets the remote application endpoint addresses
 *
 * \param self Tase2_Client instance
 * \param remoteAddress structure containing ap-title, AE-qualifier, and P-, S-, and T-selectors.
 */
TASE2_API void
Tase2_Client_setRemoteApplicationAddress(Tase2_Client self, Tase2_ApplicationAddress remoteAddress);

/**
 * \brief Set application endpoint identifier of remote endpoint (AP-title and AE-qualifier)
 *
 * NOTE: It is important to set this parameter correctly to match the remote endpoint identifer.
 * The format of the AP-title is a list of positive integer variables separated by '.' or ',' sign.
 * E.g. "1.1.1.999.1" or "1,1,1,999,2".
 *
 * If not used the configured remote application address from the \ref Tase2_Endpoint is used.
 *
 * \param self Tase2_Client instance
 * \param apTitle AP-title part of the remote application endpoint identifier
 * \param aeQualifier AE-qualifier part of the remote application endpoint identifier
 */
TASE2_API void
Tase2_Client_setRemoteApTitle(Tase2_Client self, const char* apTitle, int aeQualifier);

/**
 * \brief set remote addresses for the lower layers
 *
 * This function can be used to set the addresses for the lower layer protocols (presentation, session, and transport
 * layer). Calling this function is optional and not recommended. It might be required when the server expects specific
 * values for the parameters.
 *
 * If not used the configured remote application address from the \ref Tase2_Endpoint is used.
 *
 * \param self Tase2_Client instance
 * \param pSelector the P-Selector (presentation layer address)
 * \param sSelector the S-Selector (session layer address)
 * \param tSelector the T-Selector (ISO transport layer address)
 */
TASE2_API void
Tase2_Client_setRemoteAddresses(Tase2_Client self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector);

/**
 * \brief Set remote AP-invocation-ID (for active endpoint)
 *
 * \param self endpoint instance to use
 *
 * \param apInvocationId the new AP-invocation-ID value
 */
TASE2_API void
Tase2_Client_setRemoteApInvocationId(Tase2_Client self, int apInvocationId);

/**
 * \brief Set remote AE-invocation-ID (for active endpoint)
 *
 * \param self endpoint instance to use
 *
 * \param aeInvocationId the new AE-invocation-ID value
 */
TASE2_API void
Tase2_Client_setRemoteAeInvocationId(Tase2_Client self, int aeInvocationId);

/**
 * \brief Set application endpoint identifier of the local endpoint
 *
 * NOTE: It is important to set this parameter correctly to match the remotely configured parameter.
 * The format of the AP-title is a list of positive integer variables separated by '.' or ',' sign.
 * E.g. "1.1.1.999.1" or "1,1,1,999,2".
 *
 * \param self Tase2_Client instance
 * \param apTitle AP-title part of the local application endpoint identifier
 * \param aeQualifier AE-qualifier part of the local application endpoint identifier
 */
TASE2_API void
Tase2_Client_setLocalApTitle(Tase2_Client self, const char* apTitle, int aeQualifier);

/**
 * \brief set local addresses for the lower layers
 *
 * This function can be used to set the addresses for the lower layer protocols (presentation, session, and transport
 * layer). Calling this function is optional. It might be required when the peer expects specific
 * values for the parameters.
 *
 *  \param self Tase2_Client instance
 *  \param pSelector the P-Selector (presentation layer address)
 *  \param sSelector the S-Selector (session layer address)
 *  \param tSelector the T-Selector (ISO transport layer address)
 */
TASE2_API void
Tase2_Client_setLocalAddresses(Tase2_Client self, Tase2_PSelector pSelector, Tase2_SSelector sSelector, Tase2_TSelector tSelector);

/**
 * \brief Set local AP-invocation-ID
 *
 * \param self endpoint instance to use
 *
 * \param apInvocationId the new AP-invocation-ID value
 */
TASE2_API void
Tase2_Client_setLocalApInvocationId(Tase2_Client self, int apInvocationId);

/**
 * \brief Set local AE-invocation-ID
 *
 * \param self endpoint instance to use
 *
 * \param aeInvocationId the new AE-invocation-ID value
 */
TASE2_API void
Tase2_Client_setLocalAeInvocationId(Tase2_Client self, int aeInvocationId);

/**
 * \brief connect to server and collect basic server information
 *
 * \param self Tase2_Client instance
 * \param hostname hostname or IP address of the server
 * \param apTitle local (own) AP-title part of the application endpoint identifier
 * \param aeQualifier local (own) AE-qualifier part of the application endpoint identifier
 *
 * \return TASE2_CLIENT_ERROR_OK in case of success, otherwise error specific value
 */
TASE2_API Tase2_ClientError
Tase2_Client_connect(Tase2_Client self, const char* hostname, const char* apTitle, int aeQualifier);

/**
 * \brief Connect to server with default settings from endpoint
 *
 * NOTE: if already connected this function will only read the TASE2 version and supported features from
 * the peer server.
 *
 * \param self Tase2_Client instance
 *
 * \return TASE2_CLIENT_ERROR_OK in case of success, otherwise error specific value
 */
TASE2_API Tase2_ClientError
Tase2_Client_connectEx(Tase2_Client self);

/**
 * \brief disconnect from the server
 *
 * NOTE: This function will call \ref Tase2_Endpoint_disconnect on the underlying
 * endpoint. This means that other clients and servers connected to the endpoint will
 * also be disconnected!
 *
 * \param self Tase2_Client instance
 */
TASE2_API void
Tase2_Client_disconnect(Tase2_Client self);

/**
 * \brief Get the connection state of the connection
 *
 * \param self Tase2_Client instance
 *
 * \return one of IDLE, CONNECTED, CLOSED
 */
TASE2_API Tase2_ClientState
Tase2_Client_getState(Tase2_Client self);

/**
 * \brief return the bilateral table ID associated with a TASE.2 domain
 *
 * This function has to be called after the Tase2_Client_connect function. When the bilateral table ID
 * does not match with the configured one, the client has to close the connection.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 *
 * \param[out] err the error return code
 * \param[in] domainName the domain name
 *
 * \return the table ID as string. Memory is allocated by the function and has to be released by caller.
 */
TASE2_API char*
Tase2_Client_getBilateralTableId(Tase2_Client self, Tase2_ClientError* err, const char* domainName);

/**
 * \brief Get the TASE.2 version reported by the server
 *
 * \param self Tase2_Client instance
 *
 * \return structure containing major and minor version numbers
 */
TASE2_API Tase2_Version
Tase2_Client_getServerVersion(Tase2_Client self);

/**
 * \brief Get the major version part of the server version
 *
 * \param self Tase2_Client instance
 *
 * \return major version number
 */
TASE2_API int
Tase2_Client_getServerVersionMajor(Tase2_Client self);

/**
 * \brief Get the minor version part of the server version
 *
 * \param self Tase2_Client instance
 *
 * \return minor version number
 */
TASE2_API int
Tase2_Client_getServerVersionMinor(Tase2_Client self);

/**
 * \brief Get a bit set value representing the support conformance blocks (CB) of the server.
 *
 * \param self Tase2_Client instance
 *
 * \return get the supported features reported by the server (bit mask)
 */
TASE2_API int
Tase2_Client_getSupportedFeatures(Tase2_Client self);

/**
 * \brief Get the identity of the peer
 *
 * NOTE: The returned strings have to be released by the caller (with \ref Tase2_FreeMemory )!
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 * \param[out] vendor the vendor name of the peer application
 * \param[out] model the model name of the peer application
 * \param[out] revision the revision string of the peer application
 */
TASE2_API void
Tase2_Client_readPeerIdentity(Tase2_Client self, Tase2_ClientError* err, char** vendor, char** model, char** revision);

/***************************************************************
 * Functions for server data model discovery
 ***************************************************************/

/**
 * \brief Get the list of VCC scope variables
 *
 * NOTE: The returned list is in the responsibility of the caller and can
 * be deleted with \ref LinkedList_destroy
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 *
 * \return list of variable name strings
 */
TASE2_API LinkedList /* <char*> */
Tase2_Client_getVCCVariables(Tase2_Client self, Tase2_ClientError* err);

/**
 * \brief Get the list of VCC scope data sets
 *
 * NOTE: The returned list is in the responsibility of the caller and can
 * be deleted with \ref LinkedList_destroy
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 *
 * \return list of data set name strings
 */
TASE2_API LinkedList /* <char*> */
Tase2_Client_getVCCDataSets(Tase2_Client self, Tase2_ClientError* err);

/**
 * \brief Get the list of available domains
 *
 * NOTE: The returned list is in the responsibility of the caller and can
 * be deleted with \ref LinkedList_destroy
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 *
 * \return list of domain name strings
 */
TASE2_API LinkedList /* <char*> */
Tase2_Client_getDomainNames(Tase2_Client self, Tase2_ClientError* err);

/**
 * \brief Get the list of domain scope variables
 *
 * NOTE: The returned list is in the responsibility of the caller and can
 * be deleted with \ref LinkedList_destroy
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param domainName domain name
 * \param[out] err the error return code
 *
 * \return list of variable name strings
 */
TASE2_API LinkedList /* <char*> */
Tase2_Client_getDomainVariables(Tase2_Client self, const char* domainName, Tase2_ClientError* err);

/**
 * \brief Get the list of domain scope data sets
 *
 * NOTE: The returned list is in the responsibility of the caller and can
 * be deleted with \ref LinkedList_destroy
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param domainName domain name
 * \param[out] err the error return code
 *
 * \return list of data set name strings
 */
TASE2_API LinkedList /* <char*> */
Tase2_Client_getDomainDataSets(Tase2_Client self, const char* domainName, Tase2_ClientError* err);


/**
 * \brief A data set entry specification
 */
typedef struct
{
    /**
     * domain name of the data set entry or NULL for VMD scope data set entry
     */
    const char* domainId;

    /**
     * variable (item) name of the data set entry
     */
    const char* itemId;

    /**
     * Array index of the data set entry (-1 --> no index present / ignore index)
     */
    int32_t arrayIndex;

    /**
     * the component name in case of a sub variable
     */
    const char* componentName;
}* Tase2_DataSetEntrySpec;

/**
 * \brief Get the domain ID of the data set entry
 */
TASE2_API const char*
Tase2_DataSetEntrySpec_getDomainId(Tase2_DataSetEntrySpec self);

/**
 * \brief Get the item ID of the data set entry
 */
TASE2_API const char*
Tase2_DataSetEntrySpec_getItemId(Tase2_DataSetEntrySpec self);

/**
 * \brief Delete a data set entry specification
 */
TASE2_API void
Tase2_DataSetEntrySpec_destroy(Tase2_DataSetEntrySpec self);

/**
 * \brief Read a data set directory
 *
 * @deprecated This function is deprecated since 1.1. please use \ref Tase2_Client_getDataSet instead
 *
 * NOTE: The returned list is in the responsibility of the caller. The data set entries have to be release with
 *       the \ref Tase2_DataSetEntrySpec_destroy function. The complete list can be deleted with the
 *       \ref LinkedList_destroyDeep function with \ref Tase2_DataSetEntrySpec_destroy as second argument.
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param domainName the domain name of the data set or NULL for VMD scope data set
 * \param dataSetName the name of the data set
 * \param[out] err the error return code
 *
 * \return a list of data set entries (\ref Tase2_DataSetEntrySpec).
 */
TASE2_API LinkedList /* <Tase2_DataSetEntrySpec> */
Tase2_Client_getDataSetDirectory(Tase2_Client self, const char* domainName, const char* dataSetName, Tase2_ClientError* err);



/***************************************************************
 * Functions for control point handling
 ***************************************************************/


/**
 * \brief Select a device (command or setpoint) for SBO control mode
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 *
 * \return check back name value
 */
TASE2_API int
Tase2_Client_selectDevice(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName);

/**
 * \brief Set tag operation. Set a tag value and a reason message for a device (control point).
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 * \param tagValue the new tag value
 * \param reason a text string to specify the reason of the tag
 *
 * \return true when set tag was successful, false otherwise
 */
TASE2_API bool
Tase2_Client_setTag(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName, Tase2_TagValue tagValue, const char* reason);

/**
 * \brief Read the tag
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 * \param domainName the domain name of the tag or NULL if the tag has VCC scope
 * \param deviceName the point name of the tag
 * \param[out] device is selected (armed) - only valid for SBO control type
 * \param[out] reasonBuf user provided buffer to store the reason text of the tag
 * \param reasonBufSize size of the user provided buffer to store the reason text of the tag
 *
 * \return the tag value
 */
TASE2_API Tase2_TagValue
Tase2_Client_getTag(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName, bool* isArmed, char* reasonBuf, int reasonBufSize);

/**
 * \brief Send a command to a device
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 * \param domainName the domain name of the device or NULL if the tag has VCC scope
 * \param deviceName the point name of the device
 * \param commandValue the command value
 *
 * \return true if the command has been accepted, false otherwise
 */
TASE2_API bool
Tase2_Client_sendCommand(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName, int16_t commandValue);

/**
 * \brief Send a discrete set point to a device
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 * \param domainName the domain name of the device or NULL if the tag has VCC scope
 * \param deviceName the point name of the device
 */
TASE2_API bool
Tase2_Client_sendDiscreteSetPoint(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName, int16_t setPointValue);

/**
 * \brief Send a "real" (float) set point to a device
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 * \param domainName the domain name of the device or NULL if the tag has VCC scope
 * \param deviceName the point name of the device
 */
TASE2_API bool
Tase2_Client_sendRealSetPoint(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* deviceName, float setPointValue);


/******************************************************************
 * Functions for protection events and indication point handling
 ******************************************************************/

/**
 * \brief Read a data point (indication point or protection equipment) from the server
 *
 * NOTE: the returned point value has to be released by the caller (using \ref Tase2_PointValue_destroy)
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 * \param domainName the domain name of the data point or NULL if the data point has VCC scope
 * \param pointName the point name of the data point
 *
 * \return The point value, or NULL if the read has not been successful.
 */
TASE2_API Tase2_PointValue
Tase2_Client_readPointValue(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* pointName);

/**
 * \brief Read multiple data points from the server
 *
 *  NOTE: the returned list has to be released by the caller with \ref LinkedList_destroyDeep using \ref Tase2_PointValue_destroy
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 * \param domainName the domain name of the data point or NULL if the data point has VCC scope
 * \param pointNames list with the data point names as C strings (char*)
 *
 * \return a list of point values, or NULL if the read has not been successful.
 */
TASE2_API LinkedList /* <Tase2_PointValue> */
Tase2_Client_readMultiplePointValues(Tase2_Client self, Tase2_ClientError* err, const char* domainName, LinkedList pointNames);

/**
 * \brief Write a data point (indication point or protection equipment) value to the server
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param self Tase2_Client instance
 * \param[out] err the error return code
 * \param domainName the domain name of the data point or NULL if the data point has VCC scope
 * \param pointName the point name of the data point
 * \param value the new value to write
 *
 */
TASE2_API void
Tase2_Client_writePointValue(Tase2_Client self, Tase2_ClientError* err, const char* domainName, const char* pointName, Tase2_PointValue value);

/******************************************************************
 * Functions for information message transfer set handling
 ******************************************************************/


/**
 * \brief Enable information message transfer set
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param[out] err pointer to Tase2_ClientError variable to store the error code of the service call
 */
TASE2_API void
Tase2_Client_IMTransferSetEnable(Tase2_Client self, Tase2_ClientError* err);

/**
 * \brief Disable information message transfer set
 *
 * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function inside library
 * callbacks can cause deadlock situations.
 *
 * \param[out] err pointer to Tase2_ClientError variable to store the error code of the service call
 */
TASE2_API void
Tase2_Client_IMTransferSetDisable(Tase2_Client self, Tase2_ClientError* err);

/**
 * \brief Install callback handler for information messages (IM transfer set)
 *
 * \param handler the callback function
 * \param parameter a user provided context parameter that is passed to the callback handler
 */
TASE2_API void
Tase2_Client_installIMTransferSetHandler(Tase2_Client self, Tase2_InformationMessageHandler handler, void* parameter);

/** @} */

/**
 * \brief Release memory allocated by the library (intended to be used by API wrappers)
 *
 * \param pointer to the allocated memory
 */
TASE2_API void
Tase2_FreeMemory(void* ptr);

#ifdef __cplusplus
}
#endif

#endif /* SRC_TASE2_CLIENT_TASE2_CLIENT_H_ */
