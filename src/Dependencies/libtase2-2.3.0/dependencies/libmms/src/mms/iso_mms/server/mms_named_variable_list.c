/*
 *  mms_named_variable_list.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "mms_named_variable_list.h"
#include "libmms_platform_includes.h"

MmsNamedVariableListEntry
MmsNamedVariableListEntry_create(MmsAccessSpecifier accessSpecifier)
{
	MmsNamedVariableListEntry listEntry = (MmsNamedVariableListEntry) GLOBAL_MALLOC(sizeof(MmsAccessSpecifier));

	listEntry->domain = accessSpecifier.domain;

	listEntry->variableName = StringUtils_copyString(accessSpecifier.variableName);
	listEntry->arrayIndex = accessSpecifier.arrayIndex;

	if (accessSpecifier.componentName)
		listEntry->componentName = StringUtils_copyString(accessSpecifier.componentName);
	else
		listEntry->componentName = NULL;

	return listEntry;
}

void
MmsNamedVariableListEntry_destroy(MmsNamedVariableListEntry self)
{
	GLOBAL_FREEMEM(self->variableName);
	GLOBAL_FREEMEM(self->componentName);
	GLOBAL_FREEMEM(self);
}


MmsDomain*
MmsNamedVariableListEntry_getDomain(MmsNamedVariableListEntry self)
{
	return self->domain;
}

char*
MmsNamedVariableListEntry_getVariableName(MmsNamedVariableListEntry self) {
	return self->variableName;
}

MmsNamedVariableList
MmsNamedVariableList_create(MmsDomain* domain, char* name, bool deletable)
{
	MmsNamedVariableList self = (MmsNamedVariableList) GLOBAL_MALLOC(sizeof(struct sMmsNamedVariableList));

	if (self) {
        self->deletable = deletable;
        self->name = StringUtils_copyString(name);
        self->listOfVariables = LinkedList_create();
        self->domain = domain;
        self->lastVariable = NULL;
	}

	return self;
}

MmsDomain*
MmsNamedVariableList_getDomain(MmsNamedVariableList self)
{
    return self->domain;
}

char*
MmsNamedVariableList_getName(MmsNamedVariableList self)
{
	return self->name;
}

bool
MmsNamedVariableList_isDeletable(MmsNamedVariableList self)
{
	return self->deletable;
}

void
MmsNamedVariableList_addVariable(MmsNamedVariableList self, MmsNamedVariableListEntry variable)
{
    if (self->lastVariable == NULL)
        self->lastVariable = LinkedList_append(self->listOfVariables, variable);
    else
        self->lastVariable = LinkedList_append(self->lastVariable, variable);
}

LinkedList
MmsNamedVariableList_getVariableList(MmsNamedVariableList self)
{
	return self->listOfVariables;
}

static void
deleteVariableListEntry(void* listEntry)
{
	MmsNamedVariableListEntry entry = (MmsNamedVariableListEntry) listEntry;
	MmsNamedVariableListEntry_destroy(entry);
}

void
MmsNamedVariableList_destroy(MmsNamedVariableList self)
{
	LinkedList_destroyDeep(self->listOfVariables, deleteVariableListEntry);
	GLOBAL_FREEMEM(self->name);
	GLOBAL_FREEMEM(self);
}


