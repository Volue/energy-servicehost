#ifndef __RWEP_HASH_H__
#define __RWEP_HASH_H__

/***************************************************************************
 *
 * ep_hash.h - RW_PHAssoc<StdColl,RWColl,T>
 *   
 * : pointer-based interface components for STL Hashed Associative Containers
 * 
 * $Id: ep_hash.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.5  1996/08/09 18:52:01  hart
 * HP (aCC compiler) port
 *
 * Revision 7.4  1996/07/18 04:27:33  hart
 * ObjectStore changes
 *
 * Revision 7.3  1996/05/08 17:00:19  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.2  1996/02/02 01:32:11  jims
 * Move definition of hashCollEqual to ep_hash.cc
 *
 * Revision 7.1  1996/02/01 21:51:13  jims
 * Move to revision 7.1
 *
 * 
 ****************************************************************************/

#include "rw/ep_inset.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class T>
class RWTExport RW_PHAssoc : public RW_PAssoc<StdColl,RWColl,T>
{
private:
  typedef RW_PAssoc<StdColl,RWColl,T>         base_type;

public:
  typedef typename StdColl::value_type        value_type;
  typedef typename StdColl::size_type         size_type;
  typedef typename StdColl::iterator          iterator;
  typedef typename StdColl::const_iterator    const_iterator;
  typedef typename StdColl::reference         reference;
  typedef typename StdColl::const_reference   const_reference;
  typedef const T*                            const_value;

  void     UnionHash(const RWColl& rwcoll)
    { UnionHash((StdColl&)rwcoll.std()); }

  void     intersectionHash(const RWColl& rwcoll)
    { intersectionHash((StdColl&)rwcoll.std()); }

  void     differenceHash(const RWColl& rwcoll)
    { differenceHash((StdColl&)rwcoll.std()); }

  void     symmetricDifferenceHash(const RWColl& rwcoll)
    { symmetricDifferenceHash((StdColl&)rwcoll.std()); }

protected:
  void     UnionHash(const StdColl&);
  void     intersectionHash(const StdColl&);
  void     differenceHash(const StdColl&);
  void     symmetricDifferenceHash(const StdColl&);

protected:
  RW_PHAssoc() { } // can only be used as base class 
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ep_hash.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ep_hash.cc"
#endif

/*
 * Special function for testing equality of two hash collections.
 * The standard algorithm 'equal' is not suitable due to the
 * inherent un-orderedness of hash collections.
 */
template <class RWColl>
bool hashCollEqual(const RWColl& h1, const RWColl& h2);

#ifdef RW_STD_COLLECTION_PROBLEM
template < class StdColl >
bool StdCollEqual( const StdColl& h1, const StdColl& h2 );
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEP_HASH_H__ */
