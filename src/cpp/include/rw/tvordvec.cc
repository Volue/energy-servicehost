/*
 * Template definitions for RWTValOrderedVector<T>
 *
 * $Id: tvordvec.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.16  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.15  1996/08/09 19:41:38  hart
 * HP (aCC compiler) port
 *
 * Revision 7.14  1996/02/18 01:49:21  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.13  1996/02/12 21:03:06  kevinj
 * Switched stream and class args in rw*Guts.
 *
 * Revision 7.12  1996/01/25 01:51:32  kevinj
 * Improved ETP macros.
 *
 * Revision 7.11  1996/01/13 22:07:32  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.10  1995/09/17 07:49:11  jims
 * Don't adjust nitems_ until no possibility of bounds error
 *
 * Revision 7.9  1995/09/13  04:08:33  jims
 * Uncomment useful code
 *
 * Revision 7.8  1995/09/07  02:06:28  jims
 * Remove more diagnostic code
 *
 * Revision 7.7  1995/09/07  02:04:11  jims
 * Remove diagnostic code
 *
 * Revision 7.6  1995/09/05  21:25:43  jims
 * Use new copyright macro
 *
 * Revision 7.5  1995/09/01  19:56:16  jims
 * Scopus 1940: Changed variable to size_t to avoid msvc warning
 *
 * Revision 7.4  1995/08/01  20:28:11  jims
 * Move clear() out of line
 *
 * Revision 7.3  1995/08/01  20:00:39  jims
 * Scopus #2019.  Reset unused cells to default value when
 * removing or clearing elements.
 *
 * Revision 7.2  1995/06/27  17:10:44  jims
 * Use <class T> instead of <class TP> in boundsCheck mf template
 *
 * Revision 7.1  1994/10/16  03:22:16  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.1  1994/04/15  19:48:23  vriezen
 * Move all files to 6.1
 *
 * Revision 6.1.7.2  1994/04/20  22:14:27  vriezen
 * Bug #337 Fix bounds check for ordered and sorted vectors.
 *
 * Revision 6.1.7.1  1994/04/15  22:45:57  vriezen
 * Version 7, Initial code
 *
 * Revision 6.1  1994/04/15  19:48:23  vriezen
 * Move all files to 6.1
 *
 * Revision 1.5  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
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
 * Revision 1.1  1993/01/28  02:02:26  keffer
 * Initial revision
 *
 ***************************************************************************
 */

#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTValOrderedVector)

template <class T, class S>
void rwSaveGuts(S& str, const RWTValOrderedVector<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  for (typename RWTValOrderedVector<T>::size_type i=0; i < c.entries(); ++i)
    str << c[i];
}

template <class T, class S>
void rwRestoreGuts(S& str, RWTValOrderedVector<T>& c)
{
  typedef typename RWTValOrderedVector<T>::size_type size_type;
  size_type count;
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
