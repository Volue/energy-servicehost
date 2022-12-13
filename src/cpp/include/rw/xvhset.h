#ifndef __RWXVHSET_H__
#define __RWXVHSET_H__
/*
 * RWTValHashSet<T>:  A Set of values of type T, using a hashed lookup
 *
 * $Id: xvhset.h 18623 2000-03-03 08:28:13Z sih $
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
 * This class implements a parameterized Set of types T.  In a Set,
 * only one instance of an object of a given value can be inserted into
 * the collection.
 *
 * The implementation uses a hash table.
 *
 * Example use of this class:
 *
 *   #include <rw/cstring.h>
 *   #include <rw/tvhset.h>
 *   
 *   unsigned myHash(const RWCString& s){ return s.hash(); }
 *   
 *   RWTValHashSet<RWCString> set(myHash);	// A Set of RWCStrings
 *   
 *   set.insert("a string");	// Type conversion: char* to RWCString happens
 *   set.insert("another string");
 *   set.insert("a string");	// Rejected (already exists in collection)
 *   set.contains("a string");	// Returns true.
 *
 *
 * Note that the constructor for RWTValHashSet<T> takes a function with
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
 * Revision 7.8  1996/07/18 20:51:01  hart
 * removed guard around include thashfun.h
 *
 * Revision 7.7  1996/07/18 04:03:18  hart
 * ObjectStore changes
 *
 * Revision 7.6  1996/05/07 17:39:57  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.5  1996/01/04 17:20:15  pearson
 * Change isSubset to isSubsetOf and isProperSubset to isProperSubsetOf
 *
 * Revision 7.4  1995/12/15 00:27:44  pearson
 * Remove isUnequal and rename isEqual to isEquivalent
 * Move isEquivalent, isSubset and isProperSubset to the base class
 * Add access adjustment due to private inheritance
 *
 * Revision 7.3  1995/12/14 21:45:37  pearson
 * Remove intersectWith
 *
 * Revision 7.2  1995/12/13 16:26:45  pearson
 * Make interface match the Tools STL-like container interface with respect
 * to the set operations
 *
 * Revision 7.1  1995/11/17 08:53:49  jims
 * Merge old and new families of templates
 *
 * Revision 7.3  1995/09/05 21:24:34  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:37:32  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:08:43  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.1.7.3  1994/09/20  16:34:20  nevis
 * Added set operations.
 *
 * Revision 6.3  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/06/21  00:34:22  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 6.1  1994/04/15  19:48:22  vriezen
 * Move all files to 6.1
 *
 * Revision 2.8  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.7  1993/11/08  20:32:10  jims
 * Port to ObjectStore
 *
 * Revision 2.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.5  1993/06/03  20:47:30  griswolf
 * remove inline insert() to avoid Sun CC problem.
 *
 * Revision 2.4  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.3  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 2.2  1993/01/29  01:27:09  keffer
 * Ported to cfront v3.0
 *
 *    Rev 1.2   17 Mar 1992 12:00:44   KEFFER
 * 
 *    Rev 1.1   04 Mar 1992 10:16:46   KEFFER
 * 
 *    Rev 1.0   02 Mar 1992 16:10:54   KEFFER
 * Initial revision.
 */

#include "rw/thashfun.h"
#include "rw/xvhasht.h"

/****************************************************************
 *								*
 *		Declarations for RWTValHashSet<T>		*
 *								*
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T> class RWTExport RWTValHashSet : public RWTValHashTable<T>
{

public:

  RWTValHashSet
  (
    RW_HASH_FUNC(T, hashFun),
    size_t size = RWDEFAULT_CAPACITY
  ) : RWTValHashTable<T>(hashFun, size)
  {;}

  virtual ~RWTValHashSet();

  // Member functions:
  virtual void		insert(const T& val);
 
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
  RWTValHashSet<T> &  Union              (const RWTValHashSet<T> & h);
  RWTValHashSet<T> &  difference         (const RWTValHashSet<T> & h);
  RWTValHashSet<T> &  intersection       (const RWTValHashSet<T> & h);
  RWTValHashSet<T> &  symmetricDifference(const RWTValHashSet<T> & h);
  RWBoolean           isSubsetOf       (const RWTValHashSet<T> & h) const;
  RWBoolean           isProperSubsetOf (const RWTValHashSet<T> & h) const;
  RWBoolean           isEquivalent       (const RWTValHashSet<T> & h) const;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xvhset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xvhset.cc"
#endif

#define RWTValHashSetIterator RWTValHashTableIterator

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXVHSET_H__ */

