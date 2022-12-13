#ifndef _STDEX_HASHMMAP_H_
#define _STDEX_HASHMMAP_H_
/***************************************************************************
 *
 * declaration for STL-like multimap based on hash table
 *
 * $Id: hashmmap.h 18623 2000-03-03 08:28:13Z sih $
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
class rw_hashmultimap
{
public:
#ifndef RWSTD_NO_CONST_INST
  typedef RW_SL_STD(pair)<const K, V>               value_type;
#else
  typedef RW_SL_STD(pair)<K,V>                      value_type;
#endif

private:
  struct h_k {
    Hash h_;
    h_k(const Hash& h = Hash()) : h_(h) {}
    inline unsigned long operator()(const value_type& p) const {
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
public:
///// typdefs /////

#ifndef RW_ALLOCATOR_PLAIN
  typedef LHashTable<value_type,h_k,eq_k,A> impl_type;
#else
  typedef LHashTable<value_type,h_k,eq_k> impl_type;
#endif
  typedef K                             key_type;
  //typedef pair<...K,V>                value_type  defined above
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

  key_hash_type key_hash() const        { return Hash(); }
  key_equal_type key_equal() const      { return EQ(); }
  value_hash_type value_hash() const    { return impl.key_hash(); }
  value_equal_type value_equal() const  { return impl.key_equal(); }
    
///// constructors /////
#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  rw_hashmultimap()
  {
    impl = impl_type(size_type(1024), value_hash_type(Hash()), value_equal_type(EQ()));
  }
  rw_hashmultimap(size_type sz)
  {
    impl = impl_type(sz, value_hash_type(Hash()), value_equal_type(EQ()));
  }
  rw_hashmultimap(size_type sz, const Hash& h)
  {
    impl = impl_type(sz, value_hash_type(h), value_equal_type(EQ()));
  }

  rw_hashmultimap(const value_type* first, const value_type* bound);
  rw_hashmultimap(const value_type* first, const value_type* bound,
             size_type sz
			 );
  rw_hashmultimap(const value_type* first, const value_type* bound,
             size_type sz, const Hash& h
			 );

  rw_hashmultimap(const_iterator first, const_iterator bound);
  rw_hashmultimap(const_iterator first, const_iterator bound,
             size_type sz
			 );
  rw_hashmultimap(const_iterator first, const_iterator bound,
             size_type sz, const Hash& h
			 );
#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  rw_hashmultimap(size_type sz RW_DEFAULT_ARG(1024),
	              const Hash& h RW_DEFAULT_ARG(Hash()),
				  const EQ& eq RW_DEFAULT_ARG(EQ()))
    : impl(sz,value_hash_type(h),value_equal_type(eq)) {}
#ifndef RW_ALLOCATOR_PLAIN
  rw_hashmultimap(const rw_hashmultimap<K,V,Hash,EQ,A>& arg) : impl(arg.impl) {}
#else
  rw_hashmultimap(const rw_hashmultimap<K,V,Hash,EQ>& arg) : impl(arg.impl) {}
#endif
  rw_hashmultimap(const value_type* first, const value_type* bound,
             size_type sz RW_DEFAULT_ARG(1024),
			 const Hash& h RW_DEFAULT_ARG(Hash()),
			 const EQ& eq RW_DEFAULT_ARG(EQ())
             );
  rw_hashmultimap(const_iterator first, const_iterator bound,
             size_type sz RW_DEFAULT_ARG(1024),
			 const Hash& h RW_DEFAULT_ARG(Hash()),
			 const EQ& eq RW_DEFAULT_ARG(EQ())
            );
  ~rw_hashmultimap() {} // mainly to document no (non-base) action

///// operators ////
#ifndef RW_ALLOCATOR_PLAIN
  rw_hashmultimap<K,V,Hash,EQ,A>& operator=(const rw_hashmultimap<K,V,Hash,EQ,A>& t)
#else
  rw_hashmultimap<K,V,Hash,EQ>&   operator=(const rw_hashmultimap<K,V,Hash,EQ>& t)
#endif
  { impl = t.impl; return *this; }

  // tests both K and V in each pair. K is tested using EQ. V is tested
  // using operator==() (which must therefore be defined for type V).
  // May take significantly longer than the method equal_by_keys() below.
  bool
#ifndef RW_ALLOCATOR_PLAIN
    operator==(const rw_hashmultimap<K,V,Hash,EQ,A>& t) const;
#else
    operator==(const rw_hashmultimap<K,V,Hash,EQ>& t) const;
#endif

/*
 * Not to be used.  Present only to appease overacheiving compilers (the
 * ones that compile all member functions whether used or not).
 */
  bool
#ifndef RW_ALLOCATOR_PLAIN
    operator<(const rw_hashmultimap<K,V,Hash,EQ,A>&) const
#else
    operator<(const rw_hashmultimap<K,V,Hash,EQ>&) const
#endif
  { assert(false); return false; }

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

  size_type
    size() const { return impl.size(); }

  size_type
    count(const key_type& k) const
      { value_type dum(k,V()); return impl.count(dum); }

  const_iterator
    find(const key_type& k) const
    { value_type dum(k,V()); return impl.find(dum); }

  iterator
    find(const key_type& k)
    { value_type dum(k,V()); return impl.find(dum); }

  iterator
    lower_bound(const key_type& k)
    { value_type dum(k,V()); return impl.lower_bound(dum); }

  const_iterator
    lower_bound(const key_type& k) const
    { value_type dum(k,V()); return impl.lower_bound(dum); }

  iterator
    upper_bound(const key_type& k)
    { value_type dum(k,V()); return impl.upper_bound(dum); }

  const_iterator
    upper_bound(const key_type& k) const
    { value_type dum(k,V()); return impl.upper_bound(dum); }

  RW_SL_STD(pair)<iterator,iterator>
    equal_range(const key_type& k)
    { value_type dum(k,V()); return impl.equal_range(dum); }

  RW_SL_STD(pair)<const_iterator,const_iterator>
    equal_range(const key_type& k) const
    { value_type dum(k,V()); return impl.equal_range(dum); }

  size_type
    capacity() const { return impl.capacity(); }

  // Method equal_by_keys is a fast and useful approximation to
  // operator==(): It tests that the two maps have the same set of K
  // parts.  The V part of each stored pair is ignored.
  bool
#ifndef RW_ALLOCATOR_PLAIN
    equal_by_keys(const rw_hashmultimap<K,V,Hash,EQ,A>& t) const 
#else
    equal_by_keys(const rw_hashmultimap<K,V,Hash,EQ>& t) const 
#endif
    { return impl == t.impl; }

  float
    fill_ratio() const { return impl.fill_ratio(); }

  void
    resize(size_t n) { impl.resize(n); }
  void
#ifndef RW_ALLOCATOR_PLAIN
    swap(rw_hashmultimap<K,V,Hash,EQ,A>& t) { impl.swap(t.impl); }
#else
    swap(rw_hashmultimap<K,V,Hash,EQ>& t) { impl.swap(t.impl); }
#endif

///// insertion and erasure ////

  RW_SL_STD(pair)<iterator,bool>
    insert(const value_type& pr)
{
#ifndef RW_ALLOCATOR_PLAIN
#  ifndef RW_NO_FQ_TYPENAME
  typename rw_hashmultimap<K,V,Hash,EQ,A>::iterator itr;
#  else
  iterator itr;
#  endif
  bool fits = impl.insertMaybe(pr,itr,false);
  return RW_SL_STD(pair)<rw_hashmultimap<K,V,Hash,EQ,A>::iterator,bool> (itr,fits);
#else
#  ifndef RW_NO_FQ_TYPENAME
  typename rw_hashmultimap<K,V,Hash,EQ>::iterator itr;
#  else
  iterator itr;
#  endif
  bool fits = impl.insertMaybe(pr,itr,false);
  return RW_SL_STD(pair)<iterator,bool> (itr,fits);
#endif
}

  iterator
    insert(iterator location_hint, const value_type& k)
{
#ifndef RW_ALLOCATOR_PLAIN
#  ifndef RW_NO_FQ_TYPENAME
  typename rw_hashmultimap<K,V,Hash,EQ,A>::iterator ret;
#  else
  iterator ret;
#  endif
#else
#  ifndef RW_NO_FQ_TYPENAME
  typename rw_hashmultimap<K,V,Hash,EQ>::iterator ret;
#  else
  iterator ret;
#  endif
#endif
  if(! impl.insertMaybe(k,ret,false))
    ret = end();
  return ret;
}

  size_type
    insert(const value_type* first, const value_type* bound)
    { iterator dum; return impl.insertMaybe(first,bound,dum,false); }

  size_type
    insert(const_iterator first, const_iterator bound)
    { iterator dum; return impl.insertMaybe(first,bound,dum,false); }

  size_type
    erase(const key_type& k)
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
#pragma implementation ("hashmmap.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/stdex/hashmmap.cc"
#endif

#endif /* _STDEX_HASHMMAP_H_ */
