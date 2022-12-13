#ifndef __RWTPHMSET_H__
#define __RWTPHMSET_H__

/***************************************************************************
 *
 * tphasht.h -  RWTPtrHashMultiSet<T,H,EQ>
 *   
 *     : pointer-based set wrapper for STL-like rw_hashmultiset
 * 
 * $Id: tphasht.h 18623 2000-03-03 08:28:13Z sih $
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
#  include "rw/xphasht.h"
#  define RWDefHArgs(T)
#else

#ifndef RWDefHArgs
#  define RWDefHArgs(T) ,RWTHasher<T>, RW_SL_STD(equal_to)<T>
#endif

// STL-based implementation:

#include "rw/stdex/hashmset.h"
#include "rw/ep_hash.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

#ifndef RW_ALLOCATOR_PLAIN
template <class T, class H, class EQ>
class RWTExport RWTPtrHashMultiSet : private
 RW_PHAssoc<rw_hashmultiset<T*,rw_deref_hash<H,T>,
            rw_deref_compare<EQ,T>, RW_SL_STD(allocator)<T*> >,
            RWTPtrHashMultiSet<T,H,EQ>, T>
#else
template <class T, class H, class EQ>
class RWTExport RWTPtrHashMultiSet : private
 RW_PHAssoc<rw_hashmultiset<T*,rw_deref_hash<H,T>,
            rw_deref_compare<EQ,T> >,
            RWTPtrHashMultiSet<T,H,EQ>, T>
#endif   
{
public:
// typedefs
  typedef rw_deref_compare<EQ,T>                           container_eq;
  typedef rw_deref_hash<H,T>                               container_hash;
#ifndef RW_ALLOCATOR_PLAIN
  typedef rw_hashmultiset<T*,container_hash, container_eq,RW_SL_STD(allocator)<T*> >
                                                           container_type;
#else
  typedef rw_hashmultiset<T*,container_hash, container_eq> container_type;
#endif
  typedef RWTPtrHashMultiSet<T,H,EQ>                       this_type;
  typedef RW_PHAssoc<container_type, this_type, T>         base_type;
  typedef base_type                                        base1_type;
  
# ifndef RW_BROKEN_ACCESS_ADJUST
  typedef base1_type                                       base2_type;
  typedef base1_type                                       base3_type;
  typedef base1_type                                       base4_type;
# else
  typedef RW_PAssoc<container_type, this_type, T>          base2_type;
  typedef RW_PCntnr<container_type, this_type, T>          base3_type;
  typedef RW_PBase<container_type, this_type>              base4_type;
# endif  /* RW_BROKEN_ACCESS_ADJUST */

  typedef typename container_type::size_type               size_type;
  typedef typename container_type::difference_type         difference_type;

  typedef typename base1_type::iterator                    iterator;
  typedef typename base1_type::const_iterator              const_iterator;
  typedef typename base1_type::value_type                  value_type;
  typedef typename base1_type::const_value                 const_value;
  typedef typename base1_type::reference                   reference;
  typedef typename base1_type::const_reference             const_reference;

// STL-level access
  const container_type&  std() const { return impl_; }
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

  // selective inheritance (hides other same-name components)
  void  apply(void (*fn)(const T*,void*),void *x) const
    { base_type::apply(fn,x); }


  RWBoolean
    isSubsetOf(const RWTPtrHashMultiSet<T,H,EQ>& rhs) const
    { return base_type::isSubsetMultiOf(rhs); }

  RWBoolean
    isProperSubsetOf(const RWTPtrHashMultiSet<T,H,EQ>& rhs) const
    { return base_type::isProperSubsetMultiOf(rhs); }

  RWBoolean
    isEquivalent(const RWTPtrHashMultiSet<T,H,EQ>& rhs) const
    { return base_type::isEquivalentMulti(rhs); }

  void
    Union(const RWTPtrHashMultiSet<T,H,EQ>& rhs)
      { base_type::UnionHash(rhs); }

  void
    intersection(const RWTPtrHashMultiSet<T,H,EQ>& rhs)
        { base_type::intersectionHash(rhs); }

  void
    difference(const RWTPtrHashMultiSet<T,H,EQ>& rhs)
        { base_type::differenceHash(rhs); }

  void
    symmetricDifference(const RWTPtrHashMultiSet<T,H,EQ>& rhs)
        { base_type::symmetricDifferenceHash(rhs); }

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
  bool  insert(T* datum) { impl_.insert(datum); return true; } 

  size_type
    capacity() const { return impl_.capacity(); }

  float
    fillRatio() const { return impl_.fill_ratio(); }

  void
    resize(size_type newsz) { impl_.resize(newsz); }

// assignment operators
  this_type&   operator=(const this_type& rwmset)
    { impl_ = rwmset.impl_; return *this; }

// constructors
#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrHashMultiSet()
  {
    impl_ = container_type(size_type(1024),rw_deref_hash<H,T>(H()),rw_deref_compare<EQ,T>(EQ()));
  }

  RWTPtrHashMultiSet(size_type sz)
  {
	impl_ = container_type(sz, rw_deref_hash<H,T>(H()), rw_deref_compare<EQ,T>(EQ()));
  }
  RWTPtrHashMultiSet(size_type sz, const H& h)
  {
	impl_ = container_type(sz, rw_deref_hash<H,T>(h), rw_deref_compare<EQ,T>(EQ()));
  }
#endif  /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  RWTPtrHashMultiSet(size_type sz RW_DEFAULT_ARG(1024), const H& h  RW_DEFAULT_ARG(H()),
	  const EQ& eq RW_DEFAULT_ARG( EQ()) )
    : impl_(sz,rw_deref_hash<H,T>(h),rw_deref_compare<EQ,T>(eq)) { }


  RWTPtrHashMultiSet(const this_type& rws)      : impl_(rws.impl_) { }
  RWTPtrHashMultiSet(const container_type& s)   : impl_(s) { }



#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrHashMultiSet(T*const* first,T*const* last)
  {
    impl_ = container_type(first, last, size_type(1024), H(), EQ());
  }

  RWTPtrHashMultiSet(T*const* first,T*const* last,size_type sz)
  {
	impl_ = container_type(first, last, sz, H(), EQ());
  }
  RWTPtrHashMultiSet(T*const* first,T*const* last,size_type sz, const H& h)
  {
	impl_ = container_type(first, last, sz, h, EQ());
  }
#endif  /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  RWTPtrHashMultiSet(T*const* first,T*const* last,
                 size_type sz RW_DEFAULT_ARG(1024), const H& h RW_DEFAULT_ARG(H()), const EQ& eq RW_DEFAULT_ARG(EQ()))
    : impl_(first,last,
            sz,rw_deref_hash<H,T>(h),rw_deref_compare<EQ,T>(eq)) {}

  // Tools-6 compatible constructor:
  RWTPtrHashMultiSet(const H& h, size_type sz = RWDEFAULT_CAPACITY)
    : impl_(sz,h,EQ()) { }

private:
  container_type impl_;
  friend class RW_PBase<container_type, this_type>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

RWDECLARE_PERSISTABLE_TEMPLATE_3(RWTPtrHashMultiSet)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tphasht.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tphasht.cc"
#endif

/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrHashMultiSetIterator<T,H,EQ>     *
 *                                                              *
 ****************************************************************/

template <class T, class H, class EQ>
class RWTExport RWTPtrHashMultiSetIterator
{

public:

  RWTPtrHashMultiSetIterator(RWTPtrHashMultiSet<T,H,EQ>& t)
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
        
  RWTPtrHashMultiSet<T,H,EQ>* container() const  { return table_; }

  T*    key() const   { return *iter_; }

  void  reset()       { head_ = true; tail_ = false; } 
  void  reset(RWTPtrHashMultiSet<T,H,EQ>& t)
                              { head_ = true; tail_ = false; table_ = &t; } 

private:

  RWTPtrHashMultiSet<T,H,EQ>*                    table_;
  typename RWTPtrHashMultiSet<T,H,EQ>::iterator  iter_;
  bool                                           head_;
  bool                                           tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


#define RWTPtrHashTable RWTPtrHashMultiSet
#define RWTPtrHashTableIterator RWTPtrHashMultiSetIterator

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif


#endif /* RW_NO_STL */
#endif /*  __RWTPHMSET_H__ */
