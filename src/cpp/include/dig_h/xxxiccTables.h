/**************************************************************************
           Object Database Interface Table and Field Name Declarations
                Copyright Objekt Systemer / Powel Data AS
***************************************************************************
*
* File name....: iccTables.h
* Version......: 4.22  of the class DBTable in the DIG program
* Last modified: 2003/01/13 13:28:10 
* Created by...: ob	(Using the DIG program)
* Created date.: 106/12/28 16:58
*
* DESCRIPTION:
*	Table and field name structure declarations.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	106/12/28 16:58	ob	New file.
*
***************************************************************************
*/

#ifndef ICCTABLES_H
#define ICCTABLES_H

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (1)
#endif

#include "OSTypes.h"
#include "DBInterf.h"

#ifdef daylight //daylight is defined in TIME.h in MSVC 4.1
#undef daylight
#endif


struct iccTables
{
	//pCHAR	Keytab;
	//pCHAR	Keytab2;
	pCHAR	Keytab9;
	pCHAR	AccbaTp;
	pCHAR	Accntrec;
	pCHAR	Accntrp;
	pCHAR	Accrpobj;
	//pCHAR	AccuTp;
	pCHAR	AccuVl;
	pCHAR	Aghcohyr;
	pCHAR	Aghcoprr;
	pCHAR	AghydDs;
	pCHAR	Agrmpart;
	pCHAR	AgrmtTp;
	//pCHAR	Alterna;
	//pCHAR	Aureg1;
	//pCHAR	Aureg2;
	pCHAR	AuregCo;
	//pCHAR	Bidarea;
	pCHAR	Bidblock;
	pCHAR	Bidcots;
	pCHAR	Bid24frm;
	pCHAR	Bid24hou;
//	pCHAR	Bidregpo;
	pCHAR	Bidrpfrm;
	pCHAR	Bidrpobj;
	pCHAR	Breaker;
	pCHAR	Busbar;
	pCHAR	BuyslTp;
	pCHAR	Catmarea;
	pCHAR	CatmaTp;
	pCHAR	CcmetTp;
	pCHAR	Cntper;
	pCHAR	CntPers;
	pCHAR	ConstCo;
	pCHAR	Contact;
	pCHAR	Contnode;
	pCHAR	Contract;
	pCHAR	CosetTp;
	pCHAR	CostaTp;
	pCHAR	Counter;
	pCHAR	Country;
	pCHAR	CouseTp;
	pCHAR	Currency;
	pCHAR	DayVl;
	pCHAR	DebitTp;
	pCHAR	Debitsys;
	pCHAR	DebssTp;
	pCHAR	Delcobal;
	pCHAR	Delivagm;
	pCHAR	DirecTp;
	//pCHAR	Docref;
	pCHAR	Doctext;
	pCHAR	Document;
	//pCHAR	Edklog;
	pCHAR	Elccoelc;
	//pCHAR	Elcmail;
	pCHAR	ElcmpTp;
	pCHAR	Eleccomp;
	pCHAR	Elecnode;
	pCHAR	ElspotBlocksTp;
	pCHAR	ExchpTp;
	pCHAR	ExchPnt;
	pCHAR	Excpfact;
	pCHAR	Exchtask;
	pCHAR	Fieldsta;
	//pCHAR	FiestTp;
	pCHAR	Fistcent;
	pCHAR	Fngroup;
	pCHAR	FuncDs;
	pCHAR	Generato;
	pCHAR	Groups;
	pCHAR	Hfncofun;
	pCHAR	HourVl;
	pCHAR	Hydcomp;
	pCHAR	HydcoFn;
	pCHAR	HydcoTp;
	pCHAR	HydfnTp;
	pCHAR	HydfnVl;
	pCHAR	Hydgenun;
	pCHAR	Hydplant;
	//pCHAR	HydplTp;
	//pCHAR	Hydturb;
	pCHAR	HypsoVl;
	pCHAR	HyrelAc;
	//pCHAR	HyvcGrp;
	pCHAR	IccApp;
	pCHAR	IccAppl;
	pCHAR	Exchmeth;
	pCHAR	Impexpts;
	pCHAR	InvarTp;
	pCHAR	Invper;
	pCHAR	Invpfrm;
	pCHAR	Invplin;
	pCHAR	LastOkExported;
	//pCHAR	Kwphctr;
	pCHAR	Ldfarea;
	pCHAR	MacroDs;
	//pCHAR	MacroVl;
	pCHAR	Madcomaa;
	pCHAR	MailnTp;
	pCHAR	Maradiv;
	pCHAR	Mardicol;
	pCHAR	MardiTp;
	pCHAR	MareaTp;
	pCHAR	Meacoexc;
	pCHAR	MeasPnt;
	pCHAR	MeasTs;
	pCHAR	MeatsTp;
	pCHAR	MeavlCo;
	//pCHAR	MemeaTp;
	pCHAR	MimaxCt;
	pCHAR	Min15Vl;
	//pCHAR	Miregion;
	//pCHAR	Month;
	pCHAR	MonthVl;
	//pCHAR	MtrCmp;
	//pCHAR	MtrTp;
	pCHAR	Mtrcotim;
	pCHAR	Netarea;
	pCHAR	NetarTp;
	pCHAR	Opbidare;
	pCHAR	Oprcofun;
	pCHAR	Oprrst;
	pCHAR	OprrsTp;
	pCHAR	OprrsVl;
	pCHAR	Oprtunit;
	pCHAR	OwnerTp;
	pCHAR	Partempl;
	pCHAR	Partner;
	pCHAR	ParVl;
	pCHAR	Prdeprpl;
	pCHAR	Priceint;
	pCHAR	Prlirepl;
	pCHAR	Procotro;
	pCHAR	Profile;
	pCHAR	Program;
	pCHAR	Project;
	pCHAR	ProtoTp;
	pCHAR	PrparDs;
	pCHAR	PrparMx;
	//pCHAR	PrpmxVl;
	pCHAR	PrrunDs;
	pCHAR	Recadr;
	pCHAR	RefTrobject;
	pCHAR	ReporTp;
	pCHAR	RepvlCt;
	pCHAR	Reservoi;
	pCHAR	ResulVl;
	pCHAR	ResvoTp;
	pCHAR	Secpoint;
	//pCHAR	SecpTp;
	//pCHAR	SectTp;
	pCHAR	Sensor;
	pCHAR	Sign;
	pCHAR	Simuler;
	pCHAR	SpparDs;
	//pCHAR	SpparVl;
	pCHAR	Stacoprr;
	//pCHAR	StamxVl;
	pCHAR	Statempl;
	pCHAR	StateDs;
	//pCHAR	StateMx;
	//pCHAR	StateVl;
	//pCHAR	StlogTp;
	//pCHAR	StlogVl;
	//pCHAR	Substrat;
	pCHAR	Tarifdef;
	pCHAR	Tariff;
	pCHAR	Tdscotim;
	pCHAR	Timcohyd;
	pCHAR	TimcoTp;
	pCHAR	Timcovcg;
	pCHAR	Timeser;
	pCHAR	TimseDs;
	pCHAR	Tmarkar;
	pCHAR	Trafo2w;
	pCHAR	Translin;
	pCHAR	TrareTp;
	pCHAR	Trbusbar;
	pCHAR	Trdreport;
	pCHAR	TrdrpTp;
	pCHAR	Trelsbid;
	pCHAR	Trobcomm;
	pCHAR	Trobject;
	pCHAR	TrobjTp;
	pCHAR	Trocotro;
	pCHAR	Troption;
	pCHAR	Trprod;
	pCHAR	TsapmCc;
	pCHAR	TscaTp;
	pCHAR	MeterVl;
	pCHAR	Tscoupl;
	pCHAR	TseriTp;
	pCHAR	TsfunVl;
	pCHAR	TsintTp;
	pCHAR	TsorgTp;
	pCHAR	TsorgVl;
	pCHAR	Tsproduc;
	pCHAR	TsquTp;
	//pCHAR	Tsrefreg;
	pCHAR	Tsrefwei;
	pCHAR	Tsstat;
	pCHAR	Tsstatus;
	pCHAR	TstexVl;
	pCHAR	Tstradef;
	//pCHAR	Uaccess;
	pCHAR	UnfacVl;
	pCHAR	UnmeaTp;
	pCHAR	UnmeaVd;
	pCHAR	Userprog;
	//pCHAR	Users;
	//pCHAR	Usertype;
	//pCHAR	VctypTp;
	pCHAR	Watercou;
	pCHAR	WeekVl;
	pCHAR	Wtfcohyr;
	pCHAR	Wtgate;
	pCHAR	WtgtTp;
	//pCHAR	WtrfnTp;
	pCHAR	Wtroujun;
	pCHAR	Wtroute;
	//pCHAR	WtrtTp;
	pCHAR	YearVl;
};

struct iccFields
{
	//pCHAR	a11;
	//pCHAR	a12;
	//pCHAR	a13;
	//pCHAR	a14;
	//pCHAR	a15;
	//pCHAR	a16;
	//pCHAR	a21;
	//pCHAR	a22;
	//pCHAR	a23;
	//pCHAR	a24;
	//pCHAR	a25;
	//pCHAR	a26;
	//pCHAR	a31;
	//pCHAR	a32;
	//pCHAR	a33;
	//pCHAR	a34;
	//pCHAR	a35;
	//pCHAR	a36;
	//pCHAR	a41;
	//pCHAR	a42;
	//pCHAR	a43;
	//pCHAR	a44;
	//pCHAR	a45;
	//pCHAR	a46;
	//pCHAR	a51;
	//pCHAR	a52;
	//pCHAR	a53;
	//pCHAR	a54;
	//pCHAR	a55;
	//pCHAR	a56;
	//pCHAR	a61;
	//pCHAR	a62;
	//pCHAR	a63;
	//pCHAR	a64;
	//pCHAR	a65;
	//pCHAR	a66;
	pCHAR	acbaKey;
	//pCHAR	accCode;
	pCHAR	acceptedStart;
	pCHAR	acceptedEnd;
	pCHAR	accKey;
	pCHAR	accType;
	pCHAR	achKey;
	pCHAR	action;
	//pCHAR	acttime;
	pCHAR	address;
	pCHAR	adjDate;
	pCHAR	adjRef;
	pCHAR	agredate;
	pCHAR	altKey;
	pCHAR	altitude;
	pCHAR	aperak;
	pCHAR	appKey;
	pCHAR	applId;
	pCHAR	appstat;
	pCHAR	area;
	pCHAR	arg1;
	pCHAR	arg2;
	pCHAR	arg3;
	pCHAR	artDesc;
	pCHAR	artKey;
	pCHAR	artNo;
	pCHAR	artText;
	//pCHAR	auditOn;
	pCHAR	backtime;
	pCHAR	balance;
	pCHAR	balResp;
	pCHAR	balSale;
	pCHAR	bankacc;
	pCHAR	basCurr;
	pCHAR	basevolt;
	pCHAR	bastime;
	//pCHAR	beta0;
	//pCHAR	beta1;
	//pCHAR	beta2;
	//pCHAR	beta3;
	//pCHAR	beta4;
	//pCHAR	beta5;
	pCHAR	bid24;
	pCHAR	bidRef;
	//pCHAR	bidable;
	pCHAR	bidcobid;
	pCHAR	bidquant;
	pCHAR	bidrp;
	pCHAR	bidtpKey;
	pCHAR	bidwm;
	//pCHAR	bitno;
	pCHAR	bloccode;
	pCHAR	blockno;
	pCHAR	botlevel;
	pCHAR	busbacod;
	pCHAR	busbarno;
	pCHAR	buslKey;
	//pCHAR	byteno;
	pCHAR	byteprvl;
	pCHAR	calcLos;
	pCHAR	calcdef;
	pCHAR	cataKey;
	pCHAR	catchare;
	pCHAR	ccmeKey;
	pCHAR	charvl;
	pCHAR	cnPort;
	pCHAR	code;
	pCHAR	colour;
	pCHAR	compKey;
	pCHAR	compTyp;
	pCHAR	constval;
	pCHAR	contline;
	pCHAR	contrNo;
	pCHAR	convertusinglocaltime;
	pCHAR	coplKey;
	pCHAR	copyExport;
	pCHAR	copySettlement;
	pCHAR	copySvk;
	pCHAR	corFact;
	pCHAR	corKey;
	pCHAR	corMeth;
	pCHAR	corcok;
	pCHAR	correctInvalidBid;
	pCHAR	corrfact;
	pCHAR	coseKey;
	pCHAR	country;
	pCHAR	countryKey;
	pCHAR	cre1008;
	pCHAR	cstpKey;
	//pCHAR	ctrid;
	//pCHAR	ctrvalue;
	pCHAR	currFac;
	pCHAR	currGr;
	pCHAR	curvetyp;
	pCHAR	custKey;
	pCHAR	cutpKey;
	pCHAR	cvalue;
	pCHAR	dagmKey;
	pCHAR	dagmpart;
	pCHAR	dalastme;
	//pCHAR	dalastst;
	pCHAR	datasize;
	pCHAR	dateclos;
	pCHAR	dateesta;
	pCHAR	datetim;
	//pCHAR	day01;
	//pCHAR	day02;
	//pCHAR	day03;
	//pCHAR	day04;
	//pCHAR	day05;
	//pCHAR	day06;
	//pCHAR	day07;
	//pCHAR	day08;
	//pCHAR	day09;
	//pCHAR	day10;
	//pCHAR	day11;
	//pCHAR	day12;
	//pCHAR	day13;
	//pCHAR	day14;
	//pCHAR	day15;
	//pCHAR	day16;
	//pCHAR	day17;
	//pCHAR	day18;
	//pCHAR	day19;
	//pCHAR	day20;
	//pCHAR	day21;
	//pCHAR	day22;
	//pCHAR	day23;
	//pCHAR	day24;
	//pCHAR	day25;
	//pCHAR	day26;
	//pCHAR	day27;
	//pCHAR	day28;
	//pCHAR	day29;
	//pCHAR	day30;
	//pCHAR	day31;
	pCHAR	daylight;
	//pCHAR	dbOwner;
	pCHAR	dbuser;
	//pCHAR	dbusrId;
	pCHAR	debsKey;
	pCHAR	debtKey;
	pCHAR	debtTyp;
	pCHAR	decimals;
	pCHAR	declared;
	pCHAR	decltime;
	pCHAR	delNorm;
	pCHAR	delResp;
	pCHAR	descript;
	pCHAR	designno;
	pCHAR	deviaFl;
	pCHAR	dircKey;
	pCHAR	direct;
	pCHAR	distance;
	pCHAR	docindex;
	pCHAR	doReduction;
	//pCHAR	drafRef;
	//pCHAR	drefKey;
	pCHAR	dtubeNo;
	pCHAR	dueDate;
	//pCHAR	duration;
	pCHAR	eanNumber;
	pCHAR	edcowner;
	pCHAR	edcpassw;
	pCHAR	edcuseid;
	pCHAR	edcvers;
	pCHAR	ediCode;
	pCHAR	ediCodelistResp;
	//pCHAR	edkKey;
	pCHAR	efctKey;
	pCHAR	eicCode;
	pCHAR	elKey1;
	pCHAR	elKey2;
	pCHAR	elcmKey;
	pCHAR	elecKey;
	pCHAR	elnoKey;
	pCHAR	elnodnum;
	//pCHAR	elspot;
	pCHAR	email;
	pCHAR	emailAd;
	pCHAR	emaildom;
	//pCHAR	enabled;
	pCHAR	endcont;
	pCHAR	enddate;
	pCHAR	endtime;
	pCHAR	enerfact;
	pCHAR	entregno;
	pCHAR	enumval;
	pCHAR	errorpct;
	pCHAR	esttime;
	pCHAR	exchKey;
	pCHAR	exchTyp;
	pCHAR	exeok;
	pCHAR	exetime;
	pCHAR	exmtKey;
	pCHAR	expPath;
	pCHAR	exportDatesInNormalTime;
	pCHAR	exportQueue;
	pCHAR	extC;
	pCHAR	extP;
	pCHAR	extraA;
	pCHAR	extraB;
	pCHAR	extraC;
	pCHAR	extraD;
	pCHAR	extval;
	pCHAR	factor;
	pCHAR	faxnumb;
	pCHAR	fcno;
	pCHAR	field;
	pCHAR	fiesKey;
	pCHAR	filename;
	pCHAR	fintime;
	pCHAR	firstPriceStrategy;
	pCHAR	firsyear;
	pCHAR	fnKey;
	pCHAR	fnVal;
	pCHAR	fnexpr;
	pCHAR	fngrNam;
	pCHAR	fngrNum;
	pCHAR	fngracc;
	pCHAR	fnvlKey;
	pCHAR	formTp;
	pCHAR	formdays;
	pCHAR	formno;
	pCHAR	formstat;
	pCHAR	fromHour;
	pCHAR	fromdate;
	pCHAR	fromtime;
	pCHAR	fsno;
	pCHAR	funKey;
	pCHAR	fusesz;
	pCHAR	genAmo;
	pCHAR	genunTp;
	pCHAR	genunno;
	pCHAR	glacarea;
	pCHAR	globeref;
	pCHAR	gmsmod;
	pCHAR	grupKey;
	pCHAR	header1;
	pCHAR	header2;
	pCHAR	header3;
	pCHAR	header4;
	pCHAR	headloss;
	pCHAR	higalttu;
	pCHAR	hihvmode;
	//pCHAR	hour00;
	//pCHAR	hour01;
	//pCHAR	hour02;
	//pCHAR	hour03;
	//pCHAR	hour04;
	//pCHAR	hour05;
	//pCHAR	hour06;
	//pCHAR	hour07;
	//pCHAR	hour08;
	//pCHAR	hour09;
	//pCHAR	hour10;
	//pCHAR	hour11;
	//pCHAR	hour12;
	//pCHAR	hour13;
	//pCHAR	hour14;
	//pCHAR	hour15;
	//pCHAR	hour16;
	//pCHAR	hour17;
	//pCHAR	hour18;
	//pCHAR	hour19;
	//pCHAR	hour20;
	//pCHAR	hour21;
	//pCHAR	hour22;
	//pCHAR	hour23;
	//pCHAR	hour24;
	//pCHAR	hourno;
	pCHAR	hrv;
	pCHAR	hvhcmode;
	pCHAR	hydKey;
	pCHAR	hydKeyh;
	pCHAR	hydKeyo;
	pCHAR	hydfKey;
	pCHAR	hydtKey;
	pCHAR	hyplKey;
	pCHAR	hypsKey;
	pCHAR	hyretype;
	pCHAR	iccSender;
	pCHAR	idate;
	pCHAR	idxDate;
	pCHAR	ilevel;
	pCHAR	impPersist;
	pCHAR	inflowperiod;
	pCHAR	info;
	pCHAR	infotype;
	pCHAR	inlgtNo;
	pCHAR	insertVolume0;
	pCHAR	inslKey;
	pCHAR	instdate;
	pCHAR	invAble;
	pCHAR	invAdr;
	pCHAR	invFax;
	pCHAR	invRef;
	pCHAR	invStat;
	pCHAR	invTele;
	pCHAR	invTp;
	pCHAR	invinter;
	pCHAR	invpKey;
	pCHAR	isAccSeries;
	pCHAR	ivalue;
	pCHAR	ivalue1;
	pCHAR	ivalue2;
	pCHAR	kCoef;
	pCHAR	knockingDown;
	pCHAR	lapDate;
	pCHAR	larDate;
	pCHAR	lastOkExported;
	pCHAR	lastyear;
	pCHAR	lcdate;
	pCHAR	ldfKey;
	pCHAR	lenght;
	pCHAR	limitr;
	pCHAR	linenumb;
	pCHAR	listno;
	pCHAR	location;
	//pCHAR	locked;	
	pCHAR	loginfo;
	pCHAR	logo1;
	pCHAR	logo2;
	pCHAR	lowalttu;
	pCHAR	lrv;
	pCHAR	lvdate;
	//pCHAR	m150000;
	//pCHAR	m150015;
	//pCHAR	m150030;
	//pCHAR	m150045;
	//pCHAR	m150100;
	//pCHAR	m150115;
	//pCHAR	m150130;
	//pCHAR	m150145;
	//pCHAR	m150200;
	//pCHAR	m150215;
	//pCHAR	m150230;
	//pCHAR	m150245;
	//pCHAR	m150300;
	//pCHAR	m150315;
	//pCHAR	m150330;
	//pCHAR	m150345;
	//pCHAR	m150400;
	//pCHAR	m150415;
	//pCHAR	m150430;
	//pCHAR	m150445;
	//pCHAR	m150500;
	//pCHAR	m150515;
	//pCHAR	m150530;
	//pCHAR	m150545;
	//pCHAR	m150600;
	//pCHAR	m150615;
	//pCHAR	m150630;
	//pCHAR	m150645;
	//pCHAR	m150700;
	//pCHAR	m150715;
	//pCHAR	m150730;
	//pCHAR	m150745;
	//pCHAR	m150800;
	//pCHAR	m150815;
	//pCHAR	m150830;
	//pCHAR	m150845;
	//pCHAR	m150900;
	//pCHAR	m150915;
	//pCHAR	m150930;
	//pCHAR	m150945;
	//pCHAR	m151000;
	//pCHAR	m151015;
	//pCHAR	m151030;
	//pCHAR	m151045;
	//pCHAR	m151100;
	//pCHAR	m151115;
	//pCHAR	m151130;
	//pCHAR	m151145;
	//pCHAR	m151200;
	//pCHAR	m151215;
	//pCHAR	m151230;
	//pCHAR	m151245;
	//pCHAR	m151300;
	//pCHAR	m151315;
	//pCHAR	m151330;
	//pCHAR	m151345;
	//pCHAR	m151400;
	//pCHAR	m151415;
	//pCHAR	m151430;
	//pCHAR	m151445;
	//pCHAR	m151500;
	//pCHAR	m151515;
	//pCHAR	m151530;
	//pCHAR	m151545;
	//pCHAR	m151600;
	//pCHAR	m151615;
	//pCHAR	m151630;
	//pCHAR	m151645;
	//pCHAR	m151700;
	//pCHAR	m151715;
	//pCHAR	m151730;
	//pCHAR	m151745;
	//pCHAR	m151800;
	//pCHAR	m151815;
	//pCHAR	m151830;
	//pCHAR	m151845;
	//pCHAR	m151900;
	//pCHAR	m151915;
	//pCHAR	m151930;
	//pCHAR	m151945;
	//pCHAR	m152000;
	//pCHAR	m152015;
	//pCHAR	m152030;
	//pCHAR	m152045;
	//pCHAR	m152100;
	//pCHAR	m152115;
	//pCHAR	m152130;
	//pCHAR	m152145;
	//pCHAR	m152200;
	//pCHAR	m152215;
	//pCHAR	m152230;
	//pCHAR	m152245;
	//pCHAR	m152300;
	//pCHAR	m152315;
	//pCHAR	m152330;
	//pCHAR	m152345;
	pCHAR	maCent;
	pCHAR	maLeft;
	pCHAR	maRight;
	pCHAR	madsKea;
	pCHAR	madsKey;
	pCHAR	mailKea;
	pCHAR	mailKey;
	//pCHAR	mailtime;
	pCHAR	mainrive;
	pCHAR	mardKey;
	pCHAR	mardiv;
	pCHAR	martKey;
	pCHAR	mask;
	//pCHAR	matrtype;
	//pCHAR	mavlKey;
	pCHAR	maxBidQuantity;
	//pCHAR	maxlim;
	pCHAR	maxrep;
	pCHAR	maxvalue;
	pCHAR	maxvltlm;
	pCHAR	mcmpKey;
	pCHAR	meadjust;
	pCHAR	meaoutfl;
	pCHAR	measKey;
	pCHAR	memeKey;
	pCHAR	messRef;
	//pCHAR	message;
	pCHAR	method;
	pCHAR	metsKey;
	//pCHAR	mid;
	pCHAR	minBidQuantity;
	//pCHAR	minDuration;
	pCHAR	mindelay;
	pCHAR	minextre;
	//pCHAR	minlim;
	pCHAR	minvalue;
	pCHAR	minvltlm;
	pCHAR	mireKey;
	pCHAR	modnumb;
	pCHAR	mondate;
	//pCHAR	montKey;
	//pCHAR	msgno;
	//pCHAR	mtrKey;
	pCHAR	mxconrep;
	pCHAR	mxflow;
	pCHAR	mxvol;
	pCHAR	nPhases;
	pCHAR	name;
	pCHAR	netaKey;
	pCHAR	netbKey;
	pCHAR	netlevel;
	pCHAR	netowner;
	pCHAR	nettKey;
	pCHAR	noDtube;
	pCHAR	noPstoc;
	pCHAR	nomessages;
	pCHAR	nomhead;
	pCHAR	numbmeas;
	pCHAR	numofarg;
	pCHAR	nunit;
	pCHAR	nxtAdj;
	pCHAR	offdate;
	pCHAR	offdur;
	pCHAR	offset;
	pCHAR	opname;
	pCHAR	oprsKey;
	pCHAR	oprstatu;
	pCHAR	oprtKey;
	pCHAR	optprice;
	pCHAR	opttime;
	pCHAR	opunKea;
	pCHAR	opunKey;
	//pCHAR	opusrId;
	pCHAR	orderId;
	pCHAR	orderno;
	pCHAR	osuser;
	pCHAR	ourRef;
	pCHAR	ownerPct;
	pCHAR	ownrKey;
	pCHAR	pOffice;
	pCHAR	pPower;
	pCHAR	painKey;
	//pCHAR	paraKey;
	pCHAR	paraflag;
	pCHAR	pardKey;
	pCHAR	partKea;
	pCHAR	partKey;
	pCHAR	partNum;
	pCHAR	parvKea;
	pCHAR	parvKey;
	pCHAR	passwd;
	pCHAR	pattern;
	pCHAR	payPer;
	pCHAR	payTerm;
	pCHAR	pconsmax;
	//pCHAR	pensRef;
	pCHAR	period;
	pCHAR	persNum;
	pCHAR	phonenr1;
	pCHAR	phonenr2;
	pCHAR	phonenum;
	pCHAR	pmax;
	pCHAR	portfolioId;
	pCHAR	postacc;
	pCHAR	postcode;
	pCHAR	posttown;
	pCHAR	powType;
	pCHAR	power;
	pCHAR	ppmconst;
	pCHAR	prdKey;
	pCHAR	price;
	pCHAR	pricePt;
	pCHAR	priceStrategy;
	pCHAR	priceTimsKey;
	pCHAR	pricelev;
	pCHAR	pricemax;
	pCHAR	pricemin;
	pCHAR	printco1;
	pCHAR	prioInv;
	pCHAR	priority;
	pCHAR	prmxKey;
	pCHAR	proKey;
	pCHAR	proKeyp;
	pCHAR	productCode;
	pCHAR	profileKey;
	pCHAR	progKey;
	pCHAR	protKey;
	pCHAR	pstocNo;
	pCHAR	qPower;
	pCHAR	qmaks;
	pCHAR	qmin;
	pCHAR	quality;
	pCHAR	quantity;
	pCHAR	rcKey;
	pCHAR	rcount;
	pCHAR	readKey;
	pCHAR	readable;
	pCHAR	readno;
	pCHAR	reasKey;
	pCHAR	receiver;
	pCHAR	reduceNPrices;
	pCHAR	reductionMethod;
	pCHAR	refAltitude;
	pCHAR	referee1;
	pCHAR	referee2;
	pCHAR	referee3;
	pCHAR	referenc;
	pCHAR	refpower;
	pCHAR	refsubj;
	pCHAR	refvalue;
	pCHAR	refvoltp;
	pCHAR	refvolts;
	pCHAR	relDist;
	pCHAR	relKey;
	pCHAR	relX;
	pCHAR	relY;
	pCHAR	relZ;
	pCHAR	remvdate;
	pCHAR	repCurr;
	pCHAR	reporter;
	pCHAR	reptKey;
	pCHAR	resolut;
	pCHAR	restIs;
	pCHAR	restKey;
	pCHAR	restVl1;
	pCHAR	restVl2;
	//pCHAR	restingtime;
	pCHAR	revlKey;
	pCHAR	rpcode;
	pCHAR	rresarea;
	pCHAR	runInNormalTime;
	pCHAR	rval1;
	//pCHAR	rval10;
	//pCHAR	rval11;
	//pCHAR	rval12;
	//pCHAR	rval13;
	//pCHAR	rval14;
	//pCHAR	rval15;
	//pCHAR	rval16;
	//pCHAR	rval17;
	//pCHAR	rval18;
	//pCHAR	rval19;
	pCHAR	rval2;
	//pCHAR	rval20;
	//pCHAR	rval21;
	//pCHAR	rval22;
	//pCHAR	rval23;
	//pCHAR	rval24;
	//pCHAR	rval3;
	//pCHAR	rval4;
	//pCHAR	rval5;
	//pCHAR	rval6;
	//pCHAR	rval7;
	//pCHAR	rval8;
	//pCHAR	rval9;
	pCHAR	rvalue;
	pCHAR	rvalue2;
	pCHAR	scalFac;
	pCHAR	seaarea;
	pCHAR	secpKey;
	pCHAR	sectKey;
	pCHAR	security;
	pCHAR	sender;
	pCHAR	sensno;
	pCHAR	sentdate;
	pCHAR	seqno;
	pCHAR	seqnoeea;
	pCHAR	seqnoeee;
	pCHAR	seqnommm;
	//pCHAR	seqnosss;
	pCHAR	serialstart;
	pCHAR	shorname;
	pCHAR	signDatetim;
	pCHAR	signOsuser;
	pCHAR	signThisEvent;
	pCHAR	signcode;
	pCHAR	simKey;
	pCHAR	smailadr;
	pCHAR	smothFl;
	pCHAR	smthPrm;
	pCHAR	snitt;
	//pCHAR	special;
	pCHAR	spot;
	pCHAR	stamonth;
	pCHAR	stardate;
	//pCHAR	startRegulation;
	pCHAR	startdelay;
	pCHAR	startend;
	pCHAR	startime;
	pCHAR	state;
	//pCHAR	stateno;
	pCHAR	stcont;
	//pCHAR	stdsKea;
	pCHAR	stdsKey;
	pCHAR	steptype;
	//pCHAR	stime;
	//pCHAR	stmxKey;
	//pCHAR	stopRegulation;
	pCHAR	stopdate;
	pCHAR	storflag;
	pCHAR	sttpKea;
	pCHAR	sttpKey;
	pCHAR	subsKey;
	pCHAR	svkId;
	pCHAR	systemfl;
	pCHAR	tarfKey;
	pCHAR	tdsKey;
	pCHAR	testmode;
	//pCHAR	textVl;
	pCHAR	ticoKey;
	pCHAR	timdelay;
	pCHAR	timeLimit;
	pCHAR	timelag;
	pCHAR	timestepDiffHandling;
	pCHAR	timsKey;
	pCHAR	timsRef;
	pCHAR	timsVsc;
	pCHAR	timstamp;
	pCHAR	toHour;
	pCHAR	todate;
	pCHAR	toplevel;
	pCHAR	total;
	pCHAR	totime;
	pCHAR	traStat;
	pCHAR	trade;
	pCHAR	tradeKey;
	pCHAR	tratKea;
	pCHAR	tratKey;
	//pCHAR	trbcap;
	pCHAR	trcode;
	pCHAR	trcode1;
	pCHAR	trcode2;
	pCHAR	trcodeii;
	pCHAR	trobjco1;
	pCHAR	trobjco2;
	pCHAR	trotKey;
	pCHAR	trrpKey;
	pCHAR	tsType;
	pCHAR	tscaKey;
	pCHAR	tscode;
	pCHAR	tserKey;
	pCHAR	tsexp;
	pCHAR	tsfnKey;
	pCHAR	tsforget;
	pCHAR	tsinKey;
	pCHAR	tsinKeyExport;
	pCHAR	tsmax;
	pCHAR	tsmean;
	pCHAR	tsmin;
	pCHAR	tsoOpunKey;
	pCHAR	tsorKey;
	pCHAR	tsprKey;
	pCHAR	tsquKey;
	pCHAR	tsstatus;
	pCHAR	tsteRef;
	pCHAR	tstrKey;
	pCHAR	tsvalue;
	pCHAR	tsvar1;
	pCHAR	tsvar2;
	pCHAR	tswiRef;
	pCHAR	tunnarea;
	pCHAR	unit;
	pCHAR	unitregn;
	pCHAR	unittype;
	pCHAR	unmeKea;
	pCHAR	unmeKey;
	pCHAR	unvdKey;
	//pCHAR	upDown;
	pCHAR	updint;
	pCHAR	upperlim;
	//pCHAR	used;
	pCHAR	useRegistrationHandling;
	pCHAR	userKey;
	//pCHAR	ustpKey;
	pCHAR	utcOffsetExportDates;
	pCHAR	utmEast;
	pCHAR	utmNort;
	pCHAR	valFact;
	pCHAR	valKey;
	pCHAR	valMeth;
	pCHAR	valcok;
	pCHAR	valftime;
	pCHAR	valtran;
	pCHAR	valttime;
	//pCHAR	value;
	//pCHAR	variance;
	//pCHAR	vcgrKey;
	//pCHAR	vcgrTyp;
	pCHAR	version;
	pCHAR	violdate;
	pCHAR	visitadr;
	pCHAR	vlcode;
	pCHAR	volts;
	pCHAR	volume;
	//pCHAR	vsta000;
	//pCHAR	vsta001;
	//pCHAR	vsta002;
	//pCHAR	vsta003;
	//pCHAR	vsta004;
	//pCHAR	vsta005;
	//pCHAR	vsta006;
	//pCHAR	vsta007;
	//pCHAR	vsta008;
	//pCHAR	vsta009;
	//pCHAR	vsta010;
	//pCHAR	vsta011;
	//pCHAR	vsta012;
	//pCHAR	vsta013;
	//pCHAR	vsta014;
	//pCHAR	vsta015;
	//pCHAR	vsta016;
	//pCHAR	vsta017;
	//pCHAR	vsta018;
	//pCHAR	vsta019;
	//pCHAR	vsta020;
	//pCHAR	vsta021;
	//pCHAR	vsta022;
	//pCHAR	vsta023;
	//pCHAR	vsta024;
	//pCHAR	vsta025;
	//pCHAR	vsta026;
	//pCHAR	vsta027;
	//pCHAR	vsta028;
	//pCHAR	vsta029;
	//pCHAR	vsta030;
	//pCHAR	vsta031;
	//pCHAR	vsta032;
	//pCHAR	vsta033;
	//pCHAR	vsta034;
	//pCHAR	vsta035;
	//pCHAR	vsta036;
	//pCHAR	vsta037;
	//pCHAR	vsta038;
	//pCHAR	vsta039;
	//pCHAR	vsta040;
	//pCHAR	vsta041;
	//pCHAR	vsta042;
	//pCHAR	vsta043;
	//pCHAR	vsta044;
	//pCHAR	vsta045;
	//pCHAR	vsta046;
	//pCHAR	vsta047;
	//pCHAR	vsta048;
	//pCHAR	vsta049;
	//pCHAR	vsta050;
	//pCHAR	vsta051;
	//pCHAR	vsta052;
	//pCHAR	vsta053;
	//pCHAR	vsta054;
	//pCHAR	vsta055;
	//pCHAR	vsta056;
	//pCHAR	vsta057;
	//pCHAR	vsta058;
	//pCHAR	vsta059;
	//pCHAR	vsta060;
	//pCHAR	vsta061;
	//pCHAR	vsta062;
	//pCHAR	vsta063;
	//pCHAR	vsta064;
	//pCHAR	vsta065;
	//pCHAR	vsta066;
	//pCHAR	vsta067;
	//pCHAR	vsta068;
	//pCHAR	vsta069;
	//pCHAR	vsta070;
	//pCHAR	vsta071;
	//pCHAR	vsta072;
	//pCHAR	vsta073;
	//pCHAR	vsta074;
	//pCHAR	vsta075;
	//pCHAR	vsta076;
	//pCHAR	vsta077;
	//pCHAR	vsta078;
	//pCHAR	vsta079;
	//pCHAR	vsta080;
	//pCHAR	vsta081;
	//pCHAR	vsta082;
	//pCHAR	vsta083;
	//pCHAR	vsta084;
	//pCHAR	vsta085;
	//pCHAR	vsta086;
	//pCHAR	vsta087;
	//pCHAR	vsta088;
	//pCHAR	vsta089;
	//pCHAR	vsta090;
	//pCHAR	vsta091;
	//pCHAR	vsta092;
	//pCHAR	vsta093;
	//pCHAR	vsta094;
	//pCHAR	vsta095;
	pCHAR	vstaint;
	pCHAR	vuNo;
	pCHAR	watcKey;
	//pCHAR	week01;
	//pCHAR	week02;
	//pCHAR	week03;
	//pCHAR	week04;
	//pCHAR	week05;
	//pCHAR	week06;
	//pCHAR	week07;
	//pCHAR	week08;
	//pCHAR	week09;
	//pCHAR	week10;
	//pCHAR	week11;
	//pCHAR	week12;
	//pCHAR	week13;
	//pCHAR	week14;
	//pCHAR	week15;
	//pCHAR	week16;
	//pCHAR	week17;
	//pCHAR	week18;
	//pCHAR	week19;
	//pCHAR	week20;
	//pCHAR	week21;
	//pCHAR	week22;
	//pCHAR	week23;
	//pCHAR	week24;
	//pCHAR	week25;
	//pCHAR	week26;
	//pCHAR	week27;
	//pCHAR	week28;
	//pCHAR	week29;
	//pCHAR	week30;
	//pCHAR	week31;
	//pCHAR	week32;
	//pCHAR	week33;
	//pCHAR	week34;
	//pCHAR	week35;
	//pCHAR	week36;
	//pCHAR	week37;
	//pCHAR	week38;
	//pCHAR	week39;
	//pCHAR	week40;
	//pCHAR	week41;
	//pCHAR	week42;
	//pCHAR	week43;
	//pCHAR	week44;
	//pCHAR	week45;
	//pCHAR	week46;
	//pCHAR	week47;
	//pCHAR	week48;
	//pCHAR	week49;
	//pCHAR	week50;
	//pCHAR	week51;
	//pCHAR	week52;
	//pCHAR	week53;
	pCHAR	weekoff;
	pCHAR	worktask;
	pCHAR	wtgateno;
	pCHAR	wtgtKey;
	pCHAR	wtrfKey;
	pCHAR	wtroKey;
	pCHAR	wtroutno;
	pCHAR	xCoordinate;
	pCHAR	xunmeas;
	pCHAR	yCoordinate;
	pCHAR	yearweek;
	pCHAR	yunmeas;
	pCHAR	zipCode;
	pCHAR	zunmeas;
};

// extern DBKeyWords k;
extern iccTables icc_t;
extern iccFields icc_f;
#endif	// ICCTABLES_H
