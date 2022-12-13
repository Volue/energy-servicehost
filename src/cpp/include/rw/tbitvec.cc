/*
 * Template definitions for RWTBitVec<N>
 *
 * $Id: tbitvec.cc 18623 2000-03-03 08:28:13Z sih $
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
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.5  1996/02/18 01:46:53  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.4  1995/12/15 10:33:36  jims
 * Add definition for operator<(const RWTBitVec<N>&) const
 *
 * Revision 7.3  1995/09/19 10:48:33  jims
 * Don't try and initialize static template data when using DLL
 *
 * Revision 7.2  1995/09/05  19:46:53  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:13:55  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.1  1994/04/15  19:11:06  vriezen
 * Move all files to 6.1
 *
 * Revision 1.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.5  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 1.4  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 1.3  1993/02/12  20:19:03  keffer
 * Ported to g++ v2.3
 *
 * Revision 1.2  1993/02/06  22:13:47  keffer
 * Ported to cfront V3.0
 *
 * Revision 1.1  1993/01/27  21:38:32  keffer
 * Initial revision
 ***************************************************************************
 */

// Neither cfront v3.0 nor Gnu g++ can handle template static data;
// Borland and Metaware can (as long as DLLs aren't lurking)

#if defined(RW_BROKEN_TEMPLATES) || defined(__GNUC__) || defined(_RWTOOLSDLL)
# define nfull_ ((size_t)N>>3)
# define mask_ ((RWByte)((1 << ((size_t)N&7)) - 1))
#else
  template <int N> const size_t RWTBitVec<N>::nfull_ = (size_t)N >> 3;
  template <int N> const RWByte   RWTBitVec<N>::mask_  = (1 << ((size_t)N&7)) - 1;
#endif

template <int N> RWBoolean
RWTBitVec<N>::operator==(RWBoolean b) const
{
  register RWByte val = b ? ~0 : 0;

  // Check the full bytes:
  for (register size_t i=0; i<nfull_; i++) if (vec_[i] != val) return FALSE;
  
  // Check the last (partially full) byte, if any:
  return (nfull_==sizeof(vec_)) ? TRUE : (val & mask_) == (vec_[nfull_] & mask_);
}

template <int N> RWBoolean
RWTBitVec<N>::operator==(const RWTBitVec<N>& u) const
{
  // Check the full bytes:
  for (register size_t i=0; i<nfull_; i++)
    if (vec_[i] != u.vec_[i])
      return FALSE;

  // Check the last (partially full) byte, if any:
  return (nfull_==sizeof(vec_)) ? TRUE : (vec_[nfull_] & mask_) == (u.vec_[nfull_] & mask_);
}

template <int N> RWBoolean
RWTBitVec<N>::operator<(const RWTBitVec<N>& u) const
{
  // Check the full bytes:
  for (register size_t i=0; i<nfull_; i++)
    if (!(vec_[i] < u.vec_[i]))
      return FALSE;

  // Check the last (partially full) byte, if any:
  return (nfull_==sizeof(vec_)) ? TRUE : (vec_[nfull_] & mask_) < (u.vec_[nfull_] & mask_);
}

#if defined(RW_BROKEN_TEMPLATES) || defined(__GNUC__)
# undef nfull_
# undef mask_
#endif
