#ifndef  __RWHASHTAB_H__
#define  __RWHASHTAB_H__

/*
 * Declarations for RWHashTable --- hash table lookup.
 *
 * $Id: hashtab.h 18623 2000-03-03 08:28:13Z sih $
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
 * Hash table look up with chaining
 * Duplicates are kept as distinct entries.
 * 
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:42  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.7  1996/07/18 04:30:55  hart
 * ObjectStore changes
 *
 * Revision 7.6  1996/05/07 23:24:50  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.5  1996/03/07 19:54:15  kevinj
 * Bug #2684: Added RW_GLOBAL_BEFORE_CLASS_SCOPE to make inline
 * specialized template operator<= for times when <utility>'s
 * templatized operator<= is used instead of
 * the class member operator<=.
 *
 * Revision 7.4  1996/02/18 01:45:00  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1996/01/29 20:28:29  pearson
 * Specialize global template function operator<= for Borland 5.0
 *
 * Revision 7.2  1995/09/05 19:03:26  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:51:16  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/06/15  03:05:28  myersn
 * make RWHashTable::buckets() public, for use with resize().
 *
 * Revision 6.1  1994/04/15  19:10:15  vriezen
 * Move all files to 6.1
 *
 * Revision 2.11  1994/03/22  23:04:52  vriezen
 * Cast entire expression to size_t in hashIndex, not just lhs.
 * Fixes problem on SunOS where size_t is signed, and hashIndex
 * returned negative number.
 *
 * Revision 2.10  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.9  1993/07/19  20:45:26  keffer
 * friend classes now use elaborated-type-specifier (ARM Sec. 11.4)
 *
 * Revision 2.8  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.7  1993/04/01  00:32:13  myersn
 * add RW prefix to GVector mention.
 *
 * Revision 2.6  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.5  1993/03/17  21:05:21  keffer
 * Return type of binaryStoreSize() is now RWspace
 *
 * Revision 2.4  1993/02/07  21:31:32  keffer
 * Provided copy constructor for iterator
 *
 * Revision 2.3  1993/01/29  21:52:56  keffer
 * *** empty log message ***
 *
 * Revision 2.2  1993/01/28  22:40:22  keffer
 * Optimized insertions slightly.
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.5   04 Aug 1992 18:59:52   KEFFER
 * Added keyword "virtual" to destructor for documentation purposes.
 * 
 *    Rev 1.4   25 May 1992 15:50:36   KEFFER
 * Optimized, reducing size.
 * 
 *    Rev 1.3   22 May 1992 17:02:32   KEFFER
 * Simplified algorithms
 * 
 *    Rev 1.2   22 May 1992 10:41:04   KEFFER
 * Uses RWDECLARE_COLLECTABLE.
 * 
 *    Rev 1.1   29 Apr 1992 15:51:26   KEFFER
 * Removed default argument from copyOld()
 * 
 *    Rev 1.0   29 Apr 1992 14:51:18   KEFFER
 * Hashing now uses chaining to resolve collisions
 */

#include "rw/colclass.h"
#include "rw/iterator.h"
#include "rw/gvector.h"

class RWExport RWSlistCollectables;
class RWExport RWSlistCollectablesIterator;
class RWExport RWHashTableIterator;
// Declare a vector of pointers to RWSlistCollectables:
typedef RWSlistCollectables* RWSlistCollectablesP;
declare(RWGVector,RWSlistCollectablesP);

/****************************************************************
 *                                                              *
 *                      RWHashTable                             *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWHashTable : public RWCollection {

  friend class RWExport RWHashTableIterator;
  RWDECLARE_COLLECTABLE(RWHashTable)

public:

  RWHashTable(size_t N = RWCollection::DEFAULT_CAPACITY);
  RWHashTable (const RWHashTable&);
  virtual ~RWHashTable();

  /******************** Member operators ****************************/
  RWHashTable&                  operator=(const RWHashTable&);
  RWBoolean                     operator<=(const RWHashTable&) const;
  RWBoolean                     operator==(const RWHashTable&) const;
  RWBoolean                     operator!=(const RWHashTable&) const;

  /****************** Virtual member functions *******************/
  virtual void                  apply(RWapplyCollectable, void*);
//virtual RWspace               binaryStoreSize() const;
  virtual void                  clear();
//virtual void                  clearAndDestroy();
//virtual int                   compareTo(const RWCollectable*) const;
//virtual RWBoolean             contains(const RWCollectable*) const;
  virtual size_t                entries() const         {return nitems_;}
  virtual RWCollectable*        find(const RWCollectable*) const;
//virtual unsigned              hash() const;
  virtual RWCollectable*        insert(RWCollectable*);
  virtual RWBoolean             isEmpty() const         {return nitems_==0;}
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual size_t                occurrencesOf(const RWCollectable*) const;
  virtual RWCollectable*        remove(const RWCollectable*);
//virtual void                  removeAndDestroy(const RWCollectable*); 
//virtual void                  restoreGuts(RWvistream&);
//virtual void                  restoreGuts(RWFile&);
//virtual void                  saveGuts(RWvostream&) const;
//virtual void                  saveGuts(RWFile&) const;

/********************** Special functions **********************************/
  virtual void                  resize(size_t n = 0);
  size_t                        buckets() const {return table_.length();}

protected:

  RWGVector(RWSlistCollectablesP) table_;  // Table of pointers to chains.
  size_t                        nitems_;   // Total number of stored objects.

protected:

  size_t                        hashIndex(const RWCollectable* p) const
     { return (size_t)(p->hash() % buckets()); }
  RWCollectable*                insertIndex(size_t, RWCollectable*);
};



/****************************************************************
 *                                                              *
 *              RWHashTableIterator                             *
 *                                                              *
 ****************************************************************/

class RWExport RWHashTableIterator : public RWIterator {

public:

  RWHashTableIterator(RWHashTable& h);
  RWHashTableIterator(const RWHashTableIterator&);
  virtual ~RWHashTableIterator();

  RWHashTableIterator&  operator=(const RWHashTableIterator&);

/*********** Virtual functions inherited from class RWIterator ***********/
  virtual RWCollectable*        findNext(const RWCollectable*); // next match
  virtual RWCollectable*        key() const;                    //Ret current
  virtual RWCollectable*        operator()();                   //Advance iter
  virtual void                  reset();
/******************* Special iterator functions *******************************/
  RWCollectable*                remove();               // Remove current item
  RWCollectable*                removeNext(const RWCollectable*);//Rem nxt match

protected:

  void                          nextIterator();

private:

  RWHashTable*                  myHash_;
  size_t                        idx_;           // which Slist is current
  RWSlistCollectablesIterator*  iterator_;      // an iterator for that Slist
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

//Special fix for Borland 5.0, this is related to bug #2437.  Borland 5.0
//will not choose class scope over global scope ever, but will recognize
//this as a function template specialization.

#if !defined(RW_NO_STL) && defined(RW_GLOBAL_BEFORE_CLASS_SCOPE)
inline bool operator<=
(const RWHashTable& wasThis, const RWHashTable& btr)
{
     return wasThis.operator<=(btr);
}
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWHASHTAB_H__ */
