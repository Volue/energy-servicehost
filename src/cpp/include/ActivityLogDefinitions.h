/**** File_Header *************************************************************
 *
 *  $Id: ActivityLogDefinitions.h 271277 2015-11-19 06:22:15Z job $
 *
 *  $RCSfile$
 *  Author    : Odd Broenner    PD     DATE: 980721
 *      Copyright : Powel Data AS
 *
 *      Keywords  : 
 *  Contents  : 
 *
 *  Includes and defines:
 */
#ifndef _ACTIVITYLOGDEFINITIONS_H
#define _ACTIVITYLOGDEFINITIONS_H

enum AL_HANDLER_TYPE
{
    AL_HANDLE_KEEP,
    AL_HANDLE_STORE,
    AL_HANDLE_RESET
};

enum AL_MESSAGE_TYPE
{
    AL_MESS_INFO    = 0,
    AL_MESS_WARNING = 1,
    AL_MESS_ERROR   = 2,
    AL_MESS_FATAL   = 9
};

enum AL_APPLICATION_STATUS
{
    AL_STATUS_OK        =  0,
    AL_STATUS_WARNING   =  1,
    AL_STATUS_ERROR     =  2,
    AL_STATUS_FATAL     =  9,
    AL_STATUS_RUNNING   = 10
};

enum AL_MESSAGE_LEVEL
{
    AL_LEV_NULL = 0,    /* No information at all */
    AL_LEV_STD  = 1,    /* Initial info , like application startet/stopped etc */
    AL_LEV_DEB  = 9     /* Includes DEBUG/WARNING/TRACE macroes */
};

enum ICC_MODULE
{
    M_NOMODULE          =-99,
    M_COMMON_MODULE     =0,
    M_COMMON_DB_MODULE  =1,
    M_CALCDELIVERY      =2,
    M_CALCREPORT        =3,
    M_CUSTOMER          =4,
    M_DEBIT             =5,
    M_EDIEXP            =6,
    M_EDIIMP            =7,
    M_EXCHPNT           =8,
    M_GS2EXP            =9,
    M_GS2IMP            =10,
    M_VALCORR           =11,
    M_IMPEXP            =12,
    M_MEASPNT           =13,
    M_MVSREPORT         =14,
    M_NAVIGATOR         =15,
    M_NETAREA           =16,
    M_AUDIT             =17,
    M_REGMEAS           =18,
    M_MVSAPP            =19,
    M_CONTRACTUIS       =20,
    M_ELCSUP            =21,
    M_CONSUME           =22,
    M_HBVUIS            =23,
    M_DBMAINTENANCE     =24,
    M_HYDCORR           =25,
    M_HYDVAL            =26,
    M_INTEGRATOR        =27,
    M_ACCOUNTING        =29,
    M_PDLOG             =30,
    M_PGENMINMAX        =31,
    M_SIMDOC            =32,
    M_EDKIN             =33,
    M_EDKOUT            =34,
    M_STRUCTLOAD        =35,
    M_UIS               =38,
    M_USERADM           =39,
    M_ELCEXP            =40,
    M_ELCIMP            =41,
    M_LOGON             =42,
    M_ESTER             =43,
    M_SIM               =45,
    M_TSSEARCH          =47,
    M_TSRSERVER         =49,
    M_TSLOCKER          =50,
    M_TSRICLIENT        =51,
    M_TSRVIEWEXCELFILE  =52,
    M_TSRVIEWHTMLFILE   =53,
    M_TSRVIEWPSFILE     =54,
    M_TSRPRINTPS        =55,
    M_KISCLIENT         =56,
    M_KISDATALOAD       =57,
    M_METER             =58,
    M_BID               =59,
    M_VALCORRSRV        =75,
    M_STATUSSEARCH      =76,
    M_TSHYD             =77,
    M_SIMREVAPP         =78,
    M_RCEDIT            =79,
    M_CHART             =80,
    M_DB_INTERNAL_USAGE =81,
    M_ETLIMPORT         =82,    // External Table Loader
    M_WSH               =83,    // Windows Script Host
    M_EVNTLOG           =84,
    M_TSREPORT          =85,
    M_PONOIMP           =86,
    M_CREATEVTS         =87,
    M_ETLSERVER         =88,
    M_SCADAEX           =89, // SCADA Extension or SCADA Exchange, 1. version Nematic/Netcontrol/birka
    M_INSTRREG          =90,
    M_SHOP              =91,
    M_EXPREPORT         =92,
    /* 
    mvsexport = 93
    impcommon = 94  
    expcommon = 95
    */
    M_TRORG             =96,  // Trade Organizer
    M_PRODCODE          =97,   // Product Code
    M_INSTALLATION      =98,    
    M_SCADA             =99,
    /*
    metervl        = 100
    batchserver_db = 101
    cis_update     = 102
    */
    M_WATERIMP              =103,
    /*
    P_PROTIL   = 104
    P_SAMKJMOD = 105
    icclock    = 106
    maintmsa   = 107
    */
    M_ALARM_CLIENT          = 108,
    M_ALARM_ADM             = 109,
    M_SETUPAPP              = 110,
    M_AVAILABILITY          = 111,
    M_ALARM_SERVICE         = 112,
    /*
    */
    M_MAXCONTROL            = 139,
    M_PLAUSREPORT           = 140,
    /*
    */
    M_UTSEXP                = 142,
    M_UTSIMP                = 143,
    /*
    */
    M_MESSAGELOG            = 155,
    M_MPREPORT              = 159,
    M_NIMBUSCONFIG          = 160,
    M_IDLOAD                = 161,
    M_NIMBUS                = 162,
    M_IMMEDIATE_EXPORT      = 167,
    M_CALCMPREPORT          = 173,
    M_ACTIVATION_REQUEST    = 180,
    M_BLOCKIMP              = 181,
	M_VIND_POWER_FORECAST	= 190,
};

#define PDLOG_LOG_TO_ALL        0xf
#define PDLOG_LOG_TO_DATABASE   0x1
#define PDLOG_LOG_TO_WINDOW     0x2 
#define PDLOG_LOG_TO_FILE       0x4
#define PDLOG_LOG_TO_SYSTEMLOG  0x8

#ifndef __cplusplus
typedef enum AL_MESSAGE_TYPE    AL_MESSAGE_TYPE;
typedef enum AL_MESSAGE_LEVEL   AL_MESSAGE_LEVEL;
#endif /* #ifndef __cplusplus */

#endif /* #ifndef _ACTIVITYLOGDEFINITIONS_H */
