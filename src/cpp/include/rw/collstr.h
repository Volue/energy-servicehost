#ifndef __RWCOLLSTR_H__
#define __RWCOLLSTR_H__

/*
 * RWCollectableString --- collectable strings
 *
 * $Id: collstr.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/cstring.h"
#include "rw/collect.h"

/****************************************************************
 *                                                              *
 *                      RWCollectableString                     *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWCollectableString : public RWCollectable, public RWCString {

  RWDECLARE_COLLECTABLE(RWCollectableString)

public:

  RWCollectableString();
  RWCollectableString(const char* a) :          RWCString(a)   { }
  RWCollectableString(const char* a, size_t N) : RWCString(a,N) { }
  RWCollectableString(char c, size_t N) :       RWCString(c,N) { }
  RWCollectableString(const RWCString& s) :     RWCString(s)   { }
  RWCollectableString(const RWCSubString& s) :  RWCString(s)   { }
  virtual ~RWCollectableString();

  /* Virtual functions inherited from RWCollectable */
  virtual RWspace               binaryStoreSize() const {return RWCString::binaryStoreSize();}
  virtual int                   compareTo(const RWCollectable*) const;
  virtual unsigned              hash() const;
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual void                  restoreGuts(RWvistream&);
  virtual void                  restoreGuts(RWFile&);
  virtual void                  saveGuts(RWvostream&) const;
  virtual void                  saveGuts(RWFile&) const;

  int               compareTo(const char*,      caseCompare cmp = exact) const;
  int               compareTo(const RWCString&, caseCompare cmp = exact) const;
  unsigned          hash(caseCompare) const;
  static unsigned   hash(const RWCString&);

};

/*
 * The following functions help compilers disambiguate in the face of 
 * multiple derivation, casting operators, etc.
 */

inline RW_SL_IO_STD(ostream)& operator<<(RW_SL_IO_STD(ostream)& os, const RWCollectableString& cts)
{ return os << (const RWCString&)cts; }

inline RW_SL_IO_STD(istream)& operator>>(RW_SL_IO_STD(istream)& is, RWCollectableString& cts)
{ return is >> (RWCString&)cts; }

#ifndef RW_NO_RESOLVE_TEMPL_AMBIGUITY
#include "rw/strmshft.h"
class RWExport RWpostream;
class RWExport RWbostream;
class RWExport RWeostream;

RW_PROVIDE_DVSTREAM_INSERTER(RWpostream,RWCollectableString)
RW_PROVIDE_DVSTREAM_INSERTER(RWbostream,RWCollectableString)
RW_PROVIDE_DVSTREAM_INSERTER(RWeostream,RWCollectableString)

#ifndef RW_NO_XDR
   class RWExport RWXDRostream;
   RW_PROVIDE_DVSTREAM_INSERTER(RWXDRostream,RWCollectableString)
#endif /*  RW_NO_XDR  */

#endif /* RW_NO_RESOLVE_TEMPL_AMBIGUITY */

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWCOLLSTR_H__ */
