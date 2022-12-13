/**** File_Header *************************************************************
 *
 *      $Revision: 238781 $
 *
 *	$RCSfile$
 *	Author    : Odd Broenner    PD     DATE: 970505
 *	Copyright : Powel Data AS
 *	Compiler  : 
 *
 *      Keywords  : Definitions for Powel Data applications
 *	Contents  : 
 *
 *	Includes and defines:
 */
	

#ifndef _ICC_DB_PROGRAMS_H_
#define _ICC_DB_PROGRAMS_H_


#include <icc_gen.h>

/*
 *	The enums below are used in the database; their values
 *	cannot be changed easily. Add new members at the end only!
 */

enum ICC_PROGRAM
{
    P_UNKNOWN,
    P_HEC2,
    P_DAMBRK,
    P_ELV,
    P_RICE,
    P_HBV,
    P_ENMAG,
    P_FINNECO,
    P_QUAL2E,
    P_VANSIMTAP,
    P_BIORIV		=10,
    P_BIOLAKE,
    P_AKVAS,
    P_SEDIMENT,
    P_FRILUFT,
    P_SIM,
    P_HYDIN,
    P_HYDVAL,
    P_HYDCORR,
    P_UIS,
    P_CIS		=20,
    P_LOG,
    P_SAKON,
    P_ESTER,
    P_ELCBAS,
    P_KOFOT,
    P_TRADE,
    P_EDKUIS,
    P_EDKIN,
    P_EDKOUT,
    P_TSIMPORT		=30,
    P_TSEXPORT,
    P_TSIMPUIS,
    P_MAINT,
    P_SOLVER,
    P_VANENER,
    P_PROTIL,
    P_SNOMOD,
    P_SAMKJMOD,
    P_SHOP,
    P_CUSTOMERUIS	=40,
    P_NETAREAUIS=41,
    P_EXCHPNTUIS=42,
    P_MEASPNTUIS=43,
    P_CALCBALANCE=44,
    P_CALCREPORT=45,
    P_REGMEASUIS=46,
    P_DEBITUIS=47,
    P_TSPANEL=48,
    P_PDAUDITUIS=49,
    P_TSLOAD		=50,
    P_IDLOAD		=51,
    P_STRUCTLOADUIS=52,
    P_ACTIVITYLOGUIS=53,
    P_TSSEARCH=54,
    P_TSRSERVER=55,
    P_TSEDITOR=56,
    P_KISCLIENT=57,
    P_KISDATALOAD=58,
    P_METERUIS=59,
    P_BID=60,
    P_TSLOCKER=61,
    P_ACCOUNTING=62,
    P_MVSAPP=63,
    P_CHART=64,
    P_RCEDIT=65,
    P_INTEGRATOR=66,
    P_DBIMPORT=67,
    P_TSREPORT=68,
    P_CREATEVTS=69,
    P_SCADAEX=70,
    P_SIMREV=71,
    P_INSTRREG=72,
    P_TRORG=73,
    P_INSTALLATION=74,
    P_SCADA=75,
    P_CIS_UPDATE=76,
    P_WATERIMP=77,
    P_SETUPAPP=78,
    P_ALARM				=79,
    P_AVAILABILITY		=80,
    P_TSHYD				=98,
    P_ALARMADM			=99,
	P_NIMBUSCONFIG		=101,
	P_IMMEDIATE_EXPORT	=102,
	P_CALCMPREPORT		=103,
    P_HVCUIS = 9999,
/* customer applications starts at 10000 */
    P_DEFAULT = 9999
};


#ifndef __cplusplus
typedef enum ICC_PROGRAM 	ICC_PROGRAM;
#endif /* __cplusplus */
/*	Externals and statics:
 */

/*	Function prototypes
 */

/*****************************************************************************/

#endif /* _ICC_DB_PROGRAMS_H_*/
