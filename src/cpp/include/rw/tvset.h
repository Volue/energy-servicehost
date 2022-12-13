#ifndef __RWTVSET_H__
#define __RWTVSET_H__

/***************************************************************************
 *
 * tvset.h - RWTValSet<T,Compare>
 *
 *     : value-based set wrapper for STL set
 * 
 * $Id: tvset.h 18623 2000-03-03 08:28:13Z sih $
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
#include "rw/ev_inset.h"
#include "rw/edefs.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T,class C> // contained type, compare object
#ifndef RW_ALLOCATOR_PLAIN
class RWTExport RWTValSet : 
        private RW_VAssoc<RW_SL_STD(set)<T,C,RW_SL_STD(allocator)<T> >,RWTValSet<T,C> >
#else
class RWTExport RWTValSet : 
        private RW_VAssoc<RW_SL_STD(set)<T,C,RW_SL_STD(allocator)>,RWTValSet<T,C> >
#endif
{
public:
// typedefs
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(set)<T,C,RW_SL_STD(allocator)<T> >                        container_type;
#else
  typedef RW_SL_STD(set)<T,C,RW_SL_STD(allocator)>                            container_type;
#endif
  typedef RWTValSet<T,C>                                this_type;
  typedef RW_VAssoc<container_type,this_type>           base_type;
  typedef typename container_type::iterator             iterator;
  typedef typename container_type::const_iterator       const_iterator;
  typedef typename container_type::size_type            size_type;
  typedef typename container_type::value_type           value_type;
  typedef typename container_type::key_type             key_type;
  typedef typename container_type::reference            reference;
  typedef typename container_type::const_reference      const_reference;

// STL-level access
  container_type&  std()             { return impl_; }
  const container_type&  std() const { return impl_; }

// inherited interface components
  base_type::begin;
  base_type::end;
  base_type::isEmpty;
  base_type::entries;
  base_type::operator==;
  base_type::operator<;
  base_type::clear;
  base_type::contains;
  base_type::find;
  base_type::occurrencesOf;
  base_type::remove;
  base_type::removeAll;
  base_type::Union;
  base_type::intersection;
  base_type::difference;
  base_type::symmetricDifference;
  base_type::isSubsetOf;
  base_type::isProperSubsetOf;
  base_type::isEquivalent;

  // selective inheritance (hides other same-name components)
  void  apply(void (*fn)(const_reference,void*),void *x) const
    { base_type::apply(fn,x); }





// additional interface components
  /** bool return value is a change to Tools 6.1 interface **/
  bool  insert(const_reference datum) { return (impl_.insert(datum)).second; }

// assignment operators
  RWTValSet<T,C>&   operator=(const RWTValSet<T,C>& rwset)
    { impl_ = rwset.impl_; return *this; }
  RWTValSet<T,C>&   operator=(const container_type& stdset)
    { impl_ = stdset; return *this; }
  
// constructors
#if defined (RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTValSet() { impl_ = container_type(C()) ; }
#endif
  RWTValSet(const C& cmp RW_DEFAULT_ARG(C()) ) : impl_(cmp) { }

  RWTValSet(const container_type& s) : impl_(s) { }
  RWTValSet(const RWTValSet<T,C>& rws) : impl_(rws.impl_) { }

#if defined (RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTValSet(const T* first, const T* last)
#ifdef RW_STLCTOR_PTR_HACK
  { impl_ = container_type(first,last,C()) ; }
#else   
    { impl_ = container_type(C());  while(first != last)
		{ impl_.insert(*first); ++first; }
	}
#endif
#endif
  RWTValSet(const T* first, const T* last, const C& cmp RW_DEFAULT_ARG(C()))
#ifdef RW_STLCTOR_PTR_HACK
    : impl_(first,last,cmp) { }
#else
    : impl_(cmp)
    {  while(first != last) { impl_.insert(*first); ++first; } }
#endif
private:
  container_type impl_;
  friend class RW_VBase<container_type,this_type>;
};

RWDECLARE_PERSISTABLE_TEMPLATE_2(RWTValSet)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvset.cc"
#endif

/****************************************************************
 *                                                              *
 *      Declarations for RWTValSetIterator<T,C>                 *
 *                                                              *
 ****************************************************************/

template <class T, class C>
class RWTExport RWTValSetIterator
{

public:

  RWTValSetIterator(RWTValSet<T,C>& t)
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
        
  RWTValSet<T,C>* container() const  { return table_; }

  T             key() const   { return *iter_; }

  void          reset()       { head_ = true; tail_ = false; } 
  void          reset(RWTValSet<T,C>& t)
                              { head_ = true; tail_ = false; table_ = &t; } 
private:

  RWTValSet<T,C>*                    table_;
  typename RWTValSet<T,C>::iterator  iter_;
  bool                               head_;
  bool                               tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWTVSET_H__ */
