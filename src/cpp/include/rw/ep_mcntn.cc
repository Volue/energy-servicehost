/***************************************************************************
 *
 * ep_mcntn.cc - template definitions for RW_PMapCntnr<StdColl,RWColl,Key,T>
 *
 * $Id: ep_mcntn.cc 18623 2000-03-03 08:28:13Z sih $
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
#define PMC      RW_PMapCntnr<StdColl, RWColl, Key, T>

INFILE_TEMPL_DECL typename PMC::value_type
PMC::find(typename PMC::const_reference key) const
{
    const_iterator iter = this->begin();
    while(iter != this->end()) {
      if(deref_equal()(*iter,key)) break;
      ++iter;
    }
    if (iter == this->end()) return nilValue(); else return *iter;
}

INFILE_TEMPL_DECL typename PMC::value_type
PMC::find(bool (*fn)(typename PMC::value_type, void*), void* x) const
{
    const_iterator iter = this->begin();
    while(iter != this->end()) {
      if((*fn)(*iter,x)) break;
      ++iter;
    }
    if (iter == this->end()) return nilValue(); else return *iter;
}

#if !defined(RW_NO_INHERIT_TEMPL_TYPEDEF)
INFILE_TEMPL_DECL typename PMC::size_type
PMC::index(const_reference key) const
{
  const_iterator i = this->begin();
  size_type idx = 0;
  for(/**/; i != this->end(); ++i) {
    if(deref_equal()(*i,key))
      return idx;
    ++idx;
  }
  return this->npos;
}
#endif /* RW_NO_INHERIT_TEMPL_TYPEDEF */

#undef INFILE_TEMPL_DECL
#undef PMC

