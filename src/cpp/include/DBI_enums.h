/*** File_Header *** -*- C++ -*- **********************************************
 *
 *	Author    : Odd Broenner    EFI           DATE: 920717
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : AT&T C++ 2.1
 *
 *	Path         : %P%
 *	$Revision: 193705 $
 *	Last changed : %E%
 *
 *	Contents : enumerations to be used from both C and C++ functions.
 *	           
 *	
 *	Includes and defines:
 */

/*	Externals and statics:
 */

/*****************************************************************************/
	
#ifndef DBI_ENUMS_H
#define DBI_ENUMS_H

/*  E n u m e r a t i o n s */

enum DB_READ_MODE
{
	READ_CURRENT,
	READ_FIRST,
	READ_LAST,
	READ_NEXT,
	READ_PREVIOUS,
	READ_ABORT
};

enum DB_BATCH_MODE
{
	DB_NO_BATCH,
	DB_OPEN_BATCH,
	DB_NEXT_BATCH,
	DB_CLOSE_BATCH
};

enum DB_RETURN
{
	REQUEST_OK,
	DB_EXCEPTION,
	DB_ERROR
};

enum DBI_OVERWRITE
{
	OW_NO,
	OW_APPEND,
	OW_YES
};

enum DBI_TIME_TYPE
{
    ABS_TIME,
    DELTA_TIME,
    NO_TIME
};

enum DBI_TIME_STEP_TYPE
{
    INT_UNKNOWN,
    FIX_INT,
    VARIABLE_INT,
    TEXT_INT,
    HOUR_INT,
    DAY_INT,
    WEEK_INT,
    MIN15_INT,
    MONTH_INT,
    YEAR_INT,
    MIN3_INT,
    MIN5_INT,
    MIN10_INT,
    MIN30_INT
};

enum
{
    DBI_NO_OF_AREA_CODES=4999
};

enum DBI_TS_VALUE_TYPE
{
    /*  NB NB NB NB NB
     *	For ICC_MVS : all negative values are in use for VLCODE
     *  NB NB NB NB NB
     */
    DBI_TS_UNDEF_VALUE_TYPE = -1,
    DBI_TS_VALUES = 0,
    DBI_TS_QUALITY_CODES,
    DBI_TS_SHADOW_VALUES,
    DBI_TS_QUALITY_CODES_4,
    DBI_TS_PRIMARY_VALUES,
    DBI_TS_ORIGINAL=100,
    DBI_TS_AUX_VALUES_1 = 101,
    DBI_TS_AUX_VALUES_2,
    DBI_TS_AUX_VALUES_3,
    DBI_TS_AUX_VALUES_4,
    DBI_TS_AUX_VALUES_5,
    DBI_TS_AUX_VALUES_6,
    DBI_TS_AUX_VALUES_7,
    DBI_TS_AUX_VALUES_8,
    DBI_TS_AUX_VALUES_9,
    DBI_DATA_STATUS = 200,
    DBI_TS_REPORTED = 300,
    DBI_AREA_CODES = 10000,
    DBI_LAST_AREA_CODE = (int)DBI_AREA_CODES+(int)DBI_NO_OF_AREA_CODES,
    DBI_RPT_FLAGS = 100000,
    DBI_COPY_EXPORTED = 1000000  /* VLCODE for a copy of exported series is DBI_COPY_EXPORTED + OPRTUNIT.OPUN_KEY */
};

/* Enumeration of valid status codes */
enum DBI_DATA_STATUS_CODE
{
    NO_STAUS = 0,
    STATUS_MEASURED,
    STATUS_MANUAL_ENTERED,
    STATUS_ESTIMATED,
    STATUS_COMPUTED,
    STATUS_HELD,
    STATUS_AUTOMATIC_CORRECTED
};

enum 
{
    DBI_TS_MAX_AUX_VALUES_SETS =
	(int) DBI_TS_AUX_VALUES_9-(int) DBI_TS_AUX_VALUES_1 + 1
};

/* QUALITY_CODE is typedef'ed as TS_VL_STATUS_TYPE */
typedef unsigned int QUALITY_CODE;
enum
{
    NO_QUALITY_CODE = 0
};

/* Constant that flags undefined foreign keys. */
enum
{
    NO_FOREIGN_KEY = 0
};

/* Constant that flags that no DB-cursor where available */
enum
{
    DBI_NO_CURSOR = -1
};

/* Constant that flags that no watercourse is defined
 *	(Should be moved to icc.h?) */
enum
{
    NO_WATER_COURSE = 0
};

enum DBI_START_END
{
    DBI_START,
    DBI_END
};

enum DBI_CURVETYPE
{
    CURVETYPE_UNDEFINED = -2,
    STAIRCASE_START_OF_STEP = -1,
    PIECEWISE_LINEAR,
    STAIRCASE_END_OF_STEP
};

enum HYDDBI_COR_VAL
{
    VALIDATION,
    CORRECTION
};

#ifndef __cplusplus
/* In C is typedef's and struct's not considered proper types. */
typedef enum DB_READ_MODE DB_READ_MODE;
typedef enum DB_BATCH_MODE DB_BATCH_MODE;
typedef enum DB_RETURN DB_RETURN;
typedef enum DBI_OVERWRITE DBI_OVERWRITE;
typedef enum DBI_TIME_TYPE DBI_TIME_TYPE;
typedef enum DBI_TIME_STEP_TYPE DBI_TIME_STEP_TYPE;
typedef enum DBI_TS_VALUE_TYPE DBI_TS_VALUE_TYPE;
typedef enum DBI_START_END DBI_START_END;
typedef enum DBI_CURVETYPE DBI_CURVETYPE;
typedef enum HYDDBI_COR_VAL HYDDBI_COR_VAL;
typedef enum DBI_DATA_STATUS_CODE DBI_DATA_STATUS_CODE;
#endif	/* __cplusplus */

/* Undefined values in the db. */
/* Not used. Use icc_isfloat(), icc_nodouble() */
/* static const double DBI_UNDEF_FLOAT=-9e30; */
/* #define DBI_UNDEF_FLOAT NO_FLOAT_VALUE */
/* pull in module dll macroes here since this file is
 included by all soclasses modules.*/
#include "dbmsapi.h"
#endif /* DBI_ENUMS_H */

