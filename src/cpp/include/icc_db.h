/**** File_Header *************************************************************
 *
 *      $Revision: 327992 $  $Date: 2021-11-04 08:00:47 +0000 (Thu, 04 Nov 2021) $
 *
 *	$RCSfile$
 *	Author    : Roland Kruse    EFI-20     DATE: 930212
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : gcc
 *
 *  Keywords  : Definitions for ICC-Database.
 *	Contents  :
 *
 *	Includes and defines:
 */


#ifndef _ICC_DB_H_
#define _ICC_DB_H_

#include "icc_gen.h"
#include "icc_db_programs.h"

/*
 *	The enums below are used in the database; their values
 *	cannot be changed easily. Add new members at the end only!
 */


/*
 *	ICC process identifiers
 */

enum ICC_CATMA
{
	UNDEF_CATMA		= 0,
	HBV_CATMA		= 1,
	PROTIL_CATMA	= 20,
	SIM_CATMA		= 30,
	MANUAL_CATMA	= 999,
	LAST_PD_CATMA	= 999
};




/*
 *	Measurement units
 */
enum ICC_UNMEA
{
	UNDEF_UNMEA,
	M3 = 100,		    		/* Square_meters */
	MM3,		    			/* Million_square_meters */
	METER,		    			/* Meter */
	PERCENT,		    		/* Percent */
	MW,	            			/* Mega_watt */
	GWH,		    			/* Giga_watt_hours */
	MWH,		    			/* Mega_watt_hours */
	WATT_HOUR,        			/* Watt_hours */
	M3SEC,						/* m3/second */
	LSEC,						/* Liters_/_second */
	LSECKM2 = 110,	    		/* Liters/second/square-kilometer */
	M2,							/* Square-meter */
	KM2,						/* Square-kilometer */
	DEGREE,		    			/* Degrees_0-360 */
	CELSIUS,		    		/* Degrees_Celsius */
	KILOGRAM,		    		/* Veight_in_kilogram */
	ID_PASCAL,	        		/* Pascal */
	VOLT,						/* Volt */
	AMPERE,	        			/* Ampere */
	W_PER_M2,	        		/* Watt_/_square-meters */
	M_PER_SEC = 120,        	/* Meter_/_second */
	S_PER_M,	        		/* Conductivity_(s/m) */
	MG_PER_L,	        		/* Milligram_/_liter */
	KG_PER_S,	        		/* Kilogram_/_sekund */
	MILLIMETER,	    			/* Millimeter */
	KW_METER,		    		/* Kilo_watt_/_meter */
	KRONER,		    			/* Norwegian_crowns */
	MILL_NOK,		    		/* NOK_1_000_000_. */
	NOK_MWH,		    		/* NOK/MWh */
	MWH_H,						/* Megawatt_hours/hour */
	MILLIGRAM=130,	    		/* Milligram */
	CENTIMETER,        			/* Centimeter */
	PERCENT_MAX,	    		/* Percent_of_max */
	PERCENT_NORM,	    		/* Percent_of_normal_value */
	KWH_H=140,		    		/* Kilo_watt_hours/hour */
	USD_MWH=141,
	UNMEA_USD_CURRENCY,			/* Amerikanske dollar */
	KWH = 800,		    		/* Kilo_watt_hours */
	CELSIUS_100M = 810,			/* Degrees_Celsius_per_100_meter */
	UNMEA_HOUR = 811, 			/* Number_of_hours */
	HERTZ = 820,				/* Hertz */
	MVA,						/* Mega_volt_Ampere */
	MVAR,						/* MVA_reactive_effect */
	MVARH,						/* MVA_reactive_energy */
	MVARH_H,		    		/* MVA_reactive_effect */
	MW_HERTZ,					/* Degree_of_regulation */
	KVAR,						/* kVA_reactive_effect */
	KVARH,						/* kVA_reactive_energy */
	KVARH_H,		    		/* kVA_reactive_effect */
	KPA,						/* Kilo_Pascal */
	M3_H = 830,					/* Square_meters/hour */
	SEK_MWH,		    		/* SEK_per_MWh */
	MWH_C,						/* MWh_per_degree_Celsius */
	DEGREES_F,		    		/* Degrees_Fahrenheit */
	KV,	            			/* Kilo_volt */
	KW,	            			/* Kilo_Watt */
	W,		            		/* Watt */
	CZK_MWH,		    		/* CZK/MWh */
	PLZ_MWH,		    		/* PLZ/MWh */
	EUR_MWH,		    		/* EUR/MWh */
	DKK_MWH = 840,	    		/* DKK/MWh */
	RUR_MWH,		    		/* RUR/MWh */
	DKK_GJ,						/* DKK/GJ */
	EUA,						/* EUA (EU emission Allowances volume) */
	EUR_EUA,					/* EUR/EUA */
	CZK_EUA,					/* CZK/EUA */
	MW_MW,						/* MW/MW (Virkningsgrad) */
	L_H,						/* l/h */
	L,							/* l */
	MM_H,						/* mm/h */
	MJ_SM3 = 850,				/* MJ/Sm3 */
	KWH_SM3,					/* kWh/Sm3 */
	KG_SM3,						/* kg/Sm3 */
	MOL_PERC,					/* mol% (Vanninnhold i gass i prosent) */
	TS_SEK = 999,				/* TS/sek */
	/* Start of corrency area */
	UNMEA_CURRENCY_MIN=1000,	/* NOK */
	UNMEA_NOK_CURRENCY=1000,	/* NOK */
	UNMEA_SEK_CURRENCY,			/* SEK */
	UNMEA_DKK_CURRENCY,			/* DEK */
	UNMEA_FIM_CURRENCY,			/* FIM */
	UNMEA_DEM_CURRENCY,			/* DEM */
	UNMEA_RUR_CURRENCY,			/* RUR */
	UNMEA_NLG_CURRENCY,			/* NLG */
	CZK,						/* Czech_koruna */
	PLN,						/* Poland_zloty */
	UNMEA_GBP_CURRENCY,			/* British Pound sterling */
	UNMEA_EUR_CURRENCY=1999,	/* Euro */
	UNMEA_CURRENCY_MAX=1999,	/* Euro */
	MWM3S = 2000,				/*MW/m3/s*/
	EUR_MW = 2001,				/*(EUR/MW)*/
	USD_MW = 2002,				/*(USD/MW)*/
	SEK_MW = 2003,				/*(SEK/MW)*/
	NOK_MW = 2004,				/*(NOK/MW)*/
	DKK_MW = 2005,				/*(DKK/MW)*/

	EUR_MM3 = 2006,				/*(EUR/Mm3)*/
	USD_MM3 = 2007,				/*(USD/Mm3)*/
	SEK_MM3 = 2008,				/*(SEK/Mm3)*/
	NOK_MM3 = 2009,				/*(NOK/Mm3)*/
	DKK_MM3 = 2010,				/*(DKK/Mm3)*/

	EUR_MM3s = 2011,				/*(EUR/Mm3/s)*/
	USD_MM3s = 2012,				/*(USD/Mm3/s)*/
	SEK_MM3s = 2013,				/*(SEK/Mm3/s)*/
	NOK_MM3s = 2014,				/*(NOK/Mm3/s)*/
	DKK_MM3s = 2015,				/*(DKK/Mm3/s)*/


	/* End of corrency area */
	NO_UNIT = NO_SHORT_VALUE
};


/*
 *	Time series intervals.
 */
enum ICC_TSINT
{
	VARI_INT = -1,				/* !? */
	UNDEF_TSINT = 0,
	MILLISEC_1 = 1,				/* 1 millisecond	    */
	MILLISEC_10,				/* 10 milliseconds	    */
	MILLISEC_50,				/* 50 milliseconds	    */
	MILLISEC_100,				/* 100 milliseconds	    */
	SECOND_1 = 11,				/* 1 second				*/
	SECOND_5,					/* 5 seconds		    */
	SECOND_15,					/* 15 seconds		    */
	SECOND_30,					/* 30 seconds		    */
	MINUTE_1 = 21,				/* 1 minute				*/
	MINUTE_5,					/* 5 minutes		    */
	MINUTE_15,					/* 15 minutes		    */
	MINUTE_30,					/* 30 minutes		    */
	MINUTE_3,					/* 3 minutes		    */
	MINUTE_10,					/* 10 minutes		    */
	HOUR_1 = 101,				/* 1 hour				*/
	HOUR_3,						/* 3 hours				*/
	HOUR_6,						/* 6 hours				*/
	HOUR_12,					/* 12 hours				*/
	HOUR_24,					/* 24 hours				*/
	HOUR_2,						/* 2 hours				*/
	HOUR_4,						/* 4 hours				*/
	HOUR_8,						/* 8 hours				*/
	DAY_1 = 111,				/* 1 day				*/
	WEEK_1 = 121,				/* 1 week				*/
	MONTH_1 = 131,				/* 1 month				*/
	YEAR_1 = 141,				/* 1 year				*/
	HOUR_1_ACC = 201,			/* 1 hour+				*/
	HOUR_3_ACC,					/* 3 hours+				*/
	HOUR_6_ACC,					/* 6 hours+				*/
	HOUR_12_ACC,				/* 12 hours+			*/
	HOUR_24_ACC		 			/* 24 hours+			*/
};

enum ICC_TSERI
{
	UNDEF_TSERI = -1,
	PRECIPITATION = 0,			/* Nedboer					*/
	EVAPORATION,				/* Fordampning				*/
	AIR_MOISTURE,				/* Luftfuktighet			*/
	WATER_VAPOUR_PRESS,			/* Vanndamptrykk			*/
	AIR_PRESSURE_1,				/* Lufttrykk ST				*/
	AIR_PRESSURE_2,				/* Lufttrykk 00				*/
	RADITATION_1,				/* Straaling global			*/
	RADITATION_2,				/* Straaling netto			*/
	RADITATION_3,				/* Straaling kortboelget	*/
	RADITATION_4,				/* Straaling langboelget    */
	ALBEDO,						/* Albedo					*/
	SUNTIME,					/* Time with sunshine	    */
	CLOUD_COVER_1,				/* Skydekke 1/8				*/
	CLOUD_COVER_2,				/* Skydekke 1/10			*/
	WINDDIR,					/* Wind direction			*/
	WINDSPEED1,					/* Wind speed 2 meter	    */
	WINDSPEED2,					/* Wind speed 10 meter	    */
	AIRTEMP1,					/* Air temperature 2 meter  */
	AIRTEMP2,					/* Air temperature 10 meter */
	AIRTEMP3,					/* Air temperature, no 	    */
								/* specific height			*/
	GUSTWIND = 30,
	WATERLEVEL = 1000,			/* Water level				*/
	WATERFLOW,					/* Water flow				*/
	WATERSPEED,					/* Water speed				*/
	WATERTEMP,					/* Water temperature		*/
	RESVOL,						/* Reservoir volume			*/
	ICE_THICKNESS,				/* Ice thickness			*/
	CONDUCTIVITY,				/* Conductivity				*/
	PHVALUE,					/* Ph-value					*/
	OVER_FLOW,					/* Overloep					*/
	GATEOPENING,				/* Gate opening				*/
	WATERDEPTH,					/* Water depth of course!!  */
	FREEBOARD = 1011,		    /* Fribord					*/
	RESERVOIR_CHANGE,		    /* Magasinendring      	    */

	INFLOW_SERIES = 1050,		/* Inflow @(Name used in    */
								/* ICC-SIM, not to be 	    */
								/* changed)@				*/
	/*  OBS! OBS!
	 *	Hvis noen faar bruk for de norske navnene, vennligst si fra til
	 *	rk eller tsa, da det er meningen disse skal oversettes til engelsk.
	 *	(kom gjerne med forslag til engelsk navn!)
	 */
	RIVER_LEVEL = 1053,				/* Vannstand i elv				*/
	VANNFORING = 1054,				/* Vannføring					*/
	Drftvannf = 1055,				/* Driftsvannfoering			*/
	SPECIFIC_FLOW = 1056,			/* Spesifikk vannføring			*/
	SPILLAGE = 1060,				/* Forbitapping					*/
	PLANT_LOSS,				        /* Tap pga. flom/forbitapping	*/
	SPILL = 1063,					/* Spill						*/
	FLOOD = 1065,					/* Flom							*/
	Konstapp = 1070,				/* Konsesjonstapping			*/
	PUMP_WATERFLOW = 1075,			/* Pump waterflow				*/
	Prodvannf = 1080,				/* Produksjonsvannfoering		*/

	Konsspmtr = 1200,				/* Konsentrasjon susp. 			*/
						/* materiale		     */
	Trassomtr = 1202,				/* Transport susp. materiale	*/
	Drmlbntra = 1204,				/* Direkte maalt 				*/
						/* bunntransport	     */
	Idrmlbntra = 1206,				/* Indirekte maalt 				*/
						/* bunntransport	     */
	Konorgmtr = 1208,				/* Konsentrajon organisk		*/
						/* materiale		     */
	Traorgmtr = 1210,				/* Transport organisk 			*/
						/* materiale		     */
	Kumkorssp = 1212,				/* Kum. kornfordelingskurve		*/
						/* susp. mat.		     */
	Kumkorbtr = 1214,				/* Kum. kornfordelingskurve		*/
						/* bunntr. mat.		     */

	Grnvannst = 2000,			/* Grunnvannstand			*/
	Markfukt,					/* Markfuktighet			*/
	Snoedybde,					/* Snoedybde				*/
	Snoevannekv,				/* Snoens vanekvivalent		*/
	Teledybde,					/* Teledybde				*/
	Poretrykk,					/* Poretrykk				*/
	Jordtemp1,					/* Jordtemp -0,1 m			*/
	Jordtemp2,					/* Jordtemp -0,2 m			*/
	Jordtemp3,					/* Jordtemp -0,5 m			*/
	Gresstemp1,					/* Gresstemp +0,1 m			*/
	Smltv,						/* Smeltevann				*/
	Smltv_og_ndb,				/* Smeltevann og nedboer	*/
	Snoareal,					/* Snoedekt areal			*/

	Gkumkorkur = 2100,			/* Gen. kum.				*/
								/* kornfordelingskurve		*/

	Spenning = 4000,			/* Spenning									*/
	Forbruk = 4010,				/* Forbruk									*/
	CONSUMPTION = 4010,			/* Normal consumption, hourly reading		*/
	CONSUMPTION_LOW,			/* Normal consumption, low price period		*/
	CONSUMPTION_HIGH,			/* Normal consumption, high price period	*/
	CONSUMPTION_METER,			/* Normal consumption, meter reading		*/
	POWER_GENERATION = 4015,	/* Power generation.						*/
	Pumping,					/* Pumping									*/
	NET_POWER_GENERATION,		/* Netto produksjon							*/
	Stroem = 4020,				/* Stroem									*/

	Avloep = 4040,				/* Avloepsserie				*/
	Energi,						/* Energiserie				*/
	Boelgekraft,				/* Boelgekraftserie			*/
	WINDSPEED3,					/* Windspeed, no specific   */
								/* height					*/
	Magasin,					/* Magasinserie				*/
	Utveksling,					/* Utvekslingsseire			*/

    POWER_MAX=4118,
    POWER_MIN,
    POWER_MEAN,

	/* ICC_Trade */
	CONTRACT_OBLIGATION = 6001,	/* Kontraksforpliktelse		*/

	/* ICC-REG */
	RESULT_DAY_PRICE_TS = 8001,	/* Resulting price of bid on
								   the daily market         */
	RESULT_REG_PRICE_TS,		/* Resulting price of bid on
								   the regulation market    */
	REG_BID_INFO_TS,			/* Information about each bid
								   on the regulation market */
	/* HBV */
	SIM_AVLOEP_UTEN_TILBAKEK = 8100,	/* Sim avloep uten tilbakekobling */
	SIM_AVLOEP_MED_TILBAKEK,			/* Sim avloep med  tilbakekobling */
	AKKUMULERT_DIFFERANSE,				/* Akkumulert differanse */

	/* NHL */
	WATER_QUAL = 8200,

	/* ICC-INI (ELCBAS) */
	PRODUCTION_SCHEDULE = 8300,

	/* ICC-SIM (Simulator) */
	POWER_AVAILABLE=8400,
	REG_DEGREE,
	SPIN_RESERVE,
	SLOW_RESERVE,
	STATICS,
	DEGREG_SETTING,
	DRFORRES,
	FRFORRES,
	HURTIGRES,
	UTILGEFF,
	
	
	/* EDIEL types etc. */
	DECENTRAL_GENERATION=8500,
	WINDPOWER_GENERATION,
	BALANCE_SUM_PLAN,

	/* New values start at 8400!!!! */

	TIME_SERIES = 9000,				/* gaa(280596) Time series  */
	NE_TIME_SERIES,			    	/* Network time series	 */
		ENERGY_VALUE,			    /* Energy value	 */
	CU_LOAD_RATIO,			    	/* CU load ratio */
	METER_READING,			    	/* Meter reading  */

	/* More types created from ID-Sim  */
						/* aab 12/3-97 */
	TOTAL_INFLOW = 9100,		/* Totalt tilsig */
	TOTAL_Konstapp,				/* Total konsesjonstapping */
	TOTAL_FLOOD,				/* Total flom */
	FREE_CAPACITY,				/* Kapasitet */
	LOCAL_WATERLEVEL,			/* Magasintilstand ref stas */

	TEMP_GRAD = 9200,			/* Temperaturgradient */
	PROBABIL_P5,				/* Sanns for minst 5 mm nedb */
	PROBABIL_P10,				/* Sanns for minst 10 mm nedb*/
	MEAS = 9300,				/* Måleverdi */
	BILATTRADE,					/* Bilateral handel */
	SPOTTRADE,					/* Spotmarkedshandel */
	REGTRADE,					/* Regulerkrafthandel */
	TRANSFLOSS,					/* Transformatortap */
	JIP_S_PRO,					/* JIP Levrandørprosent */
	JIP_N_SUM,					/* JIP Nettområdeandel */
	SUPPL_HOUR,					/* Leverandør timesmålt */
	SUPPL_EST,					/* Leverandør ikke timesmålt */
	CUST_HOUR,					/* Timesmålt kunde */
	SUPPL_TOT					/* Leverandør totalt */
};


/*
 *	Hydrological functions.
 */
enum ICC_HYDFN
{

	DBI_INTERNAL_TUBES_INFO = 101,		/* For internal use in the
											* simulator part of DBI.
											*/
	UNDEF_HYDFN,
	RESERVOIR_ELEVATION_STORAGE = 10000,
	RESERVOIR_OVERFLOW_DESCRIPTION,
	RESERVOIR_M3_TO_KWH,
	PLANT_SIM_EFFICIENCY = 10003,
	PLANT_SIM_PROD_COST,
	PLANT_SIM_EFF_COST,
	PLANT_SIM_EFF_CURVE,

	GENERATOR_EFFICIENCY_CURVE,

	TURBINE_EFFICIENCY_CURVE_1,
	TURBINE_EFFICIENCY_CURVE_2,
	TURBINE_EFFICIENCY_CURVE_3,

	WATERROUTE_TIME_DELAY,

	WATERROUTE_DISCHARGE_LIMITATION_1,
	WATERROUTE_DISCHARGE_LIMITATION_2,
	WATERROUTE_DISCHARGE_LIMITATION_3,

	WATERROUTE_ELEVATION_STORAGE = 10051,
	WATERROUTE_OVERFLOW_DESCRIPTION = 10052,

	GENERATOR_MAXPROD_HEAD = 10060,
	GENERATOR_STATIC_DESC = 11000,

	HYDFN_LOWER_LIMIT_CURVES = 20000,
	TURBINE_EFFICIENCY_CURVE_START = 20000,
	WATERROUTE_DISCHARGE_LIMITATION_START = 21000,
	WTR_DISCHARGE_LIMIT_2_START=21500,
	WATERROUTE_CROSS_SECTION=21600,
	GATE_FLOW_TABLE_START = 22000,
	TURBINE_POS_CURVE_START=23000, /* given net. fallheight and q[m3s] find setpoint */
	SIM_INL_GATE_CURVE_START1=24000,
	SIM_INL_GATE_CURVE_START2=25000,
	SIM_INL_GATE_CURVE_START3=26000,
	SIM_INL_GATE_CURVE_START4=27000,
	SIM_INL_GATE_CURVE_START5=28000,
	SIM_INL_GATE_CURVE_START6=29000,
	SIM_INL_GATE_CURVE_START7=30000,
	SIM_INL_GATE_CURVE_START8=31000,
	SIM_INL_GATE_CURVE_START9=32000,
	SIM_INL_GATE_CURVE_START10=33000,

	PLANT_TAILRACE_LOSS=33400,
	PLANT_HEAD_LOSS_DESCR
};


/*
 *	Operating restrictions.
 */
enum ICC_OPRRS
{
	UNDEF_OPRRS,
	RESERVOIR_MAX_CONSTRAINT = 10200,
	RESERVOIR_MIN_CONSTRAINT,
	RESERVOIR_VAR_CONSTRAINT,
	RESERVOIR_RULE_CURVE,
	RESERVOIR_UPPER_SLACK,
	RESERVOIR_LOWER_SLACK,

	PLANT_PRODUCTION_SCHEDULE,
	PLANT_MAINTENANCE,

	GENERATOR_MAINTENANCE,
	GENERATOR_SCHEDULE,

	PUMP_UPSTREAM_MAX,
	PUMP_DOWNSTREAM_MIN,
	PUMP_MAINTENANCE,
	PUMP_SCHEDULE,

	WATERROUTE_MAINTENANCE,
	WATERROUTE_MAX_CONSTRAINT,
	WATERROUTE_MIN_CONSTRAINT,
	WATERROUTE_VAR_CONSTRAINT,

	GATE_MAINTENANCE,
	GATE_PLAN,
	PLANT_WATER_VALUE,

	RULE_CURVE_FLAG = 10230,

	PLANT_SCHEDULE_FLAG = 10240,

	GENERATOR_STATIC_SETTING = 10250,
	STATIC_LOW,
	STATIC_MEAN,
	STATIC_HIGH,
	STATIC_UNIT,

	RESERVOIR_RAMPING = 11000,
	RESERVOIR_RAMP_PENALTY,
	PLANT_MIN_P_CONSTR,
	PLANT_MAX_P_CONSTR,
	PLANT_RAMPING,
	PLANT_SCHEDULE_FLAG_TIME,
	PLANT_MIP_FLAG,
	GENERATOR_SCHEDULE_FLAG_TIME,
	GENERATOR_COMMITTED,
	GENERATOR_COMMITTED_FLAG,
	PLANT_RAMP_PENALTY,
	PLANT_SCHEDULE_M3S,

	RESTR_STARTREAD=11999,
	RESTR_MAINTENANCE=12000,
	RESTR_MAXPROD=12001,
	RESTR_MINPROD=12002,
	RESTR_RAMPING_GEN=12003,
	RESTR_RAMPING_PLANT=12003,
	RESTR_MINFLOW=12004,
	RESTR_MAXFLOW=12005,
	RESTR_MAXDELTAFLOW=12006,
	RESTR_MAXVOL=12008,
	RESTR_MINVOL=12009,
	RESTR_RAMPING_RSV=12010,
	RESTR_SUBSCRIPTGROUP=12011,
	RESTR_SUBSCRIPTGROUPMAXPROD=12012,
	RESTR_MAINTENANCE_UNA=12014,
	RESTR_SUBSCRIPTGROUPMINPROD=12015,
	RESTR_BLOCK_MW=12016,
	RESTR_BLOCK_M3S=12017
};


enum ICC_HYDCO                  /* (HYD=Hyper Data?) component types */
{
	UNKNOWN_HYDCOMP=0,
	CATMAREA = 1,
	WATERROUTE,
	WATEROUTE_JUNCTION,
	RESERVOIR,
	GATE,
	PLANT,
	GENERATOR_UNIT,
	PUMP_UNIT,
	SECTION,
	SECTION_POINT,
	DATASET = 20,
	SHOP_SYSTEM,
	FOLDER,
	INDATASET=105,
	RESULTSET=106,
	RESULTSETITEM=107,
	TASK_DEFINITION=108,
	TASK_STEP_DEF=109,
	TASK_STEP_PATH_DEF=110,
	TASK_VIEW_CONFIG_DEF=111,
	TASK_VIEW_MODULE_DEF=112,
	TASK_VIEW_LAYOUT_DEF=113,
	TASK=114,
	TASK_STEP=115,
	HBV_PARAMETER_SET=116,
	AC_PARAMETER_SET=117,
	AC_RUN_OPTIONS=118,
	AC_RESULTS=119,
	AC_QUALITY=120,
	AC_PARAMETER=121,
	SIMNETAREA=122,
	UNITSELECT=123,
	/* SIGMA project introduces these types, ref. to BidContractModel for defs,
	-- convention is caps on and _ between words, related to the class name
	- e.g TradeMarket -> TRADE_MARKET
	- remember to update icc/dbi/db_schema/8.0/data/basis/TP_HYDCO_TP.dbo when chaning these.
	*/
	TRADE_MARKET=124,
	TRADE_MARKET_AREA=125,
	TRADE_MARKET_OPERATOR=126,
	TRADE_CONTRACT=127,
	TRADE_CONTRACT_PART=128,
	TRADE_BID=129,
	TRADE_BID_PART=130,
	TRADE_BID_ACTOR=131,
	TRADE_GRID_BUSBAR=132,
	TRADE_GRID_CONTROL_AREA=133,
	TRADE_GRID_AREA=134,
	TRADE_GRID_AREA_CONNECT=135,
	TRADE_MARKET_BID_RULES=136,
	TRADE_PRODUCT=137,
	TRADE_BID_CURRENCY_RULE=138,
	TRADE_PRODUCT_BID_RULES=139,
	TRADE_BID_TIMESLOT=140,
	TRADE_PERIOD_PRICE_QUANTITY=141,
	TRADE_PRICE_QUANTITY=142,
	TRADE_REG_PRICE_QUANTITY=143,
	SIGNATURE_USER=144,
	MCSCENARIO=145,
	MCSCENARIOITEM=146,
	PRICE_CURVE=147,
	/* End of SIGMA */
	OBJECT_GROUP = 148,
	GROUP_OBLIGATION = 149
};


enum ICC_TIMCO_CONNECT
{
	MIN_SELECTABLE_CONNECT			= 0,
	CONNECT_UNDEFINED      			= 0,	/* Undefined */
	CONNECT_HISTORICAL  			= 100,	/* Historical measurement */
	CONNECT_SCHEDULE    			= 200,	/* Schedule: reservoir, plant, gate etc */
	CONNECT_FORCAST_INFLOW_MANUAL	= 1900,	/* Forecast inflow manual */
	MAX_SELECTABLE_CONNECT			= 1900,

	CONNECT_AUTO_SIMMOD				= 2000,	/* Result from simulation connected to one dataset */
	CONNECT_REF_INFLOW  			= 2100,	/* Inflow reference series */
	CONNECT_FORCAST_INFLOW_HBV		= 2200,	/* Forecast inflow produced by HBV */
	CONNECT_FORCAST_INFLOW_PROTIL	= 2300,	/* Forecast inflow produced by PROTIL */
	CONNECT_INFLOW_CALC 			= 2400,	/* Result from inflow calculation */
	CONNECT_SIM_CALC    			= 2500	/* Result from simulation */
};

enum ICC_TSORG                  /* The origin of a time series */
{
    UNKNOWN_TS = 0,				/* Unknown time series */
    AUTO_MEASURED_TS = 1,		/* Automatic measured time series */
    MANUAL_MEASURED_TS = 2,		/* Manual measured time series */
    FORECAST_TS = 3,			/* Forcasted time series */
    CALCULATED_TS = 4,			/* Calculated time series */
    HISTORICAL_TS = 5,			/* Historical time series from MI, NVE etc. */
    CALCULATED_MEASURED_TS = 6,	/* Mixture of calculated and measured time series values */
    PROFILE_TS = 7,				/* Time profile time series */
    COPY_TS = 8,				/* Copy time series */
    DUMMY_TS = 100,				/* Dummy time series */
    HBV_UPDATE = 200,
    HBV_CALIBRATION = 201,
    HBV_FORC_METE = 202,
    HBV_FORC_HIST = 203,
    HBV_FORC_COMB = 204,
    HBV_FORC_ENSEMBLE = 205,
    SIM_CONT_CALC = 300,		/* Sim kontinuerlige serier */
    SIM_CONT_INFLOW = 301,		/* Sim tilsigsberegniger */
    NETWORK_TOTAL = 800,		/*gaa  type_of_series */
    NETWORK_METERED = 801,
    NETWORK_NON_METERED = 802,
    SUPPLIER_NON_METERED = 803,
    NETWORK_LOSS = 804,
    MEASURED = 900,
    CALCULATED = 901,
    OPERATIVE_SERIE = 1000,
    MANUAL_HELP_SERIE = 1100,
    NORMAL = 1200,
    OPERATIVE_NORMAL = 1300
};

enum ICC_TSPROD                 /* The producer/owner of a time series */
{
	UNDEF_TSPROD,
	TS_SIMULATOR = 1,
	TS_HBV,
	TS_HYDINN,
	TS_ELCBAS,
	TS_ESTER,
	TS_REG,
	TS_KOFOT,
	TS_TRADE,
	TS_PROTIL,
	TS_SNOMOD=10,
	TS_SHOP,
	TS_SAMK,
	TS_AUTO,
    TS_AVAILABILITY,    // 14 - used in code 
    TS_MESH,
	TS_NVE = 100,
	TS_MI,
	TS_SCANMATIC,
	TS_GS2IMPORT = 200,
	TS_EDIIMPORT,
	TS_SCADAIMPORT
};

enum ICC_WTGT                   /* Water gate types */
{
	UNDEF_WTGT,
	THROTTLE_GATE = 1,
	DISCHARGE_GATE
};


enum ICC_WTRFN                  /* Waterroute function types */
{
	UNDEF_WTRFN,
	PRODUCTION_WTROUTE = 1,
	PENSTOCK_WTROUTE,
	DRAFT_TUBE_WTROUTE,
	OVERFLOW_WTROUTE,
	BYPASS_WTROUTE
};

enum ICC_HYDPL                  /* Plant types */
{
	UNDEF_HYDPL,
	GENERATING_PLANT = 1,
	PUMP_PLANT,
	COMBINED_PLANT				/* Both generating and pump plant */
};

enum ICC_MEMEA					/* Method of measurement for a sensor */
{
	UNDEF_MEMEA = -1,
	SENS_MOMENTAL_VL = 0,
	SENS_MAXIMAL_VL,
	SENS_MINIMAL_VL,
	SENS_MEAN_VL,
	SENS_DELTA_VL,
	SENS_SUM_VL,
	SENS_INDEPEND_MOMENTAL_VL
};

enum ICC_FIEST                  /* Field station types */
{
	UNDEF_FIEST,
	MEAS_DISCARGE=1,
	MEAS_TEMPERATURE,
	MEAS_CLIMATE,
	MEAS_PRECIPITATION,
	MEAS_WIND_SPEED,
	MEAS_WIND_DIRECTION,

	FORC_DISCARGE=21,
	FORC_TEMPERATURE,
	FORC_CLIMATE,
	FORC_PRECIPITATION,
	FORC_WIND_SPEED,
	FORC_WIND_DIRECTION
};

enum ICC_CCMET                  /* Control and correction methods for TS */
{
	UNDEF_CCMET = -1,
	TS_VALUE_OK = 0,
	ABSLIMIT = 211,
	DELTALIMIT,
	DELTA_E,
	REPVALUE,
	CTRL_METER,
	VAL_METER_SUM,
	POWER_FAILURE_CON,
	CALC_EXPRESSION,
	CONSTANT = 311,
	COPYVALUE,
	INTERPOLATE,
	EXTRAPOLATE,
	MEANSIMULTS,
	REGRESSION,
	MEANSIMDELTAT,
	/* energikontroll er 318 */
	CORR_METER_SUM=319,
	POWER_FAILURE_CORR
};

enum TASK_STATE
{
	STATE_UNDEFINED		= 0,
	STATE_NOT_STARTED	= 1,
    STATE_STARTED		= 2,
    STATE_FINISHED		= 3,
	STATE_SUSPECT		= 4,
    STATE_FAILED		= 5
};

enum TASK_TYPE
{
	POST_SPOT	= 1,
	PRE_SPOT	= 2, 
    BDM_TASK = 3
};

#define TS_VL_COR_SBIT		0
#define TS_VL_VAL_SBIT		5
#define TS_VL_EXT_SBIT		10

enum ICC_TS_VL_STATUS	/* Time series status values */
{
	TS_VL_OK				=          0,						/* Clear all bits  */
																/* Bit 31: NOT AVAILABLE (due to problems) */
	TS_VL_NOT_OK			= 0x40000000,						/* Bit 30: 1=value is NOT OK */
	TS_VL_VALIDATED			= 0x20000000,						/* Bit 29: 0=not validated */
	TS_VL_CORRECTED			= 0x10000000,						/* Bit 28: 0=not corrected */
	TS_VL_MANUAL			= 0x08000000,						/* Bit 27: 0=not manual value*/
	TS_VL_MISSING			= 0x04000000,						/* Bit 26: 0=not missing */
	TS_VL_SUSPECT			= 0x02000000,						/* Bit 25: 1=Suspect/Calcres with miss.vals*/
	TS_VL_LOCKED			= 0x01000000,						/* Bit 24: 1=value is locked for changes*/
	TS_VL_ACCEPTED			= 0x00800000,						/* Bit 23: 1=value is manually accepted*/
	TS_VL_MOVEFLAG			= 0x00400000,						/* Bit 22: 1=value is at the end of an agreement, i.e. customer is moved */
	TS_VL_ESTIMATED			= 0x00200000,						/* Bit 21: 1=value is manually estimated */
	TS_VL_VIRTUALPOINT		= 0x00100000,						/* Bit 20: 1=value is transformed/calculated */
	TS_VL_DEFAULT			= 0x00080000,						/* Bit 19: 1=value is explicitly defined to be as default */
	TS_VL_TB_INSPECTED		= 0x00040000,						/* Bit 18: 1=value treated as OK - but suspicious: to be inspected */
	TS_VL_DELETED			= 0x00020000,						/* Bit 17: 1 = value treated as deleted */
															/* Bit 16-15: Not used yet */
															/* Bit 14-10: Customer usage.
															 *	      NOT AVAILABLE FOR Powel Data,
															 *	      except for:*/
	//TS_VL_EXT_CTRL_READ		=  26 << TS_VL_EXT_SBIT,	/* method 26: Control reading flag */
	//TS_VL_EXT_METER_CHANGE	=  27 << TS_VL_EXT_SBIT,	/* method 27: Reading because a change of meter operation */
	//TS_VL_EXT_NEW_METER		=  28 << TS_VL_EXT_SBIT,	/* method 28: Reading because a new meter installation */
	//TS_VL_EXT_REMOVE_METER	=  29 << TS_VL_EXT_SBIT,	/* method 29: Reading because a meter removal operation */
	TS_VL_EXT_MAN_READ			=  30 << TS_VL_EXT_SBIT,	/* method 30: Manual reading flag */
	TS_VL_EXT_DEBITFLAG			=  31 << TS_VL_EXT_SBIT,	/* method 31: Debit flag */

	/* Bit  9-5: Powel Data validation methods */
	TS_VL_VAL_CALC_EXPRESSION = 16 << TS_VL_VAL_SBIT,		/* calculated expressioin */
	TS_VL_VAL_POWER_FAILURE =   11 << TS_VL_VAL_SBIT,    /* power failure */
	TS_VL_VAL_METER_SUM		=   6 << TS_VL_VAL_SBIT,		/* meter sum*/
	TS_VL_VAL_CTRL_METER	=   5 << TS_VL_VAL_SBIT,		/* control meter */
	TS_VL_VAL_REPVALUE		=   4 << TS_VL_VAL_SBIT,		/* repeat value */
	TS_VL_VAL_DELTA_E		=   3 << TS_VL_VAL_SBIT,		/* delta epsilon */
	TS_VL_VAL_DELTALIMIT	=   2 << TS_VL_VAL_SBIT,		/* delta value */
	TS_VL_VAL_ABSLIMIT		=   1 << TS_VL_VAL_SBIT,		/* abolute value */

	/* Bit 4-0: Powel Data correction methods */
	TS_VL_COR_POWER_FAILURE =  11 << TS_VL_COR_SBIT,	/* power failure */
	TS_VL_COR_METER_SUM		=  10 << TS_VL_COR_SBIT,		/* meter sum */
	TS_VL_COR_MEANSIMDELTAT	=   9 << TS_VL_COR_SBIT,		/* mean value delta t */
	TS_VL_COR_ENERGY		=   8 << TS_VL_COR_SBIT,		/* energy control */
	TS_VL_COR_MANUAL		=   7 << TS_VL_COR_SBIT,		/* manual corrected */
	TS_VL_COR_REGRESSION	=   6 << TS_VL_COR_SBIT,		/* regression */
	TS_VL_COR_MEANSIMULTS	=   5 << TS_VL_COR_SBIT,		/* mean value */
	TS_VL_COR_EXTRAPOLATE	=   4 << TS_VL_COR_SBIT,		/* extrapolated value */
	TS_VL_COR_INTERPOLATE	=   3 << TS_VL_COR_SBIT,		/* interpolated value */
	TS_VL_COR_COPYVALUE		=   2 << TS_VL_COR_SBIT,		/* copy old value */
	TS_VL_COR_CONSTANT		=   1 << TS_VL_COR_SBIT			/* constant value */
};

/* Mask for all correction methods. */
#define TS_VL_COR_METHOD_MASK	( 0x1f << TS_VL_COR_SBIT )
#define TS_VL_COR_MASK		( TS_VL_CORRECTED + TS_VL_COR_METHOD_MASK )

/* Mask for all validation methods. */
#define TS_VL_VAL_METHOD_MASK	( 0x1f << TS_VL_VAL_SBIT )
#define TS_VL_VAL_MASK		( TS_VL_VALIDATED + TS_VL_VAL_METHOD_MASK )

/* Mask for all external methods (i.e: comment to times series value). */
#define TS_VL_EXT_MASK		( 0x1f << TS_VL_EXT_SBIT )

enum ICC_TS_REGREASON {			/* REGREASON_TP - registration reason */
	TS_VL_REAS_UNDEF=0,			/* Undefined */
	TS_VL_REAS_CYCLIC=1,		/* Cyclic reading */
	TS_VL_REAS_CHANGE_METER=2,	/* Meter change */
	TS_VL_REAS_EXTRA=3,			/* Extra reading */
	TS_VL_REAS_SUPPLIER=4,		/* Change of supplier */
	TS_VL_REAS_CONTROL=5,		/* Control reading */
	TS_VL_REAS_NEW_METER=6,		/* New meter */
	TS_VL_REAS_REMOVE_METER=7,	/* Remove meter */
	TS_VL_REAS_MANUAL=8			/* Manual reading */
};

enum ICC_TS_READING {		/* READING_TP - registration type */
	TS_VL_READ_UNDEF=0,		/* Undefined */
	TS_VL_READ_MANUAL=14,	/* Manual reading */
	TS_VL_READ_AUTO=15,		/* Remote/automatic reading */
	TS_VL_READ_INTERPOL=16,	/* Calculated by interpolation */
	TS_VL_READ_EXTRAPOL=17	/* Calculated by extrapolation */
};

/*
 * TS_VERSION.VERSION_TYPE
 */
enum ICC_TS_VERSION_TYPE {
    TSV_ALTERNATIVES = 2,
    TSV_CPS_SCENARIO = 8,
    TSV_SCENARIO = 9,
    // NS is Normal Series type
    TSV_NS_PRECOMP = 10,
    // TSV_NS_<method>   (P05 is 5% percentile)  
    TSV_NS_MEAN = 11,
    TSV_NS_MEDIAN,
    TSV_NS_MIN,
    TSV_NS_MAX,
    TSV_NS_P05,
    TSV_NS_P10,
    TSV_NS_P15,
    TSV_NS_P20,
    TSV_NS_P25,
    TSV_NS_P30,
    TSV_NS_P35,
    TSV_NS_P40,
    TSV_NS_P45,
    TSV_NS_P50,
    TSV_NS_P55,
    TSV_NS_P60,
    TSV_NS_P65,
    TSV_NS_P70,
    TSV_NS_P75,
    TSV_NS_P80,
    TSV_NS_P85,
    TSV_NS_P90,
    TSV_NS_P95,
	TSV_CUSTOMER_COPY = 40,
	TSV_CUSTOMER_COPY_MAX,
	TSV_DAY_DEGREE = 50,
	TSV_FE_FV_CONN,
	TSV_VOLUME_ENERGY_RATIO,
	TSV_NORMAL_TS
};

/*
 * Water route
 */
enum ICC_WTRT
{
	UNDEF_WTRT,
	TUNNEL_REACH=1,
	RIVER_REACH,
	CHANNEL_REACH
};


/*
 * Lake
 */
enum ICC_RESVO
{
	UNDEF_RESVO,
	R_RESERVOIR=1,	/* A regulated lake. */
	LAKE,			/* A unregulated lake. */
	CREEK			/* A creek inflow element. */
};



/*
 * Samanheng mellom komponentar
 */
enum ICC_HYRETYPE
{
	UNDEF_HYRETYPE,
	OWNERSHIP_RELATION = 1,		/* Describes owner <-> member relationships				 */
	REFERENCE_RELATION			/* Describes reference relationships, such as water movement 
								   between two components and items contained in a folder */
};


/*
 *	Database access codes (table UACCESS)
 */
enum ICC_ACCESS
{
	ACCESS_UNDEFINED = -1,		/* Used for just initialized time series; access rights not read from DB */
	DB_NOACCESS = 0,	/* No access */
	DB_READONLY = 1,	/* Read only access */
	DB_READWRITE = 2,	/* Read/write (update) access */
	DB_DELETE = 3			/* Delete access (includes read/write) */
};

/*
 *	Types of organisation types.
 */
enum ORG_TYPES
{
	ORG_CUSTOMER = 1,	/* Customers, table OPRTUNIT */
	ORG_NETAREA,		/* Netareas, table NETAREA */
	ORG_EXCH_PNT,		/* Exchange points, table EXCH_PNT */
	ORG_MEAS_PNT,		/* Measurement points, table MEAS_PNT */
	ORG_INSTRUMENT,		/* Instruments, table INSTRUMENT */
	ORG_MEAS_TS = 10,	/* Registering time series, table TSCHA_VL */
	ORG_1008_TS = 1000	/* Calculated 1008 series */
};

/*
 *	Exchange point types (table EXCHP_TP)
 */
enum EXCHP_TP
{
	EXCHP_BORDER = 1, 			/* Exchange point between two net areas */
	EXCHP_IN=2,					/* Exchange point, input unspecified */
	EXCHP_OUT_NORMAL=3,			/* Exchange point, output normal type */
	EXCHP_OUT_DISRUPT=4,		/* Exchange point, output disruptable type */
	EXCHP_OTHER=5,				/* Exchange point, other type */
	EXCHP_BORDER_POS=6,			/* EXCHP_BORDER, only positive values */
	EXCHP_BORDER_NEG=7,			/* EXCHP_BORDER, only negative values */
	EXCHP_IN_IN=8,				/* EXCHP_IN, only input values */
	EXCHP_IN_OUT=9,				/* EXCHP_IN, only output values */
	EXCHP_OUT_IN=10,			/* EXCHP_OUT_NORMAL, only input values */
	EXCHP_OUT_OUT=11,			/* EXCHP_OUT_NORMAL, only output values */
	EXCHP_OTHER_POS=12,			/* EXCHP_OTHER, only positive values */
	EXCHP_OTHER_NEG=13,			/* EXCHP_OTHER, only negative values */
	EXCHP_EST_LOSSES=14,		/* Exchange point for estimated losses */
	EXCHP_HEAT_JUNC=15,			/* District heating, junction point */
	EXCHP_HEAT_IN=16,			/* District heating, input point */
	EXCHP_HEAT_OUT=17,			/* District heating, load point */
	EXCHP_IN_WATER=19,			/* Exchange point, input water power */
	EXCHP_IN_WIND=20,			/* Exchange point, input wind power */
	EXCHP_IN_THERMAL=21,		/* Exchange point, input thermal power */
	EXCHP_IN_NUCLEAR=22,		/* Exchange point, input nuclear power */
	EXCHP_IN_GAS_DIESEL=23,		/* Exchange point, input gas or diesel power */
    EXCHP_IN_WATER_IN=24,       /* Exchange point, input water power, only input values */
    EXCHP_IN_WATER_OUT=25,      /* Exchange point, input water power, only output values */
    EXCHP_IN_WIND_IN=26,        /* Exchange point, input wind power, only input values */
    EXCHP_IN_WIND_OUT=27,       /* Exchange point, input wind power, only output values */
    EXCHP_IN_THERMAL_IN=28,     /* Exchange point, input thermal power, only input values */
    EXCHP_IN_THERMAL_OUT=29,    /* Exchange point, input thermal power, only output values */
    EXCHP_IN_NUCLEAR_IN=30,     /* Exchange point, input nuclear power, only input values */
    EXCHP_IN_NUCLEAR_OUT=31,    /* Exchange point, input nuclear power, only output values */
    EXCHP_IN_GAS_DIESEL_IN=32,  /* Exchange point, input gas or diesel power, only input values */
    EXCHP_IN_GAS_DIESEL_OUT=33,  /* Exchange point, input gas or diesel power, only output values */
    EXCHP_IN_WAVE=34,			/* Exchange point, input wave power */
    EXCHP_IN_WAVE_IN=35,		/* Exchange point, input wave power, only input values */
    EXCHP_IN_WAVE_OUT=36,		/* Exchange point, input wave power, only output values */
    EXCHP_IN_SUN=37,			/* Exchange point, input sun power */
    EXCHP_IN_SUN_IN=38,			/* Exchange point, input sun power, only input values */
    EXCHP_IN_SUN_OUT=39,		/* Exchange point, input sun power, only output values */
	EXCHP_OTHER_NO_CUSTOMER=40,	/* Exchange point, other type - no customer*/
	EXCHP_OTHER_NO_CUSTOMER_POS=41,	/* EXCHP_OTHER_NO_CUSTOMER, only positive values */
	EXCHP_OTHER_NO_CUSTOMER_NEG=42,	/* EXCHP_OTHER_NO_CUSTOMER, only negative values */
	EXCHP_LOCALGEN_IN=43,		/* Exchange point, local generation, aggregated positive values */
	EXCHP_LOCALGEN_OUT=44,		/* Exchange point, local generation, aggregated negative values */
	EXCHP_LOCALGEN_WATER_IN=45,	/* Exchange point, local generation water, aggregated positive values */
	EXCHP_LOCALGEN_WATER_OUT=46,/* Exchange point, local generation water, aggregated negative values */
	EXCHP_LOCALGEN_WIND_IN=47,	/* Exchange point, local generation wind, aggregated positive values */
	EXCHP_LOCALGEN_WIND_OUT=48,	/* Exchange point, local generation wind, aggregated negative values */
	EXCHP_LOCALGEN_THERMAL_IN=49,/* Exchange point, local generation thermal, aggregated positive values */
	EXCHP_LOCALGEN_THERMAL_OUT=50,/* Exchange point, local generation thermal, aggregated negative values */
	EXCHP_LOCALGEN_GAS_DIESEL_IN=51,/* Exchange point, local generation gas or diesel, aggregated positive values */
	EXCHP_LOCALGEN_GAS_DIESEL_OUT=52,/* Exchange point, local generation gas or diesel, aggregated negative values */
	EXCHP_LOCALGEN_WAVE_IN=53,	/* Exchange point, local generation wave, aggregated positive values */
	EXCHP_LOCALGEN_WAVE_OUT=54,	/* Exchange point, local generation wave, aggregated negative values */
	EXCHP_LOCALGEN_SUN_IN=55,	/* Exchange point, local generation sun, aggregated positive values */
	EXCHP_LOCALGEN_SUN_OUT=56,	/* Exchange point, local generation sun, aggregated negative values */
	EXCHP_OUT_HOUR_MONTHLY=57,	/* Exchange point, output, hourly read but monthly debited */
	EXCHP_OUT_NORMAL_50MW=58,	/* Exchange point, output normal, more than 50MW */
	EXCHP_IN_1MW=59,			/* Exchange point, input unspecified, less than 1MW */
	EXCHP_IN_WATER_1MW=60,		/* Exchange point, input water power, less than 1MW */
	EXCHP_IN_WIND_1MW=61,		/* Exchange point, input wind power, less than 1MW */
	EXCHP_IN_THERMAL_1MW=62,	/* Exchange point, input thermal power, less than 1MW */
	EXCHP_IN_NUCLEAR_1MW=63,	/* Exchange point, input nuclear power, less than 1MW */
	EXCHP_IN_GAS_DIESEL_1MW=64,	/* Exchange point, input gas or diesel power, less than 1MW */
	EXCHP_IN_WAVE_1MW=65,		/* Exchange point, input wave power, less than 1MW */
	EXCHP_IN_SUN_1MW=66,		/* Exchange point, input sun power, less than 1MW */
	EXCHP_SUM_IN_1MW = 67,		/* Exchange point for reporting sum input unspecified, less than 1MW */
	EXCHP_SUM_IN_WATER_1MW = 68,		/* Exchange point for reporting sum input unspecified, less than 1MW */
	EXCHP_SUM_IN_WIND_1MW = 69,			/* Exchange point for reporting sum input unspecified, less than 1MW */
	EXCHP_SUM_IN_THERMAL_1MW = 70,		/* Exchange point for reporting sum input unspecified, less than 1MW */
	EXCHP_SUM_IN_NUCLEAR_1MW = 71,		/* Exchange point for reporting sum input unspecified, less than 1MW */
	EXCHP_SUM_IN_GAS_DIESEL_1MW = 72,	/* Exchange point for reporting sum input unspecified, less than 1MW */
	EXCHP_SUM_IN_WAVE_1MW = 73,			/* Exchange point for reporting sum input unspecified, less than 1MW */
	EXCHP_SUM_IN_SUN_1MW = 74			/* Exchange point for reporting sum input unspecified, less than 1MW */
};

/*
 *	Database object types (Table OBJECT_CHANGED - the OBJECT_TYPE )
 */
enum PD_OBJECT_TYPE {
	PD_UNKNOWN_OBJECT_TYPE = 0, 			/* Unknown object type */
	PD_TIMES_SERIES_OBJECT_TYPE = 1,		/* Times series (TIMESER.TIMS_KEY) object */
	PD_HYDRAULIC_COMPONENT_OBJECT_TYPE = 2,	/* Hydcomp (HYDCOMP.HYD_KEY) object */
	PD_TRADE_OBJECT_TYPE = 3,				/* Trade object (REF_TROBJECT.TRADE_KEY) */
	PD_CALCSET_OBJECT_TYPE = 4,
	PD_VIRTUAL_TIMES_SERIES_OBJECT_TYPE = 5,/* Virtual time series definition (RC_SET.RC_KEY) */

	PD_CUSTOMER_OBJECT_TYPE = 6,			/* Operator (OPRTUNIT.OPUN_KEY) */
	PD_NETARE_OBJECT_TYPE = 7,				/* Net area (NETAREA.NETA_KEY) */
	PD_STD_SCENARIO_TEMPLATE = 8,			/* Scenario definitions/template for standard report scenarios */
	PD_STD_SCENARIO = 9,					/* Scenario definitions for standard report scenarios */
	PD_SIM_SCENARIO_APPLICABLE = 10,		/* Sim scenario actual/work scenario */
	PD_SIM_SCENARIO_CURRENT = 11,			/* Sim scenario current plan scenario */
	PD_SIM_SCENARIO_USER_DEFINED = 12,		/* Sim scenario user defined scenario */
	PD_SIM_SCENARIO_REPLAN = 13,			/* Sim scenario replan scenario*/
	PD_SIM_SCENARIO_DERIVED = 14,			/* Sim scenario derived scenario*/
	PD_TSGROUP_OBJECT_TYPE = 15,			/* Times series group (TSGROUP.TSGR_KEY) */
	PD_HYDRAULIC_COMPONENT_PLANT = 16,		/* Hm! Variant av 2. */
	PD_HYDRAULIC_COMPONENT_RESERVOIR = 17,	/* -"- */
    PD_WORKORDER_OBJECT_TYPE = 18,          /* MDMS Work Order type */
    PD_COMPONENT_OBJECT_TYPE = 19,          /* MDMS generic Component type */
	PD_METERING_IMPDEF_CHANGED_ORDERS = 900, /* Used to track changes to import definitions when communicating with CIS and Metering Systems */

    PD_REPORT_HIST_TEMPLATE = 20,            // TSS/historical TS specific report template (RC_SET.RC_KEY)
	PD_REPORT_TEMPLATE = 21,                 // Mesh generic report template (RC_SET.RC_KEY)
	PD_VTS_TEMPLATE = 22,                    // Mesh calculation (VTS) template (RC_SET.RC_KEY)

    PD_TIME_SERIES_INSERTED = 100,           // (TIMESER.TIMS_KEY)
    PD_TIME_SERIES_DELETED = 101,            // (TIMESER.TIMS_KEY)
    PD_TIME_SERIES_INFO_CHANGED = 102,       // (TIMESER.TIMS_KEY)
    PD_RC_SET_DELETED = 103,                 // (RC_SET.RC_KEY)
    PD_AVAILABILITY = 104,                   //RESTR_COMP_CON.COMP_KEY
    PD_TASK = 114,                           //HYDCOMP.HYD_KEY with HYDCOMP.HYDT_KEY = 114 <-> Nimbus task
    PD_TASK_STEP = 115,                      //HYDCOMP.HYD_KEY with HYDCOMP.HYDT_KEY = 114 <-> Nimbus task step
    PD_TIME_MACRO_VTS = 99999,               //No associated DB key. The value is used to signal that Vts contains time macros
};

#ifndef __cplusplus
typedef enum ICC_UNMEA 		ICC_UNMEA;
typedef enum ICC_TSINT 		ICC_TSINT;
typedef enum ICC_TSERI 		ICC_TSERI;
typedef enum ICC_HYDFN 		ICC_HYDFN;
typedef enum ICC_OPRRS 		ICC_OPRRS;
typedef enum ICC_HYDCO 		ICC_HYDCO;
typedef enum ICC_TIMCO_CONNECT	ICC_TIMCO_CONNECT;
typedef enum ICC_TSORG 		ICC_TSORG;
typedef enum ICC_TSPROD 	ICC_TSPROD;
typedef enum ICC_WTGT 		ICC_WTGT;
typedef enum ICC_WTRT 		ICC_WTRT;
typedef enum ICC_WTRFN 		ICC_WTRFN;
typedef enum ICC_HYDPL 		ICC_HYDPL;
typedef enum ICC_MEMEA 		ICC_MEMEA;
typedef enum ICC_FIEST 		ICC_FIEST;
typedef enum ICC_CCMET 		ICC_CCMET;
typedef enum ICC_RESVO		ICC_RESVO;
typedef enum ICC_HYRETYPE	ICC_HYRETYPE;
typedef enum ICC_TS_VL_STATUS	ICC_TS_VL_STATUS;
typedef enum ICC_ACCESS		ICC_ACCESS;
typedef enum ORG_TYPES		ORG_TYPES;
typedef enum EXCHP_TP		EXCHP_TP;
typedef enum PD_OBJECT_TYPE	PD_OBJECT_TYPE;
#endif /* __cplusplus */


/*	Externals and statics:
 */

/*	Function prototypes
 */

/*****************************************************************************/

#endif /* _ICC_DB_H_*/
