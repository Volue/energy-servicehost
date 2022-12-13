#ifndef __RWIDLIST_H__
#define __RWIDLIST_H__

/*
 * RWIsvDlist: An intrusive doubly-linked list.
 *
 * $Id: idlist.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:42  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.5  1996/07/18 04:31:19  hart
 * ObjectStore changes
 *
 * Revision 7.4  1996/05/07 23:25:18  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:45:07  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:08:00  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:51:29  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:10:20  vriezen
 * Move all files to 6.1
 *
 * Revision 2.9  1994/03/22  23:26:13  vriezen
 * Added operator--(int) to private.
 *
 * Revision 2.8  1993/10/04  18:16:19  keffer
 * RWIsv[SD]link now does not inherit from RWMemoryPool in
 * multi-threaded case.
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
 * Revision 2.4  1993/02/11  22:47:20  keffer
 * Ported to IBM xlC compiler
 *
 * Revision 2.3  1993/01/23  00:34:10  keffer
 * Performance enhancements; simplified; flatter inheritance tree.
 *
 *    Rev 1.3   07 Jun 1992 16:06:56   KEFFER
 * Introduced HAS_POSTFIX macro
 * 
 *    Rev 1.2   28 May 1992 11:03:58   KEFFER
 * Added RWExport tag where appropriate
 * 
 *    Rev 1.1   25 May 1992 15:53:46   KEFFER
 * Optimized by using an internal link
 * 
 *    Rev 1.0   11 Mar 1992 14:10:44   KEFFER
 * Initial revision.
 */


#ifndef __RWTOOLDEFS_H__
# include "rw/tooldefs.h"
#endif
#ifndef __RWMEMPOOL_H__
# include "rw/mempool.h"
#endif


/****************************************************************
 *                                                              *
 *              Declaration for RWIsvDlink                      *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWIsvDlink RWMemoryPool_OPTION
{

public:

#ifdef RWDEBUG
  RWIsvDlink() : next_(rwnil) {;}
#else
  RWIsvDlink() {;}
#endif

  RWIsvDlink*           next() const {return next_;}
  RWIsvDlink*           prev() const {return prev_;}
  RWIsvDlink*           clear()
        {RWIsvDlink* ret = next_; next_=rwnil; return ret;}

  RWIsvDlink*           next_;          // Pointer to next link
  RWIsvDlink*           prev_;          // Pointer to previous link

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *                                                              *
 *              Declarations for RWIsvDlist                     *
 *                                                              *
 ****************************************************************/

class RWExport RWIsvDlist
{

public:

  RWIsvDlist()          {init();}
  RWIsvDlist(RWIsvDlink* a);
  ~RWIsvDlist()         {clear();}

  /********************* Member functions **************************/
  void                  append(RWIsvDlink* a){insertAfterLink(tail_.prev_, a);}
  RWIsvDlink*           at(size_t i) const;     // Index must be in range
  void                  clear();
  RWBoolean             containsReference(const RWIsvDlink*) const;
  size_t                entries() const         {return nitems_;}
  RWIsvDlink*           first() const   {return isEmpty() ? rwnil: head_.next_;}
  RWIsvDlink*           get()                   {return removeFirst();}
  void                  insert(RWIsvDlink* a)   {append(a);}
  void                  insertAt(size_t, RWIsvDlink*);
  RWBoolean             isEmpty() const         {return nitems_ == 0;}
  RWIsvDlink*           last() const    {return isEmpty() ? rwnil : tail_.prev_;}
  size_t                occurrencesOfReference(const RWIsvDlink* a) const
    { return containsReference(a) ? 1u : 0;}
  void                  prepend(RWIsvDlink* a)  {insertAfterLink(&head_, a);}
  RWIsvDlink*           removeAt(size_t);               // Relatively slow
  RWIsvDlink*           removeFirst()
        {return isEmpty() ? rwnil : removeReference(head_.next_);}
  RWIsvDlink*           removeLast()
        {return isEmpty() ? rwnil : removeReference(tail_.prev_);}
  RWIsvDlink*           removeReference(RWIsvDlink* a);//rem,ret link at addr a

protected:

  void                  init();
  void                  insertAfterLink(RWIsvDlink*, RWIsvDlink*);

  RWIsvDlink            head_;          // Sentinel
  RWIsvDlink            tail_;          // Sentinel
  size_t                nitems_;        // Number of entries in the list

private:
  friend class RWExport RWIsvDlistIterator;
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};


/****************************************************************
 *                                                              *
 *              Declarations for RWIsvDlistIterator             *
 *                                                              *
 ****************************************************************/

// Iterator for doubly linked list:

class RWExport RWIsvDlistIterator
{

public:

  RWIsvDlistIterator(RWIsvDlist& s) : dlist_(&s), dhere_(&s.head_) {;}

  // Operators:
  RWIsvDlink*           operator++();           // Advance and return.
  RWIsvDlink*           operator--();           // Backup and return.
  RWIsvDlink*           operator+=(size_t n);   // Advance n links and return.
  RWIsvDlink*           operator-=(size_t n);   // Backup n links and return.
  RWIsvDlink*           operator()();           // Advance and return.

  RWBoolean             atFirst() const;
  RWBoolean             atLast() const;
  void                  toFirst();
  void                  toLast();

  RWIsvDlist*           container() const       {return dlist_;}
  void                  insertAfterPoint(RWIsvDlink*);// Insert after position.
  RWIsvDlink*           key() const;            // Return current item
  RWIsvDlink*           remove();               // Remove current item
  void                  reset();
  void                  reset(RWIsvDlist&);     //set container to be interated

protected:

  void                  advance()       {dhere_ = dhere_->next_;}
  void                  backup()        {dhere_ = dhere_->prev_;}
  RWBoolean             isActive() const;

  RWIsvDlist*           dlist_;                 // The list we are iterating
  RWIsvDlink*           dhere_;                 // Iterator position

private:

#ifndef RW_NO_POSTFIX
  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
  RWBoolean             operator--(int);
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

/*
 * Prefix operator: advance iterator one link then return new link.
 * Returns nil if we were at the last link or if the list is empty.
 */
inline RWIsvDlink*
RWIsvDlistIterator::operator++()
{
  // Make sure that tail_ points to itself:
  RWPRECONDITION(dlist_->tail_.next_ == &dlist_->tail_);

  advance();
  return dhere_ == &dlist_->tail_ ? rwnil : dhere_;
}

/*
 * Prefix operator: backup iterator one link then return new link.
 * Returns nil if we were at the first link or if the list is empty.
 */
inline RWIsvDlink*
RWIsvDlistIterator::operator--()
{
  // Make sure that head_ points to itself:
  RWPRECONDITION(dlist_->head_.prev_ == &dlist_->head_);

  backup();
  return dhere_ == &dlist_->head_ ? rwnil : dhere_;
}

inline RWIsvDlink*
RWIsvDlistIterator::operator()()
{
  return ++(*this);
}

inline RWBoolean
RWIsvDlistIterator::isActive() const
{
  return dhere_ != &dlist_->head_ && dhere_ !=&dlist_->tail_;
}

/*
 * Return the current item.  This will be nil if the iterator
 * is out of range: Either past the end, or before the beginning
 * of the list.
 */
inline RWIsvDlink*
RWIsvDlistIterator::key() const
{
  return isActive() ? dhere_ : rwnil;
}

/*
 * Reset iterator: position it to the phantom link at the head of the list
 */
inline void
RWIsvDlistIterator::reset()
{
  dhere_ = &dlist_->head_;
}

#endif  /* __RWIDLIST_H__ */
