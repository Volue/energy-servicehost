/*
$Header$
Description:

Copyright (C) Powel Data AS
All rights reserved.
*/

static char rcsid[]="$Id: registry.c 35146 2004-01-16 15:03:01Z sih $ PD";
static void* use_rcsid[2]={use_rcsid, (void*) rcsid}; /* Referencing rcsid */

#ifdef WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winreg.h>
#include <winerror.h>
#include "icc_env.h"


static int hkey_open(char *key, HKEY *result_hkey);


/*****************************************************************************/


int read_env_from_reg(char *key,
                      void (*handler)(char *key, char* name, char *value,
                            int type),char *username)
{
    HKEY hkey,subhkey;
    LONG rval;
    DWORD dwIndex;
    char Name[256];
    DWORD lName = sizeof(Name);
    char Class[256];
    DWORD lClass = sizeof(Class);
    char Data[1024];
    DWORD lData = sizeof(Data);
    DWORD Type;
    char *subkey;


    /* If no key specified, use default key for ID-System. */
    if (key == NULL)
        key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Powel Data"
                    "\\ID-System\\Environment";
    
    /* Open specified key */
    if ((rval = hkey_open(key, &hkey)) != ERROR_SUCCESS)
    {
        fprintf(stderr,"read_env_from_reg: open_hkey(%s) failed (%ld)\n",
            key, rval);
        return rval;
    }
    
    /* Scan for values */
    for (dwIndex  = 0; (rval = RegEnumValue(hkey, dwIndex, Name, &lName,
        NULL, &Type, Data, &lData)) == ERROR_SUCCESS;
        dwIndex++)
    {
        switch (Type)
        {
        case REG_EXPAND_SZ:
            /* This type needs environment expansion. */
        case REG_SZ:
            if (handler) handler(key, Name, Data, Type);
            break;
        default:
            fprintf(stderr,"read_env_from_reg: Unsupported type: %d\n", Type);
            break;
        }
        lName = sizeof(Name); lData = sizeof(Data);
    }
    
    if (rval == ERROR_NO_MORE_ITEMS)
        rval = ERROR_SUCCESS;
    else
    {
        fprintf(stderr,"read_env_from_reg: RegEnumKeyEx() failed (item #%d error %ld)\n",
                dwIndex, rval);

    }

    /* Scan for subkeys that matches ./%/username 
     * here we are a bit lazy, - calling the recursive although it should not
     * do anything more than just Environment/Group/user
     */
    for (dwIndex  = 0; (rval = RegEnumKeyEx(hkey, dwIndex, Name, &lName,
        NULL, Class, &lClass, NULL)) == ERROR_SUCCESS;
        dwIndex++)
    {
        if (subkey = malloc(strlen(key) + strlen(Name) + 2 + strlen(username)+2)) {
            sprintf(subkey,"%s\\%s\\%s",key,Name,username);
            if ( hkey_open(subkey, &subhkey) == ERROR_SUCCESS) {
                RegCloseKey(subhkey);
                sprintf(subkey, "%s\\%s", key, Name);
                read_env_from_reg(subkey, handler,username);
                sprintf(subkey,"%s\\%s\\%s",key,Name,username);
                read_env_from_reg(subkey, handler,username);
                /* here we could break out, but I leave it as is:user in several groups is possible */
            }
            free(subkey);
        }
        else
            fprintf(stderr,
                "read_env_from_reg: Out of memory for subkey %s\n", Name);
        
        lName = sizeof(Name); lClass = sizeof(Class);
    }
    if (rval == ERROR_NO_MORE_ITEMS)
        rval = ERROR_SUCCESS;
    else
    {
        fprintf(stderr,"read_env_from_reg: RegEnumKeyEx() failed (item #%d error %ld)\n",
                dwIndex, rval);

    }
    RegCloseKey(hkey);
    return rval;
}


/* Open an existing registry key. `key' is a full key path.            */
/* This interface should have been supplied by Micro$oft...            */
/* Returns: 0 if OK (result_hkey set), else nonzero. */
static int hkey_open(char *key, HKEY *result_hkey)
{
    static struct hive
    {
        char *name;
        int size;
        HKEY hkey;
    } hivemap[] =
    {
        { "HKEY_LOCAL_MACHINE", sizeof("HKEY_LOCAL_MACHINE")-1, HKEY_LOCAL_MACHINE },
        { "HKEY_CLASSES_ROOT",    sizeof("HKEY_CLASSES_ROOT")-1, HKEY_CLASSES_ROOT },
        { "HKEY_USERS",            sizeof("HKEY_USERS")-1, HKEY_USERS },
        { "HKEY_CURRENT_USER",    sizeof("HKEY_CURRENT_USER")-1, HKEY_CURRENT_USER },
        { NULL,    0,        NULL }
    };
    int n;
    
    if (key == NULL || result_hkey == NULL) return -1;

    if ((n = strcspn(key, "\\")) > 0)
    {
        struct hive *hp;
        
        for (hp = hivemap; hp->name; hp++)
        {
            if ((n == hp->size) && (strncmp(hp->name, key, n) == 0))
                return RegOpenKeyEx(hp->hkey, key+n+1, 0, KEY_READ, result_hkey);
        }
    }
    return -1;
}

#else
/* Dummy for Unix systems */
int read_env_from_reg(char *key,
                      void (*handler)(char *key, char* name, char *value,
                            int type))
{
    return 0;
}
#endif /* WIN32 */
