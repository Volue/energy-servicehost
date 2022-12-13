#ifndef  __RWGDLIST_H__
#define  __RWGDLIST_H__

/*
 * Declarations for General Doubly-linked lists.
 *
 * $Id: gdlist.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:41  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.5  1996/07/18 04:29:17  hart
 * ObjectStore changes
 *
 * Revision 7.4  1996/04/26 22:37:53  groening
 * removed the RW_BROKEN_TOKEN_PASTE wrapper.  it is never defined anywhere.
 * so yell at me if i broke things for MPW C++ 3.0 port.
 *
 * Revision 7.3  1996/02/18 01:41:51  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 18:32:26  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:49:29  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/14  21:04:32  jims
 * Use RWGExport for generic collection classes
 *
 * Revision 6.2  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:09:59  vriezen
 * Move all files to 6.1
 *
 * Revision 2.7  1994/01/13  05:03:17  jims
 * Add member function  const type* at(size_t i) const as documented in manual
 *
 * Revision 2.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.5  1993/08/03  21:02:38  dealys
 * Ported to MPW C++ 3.3 - RW_BROKEN_TOKEN_PASTE
 *
 * Revision 2.4  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.3  1993/03/25  03:50:41  keffer
 * Added prefix RW to class name
 *
 * Revision 2.2  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 *    Rev 1.3   18 Feb 1992 19:22:58   KEFFER
 * Now includes "rw/generic.h".
 * Class tag is now RWExport instead of huge.
 * 
 *    Rev 1.2   28 Oct 1991 09:08:14   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.1   09 Oct 1991 18:34:32   keffer
 * Ported to Zortech V3.0
 * 
 *    Rev 1.0   28 Jul 1991 08:11:18   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 */

#include "rw/dlist.h"
#include "rw/generic.h"

#  define RWGDlist(type) name2(type,RWGDlist)
#  define RWGDlistIterator(type) name2(type,RWGDlistIterator)

#define RWGDlistdeclare(type)                                              \
class RWGExport RWGDlist(type) : public RWDlist {                          \
public:                                                                    \
  RWGDlist(type)() { }                                                     \
  RWGDlist(type)(type* a) : RWDlist(a) { }                                 \
  type* append(type* a)                                                    \
    { return (type*)RWDlist::append(a); }                                  \
  type*& at(size_t i)                                                      \
    { return (type*&)RWDlist::at(i); }                                     \
  const type* at(size_t i) const                                           \
    { return (const type*)RWDlist::at(i); }                                \
  RWBoolean contains( RWBoolean (*t)(const type*, const void*), const void* a) const\
    { return RWDlist::contains((RWtestGeneric)t, a); }                     \
  RWBoolean containsReference(const type* a) const                         \
    { return RWDlist::containsReference(a); }                              \
  void  apply(void (*ap)(type*, void*), void* x)                           \
    { RWDlist::apply((RWapplyGeneric)ap, x); }                             \
  type* find(RWBoolean (*t)(const type*, const void*), const void* a) const\
    { return (type*)RWDlist::find((RWtestGeneric)t, a); }                  \
  type* findReference(const type* a) const                                 \
    { return (type*)RWDlist::findReference(a); }                           \
  type* first() const                                                      \
    { return (type*)RWDlist::first(); }                                    \
  type* get()                                                              \
    { return (type*)RWDlist::get(); }                                      \
  type* insert(type* a)                                                    \
    { return (type*)RWDlist::append(a); }                                  \
  type* insertAfter(int i, type* a)                                        \
    { return (type*)RWDlist::insertAfter(i,a); }                           \
  type* insertAt(size_t i, type* a)                                        \
    { return (type*)RWDlist::insertAt(i,a); }                              \
  type* last() const                                                       \
    { return (type*)RWDlist::last(); }                                     \
  size_t occurrencesOf(RWBoolean (*t)(const type*, const void*), const void* a) const\
    { return RWDlist::occurrencesOf((RWtestGeneric)t, a); }                \
  size_t occurrencesOfReference(const type* a) const                       \
    { return RWDlist::occurrencesOfReference(a); }                         \
  type*  prepend(type* a)                                                  \
    { return (type*)RWDlist::prepend(a); }                                 \
  type* remove(RWBoolean (*t)(const type*, const void*), const void* a)    \
    { return (type*)RWDlist::remove((RWtestGeneric)t, a); }                \
  type* removeReference(const type* a)                                     \
    { return (type*)RWDlist::removeReference(a); }                         \
  RW_T_TYPESPEC  /* This macro usually expands to nothing */               \
};                                                                         \
                                                                           \
class RWGExport RWGDlistIterator(type) : public RWDlistIterator {          \
public:                                                                    \
  RWGDlistIterator(type)(RWGDlist(type)& s) : RWDlistIterator(s) { }       \
  type* findNext(RWBoolean (*t)(const type*, const void*), const void* a)  \
    { return (type*)RWDlistIterator::findNext((RWtestGeneric)t, a); }      \
  type* findNextReference(const type* a)                                   \
    { return (type*)RWDlistIterator::findNextReference(a); }               \
  type* insertAfterPoint(type* a)                                          \
    { return (type*)RWDlistIterator::insertAfterPoint(a); }                \
  type* key() const {return (type*)RWDlistIterator::key();}                \
  type* operator()() {return (type*)(RWDlistIterator::operator()()); }     \
  type* remove() {return (type*)RWDlistIterator::remove();}                \
  type* removeNext(RWBoolean (*t)(const type*, const void*), const void* a)\
    {return (type*)RWDlistIterator::removeNext((RWtestGeneric)t, a);}      \
  type* removeNextReference(const type* a)                                 \
    {return (type*)RWDlistIterator::removeNextReference(a);}               \
};

#endif /* __RWGDLIST_H__ */
