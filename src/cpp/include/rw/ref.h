#ifndef __RWREF_H__
#define __RWREF_H__

/*
 * Base class for reference counted objects.  Inherits from RWMemoryPool.
 *
 * $Id: ref.h 18623 2000-03-03 08:28:13Z sih $
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

#ifndef __RWDEFS_H__
#include "rw/defs.h"
#endif
#if defined(RW_ATOMIC_INT_OPS)
#  include <windows.h>
#endif
class RWMutex;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWReference {
 protected:
  unsigned      refs_;          // (1 less than) number of references
 public:
  enum RWReferenceFlag { STATIC_INIT };
  RWReference(int initRef = 0) : refs_((unsigned)initRef-1) { }
  void          setRefCount(unsigned r) {refs_ = r-1;}
  RWReference(RWReferenceFlag) { }  // leave refs_ alone
  unsigned      references() const      {return refs_+1;}
#if !defined(RW_MULTI_THREAD)
  void          addReference()          {refs_++;}
  unsigned      removeReference()       {return refs_--;}
#else
#if !defined(RW_ATOMIC_INT_OPS)
  void          addReference(RWMutex&);
  unsigned      removeReference(RWMutex&);
#else
  void          addReference() { InterlockedIncrement((long*)(&refs_)); }
  unsigned      removeReference()
  {
    unsigned result = InterlockedExchangeAdd((long*)(&refs_), -1);
    return result;
  }
#endif
#endif

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWREF_H__ */
