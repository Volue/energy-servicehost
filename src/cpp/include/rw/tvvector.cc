/*
 * Template definitions for RWTValVector<T>
 *
 * $Id: tvvector.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.13  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.12  1996/07/18 04:09:03  hart
 * ObjectStore changes
 *
 * Revision 7.11  1996/03/16 15:15:37  jims
 * Port non-stdlib template persistence to DEC and Watcom
 *
 * Revision 7.10  1996/02/18 01:50:27  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.9  1996/02/12 21:03:06  kevinj
 * Switched stream and class args in rw*Guts.
 *
 * Revision 7.8  1996/02/05 20:53:05  griswolf
 * Fix another typo.
 *
 * Revision 7.7  1996/02/05 20:50:14  griswolf
 * Fix typo on previous patch.
 *
 * Revision 7.6  1996/02/05 16:19:23  griswolf
 * Scopus 1834: Never allocate using new T[i=0], to avoid deletion bug when
 * T has virtual destructor.
 *
 * Revision 7.5  1996/01/27 18:33:18  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.4  1995/09/05 21:32:38  jims
 * Use new copyright macro
 *
 * Revision 7.3  1995/06/15  16:51:04  jims
 * Add comment to operator=
 *
 * Revision 7.2  1995/05/02  17:30:02  griswolf
 * put try / catch protection around reshape (op assign can throw)
 *
 * Revision 7.1  1994/10/16  03:31:21  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  20:04:35  vriezen
 * Updated Copyright.
 *
 * Revision 6.1  1994/04/15  19:48:44  vriezen
 * Move all files to 6.1
 *
 * Revision 1.7  1993/11/08  21:40:06  jims
 * Port to ObjectStore
 *
 * Revision 1.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.5  1993/06/18  23:34:44  keffer
 * New messaging architecture for localization
 *
 * Revision 1.4  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 1.3  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 1.2  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 1.1  1993/01/28  02:02:26  keffer
 * Initial revision
 *
 ***************************************************************************
 */

#include "rw/toolerr.h"
#include "rw/rwerr.h"

template<class T>
RWTValVector<T>::RWTValVector(register size_t n, const T& ival)
{
  npts_=n;
  array_= (0 == n) ? 0 : RW_NEW_TSPEC_VEC(this, T, n) T[n];
  register T* dst = array_;
  while (n--) *dst++ = ival;
}

template<class T>
RWTValVector<T>::RWTValVector(const RWTValVector<T>& a)
{
  register size_t i= npts_ = a.npts_;
  array_ = (0==i) ? 0 : RW_NEW_TSPEC_VEC(this, T, i) T[i];
  register T* dst = array_;
  register T* src = a.array_;
  while (i--) *dst++ = *src++;
}

template<class T> RWTValVector<T>&
RWTValVector<T>::operator=(const RWTValVector<T>& a)
{
  /* guard against copying self onto self, and also,
     don't bother doing anything if both array_s are nil */
  
  if(array_ != a.array_) {
    if(array_)
      RWVECTOR_DELETE(npts_) array_; /* Disconnect from old array_ */
    register size_t i = npts_ = a.npts_;
    array_ = (0==i) ? 0 : RW_NEW_TSPEC_VEC(this, T, i) T[i];
    register T* dst = array_;
    register T* src = a.array_;
    while (i--) *dst++ = *src++;
  }
  return *this;
}
                                                                                
template<class T> RWTValVector<T>&
RWTValVector<T>::operator=(const T& val)
{
  for (register size_t i=0; i<npts_; i++)
    array_[i] = val;
  return *this;
}

template<class T> void
RWTValVector<T>::reshape(size_t N)
{
  if (N==npts_) return;
  T* newArray = (0==N) ? 0 : RW_NEW_TSPEC_VEC(this, T, N) T[N];
  register size_t i = (N<=npts_) ? N:npts_;
  register T* src = array_;
  register T* dst = newArray;
  RW_TRYSTART
  while (i--) *dst++ = *src++;
  RW_TRYEND(RWVECTOR_DELETE(N) newArray) /* N never 0 here */
  if(array_) RWVECTOR_DELETE(npts_) array_;
  array_ = newArray;
  npts_ = N;
}

template<class T> void
RWTValVector<T>::boundsCheck(size_t i) const
{
  if (i>=npts_)
    RWTHROW( RWBoundsErr(RWMessage(RWTOOL_INDEX, (unsigned)i, (unsigned)npts_) ));
}


#include "rw/epersist.h"

RWDEFINE_PERSISTABLE_TEMPLATE(RWTValVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
#include "rw/pstream.h"
#include "rw/bstream.h"
#include "rw/estream.h"
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValVector,RWpistream,RWpostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValVector,RWbistream,RWbostream)
RWDEFINE_PERSIST_TEMPLATE_IO(RWTValVector,RWeistream,RWeostream)
#endif     

template <class T, class S>
void rwSaveGuts(S& str, const RWTValVector<T>& c)
{
#ifdef RW_FIX_XSIZE_T
  str.putSizeT(c.length());
#else
  str << c.length();
#endif
  for (size_t i=0; i < c.length(); ++i)
    str << c[i];
}

template <class T, class S>
void rwRestoreGuts(S& str, RWTValVector<T>& c)
{
  size_t count;
#ifdef RW_FIX_XSIZE_T
  str.getSizeT(count);
#else
  str >> count;
#endif
  c.reshape(0);
  c.reshape(count);
  for (size_t i=0; i < count; ++i) 
    str >> c[i];
}
