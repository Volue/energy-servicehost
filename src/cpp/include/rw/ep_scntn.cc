/***************************************************************************
 *
 * ep_scntn.cc - template definitions for RW_PCntnr<StdColl,RWColl,DataTypes>
 *
 * $Id: ep_scntn.cc 18623 2000-03-03 08:28:13Z sih $
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

#define INFILE_TEMPL_DECL \
      template <class StdColl, class RWColl, class DataTypes>
#define PSC      RW_PCntnr<StdColl, RWColl, DataTypes>

INFILE_TEMPL_DECL void
PSC::apply(void (*fn)(typename PSC::const_value,void*),void *x) const {
  const_my_iter_ iter = this->begin();
  while(iter != this->end()) {
    (*fn)(*iter,x);
    ++iter;
  }
}

INFILE_TEMPL_DECL bool
PSC::contains(bool(*fn)(typename PSC::const_value,void*),void* x) const {
  const_my_iter_ iter = this->begin();
  while(iter != this->end()) {
    if((*fn)(*iter,x)) break;
    ++iter;
  }
  return iter != this->end();
}

INFILE_TEMPL_DECL bool
PSC::contains(typename PSC::const_value key) const {
  const_my_iter_ iter = this->begin();
  while(iter != this->end()) {
    if(deref_equal()(*iter,key)) break;
    ++iter;
  }
  return iter != this->end();
}

INFILE_TEMPL_DECL typename PSC::value_type
PSC::find(typename PSC::const_value key) const
{
    const_my_iter_ iter = this->begin();
    while(iter != this->end()) {
      if(deref_equal()(*iter,key)) break;
      ++iter;
    }
    if (iter == this->end()) return rwnil; else return *iter;
}

INFILE_TEMPL_DECL  typename PSC::value_type
PSC::find(bool (*fn)(typename PSC::const_value,void*), void* x) const
{
    const_my_iter_ iter = this->begin();
    while(iter != this->end()) {
      if((*fn)(*iter,x)) break;
      ++iter;
    }
    if (iter == this->end()) return rwnil; else return *iter;
}

INFILE_TEMPL_DECL  typename PSC::value_type
PSC::find(bool (*fn)(typename PSC::value_type,void*), void* x) const
{
    const_my_iter_ iter = this->begin();
    while(iter != this->end()) {
      if((*fn)(*iter,x)) break;
      ++iter;
    }
    if (iter == this->end()) return rwnil; else return *iter;
}

INFILE_TEMPL_DECL
typename PSC::size_type
PSC::index(bool (*fn)(typename PSC::const_value, void*), void* x) const
{
  const_my_iter_ i = this->begin();
  size_type idx = 0;
  for(/**/; i != this->end(); ++i) {
    if((*fn)(*i,x))
      return idx;
    ++idx;
  }
  return this->npos;
}

INFILE_TEMPL_DECL
typename PSC::size_type
PSC::index(typename PSC::const_value t) const
{
  const_my_iter_ i = this->begin();
  size_type idx = 0;
  for(/**/; i != this->end(); ++i) {
    if(deref_equal()(*i,t))
      return idx;
    ++idx;
  }
  return this->npos;
}

INFILE_TEMPL_DECL
typename PSC::size_type
PSC::occurrencesOf(bool (*fn)(typename PSC::const_value,void*),void* x) const {
  size_type ret = 0;
  const_my_iter_ iter = this->begin();
  while(iter != this->end()) {
    if((*fn)(*iter,x)) ++ret;
    ++iter;
  }
  return ret;
}

INFILE_TEMPL_DECL
typename PSC::size_type
PSC::occurrencesOf(typename PSC::const_value key) const {
  size_type ret = 0;
  const_my_iter_ iter = this->begin();
  while(iter != this->end()) {
    if(deref_equal()(*iter,key)) ++ret;
    ++iter;
  }
  return ret;
}

#undef INFILE_TEMPL_DECL
#undef PSC

