/*
 * Template definitions for RWTPtrHashSet
 *
 * $Id: tphset.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.10  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.9  1996/08/09 19:22:11  hart
 * HP (aCC compiler) port
 *
 * Revision 7.8  1996/02/18 01:47:38  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.7  1996/02/12 21:03:06  kevinj
 * Switched stream and class args in rw*Guts.
 *
 * Revision 7.6  1996/01/25 01:51:32  kevinj
 * Improved ETP macros.
 *
 * Revision 7.5  1996/01/16 22:37:38  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.4  1995/09/05 20:22:38  jims
 * Use new copyright macro
 *
 * Revision 7.3  1995/06/06  19:21:20  griswolf
 * Bug ID:1753 - fix typo on declaration of iterator.
 *
 * Revision 7.2  1995/05/01  19:20:15  griswolf
 * remove unneeded new/delete in intersection code
 *
 * Revision 7.1  1994/10/16  03:14:47  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.1.7.3  1994/09/20  16:33:54  nevis
 * Added set operations.
 *
 * Revision 6.3  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.2  1994/06/21  00:47:53  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 6.1  1994/04/15  19:47:51  vriezen
 * Move all files to 6.1
 *
 * Revision 1.2  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.1  1993/06/03  20:59:23  griswolf
 * Initial revision
 *
 */


#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE_3(RWTPtrHashSet)

template <class T, class H, class EQ, class S>
void rwSaveGuts(S& str, const RWTPtrHashSet<T,H,EQ>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  typename RWTPtrHashSet<T,H,EQ>::const_iterator it = c.begin();

  while( it != c.end() ) {
    str << *(*it);
    it++;
  }
}

template <class T, class H, class EQ, class S>
void rwRestoreGuts(S& str, RWTPtrHashSet<T,H,EQ>& c)
{
  typedef typename RWTPtrHashSet<T,H,EQ>::size_type size_type;
  size_type count;
#ifdef RW_FIX_XSIZE_T
  str.getSizeT(count);
#else
  str >> count;
#endif
  c.clear();
  for (size_type i=0; i < count; ++i) {
    T*  dataP;
    str >> dataP;
    c.insert(dataP);
  }
}
