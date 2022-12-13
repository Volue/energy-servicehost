#ifndef __RWEV_SRTSQ_H__
#define __RWEV_SRTSQ_H__

/***************************************************************************
 *
 * ev_srtsq.h - RW_VSrtSeq<StdColl,RWColl,Compare>
 *   
 * : value-based sorted-collection interface components for STL Sequences
 * 
 * $Id: ev_srtsq.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/ev_seq.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class Compare>
class RWTExport RW_VSrtSeq : public RW_VSeq<StdColl,RWColl>
{
public:
  typedef typename StdColl::const_reference              const_reference;
  typedef bool (*testFunction)(const_reference, void*);
  typedef RW_VSeq<StdColl,RWColl>                        base_type;
  typedef typename StdColl::const_iterator               my_iter_;
  typedef typename StdColl::size_type                    size_type;
  typedef typename StdColl::reference                    reference;
  typedef typename StdColl::const_iterator               const_my_iter_;

  bool          contains(const_reference) const;
  bool          contains(bool(*fn)(const_reference,void*),void* x) const
    { return base_type::contains(fn,x); }

  bool          find(const_reference, reference) const;
  bool          find(bool(*fn)(const_reference,void*),void* x,
                     reference hit) const
    { return base_type::find(fn,x,hit); }

  size_type     index(const_reference key) const;
  size_type     index(bool (*fn)(const_reference, void*), void* x) const
    { return base_type::index(fn,x); }

  size_type     insert(const StdColl& coll);
  bool          insert(const_reference a)
    { insertSorted(this->begin(),a); return true; } // only fail thru exception

  size_type     occurrencesOf(const_reference) const;
  size_type     occurrencesOf(bool (*fn)(const_reference,void*),
                              void* x) const
    { return base_type::occurrencesOf(fn,x); }

  size_type     removeAll(const_reference);
  size_type     removeAll(bool (*fn)(const_reference, void*),void* x)
    { return base_type::removeAll(fn,x); }
  
  bool          remove(const_reference key);
  bool          remove(bool (*fn)(const_reference,void*),void* x)
    { return base_type::remove(fn,x); }

/** should be guarded by #if RWDEBUG but xlC bug prevents it. **/
  bool          isSorted() const;
  
protected:
#if !defined(__SUNPRO_CC) || (__SUNPRO_CC<0x420 && !defined(RW_NO_STL))
  my_iter_ insertSorted(my_iter_, const_reference);
#else
  my_iter_ insertSorted(my_iter_ hint, const_reference a) 
  {
    typename StdColl::iterator hit;
    Compare comp = Compare();  // avoid warning "comp used before set"
    if(hint == end())
     hit = RW_SL_STD(upper_bound)(begin(),end(),a,comp);
    else {
    // test for "held is 'less than' a or a is not 'less than' held"
    // which amounts to "less than or equal"
    if(comp(*hint,a) || ! comp(a,*hint))
//      hit = RW_SL_STD(upper_bound)(hint,end(),a,comp);
     hit = RW_SL_STD(upper_bound)(begin(),end(),a,comp);
    else // a must be "greater than" the contained value
//      hit = RW_SL_STD(upper_bound)(begin(),hint,a,comp);
      hit = RW_SL_STD(upper_bound)(begin(),end(),a,comp);
   }
   return std().insert(hit,a);
  }
#endif /* !SunPro */
protected:
  RW_VSrtSeq() { }  // can only be used as base class
  RW_VSrtSeq(const RW_VSrtSeq<StdColl,RWColl,Compare>&);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ev_srtsq.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ev_srtsq.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEV_SRTSQ_H__ */
