#ifndef __RWTVSRTVEC_H__
#define __RWTVSRTVEC_H__

/***************************************************************************
 *
 * tvsrtvec.h - RWTValSortedVector<T,Compare>
 *
 *     : value-based sorted vector wrapper for STL vector
 *
 * $Id: tvsrtvec.h 18623 2000-03-03 08:28:13Z sih $
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
#  include "rw/xvsrtvec.h"
#  undef  RWDefCArgs
#  define RWDefCArgs(T)
#else
#  ifndef RWDefCArgs
#    include <functional>
#    define RWDefCArgs(T) ,RW_SL_STD(less)< T >
#  endif

// STL-based implementation:
                         
#include "rw/ev_srtsq.h" // RW_VSrtSeq module (#2904-before #include<vector>)

#ifndef RW_HEADER_WITH_EXTENSION 
# include <vector>
#else
# include <vector.hpp>
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T, class C> // hold type and comparison object
class RWTExport RWTValSortedVector : private
#ifndef RW_ALLOCATOR_PLAIN
    RW_VSrtSeq<RW_SL_STD(vector)<T,RW_SL_STD(allocator)<T> >,RWTValSortedVector<T,C>, C>
#else
    RW_VSrtSeq<RW_SL_STD(vector)<T,RW_SL_STD(allocator)>,RWTValSortedVector<T,C>, C>
#endif
{

public:
// typedefs
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(vector)<T,RW_SL_STD(allocator)<T> >                    container_type;
#else
  typedef RW_SL_STD(vector)<T,RW_SL_STD(allocator)>                        container_type;
#endif

  typedef RWTValSortedVector<T,C>                    this_type;
  typedef RW_VSrtSeq<container_type,this_type,C>     base1_type;
  typedef typename container_type::iterator          iterator;
  typedef typename container_type::const_iterator    const_iterator;
  typedef typename container_type::size_type         size_type;
  typedef typename container_type::value_type        value_type;
  typedef typename container_type::reference         reference;
  typedef typename container_type::const_reference   const_reference;


// STL-level access (const only)
  const container_type&  std() const { return impl_; }
  container_type&  std() { return impl_; }

public:  
// inherited interface components

#ifndef RW_BROKEN_ACCESS_ADJUST
  typedef base1_type                                  base2_type;
  typedef base1_type                                  base3_type;
#else
  typedef RW_VSeq<container_type,this_type>           base2_type;
  typedef RW_VBase<container_type,this_type>          base3_type;
#endif  /* RW_BROKEN_ACCESS_ADJUST */
  

  base3_type::begin;
  base2_type::clear;
  base1_type::contains;
  base3_type::end;
  base3_type::entries; 
  base1_type::find;
  base3_type::first;
  base3_type::index;
  base1_type::insert;
  base3_type::isEmpty;
  base1_type::isSorted;
  base3_type::npos;
  base1_type::occurrencesOf;
  base3_type::operator<;
  base3_type::operator==;
  base3_type::operator[];
  base3_type::operator();
  base1_type::remove;
  base1_type::removeAll;
  base2_type::removeAt;
  base2_type::removeFirst;
  base3_type::at;



  // selective inheritance (hides other same-name components)
  void              apply(void (*fn)(const_reference,void*),void *x) const
    { base1_type::apply(fn,x); }




// additional interface components
  const T*          data() const      { return &(const T&)(*(this->begin())); }
  const_reference   last() const      { return std().back(); }
  size_type         merge(const this_type& coll)
    {
      size_type ret = coll.entries();
      if (ret) {
        size_type n = this->entries() + ret;
        container_type v(n);
        RW_SL_STD_GLOBAL(merge)(this->begin(),this->end(),
		coll.begin(),coll.end(),v.begin(),C());
        impl_ = v;
      }
      return ret;
    }

  value_type        removeLast()
    { T ret = last(); impl_.pop_back(); return ret; }
  size_type         length() const    { return std().size(); }
  void              resize(size_t n)  { impl_.reserve(n); }


// constructors
  RWTValSortedVector() { }
  RWTValSortedVector(const RWTValSortedVector<T,C>& t) : impl_(t.impl_) { }
  RWTValSortedVector(const container_type& t) {
    resize(t.size()); // purely for efficiency
    insert(t);
  }
  RWTValSortedVector(size_type n, const T& val) : impl_(n,val) { }
  RWTValSortedVector(const T* first, const T* last);

  // For Tools-6 compatibility (not particularly efficient):
  RWTValSortedVector(size_type n) { resize(n); }


// ~RWTValSortedVector(); // handled by auto-generated destructor

private:
  container_type impl_;
  friend class RW_VBase<container_type,this_type>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

RWDECLARE_PERSISTABLE_TEMPLATE_2(RWTValSortedVector)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvsrtvec.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvsrtvec.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */

#endif /* __RWTVSRTVEC_H__ */
