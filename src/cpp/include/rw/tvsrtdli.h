#ifndef __RWTVSRTDLI_H__
#define __RWTVSRTDLI_H__

/***************************************************************************
 *
 * tvsrtdli.h - RWTValSortedDlist<T,Compare>
 *   
 *     : value-based sorted doubly-linked list wrapper for STL list
 *
 * $Id: tvsrtdli.h 18623 2000-03-03 08:28:13Z sih $
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

#ifndef RW_HEADER_WITH_EXTENSION
# include <list>
#else
# include <list.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

#include "rw/ev_srtsq.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T, class C>
#ifndef RW_ALLOCATOR_PLAIN
class RWTExport RWTValSortedDlist
         : private RW_VSrtSeq<RW_SL_STD(list)<T,RW_SL_STD(allocator)<T> >,RWTValSortedDlist<T,C>, C>
#else
class RWTExport RWTValSortedDlist
         : private RW_VSrtSeq<RW_SL_STD(list)<T,RW_SL_STD(allocator)>,RWTValSortedDlist<T,C>, C>
#endif
{
public:
// typedefs
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(list)<T,RW_SL_STD(allocator)<T> >                         container_type;
#else
  typedef RW_SL_STD(list)<T,RW_SL_STD(allocator)>                             container_type;
#endif
  typedef RWTValSortedDlist<T,C>                        this_type;
  typedef RW_VSrtSeq<container_type,this_type,C>        base_type;
  typedef typename container_type::iterator             iterator;
  typedef typename container_type::const_iterator       const_iterator;
  typedef typename container_type::size_type            size_type;
  typedef typename container_type::value_type           value_type;
  typedef typename container_type::reference            reference;
  typedef typename container_type::const_reference      const_reference;

// STL-level access (const only)
  const container_type&  std() const { return impl_; }
  container_type&  std() { return impl_; }

// inherited interface components

  base_type::begin;
  base_type::clear;
  base_type::contains;
  base_type::end;
  base_type::entries;
  base_type::find;
  base_type::first;
//  base_type::index;
  size_type index(const_reference) const;
  size_type index(bool (*fn)(const_reference, void*), void* x) const
  { return base_type::index(fn,x); }
  base_type::insert;
  base_type::isEmpty;
  base_type::isSorted;
  base_type::npos;
  base_type::occurrencesOf;
  base_type::operator<;
  base_type::operator==;
  base_type::remove;
  base_type::removeAll;
  base_type::removeAt;
  base_type::removeFirst;


  // selective inheritance (hides other same-name components)
  void              apply(void (*fn)(const_reference,void*),void *x) const
    { base_type::apply(fn,x); }


  const_reference   operator[](size_type index) const
    { return base_type::operator[](index); }


  const_reference   operator()(size_type index) const
    { return base_type::operator()(index); }

  const_reference   at(size_type index) const { return base_type::at(index); }




// additional interface components
  const_reference   last() const { return std().back(); }
  T                 removeLast()
    { T ret = *(--this->end()); impl_.pop_back(); return ret; }

  // (Can't use STL list<T>::merge mf until member function templates)
  size_type         merge(const this_type& coll);

// assignment operators
  RWTValSortedDlist<T,C>&   operator=(const RWTValSortedDlist<T,C>& rwcoll)
    { impl_ = rwcoll.impl_; return *this; }
  RWTValSortedDlist<T,C>&   operator=(const container_type& stdcoll)
  { this->clear(); insert(stdcoll); return *this; }
  
// constructors
  RWTValSortedDlist() { }
  RWTValSortedDlist(const RWTValSortedDlist<T,C>& t) : impl_(t.impl_) { }
  RWTValSortedDlist(const container_type& t) { insert(t); }

#ifdef RW_NO_DEFAULT_TEMPLATE_ARGS
  RWTValSortedDlist(size_type n)  {impl_ = container_type(n,T());}
#endif
  
#if defined (__TURBOC__) && (__TURBOC__ == 0x0540)
  RWTValSortedDlist(size_type n, const T& val RW_DEFAULT_ARG(T()) )
  {
	  for (int i=0; i<n; i++)
		  impl_.push_back(val);
  }
#else
  RWTValSortedDlist(size_type n, const T& val RW_DEFAULT_ARG(T()) )   : impl_(n,val) {}
#endif //(__TURBOC__) && (__TURBOC__ == 0x0540)

  RWTValSortedDlist(const T* first, const T* last) {
    const_iterator it = this->begin();
    while(first != last)
      it = insertSorted(it,*first++);
  }
// ~RWTValSortedDlist(); // handled by auto-generated destructor

private:
  container_type impl_;
  friend class RW_VBase<container_type,this_type>;
};

RWDECLARE_PERSISTABLE_TEMPLATE_2(RWTValSortedDlist)

/****************************************************************
 *                                                              *
 *      Declarations for RWTValSortedDlistIterator<T,C>         *
 *                                                              *
 ****************************************************************/

template <class T, class C> class RWTExport RWTValSortedDlistIterator
{

public:
  RWTValSortedDlistIterator(RWTValSortedDlist<T,C>& s)
    : list_(&s), head_(true), tail_(false) { }

  // Operators:
  RWBoolean     operator++();
  RWBoolean     operator--();

  RWBoolean     operator+=(size_t n);
  RWBoolean     operator-=(size_t n);

  RWBoolean     operator()()         { return ++(*this); }

  // Methods:
  RWTValSortedDlist<T,C>*       container() const { return list_; }

  RWBoolean     findNext(const T& p);
  RWBoolean     findNext(RWBoolean (*testFun)(const T&, void*), void* d);

  T                     key() const   { return *iter_; }

  RWBoolean             remove();       // Remove item at cursor

  RWBoolean             removeNext(const T& p);
  RWBoolean             removeNext(RWBoolean (*testFun)(const T&, void*), void* d);

  void                  reset()       { head_ = true; tail_ = false; }
  void                  reset(RWTValSortedDlist<T,C>& s)
    { head_ = true; tail_ = false; list_ = &s; }

private:
  RWTValSortedDlist<T,C>*                    list_;
#ifndef RW_ALLOCATOR_PLAIN
  typename RW_SL_STD(list)<T,RW_SL_STD(allocator)<T> >::iterator   iter_;
#else
  typename RW_SL_STD(list)<T,RW_SL_STD(allocator)>::iterator       iter_;
#endif
  bool                                       head_;  // simulate head sentinel
  bool                                       tail_;  // simulate tail sentinel

  // Disallow postfix increment & decrement.  Unless we hide it, some
  // compilers will substitute the prefix operator in its place.
  RWBoolean             operator++(int);
  RWBoolean             operator--(int);

};



#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvsrtdli.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#  include "rw/tvsrtdli.cc"
#endif




#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWTVSRTDLI_H__ */
