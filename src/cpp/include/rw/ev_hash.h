#ifndef __RWEV_HASH_H__
#define __RWEV_HASH_H__

/***************************************************************************
 *
 * ev_hash.h - RW_VHAssoc<StdColl,RWColl>
 *   
 * : value-based interface components for STL Hashed Associative Containers
 * 
 * $Id: ev_hash.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:41  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.3  1996/07/18 04:27:33  hart
 * ObjectStore changes
 *
 * Revision 7.2  1996/05/07 23:45:09  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.1  1996/02/01 21:51:13  jims
 * Move to revision 7.1
 *
 * 
 ****************************************************************************/

#include "rw/ev_inset.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl>
class RWTExport RW_VHAssoc : public RW_VAssoc<StdColl, RWColl> {

private:
  typedef RW_VAssoc<StdColl, RWColl> base_type;

public:

  void	   UnionHash(const StdColl&);
  void	   UnionHash(const RWColl& rwcoll)    { UnionHash(rwcoll.std()); }

  void     intersectionHash(const StdColl&);
  void     intersectionHash(const RWColl& hc) { intersectionHash(hc.std()); }

  void     differenceHash(const StdColl&);
  void     differenceHash(const RWColl& hc)   { differenceHash(hc.std()); }

  void     symmetricDifferenceHash(const StdColl&);
  void     symmetricDifferenceHash(const RWColl& rwcoll)
				{ symmetricDifferenceHash(rwcoll.std()); }
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ev_hash.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ev_hash.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEV_INSET_H__ */

