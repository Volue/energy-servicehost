/***************************************************************************
 *
 * ev_hash.cc - 
 * 
 * $Id: ev_hash.cc 18623 2000-03-03 08:28:13Z sih $
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

#define INFILE_TEMPL_DECL template <class StdColl, class RWColl>
#define VHA      RW_VHAssoc<StdColl, RWColl>

INFILE_TEMPL_DECL
void VHA::UnionHash(const StdColl& stdcoll)
{
// depends on all eq items being adjacent in each collection
  typename StdColl::key_equal_type eq = this->std().key_equal();
  typename StdColl::const_iterator probe = stdcoll.begin();
  while(probe != stdcoll.end()) {
    typename StdColl::iterator found = this->std().find(*probe);
    if(found != this->end())          // cope with (multiple) match(es)
    {
      const typename StdColl::value_type& match = *found;
      while(   found != this->end() && probe != stdcoll.end()
            && eq(match,*found) && eq(*found,*probe))
        // if we have a match, then march in sequence 1 or more times
      {
        ++found;
        ++probe;
      }
      // if no match, or no _more_ match; insert (remainder) into self
      while(probe != stdcoll.end() && eq(match,*probe))
      {
        this->std().insert(*probe++);
      }
    }
    else                        // self has no match to *probe
    {
      const typename StdColl::value_type& match = *probe;
      while(probe != stdcoll.end() && eq(match,*probe)) {
        this->std().insert(*probe++);
      }
    }
  }
}

INFILE_TEMPL_DECL
void VHA::intersectionHash(const StdColl& stdcoll)
{
  // depends on all eq items being adjacent in each collection
  typename StdColl::key_equal_type eq = this->std().key_equal();
  typename StdColl::const_iterator look = this->begin();
  while(look != this->end()) {
    typename StdColl::const_iterator probe = stdcoll.find(*look);
    const typename StdColl::value_type& match = *look;
    while(   probe != stdcoll.end() && look != this->end() 
          && eq(match,*look) && eq(*look,*probe)) 
      // if we have a match, then march in sequence 1 or more times
    {
      ++probe;
      ++look;
    }
    // no match, or no _more_ match: remove (trailing) part in self
    while(look != this->end() && eq(match,*look))
      look = this->std().erase(look); // depends on "next iter" semantics
  }
}

INFILE_TEMPL_DECL
void VHA::differenceHash(const StdColl& stdcoll)
{
  // depends on all eq items being adjacent in each collection
  typename StdColl::key_equal_type eq = this->std().key_equal();
  typename StdColl::const_iterator look = this->begin();
  while(look != this->end()) {
    typename StdColl::const_iterator probe = stdcoll.find(*look);
    const typename StdColl::value_type& match = *look;
    while(   probe != stdcoll.end() && look != this->end()
          && eq(match,*look) && eq(*look,*probe))
    { // if there is a match, then erase (them) in sequence
      look = this->std().erase(look); // depends on "next iter" semantics
      ++probe;
    }
    // no match, or no more match: skip (trailing) part in self
    while(look != this->end() && eq(match,*look))
      ++look;
  }
}  

INFILE_TEMPL_DECL
void VHA::symmetricDifferenceHash(const StdColl& stdcoll)
{
  // depends on all eq items being adjacent in each collection
  typename StdColl::key_equal_type eq = this->std().key_equal();
  typename StdColl::const_iterator probe = stdcoll.begin();
  while(probe != stdcoll.end()) {
    typename StdColl::iterator found = this->std().find(*probe);
    if(found != this->end())          // cope with (multiple) match(es)
    {
      const typename StdColl::value_type& match = *found;
      while(   found != this->end() && probe != stdcoll.end()
            && eq(match,*found) && eq(*found,*probe))
        // if we have a match, then remove in sequence 1 or more times
      {
        found = this->std().erase(found);
        ++probe;
      }
      // if no match, or no _more_ match; insert (remainder) into self
      while(probe != stdcoll.end() && eq(match,*probe))
      {
        this->std().insert(*probe++); 
      }
    }
    else                        // self has no match to *probe 
    {
      const typename StdColl::value_type& match = *probe;
      while(probe != stdcoll.end() && eq(match,*probe)) {
        this->std().insert(*probe++);
      }
    }
  }
}

#undef INFILE_TEMPL_DECL
#undef VHA
