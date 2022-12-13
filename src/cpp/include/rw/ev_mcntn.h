#ifndef __RWEV_MCNTN_H__
#define __RWEV_MCNTN_H__

/***************************************************************************
 *
 * ev_mcntn.h - RW_VMapCntnr<StdColl,RWColl,Key,T>
 *   
 *     : value-based Key/Data interface components for STL Containers
 * 
 * $Id: ev_mcntn.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************/

#include "rw/edefs.h"
#include "rw/ev_cntnr.h"

class RWExport RWFile;
class RWExport RWvistream;
class RWExport RWvostream;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class Key, class T>
class RWTExport RW_VMapCntnr : public RW_VBase<StdColl, RWColl>
{
public:
  typedef typename StdColl::value_type               value_type;
  typedef typename StdColl::size_type                size_type;
  typedef typename StdColl::const_reference          const_reference;
  typedef typename StdColl::const_iterator           const_iterator;
  typedef typename StdColl::const_iterator           const_my_iter_;
  typedef typename StdColl::iterator                 my_iter_;
  typedef typename StdColl::iterator                 iterator;
  typedef typename StdColl::reference                reference;

  bool          find(
                        const_reference,
                        RW_SL_STD(pair)<Key,T>& val
                      ) const;

#ifndef RW_REF_SEEMS_PTR
  bool    find(
            bool(*fn)(const_reference,void*),
            void* x, RW_SL_STD(pair)<Key,T>& val
                      ) const;
#endif

protected:
  RW_VMapCntnr() { } // prevent direct usage
  RW_VMapCntnr(const RW_VMapCntnr<StdColl,RWColl,Key,T>&) { }
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ev_mcntn.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ev_mcntn.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEV_MCNTN_H__ */
