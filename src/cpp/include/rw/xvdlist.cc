/*
 * Template definitions for RWTValDlist<T> and RWTValDlistIterator<T>
 *
 * $Id: xvdlist.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.10  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.9  1996/08/26 21:51:23  hart
 * HP (aCC) compiler port
 *
 * Revision 7.8  1996/07/18 04:04:37  hart
 *  ObjectStore changes
 *
 * Revision 7.7  1996/03/16 15:15:37  jims
 * Port non-stdlib template persistence to DEC and Watcom
 *
 * Revision 7.6  1996/03/06 18:47:13  griswolf
 * Fix saveGuts to avoid copying data prior to streaming.
 *
 * Revision 7.5  1996/03/01 19:41:43  griswolf
 * cast away constness for iterator in rwSaveGuts... to avoid a copy.
 *
 * Revision 7.4  1996/02/21 23:37:41  griswolf
 * Fix an N^2 order saveGuts technique to make it linear.
 *
 * Revision 7.3  1996/02/12 21:03:06  kevinj
 * Switched stream and class args in rw*Guts.
 *
 * Revision 7.2  1996/01/27 00:48:35  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.1  1995/11/13 12:30:40  jims
 * Move to revision 7.1
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
 *
 ***************************************************************************
 */

template <class T> RWTValDlist<T>::RWTValDlist(const RWTValDlist<T>& s)
{
  // Construct an iterator, casting away "constness"
  // (which we promise to honor anyway):
  RWTValDlistIterator<T> next((RWTValDlist<T>&)s);
  while (++next) append(next.key());
  RWPOSTCONDITION(s.entries()==entries());
}

template <class T> RWTValDlist<T>&
RWTValDlist<T>::operator=(const RWTValDlist<T>& s)
{
  if (this!=&s)
  {
    clear();
    // Construct an iterator, casting away "constness"
    // (which we promise to honor anyway):
    RWTValDlistIterator<T> next((RWTValDlist<T>&)s);
    while (++next) append(next.key());
  }
  RWPOSTCONDITION(s.entries()==entries());
  return *this;
}

template <class T> void
RWTValDlist<T>::append(const T& a)
{
  RWTIsvDlist<RWTValDlink<T> >
    ::append(RW_NEW(this, RWTValDlink<T>::get_os_typespec()) RWTValDlink<T>(a));
}

template <class T> void
RWTValDlist<T>::apply(void (*applyFun)(T&, void*), void* d)
{
  RWTValDlink<T>* link = firstLink();
  while (link != tailLink())
  {
    applyFun(link->info_, d);	// Apply the function
    link = link->next();	// Advance
  }
}

template <class T> RWBoolean
RWTValDlist<T>::contains(const T& val) const
{
  return findVal(val)!=rwnil;
}

template <class T> RWBoolean
RWTValDlist<T>::contains(RWBoolean (*testFun)(const T&, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  return findFun(testFun, d)!=rwnil;
}

template <class T> RWBoolean
RWTValDlist<T>::find(const T& val, T& ret) const
{
  RWTValDlink<T>* link = findVal(val);
  return link ? (ret = link->info_, TRUE) : FALSE;
}

template <class T> RWBoolean
RWTValDlist<T>::find(RWBoolean (*testFun)(const T&, void*), void* d, T& ret) const
{
  RWPRECONDITION(testFun!=rwnil);

  RWTValDlink<T>* link = findFun(testFun, d);

  return link ? (ret = link->info_, TRUE) : FALSE;
}

template <class T> size_t
RWTValDlist<T>::index(const T& val) const
{
  size_t count = 0;
  RWTValDlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (link->info_ == val)
      return count;
    ++count;
    link = link->next();	// Advance
  }
  return RW_NPOS;
}

template <class T> size_t
RWTValDlist<T>::index(RWBoolean (*testFun)(const T&, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  size_t count = 0;
  RWTValDlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link->info_,d))
      return count;
    ++count;
    link = link->next();	// Advance
  }
  return RW_NPOS;
}

template <class T> void
RWTValDlist<T>::insert(const T& a)
{
  RWTIsvDlist<RWTValDlink<T> >::insert(RW_NEW(this, RWTValDlink<T>::get_os_typespec())
                                       RWTValDlink<T>(a));
}

template <class T> void
RWTValDlist<T>::insertAt(size_t i, const T& a)
{
  RWTIsvDlist<RWTValDlink<T> >
    ::insertAt(i, RW_NEW(this, RWTValDlink<T>::get_os_typespec()) RWTValDlink<T>(a));
}

template <class T> size_t
RWTValDlist<T>::occurrencesOf(const T& val) const
{
  size_t count = 0;
  RWTValDlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (link->info_ == val)
      ++count;
    link = link->next();	// Advance
  }
  return count;
}

template <class T> size_t
RWTValDlist<T>::occurrencesOf(RWBoolean (*testFun)(const T&, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  size_t count = 0;
  RWTValDlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link->info_, d))
      ++count;
    link = link->next();	// Advance
  }
  return count;
}

template <class T> void
RWTValDlist<T>::prepend(const T& a)
{
  RWTIsvDlist<RWTValDlink<T> >
    ::prepend(RW_NEW(this, RWTValDlink<T>::get_os_typespec()) RWTValDlink<T>(a));
}

template <class T> RWBoolean
RWTValDlist<T>::remove(const T& val)
{
  RWPRECONDITION(lastLink()->next()==tailLink());

  // Set up a sentinel:
  RWTValDlink<T> newTail(val);
  lastLink()->next_ = &newTail;

  // Now search for it:
  RWTValDlink<T>* link = firstLink();
  while (!(link->info_ == val))
    link = link->next();

  // Reset the pointer in the last link:
  lastLink()->next_ = tailLink();

  if (link != &newTail)
  {
    RWPOSTCONDITION(link->info_ == val);
    delete removeReference(link);
    return TRUE;
  }
  return FALSE;
}

template <class T> RWBoolean
RWTValDlist<T>::remove(RWBoolean (*testFun)(const T&, void*), void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  RWTValDlink<T>* link = removeReference(findFun(testFun, d));
  if (link)
  {
    RWPOSTCONDITION( testFun(link->info_, d) );
    delete link;
    return TRUE;
  }
  return FALSE;
}

template <class T> size_t
RWTValDlist<T>::removeAll(const T& val)
{
  size_t count = 0;
  RWTValDlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (link->info_ == val)
    {
      RWTValDlink<T>* next = link->next();
      delete removeReference(link);
      link = next;
      count++;
    }
    else
      link = link->next();
  }
  return count;
}

template <class T> size_t
RWTValDlist<T>::removeAll(RWBoolean (*testFun)(const T&, void*), void* d)
{
  RWPRECONDITION(testFun != rwnil);

  size_t count = 0;
  RWTValDlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link->info_ ,d))
    {
      RWTValDlink<T>* next = link->next();
      delete removeReference(link);
      link = next;
      count++;
    }
    else
      link = link->next();
  }
  return count;
}

/***********************************************************
 *							   *
 *	    RWTValDlist<T> protected functions		   *
 *							   *
 ***********************************************************/

/*
 * Find the link containing the indicated value.
 */
template <class T> RWTValDlink<T>*
RWTValDlist<T>::findVal(const T& val) const
{
  RWTValDlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (link->info_ == val)
      return link;
    link = link->next();
  }
  return rwnil;
}

/*
 * Find the link that contains info_ that tests true.
 */
template <class T> RWTValDlink<T>*
RWTValDlist<T>::findFun(RWBoolean (*testFun)(const T&, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  RWTValDlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link->info_, d))
      return link;
    link = link->next();
  }
  return rwnil;
}

/*
 * Extracts the value out of a link then throws the link away:
 */
template <class T> T
RWTValDlist<T>::peel(RWTValDlink<T>* link)
{
  RWPRECONDITION2(link, "RWTValDlist<T>::peel(RWTValDlink<T>*): nil link");

  T ret = link->info_;
  delete link;
  return ret;
}

/****************************************************************
 *								*
 *	Definitions for RWTValDlistIterator<T>			*
 *								*
 ****************************************************************/

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
RWTValDlistIterator<T>::findNext(RWBoolean (*testFun)(const T&, void*), void* d)
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
  RWTIsvDlistIterator<RWTValDlink<T> >
    ::insertAfterPoint(RW_NEW(this, RWTValDlink<T>::get_os_typespec()) RWTValDlink<T>(a));
}

template <class T> RWBoolean
RWTValDlistIterator<T>::remove()
{
  RWTValDlink<T>* link = RWTIsvDlistIterator<RWTValDlink<T> >::remove();
  return link ? (delete link, TRUE) : FALSE;
}

template <class T> RWBoolean
RWTValDlistIterator<T>::removeNext(const T& val)
{
  while (cursor()->next() != container()->tailLink())
  {
    if (cursor()->next()->info_ == val)
    {
      delete container()->removeReference(cursor()->next());
      return TRUE;
    }
    this->advance();
  }
  return FALSE;
}

template <class T> RWBoolean
RWTValDlistIterator<T>::removeNext(RWBoolean (*testFun)(const T&, void*), void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  while (cursor()->next() != container()->tailLink())
  {
    if (testFun(cursor()->next()->info_ , d))
    {
      delete container()->removeReference(cursor()->next());
      return TRUE;
    }
    this->advance();
  }
  return FALSE;
}

// Simple External Template Persistence.

#include "rw/epersist.h"
// look for RWDEFINE_PERSISTABLE_things and rwInsertInXxxx()

RWDEFINE_PERSISTABLE_TEMPLATE(RWTValDlist)

template <class T, class S>
void rwRestoreGuts(S& str, RWTValDlist<T>& c)
{
  size_t count;
#ifdef RW_FIX_XSIZE_T
  str.getSizeT(count);
#else
  str >> count;
#endif
  c.clear();
  for (size_t i=0; i < count; ++i) {
    T t;
    str >> t;
    c.insert(t);
  }
}

#ifndef RW_NO_TEMPLINST_ON_BASE
template <class T>
void rwSaveGuts(RWvostream& str, const RWTValDlist<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  ((RWTValDlist<T>&)c).apply(rwInsertInStream,&str);
}

template <class T>
void rwSaveGuts(RWFile& str, const RWTValDlist<T>& c)
{
  str << c.entries();
  ((RWTValDlist<T>&)c).apply(rwInsertInFile,&str);
}

#else

RWDEFINE_PERSIST_TEMPLATE_IO(RWTValDlist,RWpistream,RWpostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValDlist,RWbistream,RWbostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValDlist,RWeistream,RWeostream)

template <class T, class S>
void rwSaveGuts(S& str, const RWTValDlist<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  for (size_t i=0; i < c.entries(); ++i)
    str << c[i];
}
#endif
