#ifndef __RWTVVECTOR_H__
#define __RWTVVECTOR_H__
/*
 * RWTValVector: Parameterized vectors of values
 *
 * $Id: tvvector.h 18623 2000-03-03 08:28:13Z sih $
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
 * Stores a *copy* of the inserted item into the collection.
 *
 * Assumes that T has:
 *   - well-defined copy semantics (T::T(const T&) or equiv.);
 *   - well-defined assignment semantics (T::operator=(const T&) or equiv.);
 *
 * Note that while these are automatically defined for builtin types
 * (such as "int", "double", or any pointer), you may need to provide
 * appropriate operators for your own classes, particularly those with
 * constructors and/or pointers to other objects.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.12  1996/07/18 04:13:33  hart
 * ObjectStore changes
 *
 * Revision 7.11  1996/05/07 18:31:08  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.10  1996/03/18 16:04:45  jims
 * Port to cfront template mechanism
 *
 * Revision 7.9  1996/03/16 15:15:37  jims
 * Port non-stdlib template persistence to DEC and Watcom
 *
 * Revision 7.8  1996/02/18 01:50:29  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.7  1996/02/05 20:45:28  griswolf
 * Fix typo on previous patch.
 *
 * Revision 7.6  1996/02/05 16:12:38  griswolf
 * Scopus 1834: Never allocate using new T[i=0], to avoid deletion bug when
 * T has virtual destructor.
 *
 * Revision 7.5  1996/01/27 18:33:18  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.4  1995/11/13 12:36:46  jims
 * return by const ref instead of by value (operators () and [])
 *
 * Revision 7.3  1995/09/05 21:32:58  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:38:46  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:11:55  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:48:45  vriezen
 * Move all files to 6.1
 *
 * Revision 2.11  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.10  1993/11/16  03:19:22  jims
 * Add RWTExport modifier
 *
 * Revision 2.9  1993/11/08  21:40:06  jims
 * Port to ObjectStore
 *
 * Revision 2.8  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.7  1993/06/18  23:33:59  keffer
 * Removed unneeded #include "toolerr.h"
 *
 * Revision 2.6  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.5  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.4  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 2.3  1993/02/12  00:35:29  keffer
 * Ported to the IBM xlC compiler
 *
 * Revision 2.2  1993/01/28  02:01:20  keffer
 * Put definitions in a separate tvvector.cc
 *
 *    Rev 1.4   04 Sep 1992 12:59:58   KEFFER
 * Added comment about requiring less-than semantics
 * 
 *    Rev 1.3   17 Mar 1992 19:21:10   KEFFER
 * Changed BOUNDS_CHECK to RWBOUNDS_CHECK
 * 
 *    Rev 1.0   02 Mar 1992 16:10:56   KEFFER
 * Initial revision.
 */

#ifndef __RWTOOLDEFS_H__
#  include "rw/tooldefs.h"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T> class RWTExport RWTValVector
{

public:

  RWTValVector()           : npts_(0), array_(0)        {;}
  RWTValVector(size_t n) : npts_(n)
  { array_ = (0==n) ? 0 : RW_NEW_TSPEC_VEC(this, T, n) T[n]; }
  RWTValVector(size_t n, const T& ival);
  RWTValVector(const RWTValVector<T> &);
  ~RWTValVector()       { if(array_) RWVECTOR_DELETE(npts_) array_; }

  RWTValVector<T> &     operator=(const RWTValVector<T> &);
  RWTValVector<T> &     operator=(const T& ival);
  const T*              data() const     {return array_;}
  size_t                length() const   {return npts_;}
  const T&              ref(size_t i) const {return array_[i];}
  void                  reshape(size_t);
  T&                    operator()(size_t i){
#ifdef RWBOUNDS_CHECK
    boundsCheck(i);
#endif
    return array_[i]; }
  const T&              operator()(size_t i) const {
#ifdef RWBOUNDS_CHECK
    boundsCheck(i);
#endif
    return array_[i]; }
  T&                    operator[](size_t i)    { boundsCheck(i); return array_[i];}
  const T&              operator[](size_t i) const  { boundsCheck(i); return array_[i];}

protected:

  void                  boundsCheck(size_t i) const;

  size_t                npts_;
  T*                    array_;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTValVector)

#ifdef RW_TEMPLATE_BASE_ARG_REJECTION
class RWExport RWpistream; class RWExport RWpostream;
class RWExport RWbistream; class RWExport RWbostream; 
class RWExport RWeistream; class RWExport RWeostream;
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValVector,RWpistream,RWpostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValVector,RWbistream,RWbostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValVector,RWeistream,RWeostream)
#endif     


template <class T, class S>
void rwSaveGuts(S& str, const RWTValVector<T>& c);

template <class T, class S>
void rwRestoreGuts(S& str, RWTValVector<T>& c);


#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvvector.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvvector.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWTVVECTOR_H__ */
