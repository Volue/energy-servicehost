#ifndef __RWTVMMAP_H__
#define __RWTVMMAP_H__

/***************************************************************************
 *
 * tvmmap.h - RWTValMultiMap<Key,T,Compare>
 *   
 *     : value-based key/data dictionary wrapper for STL mulitmap
 * 
 * $Id: tvmmap.h 18623 2000-03-03 08:28:13Z sih $
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

#include <map>
#include "rw/ev_inmap.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class K, class T, class C>
#ifndef RW_ALLOCATOR_PLAIN
class RWTExport RWTValMultiMap :
     private RW_VMapAssoc<RW_SL_STD(multimap)<K,T,C,RW_SL_STD(allocator)<T> >,RWTValMultiMap<K,T,C>,K,T>
#else
class RWTExport RWTValMultiMap :
     private RW_VMapAssoc<RW_SL_STD(multimap)<K,T,C,RW_SL_STD(allocator)>,RWTValMultiMap<K,T,C>,K,T>
#endif
{
public:
// typedefs
#ifndef RW_ALLOCATOR_PLAIN
  typedef RW_SL_STD(multimap)<K,T,C,RW_SL_STD(allocator)<T> >              container_type;
#else
  typedef RW_SL_STD(multimap)<K,T,C,RW_SL_STD(allocator)>                  container_type;
#endif
  typedef RWTValMultiMap<K,T,C>                      this_type;
  typedef RW_VMapAssoc<container_type,this_type,K,T> base_type;
  typedef typename container_type::iterator          iterator;
  typedef typename container_type::const_iterator    const_iterator;
  typedef typename container_type::size_type         size_type;
  typedef typename container_type::value_type        value_type;
  typedef typename container_type::key_type          key_type;
  typedef T                                          data_type;
  typedef typename container_type::reference         reference;
  typedef typename container_type::const_reference   const_reference;

// STL-level access
  container_type&  std()             { return impl_; }
  const container_type&  std() const { return impl_; }

// inherited interface components

  base_type::apply;
  base_type::applyToKeyAndValue; // deprecated
  base_type::begin;
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
  bool       insert(const K& key, const T& val)
  {
    std().insert(value_type(key,val));
    return true; // fails only via exception
  }

  bool       insertKeyAndValue(const K& key, const T& val) // deprecated
  { return insert(key,val);  }

// assignment operators
  RWTValMultiMap<K,T,C>&   operator=(const RWTValMultiMap<K,T,C>& rwmultimap)
    { impl_ = rwmultimap.impl_; return *this; }
  RWTValMultiMap<K,T,C>&   operator=(const container_type& stdmultimap)
    { impl_ = stdmultimap; return *this; }
  
// constructors
  RWTValMultiMap(const C& cmp = C()) : impl_(cmp) { }
  RWTValMultiMap(const container_type& s) : impl_(s) { }
  RWTValMultiMap(const RWTValMultiMap<K,T,C>& rws) : impl_(rws.impl_) { }
  RWTValMultiMap(const value_type* first, const value_type* last,
             const C& cmp = C())
#ifdef RW_STLPTR_CTR_HACK
    : impl_(first,last,cmp) { }
#else
    : impl_(cmp)
    { while(first != last) { impl_.insert(*first); ++first; } }
#endif

private:
  container_type impl_;
  friend class RW_VBase<container_type,this_type>;
};


RWDECLARE_PERSISTABLE_TEMPLATE_3(RWTValMultiMap)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvmmap.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvmmap.cc"
#endif


/****************************************************************
 *                                                              *
 *      Declarations for RWTValMultiMapIterator<K,T,C>          *
 *                                                              *
 ****************************************************************/

template <class K, class T, class C>
class RWTExport RWTValMultiMapIterator
{
public:

  RWTValMultiMapIterator(RWTValMultiMap<K,T,C>& m)
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

  RWBoolean     operator()()  { return ++(*this); }

  RWTValMultiMap<K,T,C>* container() const      { return rwmap_; }

  K             key() const   { return (*iter_).first; }

  void          reset()       { head_ = true; tail_ = false; } 
  void          reset(RWTValMultiMap<K,T,C>& m)
                              { head_ = true; tail_ = false; rwmap_ = &m; } 

  T             value() const { return (*iter_).second; }

private:
  RWTValMultiMap<K,T,C>*                       rwmap_;
  typename RWTValMultiMap<K,T,C>::iterator     iter_;
  bool                                         head_;
  bool                                         tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWTVMMAP_H__ */
