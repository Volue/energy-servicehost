/*
 * Template definitions for RWTPtrHashDictionary<KP,VP>
 * and RWTPtrHashDictionaryIterator<KP,VP>
 *
 * $Id: xphdict.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.5  1996/07/18 04:04:37  hart
 *  ObjectStore changes
 *
 * Revision 7.4  1996/05/26 23:42:25  hart
 * fixed typo
 *
 * Revision 7.3  1996/05/09 19:12:19  griswolf
 * Scopus 3146: Cannot return reference to pointer 0. Use RWASSERT
 * instead.
 *
 * Revision 7.2  1996/05/02 23:34:09  griswolf
 * Patch to allow degenerate (0 buckets) hash tables Scopus 3146
 *
 * Revision 7.1  1995/11/17 08:53:49  jims
 * Merge old and new families of templates
 *
 * Revision 7.2  1995/09/05 20:22:06  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:14:20  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.2  1994/06/21  00:42:37  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 1.8  1994/01/12  03:09:17  jims
 * Add constness to T* parameters where appropriate
 *
 * Revision 1.7  1993/11/08  12:17:42  jims
 * Port to ObjectStore
 *
 * Revision 1.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.5  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 1.4  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 1.3  1993/02/17  19:07:23  keffer
 * Now uses RWTPtrVector<T> instead of RWTValVector<T> for the buckets
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


template <class KP, class VP>
RWTPtrHashDictionary<KP,VP>::RWTPtrHashDictionary
                             (
                               RW_HASH_FUNC(KP,hashFun),
			       size_t size
			     )
  :  table_(size ? size : 2, rwnil),
     nitems_(0),
     hashFun_(hashFun)
{;}

template <class KP, class VP>
RWTPtrHashDictionary<KP,VP>::RWTPtrHashDictionary
                             (
			       const RWTPtrHashDictionary<KP,VP>& v
			     )
  :  table_(v.table_.length(), rwnil),
     nitems_(v.nitems_),
     hashFun_(v.hashFun_)
{
  size_t N = table_.length();
  for (size_t i=0; i<N; i++)
  {
    // Use copy constructor:
    if (v.table_(i))
      table_(i) =
        RW_NEW(this, (RWTPtrSlistDictionary<KP,VP>::get_os_typespec()))
          RWTPtrSlistDictionary<KP,VP>(*v.table_(i));
  }
}

template <class KP, class VP> 
RWTPtrHashDictionary<KP,VP>::~RWTPtrHashDictionary() { clear(); }

template <class KP, class VP> RWTPtrHashDictionary<KP,VP>&
RWTPtrHashDictionary<KP,VP>::operator=(const RWTPtrHashDictionary<KP,VP>& v)
{
  if( this != &v)
  {
    clear();
    hashFun_ = v.hashFun_;	// Transfer hashing function
    size_t N;
    table_.reshape(N = v.table_.length());
    for (size_t i=0; i<N; i++)	// Fill the table as needed
    {
      table_(i) = v.table_(i) ?
        RW_NEW(this, (RWTPtrSlistDictionary<KP,VP>::get_os_typespec()))
          RWTPtrSlistDictionary<KP,VP>(*v.table_(i)) : rwnil;
    }
    nitems_ = v.nitems_;	// Fix up counter
  }
  return *this;
}

template <class KP, class VP> VP*&
RWTPtrHashDictionary<KP,VP>::operator[](KP* key)
{
  RWASSERT(0 != table_.length());
  size_t idx = hashIndex(key);
  RWTPtrSlistDictionary<KP,VP>* chain = table_(idx);

  if (chain==rwnil)
    table_(idx) = chain =
      RW_NEW(this, (RWTPtrSlistDictionary<KP,VP>::get_os_typespec()))
        RWTPtrSlistDictionary<KP,VP>;

  size_t N = chain->entries();

  VP*& val = chain->operator[](key);
  if (N!=chain->entries()) nitems_++;	// Ugly, I know.

  return val;
}

template <class KP, class VP> void
RWTPtrHashDictionary<KP,VP>::applyToKeyAndValue(void (*applyFun)(KP*,VP*&,void*), void* a)
{
  size_t N = table_.length();
  for (size_t i=0; i<N; i++)
  {
    if (table_(i))
      table_(i)->applyToKeyAndValue(applyFun, a);
  }
}

template <class KP, class VP> void
RWTPtrHashDictionary<KP,VP>::clear()
{
  size_t i = table_.length();
  while (i--)
  {
    delete table_(i);		// Always ok to delete nil
    table_(i) = rwnil;
  }

  nitems_ = 0;
}

template <class KP, class VP> void
RWTPtrHashDictionary<KP,VP>::clearAndDestroy()
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

template <class KP, class VP> RWBoolean
RWTPtrHashDictionary<KP,VP>::contains(const KP* key) const
{
  if(0 == table_.length())
    return FALSE;
  // else
  RWTPtrSlistDictionary<KP,VP>* chain = table_(hashIndex(key));
  return chain ? chain->contains(key) : FALSE;
}

template <class KP, class VP> KP*
RWTPtrHashDictionary<KP,VP>::find(const KP* key) const
{
  if(0 == table_.length())
    return 0;
  // else
  RWTPtrSlistDictionary<KP,VP>* chain = table_(hashIndex(key));
  return chain ? chain->find(key) : rwnil;
}

template <class KP, class VP> VP*
RWTPtrHashDictionary<KP,VP>::findValue(const KP* key) const
{
  if(0 == table_.length())
    return 0;
  // else
  RWTPtrSlistDictionary<KP,VP>* chain = table_(hashIndex(key));
  return chain ? chain->findValue(key) : rwnil;
}

template <class KP, class VP> KP*
RWTPtrHashDictionary<KP,VP>::findKeyAndValue(const KP* key, VP*& retVal) const
{
  if(0 == table_.length())
    return 0;
  // else
  RWTPtrSlistDictionary<KP,VP>* chain = table_(hashIndex(key));
  return chain ? chain->findKeyAndValue(key, retVal) : rwnil;
}

template <class KP, class VP> KP*
RWTPtrHashDictionary<KP,VP>::remove(const KP* key)
{
  if(0 == table_.length())
    return 0;
  // else
  RWTPtrSlistDictionary<KP,VP>* chain = table_(hashIndex(key));
  KP* ret = chain ? chain->remove(key) : rwnil;
  if (ret) nitems_--;
  return ret;
}

template <class KP, class VP> void
RWTPtrHashDictionary<KP,VP>::resize(size_t N)
{
 if (table_.length() == N) return; // save the effort

#ifdef RWDEBUG
  size_t oldNitems = nitems_;
#endif

  // Save old values -- NB: this involves a copy
  RWTPtrVector<RWTPtrSlistDictionary<KP,VP> > tempTable = table_;

  table_.reshape(N);		// Resize internal table
  table_  = rwnil;		// Zero it
  nitems_ = 0;

  // Now iterate through the old collection, inserting each item
  for (size_t i=0; i<tempTable.length(); i++)
  {
    if (tempTable(i))
    {
      RWTPtrSlistDictionaryIterator<KP,VP> next(*tempTable(i));
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
 *     Definitions for RWTPtrHashDictionaryIterator<KP,VP>	*
 *								*
 ****************************************************************/

template <class KP, class VP>
RWTPtrHashDictionaryIterator<KP,VP>::RWTPtrHashDictionaryIterator
(
 RWTPtrHashDictionary<KP,VP>& d
)
  :  myDict_(&d),
     idx_(RW_NPOS),
     iterator_(rwnil)
{
  reset();
}

// Copy constructor:
template <class KP, class VP>
RWTPtrHashDictionaryIterator<KP,VP>::RWTPtrHashDictionaryIterator
(
 const RWTPtrHashDictionaryIterator<KP,VP>& h
)
  :  myDict_(h.myDict_),
     idx_(h.idx_)
{
  iterator_ = h.iterator_ ?
    RW_NEW(this, (RWTPtrSlistDictionaryIterator<KP,VP>::get_os_typespec()))
      RWTPtrSlistDictionaryIterator<KP,VP>(*h.iterator_) : rwnil;
}

// Assignment operator:
template <class KP, class VP> RWTPtrHashDictionaryIterator<KP,VP>&
RWTPtrHashDictionaryIterator<KP,VP>::operator=
(
 const RWTPtrHashDictionaryIterator<KP,VP>& h
)
{
  if (this != &h)
  {
    delete iterator_;
    myDict_  = h.myDict_;
    idx_     = h.idx_;
    iterator_ = h.iterator_ ?
      RW_NEW(this, (RWTPtrSlistDictionaryIterator<KP,VP>::get_os_typespec()))
        RWTPtrSlistDictionaryIterator<KP,VP>(*h.iterator_) : rwnil;
  }
  return *this;
}

template <class KP, class VP> RWBoolean
RWTPtrHashDictionaryIterator<KP,VP>::operator++()
{
  while (iterator_ && ++(*iterator_)==FALSE)
  {
    nextChain();
  }
  return iterator_!=rwnil;
}

template <class KP, class VP> void
RWTPtrHashDictionaryIterator<KP,VP>::reset()
{
  delete iterator_;
  iterator_ = rwnil;

  // Advance to the first non-empty bucket:
  for (idx_=0; idx_ < myDict_->table_.length(); idx_++)
  {
    if (myDict_->table_(idx_))
    {
      iterator_ = 
        RW_NEW(this, (RWTPtrSlistDictionaryIterator<KP,VP>::get_os_typespec()))
          RWTPtrSlistDictionaryIterator<KP,VP>(*myDict_->table_(idx_));
      return;
    }
  }
}

template <class KP, class VP> void
RWTPtrHashDictionaryIterator<KP,VP>::reset(RWTPtrHashDictionary<KP,VP>& t)
{
  myDict_ = &t;
  reset();
}

// Advance the iterator to work on the next chain.
template <class KP, class VP> void
RWTPtrHashDictionaryIterator<KP,VP>::nextChain()
{
  while (++idx_ < myDict_->table_.length())
  {
    if (myDict_->table_(idx_))
    {
      if (iterator_) 
	iterator_->reset(*myDict_->table_(idx_));
      else           
	iterator_ = 
          RW_NEW(this, (RWTPtrSlistDictionaryIterator<KP,VP>::get_os_typespec()))
            RWTPtrSlistDictionaryIterator<KP,VP>(*myDict_->table_(idx_));
      return;
    }
  }

  delete iterator_;		// Invalidate old iterator
  iterator_ = rwnil;

}

