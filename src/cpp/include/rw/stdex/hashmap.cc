/***************************************************************************
 *
 * Implementation for STL-like map based on a hash table
 *
 * $Id: hashmap.cc 18623 2000-03-03 08:28:13Z sih $
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
 *
 */

#undef FORMALTYPES
#undef TYPES
#ifndef RW_ALLOCATOR_PLAIN
#  define FORMALTYPES class K, class V, class Hash, class EQ, class A
#  define TYPES K,V,Hash,EQ,A
#else
#  define FORMALTYPES class K, class V, class Hash, class EQ
#  define TYPES K,V,Hash,EQ
#endif

#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
template <FORMALTYPES>
rw_hashmap<TYPES>::rw_hashmap(
			  const typename rw_hashmap<TYPES>::value_type* first,
			  const typename rw_hashmap<TYPES>::value_type* bound
			  )
  {
    
    impl = impl_type(size_type(1024), value_hash_type(Hash()), value_equal_type(EQ()));
	  
    iterator dum;
    while(first != bound)
      impl.insertMaybe(*first++,dum,true);
  } 
template <FORMALTYPES>
rw_hashmap<TYPES>::rw_hashmap(
			  const typename rw_hashmap<TYPES>::value_type* first,
			  const typename rw_hashmap<TYPES>::value_type* bound,
			  size_type sz
			  )
  {
    impl = impl_type(sz, value_hash_type(Hash()), value_equal_type(EQ()));
    iterator dum;
    while(first != bound)
      impl.insertMaybe(*first++,dum,true);
  }

template <FORMALTYPES>
rw_hashmap<TYPES>::rw_hashmap(
			  const typename rw_hashmap<TYPES>::value_type* first,
			  const typename rw_hashmap<TYPES>::value_type* bound,
			  size_type sz,
			  const Hash& h)
  {
    impl = impl_type(sz, value_hash_type(h), value_equal_type(EQ()));
    iterator dum;
    while(first != bound)
      impl.insertMaybe(*first++,dum,true);
  }

template <FORMALTYPES>
rw_hashmap<TYPES>::rw_hashmap(
			      const_iterator first,
			      const_iterator bound
			      )
  { 
    impl = impl_type(size_type(1024), value_hash_type(Hash()), value_equal_type(EQ()));
    iterator dum;
    impl.insertMaybe(first,bound,dum,true);
  }

template <FORMALTYPES>
rw_hashmap<TYPES>::rw_hashmap(
			      const_iterator first,
			      const_iterator bound,
			      size_type sz
				  )
  {
    impl = impl_type(sz, value_hash_type(Hash()), value_equal_type(EQ()));
    iterator dum;
    impl.insertMaybe(first,bound,dum,true);
  } 
template <FORMALTYPES>
rw_hashmap<TYPES>::rw_hashmap(
			      const_iterator first,
			      const_iterator bound,
			      size_type sz,
			      const Hash& h
			     )
  {
    impl = impl_type(sz, value_hash_type(h), value_equal_type(EQ()));
    iterator dum;
    impl.insertMaybe(first,bound,dum,true);
  } 
#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
template <FORMALTYPES>
rw_hashmap<TYPES>::rw_hashmap(
			  const typename rw_hashmap<TYPES>::value_type* first,
			  const typename rw_hashmap<TYPES>::value_type* bound,
			  size_type sz,
			  const Hash& h, const EQ& eq
				 )
	: impl(sz,value_hash_type(h),value_equal_type(eq))
{
  iterator dum;
  while(first != bound)
    impl.insertMaybe(*first++,dum,true);
}

template <FORMALTYPES>
rw_hashmap<TYPES>::rw_hashmap(
			      const_iterator first,
			      const_iterator bound,
			      size_type sz,
			      const Hash& h, const EQ& eq
			     )
	: impl(sz,value_hash_type(h),value_equal_type(eq))
{
  iterator dum;
  impl.insertMaybe(first,bound,dum,true);
}

template <FORMALTYPES>
V& rw_hashmap<TYPES>::operator[](const K& k)
{
  value_type t(k,V());
  return (*((insert(t)).first)).second;
}

#if 0 
// const version of operator[] not implemented for now
template <FORMALTYPES>
const V&
rw_hashmap<TYPES>::operator[](const K& k) const
{
  V ret;
  value_type t(k,V());
  iterator hit = impl.find(k);
  if(hit != end())
    ret = (*hit).second;
  return ret;
}
#endif

template <FORMALTYPES>
bool
rw_hashmap<TYPES>::operator==(const rw_hashmap<TYPES>& m) const
{
  if(size() == m.size())		// must be same size
  {
    const_iterator i = begin();
    while(i != end()) {
      const_iterator j = m.find((*i).first);
      if(j == m.end())			// must have same keys
	return false;
      // note use of op==() below. No relationship to EQ
      if(! ((*j).second == (*i).second)) // and same values
	return false;
      ++i;
    }
    return true;
  }
  return false;
}
    
#if 0
// make inline for Borland 4.5
template <FORMALTYPES>
pair<rw_hashmap<TYPES>::iterator,bool>
rw_hashmap<TYPES>::insert(const value_type& pr)
{
  rw_hashmap<TYPES>::iterator itr;
  bool fits = impl.insertMaybe(pr,itr,true);
  return pair<rw_hashmap<TYPES>::iterator,bool> (itr,fits);
}
#endif

#if 0
// make inline for Borland 4.5
template <FORMALTYPES>
rw_hashmap<TYPES>::iterator
rw_hashmap<TYPES>::insert(
			  iterator, /*ignore*/
			  const rw_hashmap<TYPES>::value_type& k
			 )
{
  rw_hashmap<TYPES>::iterator ret;
  if(! impl.insertMaybe(k,ret,true))
    ret = end();
  return ret;
}
#endif
#undef TYPES
#undef FORMALTYPES


