#ifndef __RWTBITVEC_H__
#define __RWTBITVEC_H__

/*
 * RWTBitVec<N>: Parameterized bit vector of length N
 *
 * $Id: tbitvec.h 18623 2000-03-03 08:28:13Z sih $
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
 *************************************************************************/

#include "rw/bitref.h"

STARTWRAP
#include "rw/rwassert.h"
#ifdef RW_NON_ANSI_HEADERS
#  include <memory.h>           /* Looking for memcpy() */
#else
#  include <string.h>
#endif
ENDWRAP

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <int N> class RWTExport RWTBitVec
{

public:

  RWTBitVec()           { memset(vec_, 0, sizeof(vec_)); }
  RWTBitVec(RWBoolean f){ memset(vec_, f ? ~0 : 0, sizeof(vec_)); }

  ~RWTBitVec() {}

  RWTBitVec<N>&         operator=(const RWTBitVec<N>& v)
    { memcpy(vec_,v.vec_,sizeof(vec_)); return *this; }
  RWTBitVec<N>&         operator=(RWBoolean f)
    { memset(vec_,f ? ~0 : 0,sizeof(vec_)); return *this; }
  RWTBitVec<N>&         operator&=(const RWTBitVec<N>& v)
    { RWand(vec_,v.vec_,sizeof(vec_)); return *this; }
  RWTBitVec<N>&         operator^=(const RWTBitVec<N>& v)
    { RWxor(vec_,v.vec_,sizeof(vec_)); return *this; }
  RWTBitVec<N>&         operator|=(const RWTBitVec<N>& v)
    { RWor (vec_,v.vec_,sizeof(vec_)); return *this; }

  RWBitRef              operator[](size_t i)
    { assert(i<(size_t)N); return RWBitRef(vec_,i); }
  RWBitRef              operator()(size_t i)
    { return RWBitRef(vec_,i); }
  RWBoolean             operator==(RWBoolean b) const;
  RWBoolean             operator!=(RWBoolean b) const
    { return !operator==(b); }
  RWBoolean             operator==(const RWTBitVec<N>& v) const;
  RWBoolean             operator< (const RWTBitVec<N>& v) const;
  RWBoolean             operator!=(const RWTBitVec<N>& v) const
    { return !(operator==(v)); }

   RWTBitVec<N> operator&(const RWTBitVec<N>& v2)
    { RWTBitVec<N> temp(*this); temp &= v2; return temp; }
   RWTBitVec<N> operator^(const RWTBitVec<N>& v2)
    { RWTBitVec<N> temp(*this); temp ^= v2; return temp; }
   RWTBitVec<N> operator|(const RWTBitVec<N>& v2)
    { RWTBitVec<N> temp(*this); temp |= v2; return temp; }

  void                  clearBit(size_t i)
    { vec_[i>>3] &= ~(1 << (7&i)); }
  const RWByte*         data() const
    {return vec_;}
  void                  setBit(size_t i)
    { vec_[i>>3] |=  (1 << (7&i)); }
  RWBoolean             testBit(size_t i) const
    { return (  vec_[i>>3] & ( 1 << (7&i) )  ) != 0; }

  size_t                firstTrue() const
    { return rwFirstTrue(vec_, (size_t)N); }
  size_t                firstFalse() const
    { return rwFirstFalse(vec_, (size_t)N); }
  
private:

  RWByte                vec_[ (N+7) >> 3 ];

#if !defined(RW_BROKEN_TEMPLATES) && !defined(__GNUC__)
  static const size_t   nfull_;
  static const RWByte   mask_;
#endif

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tbitvec.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tbitvec.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWTBITVEC_H__ */
