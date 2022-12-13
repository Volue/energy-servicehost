/****************************************************************************
*****    U B I O  -  U N I B U S   I / O   C L A S S   L I B R A R Y    *****
*****                Copyright Scanvest Olivetti - Norway               *****
*****************************************************************************
*
* File name....: ErrMessage.h
* Version......: %I%
* Last modified: %E%  
* Path name....: %P%
* Created by...: Kaare Veland
* Created date.: 91/02/19
*
* DESCRIPTION:
*       UBIO header file for error message handling.
*
* MODIFICATION HISTORY:
*       Version Date            Sign    Description
*       1.1     91/02/19        KV              New file.
*       1.2             91/07/11        JA              Error message class removed, users may now
*                                                               call only GetError.
*
*****************************************************************************
*/

#ifndef ERRMESSAGE_H
#define ERRMESSAGE_H

#include "DBI_ERRNO.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */


void errExit(ERRNO errNo, char *str = NULL);
void errExit(char *str);

class DBMS_API ErrMessage
{
public:
        // if ERRSOURCETYPE is ERRARRAY, array pointer must be cast to char *
        ErrMessage      ();
        ~ErrMessage ();

        // If ERRSOURCETYPE = ERRARRAY, GetError will cast char* errSource to char**
        char    *GetError       (const ERRNO, const char *freeText = (char *)NULL);

private:

        char    *BuildMessage(ERRNO errNo, const char *message, 
                        const char *freeText = NULL);
};

#endif  /* ERRMESSAGE_H */
