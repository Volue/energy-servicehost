#ifndef __RWTQUEUE_H__
#define __RWTQUEUE_H__

/*
 * Parameterized queue of T's, implemented using class C.
 *
 * $Id: tqueue.h 18623 2000-03-03 08:28:13Z sih $
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
 * Commercial Computer Software ? Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:44  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.7  1996/08/09 19:32:55  hart
 * HP (aCC compiler) port
 *
 * Revision 7.6  1996/07/18 04:13:33  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/07 19:58:42  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:48:49  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 20:56:01  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:36:39  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:07:14  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:48:07  vriezen
 * Move all files to 6.1
 *
 * Revision 2.5  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.4  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.3  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.2  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 *    Rev 1.3   25 May 1992 15:55:56   KEFFER
 * No longer uses access adjustment
 * 
 *    Rev 1.2   15 Mar 1992 12:47:32   KEFFER
 * 
 *    Rev 1.0   02 Mar 1992 16:10:52   KEFFER
 * Initial revision.
 */

#ifndef __RWDEFS_H__
#  include "rw/defs.h"
#endif

#ifdef RW_BROKEN_TEMPLATES

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T, class C> class RWTExport RWTQueue
{

public:

  void          clear()         {container_.clear();}
  size_t        entries() const {return container_.entries();}
  T             first() const   {return container_.first();}
  T             get()           {return container_.removeFirst();}
  RWBoolean     isEmpty() const {return container_.isEmpty();}
  void          insert(T a)     {container_.append(a);}
  T             last() const    {return container_.last();}

protected:

  C             container_;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#else   /* !RW_BROKEN_TEMPLATES */

#ifndef RW_INHERIT_ACCESS_PROBLEM
template <class T, class C> class RWTExport RWTQueue : private C
#else
template <class T, class C> class RWTExport RWTQueue : public C
#endif
{

public:

  C::clear;
  C::entries;
  C::first;
  T             get()           {return C::removeFirst();}
  void          insert(T a)     {C::append(a);}
  C::isEmpty;
  C::last;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#endif  /* RW_BROKEN_TEMPLATES */

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWTQUEUE_H__ */
