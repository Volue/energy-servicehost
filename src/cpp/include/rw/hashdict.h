#ifndef  __RWHASHDICT_H__
#define  __RWHASHDICT_H__

/*
 * RWHashDictionary -- dictionary implemented using a hash table
 *
 * $Id: hashdict.h 18623 2000-03-03 08:28:13Z sih $
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
 */

/*
 * For the storage and retrieval of (key, value) pairs.
 */

#include "rw/rwset.h"

class RWExport RWCollectableAssociation;

/****************************************************************
 *                                                              *
 *                      RWHashDictionary                        *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWHashDictionary : public RWSet
{

  friend class RWExport RWHashDictionaryIterator;
  RWDECLARE_COLLECTABLE(RWHashDictionary)

public:

  RWHashDictionary(size_t N = RWCollection::DEFAULT_CAPACITY);
  RWHashDictionary(const RWHashDictionary&);
  virtual ~RWHashDictionary();

  void                  operator=(const RWHashDictionary&);

#ifndef RW_GLBL_SCP_BUG2
  RWBoolean         operator<=(const RWHashDictionary&) const;
  RWBoolean         operator<=(const RWSet&) const;
  RWBoolean         operator<=(const RWHashTable&) const;
#else
// We must define an operator<=() that won't be overriden by ::operator<=()
  RWBoolean         rwHashDictionary_operator_le(const RWHashDictionary& bt) const;
#endif


  RWBoolean             operator==(const RWHashDictionary&) const;
  RWBoolean             operator==(const RWHashTable&) const;

/***********************************************************************/

  void                          applyToKeyAndValue(RWapplyKeyAndValue, void*);
  virtual void                  clear();
  virtual void                  clearAndDestroy();                      // Deletes all keys AND values.
  virtual RWCollectable*        find(const RWCollectable*) const;               // Returns key
  RWCollectable*                findKeyAndValue(const RWCollectable* key, RWCollectable*& value) const;
  RWCollectable*                findValue(const RWCollectable*) const;  // Returns value
  RWCollectable*                findValue(const RWCollectable*, RWCollectable*) const; // Replace value.
  RWCollectable*                insertKeyAndValue(RWCollectable* key, RWCollectable* value); // Returns key.
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual RWCollectable*        remove(const RWCollectable* key);               // Returns key.
  virtual void                  removeAndDestroy(const RWCollectable*); // Deletes key AND value
  RWCollectable*                removeKeyAndValue(const RWCollectable* key, RWCollectable*& value);
  virtual RWCollection*         select(RWtestCollectable t, void* x) const;
  virtual RWCollection*         select(RWtestCollectablePair t, void* x) const;

protected:

  void                          copyAssociations();
  virtual RWCollectableAssociation*
                                findAssociation(const RWCollectable* key) const;
  virtual RWCollectableAssociation*     
                                newAssociation(RWCollectable*, RWCollectable*) const;
  virtual RWCollectableAssociation*
                                removeAssociation(const RWCollectable* key);

private:

  virtual RWCollectable*        insert(RWCollectable* a){return RWSet::insert(a);}

};    


/****************************************************************
 *                                                              *
 *              RWHashDictionaryIterator                        *
 *                                                              *
 ****************************************************************/

class RWExport RWHashDictionaryIterator : private RWSetIterator {

public:

  RWHashDictionaryIterator(RWHashDictionary& hd) : RWSetIterator(hd) { }
  virtual ~RWHashDictionaryIterator();

/*********** Virtual functions inherited from class RWIterator ***********/
  RWCollectable*        findNext(const RWCollectable*); // key of next match
  RWCollectable*        key() const;                    // current key
  RWCollectable*        operator()();                   // Advance, return key
#if defined(RW_NO_ACCESS_ADJUSTMENT) || defined(RW_SPECIFIC_BASEBASE_ACCESS)
  void                  reset() { RWHashTableIterator::reset(); }
#else
  RWSetIterator::reset;
#endif

/******************* Special iterator functions *****************************/

  RWCollectable*        remove();                       // Rem. current, ret key
  RWCollectable*        removeNext(const RWCollectable*);// Rem. next match etc.
  RWCollectable*        value() const;                  // Return current value
  RWCollectable*        value(RWCollectable*) const;    // Replace, ret new val.
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

//Special fix for Borland 5.0, this is related to bug #2437.  Borland 5.0
//will not choose class scope over global scope ever, but will recognize
//this as a function template specialization.

#if !defined(RW_NO_STL) && defined(RW_GLOBAL_BEFORE_CLASS_SCOPE)
inline bool operator<=
(const RWHashDictionary& wasThis, const RWHashDictionary& btr)
{
#  ifndef RW_GLBL_SCP_BUG2
    return wasThis.operator<=(btr);
#  else
    return wasThis.rwHashDictionary_operator_le(btr);
#  endif  /* RW_GLBL_SCP_BUG2 */
}
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWHASHDICT_H__ */
