/**** File_Header *************************************************************
 *
 *      $Revision: 22758 $  $Date: 2001-01-21 16:40:00 +0000 (Sun, 21 Jan 2001) $
 *
 *	$RCSfile$
 *	Author    : Jarand Roeynstrand    EFI-20     DATE: 931126
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : gcc
 *
 *      Keywords  : 
 *	Contents  : 	Defines the macro  DBI_ERROR_LOCATION
 *
 *	Includes and defines:
 */
#ifndef _DBILINEFILE_H_
#define _DBILINEFILE_H_

//#include <stdio.h>

#define  _DBI__FILE__LEN 100
#ifndef DBMS_API
#include "dbmsapi.h"
#endif
extern DBMS_API char _dbiLineFileMessString[];
extern DBMS_API char _dbiLineFileString[];

#define DBI_ERROR_LOCATION (sprintf( _dbiLineFileString, _dbiLineFileMessString, __LINE__, _DBI__FILE__LEN, __FILE__) ,_dbiLineFileString )

/*****************************************************************************/
#endif /* _DBILINEFILE_H_ */	
