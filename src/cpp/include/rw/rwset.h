#ifndef  __RWSET_H__
#define  __RWSET_H__

/*
 * Declarations for RWSet --- hash table lookup.
 *
 * $Id: rwset.h 18623 2000-03-03 08:28:13Z sih $
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
 * Duplicates are not allowed.
 * Hash table look up with chaining: derived from RWHashTable
 *
 */

#include "rw/hashtab.h"
#include "rw/iterator.h"

class RWExport RWSetIterator;

/****************************************************************
 *                                                              *
 *                      RWSet                                   *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWSet : public RWHashTable {

  friend class RWExport RWSetIterator;
  RWDECLARE_COLLECTABLE(RWSet)

public:

  RWSet(size_t N = RWCollection::DEFAULT_CAPACITY);
  RWSet (const RWSet& s) :  RWHashTable(s) { }
  virtual ~RWSet ();

  /******************** Member operators ****************************/
  void                          operator=(const RWSet& s)
        { RWHashTable::operator=(s);}
  // Define subset and proper subset, respectively.
#ifndef RW_GLBL_SCP_BUG2
  RWBoolean         operator<=(const RWSet&) const;
  RWBoolean         operator<=(const RWHashTable&) const;
#else
// We must define an operator<=() that won't be overriden by ::operator<=()
  RWBoolean         rwSet_operator_le(const RWSet& bt) const;
#endif


  RWBoolean                     operator< (const RWSet&) const;
  RWSet&                        operator*=(const RWSet&); // Intersection
  void                          intersectWith(const RWSet&, RWSet& dest) const;

//RWBoolean                     operator==(const RWSet&) const;
//RWBoolean                     operator!=(const RWSet&) const;

  /****************** Virtual member functions *******************/
//virtual void                  apply(RWapplyCollectable, void*);
//virtual RWspace               binaryStoreSize() const;
//virtual void                  clear();
  virtual void                  clearAndDestroy();
//virtual int                   compareTo(const RWCollectable*) const;
//virtual RWBoolean             contains(const RWCollectable*) const;
//virtual size_t                entries() const;
//virtual RWCollectable*        find(const RWCollectable*) const;
//virtual unsigned              hash() const;
  virtual RWCollectable*        insert(RWCollectable*);
//virtual RWBoolean             isEmpty() const         {return items==0;}
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual size_t                occurrencesOf(const RWCollectable* a) const;
//virtual RWCollectable*        remove(const RWCollectable*);
//virtual void                  removeAndDestroy(const RWCollectable*); 
//virtual void                  restoreGuts(RWvistream&);
//virtual void                  restoreGuts(RWFile&);
//virtual void                  saveGuts(RWvostream&) const;
//virtual void                  saveGuts(RWFile&) const;

/********************** Special functions **********************************/
#ifdef RDEBUG
  // Print status of hash table:
  friend ostream&               operator<<(ostream&, const RWSet&);
#endif
};

/****************************************************************
 *                                                              *
 *                      RWSetIterator                           *
 *                                                              *
 ****************************************************************/

/*
 * RWSetIterator is an exact clone of RWHashTableIterator --
 */

class RWExport RWSetIterator : public RWHashTableIterator {
  RW_DECLARE_HOME(RWSetIterator)
public:

  RWSetIterator(RWSet& h) : RWHashTableIterator(h) { }
#ifdef RW_NO_ACCESS_ADJUSTMENT
  void                          reset() { RWHashTableIterator::reset(); }
#else
  RWHashTableIterator::reset;
#endif

  RW_TYPESPEC  /* This macro usually expands to nothing */

};

//Special fix for Borland 5.0, this is related to bug #2437.  Borland 5.0
//will not choose class scope over global scope ever, but will recognize
//this as a function template specialization.

#if !defined(RW_NO_STL) && defined(RW_GLOBAL_BEFORE_CLASS_SCOPE)
inline bool operator<=
(const RWSet& wasThis, const RWSet& btr)
{
#  ifndef RW_GLBL_SCP_BUG2
    return wasThis.operator<=(btr);
#  else
    return wasThis.rwSet_operator_le(btr);
#  endif  /* RW_GLBL_SCP_BUG2 */
}
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWSET_H__ */
