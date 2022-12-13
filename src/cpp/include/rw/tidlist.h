#ifndef __RWTIDLIST_H__
#define __RWTIDLIST_H__

/*
 * RWTIsvDlist<T>: Parameterized intrusive list of Ts
 *   (which must derive from RWIsvDlink)
 *
 * $Id: tidlist.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.6  1996/07/18 04:13:33  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/07 21:16:07  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:46:58  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 20:06:56  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:33:11  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  02:58:01  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:11:10  vriezen
 * Move all files to 6.1
 *
 * Revision 2.11  1994/03/22  23:26:13  vriezen
 * Added operator--(int) to private.
 *
 * Revision 2.10  1994/01/03  23:42:40  jims
 * ObjectStore version: add #include <ostore/ostore.hh>
 *
 * Revision 2.9  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.8  1993/11/06  04:30:01  jims
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
 * Revision 2.4  1993/01/28  01:32:12  keffer
 * Adjusted access of RWIsvDlist::init.
 *
 * Revision 2.3  1993/01/27  21:39:38  keffer
 * Now uses separate definitions file
 *
 * Revision 2.2  1993/01/23  00:32:27  keffer
 * Performance enhancements; simplified; flatter inheritance tree.
 *
 * 
 *    Rev 1.4   29 May 1992 11:50:46   KEFFER
 * 0u ->0
 * 
 *    Rev 1.3   28 May 1992 20:10:20   KEFFER
 * Ported to Metaware High-C.
 * 
 *    Rev 1.2   25 May 1992 15:53:46   KEFFER
 * Optimized by using an internal link
 * 
 *    Rev 1.0   02 Mar 1992 16:10:50   KEFFER
 * Initial revision.
 *
 ***************************************************************************
 *
 * Declares the parameterized class RWTIsvDlist<T>, an intrusive
 * doubly-linked list.  Items of type T are inserted into the list.  
 * They must inherit from RWIsvSlink.
 */

#include "rw/idlist.h"

// forward declaration of the iterator:
template <class TL> class RWTExport RWTIsvDlistIterator;

/****************************************************************
 *                                                              *
 *              Declarations for RWTIsvDlist<T>                 *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class TL> class RWTExport RWTIsvDlist : private RWIsvDlist
{

public:

  RWTIsvDlist()                        {;}
  RWTIsvDlist(TL* a) : RWIsvDlist(a)   {;}

  /********************* Member functions **************************/

  RWIsvDlist::clear;
  RWIsvDlist::entries;
  RWIsvDlist::isEmpty;

  void          append(TL* a)
        {RWIsvDlist::append(a);}

  void          apply(void (*applyFun)(TL*, void*), void*);

  TL*           at(size_t i) const
        {return (TL*)RWIsvDlist::at(i);}

  void          clearAndDestroy();

  RWBoolean     contains(RWBoolean (*testFun)(const TL*, void*), void* d) const
        {return find(testFun,d)!=rwnil;}

  RWBoolean     containsReference(const TL* a) const
        {return RWIsvDlist::containsReference(a);}

  TL*           find(RWBoolean (*testFun)(const TL*, void*), void*) const;

  TL*           first() const
        {return (TL*)RWIsvDlist::first();}

  TL*           get()
        {return (TL*)RWIsvDlist::get();}

  size_t        index(RWBoolean (*testFun)(const TL*, void*), void*) const;

  void          insert(TL* a)
        {RWIsvDlist::insert(a);}

  void          insertAt(size_t i, TL* a)
        {RWIsvDlist::insertAt(i,a);}

  TL*           last() const
        {return (TL*)RWIsvDlist::last();}

  size_t        occurrencesOf(RWBoolean (*testFun)(const TL*, void*), void*) const;

  size_t        occurrencesOfReference(const TL* a) const
        {return RWIsvDlist::occurrencesOfReference(a);}

  void          prepend(TL* a)
        {RWIsvDlist::prepend(a);}

  TL*           remove(RWBoolean (*testFun)(const TL*, void*), void*);

  TL*           removeAt(size_t i)
        {return (TL*)RWIsvDlist::removeAt(i);}

  TL*           removeFirst()
        {return (TL*)RWIsvDlist::removeFirst();}

  TL*           removeLast()
        {return (TL*)RWIsvDlist::removeLast();}

  TL*           removeReference(TL* a)
        {return (TL*)RWIsvDlist::removeReference(a);}

protected:

  TL*           headLink() const        {return (TL*)&head_;}
  TL*           tailLink() const        {return (TL*)&tail_;}
  TL*           firstLink() const       {return (TL*)head_.next_;}
  TL*           lastLink() const        {return (TL*)tail_.prev_;}

  RWIsvDlist::init;

private:

  friend class RWTExport RWTIsvDlistIterator<TL>;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *                                                              *
 *      Declarations for RWTIsvDlistIterator<TL>                *
 *                                                              *
 ****************************************************************/

template <class TL> class RWTExport RWTIsvDlistIterator : 
                                private RWIsvDlistIterator {

public:

  // Constructor: starts with iterator positioned at headLink
  RWTIsvDlistIterator(RWTIsvDlist<TL>& s) : RWIsvDlistIterator(s) {;}

  // Operators:
  TL*   operator++()          {return (TL*)RWIsvDlistIterator::operator++(); }
  TL*   operator--()          {return (TL*)RWIsvDlistIterator::operator--(); }
  TL*   operator+=(size_t n)  {return (TL*)RWIsvDlistIterator::operator+=(n);}
  TL*   operator-=(size_t n)  {return (TL*)RWIsvDlistIterator::operator-=(n);}
  TL*   operator()()          {return (TL*)RWIsvDlistIterator::operator()(); }


  // Member functions:
  RWTIsvDlist<TL>* container() const
        {return (RWTIsvDlist<TL>*)dlist_;}

  TL*           findNext(RWBoolean (*testFun)(const TL*, void*), void*);

  void          insertAfterPoint(TL* a)
        {RWIsvDlistIterator::insertAfterPoint(a);}

  TL*           key() const
        {return (TL*)RWIsvDlistIterator::key();}

  TL*           remove()
        {return (TL*)RWIsvDlistIterator::remove();}

  TL*           removeNext(RWBoolean (*testFun)(const TL*, void*), void*);

  void          reset()
        {RWIsvDlistIterator::reset();}

  void          reset(RWTIsvDlist<TL>& s)
        {RWIsvDlistIterator::reset(s);}

protected:

  TL*           cursor() const {return (TL*)dhere_;}
  RWIsvDlistIterator::advance;
  RWIsvDlistIterator::backup;
  RWIsvDlistIterator::isActive;

private:

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean             operator++(int);
  RWBoolean             operator--(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tidlist.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tidlist.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWTIDLIST_H__ */
