#ifndef __RWTPORDVEC_H__
#define __RWTPORDVEC_H__

/***************************************************************************
 *
 * tpordvec.h - RWTPtrOrderedVector<T>
 *   
 *     : pointer-based ordered vector wrapper for STL vector
 * 
 * $Id: tpordvec.h 18623 2000-03-03 08:28:13Z sih $
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
#include "rw/defs.h"

#ifdef RW_NO_STL
#  include "rw/xpordvec.h"
#else
// STL-based implementation:

#ifndef RW_HEADER_WITH_EXTENSION
# include <vector>
#else
# include <vector.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

#include "rw/ep_seq.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T>
class RWTExport RWTPtrOrderedVector :
#ifndef RW_ALLOCATOR_PLAIN
             private RW_PSeq<RW_SL_STD(vector)<T*,RW_SL_STD(allocator)<T*> >, RWTPtrOrderedVector<T>, T>
#else
             private RW_PSeq<RW_SL_STD(vector)<T*,RW_SL_STD(allocator)>, RWTPtrOrderedVector<T>, T>
#endif   
{
public:
// typedefs
#ifndef RW_ALLOCATOR_PLAIN
	typedef RW_SL_STD(vector)<T*,RW_SL_STD(allocator)<T*> >                  container_type;
#else
  typedef RW_SL_STD(vector)<T*,RW_SL_STD(allocator)>  container_type;
#endif
  typedef RWTPtrOrderedVector<T>                     this_type;
  typedef RW_PSeq<container_type,
                  RWTPtrOrderedVector<T>, T>         base1_type;

  typedef typename container_type::iterator          iterator;
  typedef typename container_type::const_iterator    const_iterator;
  typedef typename container_type::size_type         size_type;
  typedef typename container_type::difference_type   difference_type;
  typedef typename container_type::value_type        value_type;
  typedef typename container_type::reference         reference;
  typedef typename container_type::const_reference   const_reference;

// STL-level access
  const container_type&  std() const { return impl_; }
  container_type&        std()       { return impl_; }

#ifndef RW_BROKEN_ACCESS_ADJUST
  typedef base1_type                                  base2_type;
  typedef base1_type                                  base3_type;
#else
  //  inherited interface components cannot always follow base_type
  typedef RW_PCntnr<container_type,this_type,T>       base2_type;
  typedef RW_PBase<container_type,this_type>          base3_type;
#endif  
 

  base1_type::append;
  base2_type::apply;
  base3_type::at;
  base3_type::begin;
  base1_type::clear;
  base1_type::clearAndDestroy;
  base2_type::contains;
  base3_type::end;
  base3_type::entries;
  base2_type::find;
  base3_type::first;
  base2_type::index;
  base1_type::insert;
  base1_type::insertAt;
  base3_type::isEmpty;
  base1_type::minElement;
  base1_type::maxElement;
  base3_type::npos;
  base2_type::occurrencesOf;
  base3_type::operator();
  base2_type::operator<;
  base2_type::operator==;
  base3_type::operator[];
  base1_type::prepend;
  base1_type::remove;
  base1_type::removeAll;
  base1_type::removeAt;
  base1_type::removeFirst;
  base1_type::replaceAll;


// additional interface components
  T* const*     data() const       { return &(T* const &)(*(this->begin())); }
  T*&           last()             { return impl_.back(); }     
  T*const&      last() const       { return impl_.back(); }     
  size_type     length() const     { return impl_.size(); }
  void          resize(size_t n)   { impl_.reserve(n); }
  T*            removeLast()       { T* ret = last();
                                     impl_.pop_back();
                                     return ret;
                                    }
  void          sort()
    { RW_SL_STD_GLOBAL(sort)(this->begin(),this->end(),rw_deref_compare<RW_SL_STD(less)<T>,T>()); }

// assignment operators
  RWTPtrOrderedVector<T>&   operator=(const RWTPtrOrderedVector<T>& ovec)
    { impl_ = ovec.impl_; return *this; }
  RWTPtrOrderedVector<T>&   operator=(const container_type& stdv)
    { impl_ = stdv; return *this; }

// constructors
  RWTPtrOrderedVector()                                : impl_() {}
  RWTPtrOrderedVector(const container_type& lst)       : impl_(lst) {}
  RWTPtrOrderedVector(const this_type& lst)            : impl_(lst.impl_) {}
  RWTPtrOrderedVector(size_type n, T* p)               : impl_(n, p) {}
  RWTPtrOrderedVector(T*const* first, T*const* last)   
#ifdef RW_STLCTOR_PTR_HACK
    : impl_(first,last) {}
#else
   { while(first != last) {impl_.push_back(*first); ++first; } }
#endif
  // For Tools-6 compatibility (not particularly efficient):
  RWTPtrOrderedVector(size_type n) { resize(n); }

//  ~RWTPtrOrderedVector(); // handled by compiler-generated destructor

private:
  container_type impl_;         

  friend class RW_PBase<container_type, this_type>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

RWDECLARE_PERSISTABLE_TEMPLATE(RWTPtrOrderedVector)

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tpordvec.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tpordvec.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* RW_NO_STL */

#endif /* __RWTPORDVEC_H__ */
