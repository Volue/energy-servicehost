#ifndef  __RWDLISTCOL_H__
#define  __RWDLISTCOL_H__

/*
 * Declarations for Doubly-linked list of RWCollectables.
 *
 * $Id: dlistcol.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.5  1996/07/18 04:30:11  hart
 * ObjectStore changes
 *
 * Revision 7.4  1996/05/08 17:00:05  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:40:02  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 18:31:10  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:48:49  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/06/18  02:22:57  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.1  1994/04/15  19:09:53  vriezen
 * Move all files to 6.1
 *
 * Revision 2.7  1994/03/04  01:04:16  jims
 * Override isEqual member function from RWCollectable to return
 * TRUE or FALSE based on operator==
 *
 * Revision 2.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.5  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.4  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.3  1993/01/25  18:12:13  keffer
 * RW_NO_CONST_OVERLOADS->RW_NO_CONST_OVERLOAD
 *
 * Revision 2.1  1992/11/19  05:45:01  keffer
 * Introduced new <rw/compiler.h> macro directives
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.4   04 Aug 1992 18:59:40   KEFFER
 * Added keyword "virtual" to destructor for documentation purposes.
 * 
 *    Rev 1.3   22 May 1992 17:04:12   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.2   18 Feb 1992 09:54:18   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:12   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:14:36   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/dlist.h"
#include "rw/seqcltn.h"
#include "rw/iterator.h"

/****************************************************************
 *                                                              *
 *                      RWDlistCollectables                     *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWDlistCollectables : public RWSequenceable, public RWDlist {

  friend class RWExport RWDlistCollectablesIterator;
  RWDECLARE_COLLECTABLE(RWDlistCollectables)

public:  

  RWDlistCollectables();
  RWDlistCollectables(RWCollectable* a) : RWDlist(a) { }
  virtual ~RWDlistCollectables();

  RWBoolean             operator==(const RWDlistCollectables&) const;

  /***************  Virtual function inherited from RWSequenceable *****************/  
  virtual RWCollectable*        append(RWCollectable* a)        // Add at tail of list.
    {return (RWCollectable*)RWDlist::append(a);}
  virtual void          apply(RWapplyCollectable ap, void* x)
    { RWDlist::apply((RWapplyGeneric)ap, x); }
  virtual RWCollectable*&       at(size_t i)
    { return (RWCollectable*&)RWDlist::at(i); }
#ifndef RW_NO_CONST_OVERLOAD
  virtual const RWCollectable*  at(size_t i) const
    { return (const RWCollectable*)RWDlist::at(i); }
#endif
  virtual void                  clear()
    { RWDlist::clear(); }
//virtual void                  clearAndDestroy();
  virtual RWBoolean             contains(const RWCollectable* a) const
    {return RWCollection::contains(a);}
  virtual size_t                entries() const
    {return RWDlist::entries();}
  virtual RWCollectable*        find(const RWCollectable* a) const
    { return (RWCollectable*)RWDlist::find(rwIsEqualFun, a); }
  virtual RWCollectable*        first() const   // Return value at head of list.
    {return (RWCollectable*) RWDlist::first();}
  virtual size_t                index(const RWCollectable* a) const
    { return RWDlist::index(rwIsEqualFun, a); }
  virtual RWCollectable*        insert(RWCollectable* a)
    { return (RWCollectable*)RWDlist::insert(a); }
  virtual RWCollectable*        insertAt(size_t i, RWCollectable* a)
    { return (RWCollectable*)RWDlist::insertAt(i,a); }
  virtual RWBoolean             isEmpty() const
    {return RWDlist::isEmpty();}
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual RWCollectable*        last() const    // Return value at tail of list.
    {return (RWCollectable*) RWDlist::last();}
  virtual size_t                occurrencesOf(const RWCollectable* a) const
    { return RWDlist::occurrencesOf(rwIsEqualFun, a); }
  virtual RWCollectable*        prepend(RWCollectable* a)
    { return (RWCollectable*)RWDlist::prepend(a);}
  virtual RWCollectable*        remove(const RWCollectable* a)
    { return (RWCollectable*)RWDlist::remove(rwIsEqualFun, a); }
//void                          removeAndDestroy(const RWCollectable* a);

/****************  (Redefined) Special Functions *****************/
  RWBoolean                     containsReference(const RWCollectable* a) const
    { return RWDlist::containsReference(a); }
  RWCollectable*                findReference(const RWCollectable* a) const
    { return (RWCollectable*)RWDlist::findReference(a); }
  RWCollectable*                get()           // Return value and delete link at head of list.
    {return (RWCollectable*) RWDlist::get();}
  size_t                        occurrencesOfReference(const RWCollectable* a) const
    { return RWDlist::occurrencesOfReference(a); }
  RWCollectable*                removeReference(const RWCollectable* a)
    { return (RWCollectable*)RWDlist::removeReference(a); }
/*****************************************************************/

  // For backwards compatiblity:
  virtual RWCollectable*        insertAfter(int i, RWCollectable* a)
    { return (RWCollectable*)RWDlist::insertAfter(i,a); }
};

/****************************************************************
 *                                                              *
 *                      RWDlistCollectablesIterator             *
 *                                                              *
 ****************************************************************/

/*
 * Iterator for doubly-linked list (moves iterator through the list);
 * starts by pointing at last link.
 */

class RWExport RWDlistCollectablesIterator : public RWIterator, public RWDlistIterator {
public:
  RWDlistCollectablesIterator(RWDlistCollectables& s);
  virtual ~RWDlistCollectablesIterator();
  
/****************** (Redefined) Standard Iterator functions *********************/
  virtual RWCollectable*        findNext(const RWCollectable* a)
    { return (RWCollectable*)RWDlistIterator::findNext(rwIsEqualFun, a); }
  virtual RWCollectable*        key() const                             // Return current item
    { return (RWCollectable*)RWDlistIterator::key(); }
  virtual RWCollectable*        operator()()                            // Advance iterator
    { return (RWCollectable*)RWDlistIterator::operator()(); }
  virtual void                  reset()
    { RWDlistIterator::reset(); }
  
/***************** (Redefined) Special RWDlistIterator functions ********************/

  RWCollectable*                findNextReference(const RWCollectable* a)
    { return (RWCollectable*)RWDlistIterator::findNextReference(a); }
  RWCollectable*                insertAfterPoint(RWCollectable* a)      // Insert item behind iterator.
    { return (RWCollectable*)RWDlistIterator::insertAfterPoint(a); }
  RWCollectable*                remove()                                // Remove current item
    { return (RWCollectable*)RWDlistIterator::remove(); }
  RWCollectable*                removeNext(const RWCollectable* a)
    { return (RWCollectable*)RWDlistIterator::removeNext(rwIsEqualFun, a); }
  RWCollectable*                removeNextReference(const RWCollectable* a)
    { return (RWCollectable*)RWDlistIterator::removeNextReference(a); }
/*****************************************************************/

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWDLISTCOL_H__ */
