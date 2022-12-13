#ifndef _PDDate_h_
#define _PDDate_h_

#include "time_api.h"

#include <rw/rwdate.h>
#include <rw/tvsrtvec.h>

#ifndef PDDATE_YMD 
#define PDDATE_YMD "%02D.%02M.%Y"
#endif
class PDTime;
class PDZone;
extern PDZone *Zlocal_;
class PDDate;
typedef RWTValSortedVector<PDDate> PDSortedDateArray;

class PDDate : public RWDate
{
public:
	PDDate();
	PDDate(const PDDate& d) : RWDate(d) {}
	PDDate(const TIME_c);
	PDDate(TIME_w);
	PDDate(unsigned d,unsigned m,unsigned y);
	PDDate(RWDate const & d) : RWDate(d){}
	explicit PDDate(const PDTime& t,const PDZone &z=*Zlocal_);
	PDDate& operator=(const PDDate& t);
	PDDate& operator=(const TIME_c t);
	PDDate& operator=(RWDate const & d)
		{ RWDate::operator=(d); return *this;}
	void setMonthDay(const char * str);
	const char* getDateC() const;
	TIME_cp getTimeC() const;
	TIME_cp getTimeC(TIME_c& tid) const;
	void getTimeW(TIME_w& week) const;
	unsigned week(int *relYear = rwnil) const;
	unsigned int yearweek() const
		{ int rel = 0; return year()*100 + week(&rel) + rel*100; }
	RWCString getString(const char * format=PDDATE_YMD) const;
	operator TIME_cp() const 
		{ return getTimeC(); }
	unsigned hash() const
		{ return RWDate::hash(); }
	static unsigned weeksInYear(unsigned year);
	static void tm2c(const struct tm& tms,TIME_c& c);
	static TIME_cp w2c(const TIME_w week);
	static unsigned hash(const PDDate& D);

	// Debug trace function.
	// Note that this function does nothing if not macro PDDEBUG is defined when compiling
	// the source file, but the declaration here in the header is visible to everyone that
	// includes it so that the macro (preprocessor directive) only have to be defined in the
	// library project containing the source file.
	void Trace(const char* Desc = "") const;
};

#endif	/* _PDDate_h_ */
