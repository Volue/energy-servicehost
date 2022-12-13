#ifndef __RWXPSLIST_H__
#define __RWXPSLIST_H__
/*
 * RWTPtrSlist<TP>: A singly-linked list of pointers to objects of type TP.
 *
 * $Id: xpslist.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.7  1996/08/26 21:54:01  hart
 * HP (aCC) compiler port
 *
 * Revision 7.6  1996/07/18 04:03:18  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/07 17:59:38  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/03/18 16:04:45  jims
 * Port to cfront template mechanism
 *
 * Revision 7.3  1996/03/16 15:15:37  jims
 * Port non-stdlib template persistence to DEC and Watcom
 *
 * Revision 7.2  1996/01/27 01:36:34  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.1  1995/11/13 12:30:40  jims
 * Move to revision 7.1
 *
 * Revision 7.3  1995/09/05 20:55:16  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:48:15  jims
 *  Scopus TAN ID# 82: Use RWTExport for using templates with Tools DLL
 *  under MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:06:53  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/18  19:04:25  jims
 * Add copyright
 *
 * Revision 6.2  1994/07/05  21:56:42  nevis
 * Fixes bug #117 by defining explicitly a copy constructor.
 *
 * Revision 6.3  1994/07/05  21:05:30  nevis
 * Fixed typo.
 *
 * Revision 6.2  1994/07/05  20:57:22  nevis
 * Fixes bug #117 by defining copy constructor for SGI compiler..
 *
 * Revision 6.1  1994/04/15  19:48:02  vriezen
 * Move all files to 6.1
 *
 * Revision 2.12  1994/01/12  03:36:46  jims
 * Add const to parameter of findLeftVal
 *
 * Revision 2.11  1994/01/12  03:09:17  jims
 * Add constness to T* parameters where appropriate
 *
 * Revision 2.10  1993/12/31  00:25:26  jims
 * Left justify pre-preprocessor directives
 *
 * Revision 2.9  1993/11/08  13:37:24  jims
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
 * Revision 2.2  1993/01/27  21:39:43  keffer
 * Now uses separate definitions file
 *
 * 
 */

#include "rw/tislist.h"

template <class TP> class RWTExport RWTPtrSlistIterator;

/****************************************************************
 *								*
 *		Declarations for RWTPtrSlink<TP>		*
 *								*
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

/*
 * This is the actual link that is stored in the linked list.
 * It includes data of type "TP".
 */

template <class TP> class RWTExport RWTPtrSlink : public RWIsvSlink
{

public:

  RWTPtrSlink(TP* a) : info_(a) {;}
  RWTPtrSlink<TP>*	next() const	{return (RWTPtrSlink<TP>*)next_;}

  TP*		info_;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *								*
 *	Declarations for RWTPtrSlist<TP>			*
 *								*
 ****************************************************************/

template <class TP> class RWTExport RWTPtrSlist 
                                 : private RWTIsvSlist< RWTPtrSlink<TP> >
{
public:

  RWTPtrSlist() {;}
  RWTPtrSlist(const RWTPtrSlist<TP>&);
  ~RWTPtrSlist() { clear(); }

  // Operators:
  RWTPtrSlist<TP>&	operator=(const RWTPtrSlist<TP>&);

  TP*&		operator[](size_t i)
	{return RWTIsvSlist<RWTPtrSlink<TP> >::at(i)->info_;}
  TP*const&	operator[](size_t i) const
	{return RWTIsvSlist<RWTPtrSlink<TP> >::at(i)->info_;}
  
  // Member functions:
  void		append(TP* a);

  void		apply(void (*applyFun)(TP*, void*), void*);

  TP*&		at(size_t i)
	{return RWTIsvSlist<RWTPtrSlink<TP> >::at(i)->info_;}
  TP*const&	at(size_t i) const
	{return RWTIsvSlist<RWTPtrSlink<TP> >::at(i)->info_;}

  void		clear()
	{RWTIsvSlist<RWTPtrSlink<TP> >::clearAndDestroy();}

  void		clearAndDestroy();

  RWBoolean	contains(const TP* a) const;
  RWBoolean	contains(RWBoolean (*testFun)(TP*, void*), void*) const;

  size_t	entries() const
	{return RWTIsvSlist<RWTPtrSlink<TP> >::entries();}

  TP*		find(const TP* p) const;
  TP*		find(RWBoolean (*testFun)(TP*, void*), void* d) const;

  TP*&		first()      	{ return firstLink()->info_; }
  TP*const&	first() const	{ return firstLink()->info_; }

  TP*		get()
	{return peel(RWTIsvSlist<RWTPtrSlink<TP> >::get());}

  size_t	index(const TP* a) const;
  size_t	index(RWBoolean(*testFun)(TP*, void*), void*) const;

  void		insert(TP* a);

  void		insertAt(size_t i, TP* a);

  RWBoolean	isEmpty() const
	{return RWTIsvSlist<RWTPtrSlink<TP> >::isEmpty();}

  TP*&		last()      	{ return lastLink()->info_; }
  TP*const&	last() const	{ return lastLink()->info_; }

  size_t	occurrencesOf(const TP* a) const;
  size_t	occurrencesOf(RWBoolean (*testFun)(TP*, void*), void*) const;

  void		prepend(TP* a);

  TP*		remove(const TP* p);
  TP*		remove(RWBoolean (*testFun)(TP*, void*), void* d);

  TP*		removeAt(size_t i)
	{return peel(RWTIsvSlist<RWTPtrSlink<TP> >::removeAt(i));}

  TP*		removeFirst()
	{return peel(RWTIsvSlist<RWTPtrSlink<TP> >::removeFirst());}

  TP*		removeLast()
	{return peel(RWTIsvSlist<RWTPtrSlink<TP> >::removeLast()); }

  size_t	removeAll(const TP* a);
  size_t	removeAll(RWBoolean (*testFun)(TP*, void*), void*);

protected:

  RWTPtrSlink<TP>*	findLeftVal(const TP*) const;
  RWTPtrSlink<TP>*	findLeftFun(RWBoolean (*testFun)(TP*, void*), void*) const;

  static TP*		peel(RWTPtrSlink<TP>* link);	// return "info_"; delete link

private:

friend class RWTExport RWTPtrSlistIterator<TP>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};

#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTPtrSlist)

#ifdef RW_NO_TEMPLINST_ON_BASE
class RWExport RWpistream; class RWExport RWpostream;
class RWExport RWbistream; class RWExport RWbostream; 
class RWExport RWeistream; class RWExport RWeostream;
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrSlist,RWpistream,RWpostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrSlist,RWbistream,RWbostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrSlist,RWeistream,RWeostream)
#endif     


/****************************************************************
 *								*
 *	Declarations for RWTPtrSlistIterator<TP>		*
 *								*
 ****************************************************************/

template <class TP> class RWTExport RWTPtrSlistIterator :
	            private RWTIsvSlistIterator<RWTPtrSlink<TP> >
{

public:

  RWTPtrSlistIterator(RWTPtrSlist<TP>& s) :
	RWTIsvSlistIterator<RWTPtrSlink<TP> >(s) {;}

  RWTPtrSlistIterator(const RWTPtrSlistIterator<TP>&);
	// Copy ctor added for BUG#117

  // Operators:
  RWBoolean	operator++()
	{this->advance(); return cursor()!=container()->tailLink();}
  RWBoolean	operator+=(size_t n)
	{return RWTIsvSlistIterator<RWTPtrSlink<TP> >::operator+=(n)!=rwnil;}
  TP*		operator()()
	{this->advance(); return cursor()==container()->tailLink() ? rwnil : cursor()->info_;}

  // Methods
  RWTPtrSlist<TP>*	container() const
	{return (RWTPtrSlist<TP>*)RWTIsvSlistIterator<RWTPtrSlink<TP> >::container();}

  TP*		findNext(const TP* p);
  TP*		findNext(RWBoolean (*testFun)(TP*, void*), void* d);

  void		insertAfterPoint(TP* a);

  TP*		key() const
	{return this->isActive() ? cursor()->info_ : rwnil;}

  TP*		remove();

  TP*		removeNext(const TP*);
  TP*		removeNext(RWBoolean (*testFun)(TP*, void*), void*);

  void		reset()
	{RWTIsvSlistIterator<RWTPtrSlink<TP> >::reset();}
  void		reset(RWTPtrSlist<TP>& s)
	{RWTIsvSlistIterator<RWTPtrSlink<TP> >::reset(s);}

private:

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean		operator++(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

template <class TP, class S>
void rwSaveGuts(S& str, const RWTPtrSlist<TP>& c);

template <class TP, class S>
void rwRestoreGuts(S& str, RWTPtrSlist<TP>& c);


#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xpslist.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xpslist.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXPSLIST_H__ */
