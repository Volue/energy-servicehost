/***************************************************************************
 *
 * ev_inset.cc - template definitions for RW_VAssoc<StdColl,RWColl>
 * 
 * $Id: ev_inset.cc 18623 2000-03-03 08:28:13Z sih $
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
#include "rw/stdex/algo.h"

#define INFILE_TEMPL_DECL template <class StdColl, class RWColl>
#define VSA      RW_VAssoc<StdColl, RWColl>

INFILE_TEMPL_DECL
bool VSA::find(const_reference key, typename VSA::value_type& ret) const
{
  const_my_iter_ iter = this->std().find(key);
  if(iter != this->end()) {
    ret = *iter;
    return true;
  }
  return false;
}

INFILE_TEMPL_DECL
typename VSA::size_type  
VSA::removeAll(const_reference key)
{
  size_type ret = this->std().size();
  RW_SL_STD(pair)<my_iter_,my_iter_> rng = this->std().equal_range(key);
  if(rng.first != this->end())
    this->std().erase(rng.first,rng.second);
  return ret - this->std().size();
}

INFILE_TEMPL_DECL
typename VSA::size_type  
VSA::removeAll(bool (*fn)(typename VSA::const_reference, void*), void* x)
{
  size_type    ret = this->entries();
  
  while (this->entries() && (*fn)(*(this->begin()),x))
    this->std().erase(this->begin());

  my_iter_ iter(this->begin());
  my_iter_ iter2;

  while (iter != this->end()) {
    iter2 = iter;
    ++iter2;
    if (iter2 != this->end() && (*fn)(*iter2,x))
      this->std().erase(iter2);
    else
      ++iter;
  }

  return ret - this->entries();
}


INFILE_TEMPL_DECL
bool VSA::remove(bool (*fn)(typename VSA::const_reference,void*), void* x)
{
  my_iter_ hit = this->begin();
  for(/**/; hit != this->end(); ++hit)
    if((*fn)(*hit,x))
      break;
  if(hit != this->end()) {
    this->std().erase(hit);
    return true;
  }
  return false;
}

INFILE_TEMPL_DECL
void VSA::Union(const StdColl& stdcoll)
{
// use algorithm supplied with Tools.h++
  RW_SL_STD(insert_iterator)<StdColl> putHere(this->std(),this->end());
  set_union_inplace(this->begin(),this->end(),stdcoll.begin(),stdcoll.end(),
                    putHere,this->std().key_comp());
} 


INFILE_TEMPL_DECL
void VSA::intersection(const StdColl& stdcoll)
{
// use algorithm supplied with Tools.h++
  set_intersection_inplace(this->std(),this->begin(),this->end(),
                        stdcoll.begin(),stdcoll.end(),this->std().key_comp());
}


INFILE_TEMPL_DECL
void VSA::difference(const StdColl& stdcoll)
{
// use algorithm supplied with Tools.h++
  set_difference_inplace(this->std(),this->begin(),this->end(),
                      stdcoll.begin(),stdcoll.end(),this->std().key_comp());
}


INFILE_TEMPL_DECL
void VSA::symmetricDifference(const StdColl& stdcoll)
{
// use algorithm supplied with Tools.h++
  set_symmetric_difference_inplace(this->std(),this->begin(),this->end(),
                       stdcoll.begin(),stdcoll.end(),this->std().key_comp());
}  

#undef INFILE_TEMPL_DECL
#undef VSA
