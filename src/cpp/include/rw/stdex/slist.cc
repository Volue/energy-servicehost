/***************************************************************************
 *
 * slist.cc - template definitions for rw_slist<T>
 * 
 * $Id: slist.cc 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************
 */

#ifndef RW_ALLOCATOR_PLAIN
#  define FORMALTYPES class T, class A
#  define TYPES T,A
#else
#  define FORMALTYPES class T
#  define TYPES T
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
#   define RW_TEMPLATE_DECLTR__RW_SLIST
#   define RW_SCOPE_DECLTR__RW_SLIST
#   define RW_CONDITIONAL_TYPENAME 
#else
#   define RW_TEMPLATE_DECLTR__RW_SLIST template<FORMALTYPES>
#   define RW_SCOPE_DECLTR__RW_SLIST rw_slist<TYPES>::
#   define RW_CONDITIONAL_TYPENAME typename
#endif

#ifndef RW_GOT_INLINES_FOR_CLASS_BODY_SLIST_CC

RW_TEMPLATE_DECLTR__RW_SLIST 
RW_CONDITIONAL_TYPENAME RW_SCOPE_DECLTR__RW_SLIST iterator
RW_SCOPE_DECLTR__RW_SLIST insert(iterator loc, const T& val)
{
#ifndef RW_ALLOCATOR_PLAIN
  Node *nn = (Node*) RW_ALLOC_TYPE(Node,nodeAllocator,nodeAllocator,1,0);
#else
  Node *nn = (Node*) RW_ALLOC_TYPE(Node,nodeAllocator,nodeAllocator,sizeof(Node),0);
#endif
  // always use placement new
  new(nn) Node(val,*(loc.prelink()));

  if(loc.prelink() == tail_)		     // is loc at end
    tail_ = nn->mynext();		     // then update end() too.
  *(loc.prelink()) = nn;		     // point list at node
  ++entries_;				     // update count of entries

  return loc;
}



RW_TEMPLATE_DECLTR__RW_SLIST void
RW_SCOPE_DECLTR__RW_SLIST insert(iterator loc, const_iterator first, const_iterator bound)
{
  while(first != bound) {
    loc = insert(loc, *first);
    ++loc;
    ++first;
  }
}




RW_TEMPLATE_DECLTR__RW_SLIST void
RW_SCOPE_DECLTR__RW_SLIST insert(iterator loc, const T* first, const T* bound)
{
  while(first != bound) {
    loc = insert(loc, *first);
    ++loc;
    ++first;
  }
}



RW_TEMPLATE_DECLTR__RW_SLIST void
RW_SCOPE_DECLTR__RW_SLIST   insert(iterator loc, size_type n, const T& t)
{
  while(n--)
    insert(loc, t);
}


RW_TEMPLATE_DECLTR__RW_SLIST
RW_SCOPE_DECLTR__RW_SLIST rw_slist(size_type n, const T& value)
    : toHead_(0),head_(&toHead_),tail_(&toHead_), entries_(0)
{  
  insert(begin(), n, value);
}

#endif // RW_SHUT_INLINES_GUARD_OFF__SLIST_CC




/*
 *  The remainder of this file is for member definitions that will never 
 *  be seen from inside a class body.
 */

#ifndef RW_GET_INLINES_FOR_CLASS_BODY

#include "rw/stdex/destroy.h"

template <FORMALTYPES>
rw_slist<TYPES>::rw_slist() 
    : toHead_(0),head_(&toHead_),tail_(&toHead_), entries_(0)
{}


template <FORMALTYPES>
rw_slist<TYPES>::rw_slist(const rw_slist<TYPES>& arg)
    : toHead_(0),head_(&toHead_),tail_(&toHead_), entries_(0)
{
  insert(begin(), arg.begin(), arg.end());
}

template <FORMALTYPES>
rw_slist<TYPES>::rw_slist(typename rw_slist<TYPES>::const_iterator begin,
			  typename rw_slist<TYPES>::const_iterator bound)
    : toHead_(0),head_(&toHead_),tail_(&toHead_), entries_(0)
{
  insert(this->begin(),begin,bound);
}

template <FORMALTYPES>
rw_slist<TYPES>::rw_slist(const T* begin, const T* bound)
    : toHead_(0),head_(&toHead_),tail_(&toHead_), entries_(0)
{
  insert(this->begin(),begin,bound);
}

template <FORMALTYPES>
rw_slist<TYPES>::~rw_slist() {
  erase(begin(),end());
}

template <FORMALTYPES> rw_slist<TYPES>&
rw_slist<TYPES>::operator=(const rw_slist<TYPES>& rhs)
{
  if(toHead_ != rhs.toHead_) {
    erase(begin(),end());
    insert(begin(),rhs.begin(),rhs.end());
  }
  return *this;
}

template <FORMALTYPES> void
rw_slist<TYPES>::swap(rw_slist<TYPES>& s)
{
  Node    **n = (s.entries_) ? s.tail_ : &toHead_;
  s.tail_     = (entries_)   ? tail_   : &(s.toHead_);
  tail_    = n;
  
  Node     *o = s.toHead_; s.toHead_  = toHead_;  toHead_  = o;
  size_type p = s.size();  s.entries_ = entries_; entries_ = p; 
}




#ifndef RW_NO_TYPEDEF_ITERATOR
template <FORMALTYPES> typename rw_slist<TYPES>::iterator
#else
template <FORMALTYPES> typename rw_slist<TYPES>::Iterator
#endif
rw_slist<TYPES>::erase(iterator loc)
{
/** precondition: loc is dereferencable **/
/** precondition: ++loc exists **/
/** bounds check for loc != end()??? 
**/
  Node *victim = *(loc.prelink());		// get ptr to victim
  if((*loc.prelink())->mynext() == tail_)	// is it the last link?
  {
    tail_ = loc.prelink();			// then fixup tail_
  }
  *(loc.prelink()) = victim->next;		// link around victim
  if(0 == --entries_)				// keep tally
  {
    tail_ = &toHead_;				// and update tail_
/** not strictly needed, but makes all vslist.end() equal
**/   
    toHead_ = 0;				// ... and toHead
  }
  rw_destroy(&(victim->item()));
#ifndef RW_ALLOCATOR_PLAIN
  RW_DEALLOC_TYPE(nodeAllocator,nodeAllocator,victim,1);
#else
  RW_DEALLOC_TYPE(nodeAllocator,nodeAllocator,victim,sizeof(Node));
#endif
/** postcondition loc is now what ++loc was **/
  return loc;
}

#ifndef RW_NO_TYPEDEF_ITERATOR
template <FORMALTYPES> typename rw_slist<TYPES>::iterator
#else
template <FORMALTYPES> typename rw_slist<TYPES>::Iterator
#endif
rw_slist<TYPES>::erase(iterator first, iterator bound)
{
  if(bound == end()) {
    while(first != end())
    {
      erase(first);		// sane and in place (not like STL list);
    }
  }
  else {
    Node *tmp = *bound.prelink(); // build psuedoiterator that will not 
    iterator boundary(&tmp);	  // be affected by erasing prev. Node
    while(first != boundary)
    {
      erase(first);		// sane and in place (not like STL list);
    }
  }
  return first;
}

template <FORMALTYPES> void
rw_slist<TYPES>::splice(iterator pos, rw_slist<TYPES>& donor, iterator item)
{
  Node *temp = *(item.prelink());
  if(temp->mynext() == donor.tail_) // is it the last link?
  {
    donor.tail_ = item.prelink();		 // then backup tail_
  }
  *(item.prelink()) = temp->next;	// point item past link
  temp->next = *(pos.prelink());	// point link into new list
  if(pos == end())			// at our end?
  {
    tail_ = temp->mynext();		// advance our tail_
  }
  *(pos.prelink()) = temp;		// point list at the link
  --(donor.entries_);			// and fix sizes
  ++entries_;				// ...
}

template <FORMALTYPES> void
rw_slist<TYPES>::splice(iterator pos,
		      rw_slist<TYPES>& donor, iterator begin, iterator bound)
{
  if(begin != bound) {
    difference_type n = 0;
    if(&donor != this) {
#ifndef RW_3ARG_DISTANCE
      n=RW_SL_STD(distance)(begin,bound);	// here is the O(n) part. Sigh.
#else
      RW_SL_STD(distance)(begin,bound,n);	// here is the O(n) part. Sigh.
#endif
    }
    cutPaste(pos,n,donor,begin,bound);
  }
}

#ifndef RW_NO_TYPEDEF_ITERATOR
template <FORMALTYPES> typename rw_slist<TYPES>::iterator
#else
template <FORMALTYPES> typename rw_slist<TYPES>::Iterator
#endif
rw_slist<TYPES>::cutPaste(iterator pos,
			  size_type numItems,
			  rw_slist<TYPES>& donor,
			  iterator begin,
			  iterator bound
			 )
{
// protected method to do splicing with length knowledge already available
  Node *donorEnd = *bound.prelink();
  Node *donorStart = *begin.prelink();
  if(*bound.prelink() == *donor.tail_)
  {
    donor.tail_ = begin.prelink();
  }
  *(bound.prelink()) = *(pos.prelink());	 // merge donor end into list

  if(*pos.prelink() == *tail_)
  {
    tail_ = bound.prelink();
  }
  *pos.prelink() = donorStart; // merge us onto begin
  *begin.prelink() = donorEnd; // close up donor list
  
  if(numItems) {
    entries_ += numItems;
    donor.entries_ -= numItems; /** error/bounds check here? **/
  }
  pos.prelink() = donorEnd->mynext(); // build a ret value
  return bound;
}

template <FORMALTYPES> void
rw_slist<TYPES>::remove(const T& val) 
{
 // depends on erase(iterator) leaving iterator sane, on next item
  iterator hunt = begin();
  while(hunt != end()) {
    if(*hunt == val)
      erase(hunt);
    else
      ++hunt;
  }
}

template <FORMALTYPES> void
rw_slist<TYPES>::unique()
{
// depends on erase(iterator) leaving iterator sane, on 'next' item
  iterator hunt = begin();
  T* first = &*(hunt++); // use T* to avoid assuming T::operator=()
                         // which may not exist (e.g. if T is pair<const ...>)
  while(hunt != end())
  {
    if(*first == *hunt)
      erase(hunt);
    else
      first = &*(hunt++);
  }
}

template <FORMALTYPES> void
rw_slist<TYPES>::merge(rw_slist<TYPES>& input)
{
  if(input.empty()) return;	// easy case is worth checking 
  iterator loc = begin();
  iterator in = input.begin();
  while(loc != end() && in != input.end())
  {
    if(*in < *loc)
    {
      size_type n = 0;
      iterator front=in;
      while(in != input.end() && *in < *loc) {
	++in;
	++n;
      }
      loc = cutPaste(loc,n,input,front,in);
      in = input.begin(); // always, after we merge something
    }
    else
    {
      ++loc;
    }
  }
  if(in != input.end())
    splice(loc,input,in,input.end());
}

template <FORMALTYPES> void
rw_slist<TYPES>::reverse() 
{
  if(! empty()) {
    iterator stop = end();
    while(++begin() != stop) {
      splice(stop,*this,begin());
    }
  }
}

template <FORMALTYPES> void
rw_slist<TYPES>::sortPairs() 
{
  iterator it = begin();
  iterator nx = it;
  while(it != end() && nx != end())
  {
    nx = it; ++nx;
    if(nx.isValid() && *nx < *it)
    {
      Node *b = *it.prelink();
      Node *a = *nx.prelink();
      Node *c = a->next;
      if(a->mynext() == tail_) {
	tail_ = b->mynext();
      }
      *it.prelink() = a;
      a->next = b;
      b->next = c;
      it = nx;			// swapping advanced nx;
    }
    else {
      it = nx; if(it.isValid()) ++it;
    }
  }
}

#ifndef RW_NO_TYPEDEF_ITERATOR
template <FORMALTYPES> typename rw_slist<TYPES>::iterator
#else
template <FORMALTYPES> typename rw_slist<TYPES>::Iterator
#endif
rw_slist<TYPES>::adjacentMerge(iterator i1, iterator i2, iterator i3,
			       size_type count1, size_type count2
			      )
{
  // merge [i1,i2) and [i2,i3) assuming they are sorted
  // cope correctly with edge conditions; return iter 'just past
  // second interval' (where i3 would be without the merge)

  if(i1 == i2 || i2 == i3) return i3; // handle empty ranges
  rw_slist<TYPES> t1,t2;
  t1.cutPaste(t1.end(),count1+count2,*this,i1,i3);
  t2.cutPaste(t2.end(),count2,t1,i2,t1.end());
  t1.merge(t2);
  return cutPaste(i1,t1.size(),t1,t1.begin(),t1.end());
}

template <FORMALTYPES> void
rw_slist<TYPES>::sort() 
{
  // algorithm is:
  // for i in 2,4,...2^n... size()/2 
  //   merge-sort i-length adjacent sub-lists
  // This will pass through the list about ln(size()) times for 
  // total of about N*log(N) comparisons.

  if(size() < 2) return;
  sortPairs();
  for(size_type i = 2; i < size(); i*=2)
  {
    size_type accumloc = 0;
    iterator i1 = begin();
    while(accumloc < size())
    {
      size_type count1,count2;
      iterator i2(i1);
      if(accumloc < size() -i) {
	RW_SL_STD(advance)(i2,i);
	count1 = i;;
      }
      else {
	count1 = size() - accumloc;
	i2 = end();
      }
      accumloc += count1;
      iterator i3(i2);
      if(accumloc < size() -i) {
	RW_SL_STD(advance)(i3,i);
	count2 = i;
      }
      else {
	i3 = end();
	count2 = size() - accumloc;
      }
      accumloc += count2;
      i1 = adjacentMerge(i1,i2,i3,count1,count2);
    }
  }
}

#if 0 /* a hack to provide "almost" member template stuff */

template <FORMALTYPES> void
rw_slist<TYPES>::remove_if(UnaryPredicate<T,bool>& p)
{
// depends on erase(iterator) leaving iterator sane, on next item
  iterator hunt = begin();
  while(hunt != end()) {
    if( p(*hunt) )
      erase(hunt);
    else
      ++hunt;
  }
}

template <FORMALTYPES> void
rw_slist<TYPES>::unique(BinaryPredicate<T,T,bool>& p)
{
// depends on erase(iterator) leaving iterator sane, on 'next' item
  iterator hunt = begin();
  T first = *(hunt++);
  while(hunt != end())
  {
    if(p(first,*hunt))
      erase(hunt);
    else
      first = *(hunt++);
  }
}

template <FORMALTYPES> void
rw_slist<TYPES>::merge(rw_slist<TYPES>& input, Comparison<TYPES>& comp)
{
  if(input.empty()) return;	// easy case is worth checking 
  iterator loc = begin();
  iterator in = input.begin();
  while(loc != end() && in != input.end())
  {
    if(comp(*in,*loc))
    {
      size_type n = 0;
      iterator front=in;
      while(in != input.end() && comp(*in,*loc)) {
	++in;
	++n;
      }
      loc = cutPaste(loc,n,input,front,in);
      in = input.begin(); // always, after we merge something
    }
    else
    {
      ++loc;
    }
  }
  if(in != input.end())
    splice(loc,input,in,input.end());
}

template <FORMALTYPES> void
rw_slist<TYPES>::sortPairs(Comparison<TYPES>& comp) 
{
  iterator it = begin();
  iterator nx = it;
  while(it != end() && nx != end())
  {
    nx = it; ++nx;
    if(nx.isValid() && comp(*nx,*it))
    {
      Node *b = *it.prelink();
      Node *a = *nx.prelink();
      Node *c = a->next;
      if(a->mynext() == tail_) {
	tail_ = b->mynext();
      }
      *it.prelink() = a;
      a->next = b;
      b->next = c;
      it = nx;			// swapping advanced nx;
    }
    else {
      it = nx; if(it.isValid()) ++it;
    }
  }
}

template <FORMALTYPES> rw_slist<TYPES>::iterator
rw_slist<TYPES>::adjacentMerge(iterator i1, iterator i2, iterator i3,
			     size_type count1, size_type count2,
			     Comparison<TYPES>& comp)
{
  // merge [i1,i2) and [i2,i3) assuming they are sorted
  // cope correctly with edge conditions; return iter \'just past
  // second interval\' (where i3 would be without the merge)

  if(i1 == i2 || i2 == i3) return i3; // handle empty ranges
  rw_slist<TYPES> t1,t2;
  t1.cutPaste(t1.end(),count1+count2,*this,i1,i3);
  t2.cutPaste(t2.end(),count2,t1,i2,t1.end());
  t1.merge(t2,comp);
  return cutPaste(i1,t1.size(),t1,t1.begin(),t1.end());
}

template <FORMALTYPES> void
rw_slist<TYPES>::sort(Comparison<TYPES>& comp) 
{
  // algorithm is:
  // for i in 2,4,...2^n... size()/2 
  //   merge-sort i-length adjacent sub-lists
  // This will pass through the list about ln(size()) times for 
  // total of about N*log(N) comparisons.

  if(size() < 2) return;
  sortPairs(comp);
  for(size_type i = 2; i < size(); i*=2)
  {
    size_type accumloc = 0;
    iterator i1 = begin();
    while(accumloc < size())
    {
      size_type count1,count2;
      iterator i2(i1);
      if(accumloc < size() -i) {
	RW_SL_STD(advance)(i2,i);
	count1 = i;;
      }
      else {
	count1 = size() - accumloc;
	i2 = end();
      }
      accumloc += count1;
      iterator i3(i2);
      if(accumloc < size() -i) {
	RW_SL_STD(advance)(i3,i);
	count2 = i;
      }
      else {
	i3 = end();
	count2 = size() - accumloc;
      }
      accumloc += count2;
      i1 = adjacentMerge(i1,i2,i3,count1,count2,comp);
    }
  }
}

#endif

#if 0 /* RW internal debugging methods */
template <FORMALTYPES> void
rw_slist<TYPES>::showOff(const char *s) const {
  cout << s << ":" << flush;
  char sep = ' ';
  for(const_iterator i=begin(); i != end(); ++i) {
    cout << sep << *i << flush;
    sep = ',';
  }
  cout << "; len=" << size()<< endl;
}

template <FORMALTYPES> void
rw_slist<TYPES>::iprint(const char* p,
	    rw_slist<TYPES>::const_iterator i,
	    const char* t) const
{
  cout << p;
  if(i) cout << *i << flush;
  else cout << "\"nil\"" << flush;
  size_type dist = 0;
  RW_SL_STD(distance)(begin(),i,dist);
  cout << "<" << dist << ">";
  cout << t;
}
#endif

#endif // ifndef RW_GET_INLINES_FOR_CLASS_BODY


#undef FORMALTYPES
#undef TYPES
#undef RW_TEMPLATE_DECLTR__RW_SLIST
#undef RW_SCOPE_DECLTR__RW_SLIST
#undef RW_CONDITIONAL_TYPENAME 
