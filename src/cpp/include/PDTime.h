#ifndef _PDTime_h_
#define _PDTime_h_

#include <rw/rwtime.h>
// Persistence
#include <rw/edefs.h>
#include "PDDate.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <stream_formats.h>

//---------------------------------------------
// old defs for time as defined in time_api.h
typedef char TIME_c[18];
typedef int TIME_i[7];
typedef int TIME_w[3]; // [0]=Year[dddd] [1]=Week[1..53] [2]=Day[1..7] Mo..Su
typedef const char* TIME_cp;

//-----------------------------------------------------
// pdtime, the Powel Data equivalent to time_t 
typedef __int64 pdtime;
const pdtime MAXPDTIME=0x7FFFFFFFFFFFFFFFL;
const pdtime MINPDTIME=-MAXPDTIME;
const pdtime NOPDTIME= MINPDTIME -1L;

const pdtime PDT_SEC=1;
const pdtime PDT_MIN=PDT_SEC*60;
const pdtime PDT_HOUR= PDT_MIN*60;
const pdtime PDT_DAY= PDT_HOUR*24;
const pdtime PDT_WEEK= PDT_DAY*7;
const pdtime PDT_MONTH= PDT_DAY*30;// only for abstract use
const pdtime PDT_YEAR=PDT_DAY*365; // only for abstract use

extern __declspec(thread) int timeNowDebugOffset;

// period defined as [start..end>
// a period is valid if start and end is valid timestamps
// and end>=start. This implies that a 0 period is valid.
template <class T, T noval>
struct period {
	period():start(noval),end(noval) {}
	period(T start_,T end_):start(start_),end(end_) {}
	bool   valid() const { return end!=noval && start != noval && end>=start;}
	T duration() const {return valid()?end-start:noval;}
	bool contains(T t) const {return valid()&& t>=start && t<end;}
	bool contains(period p) const {return valid()&& p.start>=start && p.end<=end;}
	bool overlap(const period& b) const {
		if(valid() && b.valid()) {
			return !(b.end <= start || b.start >= end);
		} else 
			return false;
	}
	period section(const period& b) const {
		period r;
		if( overlap(b)) {
			r.start= max(start,b.start);r.end=min(end,b.end);
		} 
		return r;
	}
	bool operator==(const period&x ) const {return x.start==start && x.end==end;}
	bool operator!=(const period&x ) const {return !operator==(x);}
	T start;
	T end;
};
typedef period<pdtime,NOPDTIME> pdperiod;

#define PDT_HMS(h,m,s) ((h)*PDT_HOUR+(m)*PDT_MIN+(s)*PDT_SEC)
    extern pdtime pdtimeNow();
#define julian1970_d 2440588L // offset in days from juliand day to 1970 UTC
	const pdtime julian1970_s=julian1970_d*PDT_DAY;
//-- basic julian date aritmetic is inline or macro:
#define macroYMD2j(year,month,day) (long(day) - 32075L + 1461L * (long(year) + 4800 + (long(month) - 14L) / 12L) / \
				4L + 367L * (long(month) - 2L - (long(month) - 14L) / 12L * 12L) / 12L - 3L * ((long(year) + 4900L + \
				(long(month) - 14L) / 12L) / 100L) / 4L)


#define macroj2YMD(jul_date, year ,month,day) {\
		long  t1,     t2,      yr,       mo; \
		t1 = (jul_date) + 68569L;   t2 = 4L * t1 / 146097L; \
		t1 = t1 - (146097L * t2 + 3L) / 4L;   yr = 4000L * (t1 + 1) / 1461001L; \
		t1 = t1 - 1461L * yr / 4L + 31;   mo = 80L * t1 / 2447L; \
		day = (int)(t1 - 2447L * mo / 80L);  t1 = mo / 11L; \
		month = (int)(mo + 2L - 12L * t1);   year = (int)(100L * (t2 - 49L) + yr + t1); \
	}

//-- for TZ lookup, need something blending fast to get to/from year and julian
// - macros below are derived from the above, m=1, d=1
#define	 macroj2Y(jul_date,year)  {\
		long  t1,t2,yr,mo; \
		t1 = (jul_date) + 68569L;   t2 = 4L * t1 / 146097L; \
		t1 = t1 - (146097L * t2 + 3L) / 4L;   yr = 4000L * (t1 + 1) / 1461001L; \
		t1 = t1 - 1461L * yr / 4L + 31;   mo = 80L * t1 / 2447L; \
		t1 = mo / 11L; \
		year= (int)(100L * (t2 - 49L) + yr + t1); \
	}

#define macroY2j(year) ( -31738L + 1461L * ((year) + 4799) / 4L  - 3L * (((year) + 4899L ) / 100L) / 4L)
	inline long YMD2j(int year, int month, int day) {
		return macroYMD2j(year,month,day);
	}
	
	inline void j2YMD(long jul_date, int& year ,   int &month, int &day) {
		macroj2YMD(jul_date,year,month,day);
	}
	inline int j2Y(long jul_date) {
		int year;macroj2Y(jul_date,year);
		return year;
	}


#define pdtime2j(t) (long( (julian1970_s + (t))/PDT_DAY))
#define j2pdtime(j) (pdtime((j)-julian1970_d)*PDT_DAY)
//	inline long pdtime2j(pdtime t) {	return julian1970_d + long( t/PDT_DAY);}
//	inline pdtime j2pdtime(int j) { return pdtime(j-julian1970_d)*PDT_DAY;}
#ifdef __cplusplus

//------------------------
// conversion between RWTime,PDTime and pdtime, TODO: add handling of 'NO time', range missmatch etc
const double DATE_MINDATE=1.0;
const double DATE_MAXDATE= 365.0 *(9999.0-1900.0);// range from 1900 to approx 9999

class PDZone;
class PDTimePeriod;
class PDTime;
class PDTimePeriodIterator;
extern void PDZone_init();
extern /*const*/ PDZone *Zlocal_;
extern /*const*/ PDZone *Zstandard_;
extern /*const*/ PDZone *Zdb_;
extern /*const*/ PDZone *Zutc_;
extern /*const*/ PDZone *Zdate_;

// Definering av bakoverkompatibilitet. Disse bør finnes i default zone initialisering.
#define Zutc (*Zutc_)
#define Zlocal (*Zlocal_)
#define Zdb (*Zdb_)
#define Zstandard (*Zstandard_)

typedef double DATE;	
extern pdtime aspdtime(const PDTime&t);
extern pdtime aspdtime(const RWTime&t);
inline pdtime aspdtime(const RWDate& t,const PDZone *z);
extern pdtime aspdtime(DATE x,const PDZone *z=Zdate_);
extern pdtime txt2pdtime(const char *c,const PDZone *z=0,pdtime Tnow=NOPDTIME,const PDZone **zused=0);// as freetext user input into pdtime using z
extern RWTime asRWTime(pdtime t);
inline RWDate asRWDate(pdtime t,const PDZone *z);// due to RW backw. compat
inline PDTime asPDTime(pdtime t);
DATE asDATE(pdtime xt,const PDZone *z=Zdate_ );
extern RWCString toXmlDateTime(pdtime t, const PDZone& zone = Zstandard);
extern pdtime fromXmlDateTime(const char* txt);
#define LeapYear(year) ((year&3) == 0 && (year % 100 != 0 || year % 400 == 0))
extern const unsigned char _DaysInMonth[12];/* = {31,28,31,30,31,30,31,31,30,31,30,31};*/
extern const unsigned _FirstDayOfEachMonth[12];/* = {0,31,59,90,120,151,181,212,243,273,304,334};*/


#define PDZone_dstyears 200
#define PDZone_dstyear_base 1900
//--------------------------------
// Standard Zones for PDZone
// Zdb for converstion to/from 17c datestrings related to db
// These global zone pointers are initialized by call to PDZone_init
// 
#ifdef WIN32
// int constructors should be explicit in order
// to avoid unwanted type conversions from any int to PDTime
#define Explicit explicit
#else
// Quite unsafe infact !!
// TODO: Check other platforms hp,sun,dec if they all supports keyword explicit
#define Explicit
#endif
class PDTime;
class PDZone {
 public:
	PDZone();//-- binary copy ct, equal and assign is OK
	PDZone(long UTCsec,const char *zoneName);// UTC+1:UTCsec=3600, UTC-1:UTCsec=-3600
	const RWCString& timeZoneName() const { return zName;};
	const RWCString& timeZoneName(pdtime x) const { return isDST(x) ? altZoneName() : timeZoneName(); }
	const RWCString& altZoneName() const { return azName;}
	void setTimeZoneName(const RWCString& s) { zName=s;};
	void setAltZoneName(const RWCString& s) {azName=s;}
	//--
	//-- Daylight Saving Time :
	bool   daylightObserved() const { return dst;}
	pdtime DST_at(pdtime x) const ;// 0: no dst for x, otherwise number of seconds DST  x
	pdtime DST_start (int year) const;// returns first time within dst for year
	pdtime DST_end   (int year) const;// returns first time after DST for  year
	pdtime DST_offset(int year) const;// returns additional DST for given year
	bool   isDST(pdtime x) const { return dst? (DST_at(x)?true:false):false;}
	bool   addDSTRule(pdtime dstStart,pdtime dstEnd,pdtime dstOffset=3600);
	bool   addDSTRule(int Yfrom,int Yto,
	                  bool dst,
	                  int Month_b,int Day_b,pdtime sec_b,
	                  int Month_e,int Day_e,pdtime sec_e,
	                  pdtime dstOffset);
	//-- Rule applies to Year Yfrom to Yto (not incl)						
	// if true, use begin/end dstOffset below
	// DST begins at Month_b(1..12) Day_b(1..31 or -1 -2 -3 -4 for nth sunday in month
	// sec_b, seconds into day when it starts, NB all magnitudes interpreted as UTC ( 01:00)
	// DST ends at Month_e, Day_e,sec_e, UTC, defined as the first sec when there is NO DST (01:00)
	// dstOffset number of seconds DST, normally PDT_HOUR
	
	//-- Timezone
	pdtime diffGMT(pdtime t=NOPDTIME) const;// returns gmt offset for t (incl dst)
	void   setTZ(pdtime secsWestOfUtc) { std_tz=secsWestOfUtc;}
	pdtime getTZ() const { return std_tz;}
	bool   getDST() const { return dst;}
	void   setDST(bool active) { dst=active;}

	//-- WEEK functions 
	enum WeekOneRule { FirstJanWeek,FirstFullWeek,First4DayWeek };
	void   setWeekStartDay(int d) { weekStartDay=d;}// d 0..6 > Sun .. Sat
	int    getWeekStartDay() const { return weekStartDay;}
	pdtime weekOne(int Y) const ;// time of week one for a given year
	void   setWeekOneRule(WeekOneRule wor) {weekOneRule=wor;}
	WeekOneRule getWeekOneRule()const { return weekOneRule;}

	//-- Zone dependent truncation/trim and addition, diff
	pdtime trimYear (pdtime t) const;
	pdtime trimMonth(pdtime t) const;
	pdtime trimWeek (pdtime t) const;
	pdtime trim (pdtime t, pdtime x) const;
	pdtime add(pdtime t, pdtime step,int n_units,bool CheckPrev = true) const;	// Adds n_units steps to t. CheckPrev=true: makes DST-check for timestamps 1 HOUR before result, =false: does linear addition on steps < WEEK.
    int diffUnits(const pdtime time1, const pdtime time2, const pdtime step, pdtime * rest = nullptr) const;
    int fill(const pdtime timeFrom, const pdtime timeTo, const pdtime step
        , std::vector<pdtime>::iterator first, std::vector<pdtime>::iterator last) const;
	//-- Converting to calendar magnitudes Y,M,D,h,m,s Wno, Dw, Wday
	bool asYMD(pdtime t,int& Y,int&M,int&D) const;
	bool asYMDhms_wd(pdtime t,int& Y,int&M,int&D,int&h,int&m,int&s,int& weekd) const;
	bool asYWDhms(pdtime t,int &Yw,int &Wn,int &Dw,int &h,int &m,int &s) const;
	bool asYW    (pdtime t,int &Yw,int &Wn) const;// YearWeek and WeekNo of t
	bool astm (pdtime t,struct tm *tx)const;
	//-- values one by one
	int year(pdtime t)const  {int y,m,d;asYMD(t,y,m,d);return y;}
	int month(pdtime t) const{int y,m,d;asYMD(t,y,m,d);return m;};
	int day (pdtime t) const {int y,m,d;asYMD(t,y,m,d);return d;};
	int hour(pdtime t) const { return int(((julian1970_s+t+diffGMT(t))%PDT_DAY)/PDT_HOUR);}
	int minute (pdtime t) const { return int(((julian1970_s+t+diffGMT(t))%PDT_HOUR)/PDT_MIN);}
	int second (pdtime t) const { return int(((julian1970_s+t+diffGMT(t))%PDT_MIN));}
	int  weekDay(pdtime t) const;// 0..6 Su..Sat
	int firstDayOfMonth(pdtime t) const { 
		int y,m,d;asYMD(t,y,m,d);
		int yd=1+_FirstDayOfEachMonth[m-1];
		return m>2 && LeapYear(y) ? yd+1:yd;
	}
	int dayOfMonth(pdtime t) const {
		int y,m,d;asYMD(t,y,m,d);
		return d;
	}
	int dayOfYear(pdtime t) const {
		long j=pdtime2j(t+diffGMT(t));
		int y=j2Y(j);
		return 1+ j-YMD2j(y,1,1);
	}
	RWCString dayName(int wd) const;// wd 0..6 su..sa
	RWCString monthName(int m) const;// m 1..31
	RWCString shortDayName(int wd) const;// wd 0..6 su..sa
	RWCString shortMonthName(int m) const;
	bool extract(pdtime t,struct tm *tx) const {return astm(t,tx);}
	//-- Converting from calendar magnitudes to pdtime
	pdtime aspdtime(int Y,int M,int D,int h,int m,int s) const;
	pdtime aspdtimeW(int Y,int W,int Wd,int h,int m,int s) const;// Year Week[1..53],WeekDay[0..6](su..sa)    
	pdtime aspdtime(const struct tm *tms) const;
	//-- converting to calendar text string
	std::string asString(PDTime t)const; 
	
	char * asString(pdtime t,const char *fmtStr, char *resultStr) const;
	RWCString asString(pdtime t,const char *fmtStr) const;
	
	static PDZone* CreateZoneFromFileOrRWZone(const char* TzName);
	static std::string Trace(pdtime t);

    const static PDZone & ReadValuesTz();
    static void SetReadValuesTzOffsetAndDlst(const int utcOffset, const bool isDlst);

 private:
	void init();
	RWCString zName;
	RWCString azName;
	int weekStartDay; // 0..6 su mo tu 
	WeekOneRule weekOneRule;
	bool   dst; // true if dst in this zone 
	pdtime std_tz;// UTC+1 is 3600, UTC-1 is -3600 etc.
	pdtime dst_start  [PDZone_dstyears];//   DST >= : works for northern sphere only
	pdtime dst_end    [PDZone_dstyears];// < DST  :index by yearOf(t) - PDZone_dstyear_base	
	pdtime dst_offset [PDZone_dstyears];// during dst, the additional # seconds west of GMT
};

inline std::ostream & operator<<(std::ostream & os, const PDZone &  tz)
{
    os << "(" << tz.timeZoneName() << ", DLST " << std::boolalpha << tz.getDST() << ", " << tz.getTZ() << ")";
    return os;
}

extern RWCString asString(pdtime t, const char *fmtStr, const PDZone & z);

inline pdtime PDZone::diffGMT(pdtime t) const {
	if(!dst) return std_tz;
	int y;macroj2Y(pdtime2j(t),y);
	y -=PDZone_dstyear_base;if(y<0 || y>=PDZone_dstyears) return std_tz;
	if (dst_start[y]<dst_end[y])return std_tz + (t>=dst_start[y] && t < dst_end[y] ? dst_offset[y]:0);
	return std_tz + (t >= dst_start[y] || t < dst_end[y] ? dst_offset[y] : 0);
}

inline pdtime PDZone::DST_start(int year) const {
	if(!dst) return NOPDTIME;
	int y=year-PDZone_dstyear_base;if(y<0 || y>=PDZone_dstyears) return NOPDTIME;
	return dst_start[y];
}
inline pdtime PDZone::DST_end (int year) const {
	if(!dst) return NOPDTIME;
	int y=year-PDZone_dstyear_base;if(y<0 || y>=PDZone_dstyears) return NOPDTIME;
	return dst_end[y];
}
inline pdtime PDZone::DST_offset(int year) const {
	if(!dst) return 0;
	int y=year-PDZone_dstyear_base;if(y<0 || y>=PDZone_dstyears) return 0;
	return dst_offset[y];
}

inline int PDZone::weekDay(pdtime t) const {
	t+=diffGMT(t);
	return int( (7+4+(t%PDT_WEEK)/PDT_DAY)%7);
}
inline pdtime PDZone::trimYear (pdtime t) const {
	t+=diffGMT(t);// add tz to get it right
	int y;macroj2Y(pdtime2j(t),y);
	t=j2pdtime(macroY2j(y));
	t -= diffGMT(t);
	return t;
}
inline pdtime PDZone::trimMonth (pdtime t) const {
	t+=diffGMT(t);
	int y,m,d;macroj2YMD(pdtime2j(t),y,m,d);
	t=j2pdtime(macroYMD2j(y,m,1));
	t -= diffGMT(t);
	return t;
}
inline pdtime PDZone::trimWeek (pdtime t) const {
	return trim(t,PDT_WEEK);
}

PDZone* ReadZoneFromFile(const char *fn,char *&_errmsg);// Function to create zone from file
PDZone *asPDZone(const RWZone &TZos) ; /*
    Create a PDZone object compatible with a given RWZone.
    Returns a heap allocated PDZone
    */
//---- PDZoneList ---------------------------------------------------
// Singleton-klasse som er en vektor av PDZone-objekter.
// Vektoren initialiseres ved lesing av miljøvariabelen ICC_TIMEZONE_LIST
// som inneholder en komma-separert liste over tidssoner som skal initialiseres.
// Hvert navn i denne listen bør korrespondere med en fil med prefix TZ_
// Denne filen definerer den enkelte tidssonen.
//
// Aksessering av listen gjøres ved PDZoneList::getInstance()

struct PDZoneElem {
	RWCString _name;	// Internal name. local, standard, db etc.
	PDZone *_z;	// Ikke eier. Det finnes ikke copy-ctor.
	bool _visible;
	PDZoneElem(const RWCString& N,PDZone *Z,bool V = false)
		: _name(N), _z(Z), _visible(V)
		{}
	PDZoneElem(const PDZoneElem& E)
		: _name(E._name), _z(E._z), _visible(E._visible)
		{}
	~PDZoneElem()
		{ /*delete _z;*/ }	// Pga. copy-ctor
	PDZoneElem& operator=(const PDZoneElem& E) 
		{ _name = E._name; _z = E._z; _visible = E._visible; return *this; }
};

class PDZoneList 
{
    std::vector<PDZoneElem> zoneList;
public:
	PDZoneList();
	~PDZoneList();
    void push_back(const PDZoneElem& e) { zoneList.push_back(e); }
    size_t size() const { return zoneList.size(); }
    PDZoneElem& operator[](size_t i) { return zoneList[i]; }

    const PDZone* findInternal(const std::string& Name) const;	// Leter i PDZoneElem._name
    const PDZone* findExternal(const std::string& Name) const;	// Leter i PDZone.zName og PDZone.azName
    const PDZone* find(const std::string& Name) const; // Searches in bot internal and external lists
    const PDZone* findEx(const std::string& Name) const; // First tries to use Z*_ global pointers, then uses find()
    const PDZoneElem& findElem(const PDZone* Z) const;
    size_t index(const RWCString& Iname) const;
    PDZoneElem at(size_t i) { return zoneList.at(i); }
    static PDZoneList* getInstance();
	static const PDZone& getDefault(const RWCString& AppName = RWCString());	// Henter ut jfr. ICC_DEFAULT_TIME(_AppName)
	static RWCString getDefaultName(const RWCString& AppName = RWCString());
	// Variabler som kan brukes i stedet for "local" rundt i koden.
	static const char* standardName;
	static const char* localName;
	static const char* dbName;
	static const char* utcName;
	static const char* dateName;
};

//___________________________________________________________________
#define SEC_PR_MIN	60
#define SEC_PR_HOUR	3600
#define SEC_PR_DAY	86400
#define SEC_PR_WEEK	604800

//---- PDTimeSpan -----------------------------------------------------------
class PDTimeSpan {
 public:
	static inline PDTimeSpan year(){return PDTimeSpan(PDT_YEAR);}
	static inline PDTimeSpan month(){return PDTimeSpan(PDT_MONTH);}
	static inline PDTimeSpan week(){return PDTimeSpan(PDT_WEEK);}
	static inline PDTimeSpan day(){return PDTimeSpan(PDT_DAY);}
	static inline PDTimeSpan hour(){return PDTimeSpan(PDT_HOUR);}
	static inline PDTimeSpan minute(){return PDTimeSpan(PDT_MIN);}
	static inline PDTimeSpan second(){return PDTimeSpan(PDT_SEC);}
	static inline PDTimeSpan tick(){return PDTimeSpan(pdtime(1));}

	PDTimeSpan():t_(0){;}
	//Explicit PDTimeSpan(long sec) :t_(sec){;}
	Explicit PDTimeSpan(pdtime s): t_(s) {;}
	PDTimeSpan(ICC_TIME_UNITS unit) :t_(seconds(unit)) {;}
	PDTimeSpan(pdtime n, ICC_TIME_UNITS unit): t_(pdtime(n) * pdtime(seconds(unit))) { }
	PDTimeSpan(const PDTimeSpan& timestep) :t_(timestep.t_) {;}
	//TODO: If this results in more efficient code, include it:~PDTimeSpan() {;}
		
	PDTimeSpan& operator=(pdtime sec) { t_=sec;return *this;}
	PDTimeSpan operator*(long sec) const { return PDTimeSpan(t_*pdtime(sec*PDT_SEC));}
	PDTimeSpan operator/(long sec) const { return PDTimeSpan(t_/pdtime(sec*PDT_SEC));}
	PDTimeSpan operator-(long sec) const { return PDTimeSpan(t_-pdtime(sec*PDT_SEC));}
	PDTimeSpan operator+(long sec) const { return PDTimeSpan(t_+pdtime(sec*PDT_SEC));}


	PDTimeSpan operator*(PDTimeSpan x) const { return PDTimeSpan(t_*x.t_);}
	PDTimeSpan operator/(PDTimeSpan x) const  { return PDTimeSpan(t_/x.t_);}

	PDTimeSpan operator-(PDTimeSpan x) const { return PDTimeSpan(t_-x.t_);}
	PDTimeSpan operator+(PDTimeSpan x) const  { return PDTimeSpan(t_+x.t_);}

	
	bool operator==(const PDTimeSpan& o) const { return t_==o.t_;}
	bool operator!=(const PDTimeSpan& o) const { return t_!=o.t_;}
	bool operator<(const PDTimeSpan& o) const { return t_<o.t_;}
	bool operator<=(const PDTimeSpan& o) const { return t_<=o.t_;}
	bool operator>(const PDTimeSpan& o) const { return t_>o.t_;}
	bool operator>=(const PDTimeSpan& o) const { return t_>=o.t_;}
	pdtime seconds() const { return t_; }
	double minutes() const { return t_/double(SEC_PR_MIN); }
	long   totalMinutes() const { return long(t_/SEC_PR_MIN); }
	double hours() const { return t_/double(SEC_PR_HOUR); }
	long   totalHours() const { return long(t_/SEC_PR_HOUR); }
	double days() const { return t_/double(SEC_PR_DAY); }
	long   totalDays() const { return long(t_/SEC_PR_DAY); }
	double weeks() const { return t_/double(SEC_PR_WEEK); }
	long   totalWeeks() const { return long(t_/SEC_PR_WEEK); }
	long   totalUnits(ICC_TIME_UNITS unit) const { long secPrUnit=seconds(unit); if (secPrUnit==0) return 0; else return long( t_/secPrUnit); }
	ICC_TIME_UNITS unit() const;
	// number of units dt fits into this timespan, including sign as pr usual math. if dt is 0, 0 is returned(avoid div/zero?)
	long units(const PDTimeSpan dt) const {return dt.t_? long(t_ / dt.t_) : 0;}
		
		
	// set-metoder.
	void setSeconds(const pdtime sec) { t_ = sec; }
	void setMinutes(const pdtime min) { t_ = min * SEC_PR_MIN; }
	void setHours(const pdtime hours) { t_ = hours * SEC_PR_HOUR; }
	void setDays(const pdtime days)   { t_ = days * SEC_PR_DAY; }
	void setWeeks(const pdtime weeks) { t_ = weeks * SEC_PR_WEEK; }

	static long seconds(ICC_TIME_UNITS unit);
    static pdtime interpretTimeSpanText(const char *txt);

	RWspace binaryStoreSize() const { return sizeof(t_); }
private:
	pdtime t_;
	friend class PDTime;
	friend class PDTimePeriod;
	friend class PDTimePeriodIterator;
	friend PDTime operator+(const PDTime& t,const PDTimeSpan& step);
	friend PDTime operator-(const PDTime& t,const PDTimeSpan& step);
	friend void rwSaveGuts(RWFile& f,const PDTimeSpan& i);
	friend void rwSaveGuts(RWvostream& s,const PDTimeSpan& i);
	friend void rwRestoreGuts(RWFile& f,PDTimeSpan& i);
	friend void rwRestoreGuts(RWvistream& s,PDTimeSpan& i);
	friend PDTimeSpan operator*(long sec,PDTimeSpan x) ;
	friend PDTimeSpan operator/(long sec,PDTimeSpan x) ;
	friend PDTimeSpan operator-(long sec,PDTimeSpan x) ;
	friend PDTimeSpan operator+(long sec,PDTimeSpan x) ;

};
inline PDTimeSpan operator*(long sec,PDTimeSpan x)  { return PDTimeSpan(pdtime(sec*PDT_SEC)*x.t_);}
inline PDTimeSpan operator/(long sec,PDTimeSpan x)  { return PDTimeSpan(pdtime(sec*PDT_SEC)/x.t_);}
inline PDTimeSpan operator-(long sec,PDTimeSpan x)  { return PDTimeSpan(pdtime(sec*PDT_SEC)-x.t_);}
inline PDTimeSpan operator+(long sec,PDTimeSpan x)  { return PDTimeSpan(pdtime(sec*PDT_SEC)+x.t_);}

RWDECLARE_PERSISTABLE(PDTimeSpan);	// Siden klassen ikke arver fra RWCollectable.

//____________________________________________________________________
//
//
class PDTime 
{
public:
	PDTime() {t_=NOPDTIME;} // RWTime uses current time, but that would destroy performance !!!
	Explicit PDTime(pdtime t) {t_=t;}
//TODO:	Explicit PDTime(unsigned long s,unsigned short ms = 0): RWTime(s),_msec(ms), _timeC(0) {}
	PDTime(unsigned h,unsigned m,unsigned s,unsigned short ms = 0,const PDZone& zone = Zstandard) {
		t_ = zone.trim(pdtimeNow(),PDT_DAY)+ PDT_HMS(h,m,s);
	}
	PDTime(const RWDate& date,unsigned h = 0,unsigned m = 0,unsigned s = 0,unsigned short ms = 0,const PDZone& zone = Zstandard) {
		//: RWTime(date,h,m,s,zone),_msec(ms), _timeC(0) {}
		t_= zone.aspdtime(date.year(),date.month(),date.dayOfMonth(),h,m,s);
	}
	PDTime(const struct tm *tms,unsigned short ms = 0,const PDZone& zone = Zstandard) {
		//	: RWTime(tms,zone),_msec(ms), _timeC(0) {}
		t_= zone.aspdtime(tms);
	}
	PDTime(const PDDate& date,const RWCString& str,const RWZone& zone = RWZone::standard(),const RWLocale& localex = RWLocale::global()) {
		//	: RWTime(date,str,zone,localex),_msec(0), _timeC(0) {}
		t_= ::aspdtime(RWTime(date,str,zone,localex));//TODO:Figure out how to fix this bastard function
	}
	PDTime(const TIME_w YWD,int h,int m,int s,const PDZone& z=Zstandard);
	PDTime(const PDTime& o){t_=o.t_; }
	PDTime(const RWTime& o){ t_=::aspdtime(o);}
	PDTime(const TIME_c t,const PDZone& zone = Zstandard) {t_ = PDTime::aspdtime(t, &zone);}
	//~PDTime(){} Better not define it to avoid a call to it 


	bool between(const PDTime& a,const PDTime& b) const { return t_>=a.t_ && t_<=b.t_;}
	RWspace binaryStoreSize() const { return sizeof(t_);}
	int compareTo(const PDTime* t) const { 
		if(!t) return -1;
		if(t->t_ == t_) return 0;
		return t_>t->t_?1:-1;
	}
	bool isValid() const { return t_ != NOPDTIME; }
    //SIV. isSpecific() added to avoid meaningless operations on NPDTIME, MAXPDTIME, MINPDTIME, like e.g. add() or addDays()
    bool isSpecific() const { return t_ != NOPDTIME && t_ != MAXPDTIME && t_ != MINPDTIME; }

//-VC7	PDTime PDTime::max(const PDTime& t) const { return t_ > t.t_? t_ : t.t_;}
//-VC7	PDTime PDTime::min(const PDTime& t) const { return t_ < t.t_? t_ : t.t_;}
friend	PDTime max(const PDTime a,const PDTime b) ;
friend	PDTime min(const PDTime a,const PDTime b) ;

	unsigned  year(const PDZone& z=Zstandard ) const { return z.year(t_); }
	bool leap(const PDZone& z=Zstandard)  const { return LeapYear(z.year(t_)); }
	unsigned  month(const PDZone& z=Zstandard) const { return z.month(t_); }
	RWCString monthName(const PDZone& z=Zstandard) const { return z.monthName(z.month(t_)); }
	unsigned firstDayOfMonth(const PDZone& z=Zstandard) const  { return z.firstDayOfMonth(t_); }
	//TODO: if used -	unsigned firstDayOfMonth(unsigned month) const{ return RWDate(*this).firstDayOfMonth(month); }
	unsigned dayOfMonth(const PDZone& z = Zstandard) const { return z.dayOfMonth(t_);}
	unsigned week(const PDZone& z=Zstandard) const { int y,w;z.asYW(t_,y,w);return w; }
	unsigned weekDay(const PDZone& z=Zstandard) const { int wd=z.weekDay(t_);return wd==0?7:wd;}
	RWCString weekDayName(const PDZone& z=Zstandard) const { return z.dayName(z.dayOfMonth(t_));}
	unsigned day(const PDZone& z=Zstandard) const { return z.dayOfYear(t_); }
	unsigned hour  (const PDZone& z= Zstandard) const { return z.hour(t_);}
	unsigned hourno(const PDZone& z = Zstandard) const	{ return z.hour(t_) + 1; }
	unsigned minute(const PDZone& z = Zstandard) const { return z.minute(t_);}
	unsigned second(const PDZone& z = Zstandard) const { return z.second(t_);}
	unsigned short millisecond() const { return 0; }
	bool isDST(const PDZone& z) { return z.isDST(t_);}
	const RWCString& timeZoneName(const PDZone& Z) const { return Z.timeZoneName(t_); }
	static PDTime beginDST(int year,const PDZone&z);
	static PDTime   endDST(int year,const PDZone&z);
	unsigned long julian() const { return pdtime2j(t_); }
	//__declspec(deprecated("This is a bug factory")) TIME_cp getTimeC(const PDZone& zone = Zstandard) const { return PDTime::asTIME_c(t_,0,&zone);}
    std::string GetTime17(const PDZone& zone = Zstandard) const { return PDTime::asTIME_c(t_, &zone); }
    TIME_cp getTimeC(TIME_c & t, const PDZone& zone = Zstandard) const { return asTIME_c(t, zone); }
    TIME_cp asTIME_c(TIME_c & t, const PDZone & zone) const
    { 
        const auto time17 = PDTime::asTIME_c(t_, &zone);
        strcpy_s(t, time17.c_str());
        return t; 
    }
    RWCString asTIME_c(const PDZone& z) const { return PDTime::asTIME_c(t_, &z).c_str();}
	RWCString asString(const char *fmtStr,const PDZone& z=Zstandard) const { return z.asString(t_,fmtStr);}
	void setTimeC(const TIME_c time,const PDZone& zone = Zstandard) { t_ = PDTime::aspdtime(time,&zone);}
	RWTime asRWTime() const { return ::asRWTime(t_);}
	RWDate asRWDate(const PDZone& z=Zstandard) const { return ::asRWDate(t_,&z);}
	PDDate asPDDate(const PDZone& z=Zstandard) const { return PDDate(::asRWDate(t_,&z));}
	pdtime aspdtime() const { return t_;}
    PDTime& addUnits(const long x = 1)
    {
        if (isSpecific())
            t_ += x;
        return *this;
    }
private:/*to detect use */
    void setMillisecond(const unsigned short ms) { ; }//TODO: add detection here

public:
    PDTime& addSeconds(const pdtime sec = 1)
    {
        if (isSpecific())
            t_ += sec;
        return *this;
    }
    PDTime& addMinutes(const long min = 1)
    {
        if (isSpecific())
            t_ += PDT_MIN * min;
        return *this;
    }
    PDTime& addHours(const long h = 1)
    {
        if (isSpecific())
            t_ += PDT_HOUR * h;
        return *this;
    }
    PDTime& addDays(const long d = 1)
    {
        if (isSpecific())
            t_ += PDT_DAY * d;
        return *this;
    }
    PDTime& trim(const PDTimeSpan & timestep, const PDZone& zone = Zstandard, PDTimeSpan* rest = nullptr)
    {
        const auto res = zone.trim(t_, timestep.t_);
        if (rest)
            *rest = long(t_ - res);
        t_ = res;
        return *this;
    }
    PDTime & add(const PDTimeSpan & dt, int n, const PDZone &z, bool CheckPrev = true)
    {//calendar z add n x dt
        t_ = z.add(t_, dt.t_, n, CheckPrev);
        return *this;
    }
    static PDTime now();
	PDTime& operator=(const PDTime& o) { t_=o.t_;return *this;}
	PDTime& operator=(const RWTime& o) { t_=::aspdtime(o);return *this;}
	// Litt usikker på forskjellen på disse, men jfr. RWTime ser det ut som følgende:
	// Prefix-inkrement: Endrer this og returnerer kopi av ny this.
	// Prefix-increment
	PDTime operator++()	 { return PDTime(++t_); }
	PDTime operator--()  { return PDTime(--t_); }
	// Postfix-inkrement: Tar kopi av this. Endrer this og returnerer kopien.
	// Postfix-increment
	PDTime operator++(int) { return PDTime(t_++); }
	PDTime operator--(int) { return PDTime(t_--); }
    PDTime& operator+=(const PDTimeSpan& timestep)
    {
        if (isSpecific())
            t_ += timestep.t_;
        return *this;
    }
    PDTime& operator-=(const PDTimeSpan& timestep)
    {
        if (isSpecific())
            t_ -= timestep.t_;
        return *this;
    }
    bool operator==(const PDTime& t) const { return t_==t.t_;}
	bool operator!=(const PDTime& t) const { return t_ != t.t_;}
	bool operator>(const PDTime& t) const { return t_>t.t_;}
	bool operator<(const PDTime& t) const { return t_<t.t_;}
	bool operator<=(const PDTime& x) const { return t_<=x.t_;}
	bool operator>=(const PDTime& x) const { return t_>=x.t_;}
	static unsigned hash(const PDTime& time);

	// Debug trace function.
	// Note that this function does nothing if not macro PDDEBUG is defined when compiling
	// the source file, but the declaration here in the header is visible to everyone that
	// includes it so that the macro (preprocessor directive) only have to be defined in the
	// library project containing the source file.
	void Trace(const char* Descr = "") const;

    static std::string asTIME_c(pdtime t, const PDZone * z); // 17 char YYYYMMDDhhmmssxxx
    static std::string PDTime::asTIME_c(const PDTime t, const PDZone *z);
    static pdtime txt2pdtime(
        PDZoneList & zoneList, const char *txt, const PDZone *z, pdtime Tnowref, const PDZone **zused, int * mcsec = nullptr);
    static pdtime aspdtime(const char *c, const PDZone *z);

protected:
//TODO: Add if neccessary this function TIME_cp timeC() const { return asTIME_c(t_,Zstandard); }
	friend PDTime operator+(const PDTime& t,long sec);
	friend PDTime operator+(const PDTime& t,const PDTimeSpan& step);
	friend PDTime operator-(const PDTime& t,long sec);
	friend void   rwSaveGuts(RWFile& f,const PDTime& t);
	friend void   rwSaveGuts(RWvostream& s,const PDTime& i);
	friend void   rwRestoreGuts(RWFile& f,PDTime& i);
	friend void   rwRestoreGuts(RWvistream& s,PDTime& i);
	friend PDTime operator-(const PDTime& t,const PDTimeSpan& step);
	friend PDTimeSpan operator-(const PDTime& x,const PDTime& y);
	friend pdtime aspdtime(const PDTime&t);
	friend class PDTimePeriod;
	friend class PDTimePeriodIterator;
private:
	pdtime t_;
};
inline	PDTime max(const PDTime a,const PDTime b) { return a.t_ > b.t_? a : b;}
inline	PDTime min(const PDTime a,const PDTime b) { return a.t_ < b.t_? a : b;}

inline PDTime operator+(const PDTime& t,long sec) 
	{ return PDTime(t.t_+sec); }
inline PDTime operator+(const PDTime& time,const PDTimeSpan& timestep)
	{ return PDTime(time.t_+timestep.t_); }
inline PDTime operator-(const PDTime& time,long sec)
	{ return PDTime(time.t_-sec); }
inline PDTime operator-(const PDTime& time,const PDTimeSpan& timestep)
	{ return PDTime(time.t_-timestep.t_); }
	
inline PDTimeSpan operator-(const PDTime& x,const PDTime& y) {
	return PDTimeSpan(x.t_-y.t_); }

inline pdtime aspdtime(const PDTime&t) {return t.t_;}
inline PDTime asPDTime(pdtime t) {return PDTime( t);}

inline RWDate asRWDate(pdtime t,const PDZone *z) 
{
	if (t == NOPDTIME) return RWDate((unsigned long)0);
	if(z) t += z->diffGMT(t);
	return RWDate( (unsigned long)(pdtime2j(t)));
}

inline pdtime aspdtime(const RWDate& d,const PDZone *z) 
{
	pdtime t=pdtime((long(d.julian()) - julian1970_d)*PDT_DAY);
	if(z) t -= z->diffGMT(t);
	return t;
}

inline std::string PDZone::asString(PDTime t)const { return PDTime::asTIME_c(t.aspdtime(), this);}

RWDECLARE_PERSISTABLE(PDTime);
RWDECLARE_PERSISTABLE(pdtime);

enum PDTimePeriodErrors{
	PER_NO_INTERSECTION = 10000,
	PER_NO_TANGENT,
	PER_NO_SECTION
};

enum PDTimePeriodContainsMode{
	PERIOD_INCLUSIVE_START = 1,
	PERIOD_INCLUSIVE_START_END,
	PERIOD_INCLUSIVE_END,
	PERIOD_EXCLUSIVE
};

//SIV. PDZone methods moved here to avoid "undefined class (PDTime)" error
inline pdtime PDZone::DST_at(pdtime x) const
{
    if (!PDTime(x).isSpecific())
        return 0;

    if (!dst)
        return 0;

    int y;
    macroj2Y(pdtime2j(x), y);
    y -= PDZone_dstyear_base;
    if (y < 0 || y >= PDZone_dstyears)
        return 0;

    if (dst_start[y] < dst_end[y])
        return x >= dst_start[y] && x < dst_end[y] ? dst_offset[y] : 0;
    else
        return x >= dst_start[y] || x < dst_end[y] ? dst_offset[y] : 0;
}

inline pdtime PDZone::trim(pdtime t, pdtime x) const
{
    if (!PDTime(t).isSpecific())
        return t;

    switch (x)
    {
    case 0:
    case NOPDTIME:
        return t;
    case PDT_YEAR:
        return trimYear(t);
    case PDT_MONTH:
        return trimMonth(t);
    case PDT_WEEK:
        {
            auto tj = julian1970_s + diffGMT(t) + t;
            tj = (tj / PDT_WEEK) * PDT_WEEK;
            t = tj - julian1970_s;
            t -= diffGMT(t);
            return t;
        }
        //break;
    case PDT_DAY:
        {
            auto tj = julian1970_s + diffGMT(t) + t;
            tj = (tj / PDT_DAY) * PDT_DAY;
            t = tj - julian1970_s;
            t -= diffGMT(t);
            return t;
        }
        //break;
    default:
        break;
    }

    t = ((t + julian1970_s) / x) * x - julian1970_s;
    
    return t;
}

/*@ManDoc:
Klassen definerer en tidsperiode. Tidsperioden defineres som fra og med 
start-tidspunkt og til slutt-tidspunkt. Dette fordi perioden brukes til
å definere varigheten på tidsserieverdier.  Der gjelder at en verdi for et døgn
lagret på tidspunktet 1998.07.15 (00:00:00.000) gjelder fra og med dette
tidspunktet og til 1998.07.16 (00:00:00.000).

Hvis man skulle tenkt seg å definert tidsperioden som fra og med start
og til og med slutt, hadde man fått en døgnverdi som hadde starttidspunkt
1998.07.15 (00:00:00.000) og slutt-tidspunkt 1998.07.15 (23:59:59.999).
Det vil da bli vanskeligere å kontrollere om man har en ubrutt tidsserie med
verdier fordi man da ikke kan sammenligne med likhet slutttidspunktet med
neste verdis starttidspunkt, men må sjekke i tillegg at tidsrommet mellom 
disse er <= minste tidsskritt.
*/

class PDTimePeriod {
 public:
	// Constructors 
	PDTimePeriod():_start(NOPDTIME),_end(NOPDTIME) {;}
	PDTimePeriod(const PDTimePeriod& Period):_start(Period._start),_end(Period._end) {;}
	PDTimePeriod(const PDTime& start,const PDTime& end):_start(start.t_),_end(end.t_) {;}
	PDTimePeriod(const TIME_c Start,const TIME_c End,const PDZone& zone = Zstandard);
	PDTimePeriod(const PDDate& Start, const PDDate& End,const PDZone& zone = Zstandard);
	//TODO if used: what zone ?? PDTimePeriod(int Year,bool Inclusive = false);
	PDTimePeriod(TIME_w Week,bool Inclusive = false,const PDZone& z = Zstandard);
	//TODO if used:PDTimePeriod(unsigned Year,unsigned Month,bool Inclusive = false,const PDZone& zone = Zstandard);
	PDTimePeriod(const PDDate& Date,bool Inclusive = false,const PDZone& zone = Zstandard);
	PDTimePeriod(const PDTime& Start,const PDTimeSpan& Dur,const PDZone& zone = Zstandard) 
		: _start(Start.t_), _end(zone.add(Start.t_,Dur.t_,1))
		{}

	// operators.
	PDTimePeriod& operator=(const PDTimePeriod& Period) {_start=Period._start;_end=Period._end;return *this;}
	bool operator==(const PDTimePeriod& Period) const { return _start==Period._start && _end==Period._end;}
	bool operator!=(const PDTimePeriod& Period) const { return _start!=Period._start || _end!=Period._end;}
    bool operator<(const PDTimePeriod& Period) const { return (_start < Period._start) || (_start == Period._start && _end < Period._end); }
		
	// get-methods.
	PDTime start() const { return PDTime(_start);}
	PDTime end() const   { return PDTime(_end);}
	double units(const PDTimeSpan& timestep) const { if (timestep.t_==0.0) return 0.0; return double( _end -_start)/double(timestep.t_);}
	long   totalUnits(const PDTimeSpan& timestep) const { if (timestep.t_==0.0) return 0; return long((_end -_start)/timestep.t_);}
	double units(ICC_TIME_UNITS timestep_units) const { return units(PDTimeSpan(timestep_units));}
	pdtime seconds() const { return _end - _start; }
	double minutes() const { return double( _end -_start)/double(PDT_MIN);}
	pdtime totalMinutes() const { return  (_end -_start)/PDT_MIN;}
	double hours() const { return double( _end -_start)/double(PDT_HOUR);}
	pdtime totalHours() const { return  (_end -_start)/PDT_HOUR;}
	int    getHours() const { return int((_end - _start)/PDT_HOUR);}
	double days() const { return double( _end -_start)/double(PDT_DAY);}
	pdtime totalDays() const { return  (_end -_start)/PDT_DAY;}
	double weeks() const {  return double( _end -_start)/double(PDT_WEEK);}
	pdtime totalWeeks() const  { return  (_end -_start)/PDT_WEEK;} 
	double getUnits(ICC_TIME_UNITS tUnit = SECOND) const { return units(tUnit);}
	//TODO: use totalHours int getHours() const { return (int)getUnits(HOUR,localT);}
	PDTimeSpan duration() const { return PDTimeSpan(_end - _start); }
			
	RWspace binaryStoreSize() const { return sizeof(_start)+sizeof(_end);}
	PDTimePeriod& setStart(const PDTime& Time) { _start=Time.t_;return *this;}
	PDTimePeriod& setEnd(const PDTime& Time) { _end=Time.t_;return *this;}
	//TODO if used:void setStart(const TIME_c Start,const RWZone& zone = RWZone::standard());
	//TODO if used:void setStart(const PDDate& Start,const RWZone& zone = RWZone::standard());
	//TODO if used:void setEnd(const TIME_c End,const RWZone& zone = RWZone::standard());
	//TODO if used:void setEnd(const PDDate& End,const RWZone& zone = RWZone::standard());
	void split(const PDTime& x,PDTimePeriod& Lower,PDTimePeriod& Upper) const {
		if(x.t_ > _start && x.t_ < _end ) {
			Lower._start=_start;Lower._end=x.t_;
			Upper._start=x.t_;Upper._end=_end;
		}
	}
	//TODO if used:void split(const TIME_c Time,PDTimePeriod& Lower,PDTimePeriod& Upper) const;
	void split(const TIME_c time,PDTimePeriod* const& sPer) const
		{ split(time,sPer[0],sPer[1]); }
    bool contains(const pdtime t, PDTimePeriodContainsMode mode = PERIOD_INCLUSIVE_START) const 
    {
        switch (mode) 
        {
            case PERIOD_INCLUSIVE_START: return t >= _start && t < _end;
            case PERIOD_INCLUSIVE_START_END: return t >= _start && t <= _end;
            case PERIOD_INCLUSIVE_END: return t > _start && t <= _end;
            case PERIOD_EXCLUSIVE: return t > _start && t < _end;
        }
        return false;
    }

    bool contains(const PDTime x,PDTimePeriodContainsMode mode = PERIOD_INCLUSIVE_START) const 
    {
		return contains(x.t_);
	}

    bool contains(PDTimePeriod const & p) const { return isOk() && p.isOk() && p._start >= _start && p._end <= _end;}
	bool intersects(const PDTimePeriod& p) const { return  isOk() && p.isOk() && p._end > _start && p._start < _end; }
	bool touches(const PDTimePeriod& p) const { return _start == p._start || _start == p._end || _end == p._start|| _end == p._end;}
	PDTimePeriod getSection(const PDTimePeriod& p) const 
    { 
		PDTimePeriod res;
		if(!p.isOk()) 
			return res;
		if( !isOk())
			return res;
        if (p._start >= _end || p._end <= _start) 
            return res;	// Outside
        res._start = p._start > _start ? p._start : _start; // max(p._start, _start)
        res._end = p._end < _end ? p._end : _end; // min(p._end, _end)
        return res;
    }
	PDTimePeriod getUnion(const PDTimePeriod& p) const {
		if(!p.isOk()) 
			return *this;
		if( !isOk())
			return p;
		PDTimePeriod res;
		if(intersects(p) || touches(p)) {
			res._start= p._start < _start?p._start:_start;// min(p._start,_start)
			res._end  = p._end   > _end  ?p._end  :_end;// max(p._end,_end)
		} // else PER_NO_INTERSECTION error, but is that a reasonable constraint
		return res;
	}
	
    PDTimePeriod& add(const PDTimePeriod& p) 
    {
		//TODO: Old code did some checking for intersects, but that is dropped here.
		// in effect this method results the 'big' union of this and p
		if (p.isOk()) 
        {
			if (_start == NOPDTIME || p._start < _start) 
                _start = p._start;
			if (_end == NOPDTIME || p._end > _end) 
                _end = p._end;
		}
		return *this;
	}
	
    PDTimePeriod& subtract(const PDTimePeriod& p) 
    {
		//TODO: ref comment above
		// this impl. returns the section of p and this;
		if (_start == NOPDTIME || p._start > _start) 
            _start = p._start;
		if (_end == NOPDTIME || p._end < _end) 
            _end  = p._end;
		return *this;
	}

	std::string GetStart(const PDZone& zone = Zstandard) const;
    std::string GetEnd(const PDZone& zone = Zstandard) const;
	
	RWCString asString(const char *FmtS,const char *FmtE,const PDZone& Z = Zlocal) const;
	RWCString asString(const char* Fmt, const PDZone& Z = Zlocal) const { return asString(Fmt, Fmt, Z); }
	RWCString asTimeC(const PDZone& Z = Zstandard) const {
		return PDTime(_start).asTIME_c(Z) + "-" + PDTime(_end).asTIME_c(Z); }

	bool isOk() const 
    { 
        //zero-length period is valid:
		return _start != NOPDTIME && _end != NOPDTIME && _start <= _end;
	};
	
	bool isBad() const { return !isOk();}
    //SIV. THROW_IF_NOT_SPECIFIC_PERIOD controls how partially specified periods are handled.
    //If it is defined and one of period boundaries is not logically specified wile the other one is,
    //PDTimePeriod::isSpecific() throws
//#define THROW_IF_NOT_SPECIFIC_PERIOD
    bool isSpecific() const;

	// Debug trace function.
	// Note that this function does nothing if not macro PDDEBUG is defined when compiling
	// the source file, but the declaration here in the header is visible to everyone that
	// includes it so that the macro (preprocessor directive) only have to be defined in the
	// library project containing the source file.
	void Trace(const char* Descr = "") const;
    static PDTimePeriod TrimPeriodToResolutionAndTimeZone(PDTimePeriod period, PDTimeSpan dT, const PDZone & tz);
    static void Union(std::set<PDTimePeriod> & periodSet, const PDTimePeriod & period);
    static std::set<PDTimePeriod> Subtract(const PDTimePeriod & period, std::set<PDTimePeriod> & periodSet);
    static std::set<PDTimePeriod> Subtract(const PDTimePeriod & period1, const PDTimePeriod & period2)
    {
        std::set<PDTimePeriod> periodSet;
        periodSet.insert(period2);
        return Subtract(period1, periodSet);
    }
    static void Subtract(std::set<PDTimePeriod> & periodSet, const PDTimePeriod & period);
    static bool Contains(const std::set<PDTimePeriod> & periodSet, const PDTimePeriod & period);

 protected:

 private:
	pdtime _start;
	pdtime _end;
	friend void rwSaveGuts(RWFile& f,const PDTimePeriod& p);
	friend void rwSaveGuts(RWvostream& s,const PDTimePeriod& p);
	friend void rwRestoreGuts(RWFile& f,PDTimePeriod& p);
	friend void rwRestoreGuts(RWvistream& s,PDTimePeriod& p);
	friend class PDTimePeriodIterator;
};

// Persistence
RWDECLARE_PERSISTABLE(PDTimePeriod);
inline PDTime asPDTime(DATE x,const PDZone *z=Zdate_) { /*
    Convert DATE to PDTime using timezone z.
    default use Zdate_, if z zero, use
    */
	return PDTime(aspdtime(x,z));
}
inline DATE asDATE(PDTime x,const PDZone *z=Zdate_ ) { /*
  Convert PDTime x to DATE using PDZone z */
	return asDATE(x.aspdtime(),z);
}
	
std::ostream& operator<<(std::ostream & os, const PDTime & t);

std::ostream & operator<<(std::ostream & os, const PDTimePeriod & p);

std::ostream & operator<<(std::ostream & os, const PDTimeSpan & dt);

#endif /* __cplusplus */

#endif /* _PDTime_h_ */
