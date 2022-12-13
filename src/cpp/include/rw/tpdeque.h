#ifndef __RWTPDEQUE_H__
#define __RWTPDEQUE_H__

/***************************************************************************
 *
 * tpdeque.h - RWTPtrDeque<T>
 *   
 *     : pointer-based double-ended queue wrapper for STL deque 
 * 
 * $Id: tpdeque.h 18623 2000-03-03 08:28:13Z sih $
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

#error RWTPtrDeque not available if RW_NO_STL is defined for your compiler

#endif

#ifndef RW_HEADER_WITH_EXTENSION
# include <deque>
#else
# include <deque.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

#include "rw/ep_seq.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T>
class RWTExport RWTPtrDeque :
#ifndef RW_ALLOCATOR_PLAIN 
           private RW_PSeq<RW_SL_STD(deque)<T*,RW_SL_STD(allocator)<T*> >, RWTPtrDeque<T>, T> {
#else
           private RW_PSeq<RW_SL_STD(deque)<T*,RW_SL_STD(allocator)>, RWTPtrDeque<T>, T> {
#endif	
public:
// typedefs
#ifndef RW_ALLOCATOR_PLAIN 
  typedef RW_SL_STD(deque)<T*,RW_SL_STD(allocator)<T*> >                   container_type;
#else
  typedef RW_SL_STD(deque)<T*,RW_SL_STD(allocator)>                        container_type;
#endif
  typedef RWTPtrDeque<T>                             this_type;
  typedef RW_PSeq<container_type, RWTPtrDeque<T>, T> base_type;

  typedef typename container_type::iterator          iterator;
  typedef typename container_type::const_iterator    const_iterator;
  typedef typename container_type::size_type         size_type;
  typedef typename container_type::difference_type   difference_type;
  typedef typename container_type::value_type        value_type;
  typedef typename container_type::reference         reference;
  typedef typename container_type::const_reference   const_reference;

// STL-level access
  const container_type&  std() const { return impl_; }
  container_type&  std() { return impl_; }

 
// inherited interface components

  base_type::append;
  base_type::apply;
  base_type::at;
  base_type::begin;
  base_type::clear;
  base_type::contains;
  base_type::end;
  base_type::entries;
  base_type::find;
  base_type::first;
  base_type::index;
  base_type::insert;
  base_type::insertAt;
  base_type::isEmpty;
  base_type::minElement;
  base_type::maxElement;
  base_type::npos;
  base_type::occurrencesOf;
  base_type::operator();
  base_type::operator<;
  base_type::operator==;
  base_type::operator[];
  base_type::prepend;
  base_type::remove;
  base_type::removeAll;
  base_type::removeAt;
  base_type::removeFirst;
  base_type::replaceAll;


  void          sort()
    { RW_SL_STD_GLOBAL(sort)(this->begin(),this->end(),rw_deref_compare<RW_SL_STD(less)<T>,T>()); }

  void  clearAndDestroy()
    { while(this->entries()) { delete *(this->begin()); impl_.pop_front();} }
                          
// additional interface components
  T*&       last()       { return impl_.back(); }
  T*const&  last() const { return impl_.back(); }

  void  pushBack(T* t)   { impl_.push_back(t); }
  void  pushFront(T* t)  { impl_.push_front(t); }
  T*    removeLast()     { T* ret = impl_.back();
                           impl_.pop_back();
                           return ret; }
  T*    popBack()        { return removeLast(); }
  T*    popFront()       { return this->removeFirst(); }

// assignment operators
  RWTPtrDeque<T>&   operator=(const RWTPtrDeque<T>& rwdeq)
    { impl_ = rwdeq.impl_; return *this; }
  RWTPtrDeque<T>&   operator=(const container_type& stddeq)
    { impl_ = stddeq; return *this; }

// constructors
  RWTPtrDeque()                                : impl_()            { }
  RWTPtrDeque(const container_type& deq)       : impl_(deq)         { }
  RWTPtrDeque(const this_type& rwdeq)          : impl_(rwdeq.impl_) { }
  RWTPtrDeque(size_type n, T* p)               : impl_(n, p)        { }
  RWTPtrDeque(T*const* first, T*const* last)   
#ifdef RW_STLCTOR_PTR_HACK
    : impl_(first,last)  { }
#else
  { while(first != last) {impl_.push_back(*first); ++first; } }
#endif

//  ~RWTPtrDeque(); // handled by compiler-generated destructor

private:
  container_type impl_;         
  friend class  RW_PBase<container_type, this_type>;
};

RWDECLARE_PERSISTABLE_TEMPLATE(RWTPtrDeque)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tpdeque.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tpdeque.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWTPDEQUE_H__ */
