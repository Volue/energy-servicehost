#ifndef __RWINT_H__
#define __RWINT_H__

/*
 * Declarations for an integer class.
 *
 * $Id: rwint.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/tooldefs.h"
#include "rw/rstream.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWInteger{
  int                   intBase;
public:
  RWInteger(int i=0)    {intBase = i;}

  // The following two should not be necessary, but are required to
  // support feeble compilers:
  RWInteger(const RWInteger& i) : intBase(i.intBase) {;}
  RWInteger& operator=(const RWInteger& i) {intBase=i.intBase; return *this;}

  RWBoolean             operator==(const RWInteger& i) const
        { return intBase==i.intBase; }

#ifndef RW_ZTC_TYPE_CONVERSION_BUG
  operator              int() const {return intBase;}
#endif
  RWspace               binaryStoreSize() const         {return sizeof(int);}
  void                  restoreFrom(RWvistream& s);
  void                  restoreFrom(RWFile& f);
  void                  saveOn(RWvostream& s) const;
  void                  saveOn(RWFile& f) const;

  int                   value() const {return intBase;}
  int                   value(int newval)
        { int temp=intBase; intBase=newval; return temp; }

friend RW_SL_IO_STD(istream)& operator>>(RW_SL_IO_STD(istream)& i, RWInteger& x)
                { return i >> x.intBase;}

    RW_TYPESPEC  /* This macro usually expands to nothing */
};

inline RW_SL_IO_STD(ostream)& operator<<(RW_SL_IO_STD(ostream)& o, const RWInteger& x)
                { return o << x.value(); }
inline RWvistream& operator>>(RWvistream& str,       RWInteger& x)
                { x.restoreFrom(str);  return str;  }
inline RWFile&     operator>>(RWFile& file,          RWInteger& x)
                { x.restoreFrom(file); return file; }
inline RWvistream& operator>>(RWvistream& str,       RWInteger*& x)
                { x = new RWInteger; x->restoreFrom(str);  return str;  }
inline RWFile&     operator>>(RWFile& file,          RWInteger*& x)
                { x = new RWInteger; x->restoreFrom(file); return file; }
inline RWvostream& operator<<(RWvostream& str, const RWInteger& x)
                { x.saveOn(str);       return str;  }
inline RWFile&     operator<<(RWFile& file,    const RWInteger& x)
                { x.saveOn(file);      return file; }

#ifndef RW_NO_RESOLVE_TEMPL_AMBIGUITY
#include "rw/strmshft.h"
class RWExport RWpostream;
class RWExport RWbostream;
class RWExport RWeostream;

class RWExport RWpistream;
class RWExport RWbistream;
class RWExport RWeistream;

RW_PROVIDE_DVSTREAM_INSERTER(RWpostream,RWInteger)
RW_PROVIDE_DVSTREAM_INSERTER(RWbostream,RWInteger)
RW_PROVIDE_DVSTREAM_INSERTER(RWeostream,RWInteger)

RW_PROVIDE_DVSTREAM_EXTRACTOR(RWpistream,RWInteger)
RW_PROVIDE_DVSTREAM_EXTRACTOR(RWbistream,RWInteger)
RW_PROVIDE_DVSTREAM_EXTRACTOR(RWeistream,RWInteger)

#ifndef RW_NO_XDR
   class RWExport RWXDRostream;
   class RWExport RWXDRistream;
   RW_PROVIDE_DVSTREAM_INSERTER(RWXDRostream,RWInteger)
   RW_PROVIDE_DVSTREAM_EXTRACTOR(RWXDRistream,RWInteger)
#endif /*  RW_NO_XDR  */

#endif /* RW_NO_RESOLVE_TEMPL_AMBIGUITY */

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWINT_H__ */
