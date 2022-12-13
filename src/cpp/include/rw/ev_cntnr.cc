/***************************************************************************
 *
 * ev_cntnr.cc - template definitions for RW_VBase<StdColl,RWColl>
 * 
 * $Id: ev_cntnr.cc 18623 2000-03-03 08:28:13Z sih $
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
 * 
 ****************************************************************************/

#define INFILE_TEMPL_DECL template <class StdColl, class RWColl>
#define VC       RW_VBase<StdColl, RWColl>

#if !defined(__SUNPRO_CC) || (__SUNPRO_CC<0x420 && !defined(RW_NO_STL))
INFILE_TEMPL_DECL
void VC::apply(
/*constApplyFunction*/  void (*fn)(typename VC::const_reference,void*),
                        void* x
                       ) const
{
  for(const_my_iter_ i= begin(); i != end(); ++i) {
    (*fn)(*i,x);
  }
}

INFILE_TEMPL_DECL
void VC::apply(
/*mutativeApplyFunction*/  void (*fn)(typename VC::reference,void*),
                        void* x
                       ) 
{
  for(my_iter_ i= begin(); i != end(); ++i) {
    (*fn)(*i,x);
  }
}

INFILE_TEMPL_DECL
bool VC::contains(
/*testFunction*/     bool (*fn)(typename VC::const_reference,void*),
                     void* x
                    ) const
{
  const_my_iter_ i = begin();
  for(/**/ ; i != end(); ++i)
    if((*fn)(*i,x))
      break;
  return i != end();
}

INFILE_TEMPL_DECL
bool VC::contains(const_reference key) const
{
  const_my_iter_ i = begin();
  for(/**/ ; i != end(); ++i)
    if(*i == key)
      break;
  return i != end();
}


INFILE_TEMPL_DECL
typename VC::size_type
VC::occurrencesOf(
                  bool (*fn)(typename VC::const_reference,void*),
                  void* x
                 ) const
{
  size_type ret(0);
  const_my_iter_ i = begin();
  for(/**/; i != end(); ++i)
    if((*fn)(*i,x))
      ++ret;
  return ret;
}

INFILE_TEMPL_DECL
typename VC::size_type
VC::occurrencesOf(const_reference key) const
{
  size_type ret(0);
  const_my_iter_ i = begin();
  for(/**/; i != end(); ++i)
    if(*i == key)
      ++ret;
  return ret;
}

INFILE_TEMPL_DECL
typename VC::size_type
VC::index(bool (*fn)(typename VC::const_reference, void*), void* x) const
{
  const_my_iter_ i = begin();
  size_type idx(0);
  for(/**/; i != end(); ++i) {
    if((*fn)(*i,x))
      return idx;
    ++idx;
  }
  return this->npos;
}

INFILE_TEMPL_DECL
typename VC::size_type
VC::index(const_reference key) const
{
  size_type ret = 0;
  const_my_iter_ i;
  for(i = begin(); i != end(); ++i)
  {
    if(*i == key) break;
    ++ret;
  }
  return i == this->end() ? this->npos : ret;
}
#endif /* !SunPro */
INFILE_TEMPL_DECL
typename VC::size_type VC::npos = ~(typename VC::size_type)0;

#undef INFILE_TEMPL_DECL
#undef VC
