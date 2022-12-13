/*
 * Template definitions for RWTValHashSet<T>
 *
 * $Id: xvhset.cc 18623 2000-03-03 08:28:13Z sih $
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
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.7  1996/08/26 21:57:15  hart
 * HP (aCC) compiler port
 *
 * Revision 7.6  1996/05/30 02:50:54  hart
 * fully qualified name references
 *
 * Revision 7.5  1996/01/04 17:19:51  pearson
 * Change isSubset to isSubsetOf and isProperSubset to isProperSubsetOf
 *
 * Revision 7.4  1995/12/15 00:27:38  pearson
 * Remove isUnequal and rename isEqual to isEquivalent
 * Move isEquivalent, isSubset and isProperSubset to the base class
 * Add access adjustment due to private inheritance
 *
 * Revision 7.3  1995/12/14 21:46:11  pearson
 * Remove intersectWith
 *
 * Revision 7.2  1995/12/13 16:26:23  pearson
 * Make interface match the Tools STL-like container interface
 *
 * Revision 7.1  1995/11/17 08:53:49  jims
 * Merge old and new families of templates
 *
 * Revision 7.4  1995/09/05 21:24:15  jims
 * Use new copyright macro
 *
 * Revision 7.3  1995/06/07  20:11:27  sanders
 * Correct Bug ID:1751.  Added declaration of v in two places, that
 * it was missing.
 *
 * Revision 7.2  1995/05/01  19:20:15  griswolf
 * remove unneeded new/delete in intersection code
 *
 * Revision 7.1  1994/10/16  03:20:02  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.1.7.3  1994/09/20  16:35:23  nevis
 * Added set operations.
 *
 * Revision 6.3  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.2  1994/06/21  00:48:06  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 6.1  1994/04/15  19:48:21  vriezen
 * Move all files to 6.1
 *
 * Revision 1.2  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.1  1993/06/03  20:52:23  griswolf
 * Initial revision
 *
 *
 */

template <class T>
RWTValHashSet<T>::~RWTValHashSet() {}

template <class T> void
RWTValHashSet<T>::insert(const T& val)
{
  insertOnce(val);
}

/*
 * Perform subset test.
 */
template <class T> RWBoolean
RWTValHashSet<T>::isSubsetOf(const RWTValHashSet<T> & h) const
{
  // Cast away self-constness when making the iterator
  RWTValHashTableIterator<T>        iterator(* (RWTValHashSet<T> *)this);

  while (iterator())
    if (!h.contains(iterator.key()))
      return FALSE;
  return TRUE;
}

template <class T> RWBoolean
RWTValHashSet<T>::isProperSubsetOf(const RWTValHashSet<T> & h) const
{
  // A proper subset is a subset where sets are not equal, thus,
  // we can perform the subset test and exclude the case where they
  // are identical.  (Avoid == test because it must perform subset
  // operation also.)
  return (isSubsetOf(h) && (this->entries() != h.entries()));
}

/*
 * Perform insert operation on ourself for all members of second set.
 */
template <class T> RWTValHashSet<T> &
RWTValHashSet<T>::Union(const RWTValHashSet<T> & h)
{
  // Cast away constness when making the iterator
  RWTValHashTableIterator<T>      iterator(*((RWTValHashSet<T> *) &h));

  while (iterator())
    insert(iterator.key());
  return *this;
}

/*
 * Perform remove operation on ourself for all members of second set.
 */
template <class T> RWTValHashSet<T> &
RWTValHashSet<T>::difference(const RWTValHashSet<T> & h)
{
  // Cast away constness when making the iterator
  RWTValHashTableIterator<T>      iterator(*((RWTValHashSet<T> *) &h));

  while (iterator())
    remove(iterator.key());
  return *this;
}

/*
 * Find all elements in the Union but not in the intersection of the two
 * sets.
 */
template <class T> RWTValHashSet<T> &
RWTValHashSet<T>::symmetricDifference(const RWTValHashSet<T> & h)
{
  // Cast away constness when making the iterator
  RWTValHashTableIterator<T>      iterator(*((RWTValHashSet<T> *) &h));
  RWTValHashSet<T>                insertionSet(this->hashFun_);

  //Remove from *h* all elements in common with *this* and place into
  //*insertionSet*
  while (iterator()){
    if(!contains(iterator.key())) insertionSet.insert(iterator.key());
  }

  iterator.reset();

  //Remove from *this* all elements of *h* in common with *this*
  while (iterator())
    remove(iterator.key());


  //Make *this* the symmetric difference
  Union(insertionSet);

  return *this;
}

/*
 * Set intersection (destructive) explanation:
 *    The template-based hash table iterators do not have facilities
 *    for deleting the current item being pointed at.  Attempts to
 *    delete the item from under the iterator will cause errant
 *    iterator behavior (It will never stop!).  Thus, we will
 *    build a deletion set while we are iterating through ourselves
 *    since we cannot delete as we go), and then perform a delete on ourself.
 *
 * Optimization: if h is the empty set, return the empty set immediately.
 */
template <class T> RWTValHashSet<T> &
RWTValHashSet<T>::intersection(const RWTValHashSet<T> & h)
{
  // Check for case where h is empty first
  if (0 == h.entries())   {
    this->clear();
  } else {
    // Cast away self-constness when making the iterator
    RWTValHashTableIterator<T>       selfiterator(* (RWTValHashSet<T> *)this);
    RWTValHashSet<T>                 deletionSet(this->hashFun_);
    T                                v;
    // Declare temp here to avoid stack allocation in loop below

    while (selfiterator())  {
      v = selfiterator.key();
      if (!h.contains(v))
	deletionSet.insert(v);
    }

    difference(deletionSet);
  }
  return *this;
}

/*
 * Set comparison: Mathematically, if two sets are subsets of each other,
 * then they are identical.  We can perform an optimization whereby
 * we test to see if both sets are the same size, and then checking the
 * subset relation in one direction only.
 */
template <class T> RWBoolean
RWTValHashSet<T>::isEquivalent(const RWTValHashSet<T> & h) const
{
  return ( (this->entries() == h.entries()) && isSubsetOf(h) );
}

