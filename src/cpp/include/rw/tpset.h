#ifndef __RWTPSET_H__
#define __RWTPSET_H__

/***************************************************************************
 *
 * tpset.h - RWTPtrSet<T,Compare>
 *   
 *     : pointer-based set wrapper for STL set
 * 
 * $Id: tpset.h 18623 2000-03-03 08:28:13Z sih $
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
#error Cannot include header if RW_NO_STL macro is defined for your compiler
#endif

#include <set> 
#include "rw/ep_inset.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T, class C>
#ifndef RW_ALLOCATOR_PLAIN
class RWTExport RWTPtrSet
    : private RW_PAssoc<RW_SL_STD(set)<T*,rw_deref_compare<C,T>,RW_SL_STD(allocator)<T*> >, 
          RWTPtrSet<T,C>, T>
#else
class RWTExport RWTPtrSet
    : private RW_PAssoc<RW_SL_STD(set)<T*,rw_deref_compare<C,T>,RW_SL_STD(allocator)>, 
          RWTPtrSet<T,C>, T>
#endif
{
public:
// typedefs
  typedef rw_deref_compare<C,T>                              container_comp;
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(set)<T*,container_comp,RW_SL_STD(allocator)<T*> >              container_type;
#else
  typedef RW_SL_STD(set)<T*,container_comp,RW_SL_STD(allocator)>                   container_type;
#endif
  typedef RWTPtrSet<T,C>                                     this_type;
  typedef RW_PAssoc<container_type, this_type, T>            base_type;
  typedef typename container_type::size_type                 size_type;
  typedef typename container_type::difference_type           difference_type;
  
  typedef typename base_type::iterator                       iterator;
  typedef typename base_type::const_iterator                 const_iterator;
  typedef typename base_type::value_type                     value_type;
  typedef typename base_type::const_value                    const_value;
  typedef typename base_type::reference                      reference;
  typedef typename base_type::const_reference                const_reference;

// STL-level access
  const container_type& std() const { return impl_; }
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
  base_type::isSubsetOf;
  base_type::isProperSubsetOf;
  base_type::isEquivalent;
  void  Union(const this_type& s) { base_type::Union(s); }
  void  intersection(const this_type& s) { base_type::intersection(s); }
  void  difference(const this_type& s) { base_type::difference(s); }
  void  symmetricDifference(const this_type& s)
        { base_type::symmetricDifference(s); }



// selective inheritance (hides other same-name components)
  void  apply(void (*fn)(const_value,void*),void* x) const
  {   
	const_iterator iter = this->begin();
    while(iter != this->end()) {
      (*fn)(*iter,x);
      ++iter;
	}
  }

// additional interface components
  bool  insert(value_type datum)
    { return impl_.insert(datum).second; }

// assignment operators
  this_type&   operator=(const this_type& rwset)
    { impl_ = rwset.impl_; return *this; }
  this_type&   operator=(const container_type& stdset)
    { impl_ = stdset; return *this; }
  
// constructors
#if  defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrSet( ) {impl_ = container_type(container_comp());  }
#endif
  RWTPtrSet(const container_comp& cmp RW_DEFAULT_ARG( container_comp()) )  : impl_(cmp) { }

  RWTPtrSet(const container_type& s)              {impl_ = s; }
  RWTPtrSet(const this_type& rws)                  : impl_(rws.impl_) { }

#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrSet(T*const* first, T*const* last)
#ifdef RW_STLPTR_CTR_HACK
  { impl_ = container_type((first,last,container_comp());  }
#else
  {impl_ = container_type(container_comp());
  while(first != last) { impl_.insert(*first); ++first; } }
#endif
#endif

  RWTPtrSet(T*const* first, T*const* last,
            const container_comp& cmp RW_DEFAULT_ARG(container_comp()))
#ifdef RW_STLPTR_CTR_HACK
    : impl_(first,last,cmp) { }
#else
  : impl_(cmp)
   { while(first != last) { impl_.insert(*first); ++first; } }
#endif

private:
  container_type impl_;
  friend class RW_PBase<container_type, this_type>;
};

RWDECLARE_PERSISTABLE_TEMPLATE_2(RWTPtrSet)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tpset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tpset.cc"
#endif


/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrSetIterator<T,H,EQ>              *
 *                                                              *
 ****************************************************************/

template <class T, class C>
class RWTExport RWTPtrSetIterator
{ 

public:

  RWTPtrSetIterator(RWTPtrSet<T,C>& t)
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
  RWTPtrSet<T,C>* container() const  { return table_; }
  T*              key() const        { return *iter_; }
  void            reset()            { head_ = true; tail_ = false; } 

  void  reset(RWTPtrSet<T,C>& t)
    { head_ = true; tail_ = false; table_ = &t; } 

private:

  RWTPtrSet<T,C>*                        table_;
  typename RWTPtrSet<T,C>::iterator      iter_;
  bool                                   head_;
  bool                                   tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWTPSET_H__ */
