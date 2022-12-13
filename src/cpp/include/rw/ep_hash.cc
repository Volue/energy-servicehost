/***************************************************************************
 *
 * ep_hash.cc - 
 * 
 * $Id: ep_hash.cc 18623 2000-03-03 08:28:13Z sih $
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

#ifndef RW_HEADER_WITH_EXTENSION
#  include <algorithm>
#else
#  include <algorithm.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

#define INFILE_TEMPL_DECL   template <class StdColl, class RWColl, class T>
#define PHA        RW_PHAssoc<StdColl,RWColl,T>


INFILE_TEMPL_DECL
void PHA::UnionHash(const StdColl& stdcoll)
{
// depends on all eq items being adjacent in each collection
  typename StdColl::key_equal_type eq = this->std().key_equal();
  const_iterator probe = stdcoll.begin();
  while(probe != stdcoll.end()) {
    const_iterator found = this->std().find(*probe);
    if(found != this->std().end())            // cope with (multiple) match(es)
    {
      const_value match = (const_value)*found;
      while(   found != this->std().end() && probe != stdcoll.end()
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
void PHA::intersectionHash(const StdColl& stdcoll)
{
  // depends on all eq items being adjacent in each collection
  typename StdColl::key_equal_type eq = this->std().key_equal();
  const_iterator look = this->std().begin();
  while(look != this->std().end()) {
    const_iterator probe = stdcoll.find(*look);
    const_value match = (const_value)*look;
    while(   probe != stdcoll.end() && look != this->std().end() 
          && eq(match,*look) && eq(*look,*probe)) 
      // if we have a match, then march in sequence 1 or more times
    {
      ++probe;
      ++look;
    }
    // no match, or no _more_ match: remove (trailing) part in self
    while(look != this->std().end() && eq(match,*look))
      look = this->std().erase(look); // depends on "next iter" semantics
  }
}

INFILE_TEMPL_DECL
void PHA::differenceHash(const StdColl& stdcoll)
{
  // depends on all eq items being adjacent in each collection
  typename StdColl::key_equal_type eq = this->std().key_equal();
  const_iterator look = this->std().begin();
  while(look != this->std().end()) {
    const_iterator probe = stdcoll.find(*look);
    const_value match = (const_value)*look;
    while(   probe != stdcoll.end() && look != this->std().end()
          && eq(match,*look) && eq(*look,*probe))
    { // if there is a match, then erase (them) in sequence
      look = this->std().erase(look); // depends on "next iter" semantics
      ++probe;
    }
    // no match, or no more match: skip (trailing) part in self
    while(look != this->std().end() && eq(match,*look))
      ++look;
  }
}  

INFILE_TEMPL_DECL
void PHA::symmetricDifferenceHash(const StdColl& stdcoll)
{
  // depends on all eq items being adjacent in each collection
  typename StdColl::key_equal_type eq = this->std().key_equal();
  const_iterator probe = stdcoll.begin();
  while(probe != stdcoll.end()) {
    const_iterator found = this->std().find(*probe);
    if(found != this->std().end())            // cope with (multiple) match(es)
    {
      const_value match = (const_value)*found;
      while(   found != this->std().end() && probe != stdcoll.end()
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
      const_value match = (const_value)*probe;
      while(probe != stdcoll.end() && eq(match,*probe)) {
        this->std().insert(*probe++);
      }
    }
  }
}

/*
 * Special function for testing equality of two hash collections.
 * The standard algorithm 'equal' is not suitable due to the
 * inherent un-orderedness of hash collections.
 */
template <class RWColl>
bool hashCollEqual(const RWColl& h1, const RWColl& h2)
{
  RWPRECONDITION(h1.entries() == h2.entries());

  typename RWColl::const_iterator i = h1.begin();
  
  while(i != h1.end()) {
    typename RWColl::size_type num = h1.std().count(*i);
    if (num != h2.std().count(*i)) {
      return false;
    }
    else {
      while(num-- ) ++i;
    }
  }
  return true;
}

#ifdef RW_STD_COLLECTION_PROBLEM
template < class StdColl >
bool StdCollEqual( const StdColl& h1, const StdColl& h2 ) {
   RWPRECONDITION( h1.size() == h2.size() );

   typename StdColl::const_iterator i = h1.begin();
 
   while( i != h1.end() ) {
      typename StdColl::size_type num = h1.count( *i );
      if ( num != h2.count( *i ) ) {
         return false;
      }
      else {
         while( num-- ) ++i;
      }
   }
   return true;
}
#endif

#undef INFILE_TEMPL_DECL
#undef PHA
