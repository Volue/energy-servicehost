#ifndef __RWSORTVEC_H__
#define __RWSORTVEC_H__

/*
 * RWSortedVector -- sorted vector; uses insertion sort.
 *
 * $Id: sortvec.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.5  1996/05/07 21:44:13  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:46:39  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 19:43:15  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/07/06  17:01:55  griswolf
 * scopus 1896: Add isSorted() method (RWDEBUG only).
 *
 * Revision 7.1  1994/10/16  02:57:41  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/06/18  01:22:18  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.1  1994/04/15  19:11:00  vriezen
 * Move all files to 6.1
 *
 * Revision 2.8  1994/03/04  01:04:16  jims
 * Override isEqual member function from RWCollectable to return
 * TRUE or FALSE based on operator==
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
 * Revision 2.4  1993/01/25  18:12:13  keffer
 * RW_NO_CONST_OVERLOADS->RW_NO_CONST_OVERLOAD
 *
 * Revision 2.2  1992/11/19  05:45:01  keffer
 * Introduced new <rw/compiler.h> macro directives
 *
 * Revision 2.1  1992/11/15  00:32:38  keffer
 * Added explicit "this->" dereference to suppress unfounded
 * cfront 3.0 warnings.
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.4   22 May 1992 17:04:16   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.3   17 Mar 1992 19:21:10   KEFFER
 * Changed BOUNDS_CHECK to RWBOUNDS_CHECK
 * 
 *    Rev 1.2   18 Feb 1992 09:54:46   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:24   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:17:18   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

/*
 * This collection inherits from RWOrdered.  A few member functions
 * must be disallowed because the insertion order is determined internally
 * rather than by function calls.  Examples are "insertAfter()", or 
 * "prepend()".  A few others can be done more efficiently because we can
 * take advantage of the internal sorting of objects.  Examples are "index()",
 * "occurrencesOf()", etc.
 */

#include "rw/ordcltn.h"

// OrderedCollection iterator is used for SortedVector:
typedef RWOrderedIterator RWSortedVectorIterator;

/****************************************************************
 *                                                              *
 *                      RWSortedVector                          *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWSortedVector : public RWOrdered {

  RWDECLARE_COLLECTABLE(RWSortedVector)

public:

  RWSortedVector(size_t size = RWCollection::DEFAULT_CAPACITY);
  virtual ~RWSortedVector();

  RWBoolean                     operator==(const RWSortedVector& c) const {return RWOrdered::operator==(c);}

  /****************** Redefined from RWOrdered *******************/

#ifndef RW_NO_CONST_OVERLOAD
  virtual const RWCollectable*  at(size_t) const;       // Cannot use as lvalue.
#endif
  virtual size_t                index(const RWCollectable* p) const;
  virtual RWCollectable*        insert(RWCollectable*);
  virtual size_t                occurrencesOf(const RWCollectable* a) const;
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  const RWCollectable*          operator[](size_t i) const              // With bounds checking
    { boundsCheck(i); return this->vec(i); }
  const RWCollectable*          operator()(size_t i) const              // Optional bounds checking
    {
#ifdef RWBOUNDS_CHECK
      boundsCheck(i);
#endif
      return this->vec(i);
    }

  /****************** Inherited from RWOrdered *******************/
//virtual void                  apply(RWapplyCollectable, void*);
//virtual void                  clear();
//virtual void                  clearAndDestroy();
//virtual RWBoolean             contains(const RWCollectable*) const;
//virtual size_t                entries() const {return nitems;}
//virtual RWCollectable*        find(const RWCollectable* p) const;
//virtual RWCollectable*        first() const;
//virtual RWBoolean             isEmpty() const {return nitems==0;}
//virtual RWCollectable*        last() const;
//virtual RWCollectable*        remove(const RWCollectable*);
//virtual void                  removeAndDestroy(const RWCollectable*);

//void                          resize(size_t); // Cannot shrink below population

  /****************** Disallowed from RWOrdered *******************/
private:
  virtual RWCollectable*&       at(size_t);     // No lvalues allowed for sorted vector
  virtual RWCollectable*        append(RWCollectable* a);
  virtual RWCollectable*        insertAt(size_t, RWCollectable*);
  virtual RWCollectable*        prepend(RWCollectable*);
  void                          push(RWCollectable*);
  RWCollectable*                pop();
  RWCollectable*                top() const;

  // For backwards compatiblity:
  virtual RWCollectable*        insertAfter(int, RWCollectable*);
#if defined(RWDEBUG)
  RWBoolean                     isSorted() const;
#endif
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWSORTVEC_H__ */
