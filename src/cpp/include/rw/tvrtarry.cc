/*
 * Template definitions for RWTValVirtualArray<T>
 *
 * $Id: tvrtarry.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.7  1996/08/09 19:42:25  hart
 * HP (aCC compiler) port
 *
 * Revision 7.6  1996/07/18 04:09:03  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/02/18 01:49:28  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.4  1996/01/11 23:07:24  pearson
 * #2175  Fix boundary error in operator=(const T&)   The number sent to
 *        slice is a length, not an index and should not be decremented first.
 *
 * Revision 7.3  1995/09/07 23:36:35  kevinj
 * #2035: Changed:
 *    RWVirtualRef& vr = *((RWVirtualRef&)sl.varray_->vref_);
 * to:-
 *    RWVirtualRef& vr = *((RWVirtualRef*)sl.varray_->vref_);
 *
 * Revision 7.3  1995/09/07  22:10:52  kevinj
 * Changed (RWVirtualRef&) to (RWVirtualRef*) in operator=.
 *
 * Revision 7.2  1995/09/05  21:28:27  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:27:47  josh
 * Merged 6.1 and 7.0 development trees
 *
 *
 * Revision 6.3  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.1.7.2  1994/04/18  20:01:03  vriezen
 * Properly cast and dereference in
 * void RWTVirtualSlice<T>::operator =(const class RWTVirtualSlice<T> &)
 *
 * Revision 6.2  1994/04/18  20:02:10  vriezen
 * Properly cast and dereference in
 * void RWTVirtualSlice<T>::operator =(const class RWTVirtualSlice<T> &)
 *
 * Revision 6.1  1994/04/15  19:48:27  vriezen
 * Move all files to 6.1
 *
 * Revision 6.1.7.1  1994/04/15  22:45:59  vriezen
 * Version 7, Initial code
 *
 * Revision 6.1  1994/04/15  19:48:27  vriezen
 * Move all files to 6.1
 *
 * Revision 1.5  1993/11/08  20:32:10  jims
 * Port to ObjectStore
 *
 * Revision 1.4  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.3  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 1.2  1993/02/06  02:05:47  keffer
 * Added copyright notice.
 *
 ***************************************************************************
 */

#include "rw/vpage.h"

template <class T>
RWTValVirtualArray<T>::RWTValVirtualArray(long size, RWVirtualPageHeap* heap)
{
  vref_ = RW_NEW(this, RWTVirtualRef<T>::get_os_typespec())
    RWTVirtualRef<T>(size, heap);
}

template <class T>
RWTValVirtualArray<T>::~RWTValVirtualArray()
{
  if (vref_->removeReference() == 0) delete vref_;
}

template <class T>
RWTValVirtualArray<T>::RWTValVirtualArray(const RWTValVirtualArray<T>& v)
{
  if (RWAlloc::is_same_allocator(this, &v)) 
  {
    vref_ = v.vref_;
    vref_->addReference();
  }
  else
  {
    vref_ = RW_NEW(this, RWTVirtualRef<T>::get_os_typespec())
      RWTVirtualRef<T>(*v.vref_);
  }
}

template <class T>
RWTValVirtualArray<T>::RWTValVirtualArray(const RWTVirtualSlice<T>& sl)
{
  vref_ =
    RW_NEW(this, RWTVirtualRef<T>::get_os_typespec())
      RWTVirtualRef<T>(sl.extent_, sl.varray_->heap());
  RWTVirtualRef<T>* vr = (RWTVirtualRef<T>*)sl.varray_->vref_;
  vref_->conformalCopy(0, *vr, sl.start_, sl.extent_);
}

template <class T> RWTValVirtualArray<T>&
RWTValVirtualArray<T>::operator=(const RWTValVirtualArray<T>& v)
{
  if (RWAlloc::is_same_allocator(this, &v))
  {
    v.vref_->addReference();
    if (vref_->removeReference() == 0) delete vref_;
    vref_ = v.vref_;
  } 
  else 
  {
    vref_ = RW_NEW(this, RWTVirtualRef<T>::get_os_typespec())
      RWTVirtualRef<T>(*v.vref_);
  }
  return *this;
}

template <class T> void
RWTValVirtualArray<T>::operator=(const RWTVirtualSlice<T>& sl)
{
  RWTValVirtualArray<T>* v2 = (RWTValVirtualArray<T>*)sl.varray_;
  RWTVirtualRef<T>* newvref =
    RW_NEW(this, RWTVirtualRef<T>::get_os_typespec())
      RWTVirtualRef<T>(sl.extent_, v2->heap());
  newvref->conformalCopy(0, *v2->vref_, sl.start_, sl.extent_);
  if (vref_->removeReference() == 0) delete vref_;
  vref_ = newvref;
}

template <class T> T
RWTValVirtualArray<T>::operator=(const T& val)
{
  slice(0, length()) = val;             // Take a slice of self
  return val;
}

template <class T> void
RWTValVirtualArray<T>::cow()
{
  if (vref_->references()>1) {
    vref_->removeReference();
    vref_ = RW_NEW(this, RWTVirtualRef<T>::get_os_typespec())
      RWTVirtualRef<T>(*vref_);
  }
}

/****************************************************************
 ****************************************************************
 *                                                              *
 *                      RWTVirtualSlice<T>                      *
 *                      Definitions                             *
 *                                                              *
 ****************************************************************
 ****************************************************************/

template <class T> T
RWTVirtualSlice<T>::operator=(const T& newVal)
{
  this->varray_->cow();
  this->varray_->vref_->set(this->start_, extent_, newVal);
  return newVal;
}

template <class T> void
RWTVirtualSlice<T>::operator=(const RWTVirtualSlice<T>& sl)
{
  RWVirtualRef& vr = *((RWVirtualRef*)sl.varray_->vref_);

  this->varray_->cow();
  this->varray_->vref_->setSlice(this->start_, extent_, vr, sl.start_, sl.extent_);
}

template <class T> void
RWTVirtualSlice<T>::operator=(const RWTValVirtualArray<T>& v)
{
  RWTValVirtualArray<T>& va = (RWTValVirtualArray<T>&)v;
  this->varray_->cow();
  this->varray_->vref_->setSlice(this->start_, extent_, *(va.vref_), 0, v.length());
}
