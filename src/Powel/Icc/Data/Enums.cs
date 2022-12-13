//using System;

namespace Powel.Icc.Data
{
	public enum AccessCode { None=0, ReadOnly=1, Write=2, Delete=3 }	
	
	public enum ATTR_TYPE 
	{
	// Common paramaters. Is legal for more than one component type 
	DESCRIPT =  397 ,
	TYPE_KEY =  28 ,
	ENERFACT =  57 ,
	OWNER_PCT =  962 ,
	NAME =  3 ,
	GENUNNO =  71 ,
	PSTOCK_NO =  72 ,
	DTUBE_NO =  73 ,
	INLGT_NO =  694 ,
	NOMFLOW =  895 ,
	// reservoir parameters
	RESERVOIR_WTR_VALUE =  66 ,
	RESERVOIR_ENDWINDOW =  766 ,
	RESERVOIR_PENALTY_COST =  960 ,
	RESERVOIR_MXVOL =  6 ,
	RESERVOIR_HRV =  8 ,
	RESERVOIR_LRV =  7 ,
	RESERVOIR_FLAG = 24, 
	// plant parameters
	PLANT_RESERVE_FLAG =  964 ,
	PLANT_SHOP_SIM_FLAG =  831 ,
	PLANT_CONTLINE =  58 ,
	PLANT_NO_PSTOC =  55 ,
	PLANT_NO_DTUBE =  56 ,
	PLANT__FLAG =  65 ,
	// generator parameters:
	GENERATOR_STARTCOST =  366 ,
	GENERATOR_BID_FLAG =  763 ,
	GENERATOR_PRIORITY =  771 ,
	GENERATOR_MINPROD =  77 ,
	GENERATOR_MAXPROD =  74 ,
	GENERATOR_NOMPROD =  76 ,
	

	// waterroute parameters:
	WTR_HEADLOSS =  31 ,
	WTR_MXFLOW =  29 ,
	WTR_STCONT =  136 ,
	WTR_ENDCONT =  33 ,
	WTR_TIMEDELAY =  30 ,
	WTR_WTRF_KEY =  1011 ,
	// gate parameters:
	GATE_LENGTH =  83 ,
	GATE_TOPLEVEL =  84 ,
	GATE_BOTLEVEL =  85 ,
	GATE_CONSTVAL =  86 ,
	GATE_CORRFACT =  87 ,
	GATE_NUMBER =  80 ,
	GATE_PRIORITY = 1138,
	// shop parameters
	SHOP_RAMPING =  1012,
	SHOP_STARTRES =  1013 ,
	SHOP_PROG_INFLOW =  1014 ,
	SHOP_RESTRICT =  1015 ,
	SHOP_HEAD_OPT =  1016 ,
	SHOP_END_RSV =  1017 ,
	SHOP_RSV_RAMP =  1018 ,
	SHOP_LOAD_PEN =  1019 ,
	SHOP_TAPP_RAMP =  1020 ,
	SHOP_MIPGAP =  1021,
	SHOP_TIMELIMIT =  1022 ,
	SHOP_MESS =  1023 ,
	SHOP_MESSVALUE =  1024,
	SHOP_RULE_SMALL= 1025,
	// Subscription group time series.
	LOCAL_NET_CONSUMPTION = 7000,
	INTERNAL_CONSUMPTION_LOSS = 7001

}

	public enum NotificationStatus : short
	{
		Unprocessed = 0,
		NoAlarm = 1,
		Alarm = 2,
		Acknowledged = 3
	}

	public enum NotificationFilterCondition
	{
		Equals = 0,
		NotEquals = 1,
		LessThan = 2,
		LessThanOrEqual = 3,
		GreaterThan = 4,
		GreaterThanOrEqual = 5
	}

	public enum SimCompType		// Types of Simulator watercourse models in COMP_TP table
	{
        Undefined = -1, // Missing in DB. Formally it may happen because HYDCOMP.HYDT_KEY has no NOT NULL constraint
        Catma = 1, // Nedslagsfelt
		WTR		= 2, // Vannveg
		WTR_J	= 3, // Knutepunkt
		RSV		= 4, // Magasin
		Gate	= 5, // Luke
		Plant	= 6, // Stasjon
		Gunit	= 7, // Aggregat
		Pump	= 8, // Pumpe
		Sec		= 9, // Seksjon
		SecP	= 10, // Seksjonspunkt
		SubGr	= 23 // SubScription group
	}

	public enum CompAttribute	// Subset of values found in OPRRS_TP
	{
		// Oprt_keys for Simrev (old restriciton) module.
		Old_Rsv_Max = 10200, // Max rsv
		Old_Rsv_Min = 10201,
		Old_Rsv_Var = 10202,
		Old_Rev_Plan = 10207,
		Old_Rev_Gunit = 10208,
		Old_Wtr_Max=10215,
		Old_Wtr_Min = 10216,
		Old_Wtr_Var = 10217,
		Old_Gate_restr = 10219,

		// Oprt_keys for Availability module
		Avail = 12000,	// Availability
		PMax = 12001,	// Max Production
		PMin = 12002,	// Min Production
		PVar = 12003,	// Max Production variation
		QMin = 12004,	// Min flow
		QMax = 12005,	// Max flow
		QVar = 12006,	// Max variation
		NReg = 12007,	// Max regulation per day
		VMax = 12008,	// Max volume
		VMin = 12009,	// Min volume
		VVar = 12010,	// Max volume variation
		Subscr = 12011,	//Nett subscription
		SubscrP = 12012,//Nett subscription PMax
		SubscrC = 12013,//Nett subscription consume
//		Unava = 12014, // Unavailable because of revision
		SubscrPmi = 12015,//Nett subscription PMin
		PBlock = 12016, // Block generation production
		QBlock = 12017, // Block generation flow
		CAvail = 12018, // All revisions
		GQMaxMean = 12019, // Total max average flow
		LQMaxMean = 12020, // Local max average flow
		GQMinMean = 12021, // Total min average flow
		LQMinMean = 12022, // Local min average flow
		Priority =12023, // 
		Weight=12024,
        RampUp= 12025, // Max level variation Shop up
        RampDown = 12026, // Max level variation Shop down
    }
	
	public enum CurveType : short
	{
		Step=-1,
		Linear=0
	};

    public enum PrimaryValueType : short
    {
        Unknown = 0,
        Accumulated = 2
    };
	
	public enum TsUnit 
	{
		Undefined=0,
		M3=100,
		Mm3=101,
		Meter=102,
		Percent=103,
		MW=104,
		GWh=105,
		MWh=106,
		Wh=107,
		M3PerSec=108,
		LitrePerSec=109,
		LitrePerSecPerKm2=110,
		M2=111,
		Km2=112,
		Degrees=113,
		DegreesCelsius=114,
		Kilogram=115,
		Pascal=116,
		Volt=117,
		Ampere=118,
		WPerM2=119,
		MeterPerSec=120,
		//		S/=121,
		MgPerLitre=122,
		KgPerSec=123,
		Millimeter=124,
		KWPerMeter=125,
		//		NOK        126
		MNOK=127,
		NOKPerMWh=128,
		MWhPerHour=129,
		Milligram=130,
		Cm=131,
		PercentOfMax=132,
		PercentOfNorm=133,
		KWhPerHour=140,
		PU=206,
		KWh=800,
		//		C / =810,
		Hours=811,
		Hz=820,
		MVA=821,
		MVAr=822,
		MVArh=823,
		MVArhPerHour=824,
		MWPerHz=825,
		KVAr=826,
		KVArh=827,
		KVArhPerHour=828,
		//		kPa               829
		//		m3/h              830
		SEKPerMWh=831,
		MWhPerC=832,
		DegreesFahrenheit=833,
		KV=834,
		KW=835,
		LitresPerHour=847,
		NOK=1000,
		SEK=1001,
		DKK=1002,
		FIM=1003,
		DEM=1004,
		RUR=1005,
		NLG=1006,
        GBP=1009,
		EUR=1999,
		W=836,
		CZK=1007,
		PLN=1008,
		CZKPerMWh=837,
		PLZPerMWh=838,
		EURPerMWh=839,
		DKKPerMWh=840,
		RURPerMWh=841
	}

	

	public enum TsType		// Contents based on TSERI_TP table with english language
	{
		Undefined          =-1 , // Undefined
		Precipitation       =0 , // PRECIPITAION
		Vaporization        =1 , // VAPORIZATION
		Humidity            =2 , // HUMIDITY
		Steampress          =3 , // STEAM PRESSURE
		Barpres_ST          =4 , // BAROMETIC_PRESSURE_ST
		Barpres_00          =5 , // BAROMETIC_PRESSURE_00
		Rad_GLOB            =6 , // Radiatin_GLOBAL
		Rad_NET             =7 , // RADIATION_NET
		Rad_SHORT           =8 , // RADIATION_SHORTWAVE
		Rad_LONG            =9 , // RADIATION_LONGWAVE
		Albedo             =10 , // ALBEDO
		SunHours           =11 , // SUNHOURS
		Cloud1_8           =12 , // CLOUD_1/ 8
		Cloud1_10          =13 , // CLOUD_1/ 10
		Winddirection      =14 , // Winddirection
		Windvel2M          =15 , // WINDVELOCITY2M
		Windvel10M         =16 , // Windvelocity10M
		Airtemp2M          =17 , // AIRTEMP_2M
		Airtmp10M          =18 , // Airtemp_10M
		Airtmp3            =19 , // Airtemp3
		Gust               =30 , // Gust
		Waterlevel		 =1000 , // Waterlevel
		Drawdown         =1001 , // Drawdown
		Watervelocit     =1002 , // Watervelocity
		Watertemp        =1003 , // Watertemp
		Reservvolume     =1004 , // Reservoir volume
		Icethickness     =1005 , // Ice thickness
		Conductivity     =1006 , // Conductivty
		PHvalue          =1007 , // PHvalue
		Overflow         =1008 , // Overflow
		Gateopeninig     =1009 , // Gate opening
		Waterdepth       =1010 , // Waterdepth
		Freeboard        =1011 , // Freeboard
		ChangeInRSVLevel =1012 , // Change reservoir level
		Inflow           =1050 , // Inflow
		WaterLevelRiver  =1053 , // Water level in river
		WaterFlow        =1054 , // Waterflow
		OperationalFlow  =1055 , // Operation flow
		SpecificFlow     =1056 , // Specific water flow
		DiversionFlow    =1060 , // Diversion flow
		LossPowerStation =1061 , // Sum loss power station
		Loss             =1063 , // Loss
		Flood            =1065 , // Flood
		LicencedDrawdown =1070 , // Licenced drawdown
		PumpFlow         =1075 , // Pump water flow
		GenFlow          =1080 , // Generation water flow
		Concspmtr        =1200 , // Concspmtr
		Trpspmtr         =1202 , // Trpspmtr
		Drbtntrp         =1204 , // Drbtntrp
		Idrbtntrp        =1206 , // Idrbtntrp
		Concorgmtr       =1208 , // Concorgmtr
		Trporgmtr		 =1210 , // Trporgmtr
		Kumkorssp		 =1212 , // Kumkorssp
		Kumkorbtr		 =1214 , // Kumkorbtr
		GroundWaterLevel =2000 , // Ground water level
		GroundMoisure    =2001 , // Ground moisture
		SnowDepth        =2002 , // Snow depth
		SnowEquiv		 =2003 , // Snow equiv .
		GroundFrostDepth =2004 , // Ground frost depth
		Portryck		 =2005 , // Portryck
		EarthTemp1       =2006 , // Earthtemp1
		EarthTemp2       =2007 , // Earthtemp2
		EarthTemp3       =2008 , // Earthtemp3
		GrassTemp1       =2009 , // Grasstemp1
		Meltwater		 =2010 , // Meltwater
		Meltw_a_p		 =2011 , // Meltw_a_p
		Snowarea		 =2012 , // Snowcovered area
		Gkumkorkur       =2100 , // Gkumkorkur
		Voltage			 =4000 , // Voltage
		ConsumpUns       =4010 , // CONSUMPTION
		ConsumpLow       =4011 , // Consumption low
		ConsumpHgh       =4012 , // Consumption high
		ConsumpMtr       =4013 , // Consumption meter
		Generation       =4015 , // Generation
		Pumping			 =4016 , // PUMPING
		Netpower		 =4017 , // Net Power Generation
		Power			 =4020 , // Power
		Drain			 =4040 , // Drain
		Energy			 =4041 , // ENERGY
		Wavepower		 =4042 , // Wave power
		Windvel3		 =4043 , // Wind velocity3
		Reservoir		 =4044 , // Reservoir
		Exchange		 =4045 , // Exchange
		Drmlbntra		 =5000 , // Drmlbntra
		Controbl		 =6001 , // Controbl
		Pricednm		 =8001 , // Pricednm
		Priceansm		 =8002 , // Priceansm
		Ansbid			 =8003 , // Ansbid
		SimDrainNfb		 =8100 , // Sim drain no feedb
		SimDrainWfb		 =8101 , // Sim drain with feedb
		AccDiff			 =8102 , // Accumulated diff
		WaterQual        =8200 , // Water_quality
		GenPlan			 =8300 , // Generation plan
		AvgEfficiency    =8400 , // Av_eff
		Regdeg			 =8401 , // REGDEG
		SpinReserve      =8402 , // Spinning reserve
		SlowReserve		 =8403 , // Slow reserve
		Static			 =8404 , // Statics
		DegRegSetting    =8405 , // Degree of regulation setting
		DecentralGen     =8500 , // Decentral generation
		Windpower		 =8501 , // Windpowergeneration
		BalCheckSum		 =8502 , // Balance check sum
		ActLoad			 =8600 , // Active load
		ReactLoad		 =8601 , // Reactive load
		Timeseries       =9000 , // Timeseries
		NetTimeser		 =9001 , // Net time series
		Energyval		 =9002 , // Energyval
		CUloadrat		 =9003 , // CUloadrat
		Metreading       =9004 , // Metreading
		InflowTotal      =9100 , // Inflow_total
		LicDrawdownTotal =9101 , // Licenced_drawd_total
		FloodTotal		 =9102 , // Flood_total
		AvailableCapacity=9103 , // Available_capasity
		Sumres			 =9104 , // Sumres
		TempGrad		 =9200 , // Temperature gradient
		P5				 =9201 , // P5
		P10				 =9202 , // P10
		MeteredValue     =9300 , // Metered value
		BilateralTrade   =9301 , // Bilateral trade
		SpotTrade        =9302 , // Spotmarket trade
		AncSTrade		 =9303 , // Ancillary services trade
		TransLoss		 =9304 , // Transformer loss
		JIP_S_pro		 =9305 , // JIP supplier percentage
		JIP_N_sum		 =9306 , // JIP net area share
		SuppHour		 =9307 , // Supplier hour meterings
		SuppEst			 =9308 , // Supplier not hour meterings
		HourCust		 =9309 , // Hour meterings customer
		SuppTotal        =9310 , // Supplier  total
		DisthPr			 =9311 , // Dist, -press
		DisthTemp        =9312 , // District heating temperature
		DisthVol         =9313 , // Disth, -volume
        WaterVol         =9315, // Water Volume
        GasEnergy        =9316 , // Gas-Energy
        GasVolume        =9317 , // Gas-Measured volume
        GasStdVol        =9318 , // Gas-Standard volume
        GasCntrVol       =9319 , // Gas-Control volume
        GasMinPress      =9320 , // Gas Pressure Min
        GasMaxPress      =9321 , // Gas Pressure MAx
        GasMinTemp       =9322 , // Gas Temperature Min
        GasMaxTemp       =9323 , // Gas Temperature Max
        HotWaterTT       =9400 , // Hot Water Temperature Turn
        HotWaterTR       =9401 , // Hot Water Temperature Return
        HotWaterME       =9402 , // Hot Water Max Energy
        HotWaterMV       =9403 , // Hot Water Max Volume
        HotWaterEn       =9404 , // Hot Water Energy
        HotWaterVo       =9405 , // Hot Water Volume
        CoolWaterTT      =9450 , // Cool Water Temperature Turn
        CoolWaterTR      =9451 , // Cool Water Temperature Return
        CoolWaterME      =9452 , // Cool Water Max Energy
        CoolWaterMV      =9453 , // Cool Water Max Volume
        CoolWaterEn      =9454 , // Cool Water Energy
        CoolWaterVo      =9455 , // Cool Water Volume
        ExchSupp        =20001 , // Exch_supp_network_area
		ExchAdjNet      =20002 , // Energy_exch_adj_networks
		GenBA			=20003 , // Generation_balance_acc_network_area
		LoadExDisr      =20004 , // Load_excl_disr_el
		LoadDisr		=20005 , // Load_disr_el
		CalcNetLoss		=20006 , // Calculated_network_losses
		ProfNetLoss		=20007 , // Profile_calculated_network_losses
		ExchCustObject  =20008 , // Energy_exch_customer object
		AreaBalance		=20009 , // Area_balance_network_area
		DiffAdjNet      =20010 , // Difference_adj_net
		NonMCNet		=20011 , // Non_measured_consumption_network_area
		Availability	=20100,	 // Availability series
		Billing			=20200	 // Billing series
	}

	public enum TsOrigin		// Based on contents in TSORG_TP
	{
		Undefined		= 0 , // Undefined
		AutoMetered		= 1 , // Automatically metered series
		ManMetered		= 2 , // Manually metered series
		Predicted		= 3 , // Predicted series
		Calculated_TS		= 4 , // Calculated series
		Historic		= 5 , // Historic series
		MeasAndCalc		= 6 , // Series with measured and calc. values
		Profile			= 7 , // Profile
		HbvUpdate		= 200 , // HBV_UPDAT
		HbvCalibrate	= 201 , // HBV_CALIBRATION
		HbvProgMeterol	= 202 , // HBV_PROG_METE
		HbvProgHist		= 203 , // HBV_PROG_HIST
		HbvProgComb		= 204 , // HBV_PROG_KOMB
		SimCont			= 300 , // Sim continuous series
		SimInflowCalc	= 301 , // Sim inflow calculations
		GS2_NetTot		= 800 , // GS2 network total
		GS2_NetM		= 801 , // GS2 network metered
		GS2_NetNM		= 802 , // GS2 network non metered
		GS2_SuppNM		= 803 , // GS2 supplier non metered
		Measured		= 900 , // Measured
		Calculated		= 901  // Calculated
	}

	public enum TsProducer		// Based on contents TSPRODUC table 
	{
		BWG			= 230 , // Sveitsiske vassf°ringer
		InDec		= 220 , // InDec
		Storm		= 210 , // Storm
		SMHI		= 215 , // SMHI
		Statkraft	= 205 , // Statkraft SF
		OTB			= 225 , // Øst-Telemarkens Brukseierforening
		Undefined	= 0 , // Undefined
		ICC_Sim		= 1 , // Sim
		ICC_Inflow	= 2 , // Inflow
		ICC_Hydinn	= 3 , // TS-HYDINN
		ICC_Elcbas	= 4 , // ELCBAS
		ICC_Demand	= 5 , // Demand
		Test		= 6 , // Test purposes
		Kofot		= 7 , // TS-KOFOT
		ICC_Trade	= 8 , // Trade
		Protil		= 9 , // TS-PROTIL
		Snomod		= 10 , // TS-SNOMOD
		ICC_Shop	= 11 , // SHOP
		MPS			= 12 , // TS-Multi-area Power Market Simulator
		NVE			= 100 , // NVE
		MI			= 101 , // MI
		ScanMatic	= 102 , // ScanMatic
		LandisGyr	= 110 , // LandisGyr
		DIS_VAX		= 200 , // DIS/VAX
		External	= 201 , // External
		ScadaEx		= 202 , // ScadaEx/TS Import
		AUTO		= 13  , // Automatically created during import
		ICC_Avail	= 14    // Availability module / restriction
	}

	public enum TsProtocol		// Based on contents TP_PROTO_TP table 
	{
		Undefined	= 0, //Undefined
		PVE			= 1, //PVE
		GS2			= 2, //GS2 v 1.2
		ELCBAS		= 4,
		EDI_MSCONS  = 6, //EDIEL MSCONS
		INV			= 7, //Invoicing
		KW2000		= 8,
		DEBIT		= 9,
        GS2_V10     = 10, //GS2 v 1.0
        GS2_V12_INFOSYNERGI = 11, //GS2 v 1.2 InfoSynergi
		EDI_DELFOR  = 12,
		SKV			= 13,
		DLS			= 14,
		TIM			= 15,
		LINKCOM		= 16,
        EDI_APERAK  = 17, //EDIEL APERAK (acknowledgment)
		ABBAS		= 18,
		KISX		= 19,
		GS2_ALBATROSS = 20, //GS2 Meter/Max
        DB_IMPORT   = 21,
        EDI_MSCONS_READINGS = 22,
        EDI_MSCONS_SETTLEMENT = 23,
		EDI_QUOTES  = 24,
        EDI_MSCONS_Z02 = 27,
        DB_EXPORT   = 29,
        POPDAT      = 30,
		TSVALUE		= 31,
		EXCEL		= 32,
        EDI_DELFOR_READINGS = 33,
		PDXML		= 34,
        FRIICE      = 36,
        NPPO        = 38,
        NETBAS_DNMI = 39,
        EDI_MSCONS_CONSUMPTION = 40,
        EDI_REQOTE  = 41,
        EDK_SLSRPT  = 43,
        EDI_REQOTE_N13 = 44,    // Norwegian Primary regulation
        EDI_REQOTE_S36 = 45,    // Norwegian Secondary regulation
        ESS_A01 = 50,
        UTILTS_N01  = 56,
        UTILTS_N02  = 57,
        UTILTS_N03  = 58,
        UTILTS_N06  = 59,
        UTILTS_E31  = 60,
        UTILTS_E26  = 61,
        UTILTS_E30  = 62,
        UTILTS_E66  = 63,
        UTILTS_ERR  = 64, //UTILTS-ERR (response on error in data)
        UTILTS_S01  = 65,
        UTILTS_S02  = 66,
        UTILTS_S03  = 67,
        UTILTS_S04  = 68,
        UTILTS_S05  = 69,
        UTILTS_S06  = 70,
        UTILTS_S07  = 71,
        UTILTS_APERAK = 73, //UTILTS APERAK (acknowledgment)
        UTILTS_N13  = 76,
        UTILTS_N14 = 77,
        UTILTS_N10 = 78,
        UTILTS_N11 = 79,
        UTILTS_S12 = 82,
        UTILTS_E66_MONTH = 84,
        UTILTS_E31_MONTH = 85,
        GS2_HourReadings = 86,
        UTILTS_S13 = 87,
        UTILTS_S37 = 88,
        ERRP = 89,
        ESP = 90, // ENTSO-E Settlement Process
        NEMM = 91,  // Nordic (TSO) Energy Market Model
        ECAN = 92,  // ENTSO-E Capacity Alloc/Nomination
        EOD = 93,   // ENTSO-E Outage Document
        EBIX = 94,  // Energy Business Info eXchange
        EPD = 95,   // ENTSO-E Publication Document
        ENTSOE_ACK = 96,    // ENTSO-E Acknowledgement
        EBIX_ACK = 97,  // ebIX Acknowledgement
        UTILTS_N17 = 98,    // Avr. av produksjonsglatting
        GS2_V12_INFOSYNERGI_V2 = 99,
        PRODAT = 100,   // PRODAT
        UTILTS_S08 = 101,   // Accepted primary bids in Sweden
	    UTILTS_N18 = 102,   // Accepted primary D-2 bids in Norway
        UTILTS_S40, // Secondary regulation market, settlement deviation basis report in Norway
        NP_REQUEST_COMMAND, // 104. Parameters for request commands to Nord Pool's Auction API. Handled by Powel Service Host.
        PARTICIPANT_MASTER_DATA = 105,   // EDI list from edielportalen.{no|se}
        //NEMM_Z05 = 106,
        //NEMM_Z06 = 107,
        //NEMM_Z08 = 108,
        //EOD = 109,
        //EBIX_E66 = 110,
        //EBIX_E31 = 111,
        //ENTSOE_ACK = 112
        NP_AUCTION = 115,   // Nord Pool Auction API. Auction definition.
        NP_TRADE = 117,      // Nord Pool Auction API. Trade/Contract definition.
        NP_CURVE = 118,      // Nord Pool Auction API. Bid curve.
        NP_BLOCKS = 119,      // Nord Pool Auction API. Block bids.
        // See TP_PROTO_TP.dbo
        NP_CURVE_ORDER = 124,    // Nord Pool Auction API. Curve order. Response from NP_CURVE, NP_CURVE_GET and NP_CURVE_PATCH
        // See TP_PROTO_TP.dbo
        NP_BLOCK_ORDER = 127,    // Nord Pool Auction API. Block order. Response from NP_BLOCKS, NP_BLOCKS_GET and NP_BLOCKS_PATCH
        NP_ERROR_MESSAGE = 128,  // Nord Pool Auction API. Error message. Response from NP_CURVE, NP_BLOCKS
        NP_AUCTION_ORDERS,		// Nord Pool Auction API. Auction orders.
        NP_AUCTION_PORTFOLIO_VOLUMES,	// Nord Pool Auction API. Auction portfolio volumes.
        NP_AUCTION_PRICES		// Nord Pool Auction API. Auction prices.
    }

    public enum GridAreaSeries
	{
		Undefined = 0,
		RegulationMarketPrice = 1,
		ElspotPrice = 2,
		ElspotAreaMembership = 3
	}
	
	public enum RegistrationReason
	{
		UNDEFINED = 0,
		CYCLIC = 1,
		CHANGEMETER = 2,
		EXTRA = 3,
		CHANGESUPPLIER = 4,
		INSPECTION = 5,
		NEWMETER = 6,
		REMOVEMETER = 7,
		MANUAL = 8,
		DEBITINGMETHOD = 9,
		CLOSED = 10,
		MOVE = 11,
		START = 12,
		CHANGE = 13,
		VAT = 14,
		NEWINSTALLATION = 15,
		PRODUCT = 16,
		SEALED = 17,
		PRICE = 18,
		FUSE = 19,
		TAX = 20
	}

	public enum TsExpDefInterval
	{
		NEVER = -1, //Fix for 7.4.19
		ONCE = 0,
		ONCE_A_YEAR = 1,
		ONCE_A_MONTH = 2,
		ONCE_A_WEEK = 3,
		ONCE_A_DAY = 4,
		ALL = 99
	}
	public enum RestrictionOwner
	{
		IsNotTsOwner = 0,
		IsTsOwner = 1,
		CreateOwnerFailed=2
	}
	public enum COMPROLE
	{
		ENABLED_IN_CAPACITY_MATRIX=1035,
		ENERGY_PRODUCTION_RESERVE=1004,
		ENERGY_STORAGE_RESERVE=1005,
		INCR_COST=752,
		ACCDEVMW=1065,
		ACCDEVM3S=1066,
		AVAILABLE_BEST_EFF=993,
		AVAILABLE_BEST_MW=991,
		AVAILABLE_BEST_M3S=992,
		AVAILABLE_MAX_EFF=996,
		AVAILABLE_MAX_MW=994,
		AVAILABLE_MAX_M3S=995,
		AVAILABLE_MIN_EFF=1002,
		AVAILABLE_MIN_MW=1000,
		AVAILABLE_MIN_M3S=1001,
		AVAILABLE_MIN_RUN_EFF=999,
		AVAILABLE_MIN_RUN_MW=997,
		AVAILABLE_MIN_RUN_M3S=998,
		BID_FLAG=763,
		COMPRESSIONTS=1063,
		COMPRESSIONTSM3S=1064,
		DISTURB_REG_RESERVE=769,
		DRAFT=73,
		EFF_CUT=1150,
		FREQ_REG_RESERVE=768,
		GEN_EFF_CURVE=78,
		GEN_NUMBER=71,
		GENSTATUS=1071,
		HIST_PROD=70,
		HYD_KEY=760,
		INLGATE=694,
		INL_GATE_CURVE=695,
		MAINTENANCE=40,
		MAX_FLOW_CONST=34,
		MAXPROD=74,
		MAXPROD_HEAD=148,
		MAXPROD_TS=912,
		MIN_FLOW_CONST=35,
		MINPROD=77,
		MINPROD_TS=939,
		NAME=3,
		NOMFLOW=895,
		NOMPROD=76,
		PENSTOCK=72,
		PRIORITY=771,
		PRIORITY_TS=917,
		PRODUCTION=75,
		QUICK_RESERVE=156,
		RAMPING_GENERATOR=938,
		REG_POWER=940,
		REG_STRENGTH=155,
		REG_STRENGTH_INPUT=1046,
		ROTATING_RESERVE=770,
		SCHEDULE_FLAG_TIME=696,
		SCHEDULE_MW=111,
		SCHEDULE_M3S=110,
		SHOP_COMMITTED=369,
		SHOP_COMMITTED_FLAG=370,
		SHOP_DISCHARGE_PLANT=502,
		SHOP_R_DISCHARGE=860,
		SHOP_R_PROD=861,
		SHOP_STARTCOST=366,
		SHOP_TURB_PROHIB_ZONES=367,
		SIM_EFF_M=681,
		SIM_RESULT_MW=26,
		SIM_RESULT_M3S=27,
		SLOW_RESERVE=158,
		STARTCOST_TS=961,
		STATIC_DESCRIPTION=934,
		STATIC_HIGH=151,
		STATIC_LOW=149,
		STATIC_MEAN=150,
		STATIC_SETTING=154,
		STATIC_SETTING_MW=153,
		STATIC_SETTING_P=152,
		TEH_TEST=989,
		TURB_EFF_CURVE=79,
		TURB_LOSS=1047,
		TURB_POS_CURVE=682,
		TYPE=28,
		UNAVAILABLE_PROD=1192,
		VAR_CONSTR=15
	}
    public enum QueueMessageType
    {
        UNKNOWN_MESSAGE = -1,
        TIME_SERIES = 0,
        TIME_SERIES_IMPORT_RESPONSE = 1,
        METERING_SERVICE_INSTALLATION = 2,
        METERING_SERVICE_COMPONENTS = 3,
        METERING_SERVICE_AGREEMENT = 4,
        METERING_SERVICE_INSTALLATION_RESPONSE = 5,
        METERING_SERVICE_COMPONENTS_RESPONSE = 6,
        METERING_SERVICE_AGREEMENT_RESPONSE = 7,
        TIME_SERIES_RESPONSE = 8,
        METERING_SERVICE_REGISTERS = 9,
        METERING_SERVICE_EXPORTS = 10,
        METERING_SERVICE_REGISTERS_RESPONSE = 11,
        METERING_SERVICE_EXPORTS_RESPONSE = 12
    }
}
