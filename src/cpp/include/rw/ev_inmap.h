#ifndef __RWEV_INMAP_H__
#define __RWEV_INMAP_H__

/***************************************************************************
 *
 * ev_inmap.h - RW_VMapAssoc<StdColl,RWColl,Key,T>
 *   
 *     : value-based Key/Data interface components for STL Associative
 *       Containers
 *
 * $Id: ev_inmap.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/ev_mcntn.h"

// handy macros, #undef'd below
#define KEY(i)    ((*i).first)
#define DATA(i)   ((*i).second)

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class Key, class T>
class RWTExport RW_VMapAssoc
    : public RW_VMapCntnr<StdColl, RWColl, Key, T>
{
public:
  typedef RW_VMapCntnr<StdColl, RWColl, Key, T>     base_type;
  typedef typename base_type::value_type            value_type;
  typedef typename base_type::size_type             size_type;
  typedef typename base_type::iterator              my_iter_;
  typedef typename base_type::const_iterator        const_my_iter_;
  typedef typename base_type::reference             reference;
  typedef typename base_type::const_reference       const_reference;

  void  apply(void (*fn)(const Key&, T&, void*), void* x);
  void  apply(void (*fn)(const Key&, const T&, void*),void*x) const;

  // deprecated in favor of "apply":
  void  applyToKeyAndValue(void (*fn)(const Key&, T&, void*),void* x)
  { apply(fn,x); }
      
  // deprecated in favor of "apply":
  void  applyToKeyAndValue(void (*fn)(const Key&, const T&, void*),
                                void*x) const
  { apply(fn,x); }

  void       clear() { this->std().erase(this->begin(),this->end()); }

  bool       contains(const Key& key) const
    { const_my_iter_ i = this->std().find(key); return i != this->end(); }

  bool          contains(bool(*fn)(const_reference,void*),void* x) const
    { return base_type::contains(fn,x); }

  bool       find(const Key& key, Key& ret) const
  {
    const_my_iter_ i=this->std().find(key);
    if(i != this->end()) {
      ret = KEY(i);
      return true;
    }
    return false;
  }

#ifndef RW_REF_SEEMS_PTR
  bool find(bool(*fn)(const_reference,void*),void* x, RW_SL_STD(pair)<Key,T>& val) const
  { return base_type::find(fn,x,val); }
#endif

  bool       findValue(const Key& key, T& ret) const
  {
    const_my_iter_ i=this->std().find(key);
    if(i != this->end()) {
      ret = DATA(i);
      return true;
    }
    return false;
  }

  bool       findKeyAndValue(const Key& key, Key& kret, T& vret) const
  {
    const_my_iter_ i=this->std().find(key);
    if(i != this->end()) {
      kret = KEY(i);
      vret = DATA(i);
      return true;
    }
    return false;
  }

  size_type  occurrencesOf(const Key& k) const { return this->std().count(k); }

  size_type  occurrencesOf(bool (*fn)(const_reference,void*),void* x) const
    { return base_type::occurrencesOf(fn,x); }

  bool       remove(const Key& key)
  {
    my_iter_ i = this->std().find(key);
    if(i != this->end()) {
      this->std().erase(i);
      return true;
    }
    return false;
  }

  bool remove(bool (*fn)(const_reference,void*),void* x);

  size_type  removeAll(const Key& key) { return this->std().erase(key); }

  size_type  removeAll(bool (*fn)(const_reference, void*), void* x);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#undef KEY
#undef DATA

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ev_inmap.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ev_inmap.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEV_INMAP_H__ */
