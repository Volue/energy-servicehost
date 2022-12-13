#ifndef __RWBITVEC_H__
#define __RWBITVEC_H__

/*
 * Ar`bit'rarily long bit vector.
 *
 * $Id: bitvec.h 18623 2000-03-03 08:28:13Z sih $
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
 */

/*
 * Defining the preprocessor directive RWBOUNDS_CHECK will
 * cause bounds checking on the subscripting operator.
 */

#include "rw/bitref.h"
#include "rw/rstream.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWBitVec RWMemoryPool_OPTION
{

public:

  RWBitVec() : vec_(rwnil), npts_(0) {;}
  RWBitVec(size_t N);
  RWBitVec(size_t N, RWBoolean initVal);
  RWBitVec(const RWByte*, size_t N);
  RWBitVec(const RWBitVec&);
  ~RWBitVec();

  // Assignment operators
  RWBitVec&             operator=(const RWBitVec&);
  RWBitVec&             operator=(RWBoolean);
  RWBitVec&             operator&=(const RWBitVec& V)
        {RWand(vec_, V.vec_, nbytes()); return *this;}
  RWBitVec&             operator^=(const RWBitVec& V)
        {RWxor(vec_, V.vec_, nbytes()); return *this;}
  RWBitVec&             operator|=(const RWBitVec& V)
        {RWor(vec_, V.vec_, nbytes()); return *this;}

  // Indexing operators
  RWBitRef              operator[](size_t i);   // Can be used as lvalue
  RWBitRef              operator()(size_t i);   //       ditto
#ifndef RW_NO_CONST_OVERLOAD
  RWBoolean             operator[](size_t i) const; // Can't be used as lvalue
  RWBoolean             operator()(size_t i) const; //       ditto
#endif

  // Other operators ---
  RWBoolean             operator==(const RWBitVec& u) const {return  isEqual(u);}
  RWBoolean             operator!=(const RWBitVec& u) const {return !isEqual(u);}
  RWBoolean             operator==(RWBoolean b) const;
  RWBoolean             operator!=(RWBoolean b) const {return !operator==(b);}

#ifndef RW_TRAILING_RWEXPORT
  friend rwexport RWBitVec      operator!(const RWBitVec&);
  friend rwexport RWBitVec      operator&(const RWBitVec&,const RWBitVec&);
  friend rwexport RWBitVec      operator^(const RWBitVec&,const RWBitVec&);
  friend rwexport RWBitVec      operator|(const RWBitVec&,const RWBitVec&);
  friend rwexport RW_SL_IO_STD(ostream)&      operator<<(RW_SL_IO_STD(ostream)&, const RWBitVec&);
  friend rwexport RW_SL_IO_STD(istream)&      operator>>(RW_SL_IO_STD(istream)&, RWBitVec&);
#else
  friend RWBitVec rwexport      operator!(const RWBitVec&);
  friend RWBitVec rwexport      operator&(const RWBitVec&,const RWBitVec&);
  friend RWBitVec rwexport      operator^(const RWBitVec&,const RWBitVec&);
  friend RWBitVec rwexport      operator|(const RWBitVec&,const RWBitVec&);
  friend RW_SL_IO_STD(ostream)& rwexport      operator<<(RW_SL_IO_STD(ostream)&, const RWBitVec&);
  friend RW_SL_IO_STD(istream)& rwexport      operator>>(RW_SL_IO_STD(istream)&, RWBitVec&);
#endif

  // Member functions
  void                  clearBit(size_t i);             // Clear bit i
  const RWByte*         data() const    { return vec_; }
  size_t                firstFalse() const;             // Find first OFF bit
  size_t                firstTrue() const;              // Find first ON  bit
  unsigned              hash() const;
  RWBoolean             isEqual(const RWBitVec&) const;
  size_t                length() const  {return npts_;}
  RW_SL_IO_STD(ostream)&              printOn(RW_SL_IO_STD(ostream)&) const;
  void                  restoreFrom(RWvistream&);
  void                  restoreFrom(RWFile&);
  void                  resize(size_t);
  void                  saveOn(RWvostream&) const;
  void                  saveOn(RWFile&) const;
  RW_SL_IO_STD(istream)&              scanFrom(RW_SL_IO_STD(istream)&);
  void                  setBit(size_t i);                       // Set bit i
  RWBoolean             testBit(size_t i) const;                // Return value of bit i

  // Other friend functions:
  friend size_t rwexport        sum(const RWBitVec&);           // Total # of bits set

private:

  void                  indexRangeErr(size_t) const;
  void                  lengthErr(size_t, size_t) const;
  size_t                nbytes() const  { return (npts_+7) >> 3; }
  size_t                nfull()  const  { return npts_ >> 3; }

  RWByte*               vec_;           // The bit data, packed within a byte vector
  size_t                npts_;          // Length of vector in bits
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

inline RWvistream& operator>>(RWvistream& str,       RWBitVec& vec)
  { vec.restoreFrom(str);  return str; }
inline RWFile&     operator>>(RWFile& file,          RWBitVec& vec)
  { vec.restoreFrom(file); return file; }
inline RWvistream& operator>>(RWvistream& str,       RWBitVec*& vec)
  { vec = new RWBitVec(1); vec->restoreFrom(str);  return str; }
inline RWFile&     operator>>(RWFile& file,          RWBitVec*& vec)
  { vec = new RWBitVec(1); vec->restoreFrom(file); return file; }
inline RWvostream& operator<<(RWvostream& str, const RWBitVec& vec)
  { vec.saveOn(str);       return str; }
inline RWFile&     operator<<(RWFile& file,    const RWBitVec& vec)
  { vec.saveOn(file);      return file; }

inline size_t RWBitVec::firstFalse() const
  { return rwFirstFalse(vec_, npts_); }
inline size_t RWBitVec::firstTrue() const
  { return rwFirstTrue(vec_, npts_); }

// This macro isolates bit 'i'
#define RWBIT(i) (*(((i)>>3) + (vec_)) & (1<<(7&(i))))

inline RWBitRef
RWBitVec::operator[](size_t i)
{
  if (i >= npts_) indexRangeErr(i);
  return RWBitRef(vec_,i);
}
        
inline RWBitRef
RWBitVec::operator()(size_t i)
{
#ifdef RWBOUNDS_CHECK
  if (i >= npts_) indexRangeErr(i);
#endif
  return RWBitRef(vec_,i);
}

#ifndef RW_NO_CONST_OVERLOAD
        
inline RWBoolean
RWBitVec::operator[](size_t i) const
{
  if (i >= npts_) indexRangeErr(i);
  return RWBIT(i) != 0;
}
        
inline RWBoolean
RWBitVec::operator()(size_t i) const
{
#ifdef RWBOUNDS_CHECK
  if (i >= npts_) indexRangeErr(i);
#endif
  return RWBIT(i) != 0;
}

#endif  /* RW_NO_CONST_OVERLOAD */

inline void
RWBitVec::clearBit(size_t i)
{ (*(vec_ + (i>>3))) &= ~(1<<(7&i)); }

inline void
RWBitVec::setBit(size_t i)
{ (*(vec_ + (i>>3))) |= (1<<(7&i)); }

inline RWBoolean
RWBitVec::testBit(size_t i) const
{ return RWBIT(i) != 0; }

#endif /* __RWBITVEC_H__ */

