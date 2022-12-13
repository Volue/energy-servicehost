#ifndef __RWTVHASHT_H__
#define __RWTVHASHT_H__

/***************************************************************************
 *
 * tvhasht.h - RWTValHashTable<T,Compare>
 *   
 *     : value-based multi hash set wrapper for STL-like rw_multihashset
 * 
 * $Id: tvhasht.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/xvhasht.h"
#define RWDefHArgs(T)

#else

#ifndef RWDefHArgs
#  define RWDefHArgs(T) ,RWTHasher<T>,RW_SL_STD(equal_to)<T>
#endif

// STL-based implementation:

#include "rw/stdex/hashmset.h"
#include "rw/ev_hash.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T,class H,class EQ>
#ifndef RW_ALLOCATOR_PLAIN
   class RWTExport RWTValHashMultiSet : private
   RW_VHAssoc<rw_hashmultiset<T,H,EQ,RW_SL_STD(allocator)<T> >, RWTValHashMultiSet<T,H,EQ> >
#else
   class RWTExport RWTValHashMultiSet : private
   RW_VHAssoc<rw_hashmultiset<T,H,EQ>, RWTValHashMultiSet<T,H,EQ> >
#endif
{
public:
// typedefs
#ifndef RW_ALLOCATOR_PLAIN
  typedef rw_hashmultiset<T,H,EQ,RW_SL_STD(allocator)<T> >     container_type;
#else
  typedef rw_hashmultiset<T,H,EQ>                   container_type;
#endif
  typedef RWTValHashMultiSet<T,H,EQ>                this_type;
  typedef RW_VHAssoc<container_type,this_type>      base_type;
  typedef base_type                                 base1_type;
  
# ifndef   RW_BROKEN_ACCESS_ADJUST
   typedef base1_type                               base2_type;
   typedef base1_type                               base3_type;
# else  
   typedef RW_VCntnr<container_type,this_type>      base2_type;
   typedef RW_VBase<container_type,this_type>       base3_type;
#endif  /* RW_BROKEN_ACCESS_ADJUST */

  typedef typename container_type::iterator         iterator;
  typedef typename container_type::const_iterator   const_iterator;
  typedef typename container_type::size_type        size_type;
  typedef typename container_type::value_type       value_type;
  typedef typename container_type::key_type         key_type;
  typedef typename container_type::reference        reference;
  typedef typename container_type::const_reference  const_reference;

// STL-level access
  container_type&  std()              { return impl_; }
  const container_type&  std() const  { return impl_; }

// inherited interface components

  base1_type::begin;
  base1_type::end;
  base3_type::isEmpty;
  base3_type::entries;
  base1_type::operator==;
  base1_type::clear;
  base1_type::contains;
  base2_type::find;
  base1_type::occurrencesOf;
  base1_type::remove;
  base1_type::removeAll;

  void
    Union(const RWTValHashMultiSet<T,H,EQ>& rhs) { base_type::UnionHash(rhs); }

  void
    intersection(const RWTValHashMultiSet<T,H,EQ>& rhs)
        { base_type::intersectionHash(rhs); }

  void
    difference(const RWTValHashMultiSet<T,H,EQ>& rhs)
        { base_type::differenceHash(rhs); }

  RWBoolean
    isSubsetOf(const RWTValHashMultiSet<T,H,EQ>& rhs) const
    { return base_type::isSubsetMultiOf(rhs); }

  RWBoolean
    isProperSubsetOf(const RWTValHashMultiSet<T,H,EQ>& rhs) const
    { return base_type::isProperSubsetMultiOf(rhs); }

  RWBoolean
    isEquivalent(const RWTValHashMultiSet<T,H,EQ>& rhs) const
    { return base_type::isEquivalentMulti(rhs); }

  void
  symmetricDifference(const RWTValHashMultiSet<T,H,EQ>& rhs)
        { base_type::symmetricDifferenceHash(rhs); }

// selective inheritance (hides other same-name components)
  void  apply(void (*fn)(const_reference,void*),void *x) const
    { base_type::apply(fn,x); }




// additional interface components
  bool  insert(const_reference datum) { impl_.insert(datum); return true; } 
  
  size_type
    capacity() const { return impl_.capacity(); }

  float
    fillRatio() const { return impl_.fill_ratio(); }

  void
    resize(size_type newsz) { impl_.resize(newsz); }

// assignment operators
  RWTValHashMultiSet<T,H,EQ>&   operator=(const RWTValHashMultiSet<T,H,EQ>& rwmset)
    { impl_ = rwmset.impl_; return *this; }
#ifndef RW_ALLOCATOR_PLAIN
  RWTValHashMultiSet<T,H,EQ>&   operator=(const rw_hashmultiset<T,H,EQ,RW_SL_STD(allocator)<T> >& stdmset)
    { impl_ = stdmset; return *this; }
#else
  RWTValHashMultiSet<T,H,EQ>&   operator=(const rw_hashmultiset<T,H,EQ>& stdmset)
    { impl_ = stdmset; return *this; }
#endif

// constructors
  RWTValHashMultiSet(size_type sz = 1024, const H& h = H(), const EQ& eq = EQ())
    : impl_(sz,h,eq)  { }

#ifndef RW_ALLOCATOR_PLAIN
  RWTValHashMultiSet(const rw_hashmultiset<T,H,EQ,RW_SL_STD(allocator)<T> >& s) : impl_(s) { }
#else
  RWTValHashMultiSet(const rw_hashmultiset<T,H,EQ>& s) : impl_(s) { }
#endif
  RWTValHashMultiSet(const RWTValHashMultiSet<T,H,EQ>& rws) : impl_(rws.impl_) { }

  RWTValHashMultiSet(const T* first, const T* last,
                 size_type sz = 1024, const H& h = H(), const EQ& eq = EQ())
    : impl_(first,last,sz,h,eq) { }

  // Tools-6 compatible constructor:
  RWTValHashMultiSet(const H& h, size_type sz = RWDEFAULT_CAPACITY)
    : impl_(sz,h,EQ()) { }

private:
#ifndef RW_ALLOCATOR_PLAIN
  rw_hashmultiset<T,H,EQ,RW_SL_STD(allocator)<T> > impl_;
#else
  rw_hashmultiset<T,H,EQ> impl_;
#endif
  friend class RW_VBase<container_type,this_type>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


RWDECLARE_PERSISTABLE_TEMPLATE_3(RWTValHashMultiSet)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvhasht.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvhasht.cc"
#endif

/****************************************************************
 *                                                              *
 *      Declarations for RWTValHashMultiSetIterator<T,H,EQ>     *
 *                                                              *
 ****************************************************************/

template <class T, class H, class EQ>
class RWTExport RWTValHashMultiSetIterator
{ 

public:

  RWTValHashMultiSetIterator(RWTValHashMultiSet<T,H,EQ>& t)
          : table_(&t), head_(true), tail_(false) { }

  RWBoolean     operator++()    // Advance and test
    {
      RWASSERT(!(head_ == true  &&  tail_ == true)); 

      if (tail_)
        /* do nothing */ ;

      else if (head_) {
        iter_ = table_->begin();
        head_ = false;
      }
      else
        ++iter_;

      if (!tail_ && iter_ == table_->end())
        tail_ = true;

      RWASSERT(!(head_ == true  &&  tail_ == true)); 

      return !tail_;
    }



  RWBoolean     operator()()    { return ++(*this); }
        
  RWTValHashMultiSet<T,H,EQ>* container() const  { return table_; }

  T             key() const   { return *iter_; }

  void          reset()       { head_ = true; tail_ = false; } 
  void          reset(RWTValHashMultiSet<T,H,EQ>& t)
                              { head_ = true; tail_ = false; table_ = &t; } 

private:

  RWTValHashMultiSet<T,H,EQ>*                    table_;
  typename RWTValHashMultiSet<T,H,EQ>::iterator  iter_;
  bool                                           head_;
  bool                                           tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#define RWTValHashTable RWTValHashMultiSet
#define RWTValHashTableIterator RWTValHashMultiSetIterator

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */

#endif /* __RWTVHASHT_H__ */
