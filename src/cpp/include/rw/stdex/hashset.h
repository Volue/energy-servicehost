#ifndef _STDEX_HASHSET_H_
#define _STDEX_HASHSET_H_
/***************************************************************************
 *
 * declaration for STL-like set based on hash table
 *
 * $Id: hashset.h 18623 2000-03-03 08:28:13Z sih $
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
template <class T, class Hash, class EQ, class A>
#else
template <class T, class Hash, class EQ>
#endif
class rw_hashset  {
public:
///// typdefs /////
  typedef T					key_type;
  typedef T					value_type;
  typedef Hash    				key_hash_type;
  typedef EQ					key_equal_type;
#ifndef RW_ALLOCATOR_PLAIN
  typedef LHashTable<T,Hash,EQ,A>		impl_type;
#else
  typedef LHashTable<T,Hash,EQ>			impl_type;
#endif
  typedef typename impl_type::const_reference	reference;
  typedef typename impl_type::const_reference	const_reference;
  typedef typename impl_type::const_iterator	iterator;
  typedef typename impl_type::const_iterator	const_iterator;
  typedef typename impl_type::size_type		size_type;
  typedef typename impl_type::difference_type	difference_type;
#ifdef RW_NO_TYPEDEF_ITERATOR
  typedef impl_type::const_iterator     	Iterator;
  typedef impl_type::const_iterator	        Const_Iterator;
#endif

  // see "lhash.h" for comments on key_compare:
  typedef typename impl_type::key_compare	key_compare;
  key_compare key_comp()                { return key_compare(); }

  key_hash_type key_hash()		{ return impl.key_hash(); }
  key_equal_type key_equal()		{ return impl.key_equal(); }




#ifdef RW_BROKEN_NAME_RESOLUTION
/* 
 *   The following system of macros allows a portion of a given ".cc" file
 *   to be used within a class body. This portion can alternatively be used
 *   to define templated function bodies in the normal fashion when needed.
 *   (See the second include of this same ".cc" file at the end of this 
 *   file).
 *   The remaining portion of the ".cc" file templates can only be used in the
 *   standard fashion. They are read during the include at the end of this 
 *   file.
 *   For more information see the comments at the beginning of the ".cc" file.
 *   Also, the methodology is commented in greatest detail in files slist.h and
 *   slist.cc. Study those files if further information is needed
 */
#define RW_GET_INLINES_FOR_CLASS_BODY
#include "rw/stdex/hashset.cc"
#undef RW_GET_INLINES_FOR_CLASS_BODY	  
#define RW_GOT_INLINES_FOR_CLASS_BODY_HASHSET_CC

#else
#  if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
rw_hashset(const value_type* first, const value_type* bound);
rw_hashset(const value_type* first, const value_type* bound,
	     size_type sz);
rw_hashset(const value_type* first, const value_type* bound,
	     size_type sz, 
		 const Hash& h);
#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */

  rw_hashset(const value_type* first, const value_type* bound,
	     size_type sz RW_DEFAULT_ARG(1024), 
		 const Hash& h RW_DEFAULT_ARG(Hash()), 
		 const EQ& eq RW_DEFAULT_ARG(EQ()) 
	    );


#  if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
rw_hashset(const_iterator first, const_iterator bound);
rw_hashset(const_iterator first, const_iterator bound,
	     size_type sz);
rw_hashset(const_iterator first, const_iterator bound,
	     size_type sz, 
		 const Hash& h);
#  endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  rw_hashset(const_iterator first, const_iterator bound,
	     size_type sz RW_DEFAULT_ARG(1024), 
		 const Hash& h RW_DEFAULT_ARG(Hash()), 
		 const EQ& eq RW_DEFAULT_ARG(EQ()) 
		);


iterator   insert(iterator hint, const_reference);

#endif // RW_BROKEN_NAME_RESOLUTION



///// constructors /////

#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  
  rw_hashset()
  {
	  impl = impl_type(size_type(1024),Hash(),EQ());
  }
  rw_hashset(size_type sz)
  {
	  impl = impl_type(sz,Hash(),EQ());
  }
  rw_hashset(size_type sz, const Hash& h)
  {
	  impl = impl_type(sz,h,EQ());
  }

#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  rw_hashset(
	     size_type sz RW_DEFAULT_ARG(1024),
	     const Hash& h RW_DEFAULT_ARG(Hash()), 
		 const EQ& eq RW_DEFAULT_ARG(EQ())
	     )
    : impl(sz,h,eq) {}

#ifndef RW_ALLOCATOR_PLAIN
  rw_hashset(const rw_hashset<T,Hash,EQ,A>& arg) : impl(arg.impl) {}
#else
  rw_hashset(const rw_hashset<T,Hash,EQ>& arg) : impl(arg.impl) {}
#endif



  ~rw_hashset() {} // mainly to document no (non-impl) action

///// operators ////
#ifndef RW_ALLOCATOR_PLAIN
  rw_hashset<T,Hash,EQ,A>&  operator=(const rw_hashset<T,Hash,EQ,A>& t)
#else
  rw_hashset<T,Hash,EQ>&    operator=(const rw_hashset<T,Hash,EQ>& t)
#endif
  { impl = t.impl; return *this; }

  bool
#ifndef RW_ALLOCATOR_PLAIN
    operator==(const rw_hashset<T,Hash,EQ,A>& t) const
#else
    operator==(const rw_hashset<T,Hash,EQ>& t) const
#endif
  { return impl == t.impl; }

/*
 * Not to be used.  Present only to appease overacheiving compilers (the
 * ones that compile all member functions whether used or not).
 */
  bool
#ifndef RW_ALLOCATOR_PLAIN
    operator<(const rw_hashset<T,Hash,EQ,A>&) const
#else
    operator<(const rw_hashset<T,Hash,EQ>&) const
#endif
  { assert(false); return false; }
    
///// various public methods ////
  iterator
    begin() const { return impl.begin(); }

  iterator
    end() const { return impl.end(); }

  bool
    empty() const { return impl.empty(); }

  size_type
    size() const { return impl.size(); }

  size_type
    count(const_reference key) const
    { return impl.find(key) == impl.end() ? 0 : 1; }

  iterator
    find(const_reference key) const { return impl.find(key); }

  iterator
    lower_bound(const_reference key) const { return impl.lower_bound(key); }

  iterator
    upper_bound(const_reference key) const { return impl.upper_bound(key); }

  RW_SL_STD(pair)<iterator,iterator>
    equal_range(const_reference key) const { return impl.equal_range(key); }

  size_type
    capacity() const { return impl.capacity(); }

  float
    fill_ratio() const { return impl.fill_ratio(); }

  void
    resize(size_type s) { impl.resize(s); }

  void
#ifndef RW_ALLOCATOR_PLAIN
    swap(rw_hashset<T,Hash,EQ,A>& t) { impl.swap(t.impl); }
#else
    swap(rw_hashset<T,Hash,EQ>& t) { impl.swap(t.impl); }
#endif

///// insertion and erasure ////

  RW_SL_STD(pair)<iterator,bool>
    insert(const_reference k)
{
  typename impl_type::Iterator itr;
  bool fits = impl.insertMaybe(k,itr,true);
#ifndef RW_ALLOCATOR_PLAIN
#  ifndef RW_NO_TYPENAME_OUTSIDE_DECLARATION
   return RW_SL_STD(pair)<typename rw_hashset<T,Hash,EQ,A>::iterator,bool> (iterator(itr),fits);
#  else
   return RW_SL_STD(pair)<rw_hashset<T,Hash,EQ,A>::iterator,bool> (iterator(itr),fits);
#  endif
#else
#  ifndef RW_NO_TYPENAME_OUTSIDE_DECLARATION
   return RW_SL_STD(pair)<typename rw_hashset<T,Hash,EQ>::iterator,bool> (iterator(itr),fits);
#  else
   return RW_SL_STD(pair)<rw_hashset<T,Hash,EQ>::iterator,bool> (iterator(itr),fits);
#  endif 
#endif
}


  size_type
    insert(const value_type* first, const value_type* bound)
    {typename impl_type::Iterator dum; return impl.insertMaybe(first,bound,dum,true); }

  size_type
    insert(const_iterator first, const_iterator bound)
    {typename  impl_type::Iterator dum; return impl.insertMaybe(first,bound,dum,true); }

  size_type
    erase(const_reference t) { return impl.erase(t); }

  iterator
	  erase(iterator it) 
		{
// the following implementation represent a temporary fix to allow the regression
//	test vhset to continue working. There is a question as to whether erase
//  should be defined upon const iterators and whether iterator and const_iterator should
//  remain both typedef'ed as const_iterators, as they are in this class.
//  See bug #8687
		return impl.erase(impl.find(*it));
		}

  iterator
    erase(iterator it, iterator bound) { return impl.erase(it,bound); }

  void clear() { impl.clear(); }

private:
#ifndef RW_ALLOCATOR_PLAIN
  LHashTable<T,Hash,EQ,A> impl;
#else
  LHashTable<T,Hash,EQ> impl;
#endif
};




#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("hashset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE

/*
 *   This is (possibly) the second include of the same ".cc" file. It 
 *   may have been included fron inside the class body above. Please see the
 *   comments in the class body above and at the beginning of the ".cc" file
 *   if you need to further understand what is accomplished with these macros.
 */

#include "rw/stdex/hashset.cc"
#endif



#endif /* _STDEX_HASHSET_H_ */
