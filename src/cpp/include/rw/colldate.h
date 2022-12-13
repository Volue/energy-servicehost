#ifndef __RWCOLLDATE_H__
#define __RWCOLLDATE_H__

/*
 * RWCollectableDate --- RWCollectable Dates.
 *
 * $Id: colldate.h 18623 2000-03-03 08:28:13Z sih $
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
 */

#include "rw/rwdate.h"
#include "rw/collect.h"
#include "rw/locale.h"

/****************************************************************
 *                                                              *
 *                      RWCollectableDate                       *
 *                                                              *
 ****************************************************************/

#ifdef RW_OBJECTSTORE_HETERO
#define RW_Pad public _os_pad_t<RWCollectableDate,4,0>,
#else
#define RW_Pad
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWCollectableDate :
        public RWCollectable, RW_Pad public RWDate { 

  RWDECLARE_COLLECTABLE(RWCollectableDate)

public:
  RWCollectableDate();
  RWCollectableDate(unsigned long j)            : RWDate(j)           { }
  RWCollectableDate(unsigned d, unsigned y)     : RWDate(d, y)        { }
  RWCollectableDate(unsigned d, const char* month, unsigned y,
                    const RWLocale& loc = RWLocale::global())
                                                : RWDate(d, month, y, loc) { }
  RWCollectableDate(unsigned d, unsigned m, unsigned y)
                                                : RWDate(d, m, y)     { }
  RWCollectableDate(RW_SL_IO_STD(istream)& s, const RWLocale& locale = RWLocale::global())
                                                : RWDate(s, locale)   { }
  RWCollectableDate(const RWCString& str,
                    const RWLocale& locale = RWLocale::global())
                                                : RWDate(str, locale) { }
  RWCollectableDate(const RWTime& t, const RWZone& zone = RWZone::local())
                                                : RWDate(t, zone)     { }
  RWCollectableDate(const RWDate& d)            : RWDate(d)           { }
  RWCollectableDate(const struct tm* tmb)       : RWDate(tmb)         { }
  virtual ~RWCollectableDate();

  /* Virtual functions inherited from RWCollectable */
  virtual RWspace               binaryStoreSize() const {return RWDate::binaryStoreSize();}
  virtual int                   compareTo(const RWCollectable*) const;
  virtual unsigned              hash() const;
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual void                  restoreGuts(RWvistream&);
  virtual void                  restoreGuts(RWFile&);
  virtual void                  saveGuts(RWvostream&) const;
  virtual void                  saveGuts(RWFile&) const;
#ifdef RW_GLBL_SCP_BUG3
  friend RWBoolean  operator>=(const RWCollectableDate& d1, const RWCollectableDate& d2)
    { return ( (RWDate)d1>=(RWDate)d2 ); }
#endif
};    

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif
#undef RW_Pad
#endif /* __RWCOLLECTABLE_DATE__ */
