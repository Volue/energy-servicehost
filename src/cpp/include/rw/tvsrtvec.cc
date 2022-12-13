/*
 * Template definitions for RWTValSortedVector<T>
 *
 * $Id: tvsrtvec.cc 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/epersist.h"

template <class T, class C>
RWTValSortedVector<T,C>::RWTValSortedVector(const T* first, const T* last) {
  iterator it = this->begin();
  while(first != last)
    it = (iterator)insertSorted(it,*first++);
}

RWDEFINE_PERSISTABLE_TEMPLATE_2(RWTValSortedVector)

template <class T, class C, class S>
void rwSaveGuts(S& str, const RWTValSortedVector<T,C>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  for (typename RWTValSortedVector<T,C>::size_type i=0; i < c.entries(); ++i)
    str << c[i];
}

template <class T, class C, class S>
void rwRestoreGuts(S& str, RWTValSortedVector<T,C>& c)
{
  typedef typename RWTValSortedVector<T,C>::size_type size_type;
  typename RWTValSortedVector<T,C>::size_type count;
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
