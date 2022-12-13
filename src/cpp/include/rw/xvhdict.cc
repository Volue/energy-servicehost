/*
 * Template definitions for RWTValHashDictionary<K,V> and RWTValHashDictionaryIterator<K,V>
 *
 * $Id: xvhdict.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.4  1996/07/18 04:04:37  hart
 *  ObjectStore changes
 *
 * Revision 7.3  1996/05/26 23:42:25  hart
 * fixed typo
 *
 * Revision 7.2  1996/05/02 23:36:35  griswolf
 * Patch to allow degenerate (0 buckets) hash tables Scopus 3146
 *
 * Revision 7.1  1995/11/17 08:53:49  jims
 * Merge old and new families of templates
 *
 * Revision 7.2  1995/09/05 21:23:35  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:19:30  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.2  1994/06/21  00:48:01  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 6.1  1994/04/15  19:48:18  vriezen
 * Move all files to 6.1
 *
 * Revision 1.8  1993/11/08  14:57:02  jims
 * Port to ObjectStore
 *
 * Revision 1.7  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.6  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 1.5  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 1.4  1993/02/17  19:46:03  keffer
 * Now uses RWTPtrVector<T> rather than RWTValVector<T*>
 *
 * Revision 1.3  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 1.2  1993/02/07  22:32:54  keffer
 * Added copy constructor and assignment operator for iterator
 *
 * Revision 1.1  1993/01/29  20:46:34  keffer
 * Initial revision
 *
 *
 ***************************************************************************
 */

template <class K, class V>
RWTValHashDictionary<K,V>::RWTValHashDictionary
                           (
                             RW_HASH_FUNC(K, hashFun),
			     size_t size
			   )
  :  table_(size ? size : 2, rwnil), // Minimum of two buckets
     nitems_(0),
     hashFun_(hashFun)
{;}

template <class K, class V>
RWTValHashDictionary<K,V>::RWTValHashDictionary(const RWTValHashDictionary<K,V>& v)
  :  table_(v.table_.length(), rwnil),
     nitems_(v.nitems_),
     hashFun_(v.hashFun_)
{
  size_t N = table_.length();
  for (size_t i=0; i<N; i++)
  {
    // Use copy constructor:
    if (v.table_(i))
      table_(i) = RW_NEW(this, (RWTValSlistDictionary<K,V>::get_os_typespec()))
        RWTValSlistDictionary<K,V>(*v.table_(i));
  }
}

template <class K, class V>
RWTValHashDictionary<K,V>::~RWTValHashDictionary() { clear(); }

template <class K, class V> RWTValHashDictionary<K,V>&
RWTValHashDictionary<K,V>::operator=(const RWTValHashDictionary<K,V>& v)
{
  if (this != &v)
  {
    clear();
    hashFun_ = v.hashFun_;	// Transfer hashing function
    size_t N;
    table_.reshape(N = v.table_.length());
    for (size_t i=0; i<N; i++)	// Fill the table as needed
    {
      table_(i) = v.table_(i) ?
        RW_NEW(this, (RWTValSlistDictionary<K,V>::get_os_typespec()))
          RWTValSlistDictionary<K,V>(*v.table_(i)) : rwnil;
    }
    nitems_ = v.nitems_;	// Fix up counter
  }
  return *this;
}

template <class K, class V> V&
RWTValHashDictionary<K,V>::operator[](const K& key)
{
  RWASSERT(0 != table_.length());
  size_t idx = hashIndex(key);
  RWTValSlistDictionary<K,V>* chain = table_(idx);

  if (chain==rwnil)
    table_(idx) = chain =
      RW_NEW(this, (RWTValSlistDictionary<K,V>::get_os_typespec()))
        RWTValSlistDictionary<K,V>;

  size_t N = chain->entries();

  V& val = chain->operator[](key);
  if (N!=chain->entries()) nitems_++;	// Ugly, I know.

  return val;
}

template <class K, class V> void
RWTValHashDictionary<K,V>::applyToKeyAndValue(void (*applyFun)(const K&, V&, void*), void* a)
{
  size_t N = table_.length();
  for (size_t i=0; i<N; i++)
  {
    if (table_(i))
      table_(i)->applyToKeyAndValue(applyFun, a);
  }
}

template <class K, class V> void
RWTValHashDictionary<K,V>::clear()
{
  size_t i = table_.length();
  while (i--)
  {
    delete table_(i);
    table_(i) = rwnil;
  }

  nitems_ = 0;
}

template <class K, class V> RWBoolean
RWTValHashDictionary<K,V>::contains(const K& key) const
{
  if(0 == table_.length())
    return FALSE;
  // else
  RWTValSlistDictionary<K,V>* chain = table_(hashIndex(key));
  return chain ? chain->contains(key) : FALSE;
}

template <class K, class V> RWBoolean
RWTValHashDictionary<K,V>::find(const K& key, K& retKey) const
{
  if(0 == table_.length())
    return FALSE;
  // else
  RWTValSlistDictionary<K,V>* chain = table_(hashIndex(key));
  return chain ? chain->find(key, retKey) : FALSE;
}

template <class K, class V> RWBoolean
RWTValHashDictionary<K,V>::findValue(const K& key, V& retVal) const
{
  if(0 == table_.length())
    return FALSE;
  // else
  RWTValSlistDictionary<K,V>* chain = table_(hashIndex(key));
  return chain ? chain->findValue(key, retVal) : FALSE;
}

template <class K, class V> RWBoolean
RWTValHashDictionary<K,V>::findKeyAndValue(const K& key, K& retKey, V& retVal) const
{
  if(0 == table_.length())
    return FALSE;
  // else
  RWTValSlistDictionary<K,V>* chain = table_(hashIndex(key));
  return chain ? chain->findKeyAndValue(key, retKey, retVal) : FALSE;
}

template <class K, class V> RWBoolean
RWTValHashDictionary<K,V>::remove(const K& key)
{
  if(0 == table_.length())
    return FALSE;
  // else
  RWTValSlistDictionary<K,V>* chain = table_(hashIndex(key));
  if (chain)
  {
    if (chain->remove(key)) {nitems_--; return TRUE;}
  }
  return FALSE;
}

template <class K, class V> void
RWTValHashDictionary<K,V>::resize(size_t N)
{
 if (table_.length() == N) return; // save the effort

#ifdef RWDEBUG
  size_t oldNitems = nitems_;
#endif

  // Save old values -- NB: this involves a copy
  RWTPtrVector<RWTValSlistDictionary<K,V> > tempTable = table_;

  table_.reshape(N);		// Resize internal table
  table_  = rwnil;		// Zero it
  nitems_ = 0;

  // Now iterate through the old collection, inserting each item
  for (size_t i=0; i<tempTable.length(); i++)
  {
    if (tempTable(i))
    {
      RWTValSlistDictionaryIterator<K,V> next(*tempTable(i));
      while ( ++next )
	insertKeyAndValue(next.key(), next.value());
      delete tempTable(i);
    }
  }
#ifdef RWDEBUG
  assert(nitems_==oldNitems);	// Make sure we got 'em all.
#endif
}

/****************************************************************
 *								*
 *     Definitions for RWTValHashDictionaryIterator<K,V>	*
 *								*
 ****************************************************************/

template <class K, class V>
RWTValHashDictionaryIterator<K,V>::RWTValHashDictionaryIterator(RWTValHashDictionary<K,V>& d) :
  myDict_(&d),
  idx_(RW_NPOS),
  iterator_(rwnil)
{
  nextChain();	// Advance to the first chain
}

// Copy constructor:
template <class K, class V>
RWTValHashDictionaryIterator<K,V>::RWTValHashDictionaryIterator
(
 const RWTValHashDictionaryIterator<K,V>& h
)
  :  myDict_(h.myDict_),
     idx_(h.idx_)
{
  iterator_ = h.iterator_ ?
    RW_NEW(this, (RWTValSlistDictionaryIterator<K,V>::get_os_typespec()))
      RWTValSlistDictionaryIterator<K,V>(*h.iterator_) : rwnil;
}

// Assignment operator:
template <class K, class V> RWTValHashDictionaryIterator<K,V>&
RWTValHashDictionaryIterator<K,V>::operator=
(
 const RWTValHashDictionaryIterator<K,V>& h
)
{
  if (this != &h)
  {
    delete iterator_;
    myDict_  = h.myDict_;
    idx_     = h.idx_;
    iterator_ = h.iterator_ ?
      RW_NEW(this, (RWTValSlistDictionaryIterator<K,V>::get_os_typespec()))
        RWTValSlistDictionaryIterator<K,V>(*h.iterator_) : rwnil;
  }
  return *this;
}

template <class K, class V> RWBoolean
RWTValHashDictionaryIterator<K,V>::operator++()
{
 while (iterator_ && ++(*iterator_)==FALSE)
  {
    nextChain();
  }
  return iterator_!=rwnil;
}

template <class K, class V> void
RWTValHashDictionaryIterator<K,V>::reset()
{
  idx_ = RW_NPOS;
  nextChain();
}

template <class K, class V> void
RWTValHashDictionaryIterator<K,V>::reset(RWTValHashDictionary<K,V>& t)
{
  myDict_ = &t;
  idx_ = RW_NPOS;
  nextChain();
}

// Advance the iterator to work on the next chain.
template <class K, class V> void
RWTValHashDictionaryIterator<K,V>::nextChain()
{
  while (++idx_ < myDict_->table_.length())
  {
    if (myDict_->table_(idx_))
    {
      if (iterator_) 
	iterator_->reset(*myDict_->table_(idx_));
      else           
        iterator_ =
          RW_NEW(this, (RWTValSlistDictionaryIterator<K,V>::get_os_typespec()))
                    RWTValSlistDictionaryIterator<K,V>(*myDict_->table_(idx_));
      return;
    }
  }
  delete iterator_;
  iterator_ = rwnil;
}
