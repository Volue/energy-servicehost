#ifndef __RWXPSRTVEC_H__
#define __RWXPSRTVEC_H__

/*
 * Sorted pointer-based vector.  Inserts pointers to values using an insertion sort.
 *
 * $Id: xpsrtvec.h 18623 2000-03-03 08:28:13Z sih $
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
 * Stores a pointer to the inserted item into the collection according
 * to an ordering determined by the less-than (<) operator.
 *
 * Assumes that TP has:
 *   - well-defined equality semantics (TP::operator==(const TP&));
 *   - well-defined less-than semantics (TP::operator<(const TP&)).
 *
 * This class uses binary searches for efficient value-based retrievals.
 *
 ***************************************************************************
 */

#include "rw/tpordvec.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class TP> class RWTExport RWTPtrSortedVector : public RWTPtrOrderedVector<TP>
{

public:

  //Constructors
  RWTPtrSortedVector
  (
    size_t capac = RWDEFAULT_CAPACITY
  ) :  RWTPtrOrderedVector<TP>(capac) {;}

  RWTPtrSortedVector
  (
    const RWTPtrSortedVector<TP>& c
  ) : RWTPtrOrderedVector<TP>(c) { }

  // Overridden virtual functions
  virtual size_t	index(const TP* p) const;
  virtual void		insert(TP* p);
  virtual size_t	occurrencesOf(const TP* p) const;
  virtual TP*		remove(const TP* p);
  virtual size_t	removeAll(const TP* p);

protected:

  RWBoolean		bsearch(const TP*, size_t&) const; // binary search routine
  size_t		indexSpan(const TP*, size_t&) const;
#ifdef RWDEBUG
  RWBoolean		isSorted() const;
#endif

  RW_T_TYPESPEC  /* This macro usually expands to nothing */ 
};

#if !defined(__TURBOC__) || (__TURBOC__ >= 0x540)

#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTPtrSortedVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
class RWExport RWpistream; class RWExport RWpostream;
class RWExport RWbistream; class RWExport RWbostream;
class RWExport RWeistream; class RWExport RWeostream;
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrSortedVector,RWpistream,RWpostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrSortedVector,RWbistream,RWbostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrSortedVector,RWeistream,RWeostream)
#endif

template <class T, class S>
void rwSaveGuts(S& str, const RWTPtrSortedVector<T>& c);

template <class T, class S>
void rwRestoreGuts(S& str, RWTPtrSortedVector<T>& c);

#endif  /* !defined(__TURBOC__) || (__TURBOC__ >= 0x540) */

template<class T>
int rwDerefCompare(const RWTPtrSortedVector<T>& lhs, const RWTPtrSortedVector<T>& rhs);
   
template <class T>
inline RWBoolean operator<(const RWTPtrSortedVector<T>& lhs, const RWTPtrSortedVector<T>& rhs)
{
  return (rwDerefCompare(lhs, rhs) < 0);
}

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xpsrtvec.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xpsrtvec.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif	/* __RWXPSRTVEC_H__ */


