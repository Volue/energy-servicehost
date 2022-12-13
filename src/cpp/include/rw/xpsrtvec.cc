/*
 * Template definitions for RWTPtrSortedVector<TP>
 *
 * $Id: xpsrtvec.cc 18623 2000-03-03 08:28:13Z sih $
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


template <class TP> size_t
RWTPtrSortedVector<TP>::index(const TP* item) const
{
  size_t idx;
  if(! bsearch(item,idx))  // Return RW_NPOS if item is not in collection:
    return RW_NPOS;

  // The item is in the collection.
  // Search downwards looking for the first instance:

  while (idx && *(*this)(idx-1) == *item)
    --idx;

  RWPOSTCONDITION(*(*this)(idx) == *item);
  RWPOSTCONDITION(idx < this->nitems_);

  return idx;
}

template <class TP> void
RWTPtrSortedVector<TP>::insert(TP* p)
{
  // This algorithm uses only equality and less-than operators

#ifdef RWDEBUG
  size_t count = occurrencesOf(p);
#endif

  size_t idx;
  if (bsearch(p,idx))
  {

    // A matching item was found.  Insert after the
    // last equal item.
    while (idx < this->nitems_ && *(*this)(idx) == *p)
      ++idx;
  }
  else
  {
    // No matching item found.  Search upward
    // for the first item greater than the value
    // and insert before it.
    while (idx < this->nitems_ && *(*this)(idx) < *p)
      ++idx;
  }

  insertAt(idx, p);

  RWPOSTCONDITION(isSorted());
  RWPOSTCONDITION(occurrencesOf(p) == count+1);
}

template <class TP> size_t
RWTPtrSortedVector<TP>::occurrencesOf(const TP* p) const
{
  size_t iend;
  size_t istart = indexSpan(p, iend);
  return istart == RW_NPOS ? 0 : iend-istart+1;
}

/*
 * Remove and return the first occurrence of an object with the
 * same value as the object pointed to by "p".
 */
template <class TP> TP*
RWTPtrSortedVector<TP>::remove(const TP* p)
{
  size_t idx = index(p);
  return idx == RW_NPOS ? rwnil : removeAt(idx);
}

template <class TP> size_t
RWTPtrSortedVector<TP>::removeAll(const TP* p)
{
  size_t iend;
  size_t istart = indexSpan(p, iend);

  if (istart == RW_NPOS) return 0;
  iend++;
  size_t nremoved = iend-istart;

  // Do a "solid body" slide left of the remaining items in the collection:
  while (iend < this->nitems_)
    (*this)(istart++) = (*this)(iend++);

  this->nitems_ -= nremoved;
  RWPOSTCONDITION(!contains(p));
  return nremoved;
}

/****************************************************************
 *								*
 *	protected members of RWTPtrSortedVector<TP>		*
 *								*
 ****************************************************************/

template <class TP> RWBoolean
RWTPtrSortedVector<TP>::bsearch(const TP* key, size_t& idx) const
{
  RWPRECONDITION(isSorted());
  idx = 0;
  if (this->entries())
  {
    size_t top = this->entries() - 1;
    size_t bottom = 0;

    while (top>bottom)
    {
      idx = (top+bottom) >> 1;
      // It is important that we use only the equality and less-than
      // operators:
      if (*key == *(*this)(idx))
        return TRUE;
      else if(*key < *(*this)(idx))
        top    = idx ? idx - 1 : 0u;
      else
        bottom = idx + 1;
    }
    return *key == *(*this)(idx=bottom);
  }
  // Not found:
  return FALSE;
}

template <class TP> size_t
RWTPtrSortedVector<TP>::indexSpan(const TP* key, size_t& end) const
{
  // Do a binary search to find the first match:
  size_t istart = index(key);

  if (istart != RW_NPOS)
  {
    // Found one.  Do a linear search, looking for the last match:
    end = istart;
    while ( end+1 < this->entries() && *((*this)(end+1)) == *key)
     ++end;
  }

  return istart;
}

#ifdef RWDEBUG
template <class TP> RWBoolean
RWTPtrSortedVector<TP>::isSorted() const
{
  // dispense with trivial cases:
  if (this->nitems_ < 2)
    return TRUE;

  // otherwise:
  for(size_t idx=0; idx<this->nitems_-1; idx++)
  {
    /*
     * Not sorted if the item at this index is not less
     * than and not equal to the item at the next index
     * (i.e., it is greater than).
     */
    if(   !(*(*this)(idx) <  *(*this)(idx+1))
       && !(*(*this)(idx) == *(*this)(idx+1))
      )
      return FALSE;
  }
  return TRUE;
}
#endif


#if !defined(__TURBOC__) || (__TURBOC__ >= 0x540)

#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTPtrSortedVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrSortedVector,RWpistream,RWpostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrSortedVector,RWbistream,RWbostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTPtrSortedVector,RWeistream,RWeostream)
#endif


template <class T, class S>
void rwSaveGuts(S& str, const RWTPtrSortedVector<T>& c)
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
void rwRestoreGuts(S& str, RWTPtrSortedVector<T>& c)
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

#endif /* !defined(__TURBOC__) || (__TURBOC__ >= 0x540) */

template<class T>
int rwDerefCompare(const RWTPtrSortedVector<T>& lhs, const RWTPtrSortedVector<T>& rhs)
{
	size_t l_len = lhs.length();
	size_t r_len = rhs.length();
	
	if (l_len == 0)
		if (r_len == 0)
			return 0;
		else
			return -1;
	if (r_len == 0)
		return 1;

	size_t count;
	if (r_len < l_len)
		count = r_len;
	else
		count = l_len;

	size_t i;
	for (i = 0; i < count; ++i)
	{
		if (!(*lhs[i] == *rhs[i]))
		{
			if (*lhs[i] < *rhs[i])
				return -1;
			else
				return 1;
		}
	}

	if (l_len == r_len)
		return 0;
	else
	{
		if (l_len < r_len)
			return -1;
		else
			return 1;
	}
}

