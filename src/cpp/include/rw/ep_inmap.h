#ifndef __RWEP_INMAP_H__
#define __RWEP_INMAP_H__

/***************************************************************************
 *
 * ep_inmap.h - RW_PMapAssoc<StdColl,RWColl,Key,T>
 *   
 *     : pointer-based Key/Data interface components for STL Associative
 *       Containers 
 *
 * $Id: ep_inmap.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/ep_mcntn.h"

// handy macros, #undef'd below
#define KEY(i)    ((*i).first)
#define DATA(i)   ((*i).second)

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class Key, class T>
class RWTExport RW_PMapAssoc
  : public RW_PMapCntnr<StdColl, RWColl, Key, T> {

private:
  typedef RW_PMapCntnr<StdColl,RWColl,Key,T>  base_type;

public:
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

  void  apply(void (*fn)(const Key*, T*&, void*),void* x)
  {
    for(iterator i = this->begin(); i != this->end(); ++i)
      (*fn)(KEY(i), DATA(i), x);
  }
  
  void  apply(void (*fn)(const Key*, const T*, void*),void*x) const
  {
    for(const_iterator i = this->begin(); i != this->end(); ++i)
      (*fn)(KEY(i), DATA(i), x);
  }

  // deprecated:
  void  applyToKeyAndValue(void (*fn)(const Key*, T*&, void*),void* x)
  { apply(fn,x); }

  // deprecated:
  void  applyToKeyAndValue(void (*fn)(const Key*,
                                      const T*, void*),void*x) const
  { apply(fn,x); }

  void       clear() { this->std().erase(this->begin(),this->end()); }

  void       clearAndDestroy();

  bool       contains(const Key* key) const
    { const_iterator i = this->std().find((Key*)key); return i != this->end();}

  bool       contains(bool(*fn)(value_type,void*),void* x) const
    { return base_type::contains(fn,x); }

  const Key*         find(const Key* key) const
  {
    const_iterator i = this->std().find((Key*)key);
    return ( i == this->end()) ? 0 : (Key*) KEY(i);
  }

  value_type find(bool (*fn)(value_type, void*), void* x) const
    { return base_type::find(fn,x); }

  const T*     findValue(const Key* key) const
  {
    const_iterator i = this->std().find((Key*)key);
    return ( i == this->end()) ? 0 : (const T*) DATA(i);
  }

  T*     findValue(const Key* key)
  {
    const_iterator i = this->std().find((Key*)key);
    return ( i == this->end()) ? 0 : (T*) DATA(i);
  }

  const Key* findKeyAndValue(const Key* key, const T*& vret) const
  {
    const_iterator i = this->std().find((Key*)key);
    if(i != this->end()) {
      vret = (const T*&) DATA(i);
      return (const Key*) KEY(i);
    }
    return 0;
  }

  const Key* findKeyAndValue(const Key* key, T*& vret)
  {
    const_iterator i = this->std().find((Key*)key);
    if(!(i == this->end())) {
      vret = (T*&) DATA(i);
      return (const Key*) KEY(i);
    }
    return 0;
  }

  size_type  occurrencesOf(const Key* k) const
    { return this->std().count((Key*)k); }

  size_type  occurrencesOf(bool (*fn)(value_type, void*),void* x) const
    { return base_type::occurrencesOf(fn,x); }

  Key*       remove(const Key* key)
  {
    iterator i = this->std().find((Key*)key);
    if(i != this->end()) {
      Key* ret = (Key*)(*i).first;
      this->std().erase(i);
      return ret;
    }
    return rwnil;
  }

  Key*        remove(bool(*fn)(value_type,void*),void* x)
    {
      iterator hit = this->begin();
      for(/**/; hit != this->end(); ++hit) {
        if ((*fn)(*(value_type*)(&*hit),x)) {
          Key* ret = (Key*)(*hit).first;
          this->std().erase(hit);
          return ret;
        }
      }
      return rwnil;
    }
      
  size_type removeAll(const Key* key) { return this->std().erase((Key*)key); }

  size_type removeAll(bool (*fn)(value_type, void*),void* x)
    {
      size_type    ret = this->entries();
      
      while (this->entries() && (*fn)(*(value_type*)&*(this->begin()),x))
        this->std().erase(this->begin());

      iterator iter(this->begin());
      iterator iter2;

      while (iter != this->end()) {
        iter2 = iter;
        ++iter2;
        if (iter2 != this->end() && (*fn)(*(value_type*)&*iter2,x))
          this->std().erase(iter2);
        else
          ++iter;
      }

      return ret - this->entries();
    }

//protected:           THIS IS NEVER USED ... ELIMINATE?
//  static value_type nilValue_;  // needed for base class
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};
#undef KEY
#undef DATA

/*
 * Special function for testing equality of two hash collections.
 * The standard algorithm 'equal' is not suitable due to the
 * inherent un-orderedness of hash collections.
 */
template <class RWColl>
#if defined( __GNUG__ ) 
bool hashMapCollEqual( const RWColl& h1, const RWColl& h2,
                       const typename RWColl::container_type &c );
#else
bool hashMapCollEqual( const RWColl& h1, const RWColl& h2 );
#endif

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ep_inmap.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ep_inmap.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEP_INMAP_H__ */
