/***************************************************************************
 *
 * ev_seq.cc - template definitions for RW_VSeq<StdColl,RWColl>
 * 
 * $Id: ev_seq.cc 18623 2000-03-03 08:28:13Z sih $
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

#define INFILE_TEMPL_DECL template <class StdColl, class RWColl>
#define VSS      RW_VSeq<StdColl, RWColl>

INFILE_TEMPL_DECL
typename VSS::size_type
VSS::removeAll(bool (*fn)(typename VSS::const_reference key,void*),void* x )
{
  typedef x_pointer_to_binary_function<const_reference, void*, bool>
          boolPtrFun;

  size_type ret = this->std().size();
  my_iter_ newEnd = RW_SL_STD(remove_if)(this->begin(),this->end(),
                                x_binder2nd<boolPtrFun>(fn,x));
  this->std().erase(newEnd,this->end());
  return ret - this->std().size();
}

INFILE_TEMPL_DECL
typename VSS::size_type
VSS::removeAll(typename VSS::const_reference key)
{
  size_type ret(this->std().size());

#if defined(__DECCXX) && !defined(RW_RWV12_STDLIB)
  my_iter_ newEnd = RW_SL_STD(remove)(this->begin(),this->end(),key);
#else
  // NOTE: probably bug when namespaces become supported since
  //       standard remove algorithm won't be in global namespace
  my_iter_ newEnd = RW_SL_STD_GLOBAL(remove)(this->begin(),this->end(),key);
#endif

  this->std().erase(newEnd,this->end());
  return ret - this->std().size();
}

INFILE_TEMPL_DECL
typename VSS::value_type
VSS::removeAt(typename VSS::size_type index)
{
#ifdef RWDEBUG
  size_type n_dbg = this->entries();
#endif  
  boundsCheck(index);
  my_iter_ i = this->begin();
  RW_SL_STD(advance)(i,index);
  value_type ret = *i;
  this->std().erase(i);
  RWPOSTCONDITION(this->entries() == n_dbg - 1);
  return ret;
}

INFILE_TEMPL_DECL
typename VSS::value_type
VSS::removeFirst()
{
  return this->removeAt(0);
}


INFILE_TEMPL_DECL
bool VSS::remove(bool (*fn)(typename VSS::const_reference,void*),void* x )
{
  for(my_iter_ hit = this->begin(); hit != this->end(); ++hit)  {
    if((*fn)(*hit,x)) {
      this->std().erase(hit);
      return true;
    }
  }
  return false;
}

INFILE_TEMPL_DECL
bool VSS::remove(typename VSS::const_reference key)
{
  for(my_iter_ i = this->begin(); i != this->end(); ++i) {
    if(*i == key) {
      this->std().erase(i);
      return true;
    }
  }
  return false;
}


INFILE_TEMPL_DECL
typename VSS::size_type
VSS::replaceAll(bool (*fn)(typename VSS::const_val_ref, void*),void* x,
                const_val_ref newVal)
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
typename VSS::size_type
VSS::replaceAll(typename VSS::const_val_ref oldVal, typename VSS::const_val_ref newVal)
{
  size_type count = 0;
  my_iter_ i = this->begin();

  for(/**/; i != this->end(); ++i) {
    if(*i == oldVal) {
      *i = newVal;
      ++count;
    }
  }
  return count;
}

#undef INFILE_TEMPL_DECL
#undef VSS     
