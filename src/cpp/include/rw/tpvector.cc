/*
 * Template definitions for RWTPtrVector<T>
 *
 * $Header$
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
 * Rogue Wave Software, Inc.
 * P.O. Box 2328
 * Corvallis, OR 97339
 *
 * Copyright (C) 1992, 1993.
 * This software is subject to copyright protection under the laws of
 * the United States and other countries.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:44  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.7  1996/12/03 22:22:10  griswolf
 * Fix bad eol in log 7.1 comment.
 *
 * Revision 7.6  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.5  1996/06/27 19:52:50  griswolf
 * *** empty log message ***
 *
 * Revision 7.4  1996/03/16 15:15:37  jims
 * Port non-stdlib template persistence to DEC and Watcom
 *
 * Revision 7.3  1996/02/12 21:03:06  kevinj
 * Switched stream and class args in rw*Guts.
 *
 * Revision 7.2  1996/01/29 18:03:02  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.1  1996/01/29 17:36:50  jims
 * Move to revision 7.1
 *
 * Revision 1.2  1993/02/17 18:28:08  keffer
 * Now obsolete.
 *
 * Revision 1.1  1993/01/28  02:39:38  keffer
 * Initial revision
 *
 *
 ***************************************************************************
 */

 
#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTPtrVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrVector,RWpistream,RWpostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrVector,RWbistream,RWbostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrVector,RWeistream,RWeostream)
#endif     

template <class T, class S>
void rwSaveGuts(S& str, const RWTPtrVector<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.length());
#else
  str << c.length();
#endif
  for (size_t i=0; i < c.length(); ++i)
    str << *c[i];
}

template <class T, class S>
void rwRestoreGuts(S& str, RWTPtrVector<T>& c)
{
  size_t count;
#ifdef RW_FIX_XSIZE_T
  str.getSizeT(count);
#else
  str >> count;
#endif
  c.reshape(0);
  c.reshape(count);
  for (size_t i=0; i < count; ++i) {
    T* p;
    str >> p;
    c[i] = (p);
  }
}
