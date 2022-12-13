#ifndef  __RWDLIST_H__
#define  __RWDLIST_H__

/*
 * Declarations for Doubly-linked lists.
 *
 * $Id: dlist.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:40  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.6  1996/07/18 04:30:11  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/08 17:00:00  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:39:56  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/12/05 23:09:51  pearson
 * Remove ZTC ifdef
 *
 * Revision 7.2  1995/09/05 18:30:55  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:48:44  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.5  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.4  1994/06/28  19:23:46  nevis
 * Fixed out of line virtual dtor.
 *
 * Revision 6.3  1994/06/20  16:52:07  nevis
 * Added out of line virtual dtor.
 *
 * Revision 6.2  1994/06/02  16:59:13  foote
 * Port to Symantec 7.0 on Macintosh
 *
 * Revision 6.1  1994/04/15  19:09:51  vriezen
 * Move all files to 6.1
 *
 * Revision 2.8  1993/11/08  08:58:51  jims
 * Port to ObjectStore
 *
 * Revision 2.7  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.6  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.5  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.4  1993/02/13  21:49:01  keffer
 * Ported to Zortech; required public derivation.
 *
 * Revision 2.3  1993/02/11  22:47:20  keffer
 * Ported to IBM xlC compiler
 *
 * Revision 2.2  1993/01/25  18:21:06  keffer
 * Simplified, improving performance
 *
 *    Rev 1.3   25 May 1992 15:49:44   KEFFER
 * inline -> Inline.
 * 
 *    Rev 1.2   18 Feb 1992 09:54:18   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:12   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:14:22   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 */

#include "rw/idlist.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

/*
 * This is the actual link which includes a pointer
 */
class RWExport RWPDlink : public RWIsvDlink
{

public:

  RWPDlink(void* a) : info_(a) {;}
  RWPDlink*     next() const  {return (RWPDlink*)next_;}

  void*         info_;
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *                                                              *
 *              Declarations for RWDlist                        *
 *                                                              *
 ****************************************************************/

class RWExport RWDlist : private RWIsvDlist
{

public:

  RWDlist()             {;}
  RWDlist(void* a) : RWIsvDlist() { append(a); }

  RWDlist(const RWDlist&);
  ~RWDlist()            {clear();} 

  // Operators:
  RWDlist&              operator=(const RWDlist&);

  // Member functions:
  void*                 append(void* a);
  void                  apply(RWapplyGeneric, void*);
  void                  applyReference(void (*apRef)(void*&, void*), void*);
  void*&                at(size_t n)    {return ((RWPDlink*)RWIsvDlist::at(n))->info_;}
#ifndef RW_NO_CONST_OVERLOAD
  const void*           at(size_t n) const      {return ((RWPDlink*)RWIsvDlist::at(n))->info_;}
#endif
  void                  clear();
  RWBoolean             contains(RWtestGeneric tfun, const void* c) const
        {return (find(tfun,c) != rwnil);}
  RWBoolean             containsReference(const void* c) const
        {return (findReference(c) != rwnil);}
#ifdef RW_NO_ACCESS_ADJUSTMENT
  size_t                entries() const { return RWIsvDlist::entries(); }
#else
  RWIsvDlist::entries;
#endif
  void*                 find(RWtestGeneric, const void*) const;
  void*                 findReference(const void*) const; // First occurrence
  void*                 first() const;
  void*                 get()   {return peel((RWPDlink*)RWIsvDlist::get());}
  size_t                index(RWtestGeneric, const void*) const;
  void*                 insert(void* a) { return append(a); }
  void*                 insertAfter(int, void*); // For backwards compatiblity
  void*                 insertAt(size_t, void*);
#ifdef RW_NO_ACCESS_ADJUSTMENT
  RWBoolean             isEmpty() const { return RWIsvDlist::isEmpty(); }
#else
  RWIsvDlist::isEmpty;
#endif
  void*                 last() const;
  size_t                occurrencesOf(RWtestGeneric, const void*) const;
  size_t                occurrencesOfReference(const void*) const;
  void*                 prepend(void* a);
  void*                 remove(RWtestGeneric, const void*);
  void*                 removeReference(const void*);

protected:

  RWPDlink*             headLink()  const;
  RWPDlink*             tailLink()  const;
  RWPDlink*             firstLink() const {return (RWPDlink*)head_.next_;}
  RWPDlink*             lastLink()  const {return (RWPDlink*)tail_.prev_;}

  // Type safe interface to base class:
  void                  insertAfterLink(RWPDlink* p, RWPDlink* v)
        {RWIsvDlist::insertAfterLink(p,v);}

  static void*          peel(RWPDlink*);

private:

friend class RWExport RWDlistIterator;

  RW_TYPESPEC  /* This macro usually expands to nothing */
};


/*
 * Declaration for iterator for doubly-linked list.
 */
class RWExport RWDlistIterator : RWBehavior2 private RWIsvDlistIterator
{

public:

  // Constructor: cast needed for Glock V2.0b:
  RWDlistIterator(RWDlist& s) : RWIsvDlistIterator((RWIsvDlist&)s) {;}
  virtual ~RWDlistIterator();

  void*                 operator++();
  void*                 operator--();
  void*                 operator+=(size_t n);
  void*                 operator-=(size_t n);
  void*                 operator()();

#ifdef RW_NO_ACCESS_ADJUSTMENT
  RWBoolean             atFirst() const {return RWIsvDlistIterator::atFirst();}
  RWBoolean             atLast() const  {return RWIsvDlistIterator::atLast(); }
  void                  toFirst()       {RWIsvDlistIterator::toFirst();       }
  void                  toLast()        {RWIsvDlistIterator::toLast();        }
#else
  RWIsvDlistIterator::atFirst;
  RWIsvDlistIterator::atLast;
  RWIsvDlistIterator::toFirst;
  RWIsvDlistIterator::toLast;
#endif

  // Methods:
  RWDlist*              container() const       {return (RWDlist*)dlist_;}
  void*                 findNext(RWtestGeneric, const void*);
  void*                 findNextReference(const void*);
  void*                 insertAfterPoint(void*);
  void*                 key() const
        {return isActive() ? cursor()->info_ : rwnil;}
  void*                 remove()
        {return RWDlist::peel((RWPDlink*)RWIsvDlistIterator::remove());}
  void*                 removeNext(RWtestGeneric, const void*);
  void*                 removeNextReference(const void*);
  void                  reset()                 {RWIsvDlistIterator::reset();}
  void                  reset(RWDlist& sl)      {RWIsvDlistIterator::reset((RWIsvDlist&)sl);}

protected:

  RWPDlink*             cursor() const          {return (RWPDlink*)dhere_;}

#ifndef RW_NO_POSTFIX
  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place. (or worse)
  void                  operator++(int);
  void                  operator--(int);
#endif  

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

//////////////////////////////////////////////////////////
//                                                      //
//                      INLINES                         //
//                                                      //
//////////////////////////////////////////////////////////

inline void*
RWDlist::first() const  {
  return isEmpty() ? rwnil : firstLink()->info_;
}

inline void*
RWDlist::last() const   {
  return isEmpty() ? rwnil : lastLink()->info_;
}

inline RWPDlink*
RWDlist::headLink() const {
  // Cast away constness to suppress unfounded cfront warning:
  return (RWPDlink*)&((RWDlist*)this)->head_;
}

inline RWPDlink*
RWDlist::tailLink() const {
  // Cast away constness to suppress unfounded cfront warning:
  return (RWPDlink*)&((RWDlist*)this)->tail_;
}

inline void*
RWDlistIterator::operator++()
{
  // Make sure tail link points to itself:
  RWPRECONDITION(container()->tailLink()->next()==container()->tailLink());

  advance();
  return cursor() == container()->tailLink() ? rwnil : cursor()->info_;
}

inline void*
RWDlistIterator::operator--()
{
  // Make sure head link points to itself:
  RWPRECONDITION(container()->headLink()->prev()==container()->headLink());

  backup();
  return cursor() == container()->headLink() ? rwnil : cursor()->info_;
}

inline void*
RWDlistIterator::operator()()
{
  return ++(*this);
}

#endif  /* __RWDLIST_H__ */
