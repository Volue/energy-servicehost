#ifndef __RWTISLIST_H__
#define __RWTISLIST_H__
/*
 * RWTIsvSlist<T>: Parameterized intrusive list of Ts (which must derive from RWIsvSlink)
 *
 * $Id: tislist.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.8  1996/08/27 00:13:10  hart
 * removed RW_INHERIT_ACCESS_PROBLEM guard
 *
 * Revision 7.7  1996/08/09 19:13:15  hart
 * HP (aCC compiler) port
 *
 * Revision 7.6  1996/07/18 04:13:33  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/07 21:19:46  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:47:03  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 20:07:26  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:47:26  jims
 * Scopus TAN ID# 82: Use RWTExport for using templates with Tools DLL
 * under MSVC 2.x
 *
 * Revision 7.1  1994/10/16  02:58:08  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/18  19:03:55  jims
 * Add copyright
 *
 * Revision 6.1  1994/04/15  19:11:14  vriezen
 * Move all files to 6.1
 *
 * Revision 2.10  1994/01/03  23:33:28  jims
 * ObjectStore version: Add #include <ostore/ostore.hh>
 *
 * Revision 2.9  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.8  1993/11/08  10:55:24  jims
 * Port to ObjectStore
 *
 * Revision 2.7  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.6  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.5  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.4  1993/01/28  01:32:36  keffer
 * Adjusted access of RWIsvSlist::init.
 *
 * Revision 2.3  1993/01/27  21:39:40  keffer
 * Now uses separate definitions file
 *
 * Revision 2.2  1993/01/23  00:32:27  keffer
 * Performance enhancements; simplified; flatter inheritance tree.
 *
 * 
 *    Rev 1.0   02 Mar 1992 16:10:52   KEFFER
 * Initial revision.
 *
 ***************************************************************************
 *
 * Declares the parameterized class RWTIsvSlist<T>, an intrusive
 * singly-linked list of links of type T, which MUST inherit from RWIsvSlink.
 */

#include "rw/islist.h"
// forward declaration of the iterator:
template <class TL> class RWTExport RWTIsvSlistIterator;

/****************************************************************
 *                                                              *
 *              Declarations for RWTIsvSlist<T>                 *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class TL> class RWTExport RWTIsvSlist : private RWIsvSlist
{

public:

  RWTIsvSlist()                         {;}
  RWTIsvSlist(TL* a) : RWIsvSlist(a)    {;}

  /********************* Member functions **************************/

  RWIsvSlist::clear;
  RWIsvSlist::entries;
  RWIsvSlist::isEmpty;

  void          append(TL* a)
        {RWIsvSlist::append(a);}

  void          apply(void (*applyFun)(TL*, void*), void*);

  TL*           at(size_t i) const
        {return (TL*)RWIsvSlist::at(i);}

  void          clearAndDestroy();

  RWBoolean     contains(RWBoolean (*testFun)(const TL*, void*), void* d) const
        {return find(testFun,d)!=rwnil;}

  RWBoolean     containsReference(const TL* a) const
        {return RWIsvSlist::containsReference(a);}

  TL*           find(RWBoolean (*testFun)(const TL*, void*), void*) const;

  TL*           first() const
        {return (TL*)RWIsvSlist::first();}

  TL*           get()
        {return (TL*)RWIsvSlist::get();}

  size_t        index(RWBoolean (*testFun)(const TL*, void*), void*) const;

  void          insert(TL* a)
        {RWIsvSlist::insert(a);}

  void          insertAt(size_t i, TL* a)
        {RWIsvSlist::insertAt(i, a);}

  TL*           last() const
        {return (TL*)RWIsvSlist::last();}

  size_t        occurrencesOf(RWBoolean (*testFun)(const TL*, void*), void*) const;

  size_t        occurrencesOfReference(const TL* a) const
        {return RWIsvSlist::occurrencesOfReference(a);}

  void          prepend(TL* a)
        {RWIsvSlist::prepend(a);}

  TL*           remove(RWBoolean (*testFun)(const TL*, void*), void*);

  TL*           removeAt(size_t i)
        {return (TL*)RWIsvSlist::removeAt(i);}

  TL*           removeFirst()
        {return (TL*)RWIsvSlist::removeFirst();}

  TL*           removeLast()
        {return (TL*)RWIsvSlist::removeLast();}

  TL*           removeReference(TL* a)
        {return (TL*)RWIsvSlist::removeReference(a);}

protected:

  TL*           headLink()  const {return (TL*)&head_;}
  TL*           tailLink()  const {return (TL*)&tail_;}
  TL*           firstLink() const {return (TL*)head_.next_;}
  TL*           lastLink()  const {return (TL*)last_;}

  TL*           removeRight(TL* a)      // Remove and return link after the argument
        {return (TL*)RWIsvSlist::removeRight(a);}

  RWIsvSlist::init;

private:

friend class RWTExport RWTIsvSlistIterator<TL>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};

/****************************************************************
 *                                                              *
 *              Declarations for RWTIsvSlistIterator<TL>        *
 *                                                              *
 ****************************************************************/

template <class TL> class RWTExport RWTIsvSlistIterator : 
                                private RWIsvSlistIterator {

public:

  // Constructor: starts with iterator positioned at headLink
  RWTIsvSlistIterator(RWTIsvSlist<TL>& s) : RWIsvSlistIterator(s) {;}

  // Operators:
  TL*   operator++()          {return (TL*)RWIsvSlistIterator::operator++(); }
  TL*   operator+=(size_t n)  {return (TL*)RWIsvSlistIterator::operator+=(n);}
  TL*   operator()()          {return (TL*)RWIsvSlistIterator::operator()(); }

  // Member functions:
  RWTIsvSlist<TL>* container() const
        {return (RWTIsvSlist<TL>*)slist_;}

  TL*           findNext(RWBoolean (*testFun)(const TL*, void*), void*);

  void          insertAfterPoint(TL* a)
        {RWIsvSlistIterator::insertAfterPoint(a);}

  TL*           key() const
        {return (TL*)RWIsvSlistIterator::key();}

  TL*           remove()        // Relatively slow
        {return (TL*)RWIsvSlistIterator::remove();}

  TL*           removeNext(RWBoolean (*testFun)(const TL*, void*), void*);

  void          reset()
        {RWIsvSlistIterator::reset();}

  void          reset(RWTIsvSlist<TL>& s)
        {RWIsvSlistIterator::reset(s);}

protected:
  RWIsvSlistIterator::advance;
  RWIsvSlistIterator::isActive;

  TL*           cursor() const {return (TL*)shere_;}

private:

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  TL*           operator++(int);

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tislist.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tislist.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWTISLIST_H__ */
