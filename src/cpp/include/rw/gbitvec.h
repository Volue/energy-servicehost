#ifndef __RWGBITVEC_H__
#define __RWGBITVEC_H__

/*
 * Generic bit vector.
 *
 * $Id: gbitvec.h 18623 2000-03-03 08:28:13Z sih $
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
 * To use this class you must declare and implement a bit vector of the length
 * you want.
 *
 * Example: To declare and implement a bit vector of length 22:
 *
 *   #include <rw/gbitvec.h>
 *
 *   declare(RWGBitVec,22)
 *
 *   // In one and only one .cpp file you must put the following:
 *   implement(RWGBitVec,22)
 *
 *   // To use:
 *   void func(){
 *     RWGBitVec(22) bv;
 *     bv(0) = TRUE;
 *     bv(3) = FALSE;   // etc.
 *   }
 *
 ***************************************************************************
 */

#include "rw/bitref.h"
#include "rw/generic.h"
STARTWRAP
#include "rw/rwassert.h"
#ifdef RW_NON_ANSI_HEADERS
#  include <memory.h>           /* Looking for memcpy() */
#else
#  include <string.h>
#endif
ENDWRAP

#define RWGBitVec(sz) name2(RWGBitVec,sz)

#if !defined(RW_NO_STL) && defined(RW_NO_RELOPS_NAMESPACE)

//#2437  C++ Standard operators >=, <=, >, and !=
#ifndef RW_HEADER_WITH_EXTENSION        // djk
#  include<utility>
#else
#  include<utility.hpp>
#endif /* RW_HEADER_WITH_EXTENSION */

#define RWGBitVecdeclare(sz)                                            \
class RWGExport RWGBitVec(sz) {                                         \
  RWByte        vec_[ (sz+7) >> 3 ];                                    \
public:                                                                 \
  RWGBitVec(sz)() { memset(vec_,0,sizeof(vec_)); }                      \
  RWGBitVec(sz)(RWBoolean f) { memset(vec_, f ? ~0 : 0,sizeof(vec_)); } \
                                                                        \
  RWGBitVec(sz)&                operator=(const RWGBitVec(sz)& v)       \
    { memcpy(vec_,v.vec_,sizeof(vec_)); return *this; }                 \
  RWGBitVec(sz)&                operator=(RWBoolean f)                  \
    { memset(vec_,f ? ~0 : 0,sizeof(vec_)); return *this; }             \
  RWGBitVec(sz)&                operator&=(const RWGBitVec(sz)& v)      \
    { RWand(vec_,v.vec_,sizeof(vec_)); return *this; }                  \
  RWGBitVec(sz)&                operator^=(const RWGBitVec(sz)& v)      \
    { RWxor(vec_,v.vec_,sizeof(vec_)); return *this; }                  \
  RWGBitVec(sz)&                operator|=(const RWGBitVec(sz)& v)      \
    { RWor (vec_,v.vec_,sizeof(vec_)); return *this; }                  \
                                                                        \
  RWBitRef              operator[](size_t i)                            \
    { assert(i<sz); return RWBitRef(vec_,i); }                          \
  RWBitRef              operator()(size_t i)                            \
    { return RWBitRef(vec_,i); }                                        \
  friend RWBoolean      operator==(const RWGBitVec(sz)& v1,const RWGBitVec(sz)& v2);\
                                                                        \
  RW_INLINE_FRIEND RWGBitVec(sz) operator&(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2);\
  RW_INLINE_FRIEND RWGBitVec(sz) operator^(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2);\
  RW_INLINE_FRIEND RWGBitVec(sz) operator|(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2);\
                                                                        \
  void                  clearBit(size_t i)                              \
    { vec_[i>>3] &= ~(1 << (7&i)); }                                    \
  const RWByte*         data() const                                    \
    {return vec_;}                                                      \
  void                  setBit(size_t i)                                \
    { vec_[i>>3] |=  (1 << (7&i)); }                                    \
  RWBoolean             testBit(size_t i) const                         \
    { return (  vec_[i>>3] & ( 1 << (7&i) )  ) != 0; }                  \
};                                                                      \
inline RWGBitVec(sz)    operator&(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2)\
    { RWGBitVec(sz) temp(v1); temp &= v2; return temp; }                \
inline RWGBitVec(sz)    operator^(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2)\
    { RWGBitVec(sz) temp(v1); temp ^= v2; return temp; }                \
inline RWGBitVec(sz)    operator|(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2)\
    { RWGBitVec(sz) temp(v1); temp |= v2; return temp; }

#else //defined(RW_NO_STL) || !defined(RW_NO_RELOPS_NAMESPACE)

#define RWGBitVecdeclare(sz)                                            \
class RWGExport RWGBitVec(sz) {                                         \
  RWByte        vec_[ (sz+7) >> 3 ];                                    \
public:                                                                 \
  RWGBitVec(sz)() { memset(vec_,0,sizeof(vec_)); }                      \
  RWGBitVec(sz)(RWBoolean f) { memset(vec_, f ? ~0 : 0,sizeof(vec_)); } \
                                                                        \
  RWGBitVec(sz)&                operator=(const RWGBitVec(sz)& v)       \
    { memcpy(vec_,v.vec_,sizeof(vec_)); return *this; }                 \
  RWGBitVec(sz)&                operator=(RWBoolean f)                  \
    { memset(vec_,f ? ~0 : 0,sizeof(vec_)); return *this; }             \
  RWGBitVec(sz)&                operator&=(const RWGBitVec(sz)& v)      \
    { RWand(vec_,v.vec_,sizeof(vec_)); return *this; }                  \
  RWGBitVec(sz)&                operator^=(const RWGBitVec(sz)& v)      \
    { RWxor(vec_,v.vec_,sizeof(vec_)); return *this; }                  \
  RWGBitVec(sz)&                operator|=(const RWGBitVec(sz)& v)      \
    { RWor (vec_,v.vec_,sizeof(vec_)); return *this; }                  \
                                                                        \
  RWBitRef              operator[](size_t i)                            \
    { assert(i<sz); return RWBitRef(vec_,i); }                          \
  RWBitRef              operator()(size_t i)                            \
    { return RWBitRef(vec_,i); }                                        \
  friend RWBoolean      operator==(const RWGBitVec(sz)& v1,const RWGBitVec(sz)& v2);\
  friend RWBoolean      operator!=(const RWGBitVec(sz)& v1,const RWGBitVec(sz)& v2)\
    { return !(v1==v2); }                                               \
                                                                        \
  RW_INLINE_FRIEND RWGBitVec(sz) operator&(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2);   \
  RW_INLINE_FRIEND RWGBitVec(sz) operator^(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2);   \
  RW_INLINE_FRIEND RWGBitVec(sz) operator|(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2);   \
                                                                        \
  void                  clearBit(size_t i)                              \
    { vec_[i>>3] &= ~(1 << (7&i)); }                                    \
  const RWByte*         data() const                                    \
    {return vec_;}                                                      \
  void                  setBit(size_t i)                                \
    { vec_[i>>3] |=  (1 << (7&i)); }                                    \
  RWBoolean             testBit(size_t i) const                         \
    { return (  vec_[i>>3] & ( 1 << (7&i) )  ) != 0; }                  \
};                                                                      \
inline RWGBitVec(sz)    operator&(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2)     \
    { RWGBitVec(sz) temp(v1); temp &= v2; return temp; }                                \
inline RWGBitVec(sz)    operator^(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2)     \
    { RWGBitVec(sz) temp(v1); temp ^= v2; return temp; }                                \
inline RWGBitVec(sz)    operator|(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2)     \
    { RWGBitVec(sz) temp(v1); temp |= v2; return temp; }

#endif //if RW_NO_STL


#define RWGBitVecimplement(sz)                                          \
RWBoolean operator==(const RWGBitVec(sz)& v1, const RWGBitVec(sz)& v2)  \
{                                                                       \
  size_t nfull = sz>>3;                                                 \
  if (memcmp(v1.vec_, v2.vec_, nfull)) return FALSE;                    \
  if (nfull==sizeof(v1.vec_)) return TRUE;                              \
  RWByte mask = (1 << ((size_t)sz&7)) - 1;                              \
  return (v1.vec_[nfull] & mask) == (v2.vec_[nfull] & mask);            \
}

#endif /* __RWGBITVEC_H__ */

