#ifndef __RWXPDLIST_H__
#define __RWXPDLIST_H__
/*
 * RWTPtrDlist<TP>: A doubly-linked list of pointers to objects of type TP.
 *
 * $Id: xpdlist.h 18623 2000-03-03 08:28:13Z sih $
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
 * Stores a *pointer* to the inserted item into the collection.
 *
 * Assumes that TP has:
 *   - well-defined equality semantics (TP::operator==(const TP&)).
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.8  1996/08/26 21:56:32  hart
 * HP (aCC) compiler port
 *
 * Revision 7.7  1996/07/18 04:03:18  hart
 * ObjectStore changes
 *
 * Revision 7.6  1996/05/07 18:16:51  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.5  1996/03/18 16:04:45  jims
 * Port to cfront template mechanism
 *
 * Revision 7.4  1996/03/16 15:15:37  jims
 * Port non-stdlib template persistence to DEC and Watcom
 *
 * Revision 7.3  1996/02/18 01:50:54  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1996/01/27 00:48:35  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.1  1995/11/13 12:30:40  jims
 * Move to revision 7.1
 *
 * Revision 7.3  1995/09/05  20:13:20  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:34:12  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:00:15  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:47:40  vriezen
 * Move all files to 6.1
 *
 * Revision 2.13  1994/01/12  03:09:17  jims
 * Add constness to T* parameters where appropriate
 *
 * Revision 2.12  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.11  1993/12/10  19:40:11  jims
 * ObjectStore version: remove private postfix operators
 *
 * Revision 2.10  1993/11/06  02:29:18  jims
 * Fix ppp directive
 *
 * Revision 2.9  1993/11/06  02:08:54  jims
 * Port to ObjectStore
 *
 * Revision 2.8  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.7  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.6  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.5  1993/02/12  23:33:45  keffer
 * Ported to g++ v2.3
 *
 * Revision 2.4  1993/02/12  00:18:50  keffer
 * Ported to the IBM xlC compiler
 *
 * Revision 2.3  1993/01/28  01:33:50  keffer
 * Added clearAndDestroy(); corrected syntax error with removeAt(int).
 *
 * Revision 2.2  1993/01/27  21:39:41  keffer
 * Now uses separate definitions file
 *
 *    Rev 1.3   09 Sep 1992 19:36:52   KEFFER
 * Pointer iterators now return TP* where appropriate
 * 
 *    Rev 1.1   04 Sep 1992 12:54:10   KEFFER
 * remove taking a tester function now returns a TP*, instead of RWBoolean.
 * 
 *    Rev 1.0   25 May 1992 15:59:04   KEFFER
 * Initial revision.
 * 
 */

#include "rw/tidlist.h"

template <class TP> class RWTExport RWTPtrDlistIterator;

/****************************************************************
 *                                                              *
 *              Declarations for RWTPtrDlink<TP>                *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

/*
 * This is the actual link that is stored in the linked list.
 * It includes data of type "TP".
 */

template <class TP> class RWTExport RWTPtrDlink : public RWIsvDlink
{

public:

  RWTPtrDlink(TP* a) : info_(a) {;}
  RWTPtrDlink<TP>*      next() const
    { return (RWTPtrDlink<TP>*)RWIsvDlink::next();}
  RWTPtrDlink<TP>*      prev() const
    { return (RWTPtrDlink<TP>*)RWIsvDlink::prev();}

  TP*           info_;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrDlist<TP>                        *
 *                                                              *
 ****************************************************************/

template <class TP> class RWTExport RWTPtrDlist 
                                    : private RWTIsvDlist< RWTPtrDlink<TP> > 
{

public:

  RWTPtrDlist() {;}
  RWTPtrDlist(const RWTPtrDlist<TP>&);
  ~RWTPtrDlist() { clear(); }

  // Operators:
  RWTPtrDlist<TP>&      operator=(const RWTPtrDlist<TP>&);

  TP*&          operator[](size_t i)
        {return RWTIsvDlist<RWTPtrDlink<TP> >::at(i)->info_;}
  TP*const&     operator[](size_t i) const
        {return RWTIsvDlist<RWTPtrDlink<TP> >::at(i)->info_;}
  
  // Members
  void          append(TP* a);

  void          apply(void (*applyFun)(TP*, void*), void*);

  TP*&          at(size_t i)
        {return RWTIsvDlist<RWTPtrDlink<TP> >::at(i)->info_;}
  TP*const&     at(size_t i) const
        {return RWTIsvDlist<RWTPtrDlink<TP> >::at(i)->info_;}

  void          clear()
        {RWTIsvDlist<RWTPtrDlink<TP> >::clearAndDestroy();}

  void          clearAndDestroy();

  RWBoolean     contains(const TP* a) const;
  RWBoolean     contains(RWBoolean (*testFun)(TP*, void*), void*) const;

  size_t        entries() const
        {return RWTIsvDlist<RWTPtrDlink<TP> >::entries();}

  TP*           find(const TP* p) const;
  TP*           find(RWBoolean (*testFun)(TP*, void*), void* d) const;

  TP*&          first()       { return firstLink()->info_; }
  TP*const&     first() const { return firstLink()->info_; }

  TP*           get()
        {return peel(RWTIsvDlist<RWTPtrDlink<TP> >::get());}

  size_t        index(const TP* a) const;
  size_t        index(RWBoolean(*testFun)(TP*, void*), void*) const;

  void          insert(TP* a);

  void          insertAt(size_t i, TP* a);

  RWBoolean     isEmpty() const
        {return RWTIsvDlist<RWTPtrDlink<TP> >::isEmpty();}

  TP*&          last()          { return lastLink()->info_; }
  TP*const&     last() const    { return lastLink()->info_; }

  size_t        occurrencesOf(const TP* a) const;
  size_t        occurrencesOf(RWBoolean (*testFun)(TP*, void*), void*) const;

  void          prepend(TP* a);

  TP*           remove(const TP* p);
  TP*           remove(RWBoolean (*testFun)(TP*, void*), void* d);

  TP*           removeAt(size_t i)
        {return peel(RWTIsvDlist<RWTPtrDlink<TP> >::removeAt(i));}

  TP*           removeFirst()
        {return peel(RWTIsvDlist<RWTPtrDlink<TP> >::removeFirst());}

  TP*           removeLast()
        {return peel(RWTIsvDlist<RWTPtrDlink<TP> >::removeLast()); }

  size_t        removeAll(const TP* a);
  size_t        removeAll(RWBoolean (*testFun)(TP*, void*), void*);

protected:
    
  RWTPtrDlink<TP>*      findVal(const TP*) const;
  RWTPtrDlink<TP>*      findFun(RWBoolean (*testFun)(TP*, void*), void*) const;
  static TP*            peel(RWTPtrDlink<TP>* link);

private:

friend class RWTExport RWTPtrDlistIterator<TP>;  

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTPtrDlist)

#ifdef RW_NO_TEMPLINST_ON_BASE
class RWExport RWpistream; class RWExport RWpostream;
class RWExport RWbistream; class RWExport RWbostream; 
class RWExport RWeistream; class RWExport RWeostream;
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrDlist,RWpistream,RWpostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrDlist,RWbistream,RWbostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrDlist,RWeistream,RWeostream)
#endif     

/****************************************************************
 *                                                              *
 *      Declarations for RWTPtrDlistIterator<TP>                *
 *                                                              *
 ****************************************************************/

template <class TP> class RWTExport RWTPtrDlistIterator : 
                    public RWTIsvDlistIterator<RWTPtrDlink<TP> >
{

public:

  RWTPtrDlistIterator(RWTPtrDlist<TP>& s) : 
                RWTIsvDlistIterator<RWTPtrDlink<TP> >(s) { }

  // Operators:
  RWBoolean     operator++()
        {this->advance(); return cursor()!=container()->tailLink();}
  RWBoolean     operator--()
        {this->backup(); return cursor()!=container()->headLink();}
  RWBoolean     operator+=(size_t n)
        {return RWTIsvDlistIterator<RWTPtrDlink<TP> >::operator+=(n)!=rwnil;}
  RWBoolean     operator-=(size_t n)
        {return RWTIsvDlistIterator<RWTPtrDlink<TP> >::operator-=(n)!=rwnil;}
  TP*           operator()()
        {this->advance(); return cursor()==container()->tailLink() ? rwnil : cursor()->info_;}

  // Methods:
  RWTPtrDlist<TP>* container() const
        {return (RWTPtrDlist<TP>*)RWTIsvDlistIterator<RWTPtrDlink<TP> >::container();}

  TP*           findNext(const TP* p);
  TP*           findNext(RWBoolean (*testFun)(TP*, void*), void* d);

  void          insertAfterPoint(TP* a);

  TP*           key() const
        {return isActive() ? cursor()->info_ : rwnil;}

  TP*           remove();

  TP*           removeNext(const TP* p);
  TP*           removeNext(RWBoolean (*testFun)(TP*, void*), void* d);

  void          reset()
        {RWTIsvDlistIterator<RWTPtrDlink<TP> >::reset();}
  void          reset(RWTPtrDlist<TP>& s)
        {RWTIsvDlistIterator<RWTPtrDlink<TP> >::reset(s);}

private:

  // Disallow postfix operators.  Unless we hide it, some compilers will
  // substitute the prefix operator in its place.
  RWBoolean             operator++(int);
  RWBoolean             operator--(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


template <class TP, class S>
void rwSaveGuts(S& str, const RWTPtrDlist<TP>& c);

template <class TP, class S>
void rwRestoreGuts(S& str, RWTPtrDlist<TP>& c);


#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xpdlist.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xpdlist.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXPDLIST_H__ */
