#ifndef __RWEP_INSET_H__
#define __RWEP_INSET_H__

/***************************************************************************
 *
 * ep_inset.h - RW_PAssoc<StdColl,RWColl,DataTypes>
 *   
 *     : pointer-based interface components for STL Associative Containers
 * 
 * $Id: ep_inset.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/ep_scntn.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class T>
class RWTExport RW_PAssoc : public RW_PCntnr<StdColl,RWColl,T>
{
private:
  typedef RW_PCntnr<StdColl,RWColl,T>         base_type;

public:
  typedef typename StdColl::value_type        value_type;
  typedef typename StdColl::size_type         size_type;
  typedef typename StdColl::iterator          iterator;
  typedef typename StdColl::const_iterator    const_iterator;
  typedef typename StdColl::reference         reference;
  typedef typename StdColl::const_reference   const_reference;
  typedef const T*                            const_value;


// Additional interface components
  void  clear() { this->std().erase(this->begin(),this->end()); }

// clearAndDestroy should be overridden in any leaf class that knows
// how to save its iterators while deleting. This version uses 2
// passes: One to delete, then clear()
  void  clearAndDestroy();
#if !defined(RW_NO_INHERIT_TEMPL_TYPEDEF) && (!defined(__SUNPRO_CC) || (__SUNPRO_CC<0x420 && !defined (RW_NO_STL)))
  typename StdColl::size_type    removeAll(const_value key);
  typename StdColl::size_type    removeAll(bool (*fn)(const_value, void*),void*);
#else

#if !defined(RW_NO_INHERIT_TEMPL_TYPEDEF)
  size_type    removeAll(const_value key);
  size_type    removeAll(bool (*fn)(const_value, void*),void*);
#else

  size_type removeAll(const_value key)
{
  size_type ret = this->entries();
  RW_SL_STD(pair)<iterator,iterator>
    rng = this->std().equal_range((T*)key);

  if(rng.first != this->end())
    this->std().erase(rng.first,rng.second);

  return ret - this->entries();
}

/*
 * NB: Assumes that erase does not invalidate iterators referring
 * to elements in front of the one being erased.
 */
 size_type removeAll(bool (*fn)(const_value, void*),void* x)
{
  size_type    ret = this->entries();
  
  while (this->entries() && (*fn)(*this->begin(),x))
    this->std().erase(this->begin());

  iterator iter(this->begin());
  iterator iter2;

  while (iter != this->end()) {
    iter2 = iter;
    ++iter2;
    if (iter2 != this->end() && (*fn)((const_value)*iter2,x))
      this->std().erase(iter2);
    else
      ++iter;
  }

  return ret - this->entries();
}
#endif // RW_NO_INHERIT_TEMPL_TYPEDEF

#endif /* !SunPro */

  value_type            remove(const_value key)
    {
      const_value ret = 0;
      iterator hit = this->std().find((T*)key);
      if(hit != this->end()) {
        ret = *iterator(hit);
        this->std().erase(hit);
      }
      return (value_type)ret;
    }

  value_type  remove(bool (*fn)(const_value,void*),void*);

// overridden interface components
  bool  contains(const_value key) const
  {
    const_iterator iter = this->std().find((T*)key);
    return iter != this->end();
  }

  const_value   find(const_value) const;

  size_type     occurrencesOf(const_value key)
    { return this->std().count((value_type)key); }

// Avoid hiding overloads
  bool  contains(bool(*fn)(const_value, void*),void* x) const
    { return  base_type::contains(fn,x); }

  size_type     occurrencesOf(bool(*fn)(const_value,void*),void* x) const
    { return base_type::occurrencesOf(fn,x); }

  const_value   find(bool(*fn)(const_value, void*),void* x) const
    { return base_type::find(fn,x); }

  void     Union(const RWColl& rwcoll)
    { Union((StdColl&)rwcoll.std()); }

  void     intersection(const RWColl& rwcoll)
    { intersection((StdColl&)rwcoll.std()); }

  void     difference(const RWColl& rwcoll)
    { difference((StdColl&)rwcoll.std()); }

  void     symmetricDifference(const RWColl& rwcoll)
    { symmetricDifference((StdColl&)rwcoll.std()); }

  bool isSubsetOf(const RWColl& rhs) const
  {
     if(this->entries() > rhs.entries())
        return FALSE;

     const_iterator itr = this->begin();
     while(itr != this->end() && rhs.contains(*itr) )
          ++itr;

     return this->end() == itr;
  }

  bool isSubsetMultiOf(const RWColl& rhs) const
  {
     if(this->entries() > rhs.entries())
        return FALSE;

     const_iterator itr = this->begin();
     while(itr != this->end() && this->std().count(*itr) 
	                        <= rhs.std().count(*itr) )
         ++itr;

     return this->end() == itr;
  }

  bool isProperSubsetOf(const RWColl& rhs) const
  { return isSubsetOf(rhs) && (this->entries() < rhs.entries()); }

  bool isProperSubsetMultiOf(const RWColl& rhs) const
  { return isSubsetMultiOf(rhs) && (this->entries() < rhs.entries()); }

  bool isEquivalent(const RWColl& rhs) const
  { return isSubsetOf(rhs) && (this->entries() == rhs.entries()); }

  bool isEquivalentMulti(const RWColl& rhs) const
  { return isSubsetMultiOf(rhs) && (this->entries() == rhs.entries()); }

protected:
  void     Union(const StdColl&);
  void     intersection(const StdColl&);
  void     difference(const StdColl&);
  void     symmetricDifference(const StdColl&);

protected:
  RW_PAssoc() { } // can only be used as base class 
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ep_inset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ep_inset.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEP_INSET_H__ */
