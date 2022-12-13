/***************************************************************************
 *
 * ev_inmap.cc
 *      - template definitions for RW_VMapAssoc<StdColl,RWColl,Key,T>
 * 
 * $Id: ev_inmap.cc 18623 2000-03-03 08:28:13Z sih $
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


#define INFILE_TEMPL_DECL \
          template <class StdColl, class RWColl, class Key, class T>
#define VMA      RW_VMapAssoc<StdColl, RWColl, Key, T>

// handy macros, #undef'd below
#define KEY(i)    ((*i).first)
#define DATA(i)   ((*i).second)

INFILE_TEMPL_DECL void
VMA::apply(void (*fn)(const Key&, T&, void*), void* x)
{
  for(my_iter_ i = this->begin(); i != this->end(); ++i)
    (*fn)(KEY(i), DATA(i), x);
}

INFILE_TEMPL_DECL void
VMA::apply(void (*fn)(const Key&, const T&, void*),void*x) const
{
  for(const_my_iter_ i = this->begin(); i != this->end(); ++i)
    (*fn)(KEY(i), DATA(i), x);
}

INFILE_TEMPL_DECL bool
VMA::remove(bool (*fn)(typename VMA::const_reference,void*),void* x)
{
  my_iter_ hit = this->begin();
  for(/**/; hit != this->end(); ++hit) {
    if((*fn)(*hit,x)) {
      this->std().erase(hit);
      return true;
    }
  }
  return false;
}

INFILE_TEMPL_DECL typename VMA::size_type
VMA::removeAll(bool (*fn)(typename VMA::const_reference, void*), void* x)
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

#undef KEY
#undef DATA

#undef INFILE_TEMPL_DECL
#undef VMA
