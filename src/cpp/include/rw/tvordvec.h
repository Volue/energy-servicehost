#ifndef __RWTVORDVEC_H__
#define __RWTVORDVEC_H__

/***************************************************************************
 *
 * tvordvec.h - RWTValOrderedVector<T>
 *
 *     : value-based ordered vector wrapper for STL vector
 * 
 * $Id: tvordvec.h 18623 2000-03-03 08:28:13Z sih $
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

#ifdef RW_NO_STL
#include "rw/xvordvec.h"
#else

// STL-based implementation:

#ifndef RW_HEADER_WITH_EXTENSION
# include <vector>
#else
# include <vector.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

#include "rw/ev_seq.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T>
class RWTExport RWTValOrderedVector :
#ifndef RW_ALLOCATOR_PLAIN
    private RW_VSeq<RW_SL_STD(vector)<T,RW_SL_STD(allocator)<T> >,
		RWTValOrderedVector<T> >
#else
#ifndef RW_INHERIT_ACCESS_PROBLEM
    private RW_VSeq<RW_SL_STD(vector)<T,RW_SL_STD(allocator)>,
		RWTValOrderedVector<T> >
#else
    public RW_VSeq<RW_SL_STD(vector)<T,RW_SL_STD(allocator)>, RWTValOrderedVector<T> >
#endif /* RW_INHERIT_ACCESS_PROBLEM */
#endif /* RW_ALLOCATOR_PLAIN */  
{
public:
// typedefs
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(vector)<T,RW_SL_STD(allocator)<T> >                  container_type;
#else
  typedef RW_SL_STD(vector)<T,RW_SL_STD(allocator)>                      container_type;
#endif
  typedef RWTValOrderedVector<T>                   this_type;
  typedef RW_VSeq<container_type,this_type>        base1_type;

  typedef typename container_type::iterator        iterator;
  typedef typename container_type::const_iterator  const_iterator;
  typedef typename container_type::size_type       size_type;
  typedef typename container_type::value_type      value_type;
  typedef typename container_type::reference       reference;
  typedef typename container_type::const_reference const_reference;

// STL-level access
  container_type&  std()             { return impl_; }
  const container_type&  std() const { return impl_; }

#ifndef RW_BROKEN_ACCESS_ADJUST
  typedef base1_type                                base2_type;
  typedef base1_type                                base3_type;
#else
  typedef RW_VCntnr<container_type,this_type>       base2_type;
  typedef RW_VBase<container_type,this_type>        base3_type;
#endif  /* RW_BROKEN_ACCESS_ADJUST */

// inherited interface components

  base1_type::append;
  base3_type::apply;
  base3_type::at;
  base3_type::begin;
  base1_type::clear;
  base3_type::contains;
  base3_type::end;
  base3_type::entries;
  base2_type::find;
  base3_type::first;
  base3_type::index;
  base1_type::insert;
  base1_type::insertAt;
  base3_type::isEmpty;
  base2_type::minElement;
  base2_type::maxElement;
  base3_type::npos;
  base3_type::occurrencesOf;
  base3_type::operator();
  base3_type::operator<;
  base3_type::operator==;
  base3_type::operator[];
  base1_type::prepend;
  base1_type::remove;
  base1_type::removeAll;
  base1_type::removeAt;
  base1_type::removeFirst;
  base1_type::replaceAll;

  

// additional interface components
  const T*      data() const       { return &(const T&)(*(this->begin())); }
  T&            last()             { return std().back(); }      
  const T&      last() const       { return std().back(); }  
  size_type     length() const     { return std().size(); }
  void          resize(size_t n)   { std().reserve(n); }
  void          sort()             { RW_SL_STD_GLOBAL(sort)(this->begin(),this->end()); }
  value_type    removeLast()       { value_type ret = last();
                                     std().pop_back();
                                     return ret;
                                   }

// assignment operators
  RWTValOrderedVector<T>&   operator=(const RWTValOrderedVector<T>& ovec)
    { impl_ = ovec.impl_; return *this; }
  RWTValOrderedVector<T>&   operator=(const container_type& stdv)
    { impl_ = stdv; return *this; }

//  constructors
  RWTValOrderedVector() : impl_() { }
  RWTValOrderedVector(const RWTValOrderedVector<T>& t)
#if defined(RW_BROKEN_TEMPLATE_INITIALIZER)
  { impl_ = t.impl_; }
#else
  : impl_(t.impl_) { }
#endif  
  RWTValOrderedVector(const container_type& t) : impl_(t) { }
  RWTValOrderedVector(size_type n, const T& val) : impl_(n,val) { }
  RWTValOrderedVector(const T* first, const T* last)
#if RW_STLCTOR_PTR_HACK
   : impl_(first,last) { }
#else
   { while(first != last) { impl_.push_back(*first); ++first; } }
#endif

  // For Tools-6 compatibility (not particularly efficient):
  RWTValOrderedVector(size_type n) { resize(n); }

//  ~RWTValOrderedVector(); // handled by auto-generated destructor

private:
  container_type impl_;
  friend class RW_VBase<container_type,this_type>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

RWDECLARE_PERSISTABLE_TEMPLATE(RWTValOrderedVector)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvordvec.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvordvec.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */

#endif /* __RWTVORDVEC_H__ */
