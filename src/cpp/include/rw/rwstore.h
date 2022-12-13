#ifndef __RWSTORETABLE_H__
#define __RWSTORETABLE_H__

/*
 * Definitions for internal classes used to store collectables
 *
 * $Id: rwstore.h 18623 2000-03-03 08:28:13Z sih $
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


/**
 *******************  RWStoreTable declarations ***************************
 **/

#include "rw/rwassert.h"
#include "rw/rwset.h"
#include "rw/stringid.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

#ifdef RW_TRAILING_RWEXPORT
extern RWCollectableP rwexport getRWNilCollectable();
#else
extern rwexport RWCollectableP getRWNilCollectable();
#endif //RW_TRAILING_RWEXPORT

# define RWnilCollectable (getRWNilCollectable())

class RWExport RWStoreTable : public RWSet {
public:
  RWStoreTable();
  ~RWStoreTable();
  RWBoolean             add(const void*, int&);

// RWStringID is by-reference to accomodate bug in Borland 5.0 compiler
  RWBoolean             add(const RWStringID&, int&);  

// #2437 - Forward calls to relational ops. in presence of C++ Std. Lib
#ifndef RW_NO_STL
  RWBoolean             operator<=(const RWStoreTable& rws)
                        { return RWSet::operator<=(rws); }
  RWBoolean             operator<=(const RWSet& rws) const
                        { return RWSet::operator<=(rws); }
  RWBoolean             operator<=(const RWHashTable& rwht) const 
                        { return RWSet::operator<=(rwht); }
#endif

  static RWStoreTable* getStoreTable();
  static RWStoreTable* newStoreTable();
  static void freeStoreTable();

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

class RWExport RWUseStoreTable {
  RWBoolean  firstUse_;
  RWStoreTable * table_;
  RWUseStoreTable(const RWUseStoreTable&); // Not defined!
  void operator=(const RWUseStoreTable&);  // Not defined!
public:
  RWUseStoreTable();
  RWBoolean firstUse() { return firstUse_; }
  RWBoolean add(const void* p, int& i)
    { assert(0 != table_); return table_->add(p,i); }
  RWBoolean add(const RWStringID& p, int& i)
    { assert(0 != table_); return table_->add(p,i); }
  ~RWUseStoreTable();
};

class RWExport RWStoreEntry : public RWCollectable {
  RWDECLARE_COLLECTABLE(RWStoreEntry)
  friend class RWStoreTable;
  const void*           item;
  int                   objectNumber;
public:
	RWStoreEntry() {}
	~RWStoreEntry() {}
	RWStoreEntry(const void* c, int n): item(c), objectNumber(n) {}
  unsigned              hash() const;
  RWBoolean             isEqual(const RWCollectable* c) const;

public:
  RW_TYPESPEC  /* This macro usually expands to nothing */
};


class RWExport RWStoreStringID : public RWCollectable {
  RWDECLARE_COLLECTABLE(RWStoreStringID)
  friend class RWStoreTable;
  const RWStringID      item;
  int                   objectNumber;
public:
	RWStoreStringID() {}
	~RWStoreStringID() {}
  RWStoreStringID(const RWStringID& id, int n) : item(id), objectNumber(n) {}
  unsigned              hash() const;
  RWBoolean             isEqual(const RWCollectable* c) const;

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

/**
 *******************  RWReadTable declarations ***************************
 **/

// For the read table, just use a simple extendable vector of pointers or
// RWClassID. We may assume that both are numeric types and that RWClassID
// is no greater than a pointer. So the union works fine.

struct RWConstVoidPtr {
  union {
    const void   *ptr_;
    RWClassID    id_;
  };

  // DEC cxx doesn't like union members in mem-init list:
  RWConstVoidPtr() { ptr_ = 0; }
  RWConstVoidPtr(void* p) { ptr_ = p; }
  RWConstVoidPtr(RWClassID i) { ptr_ = 0; id_ = i; }

  operator const void*() { return ptr_; }
  operator RWClassID() { return id_; }
};

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWReadTable {
  RWConstVoidPtr *table_;
  size_t entries_;
  size_t capacity_;
  enum {InitialSize = 30, IncrementSize = 100};
public:
  RWReadTable();
  ~RWReadTable();
  RWConstVoidPtr append(RWConstVoidPtr);
  size_t entries() { return entries_; }
  
  RWConstVoidPtr operator() (size_t i) { return table_[i]; }

  static RWReadTable* getReadTable();
  static RWReadTable* newReadTable();
  static void freeReadTable();
};

class RWExport RWUseReadTable {
  RWBoolean firstUse_;
  RWReadTable *table_;
  RWUseReadTable(const RWUseReadTable&); // Not defined
  void operator=(const RWUseReadTable&); // Not defined
public:
  RWUseReadTable();
  ~RWUseReadTable();
  RWBoolean       firstUse() const { return firstUse_; }
  RWConstVoidPtr append(RWConstVoidPtr p)
    { assert(0 != table_); return table_->append(p); }
  size_t          entries() { return table_->entries(); }
  RWConstVoidPtr  operator() (size_t i) { return (*table_)(i); }
  const void*     getPtr(size_t i)
    { return (const void*)((*this)(i)); }
};

//Special fix for Borland 5.0, this is related to bug #2437.  Borland 5.0
//will not choose class scope over global scope ever, but will recognize
//this as a function template specialization.

#if !defined(RW_NO_STL) && (defined __TURBOC__ && (__TURBOC__ < 0x500))
inline bool operator<=
(const RWStoreTable& wasThis, const RWStoreTable& btr)
{
     return wasThis.RWSet::operator<=(btr);
}
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWSTORETABLE_H__ */
