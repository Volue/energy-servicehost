/****** -*- C++ -*- *********************************************************
 *
 *      File name : DBI_error.h
 *      Author    : Jarand Roeynstrand    EFI           DATE: 920717
 *      Copyright : Norwegian Electric Power Research Institute
 *      Compiler  : AT&T C++ 2.1
 *
 *      Path         : %P%
 *      $Revision: 37171 $
 *      Last changed : $Date: 2004-09-29 17:43:15 +0000 (Wed, 29 Sep 2004) $
 *
 *      Contents :  Functions callable from c-programs for the DBInterface
 *      GetDBError, GetDBErrorNumber, ClearDBError and GetErrnoText  member
 *	functions.
 *      
 *      Includes and defines:
 */
#ifndef DBI_ERROR_H
#define DBI_ERROR_H
#include "DBI_ERRNO.h"

 /*      Externals and statics:
 */
#ifndef DBMS_API
#include "dbmsapi.h"
#endif
/*****************************************************************************/
        
#ifdef __cplusplus
extern "C" {
#endif
DBMS_API ERRNO   DBI_error_number( void );
DBMS_API char     * DBI_error_mess( void );
DBMS_API void    DBI_error_clear( void );
DBMS_API char	*DBI_dbms_err_mess( void );
DBMS_API const char	*DBI_errno_text( void );
DBMS_API char	*DBI_user_err_string( void );
#ifdef __cplusplus
}
#endif

#endif /* DBI_ERROR_H */
