#ifndef __RWTPMSET_H__
#define __RWTPMSET_H__

/***************************************************************************
 *
 * tpmset.h - RWTPtrMultiSet<T,Compare>
 *   
 *     : pointer-based set wrapper for STL multiset
 * 
 * $Id: tpmset.h 18623 2000-03-03 08:28:13Z sih $
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
#  error Cannot include header if RW_NO_STL macro is defined for your compiler
#endif

#include <set>
#include "rw/ep_inset.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T,class C>
class RWTExport RWTPtrMultiSet
#ifndef RW_ALLOCATOR_PLAIN
    : private RW_PAssoc<RW_SL_STD(multiset)<T*, rw_deref_compare<C,T>,RW_SL_STD(allocator)<T*> >,
                        RWTPtrMultiSet<T,C>, T>
#else
    : private RW_PAssoc<RW_SL_STD(multiset)<T*, rw_deref_compare<C,T>,RW_SL_STD(allocator)>,
                        RWTPtrMultiSet<T,C>, T>
#endif
{
public:
// typedefs
  typedef rw_deref_compare<C,T>                          container_comp;
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(multiset)<T*, container_comp,RW_SL_STD(allocator)<T*> >    container_type;
#else
  typedef RW_SL_STD(multiset)<T*, container_comp,RW_SL_STD(allocator)>         container_type;
#endif
  typedef RWTPtrMultiSet<T,C>                            this_type;
  typedef RW_PAssoc<container_type, this_type, T>        base_type;

  typedef typename container_type::size_type             size_type;
  typedef typename container_type::difference_type       difference_type;

  typedef typename base_type::iterator                   iterator;
  typedef typename base_type::const_iterator             const_iterator;
  typedef typename base_type::value_type                 value_type;
  typedef typename base_type::const_value                const_value;
  typedef typename base_type::reference                  reference;
  typedef typename base_type::const_reference            const_reference;

// STL-level access
  const container_type&  std() const { return impl_; }
  container_type& std() { return impl_; }
  
// inherited interface components

  base_type::begin; 
  base_type::isEmpty;
  base_type::end;
  base_type::entries;
  base_type::operator==;
  base_type::operator<;
  base_type::clear;
  base_type::clearAndDestroy;
  base_type::contains;
  base_type::find;
  base_type::occurrencesOf;
  base_type::remove;
  base_type::removeAll;

  void  Union(const this_type& s) { base_type::Union(s); }
  void  intersection(const this_type& s) { base_type::intersection(s); }
  void  difference(const this_type& s) { base_type::difference(s); }
  void  symmetricDifference(const this_type& s)
        { base_type::symmetricDifference(s); }

  // selective inheritance (hides other same-name components)
  void  apply(void (*fn)(const T*,void*),void *x) const
    { base_type::apply(fn,x); }

   
  RWBoolean 
     isSubsetOf(const RWTPtrMultiSet<T,C>& rhs) const
     { return base_type::isSubsetMultiOf(rhs); }

  RWBoolean
     isProperSubsetOf(const RWTPtrMultiSet<T,C>& rhs) const
     { return base_type::isProperSubsetMultiOf(rhs); }

  RWBoolean
     isEquivalent(const RWTPtrMultiSet<T,C>& rhs) const
     { return base_type::isEquivalentMulti(rhs); }

// additional interface components
  bool  insert(T* datum) { impl_.insert(datum); return true; } 

// assignment operators
  this_type&   operator=(const this_type& rwmset)
    { impl_ = rwmset.impl_; return *this; }
  this_type&   operator=(const container_type& stdmset)
    { impl_ = stdmset; return *this; }
  
// constructors

#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrMultiSet()  { container_comp cmp; impl_ = container_type(cmp); }
#endif // RW_NO_DEFAULT_TEMPLATE_ARGS


  RWTPtrMultiSet(const container_comp& cmp RW_DEFAULT_ARG(container_comp())) : impl_(cmp) {}
  RWTPtrMultiSet(const container_type& s)     :  impl_(s)   {  }
  RWTPtrMultiSet(const this_type& rws)                  : impl_(rws.impl_) { }

#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrMultiSet(T*const* first, T*const* last) 
#ifdef RW_STLCTOR_PTR_HACK
    { container_comp cmp; impl_ = container_type(first,last,cmp);}
#else
   { container_comp cmp;  impl_ = container_type(cmp);
   while(first != last) {
  impl_.insert(*first); ++first; } }
#endif


#endif // RW_NO_DEFAULT_TEMPLATE_ARGS

  RWTPtrMultiSet(T*const* first, T*const* last,
                 const container_comp& cmp RW_DEFAULT_ARG(container_comp()) )
#ifdef RW_STLCTOR_PTR_HACK
    : impl_(first,last,cmp) { }
#else
  : impl_(cmp)
   { while(first != last) { impl_.insert(*first); ++first; } }
#endif

private:
  container_type impl_;
  friend class RW_PBase<container_type, this_type>;
};

RWDECLARE_PERSISTABLE_TEMPLATE_2(RWTPtrMultiSet)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tpmset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tpmset.cc"
#endif

/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrMultiSetIterator<T,H,EQ>         *
 *                                                              *
 ****************************************************************/

template <class T, class C>
class RWTExport RWTPtrMultiSetIterator
{ 

public:

  RWTPtrMultiSetIterator(RWTPtrMultiSet<T,C>& t)
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

  T*              operator()()       { return (++(*this)) ? key() : rwnil; }
  RWTPtrMultiSet<T,C>*
                  container() const  { return table_; }
  T*              key() const        { return *iter_; }
  void            reset()            { head_ = true; tail_ = false; } 

  void  reset(RWTPtrMultiSet<T,C>& t)
    { head_ = true; tail_ = false; table_ = &t; } 

private:

  RWTPtrMultiSet<T,C>*                        table_;
  typename RWTPtrMultiSet<T,C>::iterator      iter_;
  bool                                        head_;
  bool                                        tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /*  __RWTPMSET_H__ */
