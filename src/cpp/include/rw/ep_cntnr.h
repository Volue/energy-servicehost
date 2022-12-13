#ifndef __RW_EPCNTNR_H__
#define __RW_EPCNTNR_H__

/***************************************************************************
 *
 * ep_cntnr.h - RW_PBase<StdColl,RWColl>
 *   
 *     : pointer-based interface components for STL Containers
 * 
 * $Id: ep_cntnr.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************
 * This class is at the base of the implementation hierarchy
 * for the pointer-based collections.  The methods here are
 * those which only need to know that StdColl is some sort of
 * an STL-compliant container, whether it holds singleton values
 * as does a vector or set, or pairs as does a map.  RWColl is the
 * Tools.h++ wrapper class that uses StdColl as its implementation.
 * RWColl must ultimately derive from RW_PBase<StdColl, RWColl>
 * and supply a member function std() which returns an instance
 * of StdColl.
 * 
 *****************************************************************************/

#include "rw/rwerr.h"
#include "rw/toolerr.h"
#include "rw/epaux.h"

/*
 * StdColl:     the STL collection used for implementation
 * RWColl:      the RW collection which wraps the above
 */

template <class StdColl, class RWColl>
class RW_PBase {
          
public:
  typedef typename StdColl::value_type        value_type;
  typedef typename StdColl::size_type         size_type;
  typedef typename StdColl::iterator          my_iter_;
  typedef typename StdColl::const_iterator    const_my_iter_;
  typedef typename StdColl::reference         reference;
  typedef typename StdColl::const_reference   const_reference;

#ifdef RW_NO_TYPENAME
  typedef RW_PBase<StdColl,RWColl>::value_type RW_PBase_SR_value_type;
  typedef RW_PBase<StdColl,RWColl>::reference RW_PBase_SR_reference;
#endif

  StdColl&        std()       { return ((RWColl*) this)->impl_; }
  const StdColl&  std() const { return ((const RWColl*) this)->impl_; }

  my_iter_             begin()             { return std().begin(); }
  my_iter_             end()               { return std().end();   }
  const_my_iter_       begin() const       { return std().begin(); }
  const_my_iter_       end()   const       { return std().end();   }

// RW interface: public member functions
  size_type       entries() const { return std().size(); }
  bool            isEmpty() const { return std().empty(); }


/*
 * Unsolved problem with msvc forces cast away of const below:
 */

  const_reference operator() (size_type idx) const
  {
#ifdef RWDEBUG
    boundsCheck(idx);
#endif
#ifndef _MSC_VER
    const_my_iter_ i = begin();
#else
    my_iter_ i = ((StdColl&)std()).begin();
#endif
    RW_SL_STD(advance)(i,idx);
    return *i;
  }

  const_reference operator[](size_type idx) const
  {
    boundsCheck(idx);
#ifndef _MSC_VER
    const_my_iter_ i = begin();
#else
    my_iter_ i = ((StdColl&)std()).begin();
#endif
    RW_SL_STD(advance)(i,idx);
    return *i;
  }

  reference operator()(size_type idx) 
  {
#ifdef RWDEBUG
    boundsCheck(idx);
#endif
    my_iter_ i = begin();
    RW_SL_STD(advance)(i,idx);
    return *i;
  }

  reference operator[](size_type idx) 
  {
    boundsCheck(idx);
    my_iter_ i = begin();
    RW_SL_STD(advance)(i,idx);
    return *i;
  }

///// public methods
#if !defined(__SUNPRO_CC) || __SUNPRO_CC<0x420
  void  apply(void (*fn)(value_type,void*),void *x);
  void  apply(void (*fn)(reference,void*),void *x);
#else

#ifdef RW_NO_TYPENAME
  void apply(void (*fn)(RW_PBase_SR_value_type,void*),void *x)
#else
  void apply(void (*fn)(typename RW_PBase<StdColl,RWColl>::value_type,void*),void *x)
#endif
    {
         my_iter_ iter = begin();
         while(iter != end()) {
         (*fn)(*iter,x);
         ++iter;
         }
    }

#ifdef RW_NO_TYPENAME
  void apply(void (*fn)(RW_PBase_SR_reference,void*),void *x)
#else
  void apply(void (*fn)(typename RW_PBase<StdColl,RWColl>::reference,void*),void *x)
#endif
    {
          my_iter_ iter = begin();
          while(iter != end()) {
          (*fn)(*iter,x);
          ++iter;
          }
    }
#endif /* !SunPro 420 */

  const_reference    at(size_type idx) const
        { return (*this)[idx]; }

  reference   at(size_type idx)
        { return (*this)[idx]; }
#if !defined(__SUNPRO_CC) || __SUNPRO_CC<0x420
  bool  contains(bool(*fn)(value_type,void*),void* x) const;
#else
#ifdef RW_NO_TYPENAME
  bool contains(bool(*fn)(RW_PBase_SR_value_type,void*),void* x) const
#else
  bool contains(bool(*fn)(typename RW_PBase<StdColl,RWColl>::value_type,void*),void* x) const
#endif
    {
          const_my_iter_ iter = begin();
          while(iter != end()) {
          if((*fn)(*iter,x)) break;
          ++iter;
          }
          return iter != end();
    }
#endif /* !SunPro 420 */
    
  const_reference first() const  { return *begin(); }
  reference       first()        { return *begin(); }



#if defined(RW_BROKEN_NAME_RESOLUTION) || (defined(__SUNPRO_CC) && __SUNPRO_CC>=0x420)
/* 
 *   The following system of macros allows a portion of a given ".cc" file
 *   to be used within a class body. This portion can alternatively be used
 *   to define templated function bodies in the normal fashion when needed.
 *   (See the second include of this same ".cc" file at the end of this 
 *   file).
 *   The remaining portion of the ".cc" file templates can only be used in the
 *   standard fashion. They are read during the include at the end of this 
 *   file.
 *   For more information see the comments at the beginning of the ".cc" file.
 *   Also, the methodology is commented in greatest detail in files slist.h and
 *   slist.cc. Study those files if further information is needed
 */
#define RW_GET_INLINES_FOR_CLASS_BODY
#include "rw/ep_cntnr.cc"
#undef RW_GET_INLINES_FOR_CLASS_BODY
#define RW_GOT_INLINES_FOR_CLASS_BODY_EP_CNTNR_CC

#else
  typename StdColl::
  size_type  index(bool (*fn)(value_type, void*), void* x) const;

  typename StdColl::
  size_type  occurrencesOf(bool (*fn)(value_type,void*),void* x) const;
#endif    // RW_BROKEN_NAME_RESOLUTION ...



  static size_type npos;


protected:
  void          boundsCheck(size_type i) const
    {
      if (!(i < std().size()) )
        RWTHROW( RWBoundsErr(RWMessage(RWTOOL_INDEXERR, i, entries() )));
    }

// Can only be used as a base class:
  RW_PBase() { }
  RW_PBase(const RW_PBase<StdColl,RWColl>&) { }
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


#ifndef RWDefCArgs
#  define RWDefCArgs(T) ,RW_SL_STD(less)< T >
#endif

#ifndef RWDefHArgs
#  define RWDefHArgs(T) ,RWTHasher< T >,RW_SL_STD(equal_to)< T >
#endif



#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ep_cntnr.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ep_cntnr.cc"
#endif

#endif /* __RW_EPCNTNR_H__ */

