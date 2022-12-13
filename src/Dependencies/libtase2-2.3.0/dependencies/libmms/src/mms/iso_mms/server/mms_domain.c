/*
 * 	mms_domain.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "mms_device_model.h"
#include "mms_server_internal.h"

static void
freeNamedVariables(MmsVariableSpecification** variables, int variablesCount)
{
	int i;
	for (i = 0; i < variablesCount; i++) {
	    MmsVariableSpecification_destroy(variables[i]);
	}
}

MmsDomain*
MmsDomain_create(char* domainName)
{
	MmsDomain* self = (MmsDomain*) GLOBAL_CALLOC(1, sizeof(MmsDomain));

	self->domainName = StringUtils_copyString(domainName);
	self->namedVariableLists = LinkedList_create();
	self->journals = NULL;

	return self;
}

void
MmsDomain_destroy(MmsDomain* self)
{
	GLOBAL_FREEMEM(self->domainName);

	if (self->namedVariables != NULL) {
		freeNamedVariables(self->namedVariables,
				self->namedVariablesCount);

		GLOBAL_FREEMEM(self->namedVariables);
	}

	if (self->namedVariablesHashTree)
	    HashTree_destroy(self->namedVariablesHashTree);

	if (self->journals != NULL) {
	    LinkedList_destroyDeep(self->journals, (LinkedListValueDeleteFunction) MmsJournal_destroy);
	}

	LinkedList_destroyDeep(self->namedVariableLists, (LinkedListValueDeleteFunction) MmsNamedVariableList_destroy);

	GLOBAL_FREEMEM(self);
}

char*
MmsDomain_getName(MmsDomain* self)
{
	return self->domainName;
}

void
MmsDomain_addJournal(MmsDomain* self, const char* name)
{
    if (self->journals == NULL)
        self->journals = LinkedList_create();

    MmsJournal journal = MmsJournal_create(name);

    LinkedList_add(self->journals, (void*) journal);
}


MmsJournal
MmsDomain_getJournal(MmsDomain* self, const char* name)
{
    if (self->journals != NULL) {

        LinkedList journal = LinkedList_getNext(self->journals);

        while (journal != NULL) {

            MmsJournal mmsJournal = (MmsJournal) LinkedList_getData(journal);

            if (strcmp(mmsJournal->name, name) == 0)
                return mmsJournal;

            journal = LinkedList_getNext(journal);
        }
    }

    return NULL;
}

bool
MmsDomain_addNamedVariableList(MmsDomain* self, MmsNamedVariableList variableList)
{
	LinkedList_add(self->namedVariableLists, variableList);

	return true;
}

MmsNamedVariableList
MmsDomain_getNamedVariableList(MmsDomain* self, const char* variableListName)
{
	MmsNamedVariableList variableList = NULL;

	if (self == NULL)
	    goto exit_function;

	variableList = mmsServer_getNamedVariableListWithName(self->namedVariableLists, variableListName);

exit_function:
	return variableList;
}

void
MmsDomain_deleteNamedVariableList(MmsDomain* self, char* variableListName)
{
	mmsServer_deleteVariableList(self->namedVariableLists, variableListName);
}

LinkedList
MmsDomain_getNamedVariableLists(MmsDomain* self)
{
	return self->namedVariableLists;
}

MmsVariableSpecification*
MmsDomain_addNamedVariable(MmsDomain* self, int idx, MmsVariableSpecification* varSpec)
{
    self->namedVariables[idx] = varSpec;

    if (HashTree_add(self->namedVariablesHashTree, varSpec->name, varSpec))
        return varSpec;
    else
        return NULL;
}

MmsVariableSpecification*
MmsDomain_getNamedVariable(MmsDomain* self, const char* nameId)
{
    if (self->namedVariablesHashTree) {

        MmsVariableSpecification* varSpec = (MmsVariableSpecification*) HashTree_find(self->namedVariablesHashTree, nameId);

#if (CONFIG_MMS_SUPPORT_FLATTED_NAME_SPACE == 1)
        const char* separator = strchr(nameId, CONFIG_MMS_SEPARATOR_CHAR);

        if (separator) {
            char varName[130];

            int varNameLen = separator - nameId;

            memcpy(varName, nameId, varNameLen);

            varName[varNameLen] = 0;

            MmsVariableSpecification* namedVariable = (MmsVariableSpecification*) HashTree_find(self->namedVariablesHashTree, varName);

            if (namedVariable != NULL) {
                varSpec = MmsVariableSpecification_getNamedVariableRecursive(namedVariable, separator + 1);
            }
        }
        else {
            varSpec = (MmsVariableSpecification*) HashTree_find(self->namedVariablesHashTree, nameId);
        }
#else
        varSpec = (MmsVariableSpecification*) HashTree_find(self->namedVariablesHashTree, nameId);
#endif

        return varSpec;
    }

#if (CONFIG_MMS_NO_ACCESS_OPTIMIZATION  == 1)
    else
    if (self->namedVariables != NULL) {

#if (CONFIG_MMS_SUPPORT_FLATTED_NAME_SPACE == 1)

		char* separator = strchr(nameId, CONFIG_MMS_SEPARATOR_CHAR);

		int i;

		if (separator == NULL) {

			for (i = 0; i < self->namedVariablesCount; i++) {
				if (strcmp(self->namedVariables[i]->name, nameId) == 0) {
					return self->namedVariables[i];
				}
			}

			return NULL;
		}
		else {
			MmsVariableSpecification* namedVariable = NULL;

			for (i = 0; i < self->namedVariablesCount; i++) {

				if (strlen(self->namedVariables[i]->name) == (unsigned) (separator - nameId)) {

					if (strncmp(self->namedVariables[i]->name, nameId, separator - nameId) == 0) {
						namedVariable = self->namedVariables[i];
						break;
					}
				}
			}

			if (namedVariable != NULL) {
				namedVariable = MmsVariableSpecification_getNamedVariableRecursive(namedVariable, separator + 1);
			}

			return namedVariable;
		}

#else
        int i;

        for (i = 0; i < self->namedVariablesCount; i++) {
            if (strcmp(self->namedVariables[i]->name, nameId) == 0) {
                return self->namedVariables[i];
            }
        }
#endif

	}
#endif /* (CONFIG_MMS_NO_ACCESS_OPTIMIZATION  == 1) */

	return NULL;
}
