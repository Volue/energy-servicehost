#ifndef __RWSTL_ALGORITHM_H__
#define __RWSTL_ALGORITHM_H__
/***************************************************************************
 *
 * STL-(almost)-compliant algorithms
 * 
 * $Id: algo.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************
 * 
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.1  1996/01/11 23:21:07  jims
 * Move to revision 7.1
 *
 * Revision 1.2  1996/01/05 00:41:55  griswolf
 * Scopus #2160: Fix typo, add the remaining inplace set algorithms.
 *
 */

template <class IIter1, class IIter2, class OIter, class Compare>
void
// depends on the "inplace" container
//   -- being sorted
//   -- remaining sorted after an insert at any iterator
set_union_inplace(
		IIter1 first1, IIter1 bound1,
		IIter2 first2, IIter2 bound2,
		OIter self_inserter,
		Compare comp
	       )
{
  while (first1 != bound1 && first2 != bound2) {
    if (comp(*first1,*first2))
      ++first1;
    else if (comp(*first2,*first1))
      self_inserter = *first2++;
    else {
      ++first1;
      ++first2;
    }
  }
  while (first2 != bound2)
    self_inserter = *first2++;
}

template <class Container, class IIter1, class IIter2, class Compare>
void
// depends on the "inplace" container
//   -- being sorted
//   -- remaining sorted after a deletion at any iterator
set_intersection_inplace(
		Container& con,
		IIter1 first1, IIter1 bound1,
		IIter2 first2, IIter2 bound2,
		Compare comp
	       )
{
  while (first1 != bound1 && first2 != bound2) {
    if (comp(*first1,*first2))
      con.erase(first1++);
    else if (comp(*first2,*first1))
      ++first2;
    else {
      ++first1;
      ++first2;
    }
  }
  con.erase(first1,bound1);
}

template <class Container, class IIter1, class IIter2, class Compare>
void
// depends on the "inplace" container
//   -- being sorted
//   -- remaining sorted after a deletion at any iterator
set_difference_inplace(
		Container& con,
		IIter1 first1, IIter1 bound1,
		IIter2 first2, IIter2 bound2,
		Compare comp
	       )
{
  while (first1 != bound1 && first2 != bound2) {
    if (comp(*first1,*first2))
      ++first1;
    else if (comp(*first2,*first1))
      ++first2;
    else {
      con.erase(first1++);
      ++first2;
    }
  }
}

template <class Container, class IIter1, class IIter2, class Compare>
void
// depends on the "inplace" container
//   -- being sorted
//   -- remaining sorted after a deletion or insertion at any iterator
set_symmetric_difference_inplace(
		Container& con,
		IIter1 first1, IIter1 bound1,
		IIter2 first2, IIter2 bound2,
		Compare comp
	       )
{
  while (first1 != bound1 && first2 != bound2) {
    if (comp(*first1,*first2))
      first1++;
    else if (comp(*first2,*first1))
      con.insert(*first2++);
    else {
      con.erase(first1++);
      ++first2;
    }
  }
  while (first2 != bound2) con.insert(*first2++); 
}

#endif /*__RWSTL_ALGORITHM_H__*/
