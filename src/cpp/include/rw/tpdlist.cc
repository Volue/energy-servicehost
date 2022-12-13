/***************************************************************************
 *
 * tpdlist.cc - template definitions for RWTPtrDlist<T>
 *              and RWTPtrDlistIterator
 * 
 * $Id: tpdlist.cc 18623 2000-03-03 08:28:13Z sih $
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


template <class T> void
RWTPtrDlist<T>::clearAndDestroy() {
  for(iterator i = this->begin(); i != this->end(); /**/) {
    iterator nx = i;
    ++nx;
    delete *i;
    impl_.erase(i);
    i = nx;
  }
}

#if !defined(RW_NO_INHERIT_TEMPL_TYPEDEF)
template <class T> typename RWTPtrDlist<T>::size_type
RWTPtrDlist<T>::removeAll(bool (*fn)(const T* key,void*),
                         void* x)
{
  size_type ret = 0;
  for(iterator it = this->begin(); it != this->end();/**/) {
    iterator nx = it;
    ++nx;
    if((*fn)(*it,x)) {
      impl_.erase(it);
      ++ret;
    }
    it = nx;
  }
  return ret;
}

template <class T> typename RWTPtrDlist<T>::size_type
RWTPtrDlist<T>::removeAll(bool (*fn)(T* key, void*),void* x)
{
  size_type ret = 0;
  for(iterator it = this->begin(); it != this->end();/**/) {
    iterator nx = it;
    ++nx;
    if((*fn)(*it,x)) {
      impl_.erase(it);
      ++ret;
    }
    it = nx;
  }
  return ret;
}

template <class T> typename RWTPtrDlist<T>::size_type
RWTPtrDlist<T>::removeAll(const T* key)
{
  size_type ret = 0;
  for(iterator it = this->begin(); it != this->end();/**/) {
    iterator nx = it;
    ++nx;
    if(**it == *key) {
      impl_.erase(it);
      ++ret;
    }
    it = nx;
  }
  return ret;
}
#endif /* if !defined(RW_NO_INHERIT_TEMPL_TYPEDEF) */

template <class T> void
RWTPtrDlist<T>::sort()
{
  if (impl_.size() < 2) return;
  sortPairs();
  for(size_type i = 2; i < impl_.size(); i *= 2)
  {
    size_type accumloc = 0;
    iterator i1 = this->begin();
    while(accumloc < impl_.size())
    {
      size_type count1, count2;
      iterator i2(i1);
      if(accumloc < impl_.size() -i) {
        RW_SL_STD(advance)(i2,i);
        count1 = i;
      }
      else {
        count1 = impl_.size() - accumloc;
        i2 = this->end();
      }
      accumloc += count1;
      iterator i3(i2);
      if(accumloc < impl_.size() -i) {
        RW_SL_STD(advance)(i3,i);
        count2 = i;
      }
      else {
        i3 = this->end();
        count2 = impl_.size() - accumloc;
      }
      accumloc += count2;
      pointerMerge(i1,i2,i3);
      i1 = i3;
    }
  }
}

template <class T> void
RWTPtrDlist<T>::sortPairs() {
  iterator it = this->begin();
  iterator pv;
  while(it != this->end())
  {
    pv = it++;                  // pv holds 'first', it holds 'second'
    if(it == this->end())
      break;
    if(**it < **pv) {
      T* tmp = *pv;
      *pv= *it;
      *it = tmp;
    }
    ++it;                       // it past 'second' to new 'first'
  }
}

template <class T> void
RWTPtrDlist<T>::pointerMerge(iterator i1, iterator i2, iterator i3)
{
  // merge the sub-ranges [i1,i2) and [i2,i3) assuming they hold pointers 
  // and that they are each sorted. Returns i3.
  
  iterator stop1 = impl_.insert(i2,(T*)0); // put dummy in place
  while(i1 != stop1 && i2 != i3) {
    if(**i2 < **i1) {
      iterator start2(i2);
      while(i2 != i3 && **i2 < **i1)
        ++i2;

      impl_.splice(i1,impl_,start2,i2);
    }
    else {
      ++i1;
    }
  }
  impl_.erase(stop1);           // remove the dummy
}

//===========================================

template <class T> RWBoolean
RWTPtrDlistIterator<T>::operator++()
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

template <class T> RWBoolean
RWTPtrDlistIterator<T>::operator--()
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

template <class T> RWBoolean
RWTPtrDlistIterator<T>::operator+=(size_t n)
{
  while (n--) ++(*this); return !tail_;
}     

template <class T> RWBoolean
RWTPtrDlistIterator<T>::operator-=(size_t n)
{
  while (n--) --(*this); return !head_;
}     

template <class T> T*
RWTPtrDlistIterator<T>::findNext(const T* p)
{
  T* a;
  while ( (a = (*this)()) != rwnil)
  {
    if (*a == *p)
      return a;
  }
  return rwnil;
}

template <class T> T*
RWTPtrDlistIterator<T>::findNext(RWBoolean (*testFun)(T*, void*), 
                                 void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  T* a;
  while ( (a = (*this)()) != rwnil)
  {
    if (testFun(a, d))
      return a;
  }
  return rwnil;
}

template <class T> void
RWTPtrDlistIterator<T>::insertAfterPoint(T* a)
{
  RWPRECONDITION2(!tail_, "RWTPtrDlistIterator::insertAfterPoint(T* a): Attempt to insertAfterPoint while iterator is past the end");

  typename RWTPtrDlist<T>::iterator i;

  if (head_)
    list_->prepend(a);
  else {
    i = iter_;
    ++i;
    list_->impl_.insert(i,a);
  }
}

template <class T> T*
RWTPtrDlistIterator<T>::remove()
{
  typename RWTPtrDlist<T>::iterator i;

  if (head_ || tail_)
    return rwnil;               // iterator is not active

  T* ret = *iter_;
  i = iter_;
  --i;
  list_->impl_.erase(iter_);
  iter_ = i;
  return ret;
}

template <class T> T*
RWTPtrDlistIterator<T>::removeNext(const T* p)
{
  while (++(*this)) {
    if (*(*iter_)  == *p) {
      return remove();
    }
  }
  return rwnil;
}

template <class T> T*
RWTPtrDlistIterator<T>::removeNext(RWBoolean (*testFun)(T*, void*),
                                   void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  while (++(*this)) {
    if (testFun(*iter_,d)) {
      return remove();
    }
  }
  return rwnil;
}


#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTPtrDlist)

template <class T, class S>
void rwSaveGuts(S& str, const RWTPtrDlist<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  typename RWTPtrDlist<T>::const_iterator it = c.begin();
  while(it != c.end()) {
    str << *(*it);
    ++it;
  }
}

template <class T, class S>
void rwRestoreGuts(S& str, RWTPtrDlist<T>& c)
{
  typedef typename RWTPtrDlist<T>::size_type size_type;
  size_type count;
#ifdef RW_FIX_XSIZE_T
  str.getSizeT(count);
#else
  str >> count;
#endif
  c.clear();
  for (size_type i=0; i < count; ++i) {
    T* p;
    str >> p;
    c.insert(p);
  }
}
