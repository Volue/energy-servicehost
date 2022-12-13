#ifndef __RWXPORDVEC_H__
#define __RWXPORDVEC_H__

/*
 * RWTPtrOrderedVector<TP>: Ordered collection of pointers to objects of type TP.
 *
 * $Id: xpordvec.h 18623 2000-03-03 08:28:13Z sih $
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
 * Stores a pointer to the inserted item into the collection.
 *
 * Assumes that TP has:
 *   - well-defined equality semantics (TP::operator==(const TP&)).
 *
 ***************************************************************************
 */

#include "rw/tpvector.h"
#include "rw/rwerr.h"
#include "rw/toolerr.h"

/****************************************************************
 *								*
 *	Declarations for RWTPtrOrderedVector<TP>		*
 *								*
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class TP> class RWTExport RWTPtrOrderedVector : private RWTPtrVector<TP>
{

public:

  //Constructors:
  RWTPtrOrderedVector(size_t capac=RWDEFAULT_CAPACITY)
    : RWTPtrVector<TP>(capac),
      nitems_(0)
  {;}

  TP*&		operator()(size_t i)       {
#ifdef RWBOUNDS_CHECK
    boundsCheck(i);
#endif
    return RWTPtrVector<TP>::operator()(i);
  }
  TP*const&	operator()(size_t i) const {
#ifdef RWBOUNDS_CHECK
    boundsCheck(i);
#endif
    return RWTPtrVector<TP>::operator()(i);
  }

  TP*&		operator[](size_t i)
    {boundsCheck(i); return RWTPtrVector<TP>::operator()(i);}
  TP*const&	operator[](size_t i) const
    {boundsCheck(i); return RWTPtrVector<TP>::operator()(i);}

  //Member functions:
  void			append(TP* val)		{insertAt(nitems_,val);}
  TP*&			at(size_t i)		{return (*this)[i];}
  TP*const&		at(size_t i) const	{return (*this)[i];}
  void			clear()
  { nitems_=0;
	if (RWTPtrVector<TP>::length() > RWDEFAULT_CAPACITY)
	  this->reshape(RWDEFAULT_CAPACITY);
  }
  void			clearAndDestroy();
  RWBoolean		contains(const TP* a) const {return index(a) != RW_NPOS;}
  TP*const*		data() const		{return RWTPtrVector<TP>::data();}
  size_t		entries() const		{return nitems_;}
  TP*			find(const TP* p) const;
  TP*&			first()      		{return (*this)(0);}
  TP*const&		first() const		{return (*this)(0);}
  virtual size_t	index(const TP* p) const;
  virtual void		insert(TP* val)		{insertAt(nitems_,val);}
  void			insertAt(size_t, TP*);
  RWBoolean		isEmpty() const		{return nitems_==0;}
  TP*&		        last()   		{return (*this)(nitems_-1);}
  TP*const&		last() const		{return (*this)(nitems_-1);}
  size_t		length() const		{return nitems_;}
  virtual size_t	occurrencesOf(const TP*) const;
  void			prepend(TP* val)	{insertAt(0, val);}
  virtual TP*		remove(const TP* p);
  virtual size_t	removeAll(const TP*);
  TP*			removeAt(size_t);
  TP*			removeFirst()		{return removeAt(0);}
  TP*			removeLast()
    { TP* ret = last(); --nitems_; return ret; }

  void			resize(size_t N)	{if(N>=nitems_) this->reshape(N);}

protected:
  void			boundsCheck(size_t i) const;

  size_t		nitems_;	// Number of items in the collection

  RW_T_TYPESPEC  /* This macro is usually expands to nothing */
};

#include "rw/edefs.h"
RWDECLARE_PERSISTABLE_TEMPLATE(RWTPtrOrderedVector)

#ifdef RW_NO_TEMPLINST_ON_BASE
class RWExport RWpistream; class RWExport RWpostream;
class RWExport RWbistream; class RWExport RWbostream; 
class RWExport RWeistream; class RWExport RWeostream;
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrOrderedVector,RWpistream,RWpostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrOrderedVector,RWbistream,RWbostream)
RWDECLARE_PERSIST_TEMPLATE_IO(RWTPtrOrderedVector,RWeistream,RWeostream)
#endif     

template <class T, class S>
void rwSaveGuts(S& str, const RWTPtrOrderedVector<T>& c);

template <class T, class S>
void rwRestoreGuts(S& str, RWTPtrOrderedVector<T>& c);

template<class T>
RWBoolean rwDerefCompareEqual(const RWTPtrOrderedVector<T>& lhs, const RWTPtrOrderedVector<T>& rhs);

//make sure Serialize.h++ doesn't define this function again.
#ifndef RW_TVECTOR_EQUALITY_DECLARED
#  define RW_TVECTOR_EQUALITY_DECLARED 1
#endif

template <class T>
inline RWBoolean operator==(const RWTPtrOrderedVector<T>& lhs, const RWTPtrOrderedVector<T>& rhs)
{
  return ((lhs.length() == rhs.length()) && rwDerefCompareEqual(lhs, rhs));
}

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xpordvec.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xpordvec.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXPORDVEC_H__ */

