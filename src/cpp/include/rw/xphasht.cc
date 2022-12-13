/*
 * Template definitions for RWTPtrHashTable<TP> and RWTPtrHashTableIterator<TP>
 *
 * $Id: xphasht.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.3  1996/05/02 23:34:09  griswolf
 * Patch to allow degenerate (0 buckets) hash tables Scopus 3146
 *
 * Revision 7.2  1996/02/18 01:50:56  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.1  1995/11/17 08:53:49  jims
 * Merge old and new families of templates
 *
 * Revision 7.2  1995/09/05 20:19:29  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:14:10  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.2  1994/06/21  00:07:21  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 6.1  1994/04/15  19:47:41  vriezen
 * Move all files to 6.1
 *
 * Revision 1.9  1994/01/12  03:09:17  jims
 * Add constness to T* parameters where appropriate
 *
 * Revision 1.8  1993/11/08  10:55:10  jims
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
 * Revision 1.4  1993/02/17  18:37:06  keffer
 * Now uses RWTPtrVector<T> to hold buckets
 *
 * Revision 1.3  1993/02/07  22:32:54  keffer
 * Added copy constructor and assignment operator for iterator
 *
 * Revision 1.2  1993/01/29  20:02:48  keffer
 * Fixed potential memory leak in clearAndDestroy()
 *
 * Revision 1.1  1993/01/29  01:19:41  keffer
 * Initial revision
 *
 *
 ***************************************************************************
 */


template <class TP>
RWTPtrHashTable<TP>::RWTPtrHashTable
                     (
                       RW_HASH_FUNC(TP,hashFun),
                       size_t size
                     )
  :  table_(size ? size : 2, rwnil), // Minimum of two buckets
     nitems_(0),
     hashFun_(hashFun)
{;}

template <class TP>
RWTPtrHashTable<TP>::RWTPtrHashTable(const RWTPtrHashTable<TP>& v)
  :  table_(v.table_.length(), rwnil),
     nitems_(v.nitems_),
     hashFun_(v.hashFun_)
{
  size_t N = table_.length();
  for (size_t i=0; i<N; i++)
  {
    if (v.table_(i))
      table_(i) =
        RW_NEW(this,RWTPtrSlist<TP>::get_os_typespec())
          RWTPtrSlist<TP>(*v.table_(i));
  }
}

template <class TP>
RWTPtrHashTable<TP>::~RWTPtrHashTable() { clear(); }

template <class TP> RWTPtrHashTable<TP>&
RWTPtrHashTable<TP>::operator=(const RWTPtrHashTable<TP>& v)
{
  if(&v != this)
  {
    clear();        
    hashFun_ = v.hashFun_;      // get the hash function
    size_t N;
    table_.reshape(N = v.table_.length()); // get the size, and use it
    for (size_t i=0; i<N; i++)             // fill the table as needed
    {
      table_(i) =
        v.table_(i)
          ? RW_NEW(this,RWTPtrSlist<TP>::get_os_typespec()) RWTPtrSlist<TP>(*v.table_(i))
          : rwnil;
    }
    nitems_ = v.nitems_;        // fix up counter
  }
  return *this;
}

template <class TP> void
RWTPtrHashTable<TP>::apply(void (*applyFun)(TP*, void*), void* a)
{
  size_t N = table_.length();
  for (size_t i=0; i<N; i++)
  {
    if (table_(i))
      table_(i)->apply(applyFun,a);
  }
}

template <class TP> void
RWTPtrHashTable<TP>::clear()
{
  size_t i = table_.length();
  while (i--)
  {
    delete table_(i);           // Always ok to delete nil
    table_(i) = rwnil;
  }
  nitems_ = 0;
}

template <class TP> void
RWTPtrHashTable<TP>::clearAndDestroy()
{
  size_t i = table_.length();
  while (i--)
  {
    if (table_(i))
    {
      table_(i)->clearAndDestroy();
      delete table_(i);
      table_(i) = rwnil;
    }
  }
  nitems_ = 0;
}

template <class TP> RWBoolean
RWTPtrHashTable<TP>::contains(const TP* val) const
{
  if(0 == table_.length())
    return FALSE;
  RWTPtrSlist<TP>* p = table_(hashIndex(val));
  return p ? p->contains(val) : FALSE;
}

template <class TP> TP*
RWTPtrHashTable<TP>::find(const TP *val) const
{
  if(0 == table_.length())
    return 0;
  RWTPtrSlist<TP>* p = table_(hashIndex(val));
  return p ? p->find(val) : rwnil;
}

template <class TP> void
RWTPtrHashTable<TP>::insert(TP* val)
{
  if(0 == table_.length())
    return;
  size_t idx = hashIndex(val);

  // Allocate a chain if one doesn't already exist:
  if (table_(idx)==rwnil)
    table_(idx) =
      RW_NEW(this, RWTPtrSlist<TP>::get_os_typespec()) RWTPtrSlist<TP>;

  table_(idx)->insert(val);     // Insert into the chain

  nitems_++;                    // Update count
}

template <class TP> size_t
RWTPtrHashTable<TP>::occurrencesOf(const TP* val) const
{
  if(0 == table_.length())
    return 0;
  RWTPtrSlist<TP>* p = table_(hashIndex(val));
  return  p ? p->occurrencesOf(val) : 0;
}

template <class TP> TP*
RWTPtrHashTable<TP>::remove(const TP* val)
{
  if(0 == table_.length())
    return 0;
  RWTPtrSlist<TP>* p = table_(hashIndex(val));
  TP* ret = p ? p->remove(val) : rwnil;
  if (ret) nitems_--;
  return ret;
}

template <class TP> size_t
RWTPtrHashTable<TP>::removeAll(const TP* val)
{
  if(0 == table_.length())
    return 0;
  RWTPtrSlist<TP>* p = table_(hashIndex(val));
  size_t count = p ? p->removeAll(val) : 0;
  nitems_ -= count;
  return count;
}

template <class TP> void
RWTPtrHashTable<TP>::resize(size_t N)
{
  if( table_.length() == N) return; // save the effort

#ifdef RWDEBUG
  size_t oldNitems = nitems_;
#endif

  // Save old values -- NB: this involves a copy
  RWTPtrVector<RWTPtrSlist<TP> > tempTable =table_;

  table_.reshape(N);            // Resize internal table
  table_  = rwnil;              // Zero it
  nitems_ = 0;

  // Now iterate through the old collection, inserting each item
  for (size_t i=0; i<tempTable.length(); i++)
  {
    if (tempTable(i))
    {
      RWTPtrSlistIterator<TP> next(*tempTable(i));
      while ( ++next )
        insert(next.key());
      delete tempTable(i);
    }
  }
#ifdef RWDEBUG
  assert(nitems_==oldNitems);   // Make sure we got 'em all.
#endif
}

// Insert a value only if it doesn't already exist in the collection
// (this function is useful for implementing sets).
template <class TP> RWBoolean
RWTPtrHashTable<TP>::insertOnce(TP* val)
{
  if(0 == table_.length())
    return FALSE;
  size_t idx = hashIndex(val);

  if (table_(idx)==rwnil)
    table_(idx) =
      RW_NEW(this,RWTPtrSlist<TP>::get_os_typespec()) RWTPtrSlist<TP>;

  else if (table_(idx)->contains(val))
     return FALSE;

  table_(idx)->insert(val);
  nitems_++;
  return TRUE;
}

/****************************************************************
 *                                                              *
 *      Definitions for RWTPtrHashTableIterator<TP>             *
 *                                                              *
 ****************************************************************/

template <class TP>
RWTPtrHashTableIterator<TP>::RWTPtrHashTableIterator(RWTPtrHashTable<TP>& t)
  : hashTable_(&t),
    idx_(RW_NPOS),
    iterator_(rwnil)
{
  reset();
}

template <class TP>
RWTPtrHashTableIterator<TP>::RWTPtrHashTableIterator(const RWTPtrHashTableIterator<TP>& h)
  : hashTable_(h.hashTable_),
    idx_(h.idx_)
{
  iterator_ = h.iterator_ ?
    RW_NEW(this, RWTPtrSlistIterator<TP>::get_os_typespec())
      RWTPtrSlistIterator<TP>(*h.iterator_)
        : rwnil;
}

template <class TP> RWTPtrHashTableIterator<TP>&
RWTPtrHashTableIterator<TP>::operator=(const RWTPtrHashTableIterator<TP>& h)
{
  if (this != &h)
  {
    delete iterator_;
    hashTable_ = h.hashTable_;
    idx_       = h.idx_;
    iterator_ = h.iterator_?
      RW_NEW(this,RWTPtrSlistIterator<TP>::get_os_typespec())
        RWTPtrSlistIterator<TP>(*h.iterator_)
          : rwnil;
  }
  return *this;
}

template <class TP> RWBoolean
RWTPtrHashTableIterator<TP>::operator++()
{
  // either spin over empty slots or advance into a non-empty one
  while ( iterator_ && ++(*iterator_)==FALSE )
  {
    nextIterator();
  }
  return iterator_!=rwnil;
}

template <class TP> void
RWTPtrHashTableIterator<TP>::reset()
{
  delete iterator_;
  iterator_ = rwnil;

  // Advance to the first non-empty bucket:
  for (idx_=0; idx_ < hashTable_->table_.length(); idx_++)
  {
    if (hashTable_->table_(idx_))
    {
      iterator_ = RW_NEW(this, RWTPtrSlistIterator<TP>::get_os_typespec())
        RWTPtrSlistIterator<TP>(*hashTable_->table_(idx_));
      return;
    }
  }
}

template <class TP> void
RWTPtrHashTableIterator<TP>::reset(RWTPtrHashTable<TP>& t)
{
  hashTable_ = &t;
  reset();
}


// private function ---

template <class TP> void
RWTPtrHashTableIterator<TP>::nextIterator()
{
  while (++idx_ < hashTable_->table_.length())
  {
    if (hashTable_->table_(idx_))
    {
      if (iterator_) 
        iterator_->reset(*hashTable_->table_(idx_));
      else           
        iterator_ = RW_NEW(this, RWTPtrSlistIterator<TP>::get_os_typespec())
          RWTPtrSlistIterator<TP>(*hashTable_->table_(idx_));
      return;
    }
  }

  delete iterator_;             // Invalidate old iterator
  iterator_ = rwnil;

}
