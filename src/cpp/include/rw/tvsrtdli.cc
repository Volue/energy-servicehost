/*
 * Template definitions for
 * implementation of RWTValSortedDlistIterator<T,C>
 *
 * $Id: tvsrtdli.cc 18623 2000-03-03 08:28:13Z sih $
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
 ***************************************************************************/

// the following code is optimum if iterators are not random

template <class T, class C>
typename RWTValSortedDlist<T,C>::size_type
RWTValSortedDlist<T,C>::index(
            typename RWTValSortedDlist<T,C>::const_reference key
			      ) const
{
// be sure there is a last() to compare.
  if(this->isEmpty())
    return this->npos;
  C comp = C();
// Must stop before end(): last() "<" key: we would not stop
  if(comp(last(),key))
    return this->npos;
  size_type ret = 0;
  const_iterator it = this->begin();
// loop as long as key is "<" than *iterator
  while(comp(*it,key)) {
    ++it;
    ++ret;
  }
// if *iterator "less" than key, then we have no hit
  return (comp(*it,key)) ? this->npos : ret;
}


template <class T, class C>
typename RWTValSortedDlist<T,C>::size_type
RWTValSortedDlist<T,C>::merge(const RWTValSortedDlist<T,C>& coll)
{
  size_type ret = coll.entries();

  if (ret) {
    typename container_type::iterator first1 = impl_.begin();
    typename container_type::iterator last1  = impl_.end();
    const_iterator    first2 = coll.begin();
    const_iterator    last2 = coll.end();

    while (first1 != last1 && first2 != last2) {
      while (first2 != last2 && C()(*first2,*first1)) {
        impl_.insert(first1,*first2);
        ++first2;
      }
      ++first1;
    }

    while (first2 != last2) {
      impl_.insert(this->end(), *first2);
      ++first2;
    }
  }
  return ret;
}


template <class T, class C> RWBoolean
RWTValSortedDlistIterator<T,C>::operator++()
{
  RWASSERT(!(head_ == true  &&  tail_ == true)); 

  if (tail_)
    /* do nothing */ ;

  else if (head_) {
    iter_ = list_->begin();
    head_ = false;
  }
  else
    ++iter_;

  if (!tail_ && iter_ == list_->end())
    tail_ = true;

  RWASSERT(!(head_ == true  &&  tail_ == true)); 

  return !tail_;
}

template <class T, class C> RWBoolean
RWTValSortedDlistIterator<T,C>::operator--()
{
  RWASSERT(!(head_ == true  &&  tail_ == true)); 

  if (head_)
    /* do nothing */ ;

  else if (tail_) {
    if (list_->entries() == 0) {
      tail_ = false;
      head_ = true;
    }
    else {
      tail_ = false;
      --iter_;
    }
  }
  else if (iter_ == list_->begin())
    head_ = true;
  else
    --iter_;

  RWASSERT(!(head_ == true  &&  tail_ == true)); 

  return !head_;
}

template <class T, class C> RWBoolean
RWTValSortedDlistIterator<T,C>::operator+=(size_t n)
{
  while (n--) ++(*this); return !tail_;
}     

template <class T, class C> RWBoolean
RWTValSortedDlistIterator<T,C>::operator-=(size_t n)
{
  while (n--) --(*this); return !head_;
}     

template <class T, class C> RWBoolean
RWTValSortedDlistIterator<T,C>::findNext(const T& val)
{
  while (++(*this))
  {
    if (key()==val)
      return TRUE;
  }
  return FALSE;
}

template <class T, class C> RWBoolean
RWTValSortedDlistIterator<T,C>::findNext(RWBoolean (*testFun)(const T&, void*), 
                                 void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  while (++(*this))
  {
    if (testFun(key(),d))
      return TRUE;
  }
  return FALSE;
}

template <class T, class C> RWBoolean
RWTValSortedDlistIterator<T,C>::remove()
{
#ifndef RW_ALLOCATOR_PLAIN
  typename RW_SL_STD(list)<T,RW_SL_STD(allocator)<T> >::iterator i;
#else
  typename RW_SL_STD(list)<T,RW_SL_STD(allocator)>::iterator i;
#endif
  if (head_ || tail_)
    return FALSE;               // iterator is not active

  i = iter_;
  --i;
  list_->std().erase(iter_);
  iter_ = i;
  return TRUE;
}

template <class T, class C> RWBoolean
RWTValSortedDlistIterator<T,C>::removeNext(const T& val)
{
  while (++(*this)) {
    if (*iter_  == val) {
      return remove();
    }
  }
  return FALSE;
}

template <class T, class C> RWBoolean
RWTValSortedDlistIterator<T,C>
  ::removeNext(RWBoolean (*testFun)(const T&, void*),
               void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  while (++(*this)) {
    if (testFun(*iter_,d)) {
      return remove();
    }
  }
  return FALSE;
}


#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE_2(RWTValSortedDlist)

template <class T, class C, class S>
void rwSaveGuts(S& str, const RWTValSortedDlist<T,C>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  if(c.entries()) {
// iterate backward so RestoreGuts is linear...
    typename RWTValSortedDlist<T,C>::const_iterator it = c.end();
    do {
      --it;
      str << *it;
    } while(it != c.begin());
  }
}

template <class T, class C, class S>
void rwRestoreGuts(S& str, RWTValSortedDlist<T,C>& c)
{
  typedef typename RWTValSortedDlist<T,C>::size_type size_type;
  typename RWTValSortedDlist<T,C>::size_type count;
#ifdef RW_FIX_XSIZE_T
  str.getSizeT(count);
#else
  str >> count;
#endif
  c.clear();
  for (size_type i=0; i < count; ++i) {
    T t;
    str >> t;
    c.insert(t);
  }
}
