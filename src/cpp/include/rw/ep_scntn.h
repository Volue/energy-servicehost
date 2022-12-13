#ifndef __RWEP_SCNTN_H__
#define __RWEP_SCNTN_H__

/***************************************************************************
 *
 * ep_scntn.h - RW_PCntnr<StdColl,RWColl,DataTypes>
 *
 *     : pointer-based interface components for STL Containers
 *
 * $Id: ep_scntn.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************/

#include "rw/ep_cntnr.h"
#include "rw/epersist.h"
#include "rw/edefs.h"
#include "rw/rwerr.h"
#include "rw/toolerr.h"
#include "rw/vstream.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class T>
class RWTExport RW_PCntnr : public RW_PBase<StdColl,RWColl>
{
public:
  typedef RW_PBase<StdColl,RWColl>            base_type;

  typedef typename StdColl::value_type        value_type;
  typedef typename StdColl::size_type         size_type;
  typedef typename StdColl::iterator          my_iter_;
  typedef typename StdColl::const_iterator    const_my_iter_;
  typedef typename StdColl::reference         reference;
  typedef typename StdColl::const_reference   const_reference;
  typedef const T*                            const_value;

  typedef rw_deref_compare<RW_SL_STD(less)<T>,T>     deref_less;
  typedef rw_deref_compare<RW_SL_STD(equal_to)<T>,T> deref_equal;

  bool  operator==(const RWColl& rwc)   const
  { return this->entries() == rwc.entries() &&
      RW_SL_STD(equal)(this->begin(), this->end(), rwc.begin(), deref_equal()); }

  bool  operator<(const RWColl& rwc)   const
  { return RW_SL_STD(lexicographical_compare)(this->begin(), this->end(), rwc.begin(),
                                   rwc.end(), deref_less()); }

  void apply(void (*fn)(const_value,void*),void *x) const;
  void apply(void (*fn)(value_type,void*),void *x) { base_type::apply(fn,x); }
  void apply(void (*fn)(reference,void*),void *x)  { base_type::apply(fn,x); }

  bool  contains(const_value key) const;
  bool  contains(bool(*fn)(const_value,void*),void* x) const;
  bool  contains(bool(*fn)(value_type,void*),void* x) const
    { return base_type::contains(fn,x); }

  value_type  find(const_value key) const;
  value_type  find(bool (*fn)(const_value,void*), void* x) const;
  value_type  find(bool (*fn)(value_type,void*), void* x) const;

  size_type     index(bool (*fn)(const_value, void*), void* x) const;
  size_type     index(bool (*fn)(value_type, void*), void* x) const
    { return base_type::index(fn,x); }

  size_type     index(const_value key) const;

  size_type   occurrencesOf(const_value key) const;
  size_type   occurrencesOf(bool (*fn)(const_value,void*),void* x) const;
  size_type   occurrencesOf(bool (*fn)(value_type,void*),void* x) const
    { return base_type::occurrencesOf(fn,x);  }
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ep_scntn.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ep_scntn.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEP_SCNTN_H__ */
