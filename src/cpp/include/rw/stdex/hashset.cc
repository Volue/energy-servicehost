/***************************************************************************
 *
 * Implementation for STL-like set based on a hash table
 *
 * $Id: hashset.cc 18623 2000-03-03 08:28:13Z sih $
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



#undef FORMALTYPES
#undef TYPES
#ifndef RW_ALLOCATOR_PLAIN
#  define FORMALTYPES class T, class Hash, class EQ, class A
#  define TYPES T,Hash,EQ,A
#else
#  define FORMALTYPES class T, class Hash, class EQ
#  define TYPES T,Hash,EQ
#endif



/* 
 *  PLEASE NOTE THE ABNORMAL STRUCTURE OF THIS FILE
 *
 *  The file has two major sections. The first section (between the #ifdef and
 *  the #endif for RW_SHUT_INLINES_GUARD_OFF...) contains templated class
 *  member functions which are expressed in such a way that they can be included
 *  either from within a class body or as "inlines" from outside the class body.
 *  The "INLINES_GAURD" assures that they are never seen both ways.
 *
 *  The second section of this file (between the #ifndef and the #endif of the
 *  macro RW_GET_INLINES_FOR_CLASS BODY contains the remaining templated class
 *  member functions which can only be seen from outside the class body.
 *
 *  Please modify or add new member definitions to this file with this scheme in
 *  mind. Placing new members in the wrong section will most assuredly break this
 *  scheme. If members can ever be seen from within a class body they must be placed
 *  in the first section.
 *
 *  NOTE: This scheme is commented in greatest detail in files slist.cc and slist.h.
 */

/*  
 *  This section of the include file is for members that might in some cases be 
 *  included from within their own class bodies.  Their function signatures and 
 *  template designators are constructed with macros whose expansion depends
 *  upon the macro RW_GET_INLINES_FOR_CLASS_BODY. (see below)
 *
 *  The guard RW_SHUT_INLINES_GUARD_OFF__SLIST_CC helps to assure that the
 *  compiler will NOT see this section twice, once from within a class body
 *  and once from outside the class.
 */
#ifdef RW_GET_INLINES_FOR_CLASS_BODY
#   define RW_TEMPLATE_DECLTR__RW_HASHSET
#   define RW_SCOPE_DECLTR__RW_HASHSET
#   define RW_CNDTNL_TYPENAME__RW_HASHSET
#else
#   define RW_TEMPLATE_DECLTR__RW_HASHSET template<FORMALTYPES>
#   define RW_SCOPE_DECLTR__RW_HASHSET rw_hashset<TYPES>::
#   define RW_CNDTNL_TYPENAME__RW_HASHSET typename
#endif


#ifndef RW_GOT_INLINES_FOR_CLASS_BODY_HASHSET_CC
#  if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
RW_TEMPLATE_DECLTR__RW_HASHSET
 RW_SCOPE_DECLTR__RW_HASHSET  rw_hashset(
			  const  value_type* first,
			  const  value_type* bound
			  )
{
  impl = impl_type(size_type(1024),Hash(),EQ());
  typename impl_type::Iterator dum;
  while(first != bound)
    impl.insertMaybe(*first++,dum,true);
}

RW_TEMPLATE_DECLTR__RW_HASHSET
RW_SCOPE_DECLTR__RW_HASHSET rw_hashset(
			  const typename RW_SCOPE_DECLTR__RW_HASHSET value_type* first,
			  const typename RW_SCOPE_DECLTR__RW_HASHSET value_type* bound,
			  size_type sz
			  )
{
  impl = impl_type(sz,Hash(),EQ());
  typename impl_type::Iterator dum;
  while(first != bound)
    impl.insertMaybe(*first++,dum,true);
}

RW_TEMPLATE_DECLTR__RW_HASHSET
RW_SCOPE_DECLTR__RW_HASHSET rw_hashset(
			  const typename RW_SCOPE_DECLTR__RW_HASHSET value_type* first,
			  const typename RW_SCOPE_DECLTR__RW_HASHSET value_type* bound,
			  size_type sz,
			  const Hash& h
			  )
{
  impl = impl_type(sz,h,EQ());
  typename impl_type::Iterator dum;
  while(first != bound)
    impl.insertMaybe(*first++,dum,true);
}

#  endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */

RW_TEMPLATE_DECLTR__RW_HASHSET
RW_SCOPE_DECLTR__RW_HASHSET rw_hashset(
			  const typename RW_SCOPE_DECLTR__RW_HASHSET value_type* first,
			  const typename RW_SCOPE_DECLTR__RW_HASHSET value_type* bound,
#if defined (__GNUG__)			  
			  size_type sz ,
			  const Hash& h ,
			  const EQ& eq 
#else
			  size_type sz RW_DEFAULT_ARG(1024),
			  const Hash& h RW_DEFAULT_ARG(Hash()),
			  const EQ& eq RW_DEFAULT_ARG(EQ())
#endif
				 )
	: impl(sz,h,eq)
{
  typename impl_type::Iterator dum;
  while(first != bound)
    impl.insertMaybe(*first++,dum,true);
}



#  if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
RW_TEMPLATE_DECLTR__RW_HASHSET
RW_SCOPE_DECLTR__RW_HASHSET rw_hashset(
			      const_iterator first,
			      const_iterator bound
				  )
{
  impl = impl_type(size_type(1024),Hash(),EQ());
  typename impl_type::Iterator dum;
  impl.insertMaybe(first,bound,dum,true);
}


RW_TEMPLATE_DECLTR__RW_HASHSET
RW_SCOPE_DECLTR__RW_HASHSET rw_hashset(
			      const_iterator first,
			      const_iterator bound,
			      size_type sz
				  )
{
  impl = impl_type(sz,Hash(),EQ());
  typename impl_type::Iterator dum;
  impl.insertMaybe(first,bound,dum,true);
}

RW_TEMPLATE_DECLTR__RW_HASHSET
RW_SCOPE_DECLTR__RW_HASHSET rw_hashset(
			      const_iterator first,
			      const_iterator bound,
			      size_type sz,
			      const Hash& h
				  )
{
  impl = impl_type(sz,h,EQ());
  typename impl_type::Iterator dum;
  impl.insertMaybe(first,bound,dum,true);
}
#  endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */


RW_TEMPLATE_DECLTR__RW_HASHSET
RW_SCOPE_DECLTR__RW_HASHSET rw_hashset(
			      const_iterator first,
			      const_iterator bound,
#if defined (__GNUG__)
			      size_type sz ,
			      const Hash& h ,
				  const EQ& eq 
#else
			      size_type sz RW_DEFAULT_ARG(1024),
			      const Hash& h RW_DEFAULT_ARG(Hash()),
				  const EQ& eq RW_DEFAULT_ARG(EQ())
#endif
			     )
	: impl(sz,h,eq)
{
  typename impl_type::Iterator dum;
  impl.insertMaybe(first,bound,dum,true);
}


RW_TEMPLATE_DECLTR__RW_HASHSET
#ifndef RW_NO_TYPEDEF_ITERATOR
typename RW_SCOPE_DECLTR__RW_HASHSET iterator
RW_SCOPE_DECLTR__RW_HASHSET insert(iterator /*ignore*/, const_reference k)
#else
RW_SCOPE_DECLTR__RW_HASHSET Iterator
RW_SCOPE_DECLTR__RW_HASHSET insert(Iterator /*ignore*/, const_reference k)
#endif
{
  typename impl_type::iterator ret;
  if(! impl.insertMaybe(k,ret,true))
    ret = impl.end();
  return iterator(ret);
}

#endif // RW_SHUT_INLINES_GUARD_OFF__HASHSET_CC



/*
 *  The remainder of this file is for member definitions that will never 
 *  be seen from inside a class body.
 */


#undef TYPES
#undef FORMALTYPES
#undef RW_TEMPLATE_DECLTR__RW_HASHSET
#undef RW_SCOPE_DECLTR__RW_HASHSET
#undef RW_CNDTNL_TYPENAME__RW_HASHSET
