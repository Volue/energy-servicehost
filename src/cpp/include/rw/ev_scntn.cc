/***************************************************************************
 *
 * ev_scntn.cc - template definitions for RW_VCntnr<StdColl,RWColl>
 *
 * $Id: ev_scntn.cc 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/rwfile.h"
#include "rw/vstream.h"

#define INFILE_TEMPL_DECL template <class StdColl, class RWColl>
#define VSC      RW_VCntnr<StdColl, RWColl>

INFILE_TEMPL_DECL bool
VSC::find(bool (*fn)(typename VSC::const_reference,void*), void* x, typename VSC::value_type& val) const
{
  const_my_iter_ i = this->begin();
  for(/**/ ; i != this->end(); ++i)
    if((*fn)(*i,x))
      break;
  if(i != this->end()) {
    val = *i;
    return true;
  }
  return false;
}

INFILE_TEMPL_DECL bool
VSC::find(const_reference key, typename VSC::value_type& val) const
{
  const_my_iter_ i = this->begin();
  for(/**/ ; i != this->end(); ++i)
    if(*i == key)
      break;
  if(i != this->end()) {
    val = *i;
    return true;
  }
  return false;
}

#undef INFILE_TEMPL_DECL
#undef VSC
