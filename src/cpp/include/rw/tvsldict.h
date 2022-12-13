#ifndef __RWTVSLDICT_H__
#define __RWTVSLDICT_H__
/*
 * RWTValSlistDictionary<K,V>: A dictionary of keys of type K, values of type V,
 *   implemented as a singly-linked list
 *
 * $Id: tvsldict.h 18623 2000-03-03 08:28:13Z sih $
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
 * Stores a *copy* of the inserted items into the collection.
 *
 * Assumes that K and V have:
 *   - well-defined copy constructor (T::T(const T&) or equiv.);
 *   - well-defined assignment operator (T::operator=(const T&) or equiv.);
 *
 * Assumes that K has:
 *   - well-defined equality semantics (T::operator==(const T&) or equiv.);
 *
 * Assumes that V has:
 *   - a default constructor.
 *
 * Note that while these are automatically defined for builtin types
 * (such as "int", "double", or any pointer), you may need to provide
 * appropriate operators for your own classes, particularly those with
 * constructors and/or pointers to other objects.
 *
 ***************************************************************************/

#include "rw/tislist.h"
#include "rw/tasslnk.h"

template <class K, class V> class RWTExport RWTValSlistDictionaryIterator;

/****************************************************************
 *                                                              *
 *      Declarations for RWTValSlistDictionary<K,V>             *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class K, class V>
class RWTExport RWTValSlistDictionary 
                            : private RWTIsvSlist< RWTValAssocLink<K, V> >
{

public:

  RWTValSlistDictionary() {;}
  RWTValSlistDictionary(const RWTValSlistDictionary<K,V>&);
  ~RWTValSlistDictionary()
        {RWTIsvSlist<RWTValAssocLink<K,V> >::clearAndDestroy();}

  // Operators:
  RWTValSlistDictionary<K,V> &
                        operator=(const RWTValSlistDictionary<K,V>&);

  // Look up key, add if not there:
  V&            operator[](const K& key);

  // Member functions:
  void          applyToKeyAndValue(void (*applyFun)(const K&, V& ,void*), void*);

  void          clear()
        {RWTIsvSlist<RWTValAssocLink<K,V> >::clearAndDestroy();}

  RWBoolean     contains(const K&) const;       // Contain key?

  size_t        entries() const
        {return RWTIsvSlist<RWTValAssocLink<K,V> >::entries();}

  RWBoolean     isEmpty() const
        {return RWTIsvSlist<RWTValAssocLink<K,V> >::isEmpty();}

  RWBoolean     find(const K& key, K& retKey) const;
  RWBoolean     findValue(const K& key, V& retVal) const;
  RWBoolean     findKeyAndValue(const K& key, K& retKey, V& retVal) const;

  void          insertKeyAndValue(const K& key, const V& value)
        {(*this)[key] = value;}

  RWBoolean     remove(const K& key);
  RWBoolean     remove(const K& key, K& retKey);

protected:

  RWTValAssocLink<K,V>* findLink(const K&) const;
  RWTValAssocLink<K,V>* removeLink(const K&);

private:

friend class RWTExport RWTValSlistDictionaryIterator<K,V>;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *                                                              *
 *     Declarations for RWTValSlistDictionaryIterator<K,V>      *
 *                                                              *
 ****************************************************************/

template <class K, class V>
class RWTExport RWTValSlistDictionaryIterator :
        private RWTIsvSlistIterator<RWTValAssocLink<K,V> >
{

public:

  // Constructor:
  RWTValSlistDictionaryIterator(RWTValSlistDictionary<K,V>& d) 
        : RWTIsvSlistIterator<RWTValAssocLink<K,V> >(d) {;}

  RWTValSlistDictionaryIterator(const RWTValSlistDictionaryIterator<K,V>&);
     // Copy ctor added to address BUG#117

  // Operators:
  RWBoolean     operator++()
        {this->advance(); return this->cursor()!=container()->tailLink();}
  RWBoolean     operator+=(size_t n)
        {return RWTIsvSlistIterator<RWTValAssocLink<K,V> >::operator+=(n)!=rwnil;}
  RWBoolean     operator()()
        {this->advance(); return this->cursor()!=container()->tailLink();}

  RWTValSlistDictionary<K,V>*   container() const
        {return (RWTValSlistDictionary<K,V>*)RWTIsvSlistIterator<RWTValAssocLink<K,V> >::container();}

  K             key() const     {return this->cursor()->key_;}

  void          reset()
        {RWTIsvSlistIterator<RWTValAssocLink<K,V> >::reset();}
  void          reset(RWTValSlistDictionary<K,V>& s)
        {RWTIsvSlistIterator<RWTValAssocLink<K,V> >::reset(s);}

  V             value() const   {return this->cursor()->value_;}

private:

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvsldict.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvsldict.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWTVSLDICT_H__ */
