/***************************************************************************
 *
 * tpdeque.cc - template definitions for RWTPtrDeque.
 *
 * $Id: tpdeque.cc 18623 2000-03-03 08:28:13Z sih $
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
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.8  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.7  1996/08/14 23:18:17  griswolf
 * HP (aCC compiler) port
 *
 * Revision 7.6  96/02/12  21:03:06  21:03:06  kevinj (Kevin Johnsrude)
 * Switched stream and class args in rw*Guts.
 * 
 * Revision 7.5  1996/01/25 01:51:32  kevinj
 * Improved ETP macros.
 *
 * Revision 7.4  1996/01/12 01:39:14  kevinj
 * RWT{Val,Ptr}Set External Template Persistence (ETP).
 *
 * Revision 7.3  1996/01/11 01:33:10  kevinj
 * RWFile external template persistence.
 *
 * Revision 7.2  1996/01/10 21:09:42  kevinj
 * RWDEFINE_PERSISTABLE_CONTAINER and RWDECLARE_PERSISTABLE_CONTAINER.
 *
 * Revision 7.1  1996/01/09 22:50:13  kevinj
 * Simpler external template persistence.
 *
 *
 ****************************************************************************/

#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTPtrDeque)

template <class T, class S>
void rwSaveGuts(S& str, const RWTPtrDeque<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  for (size_t i=0; i < c.entries(); ++i)
    str << *c[i];
}

template <class T, class S>
void rwRestoreGuts(S& str, RWTPtrDeque<T>& c)
{
  typedef typename RWTPtrDeque<T>::size_type size_type;
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

