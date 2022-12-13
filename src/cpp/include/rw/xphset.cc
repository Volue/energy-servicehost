/*
 * Template definitions for RWTPtrHashSet<T>
 *
 * $Id: xphset.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.8  1996/08/26 21:57:38  hart
 * HP (aCC) compiler port
 *
 * Revision 7.7  1996/05/30 02:49:14  hart
 * fully qualified name references
 *
 * Revision 7.6  1996/01/04 15:57:20  pearson
 * Add definition for symmetricDifference (it wasn't tested for before)
 *
 * Revision 7.5  1996/01/04 15:16:17  pearson
 * Rename isSubset to isSubsetOf and isProperSubset to isProperSubsetOf
 *
 * Revision 7.4  1995/12/15 00:27:27  pearson
 * Remove isUnequal and rename isEqual to isEquivalent
 * Move isEquivalent, isSubset and isProperSubset to the base class
 * Add access adjustment due to private inheritance
 *
 * Revision 7.3  1995/12/14 21:47:53  pearson
 * Remove intersectWith
 *
 * Revision 7.2  1995/12/14 19:59:38  pearson
 * Make interface match STL based container class interface
 *
 * Revision 7.1  1995/11/17 08:53:49  jims
 * Merge old and new families of templates
 *
 * Revision 7.4  1995/09/05 20:22:38  jims
 * Use new copyright macro
 *
 * Revision 7.3  1995/06/06  19:21:20  griswolf
 * Bug ID:1753 - fix typo on declaration of iterator.
 *
 * Revision 7.2  1995/05/01  19:20:15  griswolf
 * remove unneeded new/delete in intersection code
 *
 * Revision 7.1  1994/10/16  03:14:47  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.1.7.3  1994/09/20  16:33:54  nevis
 * Added set operations.
 *
 * Revision 6.3  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.2  1994/06/21  00:47:53  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 6.1  1994/04/15  19:47:51  vriezen
 * Move all files to 6.1
 *
 * Revision 1.2  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.1  1993/06/03  20:59:23  griswolf
 * Initial revision
 *
 */

template <class T>
RWTPtrHashSet<T>::~RWTPtrHashSet() {}

template <class T> void
RWTPtrHashSet<T>::insert(T* a)
{
  insertOnce(a);
}

/*
 * Perform subset test.
 */
template <class T> RWBoolean
RWTPtrHashSet<T>::isSubsetOf(const RWTPtrHashSet<T> & h) const
{
  // Cast away constness when making the iterator
  RWTPtrHashTableIterator<T>    iterator(* (RWTPtrHashSet<T> *)this);
  T                             *p;

  while (rwnil != (p=iterator()))
    if (!h.contains(p))
      return FALSE;
  return TRUE;
}

/*
 * Perform proper subset test.
 */
template <class T> RWBoolean
RWTPtrHashSet<T>::isProperSubsetOf(const RWTPtrHashSet<T> & h) const
{
  // A proper subset is a subset where sets are not equal, thus,
  // we can perform the subset test and exclude the case where they
  // are identical.  (Avoid == test because it must perform subset
  // operation also.)
  return (isSubsetOf(h) && (this->entries() != h.entries()));
}


// Implement set operation primitives union, disjuncion, intersection,
// and equivalence; respectively:

/*
 * Perform insert operation on ourself for all members of second set.
 */
template <class T> RWTPtrHashSet<T> &
RWTPtrHashSet<T>::Union(const RWTPtrHashSet<T> & h)
{
  // Cast away constness when making the iterator
  RWTPtrHashTableIterator<T>      iterator(*((RWTPtrHashSet<T> *) &h));
  T                               *p;

  while (rwnil != (p=iterator()))
    insert(p);
  return *this;
}

/*
 * Perform remove operation on ourself for all members of second set.
 */
template <class T> RWTPtrHashSet<T> &
RWTPtrHashSet<T>::difference(const RWTPtrHashSet<T> & h)
{
  // Cast away constness when making the iterator
  RWTPtrHashTableIterator<T>      iterator(*((RWTPtrHashSet<T> *) &h));
  T                               *p;

  while (rwnil != (p=iterator()))
    remove(p);
  return *this;
}

/*
 * Set intersection (destructive) explanation:
 *    The template-based hash table iterators do not have facilities
 *    for deleting the current item being pointed at.  Attempts to
 *    delete the item from under the iterator will cause errant
 *    iterator behavior (It will never stop!). Thus, we will
 *    build a deletion set while we are iterating through ourselves
 *    (since we cannot delete as we go), and then perform a delete on ourself.
 *
 * Optimization: if h is the empty set, return the empty set immediately.
 */
template <class T> RWTPtrHashSet<T> &
RWTPtrHashSet<T>::intersection(const RWTPtrHashSet<T> & h)
{
  // Check for case where h is empty first
  if (0 == h.entries())   {
    this->clear();
  } else {
    // Cast away self-constness when making the iterator
    RWTPtrHashTableIterator<T>       selfiterator(* (RWTPtrHashSet<T> *)this);
    RWTPtrHashSet<T>                 deletionSet(this->hashFun_);
    T                                *p;
    // Declare temp here to avoid stack allocation in loop below

    while (selfiterator())  {
      p = selfiterator.key();
      if (!h.contains(p))
	deletionSet.insert(p);
    }

    difference(deletionSet);
  }
  return *this;
}


/*
 * Find all elements in the Union but not in the intersection of the two
 * sets.
 */
template <class T> RWTPtrHashSet<T> &
RWTPtrHashSet<T>::symmetricDifference(const RWTPtrHashSet<T> & h)
{
  // Cast away constness when making the iterator
  RWTPtrHashTableIterator<T>      iterator(*((RWTPtrHashSet<T> *) &h));
  RWTPtrHashSet<T>                insertionSet(this->hashFun_);

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
 * Set comparison: Mathematically, if two sets are subsets of each other,
 * then they are identical.  We can perform an optimization whereby
 * we test to see if both sets are the same size, and then checking the
 * subset relation in one direction only.
 */
template <class T> RWBoolean
RWTPtrHashSet<T>::isEquivalent(const RWTPtrHashSet<T> & h) const
{
  return ( (this->entries() == h.entries()) &&  isSubsetOf(h)  );
}


