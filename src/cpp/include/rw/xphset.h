#ifndef __RWXPHSET_H__
#define __RWXPHSET_H__
/*
 * RWTPtrHashSet<T>:  A Set of pointers to type T, using a hashed lookup
 *
 * $Id: xphset.h 18623 2000-03-03 08:28:13Z sih $
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
 * This class implements a parameterized Set of pointers to type T.
 * In a Set, only one instance of an object of a given value can be 
 * inserted into the collection.
 *
 * The implementation uses a hash table.
 *
 * Example use of this class:
 *
 *   #include <rw/cstring.h>
 *   #include <rw/tphset.h>
 *   
 *   unsigned myHash(const RWCString& s){ return s.hash(); }
 *   
 *   RWTPtrHashSet<RWCString> set(myHash);	// A Set of RWCStrings
 *   
 *   set.insert(new RWCString("a string"));
 *   set.insert(new RWCString("another string"));
 *   set.insert(new RWCString("a string"));	// Rejected (already exists in collection)
 *   RWCString key("a string");
 *   set.contains(&key);	// Returns true.
 *
 *
 * Note that the constructor for RWTPtrHashSet<T> takes a function with
 * prototype
 *
 *   unsigned hashFun(const T&);
 *
 * It should return a suitable hashing value for an instance of class T.
 * Usually, the definition for such a function is trivial because hashing
 * functions have been defined for all Rogue Wave supplied classes.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.9  1996/07/18 20:51:01  hart
 * removed guard around include thashfun.h
 *
 * Revision 7.8  1996/07/18 04:03:18  hart
 * ObjectStore changes
 *
 * Revision 7.7  1996/05/07 18:00:23  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.6  1996/01/04 15:16:38  pearson
 * Rename isSubset to isSubsetOf and isProperSubset to isProperSubsetOf
 *
 * Revision 7.5  1995/12/15 00:27:50  pearson
 * Remove isUnequal and rename isEqual to isEquivalent
 * Move isEquivalent, isSubset and isProperSubset to the base class
 * Add access adjustment due to private inheritance
 *
 * Revision 7.4  1995/12/14 21:45:54  pearson
 * Remove intersectWith
 *
 * Revision 7.3  1995/12/14 19:59:04  pearson
 * Make interface match STL based container classes
 *
 * Revision 7.2  1995/12/06 18:20:49  pearson
 * Change #include"rw/txhset.cc" to #include "rw/xphset.cc"
 *
 * Revision 7.1  1995/11/17 08:53:49  jims
 * Merge old and new families of templates
 *
 * Revision 7.3  1995/09/05 20:22:48  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:34:52  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:01:09  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.1.7.3  1994/09/20  16:32:51  nevis
 * Added set operations.
 *
 * Revision 6.3  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/06/21  00:34:10  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 6.1  1994/04/15  19:47:52  vriezen
 * Move all files to 6.1
 *
 * Revision 2.7  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.6  1993/11/08  13:16:37  jims
 * Port to ObjectStore
 *
 * Revision 2.5  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.4  1993/06/03  20:49:59  griswolf
 * remove inline insert() to avoid Sun CC problem.
 *
 * Revision 2.3  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.2  1993/01/29  01:27:09  keffer
 * Ported to cfront v3.0
 *
 *    Rev 1.0   25 May 1992 15:59:24   KEFFER
 * Initial revision.
 * 
 */
#include "rw/thashfun.h"
#include "rw/xphasht.h"

/****************************************************************
 *								*
 *		Declarations for RWTPtrHashSet<T>		*
 *								*
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T> class RWTExport RWTPtrHashSet : public RWTPtrHashTable<T>
{

public:

  RWTPtrHashSet
  (
    RW_HASH_FUNC(T,hashFun),
    size_t size = RWDEFAULT_CAPACITY
  ) : RWTPtrHashTable<T>(hashFun, size) { }

  virtual ~RWTPtrHashSet();

  // Member functions:
  virtual void		insert(T* a);

  // Implement set operation primitives union, disjuncion, intersection,
  // subet, proper subset, and equivalence; respectively:

//***********************************************************************
//                    Pre-STDLIB interface
//***********************************************************************
//  RWTValHashSet<T> &    operator+=(const RWTValHashSet<T> & h);
//  RWTValHashSet<T> &    operator-=(const RWTValHashSet<T> & h);
//  RWTValHashSet<T> &    operator*=(const RWTValHashSet<T> & h);
//  RWBoolean             operator<=(const RWTValHashSet<T> & h) const;
//  RWBoolean             operator< (const RWTValHashSet<T> & h) const;
//  RWBoolean             operator==(const RWTValHashSet<T> & h) const;
//  RWBoolean             operator!=(const RWTValHashSet<T> & h) const
//                        { return !operator==(h); };
//***********************************************************************
  RWTPtrHashSet<T> &  Union              (const RWTPtrHashSet<T> & h);
  RWTPtrHashSet<T> &  difference         (const RWTPtrHashSet<T> & h);
  RWTPtrHashSet<T> &  intersection       (const RWTPtrHashSet<T> & h);
  RWTPtrHashSet<T> &  symmetricDifference(const RWTPtrHashSet<T> & h);
  RWBoolean           isSubsetOf       (const RWTPtrHashSet<T> & h) const;
  RWBoolean           isProperSubsetOf (const RWTPtrHashSet<T> & h) const;
  RWBoolean           isEquivalent       (const RWTPtrHashSet<T> & h) const;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xphset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xphset.cc"
#endif

#define RWTPtrHashSetIterator RWTPtrHashTableIterator

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXPHSET_H__ */

