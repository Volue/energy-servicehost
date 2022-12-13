#ifndef __RW_STRINGID_H__
#define __RW_STRINGID_H__

/*
 * Implements a richer set of IDs than RWClassID
 *
 * $Id: stringid.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/vstream.h"
#include "rw/rwfile.h"
#include "rw/cstring.h"

#if !defined(RW_NO_STL) && defined(RW_NO_RELOPS_NAMESPACE)
#  ifndef RW_HEADER_WITH_EXTENSION
#    include<utility>
#  else
#    include<utility.hpp>
#  endif /* RW_HEADER_WITH_EXTENSION */
#endif /* RW_NO_STL */

#if defined(_RWTOOLSDLL)  /*_export variants very complex to get right */
#  include "rw/rstream.h" /*so: use header*/
#else
#  ifndef RW_NO_IOSTD
#    include <iosfwd>
#  else
     class ostream;
     class istream;
#  endif
#endif

#ifdef RW_DECLARE_GLOBAL_FRIENDS_FIRST
class RWSExport RWStringID;

inline
RWBoolean operator==(const RWStringID& lhs, const RWStringID& rhs);
#endif

class RWExport RWStringID : protected RWCString {
public:
  RWStringID()
    : RWCString("NoID") {}

  RWStringID(const RWStringID& sid)
    : RWCString((const RWCString&)sid) {}

  RWStringID(const RWCString& s)
    : RWCString(s) {}

  RWStringID(const char* name)
    : RWCString(name) {}

  ~RWStringID() {}

  RWStringID& operator=(const RWStringID& sid)
  { return (RWStringID&)RWCString::operator=(sid);  }

  RWStringID& operator=(const RWCString& s)
  { return (RWStringID&)RWCString::operator=(s);  }

  operator const char*() const { return RWCString::data(); }
  char  operator[](size_t t) const {return RWCString::operator[](t); }
  char  operator()(size_t t) const {return RWCString::operator()(t); }

#ifndef RW_NO_ACCESS_ADJUSTMENT
  RWCString::binaryStoreSize;
  RWCString::copy;
  RWCString::data;
  RWCString::isAscii;
  RWCString::isNull;
  RWCString::length;
#else
  RWspace       binaryStoreSize() const
    { return RWCString::binaryStoreSize(); }
  RWCString     copy() const 
    { return RWCString::copy(); }
  const char*   data() const
    { return RWCString::data(); }
  RWBoolean     isAscii() const
    { return RWCString::isAscii(); }
  RWBoolean     isNull() const
    { return RWCString::isNull(); }
  size_t        length(char c) const
    { return RWCString::length(); }
#endif  

/*
 * cfront does not implement access adjustment
 * of overloaded member functions:
 */
  size_t        capacity() const
    { return RWCString::capacity(); }
  size_t        capacity(size_t N)
    { return RWCString::capacity(N); }
#ifndef RW_NO_LOCALE
  int           collate(const char* cs) const
    { return RWCString::collate(cs); }
  int           collate(const RWCString& st) const
    { return RWCString::collate(st); }
#endif
  int           compareTo(const char* cs,      caseCompare cmp = exact) const
    { return RWCString::compareTo(cs,cmp); }
  int           compareTo(const RWCString& st, caseCompare cmp = exact) const
    { return RWCString::compareTo(st,cmp); }
  RWBoolean     contains(const char* pat,      caseCompare cmp = exact) const
    { return RWCString::contains(pat,cmp); }
  RWBoolean     contains(const RWCString& pat, caseCompare cmp = exact) const
    { return RWCString::contains(pat,cmp); }
  size_t        first(char c) const
    { return RWCString::first(c); }
  size_t        first(const char* cs) const
    { return RWCString::first(cs); }
  unsigned      hash() const 
    { return RWCString::hash(); }
  size_t index(const char* pat, size_t i=0, caseCompare cmp = exact) const
    { return RWCString::index(pat,i,cmp); }
  size_t index(const RWCString& s, size_t i=0, caseCompare cmp = exact) const
    { return RWCString::index(s,i,cmp); }
  size_t index(const char* pat, size_t patlen, size_t i,caseCompare cmp) const
    { return RWCString::index(pat,patlen,i,cmp); }
  size_t index(const RWCString& s, size_t patlen, size_t i, caseCompare cmp) const
    { return RWCString::index(s,patlen,i,cmp); }
# if !defined(RW_NO_STL)
  size_t        index(const RWCRExpr& pat, size_t i=0) const
    { return RWCString::index(pat,i); }
  size_t        index(const RWCRExpr& pat, size_t* ext, size_t i=0) const
    { return RWCString::index(pat,ext,i); }
  size_t        index(const char* pat, size_t* ext, size_t i=0) const
    { return RWCString::index(pat,ext,i); }
# endif
  size_t        index(const RWCRegexp& pat, size_t i=0) const
    { return RWCString::index(pat,i); }
  size_t        index(const RWCRegexp& pat, size_t* ext, size_t i=0) const
    { return RWCString::index(pat,ext,i); }
  size_t        last(char c) const
    { return RWCString::last(c); }
  size_t        last(char c, size_t i) const
    { return RWCString::last(c,i); }

  void          restoreFrom(RWvistream& s)    { RWCString::restoreFrom(s); }
  void          restoreFrom(RWFile& f)        { RWCString::restoreFrom(f); }
  void          saveOn(RWvostream& s) const   { RWCString::saveOn(s); }
  void          saveOn(RWFile& f) const       { RWCString::saveOn(f); }

#ifdef RW_DECLARE_GLOBAL_FRIENDS_FIRST
  friend RWBoolean operator==(const RWStringID& lhs, const RWStringID& rhs);
#else
  friend  RWBoolean operator==(const RWStringID& lhs, const RWStringID& rhs)
  { return (const RWCString&)lhs == (const RWCString&)rhs; } 
#endif

  friend  RWBoolean operator==(const RWStringID& lhs, const char* rhs)
  { return (const RWCString&)lhs == rhs; } 
  friend  RWBoolean operator==(const char* lhs, const RWStringID& rhs)
  { return lhs == (const RWCString&)rhs; } 

  friend  RWBoolean operator!=(const RWStringID& lhs, const char* rhs)
  { return (const RWCString&)lhs != rhs; }
  friend  RWBoolean operator!=(const char* lhs, const RWStringID&  rhs)
  { return lhs != (const RWCString&)rhs; } 

  friend  RWBoolean operator<(const RWStringID& lhs, const RWStringID& rhs)
  { return (const RWCString&)lhs < (const RWCString&)rhs; }

#if defined(RW_NO_STL) || !defined(RW_NO_RELOPS_NAMESPACE)
  friend  RWBoolean operator!=(const RWStringID& lhs, const RWStringID& rhs)
  { return (const RWCString&)lhs != (const RWCString&)rhs; }

  friend  RWBoolean operator>(const RWStringID& lhs, const RWStringID& rhs)
  { return (const RWCString&)lhs > (const RWCString&)rhs; }

  friend  RWBoolean operator<=(const RWStringID& lhs, const RWStringID& rhs)
  { return (const RWCString&)lhs <= (const RWCString&)rhs; }

  friend  RWBoolean operator>=(const RWStringID& lhs, const RWStringID& rhs)
  { return (const RWCString&)lhs >= (const RWCString&)rhs; }
#endif

  friend RWvostream& operator<<(RWvostream& vos, const RWStringID& sid)
  { sid.saveOn(vos); return vos; }

  friend RWvistream& operator>>(RWvistream& vis, RWStringID& sid)
  { sid.restoreFrom(vis); return vis; }

  friend RWFile& operator<<(RWFile& file, const RWStringID& sid)
  { sid.saveOn(file); return file; }

  friend RWFile& operator>>(RWFile& file, RWStringID& sid)
  { sid.restoreFrom(file); return file; }

  friend RW_SL_IO_STD(ostream)& operator<<(RW_SL_IO_STD(ostream)& os, const RWStringID& sid)
  { os << (const RWCString&)sid; return os; }

  friend RW_SL_IO_STD(istream)& operator>>(RW_SL_IO_STD(istream)& is, RWStringID& sid)
  { is >> (RWCString&)sid; return is; }

static unsigned         hash(const RWStringID& sid)
    { return sid.hash(); }

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_DECLARE_GLOBAL_FRIENDS_FIRST
   inline
   RWBoolean operator==(const RWStringID& lhs, const RWStringID& rhs)
   { return (const RWCString&)lhs == (const RWCString&)rhs; } 
#endif

#endif /*__RW_STRINGID_H__*/
