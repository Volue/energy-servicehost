/*
 *  mms_client.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_CLIENT_CONNECTION_H_
#define MMS_CLIENT_CONNECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup mms_client_api_group MMS client API (for IEC 61850 use IEC 61850 client API instead!)
 */
/**@{*/

#include "libmms_common_api.h"
#include "mms_endpoint.h"
#include "iso_connection_parameters.h"
#include "linked_list.h"
#include "tls_config.h"

typedef struct {
    char* vendorName;
    char* modelName;
    char* revision;
} MmsServerIdentity;

/** Handler call indicates the first element of an information report containing multiple named variables */
#define MMS_INFO_HANDLER_REPORT_START          1

/** Handler call indicated the last element of an information report containing multiple named variables */
#define MMS_INFO_HANDLER_REPORT_END            2

/** Information report contains a single named variable list */
#define MMS_INFO_HANDLER_IS_VARIABLE_LIST_NAME 4

//typedef void (*MmsInformationReportHandler) (void* parameter, char* domainName,
//        char* variableListName, MmsValue* value, uint8_t flags);

/**
 * Opaque handle for MMS client connection instance.
 */
typedef struct sMmsConnection* MmsConnection;


/*******************************************************************************
 * MmsClient
 *
 * TODO move to own header file!
 *
 * - remote application address is used to match the server endpoint in passive (and active mode)
 * - it is possible to specify which remote address parameters are used to match/accept the peer endpoint
 *      (see MmsClient_setRemoteAddressMatchingRules). Default: Only apTitle has to match!
 * - local application address (per default is the address of the local endpoint)
 *
 *******************************************************************************/

/**
 * \brief Create a new MmsClient instance
 *
 * \return the newly created instance.
 */
LIBMMS_API MmsClient
MmsClient_create(void);

LIBMMS_API void
MmsClient_destroy(MmsClient self);

/**
* \brief Get the current remote address information
*/
LIBMMS_API IsoApplicationAddress
MmsClient_getRemoteApplicationAddress(MmsClient self);

LIBMMS_API void
MmsClient_setRemoteApplicationAddress(MmsClient self, IsoApplicationAddress address);

LIBMMS_API void
MmsClient_setRemoteAddressMatchingRules(MmsClient self,
	bool matchApTitle,
	bool matchAeQualifier,
	bool matchPSelector,
	bool matchSSelector,
	bool matchTSelector);

/**
* \brief Indicates if the MMS client is connected to an MMS server
*/
LIBMMS_API bool
MmsClient_isConnected(MmsClient self);

///**
// * \brief Create a new secure (TLS enabled) MmsConnection instance
// *
// * \param tlsConfig TLS configuration parameters and certificates
// *
// * \return the newly created instance.
// */
//LIBMMS_API MmsConnection
//MmsConnection_createSecure(TLSConfiguration tlsConfig);
//
///**
// * \brief Create a new MmsConnection instance configured for non-threaded mode
// *
// * NOTE: This constructor doesn't create a background thread for connection handling.
// * The user has to call the MmsConnection_tick function periodically to ensure that
// * the MMS connection can be handled properly.
// *
// * \param tlsConfig TLS configuration parameters and certificates or NULL for non-TLS mode.
// *
// * \return  the newly created instance.
// */
//LIBMMS_API MmsConnection
//MmsConnection_createNonThreaded(TLSConfiguration tlsConfig);

/**
 * \brief Install a handler function for MMS information reports (unsolicited messages from the server).
 *
 * The handler function will be called whenever the client receives an MMS information report message.
 * Note that the API user is responsible to properly free the passed MmsValue object.
 *
 * \param self MmsConnection instance to operate on
 * \param handler the handler function to install for this client connection
 * \param parameter a user specified parameter that will be passed to the handler function on each
 *        invocation.
 */
//LIBMMS_API void
//MmsClient_setInformationReportHandler(MmsClient self, MmsInformationReportHandler handler,
//        void* parameter);

MmsError
MmsClient_sendInformationReportListOfVariables(MmsClient self,
        LinkedList /* MmsVariableAccessSpecification */ variableAccessDeclarations,
        LinkedList /* MmsValue */ values);

LIBMMS_API void
MmsClient_setEndpointConnection(MmsClient self, MmsEndpointConnection connection);

LIBMMS_API MmsEndpointConnection
MmsClient_getEndpointConnection(MmsClient self);

LIBMMS_API IsoApplicationAddress
MmsClient_getApplicationAddress(MmsClient self);

//typedef void (*MmsConnectionStateChangedHandler) (MmsConnection connection, void* parameter, MmsConnectionState newState);

/**
 * \brief User provided handler function that will be called if the connection to the server is lost
 *
 * \param connection MmsConnection object of the lost connection
 * \param parameter user provided parameter.
 */
typedef void (*MmsConnectionLostHandler) (MmsConnection connection, void* parameter);

/**
 * \brief Destroy an MmsConnection instance and release all resources
 *
 * \param self MmsConnection instance to operate on
 */
LIBMMS_API void
MmsConnection_destroy(MmsConnection self);

/**
 * \brief Get a statistic information value
 *
 * \param statisticValueId id of the statistical value to retrieve
 */
//LIBMMS_API uint64_t
//MmsConnection_getStatistics(MmsConnection self, int statisticValueId);

/*******************************************************************************
 * Blocking functions for connection establishment and data access
 *******************************************************************************/


/**
 * \brief Connect to an MMS server.
 *
 * This will open a new TCP connection and send a MMS initiate request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param serverName hostname or IP address of the server to connect
 * \param serverPort TCP port number of the server to connect or -1 to use default port (102 for MMS or 3872 for MMS over TLS)
 *
 * \return true on success. false if the connection attempt failed.
 */
//LIBMMS_API bool
//MmsConnection_connect(MmsConnection self, MmsError* mmsError, const char* serverName, int serverPort);
//
//LIBMMS_API void
//MmsConnection_connectAsync(MmsConnection self, MmsError* mmsError, const char* serverName, int serverPort);


// return value indicates that connection is currently waiting and calling thread can be suspended

/**
 * \brief Call MmsConnection state machine and connection handling code (for non-threaded mode only)
 *
 * This function has to be called periodically by the user application in non-threaded mode.
 *
 * \return true when connection is currently waiting and calling thread can be suspended, false means
 *         connection is busy and the tick function should be called again as soon as possible.
 */
//LIBMMS_API bool
//MmsConnection_tick(MmsConnection self);

/**
 * \brief Close the connection - not recommended
 *
 * This service simply closes the TCP socket without any hand-shaking with the server.
 * This behavior is not specified. Use with care!
 *
 * \param self MmsConnection instance to operate on
 */
//LIBMMS_API void
//MmsConnection_close(MmsConnection self);

//typedef void
//(*MmsClient_ConcludeAbortHandler) (void* parameter, MmsError mmsError, bool success);

/**
 * \brief Uses the MMS/ACSE abort service to close the connection to the server
 *
 * This service should be used to abruptly interrupt the connection to the server. It is not quite clear what the
 * benefit of this service is (simply closing the TCP connection should do the same). Though it is required by
 * conformance tests. In case the server doesn't close the connection after the internal timeout interval the
 * client will close the TCP connection and set mmsError to MMS_ERROR_SERVICE_TIMEOUT.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 */
//LIBMMS_API void
//MmsConnection_abort(MmsConnection self, MmsError* mmsError);
//
//LIBMMS_API void
//MmsConnection_abortAsync(MmsConnection self, MmsError* mmsError);

/**
 * \brief Uses the MMS conclude service to close the connection to the server
 *
 * This should be used to orderly release the connection to the server. If the server denies the conclude
 * request (by sending a concludeError PDU) this service fails with an error (mmsError set accordingly) and
 * the connection remains open. In this case the close or abort methods have to be used to close the connection.
 * It is not quite clear if this service is really useful but it is required by conformance tests.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 */
//LIBMMS_API void
//MmsConnection_conclude(MmsConnection self, MmsError* mmsError);
//
//LIBMMS_API void
//MmsConnection_concludeAsync(MmsConnection self, MmsError* mmsError, MmsConnection_ConcludeAbortHandler handler, void* parameter);

typedef void
(*MmsClient_GenericServiceHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, bool success);

typedef void
(*MmsClient_GetNameListHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, LinkedList nameList, bool moreFollows);

/**
 * \brief Get the names of all VMD scope variables of the server.
 *
 * This will result in a VMD specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 *
 * \return the of VMD specific variable names or NULL if the request failed.
 */
LIBMMS_API LinkedList /* <char*> */
MmsClient_getVMDVariableNames(MmsClient self, MmsError* mmsError);

LIBMMS_API uint32_t
MmsClient_getVMDVariableNamesAsync(MmsClient self, MmsError* mmsError, const char* continueAfter,
        MmsClient_GetNameListHandler handler, void* parameter);

/**
 * \brief Get the domains names for all domains of the server.
 *
 * This will result in a VMD specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variaextern "C" {ble to store error code
 *
 * \return the list of domain names or NULL if the request failed.
 *
 */
LIBMMS_API LinkedList /* <char*> */
MmsClient_getDomainNames(MmsClient self, MmsError* mmsError);

LIBMMS_API uint32_t
MmsClient_getDomainNamesAsync(MmsClient self, MmsError* mmsError, const char* continueAfter, LinkedList result,
        MmsClient_GetNameListHandler handler, void* parameter);

/**
 * \brief Get the names of all variables present in a MMS domain of the server.
 *
 * This will result in a domain specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name for the domain specific request
 *
 * \return the of domain specific variable names or NULL if the request failed.
 */
LIBMMS_API LinkedList /* <char*> */
MmsClient_getDomainVariableNames(MmsClient self, MmsError* mmsError, const char* domainId);

LIBMMS_API uint32_t
MmsClient_getDomainVariableNamesAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* continueAfter, LinkedList result, MmsClient_GetNameListHandler handler, void* parameter);

/**
 * \brief Get the names of all named variable lists present in a MMS domain or VMD scope of the server.
 *
 * This will result in a domain specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name for the domain specific request or NULL for a VMD scope request
 *
 * \return the domain specific named variable list names or NULL if the request failed.
 */
LIBMMS_API LinkedList /* <char*> */
MmsClient_getDomainVariableListNames(MmsClient self, MmsError* mmsError, const char* domainId);

LIBMMS_API uint32_t
MmsClient_getDomainVariableListNamesAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* continueAfter, LinkedList result, MmsClient_GetNameListHandler handler, void* parameter);

/**
 * \brief Get the names of all journals present in a MMS domain of the server
 *
 * This will result in a domain specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name for the domain specific request
 *
 * \return the domain specific journal names or NULL if the request failed.
 */
LIBMMS_API LinkedList /* <char*> */
MmsClient_getDomainJournals(MmsClient self, MmsError* mmsError, const char* domainId);

LIBMMS_API uint32_t
MmsClient_getDomainJournalsAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* continueAfter, MmsClient_GetNameListHandler handler, void* parameter);

/**
 * \brief Get the names of all named variable lists associated with this client connection.
 *
 * This will result in an association specific GetNameList request.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 *
 * \return the association specific named variable list names or NULL if the request failed.
 */
LIBMMS_API LinkedList /* <char*> */
MmsClient_getVariableListNamesAssociationSpecific(MmsClient self, MmsError* mmsError);

LIBMMS_API uint32_t
MmsClient_getVariableListNamesAssociationSpecificAsync(MmsClient self, MmsError* mmsError,
        const char* continueAfter, MmsClient_GetNameListHandler handler, void* parameter);


/**
 * \brief Read a single variable from the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be read or NULL to read a VMD specific named variable
 * \param itemId name of the variable to be read
 * \param specificationWithResult will set the specification-with-result flag in the request
 *
 * \return Returns a MmsValue object or NULL if the request failed. The MmsValue object can
 * either be a simple value or a complex value or array. It is also possible that the return value is NULL
 * even if mmsError = MMS_ERROR_NON. This is the case when the servers returns an empty result list.
 */
LIBMMS_API MmsValue*
MmsClient_readVariable(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId, bool specificationWithResult);


typedef void
(*MmsClient_ReadVariableHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, MmsValue* value);

/**
 * \brief Read a single variable from the server (asynchronous version)
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be read or NULL to read a VMD specific named variable
 * \param itemId name of the variable to be read
 * \param specificationWithResult will set the specification-with-result flag in the request
 *
 * \return invoke ID of the request when the request was sent successfully
 */
LIBMMS_API uint32_t
MmsClient_readVariableAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId, bool specificationWithResult,
        MmsClient_ReadVariableHandler handler, void* parameter);

/**
 * \brief Read one or more elements of a single array variable from the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be read
 * \param itemId name of the variable to be read
 * \param startIndex index of element to read or start index if a element range is to be read
 * \param numberOfElements Number of elements to read or 0 if a single element is to be read
 *
 * \return Returns a MmsValue object or NULL if the request failed. The MmsValue object is either
 * a simple or complex type if numberOfElements is 0, or an array containing the selected
 * array elements of numberOfElements > 0.
 */
LIBMMS_API MmsValue*
MmsClient_readArrayElements(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
		uint32_t startIndex, uint32_t numberOfElements);

/**
 * \brief Read one or more elements of a single array variable from the server (asynchronous version)
 *
 * NOTE: The MmsValue object received by the callback function is either a simple or complex type if numberOfElements is 0, or an array
 * containing the selected array elements of numberOfElements > 0.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be read
 * \param itemId name of the variable to be read
 * \param startIndex index of element to read or start index if a element range is to be read
 * \param numberOfElements Number of elements to read or 0 if a single element is to be read
 *
 * \return invoke ID of the request when the request was sent successfully
 */
LIBMMS_API uint32_t
MmsClient_readArrayElementsAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        uint32_t startIndex, uint32_t numberOfElements,
        MmsClient_ReadVariableHandler handler, void* parameter);


/**
 * \brief Read a single element (with optional component specification) from the server
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be read
 * \param itemId name of the variable to be read
 * \param index array element index
 * \param componentId array element component name
 *
 * \return Returns a MmsValue object or NULL if the request failed.
 */
LIBMMS_API MmsValue*
MmsConnection_readSingleArrayElementWithComponent(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, uint32_t index, const char* componentId);

LIBMMS_API uint32_t
MmsConnection_readSingleArrayElementWithComponentAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId,
        uint32_t index, const char* componentId,
        MmsClient_ReadVariableHandler handler, void* parameter);

/**
 * \brief Read multiple variables of a domain from the server with one request message.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the requested variables.
 * \param items: LinkedList<char*> is the list of item IDs of the requested variables.
 * \param specificationWithResult will set the specification-with-result flag in the request
 *
 * \return  Returns a MmsValue object or NULL if the request failed. The MmsValue object is
 * is of type MMS_ARRAY and contains the variable values of simple or complex type
 * in the order as they appeared in the item ID list.
 */
LIBMMS_API MmsValue*
MmsClient_readMultipleVariables(MmsClient self, MmsError* mmsError, const char* domainId,
		LinkedList /*<char*>*/ items, bool specificationWithResult);

LIBMMS_API uint32_t
MmsClient_readMultipleVariablesAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, LinkedList /*<char*>*/items, bool specificationWithResult,
        MmsClient_ReadVariableHandler handler, void* parameter);

/**
 * \brief Write a single variable to the server.
 *
 * NOTE: added return value in version 1.1
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be written
 * \param itemId name of the variable to be written
 * \param value value of the variable to be written
 *
 * \return when successful, the data access error value returned by the server
 */
LIBMMS_API MmsDataAccessError
MmsClient_writeVariable(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, MmsValue* value);

typedef void
(*MmsConnection_WriteVariableHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, MmsDataAccessError accessError);

LIBMMS_API uint32_t
MmsClient_writeVariableAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, MmsValue* value,
        MmsConnection_WriteVariableHandler handler, void* parameter);

/**
 * \brief Write a single association specific variable to the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param itemId name of the variable to be written
 * \param value value of the variable to be written
 *
 * \return when successful, the data access error value returned by the server
 */
LIBMMS_API MmsDataAccessError
MmsClient_writeVariableAASpecific(MmsClient self, MmsError* mmsError,
        const char* itemId, MmsValue* value);

/**
 * \brief Write a single association specific variable to the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param itemId name of the variable to be written
 * \param value value of the variable to be written
 *
 * \return when successful, the data access error value returned by the server
 */
LIBMMS_API uint32_t
MmsClient_writeVariableAASpecificAsync(MmsClient self, MmsError* mmsError,
        const char* itemId, MmsValue* value,
        MmsConnection_WriteVariableHandler handler, void* parameter);

/**
 * \brief Write a component of a single named variable to the server
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be written
 * \param itemId name of the variable to be written
 * \param componentName name of the component
 * \param value value of the variable component to be written
 *
 * \return when successful, the data access error value returned by the server
 */
LIBMMS_API MmsDataAccessError
MmsClient_writeVariableComponent(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, const char* componentName,
        MmsValue* value);

LIBMMS_API uint32_t
MmsClient_writeVariableComponentAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, const char* componentName,
        MmsValue* value,
        MmsConnection_WriteVariableHandler handler, void* parameter);

/**
 * \brief Write a single array element or a sub array to an array type variable
 *
 *  When a single array element is addressed the MmsValue object value has to be of the type
 *  of the array elements. When multiple array elements have to be written (index range) the
 *  MmsValue object value has to be of type MMS_ARRAY containing "numberOfElements" elements.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable to be written
 * \param index the index of the array element or the start index of a index range
 * \param numberOfElements the number of array elements to write starting with index. If 0 only one array element is written.
 * \param itemId name of the variable to be written
 * \param value value of the array element(s) to be written. Has to be of the type of
 *        the array elements or of type MMS_ARRAY when it is a sub array (index range)
 *
 * \return when successful, the data access error value returned by the server
 */
LIBMMS_API MmsDataAccessError
MmsClient_writeArrayElements(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, int index, int numberOfElements,
        MmsValue* value);

LIBMMS_API uint32_t
MmsClient_writeArrayElementsAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId, int index, int numberOfElements,
        MmsValue* value,
        MmsConnection_WriteVariableHandler handler, void* parameter);


typedef void
(*MmsClient_WriteMultipleVariablesHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, LinkedList /* <MmsValue*> */ accessResults);


/**
 * \brief Write multiple variables to the server.
 *
 * This function will write multiple variables to the server.
 *
 * The parameter accessResults is a pointer to a LinkedList reference. The methods will create a new LinkedList
 * object that contains the AccessResults of the single variable write attempts. It is up to the user to free this
 * objects properly (e.g. with LinkedList_destroyDeep(accessResults, MmsValue_delete)).
 *
 * \param[in] self MmsConnection instance to operate on
 * \param[out] mmsError user provided variable to store error code
 * \param[in] domainId the common domain name of all variables to be written
 * \param[in] items a linked list containing the names of the variables to be written. The names are C strings.
 * \param[out] values values of the variables to be written
 * \param[out] the MmsValue objects of type MMS_DATA_ACCESS_ERROR representing the write success of a single variable
 *        write.
 */
LIBMMS_API void
MmsClient_writeMultipleVariables(MmsClient self, MmsError* mmsError, const char* domainId,
        LinkedList /*<char*>*/ items, LinkedList /* <MmsValue*> */ values,
        LinkedList* /* <MmsValue*> */ accessResults);

LIBMMS_API uint32_t
MmsClient_writeMultipleVariablesAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        LinkedList /*<char*>*/ items, LinkedList /* <MmsValue*> */ values,
        MmsClient_WriteMultipleVariablesHandler handler, void* parameter);

/**
 * \brief Write named variable list values to the server.
 *
 * The parameter accessResults is a pointer to a LinkedList reference. The methods will create a new LinkedList
 * object that contains the AccessResults of the single variable write attempts. It is in the responsibility of
 * the user to free this objects properly (e.g. with LinkedList_destroyDeep(accessResults, MmsValue_delete)).
 * If accessResult is the to NULL the result will not be stored.
 *
 * \param[in] self MmsConnection instance to operate on
 * \param[out] mmsError user provided variable to store error code
 * \param[in] isAssociationSpecifc true if the named variable list is an association specific named variable list
 * \param[in] domainId the common domain name of all variables to be written
 * \param[out] values values of the variables to be written
 * \param[out] the MmsValue objects of type MMS_DATA_ACCESS_ERROR representing the write success of a single variable
 *        write.
 */
LIBMMS_API void
MmsClient_writeNamedVariableList(MmsClient self, MmsError* mmsError, bool isAssociationSpecific,
        const char* domainId, const char* itemId, LinkedList /* <MmsValue*> */values,
        LinkedList* /* <MmsValue*> */accessResults);


LIBMMS_API uint32_t
MmsClient_writeNamedVariableListAsync(MmsClient self, MmsError* mmsError, bool isAssociationSpecific,
        const char* domainId, const char* itemId, LinkedList /* <MmsValue*> */values,
        MmsClient_WriteMultipleVariablesHandler handler, void* parameter);

/**
 * \brief Get the variable access attributes of a MMS named variable of the server
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the variable or NULL for a VMD specific request
 * \param itemId name of the variable
 *
 * \return Returns a MmsTypeSpecification object or NULL if the request failed.
 */
LIBMMS_API MmsVariableSpecification*
MmsClient_getVariableAccessAttributes(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId);

typedef void
(*MmsConnection_GetVariableAccessAttributesHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, MmsVariableSpecification* spec);


LIBMMS_API uint32_t
MmsClient_getVariableAccessAttributesAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* itemId,
        MmsConnection_GetVariableAccessAttributesHandler, void* parameter);

/**
 * \brief Read the values of a domain specific named variable list
 *
 * The resulting named variable list will either be of domain scope (when the domainId argument
 * is present) or VMD scope when the domainId argument is NULL.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the requested variables.
 * \param listName the name of the named variable list
 * \param specWithResult if specWithResult is set to true, a IEC 61850 compliant request will be sent.
 *
 * \return Returns a MmsValue object or NULL if the request failed. The MmsValue object is
 * is of type MMS_ARRAY and contains the variable values of simple or complex type
 * in the order as they appeared in named variable list definition.
 */
LIBMMS_API MmsValue*
MmsClient_readNamedVariableListValues(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* listName, bool specWithResult);

LIBMMS_API uint32_t
MmsClient_readNamedVariableListValuesAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* listName, bool specWithResult,
        MmsClient_ReadVariableHandler handler, void* parameter);


/**
 * \brief Read the values of a association specific named variable list
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param listName the name of the named variable list
 * \param specWithResult if specWithResult is set to true, a IEC 61850 compliant request will be sent.
 *
 * \return Returns a MmsValue object or NULL if the request failed. The MmsValue object is
 * is of type MMS_ARRAY and contains the variable values of simple or complex type
 * in the order as they appeared in named variable list definition.
 */
LIBMMS_API MmsValue*
MmsClient_readNamedVariableListValuesAssociationSpecific(MmsClient self, MmsError* mmsError,
        const char* listName, bool specWithResult);

LIBMMS_API uint32_t
MmsClient_readNamedVariableListValuesAssociationSpecificAsync(MmsClient self, MmsError* mmsError,
        const char* listName, bool specWithResult,
        MmsClient_ReadVariableHandler handler, void* parameter);

/**
 * \brief Define a new VMD or domain scoped named variable list at the server.
 *
 * The resulting named variable list will either be of domain scope (when the domainId argument
 * is present) or VMD scope when the domainId argument is NULL.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the domain for the new variable list
 * \param listName the name of the named variable list
 * \param variableSpecs a list of variable specifications for the new variable list. The list
 *        elements have to be of type MmsVariableAccessSpecification*.
 */
LIBMMS_API void
MmsClient_defineNamedVariableList(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* listName,	LinkedList variableSpecs);

LIBMMS_API uint32_t
MmsClient_defineNamedVariableListAsync(MmsClient self, MmsError* mmsError, const char* domainId,
        const char* listName, LinkedList variableSpecs,
        MmsClient_GenericServiceHandler handler, void* parameter);



/**
 * \brief Define a new association specific named variable list at the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param listName the name of the named variable list
 * \param variableSpecs list of variable specifications for the new variable list.The list
 *        elements have to be of type MmsVariableAccessSpecification*.
 */
LIBMMS_API void
MmsClient_defineNamedVariableListAssociationSpecific(MmsClient self, MmsError* mmsError,
        const char* listName,	LinkedList variableSpecs);

LIBMMS_API uint32_t
MmsClient_defineNamedVariableListAssociationSpecificAsync(MmsClient self, MmsError* mmsError,
        const char* listName, LinkedList variableSpecs,
        MmsClient_GenericServiceHandler handler, void* parameter);

/**
 * \brief Read the entry list of a named variable list at the server.
 *
 * The resulting named variable list will either be of domain scope (when the domainId argument
 * is present) or VMD scope when the domainId argument is NULL.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the domain of the variable list
 * \param listName the name of the named variable list
 * \param deletable THIS IS A OUTPUT PARAMETER - indicates if the variable list is deletable by the
 * client. The user may provide a NULL pointer if the value doesn't matter.
 *
 * \return List of names of the variable list entries or NULL if the request failed
 */
LIBMMS_API LinkedList /* <MmsVariableAccessSpecification*> */
MmsClient_readNamedVariableListDirectory(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* listName, bool* deletable);


typedef void
(*MmsConnection_ReadNVLDirectoryHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, LinkedList /* <MmsVariableAccessSpecification*> */ specs, bool deletable);


LIBMMS_API uint32_t
MmsClient_readNamedVariableListDirectoryAsync(MmsClient self, MmsError* mmsError,
        const char* domainId, const char* listName,
        MmsConnection_ReadNVLDirectoryHandler handler, void* parameter);


/**
 * \brief Read the entry list of an association specific named variable list at the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param listName the name of the named variable list
 *
 * \return List of names of the variable list entries or NULL if the request failed
 */
LIBMMS_API LinkedList /* <MmsVariableAccessSpecification*> */
MmsClient_readNamedVariableListDirectoryAssociationSpecific(MmsClient self, MmsError* mmsError,
        const char* listName, bool* deletable);

LIBMMS_API uint32_t
MmsClient_readNamedVariableListDirectoryAssociationSpecificAsync(MmsClient self, MmsError* mmsError,
        const char* listName,
        MmsConnection_ReadNVLDirectoryHandler handler, void* parameter);

/**
 * \brief Delete a named variable list at the server.
 *
 * The resulting named variable list will either be of domain scope (when the domainId argument
 * is present) or VMD scope when the domainId argument is NULL.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param domainId the domain name of the domain of the variable list
 * \param listName the name of the named variable list
 *
 * \return true if named variable list has been deleted, false otherwise
 */
LIBMMS_API bool
MmsClient_deleteNamedVariableList(MmsClient self, MmsError* mmsError, const char* domainId, const char* listName);


LIBMMS_API uint32_t
MmsClient_deleteNamedVariableListAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* listName,
        MmsClient_GenericServiceHandler handler, void* parameter);

/**
 * \brief Delete an association specific named variable list at the server.
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param listName the name of the named variable list
 *
 * \return true if named variable list has been deleted, false otherwise
 */
LIBMMS_API bool
MmsClient_deleteAssociationSpecificNamedVariableList(MmsClient self, MmsError* mmsError,
        const char* listName);


LIBMMS_API uint32_t
MmsClient_deleteAssociationSpecificNamedVariableListAsync(MmsClient self, MmsError* mmsError, const char* listName,
        MmsClient_GenericServiceHandler handler, void* parameter);

/**
 * \brief Get the MMS local detail parameter (local detail means maximum MMS PDU size).
 *
 * This defaults to 65000 (or the value specified in the stack_config.h file.
 * This function should not be called after a successful connection attempt.
 *
 * \param  self MmsConnection instance to operate on
 * \param localDetail the maximum size of the MMS PDU that will be accepted.
 */
//LIBMMS_API void
//MmsConnection_setLocalDetail(MmsConnection self, int32_t localDetail);
//
//LIBMMS_API int32_t
//MmsConnection_getLocalDetail(MmsConnection self);

/**
 * \brief get the identity of the connected server
 *
 * This function will return the identity of the server if the server supports the MMS identify service.
 * The server identity consists of a vendor name, model name, and a revision.
 *
 * \param  self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 */
LIBMMS_API MmsServerIdentity*
MmsClient_identify(MmsClient self, MmsError* mmsError);

typedef void
(*MmsConnection_IdentifyHandler) (uint32_t invokeId, void* parameter, MmsError mmsError,
        char* vendorName, char* modelName, char* revision);

LIBMMS_API uint32_t
MmsClient_identifyAsync(MmsClient self, MmsError* mmsError,
        MmsConnection_IdentifyHandler handler, void* parameter);

LIBMMS_API void
MmsServerIdentity_destroy(MmsServerIdentity* self);

/**
 * \brief get the VMD status of the connected server (is MMS status service)
 *
 * This function will return the status of the connected server by invoking the MMS status service.
 * The services returns the logical and physical states of the server.
 *
 * \param[in] self MmsConnection instance to operate on
 * \param[out] mmsError user provided variable to store error code
 * \param[out] vmdLogicalStatus user provided variable to store the logical state of the VMD
 * \param[out] vmdPhysicalStatus user provided variable to store the physical state of the VMD
 * \param[in] extendedDerivation instructs the server to invoke self-diagnosis routines to determine server status
 */
LIBMMS_API void
MmsClient_getServerStatus(MmsClient self, MmsError* mmsError, int* vmdLogicalStatus, int* vmdPhysicalStatus,
        bool extendedDerivation);

typedef void
(*MmsConnection_GetServerStatusHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, int vmdLogicalStatus, int vmdPhysicalStatus);

LIBMMS_API uint32_t
MmsClient_getServerStatusAsync(MmsClient self, MmsError* mmsError, bool extendedDerivation,
        MmsConnection_GetServerStatusHandler handler, void* parameter);

/**
 * \brief Send an information report to the server
 *
 * NOTE: function is blocking when transmit buffer is already in use!
 *
 * \param self
 * \param variableAccessDeclarations the variable access attributes of the report variables
 * \param values the values of the report variables
 */
//LIBMMS_API void
//MmsConnection_sendInformationReportListOfVariables(
//        MmsConnection self,
//        LinkedList /* MmsVariableAccessSpecification */ variableAccessDeclarations,
//        LinkedList /* MmsValue */ values
//);

/*******************************************************************************
 * functions for MMS file services
 *******************************************************************************/

typedef void
(*MmsFileDirectoryHandler) (void* parameter, char* filename, uint32_t size, uint64_t lastModified);

/**
 * \brief Callback handler for the get file directory service
 *
 * Will be called once for each file directory entry and after the last entry with \ref filename = NULL to indicate
 * with \ref moreFollows if more data is available at the server. In case of an error the callback will be called with
 * \ref mmsError != MMS_ERROR_NONE and moreFollows = false.
 */
typedef void
(*MmsConnection_FileDirectoryHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, char* filename, uint32_t size, uint64_t lastModfified,
        bool moreFollows);

typedef void
(*MmsFileReadHandler) (void* parameter, int32_t frsmId, uint8_t* buffer, uint32_t bytesReceived);

typedef void
(*MmsConnection_FileReadHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, uint8_t* buffer, uint32_t byteReceived,
        bool moreFollows);


/**
 * \brief open a file for read
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 *
 * \return the FRSM ID (file read state machine) handle of the opened file
 */
LIBMMS_API int32_t
MmsClient_fileOpen(MmsClient self, MmsError* mmsError, const char* filename, uint32_t initialPosition,
        uint32_t* fileSize, uint64_t* lastModified);

typedef void
(*MmsConnection_FileOpenHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, int32_t frsmId, uint32_t fileSize, uint64_t lastModified);

LIBMMS_API uint32_t
MmsClient_fileOpenAsync(MmsClient self, MmsError* mmsError, const char* filename, uint32_t initialPosition, MmsConnection_FileOpenHandler handler,
        void* parameter);


/**
 * \brief read the next data block from the file
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param frsmId the FRSM ID (file read state machine) handle of the file
 * \param handler callback that is invoked to deliver the received data
 * \param handlerParameter user provided paramter that is passed to the callback function
 *
 * \return true if more data follows, false if last data has been received.
 */
LIBMMS_API bool
MmsClient_fileRead(MmsClient self, MmsError* mmsError, int32_t frsmId, MmsFileReadHandler handler, void* handlerParameter);

LIBMMS_API uint32_t
MmsClient_fileReadAsync(MmsClient self, MmsError* mmsError, int32_t frsmId, MmsConnection_FileReadHandler handler, void* parameter);

/**
 * \brief close the file with the specified frsmID
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param frsmId id of the file to close
 */
LIBMMS_API void
MmsClient_fileClose(MmsClient self, MmsError* mmsError, int32_t frsmId);

LIBMMS_API uint32_t
MmsClient_fileCloseAsync(MmsClient self, MmsError* mmsError, uint32_t frsmId, MmsClient_GenericServiceHandler handler, void* parameter);

/**
 * \brief delete the file with the specified name
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param fileName name of the file to delete
 */
LIBMMS_API void
MmsClient_fileDelete(MmsClient self, MmsError* mmsError, const char* fileName);

LIBMMS_API uint32_t
MmsClient_fileDeleteAsync(MmsClient self, MmsError* mmsError, const char* fileName,
        MmsClient_GenericServiceHandler handler, void* parameter);

/**
 * \brief rename the file with the specified name
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param currentFileName name of the file to rename
 * \param newFileName new name of the file
 */
LIBMMS_API void
MmsClient_fileRename(MmsClient self, MmsError* mmsError, const char* currentFileName, const char* newFileName);

LIBMMS_API uint32_t
MmsClient_fileRenameAsync(MmsClient self, MmsError* mmsError, const char* currentFileName, const char* newFileName,
        MmsClient_GenericServiceHandler handler, void* parameter);

/**
 * \brief Send an obtainFile request to the server (used to initiate file download to server)
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param sourceFile the name of the source file (client side name)
 * \param destinationFile the name of the destination file (server side name)
 */
LIBMMS_API void
MmsClient_obtainFile(MmsClient self, MmsError* mmsError, const char* sourceFile, const char* destinationFile);

LIBMMS_API uint32_t
MmsClient_obtainFileAsync(MmsClient self, MmsError* mmsError, const char* sourceFile, const char* destinationFile,
        MmsClient_GenericServiceHandler handler, void* parameter);

/**
 * \brief get the file directory of the server.
 *
 * This function will return the directory entries of the given server directory. For each directory entry
 * the provided callback handler is called. If the
 *
 * \param self MmsConnection instance to operate on
 * \param mmsError user provided variable to store error code
 * \param fileSpecification the file specification of the directory to browse or NULL to browse the root directory
 * \param continueAfter continuation point or NULL for the first request. The continuation point is the first entry
 *                      after the provided continuation file name.
 * \param handler user provided callback handler
 * \param handlerParameter user provided parameter that is passed to the handler
 *
 * \return (more follows) true if more data is available
 */
LIBMMS_API bool
MmsClient_getFileDirectory(MmsClient self, MmsError* mmsError, const char* fileSpecification, const char* continueAfter,
        MmsFileDirectoryHandler handler, void* handlerParameter);

LIBMMS_API uint32_t
MmsClient_getFileDirectoryAsync(MmsClient self, MmsError* mmsError, const char* fileSpecification, const char* continueAfter,
        MmsConnection_FileDirectoryHandler handler, void* parameter);

typedef struct sMmsJournalEntry* MmsJournalEntry;

typedef struct sMmsJournalVariable* MmsJournalVariable;

struct sMmsJournalEntry {
    MmsValue* entryID; /* type MMS_OCTET_STRING */
    MmsValue* occurenceTime; /* type MMS_BINARY_TIME */
    LinkedList journalVariables;
};

struct sMmsJournalVariable {
    char* tag;
    MmsValue* value;
};

/**
 * \brief Destroy a single MmsJournalEntry instance.
 *
 * This function will destroy the whole MmsJournalEntry object including the attached list
 * of MmsJournalVariable objects. It is intended to be used in conjunction with the
 * LinkedList_destroyDeep function in order to free the result of MmsConnection_readJournalTimeRange
 * or MmsConnection_readJournalStartAfter
 *
 * LinkedList_destroyDeep(journalEntries, (LinkedListValueDeleteFunction)
 *                           MmsJournalEntry_destroy);
 *
 * \param self the MmsJournalEntry instance to destroy
 */
LIBMMS_API void
MmsJournalEntry_destroy(MmsJournalEntry self);

LIBMMS_API const MmsValue*
MmsJournalEntry_getEntryID(MmsJournalEntry self);

LIBMMS_API const MmsValue*
MmsJournalEntry_getOccurenceTime(MmsJournalEntry self);

LIBMMS_API const LinkedList /* <MmsJournalVariable> */
MmsJournalEntry_getJournalVariables(MmsJournalEntry self);

LIBMMS_API const char*
MmsJournalVariable_getTag(MmsJournalVariable self);

LIBMMS_API const MmsValue*
MmsJournalVariable_getValue(MmsJournalVariable self);

typedef void
(*MmsClient_ReadJournalHandler) (uint32_t invokeId, void* parameter, MmsError mmsError, LinkedList /* <MmsJournalEntry> */ journalEntries, bool moreFollows);


LIBMMS_API LinkedList /* <MmsJournalEntry> */
MmsClient_readJournalTimeRange(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        MmsValue* startTime, MmsValue* endTime, bool* moreFollows);

LIBMMS_API uint32_t
MmsClient_readJournalTimeRangeAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        MmsValue* startTime, MmsValue* endTime, MmsClient_ReadJournalHandler handler, void* parameter);

LIBMMS_API LinkedList /* <MmsJournalEntry> */
MmsClient_readJournalStartAfter(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        MmsValue* timeSpecification, MmsValue* entrySpecification, bool* moreFollows);

LIBMMS_API uint32_t
MmsClient_readJournalStartAfterAsync(MmsClient self, MmsError* mmsError, const char* domainId, const char* itemId,
        MmsValue* timeSpecification, MmsValue* entrySpecification, MmsClient_ReadJournalHandler handler, void* parameter);

/**
 * \brief Destroy (free) an MmsServerIdentity object
 *
 * \param self the object to destroy
 */
LIBMMS_API void
MmsServerIdentity_destroy(MmsServerIdentity* self);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* MMS_CLIENT_CONNECTION_H_ */
