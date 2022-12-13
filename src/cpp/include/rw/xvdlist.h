#ifndef __RWXVDLIST_H__
#define __RWXVDLIST_H__
/*
 * RWTValDlist<T>: Doubly-linked list of values of type T.
 *
 * $Id: xvdlist.h 18623 2000-03-03 08:28:13Z sih $
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
 *   - well-defined equality semantics (T::operator==(const T&)).
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.8  1996/08/26 21:51:23  hart
 * HP (aCC) compiler port
 *
 * Revision 7.7  1996/07/18 04:03:18  hart
 * ObjectStore changes
 *
 * Revision 7.6  1996/05/07 17:48:37  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.5  1996/03/21 20:29:52  jims
 * Add missing elements of Watcom Port
 *
 * Revision 7.4  1996/03/18 16:04:45  jims
 * Port to cfront template mechanism
 *
 * Revision 7.3  1996/03/16 15:15:37  jims
 * Port non-stdlib template persistence to DEC and Watcom
 *
 * Revision 7.2  1996/01/27 00:48:35  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.1  1995/11/13 12:30:40  jims
 * Move to revision 7.1
 *
 * Revision 7.3  1995/09/05 20:57:13  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:37:00  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:07:49  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:48:11  vriezen
 * Move all files to 6.1
 *
 * Revision 2.11  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.10  1993/11/08  13:49:36  jims
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
 * Revision 2.5  1993/02/12  23:33:45  keffer
 * Ported to g++ v2.3
 *
 * Revision 2.4  1993/02/12  00:18:50  keffer
 * Ported to the IBM xlC compiler
 *
 * Revision 2.3  1993/01/27  21:39:45  keffer
 * Now uses separate definitions file
 *
 * Revision 2.2  1993/01/23  00:32:27  keffer
 * Performance enhancements; simplified; flatter inheritance tree.
 *
 * 
 */

#include "rw/tidlist.h"

template <class T> class RWTExport RWTValDlistIterator;

/****************************************************************
 *								*
 *		 Declarations for RWTValDlink<T>		*
 *								*
 ****************************************************************/

/*
 * This is the actual link that is stored in the linked list.
 * It includes data of type "T".
 */
#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T> class RWTExport RWTValDlink : public RWIsvDlink
{

public:

  RWTValDlink(const T& a) : info_(a) {;}
  RWTValDlink<T>* next() const {return (RWTValDlink<T>*)next_;}
  RWTValDlink<T>* prev() const {return (RWTValDlink<T>*)prev_;}

  T		info_;
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *								*
 *		Declarations for RWTValDlist<T>			*
 *								*
 ****************************************************************/

template <class T> class RWTExport RWTValDlist 
                             : private RWTIsvDlist< RWTValDlink<T> >
{

public:

  RWTValDlist() {;}
  RWTValDlist(const RWTValDlist<T>&);
  ~RWTValDlist() {clear();}

  // Operators:
  RWTValDlist<T>&	operator=(const RWTValDlist<T>&);

  T&		operator[](size_t i)
	{return RWTIsvDlist<RWTValDlink<T> >::at(i)->info_;}
  const T&	operator[](size_t i) const
	{return RWTIsvDlist<RWTValDlink<T> >::at(i)->info_;}
  
  // Member functions:
  void		append(const T& a);

  void		apply(void (*applyFun)(T&, void*), void*);

  T&		at(size_t i)
	{return RWTIsvDlist<RWTValDlink<T> >::at(i)->info_;}
  const T&	at(size_t i) const
	{return RWTIsvDlist<RWTValDlink<T> >::at(i)->info_;}

  void		clear()
	{RWTIsvDlist<RWTValDlink<T> >::clearAndDestroy();}

  RWBoolean	contains(const T& a) const;
  RWBoolean	contains(RWBoolean (*testFun)(const T&, void*), void*) const;

  size_t	entries() const
	{return RWTIsvDlist<RWTValDlink<T> >::entries();}

  RWBoolean	find(const T& p, T& ret) const; // Find first; return in "ret"
  RWBoolean	find(RWBoolean (*testFun)(const T&, void*), void* d, T& ret) const;

  T&	        first()       { return firstLink()->info_; }
  const T&	first() const { return firstLink()->info_; }

  T		get()
	{return peel(RWTIsvDlist<RWTValDlink<T> >::get());}

  size_t	index(const T& a) const;
  size_t	index(RWBoolean(*testFun)(const T&, void*), void*) const;

  void		insert(const T& a);
  void		insertAt(size_t i, const T& a);

  RWBoolean	isEmpty() const
	{return RWTIsvDlist<RWTValDlink<T> >::isEmpty();}

  T&	        last()        { return lastLink()->info_; }
  const T&	last() const  { return lastLink()->info_; }
	
  size_t	occurrencesOf(const T& a) const;
  size_t	occurrencesOf(RWBoolean (*testFun)(const T&, void*), void*) const;

  void		prepend(const T& a);

  RWBoolean	remove(const T& a);
  RWBoolean	remove(RWBoolean (*testFun)(const T&, void*), void* d);

  size_t	removeAll(const T& a);
  size_t	removeAll(RWBoolean (*testFun)(const T&, void*), void*);

  T		removeAt(size_t i)
	{return peel(RWTIsvDlist<RWTValDlink<T> >::removeAt(i));}

  T		removeFirst()
	{return peel(RWTIsvDlist<RWTValDlink<T> >::removeFirst());}

  T		removeLast()
	{return peel(RWTIsvDlist<RWTValDlink<T> >::removeLast());}

protected:

  RWTValDlink<T>*	findVal(const T&) const;
  RWTValDlink<T>*	findFun(RWBoolean (*testFun)(const T&, void*), void*) const;

  static T		peel(RWTValDlink<T>* link);

private:

friend class RWTExport RWTValDlistIterator<T>;  

  RW_T_TYPESPEC  /* This macro usuallly expands to nothing */
};

#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTValDlist)

#ifdef RW_NO_TEMPLINST_ON_BASE
class RWExport RWpistream; class RWExport RWpostream;
class RWExport RWbistream; class RWExport RWbostream; 
class RWExport RWeistream; class RWExport RWeostream;
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValDlist,RWpistream,RWpostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValDlist,RWbistream,RWbostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValDlist,RWeistream,RWeostream)
#endif     



template <class T> class RWTExport RWTValDlistIterator :
                   private RWTIsvDlistIterator<RWTValDlink<T> >
{

public:

  RWTValDlistIterator(RWTValDlist<T>& s) :
	RWTIsvDlistIterator<RWTValDlink<T> > (s) {;}

  // Operators:
  RWBoolean	operator++()
	{this->advance(); return cursor()!=container()->tailLink();}
  RWBoolean	operator--()
	{this->backup(); return cursor()!=container()->headLink();}
  RWBoolean	operator+=(size_t n)
	{return RWTIsvDlistIterator<RWTValDlink<T> >::operator+=(n)!=rwnil;}
  RWBoolean	operator-=(size_t n)
	{return RWTIsvDlistIterator<RWTValDlink<T> >::operator-=(n)!=rwnil;}
  RWBoolean	operator()()
	{this->advance(); return cursor()!=container()->tailLink();}

  // Methods:
  RWTValDlist<T>*	container() const
	{return (RWTValDlist<T>*)RWTIsvDlistIterator<RWTValDlink<T> >::container();}

  RWBoolean		findNext(const T& p);
  RWBoolean		findNext(RWBoolean (*testFun)(const T&, void*), void* d);

  void			insertAfterPoint(const T& a);

  T			key() const
	{return cursor()->info_;}

  RWBoolean		remove();	// Remove item at cursor

  RWBoolean		removeNext(const T& p);
  RWBoolean		removeNext(RWBoolean (*testFun)(const T&, void*), void* d);

  void			reset()
	{RWTIsvDlistIterator<RWTValDlink<T> >::reset();}

  void			reset(RWTValDlist<T>& s)
	{RWTIsvDlistIterator<RWTValDlink<T> >::reset(s);}

private:

  // Disallow postfix increment & decrement.  Unless we hide it, some 
  // compilers will substitute the prefix operator in its place.
  RWBoolean		operator++(int);
  RWBoolean		operator--(int);
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


template <class T, class S>
void rwRestoreGuts(S& str, RWTValDlist<T>& c);

#ifndef RW_NO_TEMPLINST_ON_BASE

template <class T>
void rwSaveGuts(RWvostream& str, const RWTValDlist<T>& c);

template <class T>
void rwSaveGuts(RWFile& str, const RWTValDlist<T>& c);

#else

template <class T, class S>
void rwSaveGuts(S& str, const RWTValDlist<T>& c);

#endif

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xvdlist.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xvdlist.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXVDLIST_H__ */
