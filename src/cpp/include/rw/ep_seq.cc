/***************************************************************************
 *
 * ep_seq.cc - template definitions for RW_PSeq<StdColl,RWColl,DataTypes>
 * 
 * $Id: ep_seq.cc 18623 2000-03-03 08:28:13Z sih $
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
#include "rw/compiler.h"
#include "rw/epfunc.h"

#ifndef RW_HEADER_WITH_EXTENSION
#  include <algorithm>
#else
#  include <algorithm.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

#define INFILE_TEMPL_DECL template <class RWColl, class StdColl, class T>
#define PSS      RW_PSeq<RWColl,StdColl,T>

INFILE_TEMPL_DECL
void PSS::clearAndDestroy()
{
  for(my_iter_ it = this->begin(); it != this->end(); ++it)
    delete *it;
  clear();
}

INFILE_TEMPL_DECL
typename PSS::size_type
PSS::removeAll(bool (*fn)(const T*,void*),void* x )
{
  typedef x_pointer_to_binary_function<const_value, void*, bool>  boolPtrFun;
  size_type ret = this->entries();
  my_iter_ newEnd = RW_SL_STD_GLOBAL(remove_if)(this->begin(),this->end(),
                                x_binder2nd<boolPtrFun>(fn,x));
  
  this->std().erase(newEnd,this->end());
  return ret - this->entries();
}

INFILE_TEMPL_DECL
typename PSS::size_type
PSS::removeAll(bool (*fn)(T*,void*),void* x )
{
  typedef x_pointer_to_binary_function<value_type, void*, bool>  boolPtrFun;
  size_type ret = this->entries();
  my_iter_ newEnd = RW_SL_STD_GLOBAL(remove_if)(this->begin(),this->end(),
                                x_binder2nd<boolPtrFun>(fn,x));
  
  this->std().erase(newEnd,this->end());
  return ret - this->entries();
}

INFILE_TEMPL_DECL
typename PSS::size_type
PSS::removeAll(const T* key)
{
  size_type ret = this->entries();
  my_iter_ newEnd = RW_SL_STD(remove_if)(this->begin(),this->end(),deref_match<const_value>(key));
  this->std().erase(newEnd,this->end());
  return ret - this->entries();
}

INFILE_TEMPL_DECL
typename PSS::value_type
PSS::remove(bool (*fn)(const_value key,void*),void* x )
{
  value_type ret;
  for(my_iter_ hit = this->begin(); hit != this->end(); ++hit)  {
    if((*fn)(*hit,x)) {
      ret = *hit;
      this->std().erase(hit);
      return ret;
    }
  }
  return rwnil;
}

INFILE_TEMPL_DECL
typename PSS::value_type
PSS::remove(bool (*fn)(value_type key,void*),void* x )
{
  value_type ret;
  for(my_iter_ hit = this->begin(); hit != this->end(); ++hit)  {
    if((*fn)(*hit,x)) {
      ret = *hit;
      this->std().erase(hit);
      return ret;
    }
  }
  return rwnil;
}

INFILE_TEMPL_DECL
typename PSS::value_type
PSS::remove(const_value key)
{
  value_type ret;
  for(my_iter_ hit = this->begin(); hit != this->end(); ++hit) {
    if(**hit == *key) {
      ret = *hit;
      this->std().erase(hit);
      return ret;
    }
  }
  return rwnil;
}

INFILE_TEMPL_DECL
typename PSS::size_type
PSS::replaceAll(bool (*fn)(const_value, void*),
                      void* x, value_type newVal)
{
  size_type count = 0;
  my_iter_ i = this->begin();

  for(/**/; i != this->end(); ++i) {
    if((*fn)(*i,x)) {
      *i = newVal;
      ++count;
    }
  }
  return count;
}

INFILE_TEMPL_DECL
typename PSS::size_type
PSS::replaceAll(bool (*fn)(value_type, void*),
                      void* x, value_type newVal)
{
  size_type count = 0;
  my_iter_ i = this->begin();

  for(/**/; i != this->end(); ++i) {
    if((*fn)(*i,x)) {
      *i = newVal;
      ++count;
    }
  }
  return count;
}

INFILE_TEMPL_DECL
typename PSS::size_type
PSS::replaceAll(const_value oldVal, value_type newVal)
{
  size_type count = 0;
  my_iter_ i = this->begin();

  for(/**/; i != this->end(); ++i) {
    if (**i == *oldVal) {
      *i = newVal;
      ++count;
    }
  }
  return count;
}

#undef INFILE_TEMPL_DECL
#undef PSS
