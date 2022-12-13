/**** File_Header *************************************************************
 *
 *      $Revision: 9975 $  $Date: 1997-06-13 11:33:39 +0000 (Fri, 13 Jun 1997) $
 *
 *	$RCSfile$
 *	Author    : Roland Kruse    EFI-20     DATE: 930212
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : gcc
 *
 *      Keywords  : General-purpose definitions for the ICC software
 *	Contents  : 
 *
 *	Includes and defines:
 */
	
#ifndef _ICC_GEN_H_
#define _ICC_GEN_H_

#include <limits.h>


/*
 *	Boolean constants
 *	
 *	Use of TRUE and FALSE constants:
 *	Never compare boolean expressions with TRUE or FALSE!
 *	These constants are for assignment only!
 *	Examples:
 *	WRONG: if (b == TRUE)     CORRECT: if (b)
 *	WRONG: if (b == FALSE)	  CORRECT: if (!b)
 *				  CORRECT: b = TRUE;
 *				  CORRECT: return FALSE;
 */
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif



/*
 *	Error code definitions. Error codes are defined as negative numbers
 *	to allow them to be used with two classes of functions:
 *
 *	1. Functions which return either ICC_OK or an error code.
 *	2. Functions which normally return a positive integer, but may return
 *	   a (negative) error code if they fail.
 */
#define ICC_OK 		0	/* OK value returned from many API functions */

#define ERR_ARGS	-1	/* Too many/few or invalid argument(s) */
#define ERR_ENVOY	-2	/* ENVOY (GCOM) error */
#define ERR_CONTEXT	-3 	/* Wrong context */
#define ERR_NOMEM	-4	/* Out of memory */
#define ERR_GENERAL	-5  	/* General failure */
#define ERR_NOTFOUND	-6	/* Object not found */
#define ERR_AMBIGUOUS	-7	/* Ambiguous object specification */
#define ERR_NOSUPPORT	-8	/* Operation not supported */
#define ERR_NOTIMPL	-9	/* Operation not (yet) implemented */
#define ERR_SYNTAX	-10	/* Syntax error */
#define ERR_FILE	-11	/* File access failed */
#define ERR_NOPERM	-12	/* Permission denied */
/* @add more as required. try to keep the error codes general */

/*
 *	Constants indicating elements has no value set.
 */
#define NO_INT_VALUE	(INT_MIN+1)		/* Sybase restriction: +1    */
#define NO_SHORT_VALUE  (SHRT_MIN+1)		/* Sybase restriction: +1    */
#define NO_LONG_VALUE	(LONG_MIN+1)		/* Sybase restriction: +1    */
#define NO_UNSIGN_VALUE	ULONG_MAX
#define NO_USHORT_VALUE USHRT_MAX
#define NO_ULONG_VALUE  ULONG_MAX
#define NO_CHAR_VALUE	'\0'

/*
 *	NO_FLOAT_VALUE no longer used - use icc_nofloat() and icc_isfloat()
 *	instead (see genlib.h)
 */

/*
 *	This tremendous kludge is needed on HP/UX to correct an even more
 *	perverse kludge made by hp: #define index strchr   ....#@z%!
 */
#ifdef index
#undef index
#endif


/*	Externals and statics:
 */

/*	Function prototypes
 */

/*****************************************************************************/


#endif /* _ICC_GEN_H_ */
