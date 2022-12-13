#ifndef __RWTPMMAP_H__
#define __RWTPMMAP_H__

/***************************************************************************
 *
 * tpmmap.h - RWTPtrMultiMap<Key,T,Compare>
 *   
 *     : pointer-based key/data dictionary wrapper for STL mulitmap
 * 
 * $Id: tpmmap.h 18623 2000-03-03 08:28:13Z sih $
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
#else 

#include <map>
#include "rw/ep_inmap.h"

template <class K, class T, class C>
class RWTPtrMultiMap :
#ifndef RW_ALLOCATOR_PLAIN
     private RW_PMapAssoc<RW_SL_STD(multimap)<K*,T*,rw_deref_compare<C,K>,RW_SL_STD(allocator)<T*> >,
                          RWTPtrMultiMap<K,T,C>, K, T>
#else
     private RW_PMapAssoc<RW_SL_STD(multimap)<K*,T*,rw_deref_compare<C,K>,RW_SL_STD(allocator)>,
                          RWTPtrMultiMap<K,T,C>, K, T>
#endif
{
public:
// typedefs
  typedef rw_deref_compare<C,K>                      container_comp;
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(multimap)<K*,T*,container_comp,RW_SL_STD(allocator)<T*> > container_type;
#else
  typedef RW_SL_STD(multimap)<K*,T*,container_comp,RW_SL_STD(allocator)>   container_type;
#endif
  typedef RWTPtrMultiMap<K,T,C>                      this_type;
  typedef RW_PMapAssoc<container_type,this_type,K,T> base_type;
  
  typedef typename container_type::size_type          size_type;
  typedef typename container_type::difference_type    difference_type;

  typedef typename container_type::iterator           iterator;
  typedef typename container_type::const_iterator     const_iterator;
  typedef typename container_type::value_type         value_type;
  typedef typename container_type::reference          reference;
  typedef typename container_type::const_reference    const_reference;

  typedef K*                                          value_type_key;
  typedef T*                                          value_type_data;
  typedef K*&                                         reference_key;
  typedef T*&                                         reference_data;
  typedef const K*const&                              const_reference_key;
  typedef const T*const&                              const_reference_data;

// STL-level access
  const container_type&  std() const { return impl_; }
  container_type& std() { return impl_; }
  
// inherited interface components
  base_type::apply;
  base_type::applyToKeyAndValue; // deprecated
  base_type::begin;
  base_type::clearAndDestroy;
  base_type::end;
  base_type::isEmpty;
  base_type::entries;
  base_type::operator==;
  base_type::operator<;
  base_type::clear;
  base_type::contains;
  base_type::find;
  base_type::findValue;
  base_type::findKeyAndValue;
  base_type::occurrencesOf;
  base_type::remove;
  base_type::removeAll;


// additional interface components
  bool insert(K* key, T* partner)
    { impl_.insert(value_type(key,partner));
      return true; } 

  bool insertKeyAndValue(K* key, T* partner) // deprecated
  { return insert(key,partner); }

// assignment operators
  RWTPtrMultiMap<K,T,C>&   operator=(const this_type& rwmap)
    { impl_ = rwmap.impl_; return *this; }
  RWTPtrMultiMap<K,T,C>&   operator=(const container_type& stdmap)
    { impl_ = stdmap; return *this; }
  
// constructors
  #if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrMultiMap()
  { 
          impl_ = container_type(container_comp());
  }
#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  RWTPtrMultiMap(const container_comp &cmp RW_DEFAULT_ARG(container_comp())): impl_(cmp) { }
  RWTPtrMultiMap(const container_type& s)               : impl_(s) { }
  RWTPtrMultiMap(const this_type& rws)                  : impl_(rws.impl_) { }
  RWTPtrMultiMap(value_type* first, value_type* last,
             const container_comp& cmp RW_DEFAULT_ARG(container_comp()))
#  ifdef RW_STLCTOR_PTR_HACK
    : impl_(first,last,cmp) { }
#  else
    : impl_(cmp)
  { while(first != last) { impl_.insert(*first); ++first;} }
#  endif /* RW_STLCTOR_PTR_HACK */

#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  RWTPtrMultiMap(value_type* first, value_type* last)
#  ifdef RW_STLCTOR_PTR_HACK
  {
    container_comp cmp; impl_ = container_type(first, last, container_comp());
  }
#  else
  { 
    impl_ = container_type(container_comp());
        while(first != last) {
          impl_.insert(*first); 
          ++first;
        }
  }
#  endif /* RW_STLCTOR_PTR_HACK */
#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  
private:
  container_type impl_;
  friend class RW_PBase<container_type, this_type>;
};

RWDECLARE_PERSISTABLE_TEMPLATE_3(RWTPtrMultiMap)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tpmmap.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tpmmap.cc"
#endif

/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrMultiMapIterator<K,V>            *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class K, class T, class C>
class RWTExport RWTPtrMultiMapIterator
{
public:

  RWTPtrMultiMapIterator(RWTPtrMultiMap<K,T,C>& m)
          : rwmap_(&m), head_(true), tail_(false) { }

  RWBoolean     operator++()            // Advance and test
  {
    RWASSERT(!(head_ == true  &&  tail_ == true)); 

    if (tail_)
      /* do nothing */ ;

    else if (head_) {
      iter_ = rwmap_->begin();
      head_ = false;
    }
    else
      ++iter_;

    if (!tail_ && iter_ == rwmap_->end())
      tail_ = true;

    RWASSERT(!(head_ == true  &&  tail_ == true)); 

    return !tail_;
  }

  K*    operator()()  { return ++(*this) ? key() : rwnil; }

  RWTPtrMultiMap<K,T,C>* container() const      { return rwmap_; }

  K*    key() const   { return (*iter_).first; }

  void  reset()       { head_ = true; tail_ = false; } 
  void  reset(RWTPtrMultiMap<K,T,C>& m)
                              { head_ = true; tail_ = false; rwmap_ = &m; } 

  T*    value() const { return (*iter_).second; }

private:
  RWTPtrMultiMap<K,T,C>*            rwmap_;
  typename RWTPtrMultiMap<K,T,C>::iterator   iter_;
  bool                              head_;
  bool                              tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */
#endif /* __RWTPMMAP_H__ */
