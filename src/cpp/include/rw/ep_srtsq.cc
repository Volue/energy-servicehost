/***************************************************************************
 *
 * ep_srtsq.cc - template definitions for
 *               RW_PSrtSeq<StdColl,RWColl,Compare,DataTypes>
 * 
 * $Id: ep_srtsq.cc 18623 2000-03-03 08:28:13Z sih $
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

#define FrmlTArgs  class StdColl, class RWColl, class Compare, class T
#define ActTArgs   StdColl, RWColl, Compare, T
#define INFILE_TEMPL_DECL   template<FrmlTArgs>
#define PSS        RW_PSrtSeq<ActTArgs>

INFILE_TEMPL_DECL
typename PSS::const_value
PSS::find(const_value key) const
{
  const_my_iter_ hit = RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  if(hit != this->end() && **hit == *key) {
    return *hit;
  }
  return 0;
}

INFILE_TEMPL_DECL
bool PSS::contains(const_value key) const
{
  const_my_iter_ hit = RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  if(hit == this->end())
    return false;
  return (**hit == *key);
}

// Not efficient on classes without random access iterators. Override.
INFILE_TEMPL_DECL 
typename PSS::size_type 
PSS::index(const_value key) const
{
  const_my_iter_ it = RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  if (it == this->end() || **it != *key) return this->npos;
  
#ifndef RW_3ARG_DISTANCE
  return RW_SL_STD(distance)(this->begin(),it);
#else
  size_type ret = 0;
  RW_SL_STD(distance)(this->begin(),it,ret);
  return ret;
#endif
}

INFILE_TEMPL_DECL
typename PSS::size_type
PSS::occurrencesOf(const_value key) const
{
  size_type ret = 0;
  const_my_iter_ iter = RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  while(iter != this->end() && **iter == *key) {
    ++iter;
    ++ret;
  }
  return ret;
}

INFILE_TEMPL_DECL 
typename PSS::size_type
PSS::removeAll(const_value key)
{
  my_iter_ start = RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  if(start == this->end())
    return 0;
  
  size_type oldsize = this->std().size();
  my_iter_ bound = start;
  ++bound;
  while(bound != this->end() && **bound == *key) {
    ++bound;
  }
  this->std().erase(start,bound);
  return oldsize - this->std().size();
}

INFILE_TEMPL_DECL
typename PSS::value_type PSS::remove(const_value key)
{
  my_iter_ hit = RW_SL_STD(lower_bound)(this->begin(),this->end(),key,Compare());
  if(hit != this->end() && **hit == *key){
    value_type ret = *hit;
    this->std().erase(hit);
    return ret;
  }
  return rwnil;
}    

/** should be protected by #if RWDEBUG but xlC bug prevents it. **/
INFILE_TEMPL_DECL
bool PSS::isSorted() const
{
  if(this->entries() < 2) return true; // empty and single are sorted
  const_my_iter_ trail;
  for(const_my_iter_ it = this->begin(); it != this->end();/**/)
  {
    trail = it++;
    if (it == this->end()) break;
    if (Compare()(*it,*trail))
      return false;
  }
  return true;
}

INFILE_TEMPL_DECL
typename PSS::size_type
PSS::insert(const StdColl& coll)
{
  size_type ret = coll.size();
  if (ret) {
    my_iter_ it = this->begin();
    const_my_iter_ first = coll.begin();
    const_my_iter_ last = coll.end();
    while(first != last)
      it = insertSorted(it,*first++);
  }
  return ret;
}

// protected functions

INFILE_TEMPL_DECL
typename PSS::my_iter_
PSS::insertSorted(typename PSS::my_iter_ hint, typename PSS::value_type a) 
{
  my_iter_ hit;
  my_iter_ last  = this->end();
  my_iter_ first = this->begin();
  
  Compare comp = Compare();  // avoid warning "comp used before value set"
  if(hint == last)
    hit = RW_SL_STD(upper_bound)(first,last,a,comp);
  else {
    // test for "held is 'less than' a or a is not 'less than' held"
    // which amounts to "less than or equal"
    if(comp(*hint,a) || ! comp(a,*hint))
      hit = RW_SL_STD(upper_bound)(hint,last,a,comp);
    else // the contained value must be "greater than" a
      hit = RW_SL_STD(upper_bound)(first,hint,a,comp);
  }
  return this->std().insert(hit,a);
}

#undef FrmlTArgs
#undef ActTArgs
#undef INFILE_TEMPL_DECL
#undef PSS
