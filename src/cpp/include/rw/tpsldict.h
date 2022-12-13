#ifndef __RWTPSLDICT_H__
#define __RWTPSLDICT_H__
/*
 * RWTPtrSlistDictionary<KP,VP>: A dictionary of pointers to keys of type KP,
 *   and pointers to values of type VP, implemented as a singly-linked list.
 *
 * $Id: tpsldict.h 18623 2000-03-03 08:28:13Z sih $
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
 ***************************************************************************
 *
 * Assumes that KP has:
 *   - well-defined equality semantics (equiv of KP::operator==(const KP&)).
 *
 ***************************************************************************/

#include "rw/tislist.h"
#include "rw/tasslnk.h"

template <class KP, class VP> class RWTExport RWTPtrSlistDictionaryIterator;

/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrSlistDictionary<KP,VP>           *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class KP, class VP>
class RWTExport RWTPtrSlistDictionary 
                         : private RWTIsvSlist< RWTPtrAssocLink<KP, VP> >
{

public:

  RWTPtrSlistDictionary() {;}
  RWTPtrSlistDictionary(const RWTPtrSlistDictionary<KP,VP>&);
  ~RWTPtrSlistDictionary()
        {RWTIsvSlist<RWTPtrAssocLink<KP,VP> >::clearAndDestroy();}

  // Operators:
  RWTPtrSlistDictionary<KP,VP>&
                        operator=(const RWTPtrSlistDictionary<KP,VP>&);
  VP*&                  operator[](KP* key); // Look up key, return value

  // Member functions:
  void                  applyToKeyAndValue(void (*applyFun)(KP*,VP*&,void*), void*);

  void                  clear()
        {RWTIsvSlist<RWTPtrAssocLink<KP,VP> >::clearAndDestroy();}

  void                  clearAndDestroy();

  RWBoolean             contains(const KP*) const;      // Contain key?

  size_t                entries() const
        {return RWTIsvSlist<RWTPtrAssocLink<KP,VP> >::entries();}

  RWBoolean             isEmpty() const
        {return RWTIsvSlist<RWTPtrAssocLink<KP,VP> >::isEmpty();}

  KP*                   find(const KP* key) const;
  KP*                   findKeyAndValue(const KP* key, VP*& retVal) const;
  VP*                   findValue(const KP* key) const;

  void                  insertKeyAndValue(KP* key, VP* value)
        { (*this)[key] = value; }

  KP*                   remove(const KP* key);
  KP*                   removeKeyAndValue(const KP* key, VP*& retVal);

protected:

  RWTPtrAssocLink<KP,VP > *     findLink(const KP*) const;
  RWTPtrAssocLink<KP,VP > *     removeLink(const KP*);

private:

  friend class RWTExport RWTPtrSlistDictionaryIterator<KP,VP>;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


/****************************************************************
 *                                                              *
 *     Declarations for RWTPtrSlistDictionaryIterator<KP,VP>    *
 *                                                              *
 ****************************************************************/

template <class KP, class VP>
class RWTExport RWTPtrSlistDictionaryIterator :
        private RWTIsvSlistIterator<RWTPtrAssocLink<KP,VP> >
{

public:

  // Constructor:
  RWTPtrSlistDictionaryIterator(RWTPtrSlistDictionary<KP,VP>& d)
        : RWTIsvSlistIterator<RWTPtrAssocLink<KP,VP> >(d) {;}

  RWTPtrSlistDictionaryIterator(const RWTPtrSlistDictionaryIterator<KP,VP>&);
    // Added to address BUG#117

  // Operators:
  RWBoolean     operator++() { 
    this->advance(); 
    return this->cursor()!=container()->tailLink();
  }

  RWBoolean     operator+=(size_t n)  { 
    return RWTIsvSlistIterator<RWTPtrAssocLink<KP,VP> >::operator+=(n)!=rwnil;
  }

  KP*           operator()()    { 
    this->advance(); 
    return this->cursor()==container()->tailLink() ? 
      rwnil : this->cursor()->key_;
  }

  RWTPtrSlistDictionary<KP,VP>* container() const  { 
    return (RWTPtrSlistDictionary<KP,VP>*)RWTIsvSlistIterator<RWTPtrAssocLink<KP,VP> >::container();
  }

  KP*           key() const
        {return this->cursor()->key_;}

  void          reset()
        {RWTIsvSlistIterator<RWTPtrAssocLink<KP,VP> >::reset();}
  void          reset(RWTPtrSlistDictionary<KP,VP>& s)
        {RWTIsvSlistIterator<RWTPtrAssocLink<KP,VP> >::reset(s);}

  VP*           value() const
        {return this->cursor()->value_;}

private:

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tpsldict.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tpsldict.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWTPSLDICT_H__ */
