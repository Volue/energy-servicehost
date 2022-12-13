#ifndef _STDEX_HASHMAP_H_
#define _STDEX_HASHMAP_H_
/***************************************************************************
 *
 * declaration for STL-like map based on hash table
 *
 * $Id: hashmap.h 18623 2000-03-03 08:28:13Z sih $
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
 ***************************************************************************
 */

#include "rw/compiler.h"
#ifndef RW_HEADER_WITH_EXTENSION
#include <memory>
#else
#include <memory.hpp>
#endif /* RW_HEADER_WITH_EXTENSION */

#include "rw/rwassert.h"
#include "rw/stdex/lhash.h"

#ifndef RW_ALLOCATOR_PLAIN
template <class K, class V, class Hash, class EQ, class A>
#else
template <class K, class V, class Hash, class EQ>
#endif
class rw_hashmap
{
public:
#ifndef RWSTD_NO_CONST_INST
  typedef RW_SL_STD(pair)<const K, V>               value_type;
#else
  typedef RW_SL_STD(pair)<K,V>                      value_type;
#endif

private:

#ifndef RW_NO_DEFAULT_TEMPLATE_ARGS

  struct h_k {
    Hash h_;
    h_k(const Hash& h = Hash()) : h_(h) {}

    inline unsigned long operator()(const value_type & p) const {
      return h_(p.first);
    }
  };

  struct eq_k {
    EQ eq_;
    eq_k(const EQ& eq = EQ()) : eq_(eq) {}

    inline bool operator()(const value_type& p1, const value_type& p2) const {
      return eq_(p1.first, p2.first);
    }
  };

#else

  struct h_k {
    Hash h_;
    h_k(const Hash& h) : h_(h) {}
    h_k() {h_ = Hash();}

    inline unsigned long operator()(const value_type & p) const {
      return h_(p.first);
    }
  };

  struct eq_k {
    EQ eq_;
    eq_k() {eq_ = EQ();}
    eq_k(const EQ& eq) : eq_(eq) {}

    inline bool operator()(const value_type& p1, const value_type& p2) const {
      return eq_(p1.first, p2.first);
    }
  };

#endif  // RW_NO_DEFAULT_TEMPLATE_ARGS

public:
///// typdefs /////

#ifndef RW_ALLOCATOR_PLAIN
  typedef LHashTable<value_type,h_k,eq_k,A> impl_type;
#else
  typedef LHashTable<value_type,h_k,eq_k> impl_type;
#endif
  typedef K                             key_type;
  // typedef  pair<...K,V>              value_type is located above
  typedef typename impl_type::size_type         size_type;
  typedef typename impl_type::difference_type   difference_type;
  typedef typename impl_type::reference         reference;
  typedef typename impl_type::const_reference   const_reference;
  typedef typename impl_type::Iterator          iterator;
  typedef typename impl_type::ConstIterator     const_iterator;
    
  typedef Hash                          key_hash_type;
  typedef EQ                            key_equal_type;
  typedef h_k                           value_hash_type;
  typedef eq_k                          value_equal_type;

  key_hash_type   key_hash() const      { return Hash(); }
  key_equal_type  key_equal() const     { return EQ(); }
  value_hash_type value_hash() const    { return impl.key_hash(); }
  value_equal_type value_equal() const  { return impl.key_equal(); }

public:
///// constructors /////
#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  rw_hashmap()
  {
    impl = impl_type(size_type(1024),value_hash_type(Hash()),value_equal_type(EQ()));
  }

  rw_hashmap(size_type sz)
  {
	impl = impl_type(sz, value_hash_type(Hash()), value_equal_type(EQ()));
  }
  rw_hashmap(size_type sz, const Hash& h)
  {
	impl = impl_type(sz, value_hash_type(h), value_equal_type(EQ()));
  }
#endif  /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  rw_hashmap(size_type sz RW_DEFAULT_ARG(1024), const Hash& h RW_DEFAULT_ARG(Hash()),
	           const EQ& eq RW_DEFAULT_ARG(EQ()))
    : impl(sz,value_hash_type(h),value_equal_type(eq)) {}
#ifndef RW_ALLOCATOR_PLAIN
  rw_hashmap(const rw_hashmap<K,V,Hash,EQ,A>& arg) : impl(arg.impl) {}
#else
  rw_hashmap(const rw_hashmap<K,V,Hash,EQ>& arg) : impl(arg.impl) {}
#endif
#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  rw_hashmap(const value_type* first, const value_type* bound);
  rw_hashmap(const value_type* first, const value_type* bound,
             size_type sz);
  rw_hashmap(const value_type* first, const value_type* bound,
             size_type sz, const Hash& h);
  rw_hashmap(const_iterator first, const_iterator bound);
  rw_hashmap(const_iterator first, const_iterator bound,
             size_type sz);
  rw_hashmap(const_iterator first, const_iterator bound,
             size_type sz, const Hash& h);
#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  rw_hashmap(const value_type* first, const value_type* bound,
             size_type sz RW_DEFAULT_ARG(1024), const Hash& h RW_DEFAULT_ARG(Hash()),
			 const EQ& eq RW_DEFAULT_ARG(EQ()));
  rw_hashmap(const_iterator first, const_iterator bound,
             size_type sz RW_DEFAULT_ARG(1024), const Hash& h RW_DEFAULT_ARG(Hash()), 
			 const EQ& eq RW_DEFAULT_ARG(EQ()));
  ~rw_hashmap() {} // mainly to document no (non-base) action

///// operators ////
#ifndef RW_ALLOCATOR_PLAIN
  rw_hashmap<K,V,Hash,EQ,A>&   operator=(const rw_hashmap<K,V,Hash,EQ,A>& t)
#else
  rw_hashmap<K,V,Hash,EQ>&     operator=(const rw_hashmap<K,V,Hash,EQ>& t)
#endif
  { impl = t.impl; return *this; }

  // tests both K and V in each pair. K is tested using EQ. V is tested
  // using operator ==() (which must therefore be defined for type V).
  // Takes slightly longer than the method equal_by_keys() below.
  bool
#ifndef RW_ALLOCATOR_PLAIN
    operator==(const rw_hashmap<K,V,Hash,EQ,A>& t) const;
#else
    operator==(const rw_hashmap<K,V,Hash,EQ>& t) const;
#endif

/*
 * Not to be used.  Present only to appease overacheiving compilers (the
 * ones that compile all member functions whether used or not).
 */
  bool
#ifndef RW_ALLOCATOR_PLAIN
    operator<(const rw_hashmap<K,V,Hash,EQ,A>&) const
#else
    operator<(const rw_hashmap<K,V,Hash,EQ>&) const
#endif
  { assert(false); return false; }

  V& operator[](const key_type& k);
  
  
  /*
   * const version of operator[] suspect.  Leave out for now.
   * (illegal to return reference to local or temporary if not found)
   *
   *  // returns V() if the key is not found.
   *  const V& operator[](const key_type& k) const
   *
   */
    
///// various public methods ////
  iterator
    begin() { return impl.begin(); }
  const_iterator
    begin() const { return impl.begin(); }

  iterator
    end() { return impl.end(); }
  const_iterator
    end() const { return impl.end(); }

  bool
    empty() const { return impl.empty(); }

  // Method equal_by_keys his is a fast and useful approximation to
  // operator==(). It tests that the two maps have the same set of K 
  // parts.  The V part of each stored pair is ignored.
  bool
#ifndef RW_ALLOCATOR_PLAIN
    equal_by_keys(const rw_hashmap<K,V,Hash,EQ,A>& t) const 
#else
    equal_by_keys(const rw_hashmap<K,V,Hash,EQ>& t) const 
#endif
    { return impl == t.impl; }

  size_type
    size() const { return impl.size(); }

  size_type
    count(const key_type& k) const
    { return find(k) == end() ? 0 : 1; }
      
  const_iterator
    find(const key_type& k) const
//    { pair<K,V> dum(k,V()); return impl.find(dum); }
    { value_type dum(k,V()); return impl.find(dum); }

  iterator
    find(const key_type& k)
//    { pair<K,V> dum(k,V()); return impl.find(dum); }
    { value_type dum(k,V()); return impl.find(dum); }

  iterator
    lower_bound(const key_type& k)
//    { pair<K,V> dum(k,V()); return impl.lower_bound(dum); }
    { value_type dum(k,V()); return impl.lower_bound(dum); }

  const_iterator
    lower_bound(const key_type& k) const
//    { pair<K,V> dum(k,V()); return impl.lower_bound(dum); }
    { value_type dum(k,V()); return impl.lower_bound(dum); }

  iterator
    upper_bound(const key_type& k)
//    { pair<K,V> dum(k,V()); return impl.upper_bound(dum); }
    { value_type dum(k,V()); return impl.upper_bound(dum); }

  const_iterator
    upper_bound(const key_type& k) const
//    { pair<K,V> dum(k,V()); return impl.upper_bound(dum); }
    { value_type dum(k,V()); return impl.upper_bound(dum); }

  RW_SL_STD(pair)<iterator,iterator>
    equal_range(const key_type& k)
//    { pair<K,V> dum(k,V()); return impl.equal_range(dum); }
    { value_type dum(k,V()); return impl.equal_range(dum); }

  RW_SL_STD(pair)<const_iterator,const_iterator>
    equal_range(const key_type& k) const
//    { pair<K,V> dum(k,V()); return impl.equal_range(dum); }
    { value_type dum(k,V()); return impl.equal_range(dum); }

  size_type
    capacity() const { return impl.capacity(); }

  float
    fill_ratio() const { return impl.fill_ratio(); }

  void
    resize(size_t n) { impl.resize(n); }
  void
#ifndef RW_ALLOCATOR_PLAIN
    swap(rw_hashmap<K,V,Hash,EQ,A>& t) { impl.swap(t.impl); }
#else
    swap(rw_hashmap<K,V,Hash,EQ>& t) { impl.swap(t.impl); }
#endif

///// insertion and erasure ////

  RW_SL_STD(pair)<iterator,bool>
    insert(const value_type& pr)
    {
#ifndef RW_ALLOCATOR_PLAIN
#  ifndef RW_NO_FQ_TYPENAME
      typename rw_hashmap<K,V,Hash,EQ,A>::iterator itr;
#  else
      iterator itr;
#  endif
      bool fits = impl.insertMaybe(pr,itr,true);
      return RW_SL_STD(pair)<rw_hashmap<K,V,Hash,EQ,A>::iterator,bool> (itr,fits);
#else
#  ifndef RW_NO_FQ_TYPENAME
      typename rw_hashmap<K,V,Hash,EQ>::iterator itr;
#  else
      iterator itr;
#  endif
      bool fits = impl.insertMaybe(pr,itr,true);
      return RW_SL_STD(pair)<typename rw_hashmap<K,V,Hash,EQ>::iterator,bool> (itr,fits);
#endif
    }
  
  iterator
    insert(iterator location_hint, const value_type& k)
    {
#ifndef RW_ALLOCATOR_PLAIN
#  ifndef RW_NO_FQ_TYPENAME
      typename rw_hashmap<K,V,Hash,EQ,A>::iterator ret;
#  else
      iterator ret;
#  endif
#else
#  ifndef RW_NO_FQ_TYPENAME
      typename rw_hashmap<K,V,Hash,EQ>::iterator ret;
#  else
      iterator ret;
#  endif
#endif
      if(! impl.insertMaybe(k,ret,true))
        ret = end();
      return ret;
    }

  size_type
    insert(const value_type* first, const value_type* bound)
    { iterator dum; return impl.insertMaybe(first,bound,dum,true); }
  
  size_type
    insert(const_iterator first, const_iterator bound)
    { iterator dum; return impl.insertMaybe(first,bound,dum,true); }

  size_type
    erase(const key_type& k)
//    { pair<K,V> dum(k,V()); return impl.erase(dum); }
    { value_type dum(k,V()); return impl.erase(dum); }

  iterator
    erase(iterator it) { return impl.erase(it); }

  iterator
    erase(iterator it, iterator bound) { return impl.erase(it,bound); }

  void
    clear() { impl.clear(); }

private:
#ifndef RW_ALLOCATOR_PLAIN
  LHashTable<value_type,h_k,eq_k,A> impl;
#else
  LHashTable<value_type,h_k,eq_k> impl;
#endif
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("hashmap.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/stdex/hashmap.cc"
#endif

#endif /* _STDEX_HASHMAP_H_ */
