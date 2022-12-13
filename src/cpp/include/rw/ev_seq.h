#ifndef __RWEV_SEQ_H__
#define __RWEV_SEQ_H__

/***************************************************************************
 *
 * ev_seq.h - RW_VSeq<StdColl,RWColl>
 *   
 *     : value-based interface components for STL Sequences
 * 
 * $Id: ev_seq.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/ev_scntn.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl>
class RWTExport RW_VSeq : public RW_VCntnr<StdColl, RWColl>
{
public:
  typedef typename StdColl::value_type        value_type;
  typedef typename StdColl::size_type         size_type;
  typedef typename StdColl::iterator          my_iter_;
  typedef typename StdColl::const_iterator    const_my_iter_;
  typedef typename StdColl::reference         reference;
  typedef typename StdColl::const_reference   const_reference;
  typedef const value_type&                   const_val_ref;

  void          append(const_reference item)
    { this->std().insert(this->end(),item); }

  void          clear() { this->std().erase(this->begin(), this->end()); }

  bool  insert(const_reference key) // fail only thru exception
    { this->std().insert(this->end(),key); return true; }

  value_type    get() { return removeFirst(); }
  
  void          insertAt(size_type index, const_reference item)
    {
      // index equal to number of entries OK (inserts at end)
      if(index > 0) boundsCheck(index-1);
      my_iter_ i = this->begin();
      RW_SL_STD(advance)(i,index);
      this->std().insert(i,item);
    }
  
  void          prepend(const_reference item)
    { this->std().insert(this->begin(),item); }

  bool          remove(bool (*fn)(const_reference,void*),void*);
  bool          remove(const_reference key);

  size_type     removeAll(const_reference key);
  size_type     removeAll(bool (*fn)(const_reference, void*),void*);

  value_type    removeAt(size_type index);

  value_type    removeFirst();

  size_type     replaceAll(const value_type& oldVal, const value_type& newVal);
  size_type     replaceAll(bool (*fn)(const value_type&, void*), void* x,
                           const value_type& newVal);

// hide some things
private:
  RW_VSeq(const RW_VSeq<StdColl,RWColl>&);

protected:
  RW_VSeq() {} // prevent direct use
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ev_seq.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ev_seq.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEV_SEQ_H__ */
