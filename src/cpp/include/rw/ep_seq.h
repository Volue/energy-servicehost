#ifndef __RWEP_SEQ_H__
#define __RWEP_SEQ_H__

/***************************************************************************
 *
 * ep_seq.h - RW_PSeq<StdColl,RWColl,DataTypes>
 *   
 *     : pointer-based interface components for STL Sequences
 * 
 * $Id: ep_seq.h 18623 2000-03-03 08:28:13Z sih $
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

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl, class T>
class RWTExport RW_PSeq : public RW_PCntnr<StdColl,RWColl,T>
{
public:
  typedef typename RW_PCntnr<StdColl,RWColl,T>::deref_less  deref_less;
  typedef typename StdColl::value_type                      value_type;
  typedef typename StdColl::size_type                       size_type;
  typedef typename StdColl::iterator                        my_iter_;
  typedef typename StdColl::const_iterator                  const_my_iter_;
  typedef typename StdColl::iterator                        iterator;
  typedef typename StdColl::const_iterator                  const_iterator;
  typedef typename StdColl::reference                       reference;
  typedef typename StdColl::const_reference                 const_reference;
  typedef const T*                                          const_value;


///// public methods
  void  append(value_type item) { this->std().insert(this->end(), item); }

  void  clear() { this->std().erase(this->begin(), this->end()); }

// clearAndDestroy() should be overridden by the leaf class whenever possible
// since this version makes two passes through the container: delete first,
// then erase().
  void          clearAndDestroy();

  value_type    get() { return removeFirst(); }

  bool          insert(value_type item)
    { this->std().insert(this->end(), item); return true; }

  void          insertAt(size_type index, value_type item)
    {
      if(index > 0)
        boundsCheck(index-1);
      
      my_iter_ i = this->begin();
      RW_SL_STD(advance)(i,index);
      this->std().insert(i,item);
    }
  
  reference       minElement()
    { RWPRECONDITION(this->entries() != 0);
      return *RW_SL_STD(min_element)(this->begin(),this->end(),deref_less() ); }

  reference       maxElement()
    { RWPRECONDITION(this->entries() != 0);
      return *RW_SL_STD(max_element)(this->begin(),this->end(),deref_less() ); }

  const_reference minElement() const
    { RWPRECONDITION(this->entries() != 0);
      return *RW_SL_STD(min_element)(this->begin(),this->end(),deref_less() ); }

  const_reference maxElement() const
    { RWPRECONDITION(this->entries() != 0);
      return *RW_SL_STD(max_element)(this->begin(),this->end(),deref_less() ); }

  void  prepend(value_type item) { this->std().insert(this->begin(),item); }

// both removeAll() methods should be overridden by the leaf class whenever
// possible since these versions iterate size()+number_of_matches AND 
// make size()-first_match_location data swaps
  size_type     removeAll(const T* key);

  size_type     removeAll(bool (*fn)(const T*, void*),void*);
  size_type     removeAll(bool (*fn)(T*, void*),void*);

  value_type    removeAt(size_type index)
    {
      boundsCheck(index);
      my_iter_ i = this->begin();
      RW_SL_STD(advance)(i,index);
      value_type ret = *i;
      this->std().erase(i);
      return ret;
    }

  value_type    removeFirst() { value_type ret = *(this->begin());
                                this->std().erase(this->begin()); return ret; }

  value_type    remove(bool (*fn)(const_value, void*),void*);
  value_type    remove(bool (*fn)(value_type, void*),void*);

  value_type    remove(const_value key);

  size_type     replaceAll(const_value oldVal, value_type newVal);
  size_type     replaceAll(bool (*fn)(const_value, void*), void* x,
                           value_type newVal);
  size_type     replaceAll(bool (*fn)(value_type, void*), void* x,
                           value_type newVal);
protected:
  RW_PSeq() {} // prevent direct use
  RW_PSeq(const RW_PSeq<StdColl,RWColl,T>&);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

// moved struct deref_match declaration from the template 
// definition file ep_seq.cc to ep_seq.h for link time template instantiation
template <class PtrToConst>
struct deref_match : public RW_SL_STD(unary_function)<PtrToConst,bool> {
  PtrToConst val;
  deref_match(PtrToConst t) : val(t) {}
  bool operator()(PtrToConst t) {return *t == *val;}
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ep_seq.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ep_seq.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEP_SEQ_H__ */
