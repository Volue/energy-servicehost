#ifndef __RWEPAUX_H__
#define __RWEPAUX_H__

/***************************************************************************
 *
 * epaux.h - helper components for pointer-based STL wrapper classes
 *   
 * $Id: epaux.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************
*/

#include "rw/compiler.h"
#ifndef RW_HEADER_WITH_EXTENSION
# include <iterator>
# include <functional>
# include <utility>
#else
# include <utility.hpp>
# include <iterator.hpp>
# include <functional.hpp>
#endif

#include "rw/defs.h"

#ifdef RWSTD_NO_CONST_INST
#  define RWCONST            /* #undef'd below */
#else
#  define RWCONST const      /* #undef'd below */
#endif

template <class T, class U>
struct rw_deref_pair_equal
: RW_SL_STD(binary_function)<const RW_SL_STD(pair)<T*RWCONST, U*>&,
                  const RW_SL_STD(pair)<T*RWCONST, U*>&, bool>
{
  bool operator()(const RW_SL_STD(pair)<T*RWCONST, U*>& x,
                  const RW_SL_STD(pair)<T*RWCONST, U*>& y) const
  { return *x.first == *y.first && *x.second == *y.second; }
};

template <class T, class U>
struct rw_deref_pair_less
: RW_SL_STD(binary_function)<const RW_SL_STD(pair)<T*RWCONST, U*>&,
                  const RW_SL_STD(pair)<T*RWCONST, U*>&, bool>
{
  bool operator()(const RW_SL_STD(pair)<T*RWCONST, U*>& x,
                  const RW_SL_STD(pair)<T*RWCONST, U*>& y) const
    { return (*x.first < *y.first) || ( !(*y.first < *x.first) &&
                                         (*x.second < *y.second) ) ; }
};

template <class Compare, class T>
struct rw_deref_compare : public RW_SL_STD(binary_function)<T*, T*, bool> {
  Compare cmp_;
#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  rw_deref_compare()  {cmp_ = Compare();} 
#endif // RW_NO_DEFAULT_TEMPLATE_ARGS
  rw_deref_compare(const Compare& c RW_DEFAULT_ARG(Compare())) : cmp_(c) {}
  bool operator()(const T* x, const T* y) const
      { return cmp_(*x,*y); }
};

template <class Hash, class T>
struct rw_deref_hash : public RW_SL_STD(unary_function)<T*, unsigned> {
  Hash hash_;
#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  rw_deref_hash() { hash_=Hash();}
#endif // RW_NO_DEFAULT_TEMPLATE_ARGS
  rw_deref_hash(const Hash& h RW_DEFAULT_ARG(Hash())) : hash_(h) {}
  unsigned operator()(const T* x) const
      { return hash_(*x); }
};

#undef RWCONST

#endif /* __RWEPAUX_H__ */
