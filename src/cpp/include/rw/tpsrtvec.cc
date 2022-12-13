/*
 * Template definitions for RWTPtrSortedVector<TP>
 *
 * $Id: tpsrtvec.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.13  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.12  1996/08/09 19:32:29  hart
 * HP (aCC compiler) port
 *
 * Revision 7.11  96/03/16  19:13:53  19:13:53  kevinj (Kevin Johnsrude)
 * Changed ctor from T** to T*const*.
 * 
 * Revision 7.10  1996/02/19 12:34:12  jims
 * Port to Stdlib 1.2
 *
 * Revision 7.9  1996/02/12 21:03:06  kevinj
 * Switched stream and class args in rw*Guts.
 *
 * Revision 7.8  1996/02/07 06:04:13  jims
 * Remove devnotes (#2555)
 *
 * Revision 7.7  1996/01/25 01:51:32  kevinj
 * Improved ETP macros.
 *
 * Revision 7.6  1996/01/15 17:56:06  kevinj
 * Fixed bug in ETP.
 *
 * Revision 7.5  1996/01/14 00:10:18  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.4  1995/09/17 07:53:04  jims
 * Remove "pointless comparison" from postcondition
 * Check first for less than 2 elements before checking if sorted
 *
 * Revision 7.3  1995/09/05  20:55:29  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/06/26  22:31:22  griswolf
 * Scopus 1351: Partition RWPOSTCONDITION into two parts for method
 * insert(). Also add isSorted() RWPRECONDITION to bsearch().
 *
 * Revision 7.1  1994/10/16  03:18:52  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.1  1994/04/15  19:48:03  vriezen
 * Move all files to 6.1
 *
 * Revision 1.6  1994/01/12  03:09:17  jims
 * Add constness to T* parameters where appropriate
 *
 * Revision 1.5  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.4  1993/07/08  03:42:18  keffer
 * Corrected logic error in RWDEBUG version of bsearch().
 *
 * Revision 1.3  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 1.1  1993/01/29  03:06:18  keffer
 * Initial revision
 *
 *
 ***************************************************************************
 */


#include "rw/epersist.h"

template <class T, class C>
RWTPtrSortedVector<T,C>::RWTPtrSortedVector(T*const* first, T*const* last) {
  typename container_type::iterator it = impl_.begin();
  while(first != last)
    it = insertSorted(it,*first++);
}


RWDEFINE_PERSISTABLE_TEMPLATE_2(RWTPtrSortedVector)

template <class T, class C, class S>
void rwSaveGuts(S& str, const RWTPtrSortedVector<T,C>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  for (size_t i=0; i < c.entries(); ++i)
    str << *c[i];
}

template <class T, class C, class S>
void rwRestoreGuts(S& str, RWTPtrSortedVector<T,C>& c)
{
  typedef typename RWTPtrSortedVector<T,C>::size_type size_type;
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

