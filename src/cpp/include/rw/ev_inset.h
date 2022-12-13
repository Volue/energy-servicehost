#ifndef __RWEV_INSET_H__
#define __RWEV_INSET_H__

/***************************************************************************
 *
 * ev_inset.h - RW_VAssoc<StdColl,RWColl>
 *   
 *     : value-based interface components for STL Associative Containers
 * 
 * $Id: ev_inset.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/ev_scntn.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl>
class RWTExport RW_VAssoc : public RW_VCntnr<StdColl, RWColl> {

private:
  typedef RW_VCntnr<StdColl, RWColl>               base_type;

public:
  typedef typename StdColl::value_type             value_type;
  typedef typename StdColl::size_type              size_type;
  typedef typename StdColl::iterator               my_iter_;
  typedef typename StdColl::const_iterator         const_my_iter_;
  typedef typename StdColl::reference              reference;
  typedef typename StdColl::const_reference        const_reference;

// additional interface components
  void  clear() { this->std().erase(this->begin(),this->end()); }

  bool          remove(const_reference key)
  {
    my_iter_ hit = this->std().find(key);
    if(hit != this->end()) {
      this->std().erase(hit);
      return true;
    }
    return false;
  }
  
  bool          remove(/** extends the interface **/
                          bool (*fn)(const_reference,void*),void*
                         );

  size_type     removeAll(const_reference key);
  size_type     removeAll(bool (*fn)(const_reference, void*),void*);

  void     Union(const StdColl&);
  void     Union(const RWColl& rwcoll)        { Union(rwcoll.std()); }

  void     intersection(const StdColl&);
  void     intersection(const RWColl& rwcoll) { intersection(rwcoll.std()); }

  void     difference(const StdColl&);
  void     difference(const RWColl& rwcoll)   { difference(rwcoll.std()); }

  void     symmetricDifference(const StdColl&);
  void     symmetricDifference(const RWColl& rwcoll)
                                { symmetricDifference(rwcoll.std()); }

  bool isSubsetOf(const RWColl& rhs) const
  {
     if(this->entries() > rhs.entries())
        return FALSE;

     const_my_iter_ itr = this->begin();
     while(itr != this->end() && rhs.contains(*itr) )
          ++itr;

     return this->end() == itr;
  }


  bool isSubsetMultiOf(const RWColl& rhs) const
  {
     if(this->entries() > rhs.entries())
        return FALSE;

     const_my_iter_ itr = this->begin();
     while(itr != this->end() && this->std().count(*itr) <=
                       rhs.std().count(*itr) )
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


// overridden interface components
  bool  contains(const_reference key) const
  {
    const_my_iter_ iter = this->std().find(key);
    return iter != this->end();
  }
  bool find(const_reference, value_type& ret) const;
  
  
  size_type occurrencesOf(const_reference key) const
    {return this->std().count(key);}
  
// Avoid hiding overloads
  bool  contains(bool(*fn)(const_reference,void*), void* x) const
    { return base_type::contains(fn,x); }

  bool  find(bool(*fn)(const_reference,void*), void* x, value_type& hit) const
    { return base_type::find(fn, x, hit); }

  size_type occurrencesOf(bool(*fn)(const_reference,void*), void* x) const
    { return base_type::occurrencesOf(fn, x); }
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ev_inset.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ev_inset.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEV_INSET_H__ */
