/*
 *  mms_model.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_DEVICE_MODEL_H_
#define MMS_DEVICE_MODEL_H_

#include "libmms_common_api.h"
#include "mms_type_spec.h"
#include "mms_common.h"
#include "mms_named_variable_list.h"
#include "hash_tree.h"
#include "logging_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char* deviceName;

    /* MMS VMD scope variables support */
    int namedVariablesCount;

    MmsVariableSpecification** namedVariables;
    HashTree namedVariablesHashTree;

    /* MMS VMD scope named variables list support */
    LinkedList /*<MmsNamedVariableList>*/ namedVariableLists;

    /* up to here sMmsDomain structure is identical */

    /* MMS domain support */
    int domainCount;
    MmsDomain** domains;
} MmsDevice;


struct sMmsJournal {
    char* name;
    LogStorage logStorage;
};

typedef struct sMmsJournal* MmsJournal;

/*
 * Server side data structure to hold information of an MMS domain (Logical Device)
 */
struct sMmsDomain {
    char* domainName;
    int namedVariablesCount;

    MmsVariableSpecification** namedVariables;
    HashTree namedVariablesHashTree;

    LinkedList /*<MmsNamedVariableList>*/ namedVariableLists;

    /* up to here sMmsServer structure is identical */

    LinkedList /* <MmsJournal> */ journals;
};

/**
 * @defgroup mms_server_api_group MMS server API (for IEC 61850 use IEC 61850 server API instead!)
 *
 * @{
 */

/**
 * \brief Create a new MmsDomain instance
 *
 * This method should not be invoked by client code!
 *
 * \param domainName the name of the MMS domain
 *
 * \return the new MmsDomain instance
 */
LIBMMS_API MmsDomain*
MmsDomain_create(char* domainName);

LIBMMS_API char*
MmsDomain_getName(MmsDomain* self);

LIBMMS_API void
MmsDomain_addJournal(MmsDomain* self, const char* name);

LIBMMS_API MmsJournal
MmsDomain_getJournal(MmsDomain* self, const char* name);

/**
 * Delete a MmsDomain instance
 *
 * This method should not be invoked by client code!
 */
LIBMMS_API void
MmsDomain_destroy(MmsDomain* self);

/**
 * \brief Add a new MMS Named Variable List (Data set) to a MmsDomain instance
 *
 * The passed MmsNamedVariableList instance will be handled by the MmsDomain instance
 * and will be destroyed if the MmsDomain_destroy function on this MmsDomain instance
 * is called.
 *
 * \param self instance of MmsDomain to operate on
 * \param variableList new named variable list that will be added to this MmsDomain
 *
 * \return true if operation was successful.
 */
LIBMMS_API bool
MmsDomain_addNamedVariableList(MmsDomain* self, MmsNamedVariableList variableList);

/**
 * \brief Delete a MMS Named Variable List from this MmsDomain instance
 *
 * A call to this function will also destroy the MmsNamedVariableList instance.
 *
 * \param self instance of MmsDomain to operate on
 * \param variableListName the name of the variable list to delete.
 *
 */
LIBMMS_API void
MmsDomain_deleteNamedVariableList(MmsDomain* self, char* variableListName);

LIBMMS_API MmsNamedVariableList
MmsDomain_getNamedVariableList(MmsDomain* self, const char* variableListName);

LIBMMS_API LinkedList
MmsDomain_getNamedVariableLists(MmsDomain* self);

LIBMMS_API LinkedList
MmsDomain_getNamedVariableListValues(MmsDomain* self, char* variableListName);

LIBMMS_API LinkedList
MmsDomain_createNamedVariableListValues(MmsDomain* self, char* variableListName);

LIBMMS_API  MmsVariableSpecification*
MmsDomain_addNamedVariable(MmsDomain* self, int idx, MmsVariableSpecification* varSpec);

/**
 * \brief Get the MmsTypeSpecification instance of a MMS named variable
 *
 * \param self instance of MmsDomain to operate on
 * \param nameId name of the named variable
 *
 * \return MmsTypeSpecification instance of the named variable
 */
LIBMMS_API MmsVariableSpecification*
MmsDomain_getNamedVariable(MmsDomain* self, const char* nameId);

/**
 * \brief Create a new MmsDevice instance.
 *
 * MmsDevice objects are the root objects of the address space of an MMS server.
 *
 * An MmsDevice object can contain one or more MmsDomain instances.
 *
 * \param deviceName the name of the MMS device or NULL if the device has no name.
 *
 * \return the new MmsDevice instance
 */
LIBMMS_API MmsDevice*
MmsDevice_create(char* deviceName);

/**
 * \brief Delete the MmsDevice instance
 */
LIBMMS_API void
MmsDevice_destroy(MmsDevice* self);

/**
 * \brief Get the MmsDomain object with the specified MMS domain name.
 *
 * \param deviceName the name of the MMS device or NULL if the device has no name.
 *
 * \return the new MmsDevice instance
 */
LIBMMS_API MmsDomain*
MmsDevice_getDomain(MmsDevice* self, const char* domainId);

/**
 * \brief Get the MmsTypeSpecification instance of a MMS named variable of VMD scope
 *
 * \param self instance of MmsDevice to operate on
 * \param variableName name of the named variable
 *
 * \return MmsTypeSpecification instance of the named variable
 */
LIBMMS_API  MmsVariableSpecification*
MmsDevice_getNamedVariable(MmsDevice* self, const char* variableName);

LIBMMS_API LinkedList
MmsDevice_getNamedVariableLists(MmsDevice* self);

LIBMMS_API void
MmsDevice_addNamedVariableList(MmsDevice* self, MmsNamedVariableList variableList);

LIBMMS_API MmsNamedVariableList
MmsDevice_getNamedVariableListWithName(MmsDevice* self, const char* variableListName);

LIBMMS_API MmsJournal
MmsJournal_create(const char* name);

LIBMMS_API void
MmsJournal_destroy(MmsJournal self);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* MMS_DEVICE_MODEL_H_ */
