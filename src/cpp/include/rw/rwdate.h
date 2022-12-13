#ifndef __RWDATE_H__
#define __RWDATE_H__

/*
 * Declarations for RWDate Class
 *
 * $Id: rwdate.h 31098 2003-01-14 11:28:57Z sih $
 *
 * Copyright (c) 1989-1999 Rogue Wave Software, Inc.  All Rights Reserved.
 *
 * This computer software is owned by Rogue Wave Software, Inc. and is
 * protected by U.S. copyright laws and other laws and by international
 * treaties.  This computer software is furnished by Rogue Wave Software,
 * Inc. pursuant to a written license agreement and may be used, copied,
 * transmitted, and stored only in accordance with the terms of such
 * license and with the inclusion of the above copyright notice.  This
 * computer software or any other copies thereof may not be provided or
 * otherwise made available to any other person.
 *
 * U.S. Government Restricted Rights.  This computer software is provided
 * with Restricted Rights.  Use, duplication, or disclosure by the
 * Government is subject to restrictions as set forth in subparagraph (c)
 * (1) (ii) of The Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013 or subparagraphs (c) (1) and (2) of the
 * Commercial Computer Software – Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 ***************************************************************************
 *
 * An RWDate can be tested for validity using the member function isValid().
 *
 ***************************************************************************
 */

#include "rw/tooldefs.h"
#include "rw/zone.h"
#include "rw/locale.h"

//2437  C++ Standard operators >=, <=, >, and !=
#if !defined(RW_NO_STL) && defined(RW_NO_RELOPS_NAMESPACE)
#  ifndef RW_HEADER_WITH_EXTENSION
#    include<utility>
#  else
#    include<utility.hpp>
#  endif  /* RW_HEADER_WITH_EXTENSION */
#endif  /* RW_NO_STL */

#if defined(min)
# undef min
# undef max
#endif

class RWExport  RWTime;
class RWSExport RWCString;
class RWExport  RWLocale;

const unsigned long rwJul1901 = 2415386L;      // Julian day for 1/1/1901

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWDate
{

public:

  RWDate() : julnum(RWDate::currentDate()) {;} // today's date
  RWDate(unsigned long j) : julnum(j) {;} // Julian day; beware type conversion

  /* Construct a RWDate with a given day of the year and a given year.
     The base date for this computation is Dec. 31 of the previous year.
     If year == 0, Construct a RWDate with Jan. 1, 1901 as the "day zero".
        i.e., RWDate(-1) = Dec. 31, 1900 and RWDate(1) = Jan. 2, 1901. */
  RWDate(unsigned day, unsigned year);

  //  Construct a RWDate for the given day, month, and year.
  RWDate(unsigned, const char* month, unsigned year,
         const RWLocale& = RWLocale::global()); // for month names
  RWDate(unsigned d, unsigned m, unsigned);
  RWDate(RW_SL_IO_STD(istream)& s, const RWLocale& l = RWLocale::global())
                        {parseFrom(s, l);}  // Read date from stream.
  RWDate(const RWCString& str, const RWLocale& = RWLocale::global());
  RWDate(const RWTime&, const RWZone& = RWZone::local());
  RWDate(const struct tm*);  // from members: tm_year, tm_mon, tm_mday

  // The following two should not be necessary, but are required to
  // support feeble compilers:
  RWDate(const RWDate& d) : julnum(d.julnum) {;}
  RWDate& operator=(const RWDate& d) {julnum=d.julnum; return *this;}

  RWCString             asString(char format = 'x',
                                 const RWLocale& = RWLocale::global()) const;
  RWCString             asString(const char *format,
                                 const RWLocale& = RWLocale::global()) const;
  RWBoolean             between(const RWDate& d1, const RWDate& d2) const
                        { if(d1 <= d2)
						    return julnum >= d1.julnum && julnum <= d2.julnum;
						  else
						    return julnum >= d2.julnum && julnum <= d1.julnum;
						}	
  int                   compareTo(const RWDate*) const;
  unsigned              day() const;            // 1-365
  unsigned              dayOfMonth() const;     // 1-31
  void                  extract(struct tm*) const; // set all tm members
  unsigned              firstDayOfMonth(unsigned mon) const;
  unsigned              firstDayOfMonth() const
                           { return firstDayOfMonth(month());}
  unsigned              hash() const;
  RWBoolean             isValid() const { return julnum>0; }
  unsigned long         julian()  const { return julnum;   }
  void                  julian(const unsigned long j) { julnum = j; }
  RWBoolean             leap() const
#if !(defined(macintosh) && defined(RW_PRAGMA_EXPORT))
                        { return leapYear(year());}  // leap year?
#else
  ;
#endif  
  RWDate                max(const RWDate& dt) const;
  RWDate                min(const RWDate& dt) const;
  unsigned              month() const;
  RWCString             monthName(const RWLocale& =RWLocale::global()) const;
                        // Return date of previous dayName:
  RWDate                next(const char* dayName,
                                 const RWLocale& =RWLocale::global()) const;
                        // Same as above, but use numeric day of week
  RWDate                next(unsigned day) const;                
  RWDate                previous(const char* dayName,
                                 const RWLocale& =RWLocale::global()) const;
                        // Same as above, but use numeric day of week
  RWDate                previous(unsigned day) const;            
  unsigned              weekDay() const;
  RWCString             weekDayName(const RWLocale& =RWLocale::global()) const;
  unsigned              year() const;

  // Arithmetic operators:
  RWDate&               operator++()           { ++julnum; return *this; }
  RWDate&               operator--()           { --julnum; return *this; }
#ifndef RW_NO_POSTFIX
  RWDate                operator++(int)        { return RWDate(julnum++); }
  RWDate                operator--(int)        { return RWDate(julnum--); }
#endif
  RWDate&               operator+=(unsigned long dd)
        { julnum += dd; return *this; }
  RWDate&               operator-=(unsigned long dd)
        { julnum -= dd; return *this; }

  // Read or write dates:
#ifndef RW_TRAILING_RWEXPORT
  friend rwexport RW_SL_IO_STD(ostream)& operator<<(RW_SL_IO_STD(ostream)&, const RWDate&);
  friend rwexport RW_SL_IO_STD(istream)& operator>>(RW_SL_IO_STD(istream)& s, RWDate& d);
#else
  friend RW_SL_IO_STD(ostream)& rwexport operator<<(RW_SL_IO_STD(ostream)&, const RWDate&);
  friend RW_SL_IO_STD(istream)& rwexport operator>>(RW_SL_IO_STD(istream)& s, RWDate& d);
#endif
  RWspace               binaryStoreSize() const {return sizeof(julnum);}
  void                  restoreFrom(RWvistream& s);
  void                  restoreFrom(RWFile&);
  void                  saveOn(RWvostream& s) const;
  void                  saveOn(RWFile&) const;

  // Global friends:
friend RWDate           operator+(const RWDate& dt, unsigned long dd)
                                { return RWDate(dt.julnum + dd); }
friend RWDate           operator+(unsigned long dd, const RWDate& dt)
                                { return RWDate(dt.julnum + dd); }
friend RWDate           operator-(const RWDate& dt, unsigned long dd)
                                { return RWDate(dt.julnum - dd); }
friend unsigned long          operator-(const RWDate& d1, const RWDate& d2)
                                { return d1.julnum - d2.julnum; }
friend RWBoolean        operator<(const RWDate& d1, const RWDate& d2)
                                { return d1.julnum < d2.julnum; }
friend RWBoolean        operator==(const RWDate& d1, const RWDate& d2)
                                { return d1.julnum == d2.julnum; }

#if defined(RW_NO_STL) || !defined(RW_NO_RELOPS_NAMESPACE) || defined(__PowerCpp__)
friend RWBoolean        operator<=(const RWDate& d1, const RWDate& d2)
                                { return d1.julnum <= d2.julnum; }
friend RWBoolean        operator>(const RWDate& d1, const RWDate& d2)
                                { return d1.julnum > d2.julnum; }
friend RWBoolean        operator>=(const RWDate& d1, const RWDate& d2)
                                { return d1.julnum >= d2.julnum; }
friend RWBoolean        operator!=(const RWDate& d1, const RWDate& d2)
                                { return d1.julnum != d2.julnum; }
#endif


  // Static member functions:
  static  void        clobber(struct tm*);
  static  unsigned    dayOfWeek(const char* dayName,
                                const RWLocale& = RWLocale::global());
  static  unsigned    daysInYear(unsigned year);
  static  unsigned    daysInMonthYear(unsigned month, unsigned year);
  static  RWBoolean   dayWithinMonth(unsigned mon, unsigned day, unsigned yr);
   
  // useful for supplying hash functions to template hash collection ctors:  
  static  unsigned      hash(const RWDate&);
  
  static  unsigned      indexOfMonth(const char* monName,
                                     const RWLocale& = RWLocale::global());
  static  unsigned long jday(unsigned mon, unsigned day, unsigned year);
  static  RWBoolean     leapYear(unsigned year);
  static  RWCString     nameOfMonth(unsigned month,
                                    const RWLocale& =RWLocale::global());
  static  RWDate        now();                  // return today's date.
  static  RWCString     weekDayName(unsigned day,
                                    const RWLocale& =RWLocale::global());

  
  // static const data
  static const unsigned char daysInMonth[12];          // [1..31]
  static const unsigned      firstDayOfEachMonth[12];  // [0..334]

  // For functions that returned char*, use RWLocale facilities for new code:
  // for member nameOfDay()
  //    use member weekDayName(const RWLocale&)
  // for member nameOfMonth()
  //    use member monthName(const RLocale&)
  // for static member dayName(unsigned)
  //    use static member weekDayName(unsigned, const RWLocale&)
  // for static member monthName(unsigned)
  //    use static member nameOfMonth(unsigned, const RWLocale&)

protected:

  static RWBoolean      assertWeekDayNumber(unsigned d)
                        {return d>=1 && d<=7;}
  static RWBoolean      assertIndexOfMonth(unsigned m)
                        {return m>=1 && m<=12;}
private:
  friend class RWExport RWTime;
  void                  parseFrom(RW_SL_IO_STD(istream)&, const RWLocale&);
  void rwnear           mdy(unsigned& m, unsigned& d, unsigned& y) const;
  static  unsigned long currentDate();
  unsigned long         julnum; // Julian Day Number (not same as Julian date).

  RW_TYPESPEC  /* This macro usually expands to nothing */

};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

// inline members
inline RWCString RWDate::monthName(const RWLocale& locale) const
                { return nameOfMonth(month(), locale); }
inline RWCString RWDate::weekDayName(const RWLocale& locale) const
                { return weekDayName(weekDay(), locale); }

// inline statics

// Return the number, [1..7], of the day of the week named, or 0 for failure
inline unsigned RWDate::dayOfWeek(const char* dayName, const RWLocale& locale)
                { return (unsigned) locale.weekdayIndex(RWCString(dayName)); }

// Return the number, [1..12], of the month named, or 0 for failure
inline unsigned RWDate::indexOfMonth(const char* mon, const RWLocale& locale)
                { return (unsigned) locale.monthIndex(RWCString(mon)); }

// inline globals

inline RWvistream& operator>>(RWvistream& str,       RWDate& date)
                { date.restoreFrom(str);  return str;  }
inline RWFile&     operator>>(RWFile& file,          RWDate& date)
                { date.restoreFrom(file); return file; }
inline RWvistream& operator>>(RWvistream& str,       RWDate*& date)
                { date = new RWDate; date->restoreFrom(str);  return str;  }
inline RWFile&     operator>>(RWFile& file,          RWDate*& date)
                { date = new RWDate; date->restoreFrom(file); return file; }
inline RWvostream& operator<<(RWvostream& str, const RWDate& date)
                { date.saveOn(str);       return str;  }
inline RWFile&     operator<<(RWFile& file,    const RWDate& date)
                { date.saveOn(file);      return file; }

#ifdef __OS2__
#  include<stdlib.h>     //Restore definitions of min and max
#endif

#endif   /* __RWDATE_H__ */
