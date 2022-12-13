#ifndef __RWXVSRTVEC_H__
#define __RWXVSRTVEC_H__

/*
 * Sorted value-based vector.  Inserts values using an insertion sort.
 *
 * $Id: xvsrtvec.h 18623 2000-03-03 08:28:13Z sih $
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
 * Stores a *copy* of the inserted item into the collection according
 * to an ordering determined by the less-than (<) operator.
 *
 * Assumes that T has:
 *   - well-defined copy semantics (T::T(const T&) or equiv.);
 *   - well-defined assignment semantics (T::operator=(const T&) or equiv.);
 *   - well-defined equality semantics (T::operator==(const T&));
 *   - well-defined less-than semantics (T::operator<(const T&)).
 *
 * Note that while these are automatically defined for builtin types
 * (such as "int", "double", or any pointer), you may need to provide
 * appropriate operators for your own classes, particularly those with
 * constructors and/or pointers to other objects.
 *
 * This class uses binary searches for efficient value-based retrievals.
 *
 ***************************************************************************
 */

#include "rw/xvordvec.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T> class RWTExport RWTValSortedVector : public RWTValOrderedVector<T>
{

public:

  //Constructors
  RWTValSortedVector
  (
    size_t capac = RWDEFAULT_CAPACITY
  ) :  RWTValOrderedVector<T>(capac) {;}

  RWTValSortedVector
  (
    const RWTValSortedVector<T>& c
  ) : RWTValOrderedVector<T>(c) {;}

  // Overridden virtual functions
  virtual size_t	index(const T& p) const;
  virtual void		insert(const T& p);
  virtual size_t	occurrencesOf(const T& p) const;
  virtual RWBoolean	remove(const T& p);
  virtual size_t	removeAll(const T& p);

protected:

  RWBoolean		bsearch(const T&, size_t&) const;	// binary search routine
  size_t		indexSpan(const T&, size_t&) const;
#ifdef RWDEBUG
  RWBoolean		isSorted() const;
#endif

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if !defined(__TURBOC__) || (__TURBOC__ >= 0x540) 

#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTValSortedVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
class RWExport RWpistream; class RWExport RWpostream;
class RWExport RWbistream; class RWExport RWbostream; 
class RWExport RWeistream; class RWExport RWeostream;
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValSortedVector,RWpistream,RWpostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValSortedVector,RWbistream,RWbostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValSortedVector,RWeistream,RWeostream)
#endif     


template <class T, class S>
void rwSaveGuts(S& str, const RWTValSortedVector<T>& c);

template <class T, class S>
void rwRestoreGuts(S& str, RWTValSortedVector<T>& c);

#endif  /* !defined(__TURBOC__) || (__TURBOC__ >= 0x540) */

template<class T>
int rwCompare(const RWTValSortedVector<T>& lhs, const RWTValSortedVector<T>& rhs);

template <class T>
inline RWBoolean operator<(const RWTValSortedVector<T>& lhs, const RWTValSortedVector<T>& rhs)
{
  return (rwCompare(lhs, rhs) < 0);
}

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xvsrtvec.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xvsrtvec.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXVSRTVEC_H__ */

