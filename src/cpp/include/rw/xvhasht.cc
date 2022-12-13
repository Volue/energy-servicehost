/*
 * Template definitions for RWTValHashTable<T> and RWTValHashTableIterator<T>
 *
 * $Id: xvhasht.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.3  1996/07/18 04:04:37  hart
 *  ObjectStore changes
 *
 * Revision 7.2  1996/05/02 23:36:35  griswolf
 * Patch to allow degenerate (0 buckets) hash tables Scopus 3146
 *
 * Revision 7.1  1995/11/17 08:53:49  jims
 * Merge old and new families of templates
 *
 * Revision 7.2  1995/09/05 20:57:35  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:19:23  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.2  1994/06/21  00:47:57  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 6.1  1994/04/15  19:48:13  vriezen
 * Move all files to 6.1
 *
 * Revision 1.10  1993/11/08  14:57:02  jims
 * Port to ObjectStore
 *
 * Revision 1.9  1993/09/15  18:44:44  keffer
 * Casts away constness in (safe) call to slist's apply().
 *
 * Revision 1.8  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.7  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 1.6  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 1.5  1993/02/17  19:46:03  keffer
 * Now uses RWTPtrVector<T> rather than RWTValVector<T*>
 *
 * Revision 1.4  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 1.3  1993/02/10  18:29:23  keffer
 * Added assertions.
 *
 * Revision 1.2  93/02/07  22:32:54  22:32:54  keffer (Tom Keffer)
 * Added copy constructor and assignment operator for iterator
 * 
 * Revision 1.1  1993/01/29  01:19:41  keffer
 * Initial revision
 *
 *
 ***************************************************************************
 */

template <class T>
RWTValHashTable<T>::RWTValHashTable
                    (
                      RW_HASH_FUNC(T,hashFun),
		      size_t size
		    )
  :  table_(size ? size : 2, rwnil), // Minimum of two buckets
     nitems_(0),
     hashFun_(hashFun)
{;}

template <class T>
RWTValHashTable<T>::RWTValHashTable(const RWTValHashTable<T>& v)
  :  table_(v.table_.length(), rwnil),
     nitems_(v.nitems_),
     hashFun_(v.hashFun_)
{
  size_t N = table_.length();
  for (size_t i=0; i<N; i++)
  {
    if (v.table_(i))
      table_(i) =
        RW_NEW(this, RWTValSlist<T>::get_os_typespec())
          RWTValSlist<T>(*v.table_(i)); //Use list copy constructor
  }
}

template <class T> 
RWTValHashTable<T>::~RWTValHashTable() { clear(); }

template <class T> RWTValHashTable<T>&
RWTValHashTable<T>::operator=(const RWTValHashTable<T>& v)
{
  if(&v != this)
  {
    clear();	    
    hashFun_ = v.hashFun_;	// get the hashfunction
    size_t N;
    table_.reshape(N = v.table_.length()); // get the size, and use it
    for (size_t i=0; i<N; i++)		   // fill the table as needed
    {
      table_(i) = v.table_(i) ?
        RW_NEW(this, RWTValSlist<T>::get_os_typespec()) RWTValSlist<T>(*v.table_(i))
          : rwnil;
    }
    nitems_ = v.nitems_;	// fix up counter
  }
  return *this;
}

template <class T> void
RWTValHashTable<T>::apply(void (*applyFun)(const T&, void*), void* a)
{
  size_t N = table_.length();
  for (size_t i=0; i<N; i++)
  {
    if (table_(i))
      table_(i)->apply((void (*)(T&, void*))applyFun,a);
  }
}

template <class T> void
RWTValHashTable<T>::clear()
{
  register size_t i = table_.length();
  while (i--)
  {
    delete table_(i);		// Always ok to delete nil
    table_(i) = rwnil;
  }
  nitems_ = 0;
}

template <class T> RWBoolean
RWTValHashTable<T>::contains(const T& val) const
{
  if(0 == table_.length())
    return FALSE;
  // else
  RWTValSlist<T>* p = table_(hashIndex(val));
  return p ? p->contains(val) : FALSE;
}

template <class T> RWBoolean
RWTValHashTable<T>::find(const T& val, T& retVal) const
{
  if(0 == table_.length())
    return FALSE;
  // else
  RWTValSlist<T>* p = table_(hashIndex(val));
  return p ? p->find(val, retVal) : FALSE;
}

template <class T> void
RWTValHashTable<T>::insert(const T& val)
{
  if(0 == table_.length())
    return;
  // else
  size_t idx = hashIndex(val);
  RWASSERT(idx<table_.length());

  // Allocate a chain if one doesn't already exist:
  if (table_(idx)==rwnil)
    table_(idx) = RW_NEW(this, RWTValSlist<T>::get_os_typespec()) RWTValSlist<T>;

  table_(idx)->insert(val);	// Insert into the chain

  nitems_++;			// Update count
}

template <class T> size_t
RWTValHashTable<T>::occurrencesOf(const T& val) const
{
  if(0 == table_.length())
    return 0;
  // else
  RWTValSlist<T>* p = table_(hashIndex(val));
  return  p ? p->occurrencesOf(val) : 0;
}

template <class T> RWBoolean
RWTValHashTable<T>::remove(const T& val)
{
  if(0 == table_.length())
    return FALSE;
  // else
  RWTValSlist<T>* p = table_(hashIndex(val));
  RWBoolean ret = p ? p->remove(val) : FALSE;
  if (ret) nitems_--;
  return ret;
}

template <class T> size_t
RWTValHashTable<T>::removeAll(const T& val)
{
  if(0 == table_.length())
    return 0;
  // else
  RWTValSlist<T>* p = table_(hashIndex(val));
  size_t count = p ? p->removeAll(val) : 0;
  nitems_ -= count;
  return count;
}

template <class T> void
RWTValHashTable<T>::resize(size_t N)
{
  if( table_.length() == N) return; // save the effort

  // Save old values -- NB: this involves a copy
  RWTPtrVector<RWTValSlist<T> > tempTable = table_;

  table_.reshape(N);		// Resize internal table
  table_  = rwnil;		// Zero it
  nitems_ = 0;

  // Now iterate through the old collection, inserting each item
  for (size_t i=0; i<tempTable.length(); i++)
  {
    if (tempTable(i))
    {
      RWTValSlistIterator<T> next(*tempTable(i));
      while ( ++next )
	insert(next.key());
      delete tempTable(i);
    }
  }
}

// protected methods

// Insert a value only if it doesn't already exist in the collection
// (this function is useful for implementing sets).
template <class T> RWBoolean
RWTValHashTable<T>::insertOnce(const T& val)
{
  if(0 == table_.length())
    return FALSE;
  // else
  size_t idx = hashIndex(val);
  RWASSERT(idx<table_.length());

  if (table_(idx)==rwnil)
     table_(idx) = RW_NEW(this, RWTValSlist<T>::get_os_typespec()) RWTValSlist<T>;
  else if (table_(idx)->contains(val))
     return FALSE;

  table_(idx)->insert(val);
  nitems_++;
  return TRUE;
}

/****************************************************************
 *								*
 *	Definitions for RWTValHashTableIterator<T>		*
 *								*
 ****************************************************************/

template <class T>
RWTValHashTableIterator<T>::RWTValHashTableIterator(RWTValHashTable<T>& t)
  :  hashTable_(&t),
     idx_(RW_NPOS),
     iterator_(rwnil)
{
  reset();
}

template <class T>
RWTValHashTableIterator<T>::RWTValHashTableIterator(const RWTValHashTableIterator<T>& h)
  : hashTable_(h.hashTable_),
    idx_(h.idx_)
{
  iterator_ = h.iterator_ ?
      RW_NEW(this, RWTValSlistIterator<T>::get_os_typespec())
      RWTValSlistIterator<T>(*h.iterator_) : rwnil;
}

template <class T> RWTValHashTableIterator<T>&
RWTValHashTableIterator<T>::operator=(const RWTValHashTableIterator<T>& h)
{
  if (this != &h)
  {
    delete iterator_;
    hashTable_ = h.hashTable_;
    idx_       = h.idx_;
    iterator_ = h.iterator_ ?
        RW_NEW(this, RWTValSlistIterator<T>::get_os_typespec())
        RWTValSlistIterator<T>(*h.iterator_) : rwnil;
  }
  return *this;
}

template <class T> RWBoolean
RWTValHashTableIterator<T>::operator++()
{
  // either spin over empty slots or advance into a non-empty one
  while ( iterator_ && ++(*iterator_)==FALSE )
  {
    nextIterator();
  }
  return iterator_!=rwnil;
}

template <class T> void
RWTValHashTableIterator<T>::reset()
{
  delete iterator_;
  iterator_ = rwnil;

  // Advance to the first non-empty bucket:
  for (idx_=0; idx_ < hashTable_->table_.length(); idx_++)
  {
    if (hashTable_->table_(idx_))
    {
      iterator_ = RW_NEW(this, RWTValSlistIterator<T>::get_os_typespec())
                  RWTValSlistIterator<T>(*hashTable_->table_(idx_));
      return;
    }
  }
}

template <class T> void
RWTValHashTableIterator<T>::reset(RWTValHashTable<T>& t)
{
  hashTable_ = &t;
  reset();
}


// private function ---

template <class T> void
RWTValHashTableIterator<T>::nextIterator()
{
  while (++idx_ < hashTable_->table_.length())
  {
    if (hashTable_->table_(idx_))
    {
      if (iterator_) 
	iterator_->reset(*hashTable_->table_(idx_));
      else           
        iterator_ = RW_NEW(this, RWTValSlistIterator<T>::get_os_typespec())
                    RWTValSlistIterator<T>(*hashTable_->table_(idx_));
      return;
    }
  }

  delete iterator_;		// Invalidate old iterator
  iterator_ = rwnil;

}

