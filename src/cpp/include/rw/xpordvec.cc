/*
 * Template definitions for RWTPtrOrderedVector<TP>
 *
 * $Id: xpordvec.cc 18623 2000-03-03 08:28:13Z sih $
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

template <class TP> void
RWTPtrOrderedVector<TP>::clearAndDestroy()
{
  for (size_t i=0; i<entries(); i++)
  {
    delete (*this)(i);
  }
  clear();
}

template <class TP> TP*
RWTPtrOrderedVector<TP>::find(const TP* p) const
{
  size_t i=index(p);
  return i == RW_NPOS ? rwnil : (*this)(i);
}

template <class TP> size_t
RWTPtrOrderedVector<TP>::index(const TP* val) const
{
  for (register size_t i=0; i<nitems_; i++)
    if (*(*this)(i) == *val)
      return i;
  return RW_NPOS;
}

// Insert value at position "ipt"; value formerly at "ipt"
// gets moved to "ipt+1".
template <class TP> void
RWTPtrOrderedVector<TP>::insertAt(size_t ipt, TP* val)
{
  if (ipt > 0)
    boundsCheck(ipt-1);

  // Check for overflow:
  if(nitems_>=RWTPtrVector<TP>::length())
    this->reshape(nitems_ + RWDEFAULT_RESIZE);

  // Slide right (could be very expensive)
  // Need to increment nitems_ before indexing to avoid bounds error!
  for(register size_t i=nitems_++; i>ipt; i--)
    (*this)(i) = (*this)(i-1);

  (*this)(ipt) = val;
}

template <class TP> size_t
RWTPtrOrderedVector<TP>::occurrencesOf(const TP* val) const
{
  size_t count = 0;
  for (register size_t i=0; i<nitems_; i++)
    if (*(*this)(i) == *val)
      ++count;
  return count;
}

/*
 * Remove and return the first occurrence of an object with the
 * same value as the object pointed to by "p".
 */
template <class TP> TP*
RWTPtrOrderedVector<TP>::remove(const TP* p)
{
  size_t idx = index(p);
  return idx == RW_NPOS ? rwnil : removeAt(idx);
}

/*
 * Remove all occurrences of the value
 */
template <class TP> size_t
RWTPtrOrderedVector<TP>::removeAll(const TP* val)
{
  size_t j = 0;
  for (register size_t i=0; i<nitems_; i++) {
    // Are they unequal?  If so, this value should be saved.
    if (!(*(*this)(i) == *val)) {
      // Avoid the copy if possible:
      if (i!=j) (*this)(j) = (*this)(i);
      ++j;
    }
  }

  size_t nremoved = nitems_-j;
  nitems_ = j;
  return nremoved;
}

template <class TP> TP*
RWTPtrOrderedVector<TP>::removeAt(size_t ipt)
{
  boundsCheck(ipt);
  //  RWPRECONDITION2(ipt<nitems_, "RWTPtrOrderedVector::removeAt(size_t): index out of range");

  TP* temp = (*this)(ipt);

  // Slide left (could be very expensive):
  for(register size_t i=ipt; i<nitems_-1; i++)
    (*this)(i) = (*this)(i+1);

  --nitems_;
  return temp;
}

template <class TP> void
RWTPtrOrderedVector<TP>::boundsCheck(size_t i) const {
  if (i>=nitems_)
    RWTHROW( RWBoundsErr(RWMessage(RWTOOL_INDEXERR, (unsigned)i, (unsigned)nitems_) ));
}


#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTPtrOrderedVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrOrderedVector,RWpistream,RWpostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrOrderedVector,RWbistream,RWbostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrOrderedVector,RWeistream,RWeostream)
#endif     


template <class T, class S>
void rwSaveGuts(S& str, const RWTPtrOrderedVector<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  for (size_t i=0; i < c.entries(); ++i)
    str << *c[i];
}

template <class T, class S>
void rwRestoreGuts(S& str, RWTPtrOrderedVector<T>& c)
{
  size_t count;
#ifdef RW_FIX_XSIZE_T
  str.getSizeT(count);
#else
  str >> count;
#endif
  c.clear();
  for (size_t i=0; i < count; ++i) {
    T* p;
    str >> p;
    c.insert(p);
  }
}

template<class T>
RWBoolean rwDerefCompareEqual(const RWTPtrOrderedVector<T>& lhs, const RWTPtrOrderedVector<T>& rhs)
{
	size_t l_len = lhs.length();
	size_t r_len = rhs.length();
	
	if (l_len != r_len)
		return FALSE;

	size_t i;
	for (i = 0; i < l_len; ++i)
	{
		if (!(*lhs[i] == *rhs[i]))
			return FALSE;
	}
	return TRUE;
}

