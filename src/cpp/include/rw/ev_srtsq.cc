/***************************************************************************
 *
 * ev_srtsq.cc - template definitions for RW_VSrtSeq<StdColl,RWColl,Compare>
 * 
 * $Id: ev_srtsq.cc 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/epfunc.h"

#ifndef RW_HEADER_WITH_EXTENSION
#  include <algorithm>
#else
#  include <algorithm.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

#define INFILE_TEMPL_DECL template <class StdColl, class RWColl, class Compare> 
#define VSSS     RW_VSrtSeq<StdColl,RWColl,Compare>

INFILE_TEMPL_DECL
bool VSSS::contains(const_reference key) const
{
  const_my_iter_ hit = RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  if(hit == this->end())
    return false;
  return (*hit == key);
}

INFILE_TEMPL_DECL
bool VSSS::find(const_reference key, reference val) const
{
  const_my_iter_ hit = RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  if(hit != this->end() && *hit == key) {
    val = *hit;
    return true;
  }
  return false;
}

// Not efficient on classes without random access iterators. Override.
INFILE_TEMPL_DECL 
typename VSSS::size_type 
VSSS::index(const_reference key) const
{
  const_my_iter_ it = RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  if (it == this->end() || *it != key) return this->npos;
#ifndef RW_3ARG_DISTANCE
  return RW_SL_STD(distance)(this->begin(),it);
#else
  size_type ret = 0;
  RW_SL_STD(distance)(this->begin(),it,ret);
  return ret;
#endif
}

INFILE_TEMPL_DECL
typename VSSS::size_type
VSSS::insert(const StdColl& coll)
{
  size_type ret = coll.size();
  if (ret) {
    my_iter_ it = this->begin();
    my_iter_ first = coll.begin();
    my_iter_ last = coll.end();
    while(first != last)
      it = insertSorted(it,*first++);
  }
  return ret;
}


INFILE_TEMPL_DECL
typename VSSS::size_type
VSSS::occurrencesOf(const_reference key) const
{
  RW_SL_STD(pair)<const_my_iter_,const_my_iter_> range =
                        RW_SL_STD(equal_range)(this->begin(),this->end(),key, Compare());
#ifndef RW_3ARG_DISTANCE
  return RW_SL_STD(distance)(range.first,range.second);
#else
  size_type ret(0);
  RW_SL_STD(distance)(range.first,range.second,ret);
  return ret;
#endif
}

INFILE_TEMPL_DECL
typename VSSS::size_type
VSSS::removeAll(const_reference key)
{
  typename StdColl::iterator start = RW_SL_STD_GLOBAL(find)(this->begin(),this->end(),key);
  if(start == this->end())
    return 0;
  
  size_type counter(1);
  typename StdColl::iterator bound = start;
  ++bound;
  while(bound != this->end() && *bound == key) {
    ++bound;
    ++counter;
  }
  this->std().erase(start,bound);
  return counter;
}

INFILE_TEMPL_DECL
bool VSSS::remove(const_reference key)
{
  typename StdColl::iterator hit = 
    RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  if(hit != this->end() && *hit == key){
    this->std().erase(hit);
    return true;
  }
  return false;
}    

/** should be protected by #if RWDEBUG but xlC bug prevents it. Sigh **/
INFILE_TEMPL_DECL
bool VSSS::isSorted() const
{
  if(this->entries() < 2) return true; // empty and single are sorted
  const_my_iter_ trail;
  for(const_my_iter_ it = this->begin(); it != this->end(); /**/ )
  {
    trail = it++;
    if( it == this->end() ) break;
#ifdef RW_MEMBER_OPERATOR_SYNTAX_PROBLEM
    if ( Compare()( *it, *trail ))
#else
	if ( Compare().operator()( *it, *trail ))
#endif
      return false;
  }
  return true;
}

// protected methods
#if !defined(__SUNPRO_CC) || (__SUNPRO_CC<0x420 && !defined(RW_NO_STL))
INFILE_TEMPL_DECL
typename VSSS::my_iter_
VSSS::insertSorted(my_iter_ hint, const_reference a) 
{
  typename StdColl::iterator hit;
  Compare comp = Compare();  // avoid warning "comp used before set"
  if(hint == this->end())
    hit = RW_SL_STD(upper_bound)(this->begin(),this->end(),a,comp);
  else {
    // test for "held is 'less than' a or a is not 'less than' held"
    // which amounts to "less than or equal"
    if(comp(*hint,a) || ! comp(a,*hint))
      hit = RW_SL_STD(upper_bound)(this->begin(),this->end(),a,comp);
    else // a must be "greater than" the contained value
      hit = RW_SL_STD(upper_bound)(this->begin(),this->end(),a,comp);
  }
  return this->std().insert(hit,a);
}
#endif /* !SunPro*/
#undef INFILE_TEMPL_DECL
#undef VSS
