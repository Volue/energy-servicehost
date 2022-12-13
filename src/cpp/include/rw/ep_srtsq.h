#ifndef __RWEP_SRTSQ_H__
#define __RWEP_SRTSQ_H__

/***************************************************************************
 *
 * ep_srtsq.h - RW_PSrtSeq<StdColl,RWColl,Compare,DataTypes>
 *   
 * : pointer-based sorted-collection interface components for STL Sequences
 *       
 * 
 * $Id: ep_srtsq.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/compiler.h"
#include "rw/ep_seq.h"

#ifndef RW_HEADER_WITH_EXTENSION
#  include <algorithm>
#else
#  include <algorithm.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class Compare, class T>
class RWTExport RW_PSrtSeq : public RW_PSeq<StdColl,RWColl,T> {
private:
  typedef  RW_PSeq<StdColl,RWColl,T>             base_type;

public:
  typedef typename StdColl::size_type            size_type;
  typedef typename base_type::value_type         value_type;
  typedef typename base_type::const_value        const_value;
  typedef typename base_type::reference          reference;
  typedef typename base_type::const_reference    const_reference;
  typedef typename base_type::my_iter_           my_iter_;
  typedef typename base_type::const_my_iter_     const_my_iter_;
  typedef typename base_type::deref_equal        deref_equal; 
  typedef typename base_type::deref_less         deref_less;

  bool          contains(const_value key) const;
  bool          contains(bool(*fn)(const_value,void*),void* x) const
    { return base_type::contains(fn,x); }

  const_value   find(const_value key) const;
  const_value   find(bool (*fn)(const_value,void*), void* x) const
    { return base_type::find(fn,x); }

  size_type     index(const_value key) const;
  size_type     index(bool (*fn)(const_value, void*), void* x) const
    { return base_type::index(fn,x); }
    
  size_type     insert(const StdColl& coll);
  bool          insert(value_type a)
    { insertSorted(this->begin(),a); return true; } // only fail thru exception

  size_type     occurrencesOf(const_value key) const;
  size_type     occurrencesOf(bool (*fn)(const_value,void*),void* x) const
    { return base_type::occurrencesOf(fn,x); }

// both removeAll() methods should be hidden by the leaf class whenever
// possible since these versions both iterate to find, AND iterate to remove. 
  size_type     removeAll(const_value key);
  size_type     removeAll(bool (*fn)(const_value, void*),void* x)
    { return base_type::removeAll(fn,x); }

  value_type    remove(const_value key);
  value_type    remove(bool (*fn)(const_value,void*),void* x)
    { return base_type::remove(fn,x); }

/** should be protected by #if RWDEBUG but xlC bug prevents it. **/
  bool          isSorted() const;
  
protected:
  RW_PSrtSeq() {} // prevent direct usage

  my_iter_ insertSorted(my_iter_, value_type);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ep_srtsq.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ep_srtsq.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEP_SRTSQ_H__ */
