#ifndef __RWTVHMAP_H__
#define __RWTVHMAP_H__

/***************************************************************************
 *
 * tvhdict.h - RWTValHashMap<Key,T,H,EQ>
 *   
 *     : value-based key/data dictionary wrapper for STL-like rw_hashmap
 * 
 * $Id: tvhdict.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/xvhdict.h"
#define RWDefHArgs(T)

#else

#ifndef RWDefHArgs
#  define RWDefHArgs(T) ,RWTHasher<T>,RW_SL_STD(equal_to)<T>
#endif

// STL-based implementation:

#include "rw/stdex/hashmap.h"
#include "rw/ev_inmap.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class K, class T, class H, class EQ>
#ifndef RW_ALLOCATOR_PLAIN
class RWTExport RWTValHashMap : private
   RW_VMapAssoc<rw_hashmap<K,T,H,EQ,RW_SL_STD(allocator)<K> >,RWTValHashMap<K,T,H,EQ>,K,T>
#else
class RWTExport RWTValHashMap : private
   RW_VMapAssoc<rw_hashmap<K,T,H,EQ>,RWTValHashMap<K,T,H,EQ>,K,T>
#endif
{
public:
// typedefs
#ifndef RW_ALLOCATOR_PLAIN
  typedef rw_hashmap<K,T,H,EQ,RW_SL_STD(allocator)<K> >         container_type;
#else
  typedef rw_hashmap<K,T,H,EQ>                       container_type;
#endif
  typedef RWTValHashMap<K,T,H,EQ>                    this_type;
  typedef RW_VMapAssoc<container_type,this_type,K,T> base1_type;
  
# ifndef RW_BROKEN_ACCESS_ADJUST
  typedef base1_type                                 base2_type;
  typedef base1_type                                 base3_type;
# else  
  typedef RW_VMapCntnr<container_type,this_type,K,T> base2_type;
  typedef RW_VBase<container_type,this_type>         base3_type;
# endif  /* RW_BROKEN_ACCESS_ADJUST */
  
  typedef typename container_type::iterator          iterator;
  typedef typename container_type::const_iterator    const_iterator;
  typedef typename container_type::size_type         size_type;
  typedef typename container_type::value_type        value_type;
  typedef typename container_type::key_type          key_type;
  typedef T                                          data_type;
  typedef typename container_type::reference         reference;
  typedef typename container_type::const_reference   const_reference;
  typedef RW_SL_STD(pair)<K,T>                       hit_type;

// STL-level access
  container_type&  std()             { return impl_; }
  const container_type&  std() const { return impl_; }

// inherited interface components

  base1_type::apply;
  base1_type::applyToKeyAndValue; // deprecated
  base1_type::begin;
  base1_type::end;
  base3_type::isEmpty;
  base3_type::entries;
  base1_type::operator==;
	base1_type::operator<;  //Added so that this class can be used with
	                        // other containers that require this operator
  base1_type::clear;      
  base1_type::contains;
  base2_type::find;
  base1_type::findValue;
  base1_type::findKeyAndValue;
  base1_type::occurrencesOf;
  base1_type::remove;
  base1_type::removeAll;



// additional interface components
  bool insert(const K& key, const T& val)
    { return (std().insert(value_type(key,val))).second; }

  bool insertKeyAndValue(const K& key, const T& val) // deprecated
    { return insert(key,val); }

  T& operator[](const K& x)
    { return impl_[x]; }

  size_type
    capacity() const { return impl_.capacity(); }

  float
    fillRatio() const { return impl_.fill_ratio(); }

  void
    resize(size_type newsz) { impl_.resize(newsz); }

// assignment operators
  RWTValHashMap<K,T,H,EQ>&   operator=(const RWTValHashMap<K,T,H,EQ>& rwmap)
    { impl_ = rwmap.impl_; return *this; }
#ifndef RW_ALLOCATOR_PLAIN
  RWTValHashMap<K,T,H,EQ>&   operator=(const rw_hashmap<K,T,H,EQ,RW_SL_STD(allocator)<K> >& stdmap)
    { impl_ = stdmap; return *this; }
#else
  RWTValHashMap<K,T,H,EQ>&   operator=(const rw_hashmap<K,T,H,EQ>& stdmap)
    { impl_ = stdmap; return *this; }
#endif
  
// constructors
  RWTValHashMap() { }
#ifndef RW_ALLOCATOR_PLAIN
  RWTValHashMap(const rw_hashmap<K,T,H,EQ,RW_SL_STD(allocator)<K> >& s) : impl_(s) { }
#else
  RWTValHashMap(const rw_hashmap<K,T,H,EQ>& s) : impl_(s) { }
#endif
  RWTValHashMap(const RWTValHashMap<K,T,H,EQ>& rws) : impl_(rws.impl_) { }
  RWTValHashMap(const value_type* first, const value_type* last)
    : impl_(first,last) { }

  // Tools-6 compatible constructor:
  RWTValHashMap(const H& h, size_type sz = RWDEFAULT_CAPACITY)
    : impl_(sz,h,EQ()) { }

private:
#ifndef RW_ALLOCATOR_PLAIN
  rw_hashmap<K,T,H,EQ,RW_SL_STD(allocator)<K> > impl_;
#else
  rw_hashmap<K,T,H,EQ>               impl_;
#endif
  friend class RW_VBase<container_type,this_type>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

RWDECLARE_PERSISTABLE_TEMPLATE_4(RWTValHashMap)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvhdict.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvhdict.cc"
#endif

/****************************************************************
 *                                                              *
 *      Declarations for RWTValHashMapIterator<K,V>     *
 *                                                              *
 ****************************************************************/

template <class K, class T, class H, class EQ>
class RWTExport RWTValHashMapIterator
{
public:

  RWTValHashMapIterator(RWTValHashMap<K,T,H,EQ>& m)
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

  RWTValHashMap<K,T,H,EQ>* container() const    { return rwmap_; }

  K             key() const   { return (*iter_).first; }

  void          reset()       { head_ = true; tail_ = false; } 
  void          reset(RWTValHashMap<K,T,H,EQ>& m)
                              { head_ = true; tail_ = false; rwmap_ = &m; } 

  T             value() const { return (*iter_).second; }


private:
  RWTValHashMap<K,T,H,EQ>*                       rwmap_;
  typename RWTValHashMap<K,T,H,EQ>::iterator     iter_;
  bool                                           head_;
  bool                                           tail_;

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#define RWTValHashDictionary         RWTValHashMap
#define RWTValHashDictionaryIterator RWTValHashMapIterator

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */

#endif /* __RWTVHMAP_H__ */
