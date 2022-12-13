/*
 * Template definitions for RWTPtrSlist<T> and RWTPtrSlistIterator<T>
 *
 * $Id: xpslist.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.10  1996/08/26 21:54:01  hart
 * HP (aCC) compiler port
 *
 * Revision 7.9  1996/07/18 04:04:37  hart
 *  ObjectStore changes
 *
 * Revision 7.8  1996/03/16 15:15:37  jims
 * Port non-stdlib template persistence to DEC and Watcom
 *
 * Revision 7.7  1996/03/01 19:40:17  griswolf
 * cast away constness for iterator in rwSaveGuts... to avoid a copy.
 *
 * Revision 7.6  1996/02/26 22:07:11  sanders
 * Correct typeo.
 *
 * Revision 7.5  1996/02/21 23:35:00  griswolf
 * Fix an N^2 order saveGuts technique to make it linear.
 *
 * Revision 7.4  1996/02/21 19:47:24  griswolf
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
 * Revision 7.2  1995/09/05 20:55:06  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:18:41  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.4  1994/07/26  19:11:28  jims
 * Fix copy ctor to use passed in argument instead of *this
 *
 * Revision 6.3  1994/07/12  20:05:34  vriezen
 * Updated Copyright
 *
 * Revision 6.2  1994/07/05  21:56:40  nevis
 * Fixes bug #117 by defining explicitly a copy constructor.
 *
 * Revision 6.2  1994/07/05  20:57:21  nevis
 * Fixes bug #117 by defining copy constructor for SGI compiler..
 *
 * Revision 6.1  1994/04/15  19:48:00  vriezen
 * Move all files to 6.1
 *
 * Revision 1.8  1994/01/12  03:36:46  jims
 * Add const to parameter of findLeftVal
 *
 * Revision 1.7  1994/01/12  03:09:17  jims
 * Add constness to T* parameters where appropriate
 *
 * Revision 1.6  1993/11/08  13:37:24  jims
 * Port to ObjectStore
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
 * Revision 1.2  1993/01/28  01:35:03  keffer
 * Corrected error in removeNext()
 *
 * Revision 1.1  1993/01/27  21:38:32  keffer
 * Initial revision
 *
 *
 ***************************************************************************
 */

template <class TP> RWTPtrSlist<TP>::RWTPtrSlist(const RWTPtrSlist<TP>& s)
{
  // Construct an iterator, casting away "constness"
  // (which we promise to honor anyway):
  RWTPtrSlistIterator<TP> next((RWTPtrSlist<TP>&)s);
  TP* p;

  while ((p = next())!=rwnil)
    append(p);

  RWPOSTCONDITION(s.entries()==entries());
}

template <class TP> RWTPtrSlist<TP>&
RWTPtrSlist<TP>::operator=(const RWTPtrSlist<TP>& s)
{
  if (this!=&s)
  {
    clear();
    // Construct an iterator, casting away "constness"
    // (which we promise to honor anyway):
    RWTPtrSlistIterator<TP> next((RWTPtrSlist<TP>&)s);
    TP* p;

    while ((p = next())!=rwnil)
      append(p);
  }

  RWPOSTCONDITION(s.entries()==entries());
  return *this;
}

template <class TP> void
RWTPtrSlist<TP>::append(TP* p)
{
  RWTIsvSlist<RWTPtrSlink<TP> >::append(RW_NEW(this,RWTPtrSlink<TP>::get_os_typespec())
                                        RWTPtrSlink<TP>(p));
}

template <class TP> void
RWTPtrSlist<TP>::apply(void (*applyFun)(TP*, void*), void* d)
{
  RWTPtrSlink<TP>* link = firstLink();
  while (link != tailLink())
  {
    applyFun(link->info_, d);	// Apply the function
    link = link->next();	// Advance
  }
}

template <class TP> void
RWTPtrSlist<TP>::clearAndDestroy()
{
  RWTPtrSlink<TP>* link = firstLink();
  while (link != tailLink())
  {
    RWTPtrSlink<TP>* next = link->next();
    delete link->info_;
    delete link;
    link = next;
  }
  this->init();
}

template <class TP> RWBoolean
RWTPtrSlist<TP>::contains(const TP* p) const
{
  return findLeftVal(p)!=rwnil;
}

template <class TP> RWBoolean
RWTPtrSlist<TP>::contains(RWBoolean (*testFun)(TP*, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  return findLeftFun(testFun, d)!=rwnil;
}

template <class TP> TP*
RWTPtrSlist<TP>::find(const TP* p) const
{
  RWPRECONDITION(p != rwnil);

  RWTPtrSlink<TP>* link = firstLink();
  while (link!=tailLink())
  {
    if (*link->info_ == *p)
    {
      return link->info_;
    }
    link = link->next();	// Advance
  }
  return rwnil;
}

template <class TP> TP*
RWTPtrSlist<TP>::find(RWBoolean (*testFun)(TP*, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  RWTPtrSlink<TP>* link = firstLink();
  while (link!=tailLink())
  {
    if (testFun(link->info_, d))
    {
      return link->info_;
    }
    link = link->next();	// Advance
  }
  return rwnil;
}

template <class TP> size_t
RWTPtrSlist<TP>::index(const TP* p) const
{
  RWPRECONDITION(p != rwnil);

  size_t count = 0;
  RWTPtrSlink<TP>* link = firstLink();
  while (link != tailLink())
  {
    if (*link->info_ == *p)
      return count;
    ++count;
    link = link->next();	// Advance
  }
  return RW_NPOS;
}

template <class TP> size_t
RWTPtrSlist<TP>::index(RWBoolean (*testFun)(TP*, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  size_t count = 0;
  RWTPtrSlink<TP>* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link->info_,d))
      return count;
    ++count;
    link = link->next();	// Advance
  }
  return RW_NPOS;
}

template <class TP> void
RWTPtrSlist<TP>::insert(TP* a)
{
  RWTIsvSlist<RWTPtrSlink<TP> >::insert(RW_NEW(this, RWTPtrSlink<TP>::get_os_typespec())
                                        RWTPtrSlink<TP>(a));
}

template <class TP> void
RWTPtrSlist<TP>::insertAt(size_t i, TP* a)
{
  RWTIsvSlist<RWTPtrSlink<TP> >
    ::insertAt(i, RW_NEW(this, RWTPtrSlink<TP>::get_os_typespec()) RWTPtrSlink<TP>(a));
}

template <class TP> size_t
RWTPtrSlist<TP>::occurrencesOf(const TP* p) const
{
  RWPRECONDITION(p != rwnil);

  size_t count = 0;
  RWTPtrSlink<TP>* link = firstLink();
  while (link != tailLink())
  {
    if (*link->info_ == *p)
      ++count;
    link = link->next();	// Advance
  }
  return count;
}

template <class TP> size_t
RWTPtrSlist<TP>::occurrencesOf(RWBoolean (*testFun)(TP*, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  size_t count = 0;
  RWTPtrSlink<TP>* link = firstLink();
  while (link != tailLink())
  {
    if (testFun(link->info_, d))
      ++count;
    link = link->next();	// Advance
  }
  return count;
}

template <class TP> void
RWTPtrSlist<TP>::prepend(TP* a)
{
  RWTIsvSlist<RWTPtrSlink<TP> >
    ::prepend(RW_NEW(this, RWTPtrSlink<TP>::get_os_typespec()) RWTPtrSlink<TP>(a));
}

template <class TP> TP*
RWTPtrSlist<TP>::remove(const TP* p)
{
  RWPRECONDITION(p != rwnil);
  RWPRECONDITION(lastLink()->next()==tailLink());

  TP* ret = rwnil;

  // Set up a sentinel (casting away const: DO NOT MODIFY *newTail.info_):
  RWTPtrSlink<TP> newTail((TP*) p);
  lastLink()->next_ = &newTail;

  // Now search for it:
  RWTPtrSlink<TP>* link = headLink();
  while (!(*link->next()->info_ == *p))
    link = link->next();

  // Reset the pointer in the last link:
  lastLink()->next_ = tailLink();

  if (link != lastLink())
  {
    RWTPtrSlink<TP>* victim = removeRight(link);
    ret = victim->info_;
    delete victim;
  }
  RWPOSTCONDITION(ret==rwnil || *ret == *p);
  return ret;
}

template <class TP> TP*
RWTPtrSlist<TP>::remove(RWBoolean (*testFun)(TP*, void*), void* d)
{
  RWPRECONDITION(testFun!=rwnil);
  TP *ret = rwnil;
  RWTPtrSlink<TP>* link = findLeftFun(testFun,d);
  if(link)
    ret = peel(removeRight(link));

  RWPOSTCONDITION( ret==rwnil || testFun(ret, d) );

  return ret;
}

template <class TP> size_t
RWTPtrSlist<TP>::removeAll(const TP* p)
{
  RWPRECONDITION(p != rwnil);

  size_t count = 0;
  RWTPtrSlink<TP>* prev = headLink();
  while (prev != lastLink())
  {
    if (*prev->next()->info_ == *p)
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

template <class TP> size_t
RWTPtrSlist<TP>::removeAll(RWBoolean (*testFun)(TP*, void*), void* d)
{
  RWPRECONDITION(testFun != rwnil);

  size_t count = 0;
  RWTPtrSlink<TP>* prev = headLink();
  while (prev != lastLink())
  {
    if (testFun(prev->next()->info_, d))
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

/***********************************************************
 *							   *
 *	    RWTPtrSlist<TP> protected functions		   *
 *							   *
 ***********************************************************/

/*
 * Find the link *before* the link containing the indicated value.
 */
template <class TP> RWTPtrSlink<TP>*
RWTPtrSlist<TP>::findLeftVal(const TP* p) const
{
  RWPRECONDITION(p != rwnil);

  RWTPtrSlink<TP>* link = headLink();
  while (link != lastLink())
  {
    if (*link->next()->info_ == *p)
      return link;
    link = link->next();
  }
  return rwnil;
}

/*
 * Find the link *before* the link that tests true.
 */
template <class TP> RWTPtrSlink<TP>*
RWTPtrSlist<TP>::findLeftFun(RWBoolean (*testFun)(TP*, void*), void* d) const
{
  RWPRECONDITION(testFun!=rwnil);

  RWTPtrSlink<TP>* link = headLink();
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
template <class TP> TP*
RWTPtrSlist<TP>::peel(RWTPtrSlink<TP>* link)
{
  if (!link) return rwnil;
  TP* ret = link->info_;
  delete link;
  return ret;
}

/****************************************************************
 *								*
 *	Definitions for RWTPtrSlistIterator<TP>			*
 *								*
 ****************************************************************/

// Copy ctor for BUG#117
template <class TP>
RWTPtrSlistIterator<TP>::RWTPtrSlistIterator(const RWTPtrSlistIterator<TP>& si)
  : RWTIsvSlistIterator<RWTPtrSlink<TP> >(si)
{ ; }


template <class TP> TP*
RWTPtrSlistIterator<TP>::findNext(const TP* p)
{
  RWPRECONDITION(p != rwnil);

  TP* a;
  while ( (a = (*this)()) != rwnil)
  {
    if (*a == *p)
      return a;
  }
  return rwnil;
}

template <class TP> TP*
RWTPtrSlistIterator<TP>::findNext(RWBoolean (*testFun)(TP*, void*), void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  TP* a;
  while ( (a = (*this)()) != rwnil)
  {
    if (testFun(a, d))
      return a;
  }
  return rwnil;
}

template <class TP> void
RWTPtrSlistIterator<TP>::insertAfterPoint(TP* a)
{
  RWTIsvSlistIterator<RWTPtrSlink<TP> >
    ::insertAfterPoint(RW_NEW(this, RWTPtrSlink<TP>::get_os_typespec())
                       RWTPtrSlink<TP>(a));
}

template <class TP> TP*
RWTPtrSlistIterator<TP>::remove()
{
  return RWTPtrSlist<TP>::peel(RWTIsvSlistIterator<RWTPtrSlink<TP> >::remove());
}

template <class TP> TP*
RWTPtrSlistIterator<TP>::removeNext(const TP* p)
{
  RWPRECONDITION(p != rwnil);

  while (cursor()->next() != container()->tailLink())
  {
    if (*cursor()->next()->info_ == *p)
    {
      return RWTPtrSlist<TP>::peel(container()->removeRight(cursor()));
    }
    this->advance();
  }
  return rwnil;
}

template <class TP> TP*
RWTPtrSlistIterator<TP>::removeNext(RWBoolean (*testFun)(TP*, void*), void* d)
{
  RWPRECONDITION(testFun!=rwnil);

  while (cursor()->next() != container()->tailLink())
  {
    if (testFun(cursor()->next()->info_, d))
    {
      return RWTPtrSlist<TP>::peel(container()->removeRight(cursor()));
    }
    this->advance();
  }
  return rwnil;
}

#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTPtrSlist)

#ifdef RW_NO_TEMPLINST_ON_BASE
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrSlist,RWpistream,RWpostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrSlist,RWbistream,RWbostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrSlist,RWeistream,RWeostream)
#endif     


template <class TP, class S>
void rwSaveGuts(S& str, const RWTPtrSlist<TP>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  RWTPtrSlistIterator<TP> next((RWTPtrSlist<TP>&)c);
  TP* p;
  while (rwnil != (p = next()))
    str << *p;
}

template <class TP, class S>
void rwRestoreGuts(S& str, RWTPtrSlist<TP>& c)
{
  size_t count;
#ifdef RW_FIX_XSIZE_T
  str.getSizeT(count);
#else
  str >> count;
#endif
  c.clear();
  for (size_t i=0; i < count; ++i) {
    TP* p;
    str >> p;
    c.insert(p);
  }
}





