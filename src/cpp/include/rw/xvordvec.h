#ifndef __RWXVORDVEC_H__
#define __RWXVORDVEC_H__

/*
 * RWTValOrderedVector<T>: Ordered collection of objects of type T.
 *
 * $Id: xvordvec.h 49314 2006-09-27 10:48:14Z sih $
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
 *   - well-defined equality semantics (T::operator==(const T&)).
 *
 * Note that while these are automatically defined for builtin types
 * (such as "int", "double", or any pointer), you may need to provide
 * appropriate operators for your own classes, particularly those with
 * constructors and/or pointers to other objects.
 *
 ***************************************************************************
 */

#include "rw/tvvector.h"
#include "rw/rwerr.h"
#include "rw/toolerr.h"

/****************************************************************
 *								*
 *	Declarations for RWTValOrderedVector<T>			*
 *								*
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T> class RWTExport RWTValOrderedVector : private RWTValVector<T>
{

public:

  //Constructors:
  RWTValOrderedVector(size_t capac=RWDEFAULT_CAPACITY)
    : RWTValVector<T>(capac),
      nitems_(0)
  {;}

  // Destructor
  virtual ~RWTValOrderedVector() {}

  //Operators: 
  T&		operator[](size_t i)
    { boundsCheck(i); return RWTValVector<T>::operator()(i); }

  const T&	operator[](size_t i) const
    { boundsCheck(i); return RWTValVector<T>::operator()(i); }

  T&		operator()(size_t i)       {
#ifdef RWBOUNDS_CHECK
    boundsCheck(i);
#endif
    return RWTValVector<T>::operator()(i);
  }

  const T&	operator()(size_t i) const {
#ifdef RWBOUNDS_CHECK
    boundsCheck(i);
#endif
    return RWTValVector<T>::operator()(i);
  }


  //Member functions:
  void			append(const T& val)	{insertAt(nitems_,val);}
  T&			at(size_t i)		{return (*this)[i];}
  const T&		at(size_t i) const	{return (*this)[i];}
  void			clear();
  RWBoolean		contains(const T& a) const {return index(a) != RW_NPOS;}
  const T*		data() const            {return RWTValVector<T>::data();}
  size_t		entries() const		{return nitems_;}
  RWBoolean		find(const T& p, T& ret) const;
  T&		        first()      		{return (*this)(0);}
  const T&		first() const		{return (*this)(0);}
  virtual size_t	index(const T& p) const;
  virtual void		insert(const T& val)	{insertAt(nitems_,val);}
  void			insertAt(size_t, const T&);
  RWBoolean		isEmpty() const		{return nitems_==0;}
  T&			last()      		{return (*this)(nitems_-1);}
  const T&		last() const		{return (*this)(nitems_-1);}
  size_t		length() const		{return nitems_;}
  virtual size_t	occurrencesOf(const T&) const;
  void			prepend(const T& val)	{insertAt(0, val);}
  virtual RWBoolean	remove(const T& p);
  virtual size_t	removeAll(const T&);
  T			removeAt(size_t);
  T			removeFirst()		{return removeAt(0);}
  T			removeLast();
  void			resize(size_t N)	{if(N>RWTValVector<T>::length()) this->reshape(N);}

protected:
  void			boundsCheck(size_t i) const;

  size_t		nitems_;	// Number of items in the collection
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};


#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTValOrderedVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
class RWExport RWpistream; class RWExport RWpostream;
class RWExport RWbistream; class RWExport RWbostream; 
class RWExport RWeistream; class RWExport RWeostream;
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValOrderedVector,RWpistream,RWpostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValOrderedVector,RWbistream,RWbostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTValOrderedVector,RWeistream,RWeostream)
#endif     


template <class T, class S>
void rwSaveGuts(S& str, const RWTValOrderedVector<T>& c);

template <class T, class S>
void rwRestoreGuts(S& str, RWTValOrderedVector<T>& c);

template<class T>
RWBoolean rwCompareEqual(const RWTValOrderedVector<T>& lhs, const RWTValOrderedVector<T>& rhs);

//make sure Serialize.h++ doesn't define this function again.

#ifndef RW_TVECTOR_EQUALITY_DECLARED
#  define RW_TVECTOR_EQUALITY_DECLARED 1
#endif

template <class T>
inline RWBoolean operator==(const RWTValOrderedVector<T>& lhs, const RWTValOrderedVector<T>& rhs)
{
  return ((lhs.length() == rhs.length()) && rwCompareEqual(lhs, rhs));
}


#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xvordvec.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xvordvec.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXVORDVEC_H__ */
