/**** File_Header *************************************************************
 *
 *	Author    : Tor E Johansen    EFI-20     DATE: 960606
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : gcc
 *
 *	Keywords  : 
 *	Contents  : 
 *
 *	Includes and defines:
 */
#ifndef _SEC_DBCONNECT_H_
#define _SEC_DBCONNECT_H_

/*	Externals and statics:
 */

/*	Function prototypes
 */

#ifdef __cplusplus
extern "C"
{
#endif

// NB: It is the calling program's responsibility to free the memory of the returned string by calling free
char* SEC_DBGetOsUser();
char* SEC_DBGetUsername(char* dbid); 
char* SEC_DBGetPassword(char* dbid);
char* SEC_DBGetClientUser(char* dbid);
char* SEC_DBGetHostname(char* dbid);
char* SEC_DBGetServername(char* dbid);

DB_RETURN SEC_DBServerConnect(int progid, char *dbid, char *appl, char *remoteServer = nullptr, void *cursor = nullptr);

#ifdef __cplusplus
}
#endif

int CheckProgramId( int progid, char *appl );

/*****************************************************************************/
#endif /* _SEC_DBCONNECT_H_ */	
