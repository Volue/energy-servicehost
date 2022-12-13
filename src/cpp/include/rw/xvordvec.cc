/*
 * Template definitions for RWTValOrderedVector<T>
 *
 * $Id: xvordvec.cc 18623 2000-03-03 08:28:13Z sih $
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

template <class T> void
RWTValOrderedVector<T>::clear()
{
  T defaultValue = T();		// avoid "used before set" warning
  register size_t i = 0;

  while (i < nitems_) {
    (*this)(i++) = defaultValue; 
  }
  nitems_ = 0;
  if (RWTValVector<T>::length() > RWDEFAULT_CAPACITY)
    this->reshape(RWDEFAULT_CAPACITY);
}


template <class T> RWBoolean
RWTValOrderedVector<T>::find(const T& key, T& ret) const
{
  size_t idx = index(key);
  return (idx != RW_NPOS) ? (ret = this->array_[idx], TRUE) : FALSE;
}

template <class T> size_t
RWTValOrderedVector<T>::index(const T& val) const
{
  for (register size_t i=0; i<nitems_; i++) {
    if ((*this)(i) == val)
      return i;
  }
  return RW_NPOS;
}

// Insert value at position "ipt"; value formerly at "ipt"
// gets moved to "ipt+1".
template <class T> void
RWTValOrderedVector<T>::insertAt(size_t ipt, const T& val)
{
  if (ipt > 0)
    boundsCheck(ipt-1);

  // Check for overflow:
  if(nitems_>=RWTValVector<T>::length())
    this->reshape(nitems_ + RWDEFAULT_RESIZE);

  // Slide right (could be very expensive)
  // Need to increment nitems_ before indexing to avoid bounds error!
  for(register size_t i=nitems_++; i>ipt; i--)
    (*this)(i) = (*this)(i-1);

  (*this)(ipt) = val;
}

template <class T> size_t
RWTValOrderedVector<T>::occurrencesOf(const T& val) const
{
  size_t count = 0;
  for (register size_t i=0; i<nitems_; i++)
    if ((*this)(i) == val) ++count;
  return count;
}

/*
 * Remove and return the first occurrence of an object which is equal to p
 */
template <class T> RWBoolean
RWTValOrderedVector<T>::remove(const T& p)
{
  size_t idx = index(p);
  if(idx == RW_NPOS)
    return FALSE;

  //else
  removeAt(idx);
  return TRUE;
}


/*
 * Remove and return the last element of self
 */
template <class T> T
RWTValOrderedVector<T>::removeLast()
{
  T ret( (*this)(nitems_-1) );  // construct ret from last item

  // reset now-unused cell to default value:
  (*this)(nitems_-1) = T();

  // now we can decrement nitems_ without causing a bounds error:
  --nitems_;
  return ret;
}


/*
 * Remove all items equal to val
 */
template <class T> size_t
RWTValOrderedVector<T>::removeAll(const T& val)
{
  register size_t i;
  size_t j = 0;

  for (i=0; i<nitems_; i++) {
    // Are they unequal?  If so, this value should be saved.
    if (!((*this)(i) == val))
    {
      // Avoid the copy if possible:
      if (i!=j) (*this)(j) = (*this)(i);
      ++j;
    }
  }

  size_t nremoved = nitems_-j;

  // reset now-unused cells to default value:
  T defaultValue = T();	          // avoid "used before set" warning
  while (j < i)
    (*this)(j++) = defaultValue;

  // now we can decrement nitems_ without a bounds error:
  nitems_ -= nremoved;

  return nremoved;
}

template <class T> T
RWTValOrderedVector<T>::removeAt(size_t ipt)
{
  boundsCheck(ipt);

  T temp = (*this)(ipt);

  // Slide left (could be very expensive):
  for(register size_t i=ipt; i<nitems_-1; i++) {
    (*this)(i) = (*this)(i+1);
  }

  // reset now-unused cell to default value:
  (*this)(nitems_-1) = T();

  // now we can decrement nitems_ without a bounds error:
  --nitems_;

  return temp;
}

template <class T> void
RWTValOrderedVector<T>::boundsCheck(size_t i) const {
  if (i>=nitems_)
    RWTHROW( RWBoundsErr(RWMessage(RWTOOL_INDEXERR, (unsigned)i, (unsigned)nitems_) ));
}


#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTValOrderedVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValOrderedVector,RWpistream,RWpostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValOrderedVector,RWbistream,RWbostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValOrderedVector,RWeistream,RWeostream)
#endif     


template <class T, class S>
void rwSaveGuts(S& str, const RWTValOrderedVector<T>& c)
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
void rwRestoreGuts(S& str, RWTValOrderedVector<T>& c)
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

template<class T>
RWBoolean rwCompareEqual(const RWTValOrderedVector<T>& lhs, const RWTValOrderedVector<T>& rhs)
{
	size_t l_len = lhs.length();
	size_t r_len = rhs.length();
	
	if (l_len != r_len)
		return FALSE;

	size_t i;
	for (i = 0; i < l_len; ++i)
	{
		if (!(lhs[i] == rhs[i]))
			return FALSE;
	}
	return TRUE;
}

