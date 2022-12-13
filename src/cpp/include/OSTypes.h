/****************************************************************************
*****************    D I G  -  C L A S S   L I B R A R Y    *****************
*****                Copyright Objekt Systemer - Norway               *****
*****************************************************************************
*
* File name....: OSTypes.h
* Version......: %I%
* Last modified: $Date: 2004-09-29 17:43:15 +0000 (Wed, 29 Sep 2004) $
* Path name....: %P%
* Created by...: Oddbjorn Overskott
* Created date.: 94/07/01
* Tab stops....: 5 9 ...
*
* DESCRIPTION:
*       Standard typedefs used in Object Database Toolset C++ classes.
*
* MODIFICATION HISTORY:
*       Version Date      Sign  Description
*       1.0     94/07/01  OO    Converted from the old OSTypes.h
*
*****************************************************************************
*/

#ifndef OSTYPES_H
#define OSTYPES_H

enum OS_BOOL
{
        STATE_FALSE,
        STATE_TRUE
};

enum OS_ANSWER
{
        OS_YES = 'Y',
        OS_NO = 'N'
};


// Basic, no-nonsense datatypes.

#ifndef CHAR
typedef char            CHAR;
#endif
typedef CHAR          * pCHAR;

#ifndef UCHAR
typedef unsigned char   UCHAR;
#endif
typedef UCHAR         * pUCHAR;

#ifndef INT
typedef int             INT;
#endif
typedef INT           * pINT;

#ifndef UINT
typedef unsigned int    UINT;
#endif
typedef UINT          * pUINT;

#ifndef SINT
typedef short           SINT;
#endif
typedef SINT          * pSINT;

#ifndef LONG
typedef long            LONG;
#endif
typedef LONG          * pLONG;

#ifndef ULONG
typedef unsigned long   ULONG;
#endif
typedef ULONG         * pULONG;

#ifndef DOUBLE
typedef double          DOUBLE;
#endif
typedef DOUBLE        * pDOUBLE;

#ifndef pVOID
typedef void          * pVOID;
#endif


// Sophisticated datatype class hierarchy.

#include "OSDataType.h"
#include "OSNumeric.h"
#include "OSSInt.h"
#include "OSInt.h"
#include "OSLong.h"
#include "OSDouble.h"
#include "OSDateTime.h"

#endif    /* OSTYPES_H */
