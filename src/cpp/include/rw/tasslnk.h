#ifndef __RWTVASSLNK_H__
#define __RWTVASSLNK_H__
/*
 * RWTValAssocLink: Key / Value association link using values
 * RWTPtrAssocLink: Key / Value association link using pointers
 *
 * $Id: tasslnk.h 18623 2000-03-03 08:28:13Z sih $
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
 * Class RWTValAssocLink<K,V> defines an association between a key of
 * type K, and a value of type V in a singly-linked link, using value
 * semantics.
 *
 * It has a single value constructor that takes just the key.
 * This means that the value will be constructed using the default
 * constructor for type V.  Usually this works just fine.  However, if the
 * value (type V) is a builtin, then its value will be left undefined.
 * Usually this also works fine.  However, if this is unsatisfactory,
 * then you can supply your own definition that overrides the template-
 * generated definition.  For an explanation of user-specified overrides
 * of template-generated definitions, see Stroustrup II, sec. 8.4.1.
 *
 * Example:
 *
 *   RWTValAssocLink<int,double>::RWTValAssocLink(int i) :
 *     key_(i)
 *    {
 *      value_ = 0.0;   // Explicitly set the value to zero.
 *    }
 *
 * Class RWTPtrAssocLink<K,V> defines an association between a pointer to
 * a key of type K and a pointer to a value of type V in a singly-linked
 * link.
 *
 * It also has a single value constructor which takes just a pointer to
 * a key.  It always sets the value pointer to nil:
 *
 *   RWTPtrAssocLink<int,double>::RWTPtrAssocLink(int* i) :
 *     key_(i),
 *     value_(rwnil)
 *    {
 *    }
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.6  1996/07/18 04:13:33  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/07 21:15:38  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:46:51  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 19:46:42  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:31:56  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  02:57:53  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:11:04  vriezen
 * Move all files to 6.1
 *
 * Revision 2.7  1993/12/31  00:02:53  jims
 * Left justify preprocessor directives
 *
 * Revision 2.6  1993/11/04  14:06:32  jims
 * Port to ObjectStore
 *
 * Revision 2.5  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.4  1993/02/17  20:26:07  keffer
 * Added class RWTPtrAssocLink<KP,VP>
 *
 * Revision 2.3  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 2.2  1993/01/28  21:11:49  keffer
 * Ported to cfront V3.0
 *
 *    Rev 1.0   02 Mar 1992 16:10:52   KEFFER
 * Initial revision.
 */

#ifndef __RWISLIST_H__
# include "rw/islist.h"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class K, class V> struct RWTExport RWTValAssocLink : public RWIsvSlink
{
  K             key_;
  V             value_;
  RWTValAssocLink(const K& key);
  RWTValAssocLink(const K& key, const V& value) : key_(key), value_(value) { }

  RWTValAssocLink<K,V>* next() const { return (RWTValAssocLink<K,V>*)next_;}

  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};

template <class K, class V> struct RWTExport RWTPtrAssocLink : public RWIsvSlink
{
  K*            key_;
  V*            value_;
  RWTPtrAssocLink(K* key) : key_(key), value_(rwnil) {;}
  RWTPtrAssocLink(K* key, V* value) : key_(key), value_(value) {;}

  RWTPtrAssocLink<K,V>* next() const { return (RWTPtrAssocLink<K,V>*)next_;}

  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tasslnk.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tasslnk.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWTVASSLNK_H__ */
