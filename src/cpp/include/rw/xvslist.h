#ifndef __RWXVSLIST_H__
#define __RWXVSLIST_H__
/*
 * RWTValSlist<T>: Singly-linked list of values of type T.
 *
 * $Id: xvslist.h 18623 2000-03-03 08:28:13Z sih $
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
 *   - well-defined copy constructor (T::T(const T&) or equiv.);
 *   - well-defined assignment operator (T::operator=(const T&) or equiv.);
 *   - well-defined equality operator (T::operator==(const T&) or equiv.)
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.7  1996/08/26 21:50:16  hart
 * HP (aCC) compiler port
 *
 * Revision 7.6  1996/07/18 04:03:18  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/07 17:39:10  kyle
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
 * Revision 7.3  1995/09/05  21:31:49  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:49:52  jims
 *  Scopus TAN ID# 82: Use RWTExport for using templates with Tools DLL
 * under MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:11:47  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/18  19:04:25  jims
 * Add copyright
 *
 * Revision 6.2  1994/07/05  21:56:54  nevis
 * Fixes bug #117 by defining explicitly a copy constructor.
 *
 * Revision 6.2  1994/07/05  20:57:33  nevis
 * Fixes bug #117 by defining copy constructor for SGI compiler..
 *
 * Revision 6.1  1994/04/15  19:48:35  vriezen
 * Move all files to 6.1
 *
 * Revision 2.10  1993/11/08  21:14:01  jims
 * Port to ObjectStore
 *
 * Revision 2.9  1993/09/10  02:18:28  keffer
 * apply() now takes a function with a non-const reference, allowing
 * modification of the contained value.
 *
 * Revision 2.8  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.7  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.6  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 2.5  1993/02/12  00:18:50  keffer
 * Ported to the IBM xlC compiler
 *
 * Revision 2.4  1993/01/27  21:39:46  keffer
 * Now uses separate definitions file
 *
 * Revision 2.3  1993/01/23  00:32:27  keffer
 * Performance enhancements; simplified; flatter inheritance tree.
 *
 *
 */

#include "rw/tislist.h"

template <class T> class RWTExport RWTValSlistIterator;

/****************************************************************
 *								*
 *		Declarations for RWTValSlink<T>			*
 *								*
 ****************************************************************/

/*
 * This is the actual link that is stored in the linked list.
 * It includes data of type "T".
 */
 
#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T> class RWTExport RWTValSlink : public RWIsvSlink
{

public:

  RWTValSlink(const T& a) : info_(a) {;}
  RWTValSlink<T>* next() const	{return (RWTValSlink<T>*)next_;}

  T		info_;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *								*
 *		Declarations for RWTValSlist<T>			*
 *								*
 ****************************************************************/

template <class T> class RWTExport RWTValSlist 
                             : private RWTIsvSlist< RWTValSlink<T> >
{

public:

  RWTValSlist() {;}
  RWTValSlist(const RWTValSlist<T>&);
  ~RWTValSlist() {clear();}

  // Operators:
  RWTValSlist<T>&	operator=(const RWTValSlist<T>&);

  T&		operator[](size_t i)
	{return RWTIsvSlist<RWTValSlink<T> >::at(i)->info_;}
  const T&	operator[](size_t i) const
	{return RWTIsvSlist<RWTValSlink<T> >::at(i)->info_;}

  // Member functions:
  void		append(const T& a);

  void		apply(void (*applyFun)(T&, void*), void*);

  T&		at(size_t i)
	{return RWTIsvSlist<RWTValSlink<T> >::at(i)->info_;}
  const T&	at(size_t i) const
	{return RWTIsvSlist<RWTValSlink<T> >::at(i)->info_;}

  void		clear()
	{RWTIsvSlist<RWTValSlink<T> >::clearAndDestroy();}

  RWBoolean	contains(const T& a) const;
  RWBoolean	contains(RWBoolean (*testFun)(const T&, void*), void*) const;

  size_t	entries() const
	{return RWTIsvSlist<RWTValSlink<T> >::entries();}

  RWBoolean	find(const T& a, T& ret) const; // Find first; return in "ret"
  RWBoolean	find(RWBoolean (*testFun)(const T&, void*), void*, T& ret) const;

  T&	        first()        {return firstLink()->info_;}
  const T&	first() const  {return firstLink()->info_;}
	

  T		get()
	{return peel(RWTIsvSlist<RWTValSlink<T> >::get());}

  size_t	index(const T& a) const;
  size_t	index(RWBoolean (*testFun)(const T&, void*), void*) const;

  void		insert(const T& a);

  void		insertAt(size_t i, const T& a);

  RWBoolean	isEmpty() const
	{return RWTIsvSlist<RWTValSlink<T> >::isEmpty();}

  T&	        last()        {return lastLink()->info_;}
  const T&	last() const  {return lastLink()->info_;}
	

  size_t	occurrencesOf(const T& a) const;
  size_t	occurrencesOf(RWBoolean (*testFun)(const T&, void*), void*) const;

  void		prepend(const T& a);

  RWBoolean	remove(const T& val);
  RWBoolean	remove(RWBoolean (*testFun)(const T&, void*), void*);

  size_t	removeAll(const T& a);
  size_t	removeAll(RWBoolean (*testFun)(const T&, void*), void*);

  T		removeAt(size_t i)
	{return peel(RWTIsvSlist<RWTValSlink<T> >::removeAt(i));}

  T		removeFirst()
	{return peel(RWTIsvSlist<RWTValSlink<T> >::removeFirst());}

  T		removeLast()
	{return peel(RWTIsvSlist<RWTValSlink<T> >::removeLast());}

protected:

  RWTValSlink<T>*	findLeftVal(const T&) const;
  RWTValSlink<T>*	findLeftFun(RWBoolean (*testFun)(const T&, void*), void*) const;

  static T		peel(RWTValSlink<T>* link);

private:

  friend class RWTExport RWTValSlistIterator<T> ;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};

#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTValSlist)

#ifdef RW_NO_TEMPLINST_ON_BASE
class RWExport RWpistream; class RWExport RWpostream;
class RWExport RWbistream; class RWExport RWbostream; 
class RWExport RWeistream; class RWExport RWeostream;
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValSlist,RWpistream,RWpostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValSlist,RWbistream,RWbostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValSlist,RWeistream,RWeostream)
#endif     



/****************************************************************
 *								*
 *		Declarations for RWTValSlistIterator<T>		*
 *								*
 ****************************************************************/

template <class T> class RWTExport RWTValSlistIterator :
                   private RWTIsvSlistIterator< RWTValSlink<T> >
{

public:

  RWTValSlistIterator(RWTValSlist<T>& s) :
	RWTIsvSlistIterator<RWTValSlink<T> >(s) {;}

  RWTValSlistIterator(const RWTValSlistIterator<T>&);
    // Copy ctor added to address bug #117

  // Operators:
  RWBoolean	operator++()
	{this->advance(); return cursor()!=container()->tailLink();}
  RWBoolean	operator+=(size_t n)
	{return RWTIsvSlistIterator<RWTValSlink<T> >::operator+=(n)!=rwnil;}
  RWBoolean	operator()()
	{this->advance(); return cursor()!=container()->tailLink();}

  // Methods
  RWTValSlist<T>*	container() const
	{return (RWTValSlist<T>*)RWTIsvSlistIterator<RWTValSlink<T> >::container();}

  RWBoolean		findNext(const T& a);
  RWBoolean		findNext(RWBoolean (*testFun)(const T&, void*), void*);

  void			insertAfterPoint(const T& a);

  T			key() const
	{return cursor()->info_;}

  RWBoolean		remove();	// Remove item at cursor

  RWBoolean		removeNext(const T&);
  RWBoolean		removeNext(RWBoolean (*testFun)(const T&, void*), void*);

  void			reset()
	{RWTIsvSlistIterator<RWTValSlink<T> >::reset();}
  void			reset(RWTValSlist<T>& s)
	{RWTIsvSlistIterator<RWTValSlink<T> >::reset(s);}

private:

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean		operator++(int);

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


template <class T, class S>
void rwRestoreGuts(S& str, RWTValSlist<T>& c);

#ifndef RW_NO_TEMPLINST_ON_BASE

template <class T>
void rwSaveGuts(RWvostream& str, const RWTValSlist<T>& c);

template <class T>
void rwSaveGuts(RWFile& str, const RWTValSlist<T>& c);

#else

template <class T, class S>
void rwSaveGuts(S& str, const RWTValSlist<T>& c);

#endif


#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xvslist.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xvslist.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXVSLIST_H__ */
