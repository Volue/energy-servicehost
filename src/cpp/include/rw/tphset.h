#ifndef __RWTPHSET_H__
#define __RWTPHSET_H__

/***************************************************************************
 *
 * tphset.h - RWTPtrHashSet<T,H,EQ>
 *   
 *     : pointer-based set wrapper for STL-like rw_hashset
 * 
 * $Id: tphset.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************
 */

#include "rw/compiler.h"

#ifdef RW_NO_STL
#include "rw/xphset.h"
#define RWDefHArgs(T)

#else

#ifndef RWDefHArgs
#  define RWDefHArgs(T) ,RWTHasher<T>,RW_SL_STD(equal_to)<T>
#endif

// STL-based implementation:


#include "rw/stdex/hashset.h"
#include "rw/ep_hash.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T,class H,class EQ>
#ifndef RW_ALLOCATOR_PLAIN
class RWTExport RWTPtrHashSet
    : private RW_PHAssoc<
      rw_hashset<T*, rw_deref_hash<H,T>,rw_deref_compare<EQ,T>, RW_SL_STD(allocator)<T*> >,
                 RWTPtrHashSet<T,H,EQ>, T>
#else
class RWTExport RWTPtrHashSet
    : private RW_PHAssoc<
      rw_hashset<T*, rw_deref_hash<H,T>,rw_deref_compare<EQ,T> >,
                 RWTPtrHashSet<T,H,EQ>, T>
#endif
{
public:
// typedefs
  typedef rw_deref_compare<EQ,T>                      container_eq;
  typedef rw_deref_hash<H,T>                          container_hash;
#ifndef RW_ALLOCATOR_PLAIN
  typedef rw_hashset<T*,container_hash, container_eq, RW_SL_STD(allocator)<T*> >
                                                      container_type;
#else
  typedef rw_hashset<T*,container_hash, container_eq> container_type;
#endif
  typedef RWTPtrHashSet<T,H,EQ>                       this_type;
  typedef RW_PHAssoc<container_type, this_type, T>    base_type;
  typedef base_type                                   base1_type;
  
#ifndef RW_BROKEN_ACCESS_ADJUST
   typedef base1_type                                 base2_type;
   typedef base1_type                                 base3_type;
   typedef base1_type                                 base4_type;
#else  
   typedef RW_PAssoc<container_type, this_type, T>    base2_type;
   typedef RW_PCntnr<container_type, this_type, T>    base3_type;
   typedef RW_PBase<container_type, this_type>        base4_type;
#endif

  typedef typename container_type::size_type          size_type;
  typedef typename container_type::difference_type    difference_type;
  
  typedef typename base_type::iterator                iterator;
  typedef typename base_type::const_iterator          const_iterator;
  typedef typename base_type::value_type              value_type;
  typedef typename base_type::const_value             const_value;
  typedef typename base_type::reference               reference;
  typedef typename base_type::const_reference         const_reference;

// STL-level access
  const container_type& std() const { return impl_; }
  container_type& std() { return impl_; }

// inherited interface components

  base4_type::begin; 
  base4_type::isEmpty;
  base4_type::end;
  base4_type::entries;
  base2_type::clear;
  base2_type::clearAndDestroy;
  base2_type::contains;
  base2_type::find;
  base2_type::occurrencesOf;
  base2_type::remove;
  base2_type::removeAll;
  base2_type::isSubsetOf;
  base2_type::isProperSubsetOf;
  base2_type::isEquivalent;

  // selective inheritance (hides other same-name components)
  void  apply(void (*fn)(const_value,void*),void *x) const
    { base_type::apply(fn,x); }


  RWTPtrHashSet<T,H,EQ> &
    Union(const RWTPtrHashSet<T,H,EQ>& rhs)
    { base_type::UnionHash(rhs); return *this; }

  RWTPtrHashSet<T,H,EQ> &
    intersection(const RWTPtrHashSet<T,H,EQ>& rhs)
    { base_type::intersectionHash(rhs); return *this; }

  RWTPtrHashSet<T,H,EQ> &
    difference(const RWTPtrHashSet<T,H,EQ>& rhs)
    { base_type::differenceHash(rhs); return *this; }

  RWTPtrHashSet<T,H,EQ> &
    symmetricDifference(const RWTPtrHashSet<T,H,EQ>& rhs)
    { base_type::symmetricDifferenceHash(rhs); return *this; }

// overridden interface comoponents
  bool operator==(const this_type& rwset) const { 
#    ifdef RW_STD_COLLECTION_PROBLEM
        return this -> entries() == rwset.entries() && 
               StdCollEqual( this -> std(), rwset.std() );
#    else
        return this -> entries() == rwset.entries() && 
               hashCollEqual( *this, rwset );
#    endif
}

// additional interface components
  bool  insert(value_type datum)
    { return (impl_.insert(datum)).second; }

  size_type
    capacity() const { return impl_.capacity(); }

  float
    fillRatio() const { return impl_.fill_ratio(); }

  void
    resize(size_type newsz) { impl_.resize(newsz); }

// assignment operators
  this_type&   operator=(const this_type& rwset)
    { impl_ = rwset.impl_; return *this; }
  
// constructors
#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrHashSet()
  {
    impl_ = container_type(size_type(1024), H(), EQ());
  }

  RWTPtrHashSet(size_type sz)
  {
	impl_ = container_type(sz, H(), EQ());
  }

  RWTPtrHashSet(size_type sz, const H& h)
  {
	impl_ = container_type(sz, h, EQ());
  }
#endif  /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  RWTPtrHashSet(size_type sz RW_DEFAULT_ARG(1024), const H& h RW_DEFAULT_ARG(H()), 
			const EQ& eq RW_DEFAULT_ARG(EQ()))
    : impl_(sz,rw_deref_hash<H,T>(h),rw_deref_compare<EQ,T>(eq)) { }

  RWTPtrHashSet(const this_type& rws)                  : impl_(rws.impl_) { }
  RWTPtrHashSet(const container_type& s)               : impl_(s) { }


#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrHashSet(T*const* first,T*const* last)
  {
    impl_ = container_type(first, last, size_type(1024), H(), EQ());
  }

  RWTPtrHashSet(T*const* first,T*const* last, size_type sz)
  {
	impl_ = container_type(first, last, sz, H(), EQ());
  }
  RWTPtrHashSet(T*const* first,T*const* last, size_type sz, const H& h)
  {
	impl_ = container_type(first, last, sz, h, EQ());
  }
#endif  /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  RWTPtrHashSet(T*const* first,T*const* last,
            size_type sz RW_DEFAULT_ARG(1024), const H& h RW_DEFAULT_ARG(H()), const EQ& eq RW_DEFAULT_ARG(EQ()))
    : impl_(first,last,
            sz,rw_deref_hash<H,T>(h),rw_deref_compare<EQ,T>(eq)) {}

  // Tools-6 compatible constructor:
  RWTPtrHashSet(const H& h, size_type sz = RWDEFAULT_CAPACITY)
    : impl_(sz,h,EQ()) { }

private:
  container_type impl_;
  friend class RW_PBase<container_type, this_type>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

RWDECLARE_PERSISTABLE_TEMPLATE_3(RWTPtrHashSet)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tphset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tphset.cc"
#endif


/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrHashSetIterator<T,H,EQ>          *
 *                                                              *
 ****************************************************************/

template <class T, class H, class EQ>
class RWTExport RWTPtrHashSetIterator
{ 

public:

  RWTPtrHashSetIterator(RWTPtrHashSet<T,H,EQ>& t)
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

  T*    operator()()    { return (++(*this)) ? key() : rwnil; }
        
  RWTPtrHashSet<T,H,EQ>* container() const  { return table_; }

  T*    key() const   { return *iter_; }

  void  reset()       { head_ = true; tail_ = false; } 
  void  reset(RWTPtrHashSet<T,H,EQ>& t)
                              { head_ = true; tail_ = false; table_ = &t; } 

private:

  RWTPtrHashSet<T,H,EQ>*                         table_;
  typename RWTPtrHashSet<T,H,EQ>::iterator       iter_;
  bool                                           head_;
  bool                                           tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */
#endif /* __RWTPHSET_H__ */
