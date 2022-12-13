#ifndef __RWTPHMAP_H__
#define __RWTPHMAP_H__

/***************************************************************************
 *
 * tphdict.h - RWHMapPtr<Key,T,H,EQ>
 *   
 *     : pointer-based key/data dictionary wrapper for STL-like rw_hashmap
 * 
 * $Id: tphdict.h 18623 2000-03-03 08:28:13Z sih $
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
#  include "rw/xphdict.h"
#  define RWDefHArgs(T)
#else

#ifndef RWDefHArgs
#  define RWDefHArgs(T) ,RWTHasher<T>,RW_SL_STD(equal_to)<T>
#endif

// STL-based implementation:


#include "rw/stdex/hashmap.h"
#include "rw/ep_inmap.h"

#ifndef RW_ALLOCATOR_PLAIN
template <class K, class T, class H, class EQ>
class RWTPtrHashMap 
  : private RW_PMapAssoc<rw_hashmap<K*,T*,rw_deref_hash<H,K>,
                         rw_deref_compare<EQ,K>,RW_SL_STD(allocator)<K*> >,
                         RWTPtrHashMap<K,T,H,EQ>, K, T>
#else
template <class K, class T, class H, class EQ>
class RWTPtrHashMap 
  : private RW_PMapAssoc<rw_hashmap<K*,T*,rw_deref_hash<H,K>,
                         rw_deref_compare<EQ,K> >,
                         RWTPtrHashMap<K,T,H,EQ>, K, T>
#endif
{
public:
// typedefs
  typedef rw_deref_hash<H,K>                         container_hash;
  typedef rw_deref_compare<EQ,K>                     container_eq; 
#ifndef RW_ALLOCATOR_PLAIN
  typedef rw_hashmap<
   K*,T*,rw_deref_hash<H,K>,rw_deref_compare<EQ,K>,RW_SL_STD(allocator)<K*> > container_type;
#else
  typedef rw_hashmap<
   K*,T*,rw_deref_hash<H,K>,rw_deref_compare<EQ,K> > container_type;
#endif
  typedef RWTPtrHashMap<K,T,H,EQ>                    this_type;
  typedef RW_PMapAssoc<container_type,this_type,K,T> base1_type;
                       
# ifndef RW_BROKEN_ACCESS_ADJUST
  typedef base1_type                                 base2_type;
  typedef base1_type                                 base3_type;
# else  
  typedef RW_PMapCntnr<container_type,this_type,K,T> base2_type;
  typedef RW_PBase<container_type, this_type>        base3_type;
# endif /* RW_BROKEN_ACCESS_ADJUST */
                       
  typedef typename container_type::size_type         size_type;
  typedef typename container_type::difference_type   difference_type;

  typedef typename container_type::iterator          iterator;
  typedef typename container_type::const_iterator    const_iterator;
  typedef typename container_type::value_type        value_type;
  typedef typename container_type::reference         reference;
  typedef typename container_type::const_reference   const_reference;

  typedef K*                                         value_type_key;
  typedef T*                                         value_type_data;
  typedef K*&                                        reference_key;
  typedef T*&                                        reference_data;
  typedef const K*const&                             const_reference_key;
  typedef const T*const&                             const_reference_data;

// STL-level access
  const container_type&  std() const { return impl_; }
  container_type& std() { return impl_; }
  
// inherited interface components
  base1_type::apply;
  base1_type::applyToKeyAndValue;
  base1_type::begin;
  base1_type::clearAndDestroy;
  base1_type::end;
  base3_type::isEmpty;
  base3_type::entries;
  base1_type::clear;
  base1_type::contains;
  base1_type::find;
  base1_type::findValue;
  base1_type::findKeyAndValue;
  base1_type::occurrencesOf;
  base1_type::remove;
  base1_type::removeAll;


// overridden interface components
  bool operator==(const this_type& rwmap) const  { 
    if (this->entries() == rwmap.entries()) {
#   if defined(__GNUG__)
    const container_type& c = std();
#   endif
#   if defined(__GNUG__)
    return hashMapCollEqual(*this,rwmap,c);
#   else
    return hashMapCollEqual(*this,rwmap);
#   endif
    }
    else
      return false;
  }

// additional interface components
  bool insert(K* key, T* partner)
    { return (impl_.insert(value_type (key,partner))).second; }

  bool insertKeyAndValue(K* key, T* partner)
  { return insert(key,partner); }

  T*& operator[](K* x)
    { return (T*&) impl_[x]; }

  size_type
    capacity() const { return impl_.capacity(); }

  float
    fillRatio() const { return impl_.fill_ratio(); }

  void
    resize(size_type newsz) { impl_.resize(newsz); }

// assignment operators
  RWTPtrHashMap<K,T,H,EQ>&   operator=(const this_type& rwmap)
    { impl_ = rwmap.impl_; return *this; }
  RWTPtrHashMap<K,T,H,EQ>&   operator=(const container_type& stdmap)
    { impl_ = stdmap; return *this; }
  
// constructors
  RWTPtrHashMap()  { }
  RWTPtrHashMap(const container_type& s)               : impl_(s) { }
  RWTPtrHashMap(const this_type& rws)                  : impl_(rws.impl_) { }
  RWTPtrHashMap(value_type* first, value_type* last)   : impl_(first, last) { }
   
  // Tools-6 compatible constructor:
  RWTPtrHashMap(const H& h, size_type sz = RWDEFAULT_CAPACITY) : impl_(sz,h,EQ()) { }

private:
  container_type impl_;
  friend class RW_PBase<container_type, this_type>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

RWDECLARE_PERSISTABLE_TEMPLATE_4(RWTPtrHashMap)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tphdict.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tphdict.cc"
#endif

#define RWTPtrHashDictionary RWTPtrHashMap
#define RWTPtrHashDictionaryIterator RWTPtrHashMapIterator


/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrHashMapIterator<K,V>     *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class K, class T, class H, class EQ>
class RWTExport RWTPtrHashMapIterator
{
public:

  RWTPtrHashMapIterator(RWTPtrHashMap<K,T,H,EQ>& m)
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

  RWTPtrHashMap<K,T,H,EQ>* container() const    { return rwmap_; }

  K*    key() const   { return (*iter_).first; }

  void  reset()       { head_ = true; tail_ = false; } 
  void  reset(RWTPtrHashMap<K,T,H,EQ>& m)
                              { head_ = true; tail_ = false; rwmap_ = &m; } 

  T*    value() const { return (*iter_).second; }

private:
  RWTPtrHashMap<K,T,H,EQ>*                       rwmap_;
  typename RWTPtrHashMap<K,T,H,EQ>::iterator     iter_;
  bool                                           head_;
  bool                                           tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */

#endif /* __RWTPHMAP_H__ */







