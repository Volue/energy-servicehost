/*
Set up ICC environment from information read
from registry keys. Expand environment strings.

Copyright (C) Powel Data AS
All rights reserved.
*/

#ifdef WIN32

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <winreg.h>
#include <winerror.h>
#include "icc_env.h"
#include <direct.h>
#include <process.h>
#include <io.h>
#include <sys/stat.h>

char **Powel_env = NULL;
int Powel_n_env=0;
 
char ** Powel_orig_env=0; /* original environment to be kept */ 
int Powel_n_orig_env=0;

char** Powel_encrypted = NULL;
int Powel_n_encrypted = 0;
 
static void SaveInitialEnv() 
{ /*
  to be called BEFORE any Powel_env. setting
  in order to provide user override
  for var.settings.
  */
	int i;
	/* Keep original environment, it has to be restored later */
	static int veryFirstTime=1;
	if(veryFirstTime) 
    {
		veryFirstTime=0;
		(void)getenv("ABC");//just to be sure environment is loaded.
		for(Powel_n_orig_env=0;_environ[Powel_n_orig_env];Powel_n_orig_env++);// count number of Powel_env. params
		Powel_orig_env= (char **)calloc(Powel_n_orig_env+1,sizeof(char**));// +1, due to null terminated list
		for(i=0;i<Powel_n_orig_env;i++) Powel_orig_env[i] =_strdup(_environ[i]);
	}
}

void icc_cleanup_env() 
{
	int i;
	for(i=0;i<Powel_n_env;i++) {
		free(Powel_env[i]);
	}
	free(Powel_env);Powel_n_env=0;Powel_env=0;
	for(i=0;i<Powel_n_orig_env;i++) {
		free(Powel_orig_env[i]);
	}
	free(Powel_orig_env);Powel_n_orig_env=0;
	Powel_orig_env=0;
	for (i = 0; i < Powel_n_encrypted; i++) {
		free(Powel_encrypted[i]);
	}
	free(Powel_encrypted); Powel_n_encrypted = 0;
	Powel_encrypted = 0;
}
 
char ** icc_env_ptr() 
{
	return Powel_env;
}

char** icc_encrypted_ptr()
{
	return Powel_encrypted;
}

int icc_putenv(const char *s) 
{
	char* eq_str;
	if(!s) 
		return -1;
	eq_str=strchr(s,'=');
	if (!eq_str)
		return -1;
	if(strlen(eq_str)>1) {
		return _putenv(s);
	} else {
		if(strstr(s,"ABAT_"))
			 return 0;
		/* only clear if set */ {
			int nameSize=(eq_str-s)+1;
			char *sx=(char*)alloca( nameSize );
			strncpy(sx,s,nameSize-1);
			sx[nameSize-1]=0;
			if(getenv(sx)) {
				return _putenv(s);
			}
			return 0;
		}
	}
	return -1;
}
static void envhandler(char *key, char* name, char *value, int type);
static void *myalloc(void *p, size_t size);
static char *expand(char *s);
/*
 * Set up environment for ICC based on information in registry.
 * Expand environment variables (%variable%).
 * Return string array with strings of the form "name=value".
 *
 * 
 * The following environment precedence rules are followed:
 *  1) current environment  
 *  2) Registry/Environment/Company/GetUserName() (if this exists)
 *  3) Registry/Company (only if 2) exists )
 *  4) Registry/Environment
 *
 * If no key is specified default key is used.
 * All the returned environment variables are set by this function,
 * the returned pointer array is for distribution to other processes.
 */
extern void RefInitEnv();
extern char **icc_env_setup_(char *key, int *count,char *userName);
extern char **icc_db_setup_(char *key, int *count,char *userName,char*pcName,char *progName );
extern char** icc_encrypted_parameters_(char Ækey, int* count, char* userName, char* pcName, char* progName);
static char **icc_expand_env(int *count);
static  int isPathSeparator(char c) { return c=='/'|| c=='\\'?1:0;}

static int CreatePath(const char *Apath) 
{ /*
	returns TRUE if Apath exsists, or was created successfulley
	otherwise FALSE.
	*/
	char path[2024]; int i,isUncPath=FALSE,isUncDevice=FALSE;
	struct stat fst;   
	if(!Apath)
		return FALSE;
	isUncPath= isPathSeparator(Apath[0])&&isPathSeparator(Apath[1]);

	if(stat(Apath,&fst)==-1) {//missing iccdir?
		i=0;
		do {
			path[i]=Apath[i];
			if(isUncPath&&i<2)
				continue;
			if(isPathSeparator(path[i])|| path[i]==0) {
				if(isUncPath){
					isUncPath=FALSE;
					isUncDevice=TRUE;
				} else {
					if(i>0&&path[i-1]==':' || isUncDevice) {
						path[i+1]=0;//leave C:
						if(isUncDevice && path[i]==0) {
							path[i]='\\';//terminate uncdevice
						}
						isUncDevice=FALSE;
					} else
						path[i]=0;
					if(stat(path,&fst)==-1) {
						if(_mkdir(path)!=0)
							return FALSE;// dont continue, it failed
					}
					path[i]=Apath[i];// restore null-char
				}
			}
		} while(Apath[i++]);
	}
	return TRUE;
}

char **icc_env_setup_internal(char *key, int *count)
{
    char username[256];
    int size = sizeof(username);
    char computername[256];
    char unique_env_id[300];
    int sizeC = sizeof(computername);
    char **dbenv;
    int envCount = 0;
    if (Powel_env)
    {
        /* We do this ONCE only */
        if (count)
            *count = Powel_n_env;
        return Powel_env;
    }
    SaveInitialEnv();
    sprintf(unique_env_id, "ICC_ENV_%d", _getpid());
    if (getenv(unique_env_id))
    {// environment loaded for this pid?
        icc_expand_env(count); // just set internals
    }
    else
    {
        // load configuration, and put it to environment
        char unique_env_id_val[400];
        sprintf(unique_env_id_val, "%s=LOADED", unique_env_id);
        _putenv(unique_env_id_val);// mark this environment as done 
        username[0] = 0;
        GetUserName(username, &size);
        computername[0] = 0;
        GetComputerName(computername, &sizeC);
		
		// Replace SYSTEM with computer's samaccountname to ensure localsystem
		// and networkservice is handled the same way for D-85821
		if (_stricmp(username, "SYSTEM") == 0)
		{
			sprintf(username, "%s$", computername);
		}

        dbenv = !getenv("ICC_NO_CFGSERVER") ? icc_db_setup_(key, &envCount, username, computername, 0) : 0;
        if (dbenv)
        {
            Powel_env = dbenv;
            Powel_n_env = envCount;
            icc_expand_env(count);
			int nEncrypted = 0;
			char** encrypted = icc_encrypted_parameters_(key, &nEncrypted, username, computername, 0);
			if (nEncrypted > 0) {
				Powel_encrypted = encrypted;
				Powel_n_encrypted = nEncrypted;
			}
        }
        else
        {
            icc_env_setup_(key, count, username);
        }
        /* Fix for temp-dir needed by unix-style software */
        {
            char *tmpstr = NULL;
            char *envstr[1024];
            int res;

            if (getenv("ICC_TMP"))
            {
                tmpstr = _strdup(getenv("ICC_TMP"));
            }
            else
            {
                tmpstr = _strdup(getenv("TMP"));
            }

            // D-11225 TEMP path gets overwritten by ICC_TMP :
            // 	 Do not overwrite Windows TMP/TEMP variable [ob, 2018-08-29]
            //sprintf(envstr,"TMP=%s",tmpstr);
            //res=icc_putenv(envstr);
            //sprintf(envstr,"TEMP=%s",tmpstr);
            //res=icc_putenv(envstr);
            sprintf(envstr, "TMPDIR=%s", tmpstr);
            res = icc_putenv(envstr);
            sprintf(envstr, "TEMPDIR=%s", tmpstr);
            res = icc_putenv(envstr);
            CreatePath(tmpstr);
            free(tmpstr);
        }
        /* Fix for ICCDIR to be created at startup */
        if (getenv("ICCDIR"))
        {
            char *iccdir = _strdup(getenv("ICCDIR"));
            CreatePath(iccdir);
            free(iccdir);
        }
        /* Fix needed for HOMEDRIVE that caused slow startup of modules if
           homedrive was a networked drive */
        if (getenv("ICC_HOMEDRIVE"))
        {
            char *tmpstr = _strdup(getenv("ICC_HOMEDRIVE")), envstr[1024];
            int res;
            sprintf(envstr, "HOMEDRIVE=%s", tmpstr);
            res = icc_putenv(envstr);
            free(tmpstr);
        }

        if (Powel_env)
        {
            int i;
            for (i = 0; i < Powel_n_env; i++)
            {
                free(Powel_env[i]); /* release memory not needed */
            }
            free(Powel_env);
            Powel_env = 0;
            Powel_n_env = 0;
        }
        /* Now construct real environment to be returned to caller*/
        {
            int n = 0;
            int i = 0;
            for (n = 0; _environ[n]; n++); // compute the size
            Powel_env = (char **)calloc(n + 1, sizeof(char**));// alloc final Powel_env vector

            for (i = 0; i < n; i++)
                Powel_env[i] = _strdup(_environ[i]); // fill up

            Powel_env[n] = 0;
            if (count)
                *count = n;

            Powel_n_env = n;
        }
    }
    return Powel_env;
}

char **icc_env_setup_(char *key, int *count,char *username)
{
	//read from registry(key) and expand environment
	int rval;
	SaveInitialEnv();// make sure we have initial environment
	Powel_n_env = 0;
	Powel_env = myalloc(Powel_env, (Powel_n_env + 300) * sizeof(char*)); /* Initial Powel_env */
	Powel_env[Powel_n_env] = NULL;
	if ((rval = read_env_from_reg(key, envhandler,username)) != ERROR_SUCCESS) 
    {
		fprintf(stderr, "icc_env_setup: read_env_from_reg failed(%d)\n",
			rval);
		return NULL; 
	}
	Powel_env[Powel_n_env] = NULL;
	if(count) *count=Powel_n_env;
	icc_expand_env(count);
	return Powel_env;
}

static char **icc_expand_env(int *count) 
{ /*
	expand and apply Powel_env/counter environment
	and apply original Powel_env. for override,
	construct final global Powel_env/counter.
	*/
	int z;
	for(z=0;z<3;z++) 
    {
		int i;
		/* 1.a Environment variable expansion loop. */
		for (i = 0; i < Powel_n_env; i++) 
        {
			if (strchr(Powel_env[i], '%')) 
            {
				char *s = expand(Powel_env[i]);
				icc_putenv(s);
				free(Powel_env[i]);
				Powel_env[i] = s;
			}
		}
		/* Add current environment at end of Powel_env
		 * at this point the Powel_env[] contains general,company,user - profile
		 * In effect this gives another expand possibility (?)
		 */
		for( i = 0;i<Powel_n_env;i++) 
        {
			icc_putenv(Powel_env[i]);
			free(Powel_env[i]); /* release memory not needed */
		}
		if (Powel_env) 
            free(Powel_env);
        Powel_env=0;
        Powel_n_env=0;
		/* then put on the original Powel_env values */
		for(i=0; i< Powel_n_orig_env ; i++) 
			icc_putenv(Powel_orig_env[i]);

		/* Now construct real environment to be returned to caller*/ 
        {
			int n=0;
			for(n=0;_environ[n];n++); // compute the size
			Powel_env = (char **)calloc(n+1,sizeof(char**));// alloc final Powel_env vector
			for(i=0;i<n;i++) 
                Powel_env[i] =_strdup(_environ[i]); // fill up
			Powel_env[n]=0;
			if (count) 
                *count = n;
			Powel_n_env=n;
		}
	}
	return Powel_env;
}


/* Build a string array containing all environment variables read from
   the registry.
   NOTE: This initial version disregards TYPE and does not sort the
   environment list. */
static void envhandler(char *key, char* name, char *value, int type)
{
	char *nameval;

	/* Keep room for ptr array + terminating NULL pointer */
	Powel_env = myalloc(Powel_env, (Powel_n_env + 2) * sizeof(char*));

	/* 
	 * Expand environment first time here to ensure that
	 * self-referencing variables are expanded according to previous
	 * settings (eg PATH=%PATH%;C:\mybin)
	 */
	nameval = _alloca(strlen(name) + strlen(value) + 2);
	sprintf(nameval, "%s=%s", name, value);
	Powel_env[Powel_n_env] = expand(nameval);
	icc_putenv(Powel_env[Powel_n_env++]);
}


static void *myalloc(void *p, size_t size)
{
	if (p = p? realloc(p, size) : calloc(size,1))
		return p;
	else
	{
		fprintf(stderr, ": FATAL: Out of memory (%s line %d)\n", 
			__FILE__, __LINE__);
		abort();
	}
	return 0;
}


/*
 * Expand any environment variables in string s.
 * Returned value must be freed by caller.
 */
static char *expand(char *s) 
{
	if (strchr(s, '%')) 
    {
		int n=10000;
		char  *buffer=_alloca(n);
		int res=ExpandEnvironmentStrings(s, buffer,n);
		/* A variable has % as prefix and suffix. Check on missing one of them. Then don't recurse.
		 * And handle e.g. ICC_TBL_DAY_FORMAT1=$M %D, %02Y 
		 */
		BOOL recurse = TRUE;
		if (strlen(s) == strlen(buffer) && strcmp(s, buffer) == 0) {
			/* Should have had some kind of debug flag here. */
			/*fprintf(stderr, "Not expanded environment variable: %s\n", buffer);*/
			recurse = FALSE;
		}
		if (recurse && strchr(buffer, '%')) {
			char* exp = expand(buffer);
			strcpy(buffer, exp);
			free(exp);
		}
		if (res  > 0 && res <n ) 
			return _strdup(buffer);
	}
	return _strdup(s);
}

#else
/* Dummy for Unix systems
 * To avoid returning a NULL pointer indicating error, return
 * a pointer to a static array containing no values.
 */
#include <stdio.h>
char **icc_env_setup(char *key, int *count)
{
	static char *rval = NULL;

	if (count) *count = 0;
	return &rval;
}
#endif WIN32
