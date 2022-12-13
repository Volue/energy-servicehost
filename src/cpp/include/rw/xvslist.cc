/*
 * Template definitions for RWTValSlist<T> and RWTValSlistIterator<T>
 *
 * $Id: xvslist.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.11  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.10  1996/08/26 21:50:37  hart
 * HP (aCC) compiler port
 *
 * Revision 7.9  1996/07/18 04:04:37  hart
 *  ObjectStore changes
 *
 * Revision 7.8  1996/03/16 15:15:37  jims
 * Port non-stdlib template persistence to DEC and Watcom
 *
 * Revision 7.7  1996/03/06 18:52:00  griswolf
 * Fix saveGuts to avoid copying data prior to streaming.
 *
 * Revision 7.6  1996/03/01 19:42:31  griswolf
 * cast away constness for iterator in rwSaveGuts... to avoid a copy.
 *
 * Revision 7.5  1996/02/21 23:38:47  griswolf
 * Fix an N^2 order saveGuts technique to make it linear.
 *
 * Revision 7.4  1996/02/21 19:39:44  griswolf
 * Scopus #2012: Fix problem with failure to remove(testfunction).
 *
 * Revision 7.3  1996/02/12 21:03:06  kevinj
 * Switched stream and class args in rw*Guts.
 *
 * Revision 7.2  1996/01/27 01:36:34  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.1  1995/11/13 12:30:40  jims
 * Move to revision 7.1
 *
 * Revision 7.2  1995/09/05  21:31:35  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:31:10  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.4  1994/07/26  19:11:28  jims
 * Fix copy ctor to use passed in argument instead of *this
 *
 * Revision 6.3  1994/07/12  20:05:34  vriezen
 * Updated Copyright
 *
 * Revision 6.2  1994/07/05  21:56:52  nevis
 * Fixes bug #117 by defining explicitly a copy constructor.
 *
 * Revision 6.2  1994/07/05  20:57:31  nevis
 * Fixes bug #117 by defining copy constructor for SGI compiler..
 *
 * Revision 6.1  1994/04/15  19:48:33  vriezen
 * Move all files to 6.1
 *
 * Revision 1.6  1993/11/08  21:14:01  jims
 * Port to ObjectStore
 *
 * Revision 1.5  1993/09/10  02:18:28  keffer
 * apply() now takes a function with a non-const reference, allowing
 * modification of the contained value.
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


// Copy constructor:
template <class T> RWTValSlist<T>::RWTValSlist(const RWTValSlist<T>& s)
{
  // Construct an iterator, casting away "constness"
  // (which we promise to honor anyway):
  RWTValSlistIterator<T> next((RWTValSlist<T>&)s);

  while (++next)
    append(next.key());

  RWPOSTCONDITION(s.entries()==entries());
}

template <class T> RWTValSlist<T>&
RWTValSlist<T>::operator=(const RWTValSlist<T>& s)
{
  if (this!=&s)
  {
    clear();
    // Construct an iterator, casting away "constness"
    // (which we promise to honor anyway):
    RWTValSlistIterator<T> next((RWTValSlist<T>&)s);

    while (++next)
      append(next.key());

  }
  RWPOSTCONDITION(s.entries()==entries());
  return *this;
}

template <class T> void
RWTValSlist<T>::append(const T& a)
{
  RWTIsvSlist<RWTValSlink<T> >
    ::append(RW_NEW(this, RWTValSlink<T>::get_os_typespec()) RWTValSlink<T>(a));
}

template <class T> void
RWTValSlist<T>::apply(void (*applyFun)(T&, void*), void* d)
{
  RWTValSlink<T>* link = firstLink();
  while (link != tailLink())
  {
    applyFun(link->info_, d);	// Apply the function
    link = link->next();	// Advance
  }
}

template <class T> RWBoolean
RWTValSlist<T>::contains(const T& val) const
{
  return findLeftVal(val)!=rwnil;
}

template <class T> RWBoolean
RWTValSlist<T>::contains(RWBoolean (*testFun)(const T&, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  return findLeftFun(testFun, d)!=rwnil;
}
  
template <class T> RWBoolean
RWTValSlist<T>::find(const T& val, T& k) const
{
  RWTValSlink<T>* link = firstLink();
  while (link!=tailLink())
  {
    if (link->info_ == val)
    {
      k = link->info_;
      return TRUE;
    }
    link = link->next();	// Advance
  }
  return FALSE;
}

template <class T> RWBoolean
RWTValSlist<T>::find(RWBoolean (*testFun)(const T&, void*), void* d, T& k) const
{
  RWPRECONDITION(testFun!=rwnil);
  RWTValSlink<T>* link = firstLink();
  while (link!=tailLink())
  {
    if (testFun(link->info_, d))
    {
      k = link->info_;
      return TRUE;
    }
    link = link->next();	// Advance
  }
  return FALSE;
}

template <class T> size_t
RWTValSlist<T>::index(const T& val) const
{
  size_t count = 0;
  RWTValSlink<T>* link = firstLink();
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
RWTValSlist<T>::index(RWBoolean (*testFun)(const T&, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  size_t count = 0;
  RWTValSlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link->info_, d))
      return count;
    ++count;
    link = link->next();	// Advance
  }
  return RW_NPOS;
}

template <class T> void
RWTValSlist<T>::insert(const T& a)
{
  RWTIsvSlist<RWTValSlink<T> >
    ::insert(RW_NEW(this, RWTValSlink<T>::get_os_typespec()) RWTValSlink<T>(a));
}

template <class T> void
RWTValSlist<T>::insertAt(size_t i, const T& a)
{

  RWTIsvSlist<RWTValSlink<T> >
    ::insertAt(i, RW_NEW(this, RWTValSlink<T>::get_os_typespec()) RWTValSlink<T>(a));
}

template <class T> size_t
RWTValSlist<T>::occurrencesOf(const T& val) const
{
  size_t count = 0;
  RWTValSlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (link->info_ == val)
      ++count;
    link = link->next();	// Advance
  }
  return count;
}

template <class T> size_t
RWTValSlist<T>::occurrencesOf(RWBoolean (*testFun)(const T&, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  size_t count = 0;
  RWTValSlink<T>* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link->info_, d))
      ++count;
    link = link->next();	// Advance
  }
  return count;
}

template <class T> void
RWTValSlist<T>::prepend(const T& a)
{
  RWTIsvSlist<RWTValSlink<T> >
    ::prepend(RW_NEW(this, RWTValSlink<T>::get_os_typespec()) RWTValSlink<T>(a));
}


// Non-const function allows us to use a sentinel:
template <class T> RWBoolean
RWTValSlist<T>::remove(const T& val)
{
  RWPRECONDITION(lastLink()->next()==tailLink());

  // Set up a sentinel:
  RWTValSlink<T> newTail(val);
  lastLink()->next_ = &newTail;

  // Now search for it:
  RWTValSlink<T>* link = headLink();
  while (!(link->next()->info_ == val))
    link = link->next();

  // Reset the pointer in the last link:
  lastLink()->next_ = tailLink();

  if (link != lastLink())
  {
    RWPOSTCONDITION(link->next()->info_ == val);
    delete removeRight(link);
    return TRUE;
  }
  return FALSE;
}

template <class T> RWBoolean
RWTValSlist<T>::remove(RWBoolean (*testFun)(const T&, void*), void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  RWTValSlink<T>* link = findLeftFun(testFun, d);
  if(link)
    link = removeRight(link);
  if (link)
  {
    RWPOSTCONDITION( testFun(link->info_, d) );
    delete link;
    return TRUE;
  }
  return FALSE;
}

template <class T> size_t
RWTValSlist<T>::removeAll(const T& val)
{
  size_t count = 0;
  RWTValSlink<T>* prev = headLink();
  while (prev != lastLink())
  {
    if (prev->next()->info_ == val)
    {
      // This will effectively advance the cursor--
      delete removeRight(prev);
      count++;
    }
    else
      prev = prev->next();
  }
  return count;
}

template <class T> size_t
RWTValSlist<T>::removeAll(RWBoolean (*testFun)(const T&, void*), void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  size_t count = 0;
  RWTValSlink<T>* prev = headLink();
  while (prev != lastLink())
  {
    if (testFun(prev->next()->info_, d))
    {
      // This will effectively advance the cursor--
      delete removeRight(prev);
      count++;
    }
    else
    {
      prev = prev->next();
    }
  }
  return count;
}

/****************************************************************
 *								*
 *		RWTValSlist<T> protected functions		*
 *								*
 ****************************************************************/


/*
 * Find the link *before* the link containing the indicated value.
 */
template <class T> RWTValSlink<T>*
RWTValSlist<T>::findLeftVal(const T& val) const
{
  RWTValSlink<T>* link = headLink();
  while (link != lastLink())
  {
    if (link->next()->info_ == val)
      return link;
    link = link->next();
  }
  return rwnil;
}

/*
 * Find the link *before* the link that tests true.
 */
template <class T> RWTValSlink<T>*
RWTValSlist<T>::findLeftFun(RWBoolean (*testFun)(const T&, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  RWTValSlink<T>* link = headLink();
  while (link != lastLink())
  {
    if (testFun(link->next()->info_, d))
      return link;
    link = link->next();
  }
  return rwnil;
}

/*
 * Extracts the value out of a link then throws the link away:
 */
template <class T> T
RWTValSlist<T>::peel(RWTValSlink<T>* link)
{
  RWPRECONDITION2(link, "RWTValSlist<T>::peel(RWTValSlink<T>*): nil link");

  T ret = link->info_;
  delete link;
  return ret;
}


/****************************************************************
 *								*
 *	Definitions for RWTValSlistIterator<T>			*
 *								*
 ****************************************************************/

template <class T> RWBoolean
RWTValSlistIterator<T>::findNext(const T& val)
{
  while (++(*this))
  {
    if (key()==val)
      return TRUE;
  }
  return FALSE;
}

template <class T> RWBoolean
RWTValSlistIterator<T>::findNext(RWBoolean (*testFun)(const T&, void*), void* d)
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
RWTValSlistIterator<T>::insertAfterPoint(const T& a)
{
  RWTIsvSlistIterator<RWTValSlink<T> >
    ::insertAfterPoint(RW_NEW(this, RWTValSlink<T>::get_os_typespec()) RWTValSlink<T>(a));
}

template <class T> RWBoolean
RWTValSlistIterator<T>::remove()
{
  RWTValSlink<T>* link = RWTIsvSlistIterator<RWTValSlink<T> >::remove();
  return link ? (delete link, TRUE) : FALSE;
}

template <class T> RWBoolean
RWTValSlistIterator<T>::removeNext(const T& val)
{
  while (cursor()->next() != container()->tailLink())
  {
    if (cursor()->next()->info_ == val)
    {
      delete container()->removeRight(cursor());
      return TRUE;
    }
    this->advance();
  }
  return FALSE;
}

template <class T> RWBoolean
RWTValSlistIterator<T>::removeNext(RWBoolean (*testFun)(const T&, void*), void* d)
{
  while (cursor()->next() != container()->tailLink())
  {
    if (testFun(cursor()->next()->info_, d))
    {
      delete container()->removeRight(cursor());
      return TRUE;
    }
    this->advance();
  }
  return FALSE;
}

template <class T>
RWTValSlistIterator<T>::RWTValSlistIterator(const RWTValSlistIterator<T>& si)
  : RWTIsvSlistIterator<RWTValSlink<T> >(si)
{ ; }


#include "rw/epersist.h"
// look for RWDEFINE_PERSISTABLE_things and rwInsertInXxxx()

RWDEFINE_PERSISTABLE_TEMPLATE(RWTValSlist)


template <class T, class S>
void rwRestoreGuts(S& str, RWTValSlist<T>& c)
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
void rwSaveGuts(RWvostream& str, const RWTValSlist<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  ((RWTValSlist<T>&)c).apply(rwInsertInStream,&str);
}
template <class T>
void rwSaveGuts(RWFile& str, const RWTValSlist<T>& c)
{
  str << c.entries();
  ((RWTValSlist<T>&)c).apply(rwInsertInFile,&str);
}

#else

RWDEFINE_PERSIST_TEMPLATE_IO(RWTValSlist,RWpistream,RWpostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValSlist,RWbistream,RWbostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValSlist,RWeistream,RWeostream)

template <class T, class S>
void rwSaveGuts(S& str, const RWTValSlist<T>& c)
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
