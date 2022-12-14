/*
 * Template definitions for RWTIsvDlist<TL> and RWTIsvDlistIterator<TL>
 *
 * $Id: tidlist.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Commercial Computer Software ? Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.3  1996/02/18 01:46:56  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 20:06:13  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:13:58  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.1  1994/04/15  19:11:09  vriezen
 * Move all files to 6.1
 *
 * Revision 1.4  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.3  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 1.2  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 1.1  1993/01/27  21:38:32  keffer
 * Initial revision
 ***************************************************************************
 */

template <class TL> void
RWTIsvDlist<TL>::apply(void (*applyFun)(TL*, void*), void* p)
{
  TL* link = firstLink();
  while (link != tailLink())
  {
    (*applyFun)(link, p);       // Apply the function
    link = (TL*)link->next();   // Advance
  }
}
template <class TL> void
RWTIsvDlist<TL>::clearAndDestroy()
{
  TL* n;
  TL* link = firstLink();
  while (link != tailLink())
  {
    n = (TL*)link->next();
    delete link;
    link = n;
  }
  init();
}
  
template <class TL> TL*
RWTIsvDlist<TL>::find(RWBoolean (*testFun)(const TL*, void*), void* d) const
{
  TL* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link, d))
      return link;
    link = (TL*)link->next();
  }
  return rwnil;
}


template <class TL> size_t
RWTIsvDlist<TL>::index(RWBoolean (*testFun)(const TL*, void*), void* d) const
{
  size_t count = 0;
  const TL* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link, d))
      return count;
    link = (const TL*)link->next();
    ++count;
  }
  return RW_NPOS;
}

template <class TL> size_t
RWTIsvDlist<TL>::occurrencesOf(RWBoolean (*testFun)(const TL*, void*), void* d) const
{
  size_t count = 0;
  const TL* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link, d))
      ++count;
    link = (const TL*)link->next();
  }
  return count;
}

template <class TL> TL*
RWTIsvDlist<TL>::remove(RWBoolean (*testFun)(const TL*, void*), void* d)
{
  TL* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link, d))
      return removeReference(link);
    link = (TL*)link->next();
  }
  return rwnil;
}

/****************************************************************
 *                                                              *
 *      Definitions for RWTIsvDlistIterator<TL>                 *
 *                                                              *
 ****************************************************************/


/*
 * Return first occurrence where the tester returns true.
 */
template <class TL> TL*
RWTIsvDlistIterator<TL>::findNext(RWBoolean (*testFun)(const TL*, void*), void* d)
{
  TL* p;
  while ( (p= (*this)()) != 0 )
  {
    if (testFun(p, d))
      return p;
  }
  return rwnil;
}

/*
 * Remove and return first occurrence where the tester returns true.
 * In a doubly-linked list, we are unencumbered by having to remember
 * the previous link.
 */
template <class TL> TL*
RWTIsvDlistIterator<TL>::removeNext(RWBoolean (*testFun)(const TL*, void*), void* d)
{
  return findNext(testFun, d) ? remove() : rwnil;
}
