/*
 * Template definitions for RWTValSortedVector<T>
 *
 * $Id: xvsrtvec.cc 18623 2000-03-03 08:28:13Z sih $
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


template <class T> size_t
RWTValSortedVector<T>::index(const T& item) const
{
  size_t idx;
  if(! bsearch(item,idx))  // Return RW_NPOS if item is not in collection:
    return RW_NPOS;

  // The item is in the collection.
  // Search downwards looking for the first instance:

  while (idx && (*this)(idx-1) == item)
    --idx;

  RWPOSTCONDITION((*this)(idx) == item);
  RWPOSTCONDITION(idx<this->nitems_);

  return idx;
}

template <class T> void
RWTValSortedVector<T>::insert(const T& val)
{
#ifdef RWDEBUG
  size_t count = occurrencesOf(val);
#endif

  size_t idx;
  if (bsearch(val,idx))
  {

    // A matching item was found.  Insert after the
    // last equal item.
    while (idx < this->nitems_ && (*this)(idx) == val)
      ++idx;
  }
  else
  {
    // No matching item found.  Search upward
    // for the first item greater than the value
    // and insert before it.
    while (idx < this->nitems_ && (*this)(idx) < val)
      ++idx;
  }

  insertAt(idx, val);
  RWPOSTCONDITION(isSorted());
  RWPOSTCONDITION(occurrencesOf(val) == count+1);
}

template <class T> size_t
RWTValSortedVector<T>::occurrencesOf(const T& val) const
{
  size_t iend;
  size_t istart = indexSpan(val, iend);
  return istart == RW_NPOS ? 0 : iend-istart+1;
}

/*
 * Remove and return the first occurrence of an object == val
 */
template <class T> RWBoolean
RWTValSortedVector<T>::remove(const T& val)
{
  size_t idx = index(val);

  if(idx == RW_NPOS)
    return FALSE;

  removeAt(idx);
  return TRUE;
}

template <class T> size_t
RWTValSortedVector<T>::removeAll(const T& val)
{
  size_t iend;
  size_t istart = indexSpan(val, iend);

  if (istart == RW_NPOS) return 0;
  iend++;
  size_t nremoved = iend-istart;

  // Do a "solid body" slide left of the remaining items in the collection:
  while (iend < this->nitems_)
    (*this)(istart++) = (*this)(iend++);

  // reset now-unused cells to default value:
  T defaultValue = T();  // avoid "used before set" warning
  while (istart < this->nitems_) {
    (*this)(istart++) = defaultValue;
  }
  
  this->nitems_ -= nremoved;
  RWPOSTCONDITION(!contains(val));
  return nremoved;
}

/****************************************************************
 *								*
 *	protected members of RWTValSortedVector<T>		*
 *								*
 ****************************************************************/

template <class T> RWBoolean
RWTValSortedVector<T>::bsearch(const T& key, size_t& idx) const
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
      if (key == (*this)(idx))
	return TRUE;
      else if (key < (*this)(idx))
        top    = idx ? idx - 1 : 0u;
      else
        bottom = idx + 1;
    }
    return key == (*this)(idx=bottom);
  }
  // Not found:
  return FALSE;
}

template <class T> size_t
RWTValSortedVector<T>::indexSpan(const T& key, size_t& end) const
{
  // Do a binary search to find the first match:
  size_t istart = index(key);

  if (istart != RW_NPOS)
  {
    // Found one.  Do a linear search, looking for the last match:
    end = istart;
    while ( end+1 < this->entries() && (*this)(end+1) == key)
      ++end;
  }

  return istart;
}

#ifdef RWDEBUG
template <class T> RWBoolean
RWTValSortedVector<T>::isSorted() const
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
    if (   !((*this)(idx) < (*this)(idx+1))
	&& !((*this)(idx) == (*this)(idx+1))
       )
      return FALSE;
  }
  return TRUE;
}
#endif


#if !defined(__TURBOC__) || (__TURBOC__ >= 0x540) 

#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTValSortedVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValSortedVector,RWpistream,RWpostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValSortedVector,RWbistream,RWbostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValSortedVector,RWeistream,RWeostream)
#endif     


template <class T, class S>
void rwSaveGuts(S& str, const RWTValSortedVector<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.entries());
#else
  str << c.entries();
#endif
  for (size_t i=0; i < c.entries(); ++i)
    str << c[i];
}

template <class T, class S>
void rwRestoreGuts(S& str, RWTValSortedVector<T>& c)
{
  size_t count;
#ifdef RW_FIX_XSIZE_T
  str.getSizeT(count);
#else
  str >> count;
#endif
  c.clear();
  for (size_t i=0; i < count; ++i) {
    T t;
    str >> t;
    c.insert(t);
  }
}

#endif /* !defined(__TURBOC__) || (__TURBOC__ >= 0x540) */

template<class T>
int rwCompare(const RWTValSortedVector<T>& lhs, const RWTValSortedVector<T>& rhs)
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
		if (!(lhs[i] == rhs[i]))
		{
			if (lhs[i] < rhs[i])
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

