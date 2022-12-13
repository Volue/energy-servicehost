#ifndef __RWZONE_H__
#define __RWZONE_H__ 1

/*
 * Declarations for class RWTime.
 *
 * $Id: zone.h 18623 2000-03-03 08:28:13Z sih $
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
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.8  1996/07/18 03:57:35  hart
 * ObjectStore changes
 *
 * Revision 7.7  1996/05/07 17:38:30  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.6  1996/04/23 00:30:12  griswolf
 * Scopus #2426: Fix problem with int valued zone offset.
 *
 * Revision 7.5  1996/02/18 01:50:58  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.4  1996/01/18 22:53:19  jims
 * Add system() time zone setter and getter (#923)
 *
 * Revision 7.3  1995/10/11 20:25:21  griswolf
 * Scopus #2193: provide for clearing cached values to hush Purify.
 *
 * Revision 7.3  1995/10/11  20:09:50  griswolf
 * Scopus #2193: provide for clearing cached values to hush Purify.
 *
 * Revision 7.2  1995/09/05  21:37:30  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:13:47  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:48:57  vriezen
 * Move all files to 6.1
 *
 * Revision 1.19  1993/09/13  01:38:43  keffer
 * Prefixed #include wrapper with 'RW'
 *
 * Revision 1.18  1993/09/10  17:20:46  jims
 * Add RWExport to RWZoneSimple class
 *
 * Revision 1.17  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.16  1993/08/06  17:46:02  keffer
 * Changed name ZoneDefault to ZoneSimple for clarity.
 *
 * Revision 1.14  1993/07/03  02:19:31  myersn
 * allow support for mainland China DST rules (April 12 - September 12).
 *
 * Revision 1.13  1993/04/13  08:38:33  myersn
 * add RWZoneSimple(StdZone, DstRule) constructor.
 *
 * Revision 1.12  1993/04/12  21:53:49  myersn
 * change names of some time zones in StdZone.
 *
 * Revision 1.11  1993/04/10  19:52:48  keffer
 * Changed argument name tzname to tz_name to avoid ambiguity with
 * global variable of the same name.
 *
 * Revision 1.10  1993/04/09  23:17:57  myersn
 * generalize and simplify RWZoneSimple interface.
 *
 * Revision 1.9  1993/03/25  06:01:13  myersn
 * eliminate RWDaylightRule::mustFree_ member.
 *
 * Revision 1.8  1993/03/12  02:21:44  myersn
 * make usRule DST rules available to users defining zones.
 *
 * Revision 1.7  1993/02/17  03:10:23  keffer
 * Changed const notation to follow style guide
 *
 * Revision 1.6  1993/01/18  22:56:23  myersn
 * add observed_ field to RWDaylightRule to allow no-DST years
 *
 * Revision 1.4  1992/12/16  00:53:52  myersn
 * improvements to RWZoneSimple interface
 *
 * Revision 1.3  1992/12/04  05:03:59  myersn
 * make RWLocale::global() and RWZone::local() more flexible.
 *
 * Revision 1.2  1992/11/27  21:05:46  myersn
 * build up separate RWZone class for time zone & daylight encapsulation.
 *
 * Revision 1.1  1992/11/20  02:58:36  myersn
 * Initial revision
 */

#include "rw/compiler.h"
#include "rw/defs.h"
STARTWRAP
#include <time.h>                /* System time management. */
ENDWRAP
#include "rw/cstring.h"
   
#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

// Uncomment the following line if you need a link-incompatible
// fix for int-valued timeZoneOffset() or altZoneOffset()
/* #define RW_USE_LONG_ZONE_OFFSET=1 */

struct RWExport RWDaylightBoundary {
  // this struct uses <time.h> struct tm conventions:
  int month_;    // [0..11]
  int week_;     // [0..4], 4=last week in month [or, -1=use month day]
  int weekday_;  // [0..6], 0=Sunday             [or, 1..31 if week_==-1]
  int minute_;   // [0..1439]  (Usually 2 AM, = 120)

  void date(struct tm*) const;
  RWBoolean after(const struct tm*) const;
};
  
struct RWExport RWDaylightRule {
  RWDaylightRule const* next_;
  short firstYear_;
  char observed_;
  RWDaylightBoundary begin_;   // begin may be after end
  RWDaylightBoundary end_;

  RWDaylightRule& operator= (const RWDaylightRule&);
//  RWDaylightRule(const RWDaylightRule&);  // do not declare constructors.
  RWBoolean isDaylight(const struct tm*) const;
};

class RWExport RWZone {
 public:
  virtual ~RWZone();

#if defined(RW_USE_LONG_ZONE_OFFSET)
  virtual long      timeZoneOffset  () const = 0;   // seconds west of UTC
  virtual long      altZoneOffset   () const = 0;
#else
  virtual int       timeZoneOffset  () const = 0;   // seconds west of UTC
  virtual int       altZoneOffset   () const = 0;
#endif
  virtual RWBoolean daylightObserved() const = 0;
  virtual RWCString timeZoneName    () const = 0;
  virtual RWCString altZoneName     () const = 0;

  // isDaylight() may use all members of tmbuf except tm_isdst
  virtual RWBoolean isDaylight(const struct tm* tmbuf) const = 0;

  // The following functions use the tm_year member of struct tm, and
  //   set all the remaining members.
  virtual void getBeginDaylight(struct tm*) const = 0;
  virtual void getEndDaylight(struct tm*) const = 0;

  static const RWZone& local();  
  static const RWZone& standard();
  static const RWZone& utc();
  static const RWZone& os(); //get current operating system dst rules
  static const RWZone* local(const RWZone*);     // set local()
  static const RWZone* standard(const RWZone*);  // set standard()
  
  /*
   * system() is used internally for platforms that do not provide
   * an ANSI time() function that correctly returns the current
   * time in UTC. (At present, none of the certified platforms
   * fall into this category). By default it is set just as is the
   * initial default value of local(). The system() zone, if needed,
   * should correctly reflect the time zone represented by the system
   * clock in the machine.  If you need to set system(), you should do so
   * at the beginning of your program.  It should not vary during execution.
   */
  static const RWZone& system();
  static const RWZone* system(const RWZone*);  // set system()

  static void          clearUtc();               // hush Purify
    
  enum StdZone {
        NewZealand=-12,         CarolineIslands,        MarianaIslands,
        Japan,                  China,                  Java,
        Kazakh,                 Pakistan,               CaspianSea,
        Ukraine,                Nile,                   Europe,
        Greenwich,              Azores,                 Oscar,
        Greenland,              Atlantic,               USEastern,
        USCentral,              USMountain,             USPacific,
        Yukon,                  Hawaii,                 Bering
  };

  enum DstRule {    // supported Daylight Savings Time jurisdictions:
    NoDST,              // Daylight Savings Time never observed
    NoAm,               // North America (US, Canada)
    WeEu                // Western Europe, except UK
  };
  static const RWDaylightRule* dstRule(DstRule=NoAm);

  // RWZoneSimple implements DST rules simple enough to represent
  // as static instances of RWDaylightRule.
  // DST Jurisdictions too complex to represent in table form
  //  require deriving from RWZone to implement necessary semantics.

  RW_TYPESPEC  /* This macro usually expands to nothing */ 
 
};

class RWExport RWZoneSimple : public RWZone {
 public: 
  const RWDaylightRule* rule_;
  RWBoolean       daylightObserved_;
  long	  	  timeZoneOffset_;
  RWCString       timeZoneName_;
  long   	  altZoneOffset_;
  RWCString       altZoneName_;

  RWZoneSimple(const RWDaylightRule* rule,
                long tzoff,  const RWCString& tz_name,
                long altoff, const RWCString& altname)
    : rule_(rule), daylightObserved_(rule != 0)
    , timeZoneOffset_(tzoff), timeZoneName_(tz_name)
    , altZoneOffset_(altoff), altZoneName_(altname)
    { }

  RWZoneSimple(long tzoff, const RWCString& tz_name);
  RWZoneSimple(StdZone zone, const RWDaylightRule* rule);
  RWZoneSimple(StdZone zone, DstRule = NoAm);
  
  virtual ~RWZoneSimple();

#if defined(RW_USE_LONG_ZONE_OFFSET)
  virtual long      timeZoneOffset  () const;   // seconds west of UTC
  virtual long      altZoneOffset   () const;
#else
  virtual int       timeZoneOffset  () const;   // seconds west of UTC
  virtual int       altZoneOffset   () const;
#endif
  virtual RWBoolean daylightObserved() const;
  virtual RWCString timeZoneName    () const;
  virtual RWCString altZoneName     () const;

  // isDaylight() may use all members of tmbuf except tm_isdst
  virtual RWBoolean isDaylight(const struct tm* tmbuf) const;

  // The following functions use the tm_year member of struct tm, and
  //   set all the remaining members.
  virtual void getBeginDaylight(struct tm*) const;
  virtual void getEndDaylight(struct tm*) const;

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWZONE_H__ */
