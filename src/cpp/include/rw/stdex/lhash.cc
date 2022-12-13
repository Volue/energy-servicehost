/***************************************************************************
 *
 * implement hash table of iterators into an slist
 *
 * $Id: lhash.cc 18623 2000-03-03 08:28:13Z sih $
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

#undef VECSIZE
#define VECSIZE sizeof(RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)>)
#undef LISTSIZE
#define LISTSIZE sizeof(rw_slist<node_t>)


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
#   define RW_TEMPLATE_DECLTR__LHASHTABLE
#   define RW_SCOPE_DECLTR__LHASHTABLE
#   define RW_CNDTNL_TYPENAME__LHASHTABLE
#else
#   define RW_TEMPLATE_DECLTR__LHASHTABLE template<FORMALTYPES>
#   define RW_SCOPE_DECLTR__LHASHTABLE LHashTable<TYPES>::
#   define RW_CNDTNL_TYPENAME__LHASHTABLE typename
#endif

#ifndef RW_GOT_INLINES_FOR_CLASS_BODY_LHASH_CC

#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)

RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable()
: table_(0), list_(0)

{
  RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz = 1024;
  hash_ = Hash();
  eq_ = EQ();
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t, list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
}


RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(
		RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz)
: table_(0), list_(0)

{
  hash_ = Hash();
  eq_ = EQ();
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t, list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
}



RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(
		RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz,
			      const Hash& h)
: table_(0), list_(0), hash_(h)

{
  eq_ = EQ();
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t, list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
}



#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */

RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(
			 RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz RW_DEFAULT_ARG(1024),
#if defined (__GNUG__)
             const Hash& h ,
             const EQ&   equals )
#else
             const Hash& h RW_DEFAULT_ARG(Hash()),
             const EQ&   equals RW_DEFAULT_ARG(EQ()))
#endif
: table_(0), list_(0), hash_(h), eq_(equals)
{
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t, list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,sizeof(rw_slist<node_t>),0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,sizeof(RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)>),0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
}





#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)

RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(
			      ConstIterator first, ConstIterator bound)
: table_(0), list_(0)
{
  RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz = 1024;
  hash_ = Hash();
  eq_ = EQ();
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t,list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
  Iterator dum;
  insertMaybe(first,bound,dum,false); // assume donor was multi
}


RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(
			      ConstIterator first, ConstIterator bound,
				  RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz)
: table_(0), list_(0)
{
  hash_ = Hash();
  eq_ = EQ();
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t,list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
  Iterator dum;
  insertMaybe(first,bound,dum,false); // assume donor was multi
}


RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(
			      ConstIterator first, ConstIterator bound,
				  RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz,
				  const Hash& h)
: table_(0), list_(0), hash_(h)
{
  eq_ = EQ();
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t,list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
  Iterator dum;
  insertMaybe(first,bound,dum,false); // assume donor was multi
}

#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */

RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(ConstIterator first, ConstIterator bound,
			RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz RW_DEFAULT_ARG(1024),
#if defined (__GNUG__)
            const Hash& h ,
			const EQ& equals 
#else
            const Hash& h RW_DEFAULT_ARG(Hash()),
			const EQ& equals RW_DEFAULT_ARG(EQ())
#endif
           )
: table_(0), list_(0), hash_(h), eq_(equals)
{
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t,list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,sizeof(rw_slist<node_t>),0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,sizeof(RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)>),0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
  Iterator dum;
  insertMaybe(first,bound,dum,false); // assume donor was multi
}





#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)

RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(
			      const T* first, const T* bound
				  )
: table_(0), list_(0)
{
  RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz = 1024;
  hash_ = Hash();
  eq_ = EQ();
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t,list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
  Iterator dum;
  insertMaybe(first,bound,dum,false); // assume donor was multi
}


RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(
			      const T* first, const T* bound,
				  RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz
				  )
: table_(0), list_(0)
{
  hash_ = Hash();
  eq_ = EQ();
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t,list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
  Iterator dum;
  insertMaybe(first,bound,dum,false); // assume donor was multi
}


RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(
			      const T* first, const T* bound,
				  RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz,
				  const Hash& h
				  )
: table_(0), list_(0), hash_(h)
{
  eq_ = EQ();
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t,list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
  Iterator dum;
  insertMaybe(first,bound,dum,false); // assume donor was multi
}


#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */



RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(const T* first, const T* bound,
			RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type sz RW_DEFAULT_ARG(1024),
#if defined(__GNUG__)
            const Hash& h ,
			const EQ& equals 
#else
            const Hash& h RW_DEFAULT_ARG(Hash()),
			const EQ& equals RW_DEFAULT_ARG(EQ())
#endif
           )
: table_(0), list_(0), hash_(h), eq_(equals)
{
// use placement new...
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t,list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (sz); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,sizeof(rw_slist<node_t>),0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,sizeof(RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)>),0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (sz); 
#endif
  Iterator dum;
  insertMaybe(first,bound,dum,false); // assume donor was multi
}




RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE LHashTable(const LHashTable<TYPES>& t)
#ifndef RW_BROKEN_INITIALIZER
: table_(0), list_(0), hash_(t.hash_), eq_(t.eq_)
#else
: table_(0), list_(0)
#endif
{
#ifdef RW_BROKEN_INITIALIZER
hash_ = t.hash_;
eq_ = t.eq_;
#endif
#ifndef RW_ALLOCATOR_PLAIN
  list_ =
    new (RW_ALLOC_TYPE(list_t,listAllocator,listAllocator,1,0))
	 rw_slist<node_t,list_node_allocator_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,1,0))
	  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> (t.capacity()); 
#else
  list_ =
    new (RW_ALLOC_TYPE(rw_slist<node_t>,listAllocator,listAllocator,LISTSIZE,0))
	 rw_slist<node_t>;
  table_ =
    new (RW_ALLOC_TYPE(table_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> (t.capacity()); 
#endif
  Iterator dum;
  insertMaybe(t.begin(),t.end(),dum,false);
 }




RW_TEMPLATE_DECLTR__LHASHTABLE
RW_SCOPE_DECLTR__LHASHTABLE ~LHashTable()
{
  clear();
  rw_destroy(table_);

#ifndef RW_ALLOCATOR_PLAIN
  RW_DEALLOC_TYPE(vectorAllocator,vectorAllocator,table_,1);
#else
  RW_DEALLOC_TYPE(vectorAllocator,vectorAllocator,table_,VECSIZE);
#endif

  rw_destroy(list_);

#ifndef RW_ALLOCATOR_PLAIN
  RW_DEALLOC_TYPE(listAllocator,listAllocator,list_,1);
#else
  RW_DEALLOC_TYPE(listAllocator,listAllocator,list_,LISTSIZE);
#endif
}



RW_TEMPLATE_DECLTR__LHASHTABLE
RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE size_type
RW_SCOPE_DECLTR__LHASHTABLE erase(
				RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE const_reference key)
{
  size_type ret = size();
  erase(equal_range(key));
  return ret - size();
}


RW_TEMPLATE_DECLTR__LHASHTABLE                                                               
RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE ConstIterator                                                  
RW_SCOPE_DECLTR__LHASHTABLE erase(RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE ConstIterator iter)                   
{                                                                                     
  if(iter == end())                                                                   
    return iter;                                                                      
  // else                                                                             
  typename list_t::iterator kill = iter.iter_;                                        
	LHashTable_index_type_wrapper u; u.l = (*kill).second;
  typename list_t::iterator killnext = kill; ++killnext;                              
  typename table_t::iterator ti = index(u);                              
  typename list_t::iterator slot = (*ti).chain();                                     
  size_type kbucket = (*kill).second%capacity();                                      
  if(slot == kill) // erasing first item in slot                                      
  {                                                                                   
    // does slot have another item after us?                                          
    if(killnext != list_->end() && (*killnext).second%capacity() == kbucket)          
    {                                                                                 
      (*ti).chain() = killnext = list_->erase(kill);                                  
    }                                                                                 
    else // slot had only one item. invalidate slot, fixup next in list.              
    {                                                                                 
      (*ti).valid() = false;                                                          
      killnext = list_->erase(kill);                                                  
      if(killnext != list_->end()) { 
      	u.l = (*killnext).second;
				(*index(u)).chain() = killnext;                                   
      }                                                                               
    }                                                                                 
  }                                                                                   
  else // erasing item not first in slot                                              
  {                                                                                   
    // does slot have another item after us?                                          
    if(killnext != list_->end() && (*killnext).second%capacity() == kbucket)          
    {                                                                                 
      killnext = list_->erase(kill);                                                  
    }                                                                                 
    else // erasing last item in slot. fixup next                                     
    {                                                                                 
      killnext = list_->erase(kill);                                                  
      if(killnext != list_->end()) {                                                  
      	u.l = (*killnext).second;
				(*index(u)).chain() = killnext;                                   
      }                                                                               
    }                                                                                 
  }                                                                                   
  return killnext;                                                                    
}                                                                                     


RW_TEMPLATE_DECLTR__LHASHTABLE
RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE Iterator
RW_SCOPE_DECLTR__LHASHTABLE erase(RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE Iterator iter)
{
  if(iter == end())
    return iter;
  // else
  typename list_t::iterator kill = iter.iter_;
	LHashTable_index_type_wrapper u; u.l = (*kill).second;
  typename list_t::iterator killnext = kill; ++killnext;
  typename table_t::iterator ti = index(u);
  typename list_t::iterator slot = (*ti).chain();
  size_type kbucket = (*kill).second%capacity();
  if(slot == kill) // erasing first item in slot
  {
    // does slot have another item after us?
    if(killnext != list_->end() && (*killnext).second%capacity() == kbucket)
    {
      (*ti).chain() = killnext = list_->erase(kill);
    }
    else // slot had only one item. invalidate slot, fixup next in list.
    {
      (*ti).valid() = false;
      killnext = list_->erase(kill);
      if(killnext != list_->end()) {
      	u.l = (*killnext).second;
				(*index(u)).chain() = killnext;                                   
      }
    }
  }
  else // erasing item not first in slot
  {
    // does slot have another item after us?
    if(killnext != list_->end() && (*killnext).second%capacity() == kbucket) 
    {
      killnext = list_->erase(kill);
    }
    else // erasing last item in slot. fixup next
    {
      killnext = list_->erase(kill);
      if(killnext != list_->end()) {
      	u.l = (*killnext).second;
				(*index(u)).chain() = killnext;                                   
      }
    }
  }
  return killnext;
}


RW_TEMPLATE_DECLTR__LHASHTABLE
RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE Iterator
RW_SCOPE_DECLTR__LHASHTABLE erase(
			 RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE Iterator first,
			 RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE Iterator bound
			)
{
  Iterator iter;
  
  if (first != bound) {
    if(bound == end()) {
      while(first != end()) {
	first = erase(first);
      }
    }
    else {
      /*
       * erasing the element to the left of bound invalidates bound,
       * so we have to look ahead and do one last erase after the while loop.
       */
      iter = first;
      ++iter;
      while(iter != bound) {
	first = erase(first);
	iter = first;
	++iter;
      }
      first = erase(first);
    }
  }
  return first;
}



RW_TEMPLATE_DECLTR__LHASHTABLE
RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE ConstIterator
RW_SCOPE_DECLTR__LHASHTABLE erase(
			 RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE ConstIterator first,
			 RW_CNDTNL_TYPENAME__LHASHTABLE RW_SCOPE_DECLTR__LHASHTABLE ConstIterator bound
			)
{
// the following two lines represent a temporary fix to allow the regression
//	test vhset to continue working. There is a question as to whether erase
//  should be defined upon const iterators. See bug #8687. RVZ
//  Iterator first(arg_first.iter_);
//  Iterator bound(arg_bound.iter_);  // let's see if these can go???
  ConstIterator iter;
  
  if (first != bound) {
    if(bound == end()) {
      while(first != end()) {
        first = erase(first);
      }
    }
    else {
      /*
       * erasing the element to the left of bound invalidates bound,
       * so we have to look ahead and do one last erase after the while loop.
       */
      iter = first;
      ++iter;
      while(iter != bound) {
	first = erase(first);
	iter = first;
	++iter;
      }
      first = erase(first);
    }
  }
  return first;
}


#endif /* RW_SHUT_INLINES_GUARD_OFF__LHASH_CC */



/*
 *  The remainder of this file is for member definitions that will never 
 *  be seen from inside a class body.
 */

#ifndef RW_GET_INLINES_FOR_CLASS_BODY


#include "rw/stdex/destroy.h"


template <FORMALTYPES>
LHashTable<TYPES>&
LHashTable<TYPES>::operator=(const LHashTable<TYPES>& t)
{
  if(this != &t) {
    clear();
    if(capacity() > t.capacity()) {
      rw_destroy(table_);
#ifndef RW_ALLOCATOR_PLAIN
  RW_DEALLOC_TYPE(vectorAllocator,vectorAllocator,table_,1);
#else
  RW_DEALLOC_TYPE(vectorAllocator,vectorAllocator,table_,VECSIZE);
#endif
#ifndef RW_ALLOCATOR_PLAIN
      table_ = new (RW_ALLOC_TYPE(bucket_t,vectorAllocator,vectorAllocator,1,0))
	RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t>(t.capacity());
#else
      table_ = new (RW_ALLOC_TYPE(bucket_t,vectorAllocator,vectorAllocator,VECSIZE,0))
	RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)>(t.capacity());
#endif
    }
    else {
      table_->reserve(t.capacity());
      bucket_t dum;
      while(table_->size() < t.capacity())
	table_->push_back(dum);
    }
    Iterator dum;
    insertMaybe(t.begin(),t.end(),dum,false);
  }
  return *this;
}

template <FORMALTYPES> void
LHashTable<TYPES>::clear()
{
  list_->erase(list_->begin(),list_->end());
  typename table_t::iterator kill = table_->begin();
  while(kill != table_->end()) {
    (*kill).valid() = false;
    ++kill;
  }
}
#if __SUNPRO_CC!=0x420 && !defined(RW_NO_STL)
template <FORMALTYPES>
bool
LHashTable<TYPES>::testEqual(const LHashTable<TYPES>& t) const
{
/** precondition(size() == t.size()) **/
  ConstIterator i = begin();
  
  while(i != end()) {
    size_type num = t.count(*i);
    if(num != count(*i)) {
      return false;
    }
    else {
      while(num-- ) ++i;
    }
  }
  return true;
}
#endif /* !SunPro */


template <FORMALTYPES>
typename LHashTable<TYPES>::size_type
LHashTable<TYPES>::count(typename LHashTable<TYPES>::const_reference key) const
{
  size_type ret = 0;
  typename table_t::const_iterator slot = index(key);
  if(*slot) {
    bool found;
    typename list_t::iterator it =
      chunkyLowerBound((*slot).chain(), list_->end(), key, found);
    if(found) {
      while((it != list_->end() && eq_((*it).first,key))) {
	++ret;
	++it;
      }
    }
  }
  return ret;
}

template <FORMALTYPES>
typename LHashTable<TYPES>::Iterator
LHashTable<TYPES>::find(typename LHashTable<TYPES>::const_reference key)
{
  Iterator ret = end();
  typename table_t::iterator slot = index(key);
  typename list_t::iterator i;
  if(*slot) {
    bool found;
    i =  chunkyLowerBound((*slot).chain(), list_->end(), key, found);
    if(found)
      ret = i;
  }
  return ret;
}

template <FORMALTYPES>
typename LHashTable<TYPES>::ConstIterator
LHashTable<TYPES>::find(typename LHashTable<TYPES>::const_reference key) const
{
  ConstIterator ret = end();
  typename table_t::const_iterator slot = index(key);
  typename list_t::iterator i;
  if(*slot) {
    bool found;
    i = chunkyLowerBound((*slot).chain(), list_->end(), key, found);
    if(found)
      ret = ConstIterator(i);
  }
  return ret;
}

template <FORMALTYPES>
typename LHashTable<TYPES>::Iterator
LHashTable<TYPES>::lower_bound(
		       typename LHashTable<TYPES>::const_reference key
			      )
{
  Iterator ret = end();
  typename table_t::iterator slot = index(key);
  if(*slot) {
    bool dum;
    ret = chunkyLowerBound((*slot).chain(), list_->end(), key, dum);
  }
  return ret;
}

template <FORMALTYPES>
typename LHashTable<TYPES>::ConstIterator
LHashTable<TYPES>::lower_bound(
		       typename LHashTable<TYPES>::const_reference key
			      ) const
{
  ConstIterator ret = end();
  typename table_t::iterator slot = index(key);
  if(*slot) {
    bool dum;
    ret =
      ConstIterator(chunkyLowerBound((*slot).chain(), list_->end(), key, dum));
  }
  return ret;
}

template <FORMALTYPES>
typename LHashTable<TYPES>::Iterator
LHashTable<TYPES>::upper_bound(
		       typename LHashTable<TYPES>::const_reference key
			      )
{
  Iterator ret = end();
  typename table_t::iterator slot = index(key);
  if(*slot) {
    ret = chunkyUpperBound((*slot).chain(), list_->end(), key);
  }
  return ret;
}

template <FORMALTYPES>
typename LHashTable<TYPES>::ConstIterator
LHashTable<TYPES>::upper_bound(
		       typename LHashTable<TYPES>::const_reference key
			      ) const
{
  ConstIterator ret = end();
  typename table_t::iterator slot = index(key);
  if(*slot) {
    ret =
      ConstIterator(chunkyUpperBound((*slot).chain(), list_->end(), key));
  }
  return ret;
}

template <FORMALTYPES>
bool
LHashTable<TYPES>::insertMaybe(
		     typename LHashTable<TYPES>::const_reference key,
		     Iterator& ret,
		     bool single /* only one == key, or multiple? */
				 )
{
  if(0 == table_->capacity())
    return false;
  // else
	LHashTable_index_type_wrapper u;
	unsigned long hv = u.l = hash_(key);
  typename table_t::iterator idx = index(u);
  if(! *idx) // This bucket has never been used. Do it easy.
  {
    (*idx).valid() = true;
    (*idx).chain() = list_->insert(list_->end(),node_t(key,hv));
    ret = Iterator((*idx).chain());
  }
  else // This bucket is in use. Be thoughtful.
  {
    bool hit;
    typename list_t::iterator it;
    it = chunkyLowerBound((*idx).chain(), list_->end(), key, hit);
    ret = Iterator(it);
    if(single && hit) // then we won't insert
    {
      return false;
    }
    else // either not found or multiple inserts okay
    {
      // tricky: inserting at lower bound, if we had hit, is okay because
      // even if the slot iterator points at this lower bound, we are
      // inserting an eq_ key. If we had no hit, then lower bound ==
      // upper bound, and it references the first node in another slot
      // (or end()). In either of those cases, we will have to adjust 
      // the iterator in that new slot after the insert, because a
      // property of slist iterators is that inserting just prior to 
      // the node they reference invalidates them.

      it = list_->insert(it,node_t(key,hv));
      ret = Iterator(it);

      // now do iterator fixup, if needed
      if(++it != list_->end()) {
	unsigned long nexthv = (*it).second;
	if(nexthv%capacity() != hv%capacity()) {
		u.l = nexthv;
	  (*index(u)).chain() = it;
	}			// if fixup needed
      }				// if not at end during fixup
    }				// if single && hit (else)
  }				// if bucket empty (else)
  return true;
}

template <FORMALTYPES>
void
LHashTable<TYPES>::resize(typename LHashTable<TYPES>::size_type n)
{
  if(n == 0 || n == capacity()) return; 	// do only if changed
  LHashTable<TYPES> tmp(this->begin(),this->end(),
			    n,			// new size
			    hash_,eq_);
  swap(tmp);
  /** postconditions: new capacity() is n;
   ** every element is still here
   ** this == old_this
   **/
}

template <FORMALTYPES>
void
LHashTable<TYPES>::swap(LHashTable<TYPES>& t)
{
  RW_SL_STD_GLOBAL(swap)(list_,t.list_);
  RW_SL_STD_GLOBAL(swap)(table_,t.table_);
  RW_SL_STD_GLOBAL(swap)(eq_,t.eq_);
  RW_SL_STD_GLOBAL(swap)(hash_,t.hash_);
}



#endif /* ifndef RW_GET_INLINES_FOR_CLASS_BODY */

#undef FORMALTYPES
#undef TYPES
#undef RW_TEMPLATE_DECLTR__LHASHTABLE
#undef RW_SCOPE_DECLTR__LHASHTABLE
#undef RW_CNDTNL_TYPENAME__LHASHTABLE
