/***************************************************************************
 *
 * tpslist.cc - template definitions for RWTPtrSlist<T>
 * 
 * $Id: tpslist.cc 18623 2000-03-03 08:28:13Z sih $
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

/*
 * Having trouble with these overridden clearAndDestroy and
 * removeAll functions.
 * For now we'll use the version inherited from the base class.
 */

#if 0
template <class T> void
RWTPtrSlist<T>::clearAndDestroy() {
  for(iterator i = this->begin(); i != this->end(); /**/) {
    iterator nx = i;
    ++nx;
    delete *i;
    impl_.erase(i);
    i = nx;
  }
}
#endif

#if 0
template <class T> rw_slist<T*>::size_type
RWTPtrSlist<T>::removeAll(bool (*fn)(RWTPtrSlist<T>::const_value key,void*),void* x)
{
  rw_slist<T*>::size_type ret = 0;
  for(iterator it = this->begin(); it != this->end();/**/) {
    if((*fn)(*it,x)) {
      impl_.erase(it);
      ++ret;
    }
    ++it;
  }
  return ret;
}

template <class T> rw_slist<T*>::size_type
RWTPtrSlist<T>::removeAll(const_value key)
{
  rw_slist<T*>::size_type ret = 0;
  for(iterator it = this->begin(); it != this->end();/**/) {
    if(**it == *key) {
      impl_.erase(it);
      ++ret;
    }
    ++it;
  }
  return ret;
}
#endif

/*
 * This is a "for now" sorting algorithm.  Ultimately the underlying
 * implementation will have a template sort member function that
 * takes a comparitor, needed for dereferencing the elements
 * in the rw_slist<T*> impl_.
 *
 * So, for now, here is an insertion sort just like the one in "Algorithms"
 * by Robert Sedgwick, 2nd Edition, p.100:
 */

template <class T>
void RWTPtrSlist<T>::sort()
{
  int i, j;
  T* v;
  for (i=1; i<this->entries(); i++) {
    v = this->at(i);
    j = i;
    while (j > 0 && *(this->at(j-1)) > *v) {
      this->at(j) = this->at(j-1);
      --j;
    }
    this->at(j) = v;
  }
}
        

//===========================================

template <class T> RWBoolean
RWTPtrSlistIterator<T>::operator++()
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
RWTPtrSlistIterator<T>::operator+=(size_t n)
{
  while (n--) ++(*this); return !tail_;
}     

template <class T> T*
RWTPtrSlistIterator<T>::findNext(const T* p)
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
RWTPtrSlistIterator<T>::findNext(RWBoolean (*testFun)(T*, void*), 
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
RWTPtrSlistIterator<T>::insertAfterPoint(T* a)
{
  RWPRECONDITION2(!tail_, "RWTPtrSlistIterator::insertAfterPoint(T* a): Attempt to insertAfterPoint while iterator is past the end");

  typename RWTPtrSlist<T>::iterator i;

  if (head_)
    list_->prepend(a);
  else {
    i = iter_;
    ++i;
    list_->impl_.insert(i,a);
  }
}

template <class T> T*
RWTPtrSlistIterator<T>::remove()
{
  typename RWTPtrSlist<T>::iterator i, j;
  T* ret;

  if (head_ || tail_)
    return rwnil;               // iterator is not active

  if (iter_ == list_->begin()) {
    ret = list_->removeFirst();
    head_ = true;
  }
  else {
    // As the manual warns, for slists this is "relatively inefficient:"
    i = list_->begin();  
    do {
      j = i;
      ++i;
    } while (i != iter_);

    ret = *iter_;
    list_->impl_.erase(iter_);
    iter_ = j;
  }
  return ret;
}

template <class T> T*
RWTPtrSlistIterator<T>::removeNext(const T* p)
{
  while (++(*this)) {
    if (*(*iter_)  == *p) {
      return remove();
    }
  }
  return rwnil;
}

template <class T> T*
RWTPtrSlistIterator<T>::removeNext(RWBoolean (*testFun)(T*, void*),
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

RWDEFINE_PERSISTABLE_TEMPLATE(RWTPtrSlist)

template <class T, class S>
void rwSaveGuts(S& str, const RWTPtrSlist<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  typename RWTPtrSlist<T>::const_iterator it = c.begin();
  while(it != c.end()) {
    str << *(*it);
    ++it;
  }
}

template <class T, class S>
void rwRestoreGuts(S& str, RWTPtrSlist<T>& c)
{
  typedef typename RWTPtrSlist<T>::size_type size_type;
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
