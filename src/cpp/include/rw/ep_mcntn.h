#ifndef __RWEP_MCNTN_H__
#define __RWEP_MCNTN_H__

/***************************************************************************
 *
 * ep_mcntn.h - RW_PMapCntnr<StdColl,RWColl,DataTypes>
 *   
 *     : pointer-based Key/Data interface components for STL Containers
 * 
 * $Id: ep_mcntn.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************/

#include "rw/ep_cntnr.h"
#include "rw/epersist.h"
#include "rw/rwerr.h"
#include "rw/toolerr.h"
#include "rw/edefs.h"

class RWExport RWFile;
class RWExport RWvistream;
class RWExport RWvostream;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class Key, class T>
class RWTExport RW_PMapCntnr : public RW_PBase<StdColl,RWColl>
{
public:
  typedef RW_PBase<StdColl,RWColl>            base_type;

  typedef typename StdColl::value_type        value_type;
  typedef typename StdColl::size_type         size_type;
  typedef typename StdColl::iterator          iterator;
  typedef typename StdColl::const_iterator    const_iterator;
  typedef typename StdColl::reference         reference;
  typedef typename StdColl::const_reference   const_reference;

  typedef Key*              value_type_key;
  typedef const Key*        const_value_key;
  typedef Key*&             reference_key;
  typedef const Key*const&  const_reference_key;

  typedef T*                value_type_data;
  typedef const T*          const_value_data;
  typedef T*&               reference_data;
  typedef const T*const&    const_reference_data;

  typedef rw_deref_pair_less<Key,T>    deref_less;
  typedef rw_deref_pair_equal<Key,T>   deref_equal;

  bool  operator==(const RWColl& rwc)   const
  { return this->entries() == rwc.entries() &&
      RW_SL_STD(equal)(this->begin(), this->end(), rwc.begin(), deref_equal()); }

  bool  operator<(const RWColl& rwc)   const
  { return RW_SL_STD(lexicographical_compare)(
                           this->std().begin(), this->std().end(), 
                                    rwc.begin(),        rwc.end(), 
                           deref_less()); }

  bool  contains(const_value_key key) const {
    const_iterator iter = this->begin();
    while(iter != this->end()) {
      if(*((*iter).first) == *key) break;
      ++iter;
    }
    return iter != this->end();
  }

  bool  contains(bool(*fn)(value_type,void*),void* x) const
    { return base_type::contains(fn,x); }

  value_type nilValue() const { return value_type(0,0); }
  value_type find(const_reference key) const;
  value_type find(bool (*fn)(value_type,void*), void* x) const;

  typename StdColl::size_type     index(bool (*fn)(value_type, void*), void* x)
    { return base_type::index(fn,x); }
#if !defined(RW_NO_INHERIT_TEMPL_TYPEDEF)
  typename StdColl::size_type     index(const_reference key) const;
#else
  typename StdColl::size_type     index(const_reference key) const
  {
    const_iterator i = this->begin();
    size_type idx = 0;
    for(/**/; i != this->end(); ++i) {
      if(deref_equal()(*i,key))
        return idx;
      ++idx;
	  }
    return this->npos;
  }
#endif /* RW_NO_INHERIT_TEMPL_TYPEDEF */

  typename StdColl::size_type     occurrencesOf(const_value_key key) const {
    size_type ret = 0;
    const_iterator iter = this->begin();
    while(iter != this->end()) {
      if (*((*iter).first) == *key) break;
      ++iter;
    }
    while(iter != this->end()) {
      if(*((*iter).first) == *key) ++ret;
      ++iter;
    }
    return ret;
  }

  typename StdColl::size_type   occurrencesOf(bool (*fn)(value_type,void*),void* x) const
    { return base_type::occurrencesOf(fn,x);  }
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ep_mcntn.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ep_mcntn.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEP_MCNTN_H__ */

