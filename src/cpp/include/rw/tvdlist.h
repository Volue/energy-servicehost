#ifndef __RWTVDLIST_H__
#define __RWTVDLIST_H__

/***************************************************************************
 *
 * tvdlist.h - RWTValDlist<T>
 *   
 *     : value-based doubly-linked list wrapper for STL list
 * 
 * $Id: tvdlist.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/xvdlist.h"

#else

#ifndef RW_HEADER_WITH_EXTENSION
# include <list>
#else
# include <list.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */
#include "rw/ev_seq.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T>
#ifndef RW_ALLOCATOR_PLAIN
class RWTExport RWTValDlist :
                private RW_VSeq<RW_SL_STD(list)<T,RW_SL_STD(allocator)<T> >, RWTValDlist<T> >
#else
class RWTExport RWTValDlist : 
                private RW_VSeq<RW_SL_STD(list)<T,RW_SL_STD(allocator)>, RWTValDlist<T> >
#endif   
{
public:
// typedefs
  typedef RWTValDlist<T>                               this_type;
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(list)<T,RW_SL_STD(allocator)<T> >                        container_type;
#else
  typedef RW_SL_STD(list)<T,RW_SL_STD(allocator)>                            container_type;
#endif
  typedef RW_VSeq<container_type,this_type>            base1_type;
  typedef typename container_type::iterator            iterator;
  typedef typename container_type::const_iterator      const_iterator;
  typedef typename container_type::size_type           size_type;
  typedef typename container_type::value_type          value_type;
  typedef typename container_type::reference           reference;
  typedef typename container_type::const_reference     const_reference;

// STL-level access
  container_type&  std()             { return impl_; }
  const container_type&  std() const { return impl_; }

# ifndef RW_BROKEN_ACCESS_ADJUST  
    typedef base1_type                                 base2_type;
    typedef base1_type                                 base3_type;
# else
      //inherited interface components cannot always follow base_type
    typedef RW_VCntnr<container_type,this_type>        base2_type;
    typedef RW_VBase<container_type,this_type>         base3_type;
# endif  /* RW_BROKEN_ACCESS_ADJUST */
  
  // inherited interface components

  base1_type::append;
  base3_type::apply;
  base3_type::at;
  base3_type::begin;
  base1_type::clear;
  base3_type::contains;
  base3_type::end;
  base3_type::entries;
  base2_type::find;
  base3_type::first;
  base1_type::get;
  base3_type::index;
  base1_type::insert;
  base1_type::insertAt;
  base3_type::isEmpty;
  base2_type::minElement;
  base2_type::maxElement;
  base3_type::npos;
  base3_type::occurrencesOf;
  base3_type::operator();
  base3_type::operator<;
  base3_type::operator==;
  base3_type::operator[];
  base1_type::prepend;
  base1_type::remove;
  base1_type::removeAll;
  base1_type::removeAt;
  base1_type::removeFirst;
  base1_type::replaceAll;

  
// overridden interface components
  void sort() { 
    std().sort(); 
#ifdef RW_SORT_LIST_BACKWARD
    std().reverse();
#endif
}
  
// additional interface components
  T&        last()
    { RWPRECONDITION(this->entries() != 0); return std().back(); }
  const T&  last() const
    { RWPRECONDITION(this->entries() != 0); return std().back(); }

  T         removeLast()
    {
#ifdef RWDEBUG
      size_type n_dbg = this->entries();
#endif      
      RWPRECONDITION(!this->isEmpty());
      T ret = *(--this->end());
      std().pop_back();
      RWPOSTCONDITION(this->entries() == n_dbg - 1);
      return ret;
    }
  
// assignment operators
  RWTValDlist<T>&   operator=(const RWTValDlist<T>& rwset)
    { impl_ = rwset.impl_; return *this; }
  RWTValDlist<T>&   operator=(const container_type& stdset)
    { impl_ = stdset; return *this; }
  
// constructors
  RWTValDlist()                                : impl_() {}
  RWTValDlist(const container_type& lst)       : impl_(lst) {}
  RWTValDlist(const RWTValDlist<T>& lst)       : impl_(lst.impl_) {}

#if defined (RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTValDlist(size_type n)   {impl_ = container_type(n,T());}
#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  
#if defined (__TURBOC__) && (__TURBOC__ == 0x0540)
  RWTValDlist(size_type n, const T& t RW_DEFAULT_ARG(T()) )
  {
	  for (int i=0; i<n; i++)
		  impl_.push_back(t);
  }
#else
  RWTValDlist(size_type n, const T& t RW_DEFAULT_ARG(T()) )   : impl_(n,t) {}
#endif //(__TURBOC__) && (__TURBOC__ == 0x0540)
  
  RWTValDlist(const T* first, const T*last)
#ifdef RW_STLCTOR_PTR_HACK
    : impl_(first,last) {}
#else
  { while(first != last) { impl_.push_back(*first); ++first; } }
#endif
// ~RWTValDlist(); // handled by auto-generated destructor

private:
  container_type impl_;         
  friend class RW_VBase<container_type,this_type>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTValDlist)


/****************************************************************
 *                                                              *
 *      Declarations for RWTValDlistIterator<T>                 *
 *                                                              *
 ****************************************************************/

template <class T> class RWTExport RWTValDlistIterator
{
private:
  RWTValDlist<T>*                    list_;
  typename RWTValDlist<T>::iterator  iter_;
  bool                               head_;  // simulate head sentinel
  bool                               tail_;  // simulate tail sentinel
/*
 * Invariants:
 *   head_ and tail_ can never both be true
 *   if head_ and tail_ are both false then iter_ is valid
 *     (unless invalidated from outside this class)
 */
public:
  RWTValDlistIterator(RWTValDlist<T>& s)
    : list_(&s), head_(true), tail_(false) { }

  // Operators:
  RWBoolean     operator++();
  RWBoolean     operator--();

  RWBoolean     operator+=(size_t n);
  RWBoolean     operator-=(size_t n);

  RWBoolean     operator()()         { return ++(*this); }

  // Methods:
  RWTValDlist<T>*       container() const { return list_; }

  RWBoolean     findNext(const T& p);
  RWBoolean     findNext(RWBoolean (*testFun)(const T&, void*), void* d);

  void                  insertAfterPoint(const T& a);

  T                     key() const   { return *iter_; }

  RWBoolean             remove();       // Remove item at cursor

  RWBoolean             removeNext(const T& p);
  RWBoolean             removeNext(RWBoolean (*testFun)(const T&, void*), void* d);

  void                  reset()       { head_ = true; tail_ = false; } 
  void                  reset(RWTValDlist<T>& s)
    { head_ = true; tail_ = false; list_ = &s; } 

private:

  // Disallow postfix increment & decrement.  Unless we hide it, some 
  // compilers will substitute the prefix operator in its place.
  RWBoolean             operator++(int);
  RWBoolean             operator--(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvdlist.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#  include "rw/tvdlist.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */

#endif /* __RWTVDLIST_H__ */
