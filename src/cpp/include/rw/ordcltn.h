#ifndef __RWORDCLTN_H__
#define __RWORDCLTN_H__

/*
 * RWOrdered --- Ordered Collection
 *
 * $Id: ordcltn.h 18623 2000-03-03 08:28:13Z sih $
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
 * The copy constructor and assignment operator use memberwise
 * initialization and assignment, respectively.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:42  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.5  1996/07/18 04:18:20  hart
 * ObjectStore changes
 *
 * Revision 7.4  1996/05/07 22:50:07  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:45:26  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:15:08  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:53:46  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.4  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.3  1994/06/18  02:22:57  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.2  1994/06/18  01:48:36  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.1  1994/04/15  19:10:33  vriezen
 * Move all files to 6.1
 *
 * Revision 2.10  1994/03/04  01:04:16  jims
 * Override isEqual member function from RWCollectable to return
 * TRUE or FALSE based on operator==
 *
 * Revision 2.9  1994/01/13  04:35:39  jims
 * Move member function removeAt to the public interface
 *
 * Revision 2.8  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.7  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.6  1993/04/01  00:50:34  myersn
 * change GVector to RWGVector.
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
 * Revision 2.1  1992/11/15  00:03:02  keffer
 * Added explicit "this->" dereference to suppress unfounded
 * cfront 3.0 warnings.
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.8   22 May 1992 17:04:14   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.7   17 Mar 1992 19:21:10   KEFFER
 * Changed BOUNDS_CHECK to RWBOUNDS_CHECK
 * 
 *    Rev 1.6   04 Mar 1992 09:03:52   KEFFER
 * nil changed to rwnil
 * 
 *    Rev 1.5   18 Feb 1992 09:54:32   KEFFER
 * 
 *    Rev 1.4   05 Nov 1991 13:52:06   keffer
 * Now declares GVector of RWCollectableP, instead of relying on collect.h
 * 
 *    Rev 1.3   28 Oct 1991 09:08:18   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.2   20 Aug 1991 18:53:12   keffer
 * pop() now returns nitems-1
 * 
 *    Rev 1.1   29 Jul 1991 14:13:56   keffer
 * Added member function data().
 * 
 *    Rev 1.0   28 Jul 1991 08:15:54   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 */

#include "rw/seqcltn.h"
#include "rw/iterator.h"
#include "rw/gvector.h"
declare(RWGVector,RWCollectableP);


/****************************************************************
 *                                                              *
 *                      RWOrdered                               *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWOrdered : public RWSequenceable {

  friend class RWExport RWOrderedIterator;
  RWDECLARE_COLLECTABLE(RWOrdered)

public:

  RWOrdered(size_t size = RWCollection::DEFAULT_CAPACITY);
  virtual ~RWOrdered();

  /******************** Member operators ****************************/
  RWBoolean                     operator==(const RWOrdered&) const;

  /****************** Virtual member functions *******************/
  virtual RWCollectable*        append(RWCollectable* a);
  virtual void                  apply(RWapplyCollectable, void*);
  virtual RWCollectable*&       at(size_t);     // Can use as lvalue
#ifndef RW_NO_CONST_OVERLOAD
  virtual const RWCollectable*  at(size_t) const;    //Cannot use as lvalue.
#endif
  virtual void                  clear();
//virtual void                  clearAndDestroy();
//virtual RWBoolean             contains(const RWCollectable*) const;
  virtual size_t                entries() const {return nitems;}
  virtual RWCollectable*        find(const RWCollectable*) const;//"left"most
  virtual RWCollectable*        first() const;
  virtual size_t                index(const RWCollectable*) const;//RW_NPOS?
  virtual RWCollectable*        insert(RWCollectable*);          // Appends.
  virtual RWCollectable*        insertAt(size_t, RWCollectable*);
  virtual RWBoolean             isEmpty() const {return nitems==0;}
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual RWCollectable*        last() const;
  virtual size_t                occurrencesOf(const RWCollectable*) const;
  virtual RWCollectable*        remove(const RWCollectable*);   //"left"most
//virtual void                  removeAndDestroy(const RWCollectable*);
  virtual RWCollectable*        prepend(RWCollectable*);

  /*********************** Special functions ******************************/
  RWCollectable*&               operator[](size_t);     //With bounds checking
  RWCollectable*&               operator()(size_t);     //Optional boundscheck
#ifndef RW_NO_CONST_OVERLOAD
  RWCollectable*                operator[](size_t) const;//With bounds checking
  RWCollectable*                operator()(size_t) const;//Optional boundscheck
#endif
  const RWCollectableP*         data() const;           // Use with care.
  void                          push(RWCollectable*);   // Alternative stack.
  RWCollectable*                pop();
  RWCollectable*                removeAt(size_t);
  void                          resize(size_t); // dont shrink below population
  RWCollectable*                top() const;

  // For backwards compatiblity:
  virtual RWCollectable*        insertAfter(int, RWCollectable*);

protected:

  size_t                        nitems;
  RWGVector(RWCollectableP)     vec;    // An array of pointers to objects.

  void                          boundsCheck(size_t) const;

};

/****************************************************************
 *                                                              *
 *                      RWOrderedIterator                       *
 *                                                              *
 ****************************************************************/

class RWExport RWOrderedIterator : public RWIterator {
public:
  RWOrderedIterator(const RWOrdered& ord) { theCollection=&ord; here=RW_NPOS;}
  virtual ~RWOrderedIterator();
  virtual RWCollectable*        findNext(const RWCollectable*); //next match
  virtual RWCollectable*        key() const;              //Return current key
  virtual RWCollectable*        operator()();             // Advance iterator
  virtual void                  reset() {here=RW_NPOS;}
private:
  const RWOrdered*              theCollection;
  size_t                        here;
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                      INLINES                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

inline RWCollectable*&
RWOrdered::operator[](size_t i)
{ boundsCheck(i); return vec(i); }

inline RWCollectable*&
RWOrdered::operator()(size_t i)
{
#ifdef RWBOUNDS_CHECK
  boundsCheck(i);
#endif
  return vec(i);
}

#ifndef RW_NO_CONST_OVERLOAD
inline RWCollectable*
RWOrdered::operator[](size_t i) const
{ boundsCheck(i); return this->vec(i); }

inline RWCollectable*
RWOrdered::operator()(size_t i) const
{
#ifdef RWBOUNDS_CHECK
  boundsCheck(i);
#endif
  return this->vec(i);
}
#endif  /* RW_NO_CONST_OVERLOAD */

inline const RWCollectableP*
RWOrdered::data() const
{
  return vec.data();
}

inline void
RWOrdered::push(RWCollectable* c)
{ insert(c); }

inline RWCollectable*
RWOrdered::pop()
{ return nitems>0 ? removeAt(nitems-1) : rwnil; }

inline RWCollectable*
RWOrdered::top() const
{ return nitems>0 ? this->vec(nitems-1) : rwnil; }

#endif /* __RWORDCLTN_H__ */
