/**** File_Header *************************************************************
 *
 *      $Revision: 201426 $  $Date: 2012-05-30 08:04:10 +0000 (Wed, 30 May 2012) $
 *
 *	$RCSfile$
 *	Author    : Roland Kruse    EFI-20     DATE: 921103
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : gcc
 *
 *      Keywords  : Common definitions for ICC software
 *	Contents  : 
 *
 *	Includes and defines:
 */
#ifndef _ICC_H_
#define _ICC_H_

#include "icc_messages.h"
#include "icc_db.h"


#define DB_APP_VERSION 8
#define DB_APP_RELEASE 0
#define DB_VERSION "8.0"
/*
 *	ICC-MESSAGE constants.
 */
/* Severity levels */
#define LOG_FATAL	0
#define LOG_ERROR	1
#define LOG_WARNING	2
#define LOG_INFORMATION 3
#define LOG_SEVERITY_LEVELS 4

/* Log output destinations */
#define OUT_ALL		-1
#define OUT_WINDOW	1
#define OUT_FILE	2


typedef double       TS_VL_VALUE_TYPE;
typedef unsigned int TS_VL_STATUS_TYPE;
typedef float        OLD_TS_VL_VALUE_TYPE;

/*
 *	TXY time unit codes
 */
enum ICC_TIME_UNITS
{
    /* Time units */
    ICC_MILLISEC	= -400,
    SECOND		= -300,
    MINUTE		= -200,
    MIN3		= -193,
    MIN5		= -155,
    MIN10		= -110,
    MIN15		= -100,
    MIN30		= -30,
    HOUR		= 3,
    DAY			= 4,
    WEEK		= 5,
    MONTH		= 6,
    YEAR		= 99,

    NO_TIME_UNIT = NO_SHORT_VALUE
};



/*
 *	Specific ICC-SIM definitions.
 */
enum ICC_SIM_TYPE
{
    RESERVOIR_ATTRIBUTES = 25000,
    RESERVOIR_INFLOW,
    RESERVOIR_SIM_RESULT,
    SLACK_RESERVOIR,
    
    PLANT_ATTRIBUTES,
    PLANT_SIM_RESULT,
    
    GENERATOR_ATTRIBUTES,
    GENERATOR_SIM_RESULT,

    PUMP_ATTRIBUTES,
    PUMP_SIM_RESULT,

    WATERROUTE_ATTRIBUTES,
    WATERROUTE_INFLOW,
    WATERROUTE_SIM_RESULT,

    GATE_ATTRIBUTES,
    GATE_SIM_RESULT,

    CATCHMENT_ATTRIBUTES,

    SCHEDULE,
    TOTAL_SCHEDULE,
    PLANT_INFLOW,
    MANUAL_FORECAST,
    TURBINE_SIM_RESULT,
    TURBINE_EFF_HEIGHT
};




#ifndef __cplusplus
typedef enum ICC_TIME_UNITS 	ICC_TIME_UNITS;
typedef enum ICC_SIM_TYPE 	ICC_SIM_TYPE;
#endif /* !__cplusplus */
/*	Externals and statics:
 */

/*	Function prototypes
 */

/*****************************************************************************/
	

#endif

