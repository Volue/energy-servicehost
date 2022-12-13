#ifndef __RWTVRTARRY_H__
#define __RWTVRTARRY_H__

/*
 * RWTValVirtualArray<T>: A swapping virtual array of type T
 *
 * $Id: tvrtarry.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:44  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.7  1996/07/18 04:13:33  hart
 * ObjectStore changes
 *
 * Revision 7.6  1996/05/07 19:04:05  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.5  1996/02/18 01:49:29  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.4  1996/02/16 21:08:38  kyle
 * Millenium fixed spelling of "Symantec"
 *
 * Revision 7.3  1996/01/29 20:43:07  kyle
 * Millenium added support for Symantec Mac.  Moved a template declaration
 * to the top of RWTValVirtualArray because it needs it to be there and
 * changed how values in RWTVirtualSlice and RWTVirtualElement are returned
 * to accomodate compiler.
 *
 * Revision 7.2  1995/09/05 21:28:41  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  03:10:45  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:48:29  vriezen
 * Move all files to 6.1
 *
 * Revision 2.9  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.8  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.7  1993/02/17  18:32:03  keffer
 * Now passes T's by const reference, rather than by value
 *
 * Revision 2.6  1993/02/12  01:34:44  keffer
 * Ported to the IBM xlC compiler
 *
 * Revision 2.5  1993/02/07  23:58:09  keffer
 * Ported back to BCC3.1
 *
 * Revision 2.4  1993/02/07  23:37:31  keffer
 * Changed include line for compile-time instantiation.
 *
 * Revision 2.3  1993/02/06  02:06:43  keffer
 * Rearranged things to port to cfront V3.0.
 *
 *    Rev 1.0   11 Mar 1992 14:10:00   KEFFER
 * Initial revision.
 */

#include "rw/tvref.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class T> class RWTVirtualElement;
template <class T> class RWTVirtualSlice;

/****************************************************************
 *                                                              *
 *              Declaration of RWTValVirtualArray<T>            *
 *                                                              *
 ****************************************************************/

template <class T> class RWTValVirtualArray
{

public:

// jvd  Symantec Mac  wants these to be defined here ... not later.
  T        val(long i) const     // Readonly access
                {return vref_->val(i);}
  void  set(long i, const T& v)         // Set a value
                {cow(); vref_->set(i, v);}
                                
  RWTValVirtualArray(long size, RWVirtualPageHeap*);
  RWTValVirtualArray(const RWTValVirtualArray<T> &);
  RWTValVirtualArray(const RWTVirtualSlice<T> &);
  ~RWTValVirtualArray();

  RWTValVirtualArray<T>& operator=(const RWTValVirtualArray<T>&);
  void                  operator=(const RWTVirtualSlice<T>&);
  T                     operator=(const T&);

  long                  length() const                  // Length of vector
                                {return vref_->length();}
  T                     operator[](long i) const        // Readonly access
                                {return vref_->val(i);}
  inline RWTVirtualElement<T>
                        operator[](long);       // Element as lvalue
  inline RWTVirtualSlice<T>
                        slice(long start, long length); // Slice as lvalue
  void                  reshape(long newLength)
                                {cow(); vref_->reshape(newLength);}
  RWVirtualPageHeap*    heap() const
                                {return vref_->heap();}

private:

  RWTVirtualRef<T>*     vref_;

  void                  cow();          // Copy On Write
friend class RWTVirtualElement<T>;
friend class RWTVirtualSlice<T>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


/****************************************************************
 *                                                              *
 *              Declaration of RWTVirtualElement<T>             *
 *                                                              *
 ****************************************************************/

template <class T> class RWTVirtualElement
{

public:

  // These workarounds necessary in order to satisfy both Borland
  // and cfront V3.0 ---
#ifdef RW_BROKEN_TEMPLATES
  inline operator       T() const;
  inline T              operator=(const T& v);
#else
  operator      T() const             {return varray_->val(start_);}
  T             operator=(const T& v) {varray_->set(start_, v); return v;}
#endif

protected:

  // Protected constructor:
  RWTVirtualElement
  (
    RWTValVirtualArray<T>* v,
    long s
  ) : varray_(v), start_(s) { }

  RWTValVirtualArray<T>*        varray_;
  long                          start_;

private:

friend class RWTValVirtualArray<T>;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};


/****************************************************************
 *                                                              *
 *              Declaration of RWTVirtualSlice<T>               *
 *                                                              *
 ****************************************************************/

template <class T> class RWTVirtualSlice : public RWTVirtualElement<T>
{

public:

  void                  operator=(const RWTVirtualSlice<T>&);
  void                  operator=(const RWTValVirtualArray<T>&);
  T                     operator=(const T& val);

protected:

  // Protected constructor:
  RWTVirtualSlice
  (
    RWTValVirtualArray<T>* v,
    long s,                     // Slice start
    long e                      // Slice extent
  ) : RWTVirtualElement<T>(v,s),
      extent_(e)
  {;}

  long                  extent_;

private:

friend class RWTValVirtualArray<T>;
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};

/****************************************
 *                                      *
 *              INLINES                 *
 *                                      *
 ****************************************/

template <class T>
inline RWTVirtualElement<T> RWTValVirtualArray<T>::operator[](long i)
{
        RWTVirtualElement<T> value(this,i);
        return value;
}

template <class T>
inline RWTVirtualSlice<T> RWTValVirtualArray<T>::slice(long start, long length)
{
        RWTVirtualSlice<T> value(this,start,length);
        return value;
}

#ifdef RW_BROKEN_TEMPLATES
template <class T>
inline RWTVirtualElement<T>::operator T() const
{
  return varray_->val(start_);
}

template <class T>
inline T RWTVirtualElement<T>::operator=(const T& v)
{
  varray_->set(start_, v); return v;
}
#endif

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("tvrtarry.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/tvrtarry.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWTVRTARRY_H__ */
