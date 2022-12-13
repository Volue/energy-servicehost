/*
 * Template definitions for new (Tools-7)
 * implementation of RWTValDlistIterator<T>
 *
 * $Id: tvdlist.cc 18623 2000-03-03 08:28:13Z sih $
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
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:44  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.10  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.9  1996/08/09 19:34:47  hart
 * HP (aCC compiler) port
 *
 * Revision 7.8  1996/02/21 22:47:52  griswolf
 * Fix an N^2 order saveGuts technique to make it linear.
 *
 * Revision 7.7  1996/02/18 01:48:54  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.6  1996/02/12 21:03:06  kevinj
 * Switched stream and class args in rw*Guts.
 *
 * Revision 7.5  1996/01/25 01:51:32  kevinj
 * Improved ETP macros.
 *
 * Revision 7.4  1996/01/13 02:34:45  kevinj
 * Simplified External Template Persistence (ETP).
 *
 * Revision 7.3  1995/11/13 12:47:12  jims
 * New stl-based collections now one with Tools-6 templates
 *
 * Revision 7.2  1995/09/05 20:56:51  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:19:16  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.1  1994/04/15  19:48:09  vriezen
 * Move all files to 6.1
 *
 * Revision 1.7  1993/11/08  13:49:36  jims
 * Port to ObjectStore
 *
 * Revision 1.6  1993/09/10  02:18:28  keffer
 * apply() now takes a function with a non-const reference, allowing
 * modification of the contained value.
 *
 * Revision 1.5  1993/07/05  21:20:25  randall
 * fixed memory link in remove( RWBoolean (*testFun) .... )
 *
 * Revision 1.4  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 1.3  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 1.2  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 1.1  1993/01/27  21:38:32  keffer
 * Initial revision
 *
 ****************************************************************************/

template <class T> RWBoolean
RWTValDlistIterator<T>::operator++()
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
RWTValDlistIterator<T>::operator--()
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
RWTValDlistIterator<T>::operator+=(size_t n)
{
  while (n--) ++(*this); return !tail_;
}     

template <class T> RWBoolean
RWTValDlistIterator<T>::operator-=(size_t n)
{
  while (n--) --(*this); return !head_;
}     

template <class T> RWBoolean
RWTValDlistIterator<T>::findNext(const T& val)
{
  while (++(*this))
  {
    if (key()==val)
      return TRUE;
  }
  return FALSE;
}

template <class T> RWBoolean
RWTValDlistIterator<T>::findNext(RWBoolean (*testFun)(const T&, void*), 
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

template <class T> void
RWTValDlistIterator<T>::insertAfterPoint(const T& a)
{
  RWPRECONDITION2(!tail_, "RWTValDlistIterator::insertAfterPoint(const T& a): Attempt to insertAfterPoint while iterator is past the end");

  typename RWTValDlist<T>::iterator i;

  if (head_)
    list_->prepend(a);
  else {
    i = iter_;
    ++i;
    list_->std().insert(i,a);
  }
}

template <class T> RWBoolean
RWTValDlistIterator<T>::remove()
{
  typename RWTValDlist<T>::iterator i;

  if (head_ || tail_)
    return FALSE;               // iterator is not active

  i = iter_;
  --i;
  list_->std().erase(iter_);
  iter_ = i;
  return TRUE;
}

template <class T> RWBoolean
RWTValDlistIterator<T>::removeNext(const T& val)
{
  while (++(*this)) {
    if (*iter_  == val) {
      return remove();
    }
  }
  return FALSE;
}

template <class T> RWBoolean
RWTValDlistIterator<T>::removeNext(RWBoolean (*testFun)(const T&, void*),
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

RWDEFINE_PERSISTABLE_TEMPLATE(RWTValDlist)

template <class T, class S>
void rwSaveGuts(S& str, const RWTValDlist<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  typename RWTValDlist<T>::const_iterator it = c.begin();
  while(it != c.end()) {
    str << *it;
    ++it;
  }
}

template <class T, class S>
void rwRestoreGuts(S& str, RWTValDlist<T>& c)
{
  typedef typename RWTValDlist<T>::size_type size_type;
  typename RWTValDlist<T>::size_type count;
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
