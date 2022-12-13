/*
 *  mms_named_variable_list.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef MMS_NAMED_VARIABLE_LIST_H_
#define MMS_NAMED_VARIABLE_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup mms_server_api_group
 *  @{
 */

#include "libmms_common_api.h"
#include "linked_list.h"
#include "mms_common.h"

struct sMmsNamedVariableList {
	bool deletable;
	MmsDomain* domain;
	char* name;
	LinkedList listOfVariables; /* list of MmsNamedVariableListEntry elements */
	LinkedList lastVariable; /* pointer to last element of listOfVariables */
};

MmsNamedVariableListEntry
MmsNamedVariableListEntry_create(MmsAccessSpecifier accessSpecifier);

void
MmsNamedVariableListEntry_destroy(MmsNamedVariableListEntry self);

MmsDomain*
MmsNamedVariableListEntry_getDomain(MmsNamedVariableListEntry self);

char*
MmsNamedVariableListEntry_getVariableName(MmsNamedVariableListEntry self);

MmsNamedVariableList
MmsNamedVariableList_create(MmsDomain* domain, char* name, bool deletable);

char*
MmsNamedVariableList_getName(MmsNamedVariableList self);

MmsDomain*
MmsNamedVariableList_getDomain(MmsNamedVariableList self);

bool
MmsNamedVariableList_isDeletable(MmsNamedVariableList self);

void
MmsNamedVariableList_addVariable(MmsNamedVariableList self, MmsNamedVariableListEntry variable);

LinkedList
MmsNamedVariableList_getVariableList(MmsNamedVariableList self);

void
MmsNamedVariableList_destroy(MmsNamedVariableList self);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* MMS_NAMED_VARIABLE_LIST_H_ */
