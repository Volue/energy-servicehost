#ifndef __RWVREF_H__
#define __RWVREF_H__

/*
 * RWVirtualRef: Reference counted virtual array reference.
 *
 * $Id: vref.h 18623 2000-03-03 08:28:13Z sih $
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

#ifndef __RWREF_H__
#  include "rw/ref.h"
#endif
#include "rw/vpage.h"
STARTWRAP
#include <stdlib.h>
ENDWRAP
#ifdef RW_MULTI_THREAD
#include "rw/mutex.h"
#endif

typedef unsigned RWPageSlot;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWVirtualRef : public RWReference {

public:

  RWVirtualRef(long length, size_t elementSize, RWVirtualPageHeap* h);
  RWVirtualRef(const RWVirtualRef&);    // Deep copy
  virtual ~RWVirtualRef();

  long                  capacity() const        {return nSlots_*nPerPage_;}
  RWVirtualPageHeap*    heap() const            {return myHeap_;}
  long                  length() const          {return length_;}
  void                  slide(long start, long delta);
  void                  setSlice(long start1, long extent1,
                                RWVirtualRef& v, long start2, long extent2);
  void                  reshape(long newLength);

public:

  // The following three functions are formally intended to be
  // protected, but to avoid many friend functions have been made part
  // of the public interface.

  void                  findLocation(long, RWPageSlot&, unsigned&) const;
  void*                 lock(RWPageSlot p)      {return myHeap_->lock(handles_[p]);}
  void                  unlock(RWPageSlot p)    {myHeap_->unlock(handles_[p]);}

protected:

  // Neither augmentLeft nor augmentRight change the vector length.
  // They just add capacity to one side or the other.
  void                  augmentLeft(long);      // Add capacity on left
  void                  augmentRight(long);     // Add capacity on right
  void                  init();
  unsigned              pages()                 {return nSlots_;}
  unsigned              pageSize()              {return myHeap_->pageSize();}
  void                  dirty(RWPageSlot p)     {myHeap_->dirty(handles_[p]);}

  // Supplied by specializing class:
  virtual void          conformalCopy(long start1, RWVirtualRef& v2, long start2, long N) = 0;

protected:

  // Protected data.
  // Note that the length of the vector handles_ is nSlots_.

  long                  length_;        // Total number of elements
  long                  baseIndex_;     // Start of actual data
  unsigned              nPerPage_;      // Number of elements per page
  RWVirtualPageHeap*    myHeap_;        // The heap I'm getting my pages from
  RWHandle*             handles_;       // Array of handles
  RWPageSlot            nSlots_;        // Length of handles_.

#ifdef RW_MULTI_THREAD
public:
#  if !defined(RW_ATOMIC_INT_OPS)
  void addReference() { RWReference::addReference(mutexLock_); }
  unsigned removeReference()
    { return RWReference::removeReference(mutexLock_); }
#  else
  void addReference() { RWReference::addReference(); }
  unsigned removeReference()
		  { return RWReference::removeReference(); }
#  endif
private:
  RWMutex mutexLock_;
#endif

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWVREF_H__ */
