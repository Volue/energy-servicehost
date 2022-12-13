#ifndef __RWTVHSET_H__
#define __RWTVHSET_H__

/***************************************************************************
 *
 * tvhset.h - RWTValHashSet<T,Hash,Eq>
 *
 *     : value-based hash-set wrapper for STL-like rw_hashset
 * 
 * $Id: tvhset.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/xvhset.h"
#define RWDefHArgs(T)

#else

#ifndef RWDefHArgs
#  define RWDefHArgs(T) ,RWTHasher<T>,RW_SL_STD(equal_to)<T>
#endif

// STL-based implementation:

#include "rw/stdex/hashset.h"
#include "rw/ev_hash.h"
#include "rw/edefs.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T,class H,class EQ>
#ifndef RW_ALLOCATOR_PLAIN
class RWTExport RWTValHashSet
    : private RW_VHAssoc<rw_hashset<T,H,EQ,RW_SL_STD(allocator)<T> >,RWTValHashSet<T,H,EQ> >
#else
class RWTExport RWTValHashSet
    : private RW_VHAssoc<rw_hashset<T,H,EQ>,RWTValHashSet<T,H,EQ> >
#endif
{
public:
// typedefs
  typedef RWTValHashSet<T,H,EQ>                      this_type;
#ifndef RW_ALLOCATOR_PLAIN
  typedef rw_hashset<T,H,EQ,RW_SL_STD(allocator)<T> >           container_type;
#else
  typedef rw_hashset<T,H,EQ>                         container_type;
#endif
  typedef RW_VHAssoc<container_type,this_type>       base_type;
  typedef base_type                                  base1_type;
  
#ifndef RW_BROKEN_ACCESS_ADJUST
  typedef base1_type                                 base3_type; 
  typedef base1_type                                 base2_type; 
#else
  typedef RW_VAssoc<container_type,this_type>        base2_type;
  typedef RW_VCntnr<container_type,this_type>        base3_type;
#endif  /* RW_BROKEN_ACCESS_ADJUST */
  
  typedef typename container_type::iterator          iterator;
  typedef typename container_type::const_iterator    const_iterator;
  typedef typename container_type::size_type         size_type;
  typedef typename container_type::value_type        value_type;
  typedef typename container_type::key_type          key_type;
  typedef typename container_type::reference         reference;
  typedef typename container_type::const_reference   const_reference;

// STL-level access
  container_type&  std()             { return impl_; }
  const container_type&  std() const { return impl_; }

// inherited interface components

  base1_type::begin;
  base1_type::end;
  base3_type::isEmpty;
  base3_type::entries;
  base1_type::operator==;
  base1_type::clear;
  base1_type::contains;
  base1_type::find;
  base1_type::occurrencesOf;
  base1_type::remove;
  base1_type::removeAll;
  base1_type::isSubsetOf;
  base1_type::isProperSubsetOf;
  base1_type::isEquivalent;



// selective inheritance (hides other same-name components)
  RWTValHashSet<T,H,EQ> &
    Union(const RWTValHashSet<T,H,EQ>& rhs)
    { base_type::UnionHash(rhs); return *this; }

  RWTValHashSet<T,H,EQ> &
#ifndef RW_ALLOCATOR_PLAIN
    Union(const rw_hashset<T,H,EQ,RW_SL_STD(allocator)<T> >& rhs)
#else
    Union(const rw_hashset<T,H,EQ>& rhs)
#endif
    { base_type::UnionHash(rhs); return *this; }

  RWTValHashSet<T,H,EQ> &
    intersection(const RWTValHashSet<T,H,EQ>& rhs)
    { base_type::intersectionHash(rhs); return *this; }


  RWTValHashSet<T,H,EQ> &
#ifndef RW_ALLOCATOR_PLAIN
    intersection(const rw_hashset<T,H,EQ,RW_SL_STD(allocator)<T> >& rhs)
#else
    intersection(const rw_hashset<T,H,EQ>& rhs)
#endif
    { base_type::intersectionHash(rhs);  return *this; }

  RWTValHashSet<T,H,EQ> &
    difference(const RWTValHashSet<T,H,EQ>& rhs)
    { base_type::differenceHash(rhs);  return *this; }

  RWTValHashSet<T,H,EQ> &
#ifndef RW_ALLOCATOR_PLAIN
    difference(const rw_hashset<T,H,EQ,RW_SL_STD(allocator)<T> >& rhs)
#else
    difference(const rw_hashset<T,H,EQ>& rhs)
#endif
    { base_type::differenceHash(rhs);  return *this; }

  RWTValHashSet<T,H,EQ> &
    symmetricDifference(const RWTValHashSet<T,H,EQ>& rhs)
    { base_type::symmetricDifferenceHash(rhs);  return *this; }

  RWTValHashSet<T,H,EQ> &
#ifndef RW_ALLOCATOR_PLAIN
    symmetricDifference(const rw_hashset<T,H,EQ,RW_SL_STD(allocator)<T> >& rhs)
#else
    symmetricDifference(const rw_hashset<T,H,EQ>& rhs)
#endif
    { base_type::symmetricDifferenceHash(rhs);  return *this; }

  void  apply(void (*fn)(const_reference,void*),void *x) const
 {
    for(const_iterator i= this -> begin(); i != this -> end(); ++i) {
      (*fn)(*i,x);
	}
  }

// additional interface components
  /** bool return value is a change to Tools 6.1 interface **/
  bool  insert(const_reference datum) { return (impl_.insert(datum)).second; }

  size_type
    capacity() const { return impl_.capacity(); }

  float
    fillRatio() const { return impl_.fill_ratio(); }

  void
    resize(size_type newsz) { impl_.resize(newsz); }

// assignment operators
  RWTValHashSet<T,H,EQ>&   operator=(const RWTValHashSet<T,H,EQ>& rwset)
    { impl_ = rwset.impl_; return *this; }
#ifndef RW_ALLOCATOR_PLAIN
  RWTValHashSet<T,H,EQ>&   operator=(const rw_hashset<T,H,EQ,RW_SL_STD(allocator)<T> >& stdset)
    { impl_ = stdset; return *this; }
#else
  RWTValHashSet<T,H,EQ>&   operator=(const rw_hashset<T,H,EQ>& stdset)
    { impl_ = stdset; return *this; }
#endif
  
// constructors
#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTValHashSet()
  { impl_ = container_type(1024,H(),EQ()); }

  RWTValHashSet(size_type sz)
  { impl_ = container_type(sz,H(),EQ()); }

  RWTValHashSet(size_type sz, const H& h)
  { impl_ = container_type(sz,h,EQ()); }

#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  RWTValHashSet(size_type sz RW_DEFAULT_ARG(1024), const H& h RW_DEFAULT_ARG(H()), const EQ& eq RW_DEFAULT_ARG(EQ()))
    : impl_(sz,h,eq) { }


#ifndef RW_ALLOCATOR_PLAIN
  RWTValHashSet(const rw_hashset<T,H,EQ,RW_SL_STD(allocator)<T> >& s) : impl_(s) { }
#else
  RWTValHashSet(const rw_hashset<T,H,EQ>& s) : impl_(s) { }
#endif
  RWTValHashSet(const RWTValHashSet<T,H,EQ>& rws) : impl_(rws.impl_) { }

#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTValHashSet(const T* first, const T* last)
  { impl_ = container_type(first,last,1024,H(),EQ()); }
  RWTValHashSet(const T* first, const T* last,size_type sz)
  { impl_ = container_type(first,last,sz,H(),EQ()); }
  RWTValHashSet(const T* first, const T* last,size_type sz, const H& h)
  { impl_ = container_type(first,last,sz,h,EQ()); }
#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  RWTValHashSet(const T* first, const T* last,
            size_type sz RW_DEFAULT_ARG(1024), const H& h RW_DEFAULT_ARG(H()), const EQ& eq RW_DEFAULT_ARG(EQ())
           )
    : impl_(first,last,sz,h,eq) { }

  // For Tools-6 compatibility
  RWTValHashSet(const H& h, size_type sz = 1024)
    : impl_(sz,h,EQ()) { }

private:
#ifndef RW_ALLOCATOR_PLAIN
  rw_hashset<T,H,EQ,RW_SL_STD(allocator)<T> > impl_;
#else
  rw_hashset<T,H,EQ>               impl_;
#endif
  friend class RW_VBase<container_type,this_type>;
  
  RW_T_TYPESPEC  /* This usually expands to nothing */
};


RWDECLARE_PERSISTABLE_TEMPLATE_3(RWTValHashSet)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvhset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvhset.cc"
#endif

/****************************************************************
 *                                                              *
 *      Declarations for RWTValHashSetIterator<T,H,EQ>          *
 *                                                              *
 ****************************************************************/

template <class T, class H, class EQ>
class RWTExport RWTValHashSetIterator
{ 

public:

  RWTValHashSetIterator(RWTValHashSet<T,H,EQ>& t)
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
        
  RWTValHashSet<T,H,EQ>* container() const  { return table_; }

  T             key() const   { return *iter_; }

  void          reset()       { head_ = true; tail_ = false; } 
  void          reset(RWTValHashSet<T,H,EQ>& t)
                              { head_ = true; tail_ = false; table_ = &t; } 
private:

  RWTValHashSet<T,H,EQ>*                         table_;
  typename RWTValHashSet<T,H,EQ>::iterator       iter_;
  bool                                           head_;
  bool                                           tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
  
  RW_T_TYPESPEC  /* This usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */

#endif /* __RWTVHSET_H__ */
