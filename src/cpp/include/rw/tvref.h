#ifndef __RWTVREF_H__
#define __RWTVREF_H__
/*
 * RWTVirtualRef<T>: A typed interface to RWVirtualRef
 *
 * $Id: tvref.h 18623 2000-03-03 08:28:13Z sih $
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
 * Although this array could be used by the end user it is really intended
 * to be the reference part of a reference counted array.  It uses
 * pure copy semantics and hence is relatively slow.  
 *
 * It inherits from RWReference and hence contains a reference count.
 * This is used to implement copy-on-write reference counting as was
 * done with class RWTValVirtualArray<T>.
 *
 * Assumes that T has:
 *   - well-defined copy constructor (T::T(const T&) or equiv.);
 *   - well-defined assignment operator (T::operator=(const T&) or equiv.).
 *
 * Note that the location in physical memory of a particular index 
 * of the array can change from invocation to invocation.  Hence,
 * the address of an element should never be taken. 
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:44  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.6  1996/07/18 04:13:33  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/07 19:04:22  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:49:26  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 21:27:35  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:37:50  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:10:41  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:47:57  vriezen
 * Move all files to 6.1
 *
 * Revision 2.5  1993/11/08  20:32:10  jims
 * Port to ObjectStore
 *
 * Revision 2.4  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.3  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 2.2  1993/01/29  20:46:00  keffer
 * *** empty log message ***
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.2   09 Sep 1992 12:51:42   KEFFER
 * RWTVirtualRef<T> is now marked with "RWTExport" for use in Windows.
 * 
 *    Rev 1.1   17 Mar 1992 19:41:40   KEFFER
 * 
 *    Rev 1.0   11 Mar 1992 14:10:00   KEFFER
 * Initial revision.
 */

#include "rw/vref.h"
#include "rw/toolerr.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T> class RWTExport RWTVirtualRef : public RWVirtualRef
{

public:

  RWTVirtualRef(long size, RWVirtualPageHeap* h);
  RWTVirtualRef(RWTVirtualRef<T>&);             // Takes deep copy.

  T                     val(long i);
  void                  set(long i, const T& v);
  void                  set(long start, long extent, const T& v);
  virtual void          conformalCopy(long start1, RWVirtualRef& v2, long start2, long N);

  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};


#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvref.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvref.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWTVREF_H__ */
