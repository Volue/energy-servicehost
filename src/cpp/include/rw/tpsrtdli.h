#ifndef __RWTPSRTDLI_H__
#define __RWTPSRTDLI_H__

/***************************************************************************
 *
 * tpsrtdli.h - RWTPtrSortedDlist<T,Compare>
 *   
 *     : pointer-based sorted doubly-linked list wrapper for STL list
 * 
 * $Id: tpsrtdli.h 18623 2000-03-03 08:28:13Z sih $
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

// No Tools 6.1 equivalent class, STL-based implementation only

#error Cannot include header if RW_NO_STL macro is defined for your compiler

#endif

#ifndef RW_HEADER_WITH_EXTENSION
# include <list>
#else
# include <list.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

#include "rw/ep_srtsq.h"

template <class T, class C> class RWTExport RWTPtrSortedDlistIterator;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T, class C>
#ifndef RW_ALLOCATOR_PLAIN
class RWTExport RWTPtrSortedDlist
  : private RW_PSrtSeq<RW_SL_STD(list)<T*,RW_SL_STD(allocator)<T*> >,  RWTPtrSortedDlist<T,C>,
                          rw_deref_compare<C,T>, T>
#else
class RWTExport RWTPtrSortedDlist
  : private RW_PSrtSeq<RW_SL_STD(list)<T*,RW_SL_STD(allocator)>,  RWTPtrSortedDlist<T,C>,
                          rw_deref_compare<C,T>, T>
#endif
{
public:
  typedef rw_deref_compare<C,T>                    container_comp;
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(list)<T*,RW_SL_STD(allocator)<T*> >                  container_type;
#else
  typedef RW_SL_STD(list)<T*,RW_SL_STD(allocator)>                       container_type;
#endif
  typedef RWTPtrSortedDlist<T,C>                   this_type;

  typedef RW_PSrtSeq<container_type,
                      this_type,container_comp, T> base_type;

  typedef typename container_type::size_type       size_type;
  typedef typename container_type::difference_type difference_type;

  typedef typename base_type::value_type           value_type;
  typedef typename base_type::const_value          const_value;
  typedef typename base_type::reference            reference;
  typedef typename base_type::const_reference      const_reference;
  typedef typename base_type::const_iterator       const_iterator;
  typedef typename base_type::iterator             iterator;
  
// STL-level access
  const container_type&  std() const { return impl_; }
  container_type&        std()       { return impl_; }

// inherited interface components

  base_type::clear;
  base_type::clearAndDestroy;
  base_type::contains;
  base_type::entries;
  base_type::find;
  base_type::first;
  size_type index(const_value) const;
  size_type index(bool (*fn)(const_value, void*), void* x) const
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
  /*
   * Be careful with these, misuse can lead to the collection
   * becoming out of sorts, so to speak.  
   */
  base_type::operator[];
  base_type::operator();
  base_type::apply;
  base_type::at;
  base_type::begin;
  base_type::end;


// additional interface components
  T*&       last()       { return impl_.back(); }
  T*const&  last() const { return impl_.back(); }


  size_type         merge(const this_type& coll)
    {
      size_type ret = coll.entries();

      if (ret) {
        typename container_type::iterator first1 = impl_.begin();
        typename container_type::iterator last1  = impl_.end();
        const_iterator    first2 = coll.begin();
        const_iterator    last2 = coll.end();

        while (first1 != last1 && first2 != last2) {
          while (first2 != last2 &&
                 container_comp()(*first2,*first1)) {
            impl_.insert(first1,*first2);
            ++first2;
          }
          ++first1;
        }

        while (first2 != last2) {
          impl_.insert(impl_.end(), *first2);
          ++first2;
        }
      }
      return ret;
    }

  T*    removeLast() { T* ret = *(--this -> end());
                       impl_.pop_back(); return ret; }

// constructors
  RWTPtrSortedDlist() { }
  RWTPtrSortedDlist(const this_type& t) : impl_(t.impl_) { }
  RWTPtrSortedDlist(const container_type& t) {
    insert(t);
  }
  RWTPtrSortedDlist(size_type n, T* val) : impl_(n,val) { }

  RWTPtrSortedDlist(T*const* first, T*const* last);

// ~RWTPtrSortedDlist(); // handled by compiler-generated destructor

private:
  container_type impl_;
  friend class RW_PBase<container_type, this_type>;
  friend class RWTExport RWTPtrSortedDlistIterator<T,C>;
};

RWDECLARE_PERSISTABLE_TEMPLATE_2(RWTPtrSortedDlist)

/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrSortedDlistIterator<T>           *
 *                                                              *
 ****************************************************************/

template <class T, class C> class RWTExport RWTPtrSortedDlistIterator
{
public:
  RWTPtrSortedDlistIterator(RWTPtrSortedDlist<T,C>& s)
    : list_(&s), head_(true), tail_(false) { }

  // Operators:
  RWBoolean     operator++();
  RWBoolean     operator--();
  RWBoolean     operator+=(size_t n);
  RWBoolean     operator-=(size_t n);
  T*            operator()()   { return ++(*this) ? key() : rwnil; }         

  // Methods:
  RWTPtrSortedDlist<T,C>* container() const  { return list_; }

  T*    findNext(const T* p);
  T*    findNext(RWBoolean (*testFun)(T*, void*), void* d);

  T*    key() const
    { return *((RWTPtrSortedDlistIterator<T,C>*)this)->iter_; }

  T*    remove();

  T*    removeNext(const T* p);
  T*    removeNext(RWBoolean (*testFun)(T*, void*), void* d);

  void  reset()       { head_ = true; tail_ = false; } 
  void  reset(RWTPtrSortedDlist<T,C>& s)
    { head_ = true; tail_ = false; list_ = &s; }

private:
  RWTPtrSortedDlist<T,C>*                          list_;
  typename RWTPtrSortedDlist<T,C>::iterator iter_;
  bool                                             head_;  // simulate head sentinel
  bool                                             tail_;  // simulate tail sentinel

  // Disallow postfix operators.  Unless we hide it, some compilers will
  // substitute the prefix operator in its place.
  RWBoolean             operator++(int);
  RWBoolean             operator--(int);
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tpsrtdli.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/tpsrtdli.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWTPSRTDLI_H__ */
