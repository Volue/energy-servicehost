#ifndef __RWCOLLINT_H__
#define __RWCOLLINT_H__

/*
 * RWCollectableInt --- RWCollectable Integers.
 *
 * $Id: collint.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/rwint.h"
#include "rw/collect.h"

/****************************************************************
 *                                                              *
 *                      RWCollectableInt                        *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWCollectableInt : public RWCollectable, public RWInteger {

  RWDECLARE_COLLECTABLE(RWCollectableInt)

public:

  RWCollectableInt();
  RWCollectableInt(int j) :     RWInteger(j) { }
  RWCollectableInt(const RWCollectableInt& ci) : RWInteger(ci.value()) { }
  virtual ~RWCollectableInt();

  RWBoolean                     operator==(const RWCollectableInt& c) const
    {return value()==c.value();}
  RWBoolean                     operator==(const RWInteger& rwi) const
    {return value()==rwi.value();}

  virtual RWspace               binaryStoreSize() const {return RWInteger::binaryStoreSize();}
  virtual int                   compareTo(const RWCollectable*) const;
  virtual unsigned              hash() const;
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual void                  restoreGuts(RWvistream&);
  virtual void                  restoreGuts(RWFile&);
  virtual void                  saveGuts(RWvostream&) const;
  virtual void                  saveGuts(RWFile&) const;
};        

/* add the following functions to help compilers disambiguate */
#ifndef RW_NO_IOSTD
#  include <iosfwd>
#else
  class ostream;
  class istream;
#endif
  
inline RW_SL_IO_STD(ostream)& operator<<(RW_SL_IO_STD(ostream)& os,const RWCollectableInt& cti)
{ return os << (const RWInteger&)cti; }

inline RW_SL_IO_STD(istream)& operator>>(RW_SL_IO_STD(istream)& is, RWCollectableInt& cti)
{ return is >> (RWInteger&)cti; }

#ifndef RW_NO_RESOLVE_TEMPL_AMBIGUITY
#include "rw/strmshft.h"
class RWExport RWpostream;
class RWExport RWbostream;
class RWExport RWeostream;

class RWExport RWpistream;
class RWExport RWbistream;
class RWExport RWeistream;

RW_PROVIDE_DVSTREAM_INSERTER(RWpostream,RWCollectableInt)
RW_PROVIDE_DVSTREAM_INSERTER(RWbostream,RWCollectableInt)
RW_PROVIDE_DVSTREAM_INSERTER(RWeostream,RWCollectableInt)

RW_PROVIDE_DVSTREAM_EXTRACTOR(RWpistream,RWCollectableInt)
RW_PROVIDE_DVSTREAM_EXTRACTOR(RWbistream,RWCollectableInt)
RW_PROVIDE_DVSTREAM_EXTRACTOR(RWeistream,RWCollectableInt)

#ifndef RW_NO_XDR
   class RWExport RWXDRostream;
   class RWExport RWXDRistream;
   RW_PROVIDE_DVSTREAM_INSERTER(RWXDRostream,RWCollectableInt)
   RW_PROVIDE_DVSTREAM_EXTRACTOR(RWXDRistream,RWCollectableInt)
#endif /*  RW_NO_XDR  */

#endif /* RW_NO_RESOLVE_TEMPL_AMBIGUITY */

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif


#endif /* __RWCOLLINT_H__ */
