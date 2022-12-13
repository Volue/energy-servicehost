#ifndef __RWEV_CNTNR_H__
#define __RWEV_CNTNR_H__

/***************************************************************************
 *
 * ev_cntnr.h - RW_VBase<StdColl,RWColl>
 *   
 *     : value-based interface components for STL Containers
 * 
 * $Id: ev_cntnr.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************/

#include "rw/compiler.h"
#include "rw/defs.h"
#include "rw/rwerr.h"
#include "rw/toolerr.h"

#ifndef RW_HEADER_WITH_EXTENSION
# include <algorithm>
# include <iterator>
#else
#  include <algorithm.hpp>
#  include <iterator.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

template <class StdColl, class RWColl>
class RW_VBase {
public:
  typedef typename StdColl::iterator                 my_iter_;
  typedef typename StdColl::const_iterator           const_my_iter_;
  typedef typename StdColl::size_type                size_type;
  typedef typename StdColl::value_type               value_type;
  typedef typename StdColl::reference                reference;
  typedef typename StdColl::const_reference          const_reference;

#ifdef RW_NO_TYPENAME
typedef RW_VBase<StdColl, RWColl>::const_reference RW_VBase_SR_const_reference;
typedef RW_VBase<StdColl, RWColl>::reference RW_VBase_SR_reference;
#endif

/* The following is not supported by set -- shouldn't it be? */
//  typedef StdColl::pointer pointer

  StdColl&        std()       { return ((RWColl*) this)->impl_; }
  const StdColl&  std() const { return ((const RWColl*) this)->impl_; }

// Allow easy access to iterators:
  my_iter_        begin()         { return std().begin(); }
  my_iter_        end()           { return std().end();   }

  const_my_iter_  begin() const   { return std().begin(); }
  const_my_iter_  end()   const   { return std().end();   }

// RW interface: public member functions
  size_type       entries() const { return std().size(); }
  bool            isEmpty() const { return std().empty(); }

///// public methods
  const_reference at(size_type index) const { return (*this)[index]; }
  reference       at(size_type index)       { return (*this)[index]; }
    
#if !defined(__SUNPRO_CC) || (__SUNPRO_CC<0x420 && !defined (RW_NO_STL))
  void  apply(void (*fn)(const_reference,void*),void *x) const;

  void  apply(void (*fn)(reference,void*),void *x);
    
  bool          contains(const_reference) const;

  bool          contains(
                        bool(*fn)(const_reference,void*),
                        void* x
                        ) const;
#else
#ifdef RW_NO_TYPENAME
  bool    contains( bool (*fn)(RW_VBase_SR_const_reference,void*), void* x ) const
#else
  bool    contains( bool (*fn)(typename  RW_VBase<StdColl, RWColl>::const_reference,void*), void* x ) const
#endif
  { 
    const_my_iter_ i = begin();
    for(/**/ ; i != end(); ++i)
    if((*fn)(*i,x))
      break;
     return i != end();
   }

  bool contains(const_reference key) const
   {
     const_my_iter_ i = begin();
     for(/**/ ; i != end(); ++i)
     if(*i == key)
      break;
     return i != end();
    }
 
#ifdef RW_NO_TYPENAME
  void apply( void (*fn)(RW_VBase_SR_const_reference,void*), void* x ) const
#else
  void apply( void (*fn)(typename RW_VBase<StdColl, RWColl>::const_reference,void*), void* x ) const
#endif
   {
     for(const_my_iter_ i= begin(); i != end(); ++i) {
     (*fn)(*i,x);
     }
   }

#ifdef RW_NO_TYPENAME
  void apply( void (*fn)(RW_VBase_SR_reference,void*), void* x ) 
#else
  void apply( void (*fn)(typename RW_VBase<StdColl, RWColl>::reference,void*), void* x ) 
#endif
   {
     for(my_iter_ i= begin(); i != end(); ++i) {
     (*fn)(*i,x);
     }
   }
#endif /* SunPro */
    
///// operators
  const_reference operator[](size_type index) const
  {
    boundsCheck(index);
    const_my_iter_ i = begin();
    RW_SL_STD(advance)(i,index);
    return *i;
  }

  reference operator[](size_type index) 
  {
    boundsCheck(index);
    my_iter_ i = begin();
    RW_SL_STD(advance)(i,index);
    return (reference) *i;
  }

  const_reference operator()(size_type index) const
  {
    RWPRECONDITION(index < entries()); 
    const_my_iter_ i = begin();
    RW_SL_STD(advance)(i,index);
    return *i;
  }

  reference operator()(size_type index) 
  {
    RWPRECONDITION(index < entries()); 
    my_iter_ i = begin();
    RW_SL_STD(advance)(i,index);
    return (reference) *i;
  }

  bool    operator==(const RWColl& rwc)   const { return std() == rwc.std(); }
  bool    operator< (const RWColl& rwc)   const { return std() < rwc.std(); }
  bool    operator==(const StdColl& stdc) const { return std() == stdc; }
  bool    operator< (const StdColl& stdc) const { return std() < stdc;  }

  const_reference first() const
    { RWPRECONDITION(!isEmpty()); return *(begin()); }

  reference       first()
    { RWPRECONDITION(!isEmpty()); return (reference) *(begin()); }
#if !defined(__SUNPRO_CC) || (__SUNPRO_CC<0x420 && !defined (RW_NO_STL))
  size_type index(bool (*fn)(const_reference, void*),
                           void* x) const;

  size_type     index(const_reference key) const;

  size_type     occurrencesOf(const_reference) const;

  size_type     occurrencesOf(bool (*fn)(const_reference,void*),void* x) const;
#else
#ifdef RW_NO_TYPENAME
size_type occurrencesOf( bool (*fn)(RW_VBase_SR_const_reference,void*), void* x ) const
#else
  size_type occurrencesOf( bool (*fn)(typename RW_VBase<StdColl, RWColl>::const_reference,void*), void* x ) const
#endif
    {
      size_type ret(0);
      const_my_iter_ i = begin();
      for(/**/; i != end(); ++i)
      if((*fn)(*i,x))
      ++ret;
      return ret;
    }

  size_type occurrencesOf(const_reference key) const
    {
      size_type ret(0);
      const_my_iter_ i = begin();
      for(/**/; i != end(); ++i)
      if(*i == key)
      ++ret;
      return ret;
    }

#ifdef RW_NO_TYPENAME
  size_type index(bool (*fn)(RW_VBase_SR_const_reference, void*), void* x) const
#else
 size_type index(bool (*fn)(typename RW_VBase<StdColl, RWColl>::const_reference, void*), void* x) const
#endif
    {
      const_my_iter_ i = begin();
      size_type idx(0);
      for(/**/; i != end(); ++i) {
      if((*fn)(*i,x))
      return idx;
      ++idx;
      }
      return this->npos;
    }

size_type index(const_reference key) const
    {
      size_type ret = 0;
      const_my_iter_ i;
      for(i = begin(); i != end(); ++i)
      {
      if(*i == key) break;
      ++ret;
      }
      return i == this->end() ? this->npos : ret;
    }
#endif /* !SunPro 420 */
  static size_type npos;

protected:
// This class can only be used as a base class
// Note also that no member functions may be called during construction
//   since they all depend on the pseudo-virtual function "std()"
  RW_VBase() { }
  RW_VBase(const RW_VBase<StdColl,RWColl>&) { }

  void          boundsCheck(size_type i) const
    {
      if (i >= entries())
        RWTHROW( RWBoundsErr(RWMessage(RWTOOL_INDEXERR,
                                       (unsigned)i, (unsigned) entries()) ));
    }
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#ifndef RWDefCArgs
#  define RWDefCArgs(T) ,RW_SL_STD(less)< T >
#endif

#ifndef RWDefHArgs
#  define RWDefHArgs(T) ,RWTHasher< T >,RW_SL_STD(equal_to)< T >
#endif


#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ev_cntnr.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ev_cntnr.cc"
#endif

#endif /* __RWEV_CNTNR_H__ */
