#ifndef __RWEV_SCNTN_H__
#define __RWEV_SCNTN_H__

/***************************************************************************
 *
 * ev_scntn.h - RW_VCntnr<StdColl,RWColl>
 *
 *     : value-based interface components for STL Containers
 *
 * $Id: ev_scntn.h 18623 2000-03-03 08:28:13Z sih $
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



#include "rw/edefs.h"
#include "rw/ev_cntnr.h"

class RWExport RWFile;
class RWExport RWvistream;
class RWExport RWvostream;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class StdColl, class RWColl>
class RWTExport RW_VCntnr : public RW_VBase<StdColl, RWColl>
{
public:
  typedef typename StdColl::value_type               value_type;
  typedef typename StdColl::size_type                size_type;
  typedef typename StdColl::const_reference          const_reference;
  typedef typename StdColl::reference                reference;
  typedef typename StdColl::iterator                 my_iter_;
  typedef typename StdColl::const_iterator           const_my_iter_;


  bool          find(
            const_reference,
            value_type& val
                     ) const;

  bool          find(
            bool(*fn)(const_reference,void*),
            void* x, value_type& val
                      ) const;

  const_reference minElement() const{
    RWPRECONDITION(!this->isEmpty()); 
    return *RW_SL_STD(min_element)(this->begin(),this->end()); 
  }

  const_reference maxElement() const  { 
    RWPRECONDITION(!this->isEmpty()); 
    return *RW_SL_STD(max_element)(this->begin(),this->end()); 
  }

  reference       minElement()
  {
    RWPRECONDITION(!this->isEmpty());
#   ifdef _MSC_VER
      return (reference) *RW_SL_STD(min_element)(this->begin(),this->end());
#   else
      return (reference) *(my_iter_)RW_SL_STD(min_element)(this->begin(),this->end());
#   endif  
  }

  reference       maxElement()
  {
    RWPRECONDITION(!this->isEmpty());
#   ifdef _MSC_VER
      return (reference) *RW_SL_STD(max_element)(this->begin(),this->end());
#   else
      return (reference) *(my_iter_)RW_SL_STD(max_element)(this->begin(),this->end());
#   endif
  }

protected:
  RW_VCntnr() { } // prevent direct usage
  RW_VCntnr(const RW_VCntnr<StdColl,RWColl>&) { }
  
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("ev_scntn.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/ev_scntn.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWEV_SCNTN_H__ */
