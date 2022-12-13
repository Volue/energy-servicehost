#ifndef  __RWSLIST_H__
#define  __RWSLIST_H__
/*
 * RWSlist: A singly linked list of pointers to void
 *
 * $Id: slist.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.6  1996/07/18 04:16:22  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/07 21:43:48  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:46:36  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/12/05 23:07:59  pearson
 * Remove ZTC ifdef
 *
 * Revision 7.2  1995/09/05 19:42:55  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:57:29  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.6  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.5  1994/06/28  19:20:01  nevis
 * Fixed out of line dtor.
 *
 * Revision 6.4  1994/06/20  16:56:12  nevis
 * Added out of line virtual dtor.
 *
 * Revision 6.3  1994/06/18  01:01:26  myersn
 * move dtor out-of-line.
 *
 * Revision 6.2  1994/06/02  18:24:24  foote
 * Port to Symantec 7.0 on Macintosh
 *
 * Revision 6.1  1994/04/15  19:10:57  vriezen
 * Move all files to 6.1
 *
 * Revision 2.9  1993/11/04  13:48:54  jims
 * Port to ObjectStore
 *
 * Revision 2.8  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.7  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.6  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.5  1993/02/13  21:49:01  keffer
 * Ported to Zortech; required public derivation.
 *
 * Revision 2.4  1993/02/11  22:47:20  keffer
 * Ported to IBM xlC compiler
 *
 * Revision 2.3  1993/01/25  18:21:06  keffer
 * Simplified, improving performance
 */

#include "rw/islist.h"

/*
 * This is the actual link which includes a pointer
 */
#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWPSlink : public RWIsvSlink
{

public:

  RWPSlink(void* a) : info_(a) {;}
  RWPSlink*     next() const  {return (RWPSlink*)next_;}

  void*         info_;

  RW_TYPESPEC  /* This macro usually expands to nothing */

};

/****************************************************************
 *                                                              *
 *              Declarations for RWSlist                        *
 *                                                              *
 ****************************************************************/

class RWExport RWSlist : private RWIsvSlist
{

public:

  RWSlist() {;}
  RWSlist(void* a)  
    : RWIsvSlist() {
      append(a);
    }
  RWSlist(const RWSlist&);
  ~RWSlist();

  // Operators:
  RWSlist&              operator=(const RWSlist& s);

  // Member functions:
  void*                 append(void* a);
  void                  apply(RWapplyGeneric, void*);
  void                  applyReference(void (*apRef)(void*&, void*), void*);
  void*&                at(size_t n)    {return ((RWPSlink*)RWIsvSlist::at(n))->info_;}
#ifndef RW_NO_CONST_OVERLOAD
  const void*           at(size_t n) const      {return ((RWPSlink*)RWIsvSlist::at(n))->info_;}
#endif
  void                  clear();
  RWBoolean             contains(RWtestGeneric tfun, const void* c) const
        {return find(tfun,c) != rwnil;}
  RWBoolean             containsReference(const void* c) const
        {return findReference(c) != rwnil;}
#ifdef RW_NO_ACCESS_ADJUSTMENT
  size_t                entries() const { return RWIsvSlist::entries(); }
#else
  RWIsvSlist::entries;
#endif
  void*                 find(RWtestGeneric, const void*) const;
  void*                 findReference(const void*) const; // First occurrence
  void*                 first() const;
  void*                 get()   {return peel((RWPSlink*)RWIsvSlist::get());}
  size_t                index(RWtestGeneric, const void*) const;
  void*                 insert(void* a)         { return append(a); }
  void*                 insertAfter(int i, void* a); // For backwards compatibility
  void*                 insertAt(size_t i, void* a);
#ifdef RW_NO_ACCESS_ADJUSTMENT
  RWBoolean             isEmpty() const { return RWIsvSlist::isEmpty(); }
#else
  RWIsvSlist::isEmpty;
#endif
  void*                 last() const;
  size_t                occurrencesOf(RWtestGeneric, const void*) const;
  size_t                occurrencesOfReference(const void*) const;
  void*                 prepend(void* a);
  void*                 remove(RWtestGeneric, const void*);
  void*                 removeReference(const void*);

protected:

  RWPSlink*             headLink()  const;
  RWPSlink*             tailLink()  const;
  RWPSlink*             firstLink() const {return (RWPSlink*)head_.next_;}
  RWPSlink*             lastLink()  const {return (RWPSlink*)last_;}

  // Type safe interfaces to base class:
  void                  insertAfterLink(RWPSlink* p, RWPSlink* v)
        {RWIsvSlist::insertAfterLink(p,v);}
  RWPSlink*             removeRight(RWPSlink* p)
        {return (RWPSlink*)RWIsvSlist::removeRight(p);}

  static void*          peel(RWPSlink*);

private:

friend class RWExport RWSlistIterator;

  RW_TYPESPEC  /* This macro usually expands to nothing */

};


/*
 * Declaration for iterator for singly linked list.
 */
class RWExport RWSlistIterator : RWBehavior2 private RWIsvSlistIterator
{

public:

  // Constructor: cast needed for Glock V2.0b:
  RWSlistIterator(RWSlist& s) : RWIsvSlistIterator((RWIsvSlist&)s) {;}
  virtual ~RWSlistIterator();
  
  void*                 operator++();
  void*                 operator+=(size_t n);
  void*                 operator()();

#ifdef RW_NO_ACCESS_ADJUSTMENT
  RWBoolean             atFirst() const {return RWIsvSlistIterator::atFirst();}
  RWBoolean             atLast() const  {return RWIsvSlistIterator::atLast(); }
  void                  toFirst()       {RWIsvSlistIterator::toFirst();       }
  void                  toLast()        {RWIsvSlistIterator::toLast();        }
#else
  RWIsvSlistIterator::atFirst;
  RWIsvSlistIterator::atLast;
  RWIsvSlistIterator::toFirst;
  RWIsvSlistIterator::toLast;
#endif

  // Methods:
  RWSlist*              container() const       {return (RWSlist*)slist_;}
  void*                 findNext(RWtestGeneric, const void*);
  void*                 findNextReference(const void*);
  void*                 insertAfterPoint(void*);
  void                  insertAfterPoint(RWIsvSlink* p) { RWIsvSlistIterator::insertAfterPoint(p); }
  void*                 key() const
        {return isActive() ? cursor()->info_ : rwnil;}
  void*                 remove()
        {return RWSlist::peel((RWPSlink*)RWIsvSlistIterator::remove());}
  void*                 removeNext(RWtestGeneric, const void*);
  void*                 removeNextReference(const void*);
  void                  reset()                       {RWIsvSlistIterator::reset();}
  void                  reset(RWIsvSlist& l)          {RWIsvSlistIterator::reset(l);}
  void                  reset(RWSlist& sl)            {RWIsvSlistIterator::reset((RWIsvSlist&)sl);}

protected:

  RWPSlink*             cursor() const          {return (RWPSlink*)shere_;}

private:

#ifndef RW_NO_POSTFIX
  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place. (or worse)
  void                  operator++(int);
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
RWSlist::first() const  {
  return isEmpty() ? rwnil : firstLink()->info_;
}

inline void*
RWSlist::last() const   {
  return isEmpty() ? rwnil : lastLink()->info_;
}

inline RWPSlink*
RWSlist::headLink() const {
  // Cast away constness to suppress unfounded cfront warning:
  return (RWPSlink*)&((RWSlist*)this)->head_;
}

inline RWPSlink*
RWSlist::tailLink() const {
  // Cast away constness to suppress unfounded cfront warning:
  return (RWPSlink*)&((RWSlist*)this)->tail_;
}

inline void*
RWSlistIterator::operator++()
{
  // Make sure tail link points to itself:
  RWPRECONDITION(container()->tailLink()->next()==container()->tailLink());
  advance();
  return cursor() == container()->tailLink() ? rwnil : cursor()->info_;
}

inline void*
RWSlistIterator::operator()()
{
  return ++(*this);
}

#endif /* __RWSLIST_H__ */
