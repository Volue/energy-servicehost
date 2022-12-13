#ifndef __RWLOCALE_H__
#define __RWLOCALE_H__

/*
 * Declarations for the RWLocale class
 *
 * $Id: locale.h 18623 2000-03-03 08:28:13Z sih $
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
 * Classes RWLocale and RWLocaleSnapshot support encapsulation of local
 * formatting conventions.  RWLocaleDefault provides the same interface for
 * access to the global process state "non-encapsulation" of the same
 * functions.
 *
 ***************************************************************************
 */

#ifndef __RWCOMPILER_H__
#include "rw/compiler.h"        /* for STARTWRAP */
#endif
STARTWRAP
#if !defined(RW_NO_LOCALE)
#include <locale.h>
#endif
#include <time.h>               /* for struct tm and time_t */
ENDWRAP
#include "rw/defs.h"            /* for RWExport */
#include "rw/cstring.h"         /* for RWCString */
#include "rw/zone.h"            /* for RWZone::local() */

struct RWTimeFmtList;    // internal type
class  RWExport RWLocaleDefault;

// RWLocale is an abstract base class defining an interface to
//  locale-dependent programming facilities.

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWLocale {
public:
  virtual ~RWLocale();
  
  virtual RWCString asString(long) const = 0;
  virtual RWCString asString(unsigned long) const = 0;
  virtual RWCString asString(
      double f, int precision = 6, RWBoolean showpoint = 0) const = 0;
  
  // asString(tm*, format): format is one of [aAbBcdHIjmMpSUwWxXyYZ],
  //                        as in ANSI C Library function strftime().
  virtual RWCString asString(const struct tm* tmbuf, char format,
                             const RWZone& = RWZone::local()) const = 0;
  
  // convenience function uses above. Not virtual but still polymorhpic:
  RWCString asString(const struct tm* tmbuf, const char *format,
                     const RWZone& = RWZone::local()) const;
  
  enum CurrSymbol { NONE, LOCAL, INTL };
  
#ifdef RW_GLOBAL_ENUMS
  virtual RWCString moneyAsString(double,
                                  CurrSymbol=LOCAL) const = 0;
#else
  virtual RWCString moneyAsString(double,
                                  RWLocale::CurrSymbol=LOCAL) const = 0;
#endif
  
  virtual RWBoolean stringToNum  (const RWCString&, double*) const = 0;
  virtual RWBoolean stringToNum  (const RWCString&, long*) const = 0;
  virtual RWBoolean stringToDate (const RWCString&, struct tm*) const = 0;
  virtual RWBoolean stringToTime (const RWCString&, struct tm*) const = 0;
  
#ifdef RW_GLOBAL_ENUMS
  virtual RWBoolean stringToMoney(const RWCString&, double*,
                                  CurrSymbol=LOCAL) const = 0;
#else
  virtual RWBoolean stringToMoney(const RWCString&, double*,
                                  RWLocale::CurrSymbol=LOCAL) const = 0;
#endif
  
  // returns [1..12] (1 for January), 0 for error
  virtual int monthIndex(const RWCString&) const = 0;
  // returns 1 for Monday equivalent, 7 for Sunday, 0 for error.
  virtual int weekdayIndex(const RWCString&) const = 0;
  
  static const RWLocale& global();  // the default locale for most functions
  static const RWLocale* global(const RWLocale*);  // a function to set it.
  static const RWLocale* defaultLocale(); // get a "C" locale pointer.
  static void releaseCache();
  
#ifndef RW_IOS_XALLOC_BROKEN
  
  const RWLocale*   imbue(RW_SL_IO_STD(ios)&) const;  // assign this locale to a stream
  static const RWLocale*  unimbue(RW_SL_IO_STD(ios)&);  // remove the locale from a stream
  
#endif /* RW_IOS_XALLOC_BROKEN */
  
  static const RWLocale&  of(RW_SL_IO_STD(ios)&);     // retrieve imbued locale, if any
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

// RWLocaleDefault is used when you do not want an encapsulated locale
//  object, but need an RWLocale for the interface.  There is only one
//  instance of RWLocaleDefault, initially returned by RWLocale::global(),
//  used as the default value for RWLocale& arguments.

class RWExport RWLocaleDefault : public RWLocale {
private:
friend const RWLocale* RWLocale::defaultLocale();
  RWLocaleDefault();
public:
  
  virtual ~RWLocaleDefault();
  
  RWCString asString(const struct tm* tmbuf, const char *format,
                     const RWZone& r = RWZone::local()) const 
                     { return RWLocale::asString(tmbuf,format,r); }
  virtual RWCString asString(long) const;
  virtual RWCString asString(unsigned long) const;
  virtual RWCString asString(double f, int precision = 6,
                             RWBoolean showpoint = 0) const;
  virtual RWCString asString(const struct tm* tmbuf, char format,
                             const RWZone& = RWZone::local()) const;
  
#ifdef RW_GLOBAL_ENUMS
  virtual RWCString moneyAsString(double, CurrSymbol=LOCAL) const;
#else
  virtual RWCString moneyAsString(double, RWLocale::CurrSymbol=LOCAL) const;
#endif
  
  virtual RWBoolean stringToNum  (const RWCString&, double*) const;
  virtual RWBoolean stringToNum  (const RWCString&, long*) const;
  virtual RWBoolean stringToDate (const RWCString&, struct tm*) const;
  virtual RWBoolean stringToTime (const RWCString&, struct tm*) const;
  
#ifdef RW_GLOBAL_ENUMS
  virtual RWBoolean stringToMoney(const RWCString&, double*,
                                  CurrSymbol=LOCAL) const;
#else
  virtual RWBoolean stringToMoney(const RWCString&, double*,
                                  RWLocale::CurrSymbol=LOCAL) const;
#endif
  
  // monthIndex() returns [1..12], 0 for error
  virtual int monthIndex(const RWCString&) const;
  
  // weekdayIndex() returns 1 for Monday, 7 for Sunday, 0 for error.
  virtual int weekdayIndex(const RWCString&) const;
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

// RWLocaleSnapshot is the real working locale object.  When constructed
// it copies all necessary data from the environment into itself, for use
// later by its member functions.

class RWExport RWLocaleSnapshot : public RWLocale {
public:
  RWCString locale_name_;  // Name of the locale "snapped"
  
  // lconv members
  RWCString decimal_point_;
  RWCString thousands_sep_;
  RWCString grouping_;
  RWCString int_curr_symbol_;
  RWCString currency_symbol_;
  RWCString mon_decimal_point_;
  RWCString mon_thousands_sep_;
  RWCString mon_grouping_;
  RWCString positive_sign_;
  RWCString negative_sign_;
  char int_frac_digits_;
  char frac_digits_;
  char p_cs_precedes_;
  char p_sep_by_space_;
  char n_cs_precedes_;
  char n_sep_by_space_;
  char p_sign_posn_;
  char n_sign_posn_;
  
private:
  RWCString weekDays_[7];     // names of days of the week
  RWCString weekAbbs_[7];     // abbreviated week days
  RWCString monthNames_[12];  // names of months
  RWCString monthAbbrs_[12];  // abbreviated months
  RWCString am_;
  RWCString pm_;
  int monthNameHasDigits_;    // aargh!
  
  RWCString      timeFmt_;
  RWTimeFmtList* timeFmtList_;
  RWCString      dateFmt_;
  RWTimeFmtList* dateFmtList_;
  RWCString      dateTimeFmt_;
  RWTimeFmtList* dateTimeFmtList_;
#ifndef RW_NO_STRFTIME_CAPC
  RWCString      dateLongFmt_;
  RWTimeFmtList* dateLongFmtList_;
#endif
  const char* positiveMoneyFormat_;
  const char* negativeMoneyFormat_;
#ifdef RW_OBJECTSTORE_HETERO
public:
  char _os_pad_0[4];   /* New */
#endif
private:
  double localDivisor_;
  double intlDivisor_;
  
public:
  enum RWDateOrder { DMY, MDY, YDM, YMD RWOS_HETERO_ENUM_PAD};
  RWDateOrder dateOrder_;     // to help parse input
#ifdef RW_OBJECTSTORE_HETERO
  char _os_pad_1[4];   /* New */
#endif

private:
  // number output formatting
  int fmt(char** bufpp, long) const;
  int fmt(char** bufpp, unsigned long) const;
  int fmt(char** bufpp, double, int precision, int showdot,
          const RWCString& dec_point, const RWCString& thou_sep,
            const RWCString& grouping) const;
  
  // date output formatting
  int formatComponent(char*, const struct tm*, int, const RWZone&) const;
  int fillFormat(char*, const char*, RWTimeFmtList*,
                 const struct tm*, const RWZone&)           const;
  int fmt(char* buf, const struct tm*, char format, const RWZone&) const;
  
  // money output formatting
  size_t fmt(char** buf, double cents, const RWCString& curr_sym,
             const char* format, const RWCString& sign, int sep_by_space,
             int frac_digits, double divisor) const ;
  
public:
  RWLocaleSnapshot(const char* localeName = 0); // if 0, snap current locale
  virtual ~RWLocaleSnapshot();
  
  RWCString asString(const struct tm* tmbuf, const char *format,
                     const RWZone& r = RWZone::local()) const 
                     { return RWLocale::asString(tmbuf,format,r); }
  virtual RWCString asString(long) const;
  virtual RWCString asString(unsigned long) const;
  virtual RWCString asString(
      double f, int precision = 6, RWBoolean showpoint = 0) const;
  
  // asString(tm*, format): format is one of [aAbBcdHIjmMpSUwWxXyYZ],
  //                        as in ANSI C Library function strftime().
  virtual RWCString asString(const struct tm* tmbuf, char format,
                             const RWZone& = RWZone::local()) const;
  
#ifdef RW_GLOBAL_ENUMS
  virtual RWCString moneyAsString(double, CurrSymbol=LOCAL) const;
#else
  virtual RWCString moneyAsString(double, RWLocale::CurrSymbol=LOCAL) const;
#endif

  virtual RWBoolean stringToDate (const RWCString&, struct tm*) const;
  virtual RWBoolean stringToTime (const RWCString&, struct tm*) const;
  virtual RWBoolean stringToNum  (const RWCString&, double*) const;
  virtual RWBoolean stringToNum  (const RWCString&, long*) const;
  
#ifdef RW_GLOBAL_ENUMS
  virtual RWBoolean stringToMoney(const RWCString&, double*,
                                  CurrSymbol=LOCAL) const;
#else
  virtual RWBoolean stringToMoney(const RWCString&, double*,
                                  RWLocale::CurrSymbol=LOCAL) const;
#endif
  
  // returns [1..12], 0 for error
  virtual int monthIndex(const RWCString&) const;
  // returns 1 for Monday, 7 for Sunday, 0 for error.
  virtual int weekdayIndex(const RWCString&) const;
  
friend class RWLocaleStream;
private:
  // These functions migrated to the end of the class
  // because MetaWare 3.2 beta for OS/2 will not generate
  // the vtable when they are at the top...
  RWLocaleSnapshot(RWLocaleSnapshot&);  // not defined
  void operator= (RWLocaleSnapshot&);   // not defined
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /*__RWLOCALE_H__*/
